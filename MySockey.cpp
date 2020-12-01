#include "StdAfx.h"
#include "MySockey.h"

#define new DEBUG_NEW

#define _afxSockThreadState AfxGetModuleThreadState()
#define _AFX_SOCK_THREAD_STATE AFX_MODULE_THREAD_STATE

#pragma comment(lib, "wsock32.lib")

/////////////////////////////////////////////////////////////////////////////
// CMySocket Construction

CMySocket::CMySocket()
{
	m_pbBlocking = NULL;
	m_nConnectError = -1;
	m_nTimeOut = 2000;
}

/////////////////////////////////////////////////////////////////////////////
// CMySocket Operations

void CMySocket::CancelBlockingCall()
{
	if (m_pbBlocking != NULL)
	{
		*m_pbBlocking = FALSE;
		m_pbBlocking = NULL;
	}
}

/////////////////////////////////////////////////////////////////////////////
// CMySocket Overridable callbacks

BOOL CMySocket::OnMessagePending()
{
	MSG msg;
	if (::PeekMessage(&msg, NULL, WM_PAINT, WM_PAINT, PM_REMOVE))
	{
		::DispatchMessage(&msg);
		return FALSE;   // usually return TRUE, but OnIdle usually causes WM_PAINTs
	}
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CMySocket Implementation

CMySocket::~CMySocket()
{
	AFX_BEGIN_DESTRUCTOR

		if (m_hSocket != INVALID_SOCKET)
			Close();

	AFX_END_DESTRUCTOR
}

BOOL CMySocket::Accept(CAsyncSocket& rConnectedSocket, SOCKADDR* lpSockAddr, int* lpSockAddrLen)
{
	if (m_pbBlocking != NULL)
	{
		WSASetLastError(WSAEINPROGRESS);
		return FALSE;
	}
	while (!CAsyncSocket::Accept(rConnectedSocket, lpSockAddr, lpSockAddrLen))
	{
		if (GetLastError() == WSAEWOULDBLOCK)
		{
			if (!PumpMessages(FD_ACCEPT))
				return FALSE;
		}
		else
			return FALSE;
	}
	return TRUE;
}

void CMySocket::Close()
{
	if (m_hSocket != INVALID_SOCKET)
	{
		CancelBlockingCall();

		VERIFY(AsyncSelect(0));
		CAsyncSocket::Close();
		m_hSocket = INVALID_SOCKET;
	}
}

int CMySocket::Receive(void* lpBuf, int nBufLen, int nFlags)
{
	if (m_pbBlocking != NULL)
	{
		WSASetLastError(WSAEINPROGRESS);
		return  FALSE;
	}
	int nResult;
	while ((nResult = CAsyncSocket::Receive(lpBuf, nBufLen, nFlags)) == SOCKET_ERROR)
	{
		if (GetLastError() == WSAEWOULDBLOCK)
		{
			//if (!PumpMessages(FD_READ))
			//	return SOCKET_ERROR;
      return 0;
		}
		else
			return SOCKET_ERROR;
	}
	return nResult;
}

int CMySocket::Send(const void* lpBuf, int nBufLen, int nFlags)
{
	if (m_pbBlocking != NULL)
	{
		WSASetLastError(WSAEINPROGRESS);
		return  FALSE;
	}

	int nLeft, nWritten;
	PBYTE pBuf = (PBYTE)lpBuf;
	nLeft = nBufLen;

	while (nLeft > 0)
	{
		nWritten = SendChunk(pBuf, nLeft, nFlags);
		if (nWritten == SOCKET_ERROR)
			return nWritten;

		nLeft -= nWritten;
		pBuf += nWritten;
	}
	return nBufLen - nLeft;
}

int CMySocket::SendChunk(const void* lpBuf, int nBufLen, int nFlags)
{
	int nResult;
	while ((nResult = CAsyncSocket::Send(lpBuf, nBufLen, nFlags)) == SOCKET_ERROR)
	{
		if (GetLastError() == WSAEWOULDBLOCK)
		{
			if (!PumpMessages(FD_WRITE))
				return SOCKET_ERROR;
		}
		else
			return SOCKET_ERROR;
	}
	return nResult;
}

BOOL CMySocket::ConnectHelper(const SOCKADDR* lpSockAddr, int nSockAddrLen)
{
	if (m_pbBlocking != NULL)
	{
		WSASetLastError(WSAEINPROGRESS);
		return  FALSE;
	}

	m_nConnectError = -1;

	if (!CAsyncSocket::ConnectHelper(lpSockAddr, nSockAddrLen))
	{
		if (GetLastError() == WSAEWOULDBLOCK)
		{
			while (PumpMessages(FD_CONNECT))
			{
				if (m_nConnectError != -1)
				{
					WSASetLastError(m_nConnectError);
					return (m_nConnectError == 0);
				}
			}
		}
		return FALSE;
	}
	return TRUE;
}

int CMySocket::ReceiveFromHelper(void* lpBuf, int nBufLen, SOCKADDR* lpSockAddr, int* lpSockAddrLen, int nFlags)
{
	if (m_pbBlocking != NULL)
	{
		WSASetLastError(WSAEINPROGRESS);
		return  FALSE;
	}
	int nResult;
	while ((nResult = CAsyncSocket::ReceiveFromHelper(lpBuf, nBufLen, lpSockAddr, lpSockAddrLen, nFlags)) == SOCKET_ERROR)
	{
		if (GetLastError() == WSAEWOULDBLOCK)
		{
			if (!PumpMessages(FD_READ))
				return SOCKET_ERROR;
		}
		else
			return SOCKET_ERROR;
	}
	return nResult;
}

int CMySocket::SendToHelper(const void* lpBuf, int nBufLen, const SOCKADDR* lpSockAddr, int nSockAddrLen, int nFlags)
{
	if (m_pbBlocking != NULL)
	{
		WSASetLastError(WSAEINPROGRESS);
		return SOCKET_ERROR;
	}
	int nResult;
	while ((nResult = CAsyncSocket::SendToHelper(lpBuf, nBufLen, lpSockAddr, nSockAddrLen, nFlags)) == SOCKET_ERROR)
	{
		if (GetLastError() == WSAEWOULDBLOCK)
		{
			if (!PumpMessages(FD_WRITE))
				return SOCKET_ERROR;
		}
		else
			return SOCKET_ERROR;
	}
	return nResult;
}

int PASCAL CMySocket::ProcessAuxQueue()
{
	_AFX_SOCK_THREAD_STATE* pState = _afxSockThreadState;

	if (pState->m_plistSocketNotifications->IsEmpty())
		return 0;

	int nCount = 0;
	while(!pState->m_plistSocketNotifications->IsEmpty())
	{
		nCount++;

		MSG* pMsg = (MSG*)pState->m_plistSocketNotifications->RemoveHead();
		ASSERT(pMsg != NULL);
		if (pMsg->message == WM_SOCKET_NOTIFY)
		{
			CAsyncSocket::DoCallBack(pMsg->wParam, pMsg->lParam);
		}
		else
		{
			ASSERT(CAsyncSocket::LookupHandle((SOCKET)pMsg->wParam, TRUE) != NULL);
			CAsyncSocket::DetachHandle((SOCKET)pMsg->wParam, TRUE);
		}
		delete pMsg;
	}
	return nCount;
}

void PASCAL CMySocket::AuxQueueAdd(UINT message, WPARAM wParam, LPARAM lParam)
{
	_AFX_SOCK_THREAD_STATE* pState = _afxSockThreadState;

	MSG* pMsg = new MSG;
	pMsg->message = message;
	pMsg->wParam = wParam;
	pMsg->lParam = lParam;
	pState->m_plistSocketNotifications->AddTail(pMsg);
}

BOOL CMySocket::PumpMessages(UINT uStopFlag)
{
	// The same socket better not be blocking in more than one place.
	ASSERT(m_pbBlocking == NULL);

	_AFX_SOCK_THREAD_STATE* pState = _afxSockThreadState;

	ASSERT(pState->m_hSocketWindow != NULL);

	BOOL bBlocking = TRUE;
	m_pbBlocking = &bBlocking;
	CWinThread* pThread = AfxGetThread();

	// This is not a timeout in the WinSock sense, but more
	// like a WM_KICKIDLE to keep message pumping alive
	UINT_PTR nTimerID = ::SetTimer(pState->m_hSocketWindow, 1, m_nTimeOut, NULL);

	if (nTimerID == 0)
		AfxThrowResourceException();

	BOOL bPeek = TRUE;

	while (bBlocking)
	{
		TRY
		{
			MSG msg;
			if (::PeekMessage(&msg, pState->m_hSocketWindow,
				WM_SOCKET_NOTIFY, WM_SOCKET_DEAD, PM_REMOVE))
			{
				if (msg.message == WM_SOCKET_NOTIFY && (SOCKET)msg.wParam == m_hSocket)
				{
					if (WSAGETSELECTEVENT(msg.lParam) == FD_CLOSE)
					{
						break;
					}
					if (WSAGETSELECTEVENT(msg.lParam) == uStopFlag)
					{
						if (uStopFlag == FD_CONNECT)
							m_nConnectError = WSAGETSELECTERROR(msg.lParam);
						break;
					}
				}
				if (msg.wParam != 0 || msg.lParam != 0)
					CMySocket::AuxQueueAdd(msg.message, msg.wParam, msg.lParam);

				bPeek = TRUE;
			}
			else if (::PeekMessage(&msg, pState->m_hSocketWindow,
						WM_TIMER, WM_TIMER, PM_REMOVE))
			{
			break;
			}

			if (bPeek && ::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
			{
				if (OnMessagePending())
				{
					// allow user-interface updates
					ASSERT(pThread);
					pThread->OnIdle(-1);
				}
				else
				{
					bPeek = FALSE;
				}
			}
			else
			{
				// no work to do -- allow CPU to sleep
				WaitMessage();
				bPeek = TRUE;
			}
		}
		CATCH_ALL(e)
		{
			TRACE(traceSocket, 0, "Error: caught exception in PumpMessage - continuing.\n");
			//DELETE_EXCEPTION(e);
			bPeek = TRUE;
		}
		END_CATCH_ALL
	}

	::KillTimer(pState->m_hSocketWindow, nTimerID);

	if (!bBlocking)
	{
		WSASetLastError(WSAEINTR);
		return FALSE;
	}
	m_pbBlocking = NULL;

	::PostMessage(pState->m_hSocketWindow, WM_SOCKET_NOTIFY, 0, 0);

	return TRUE;
}

#ifdef _DEBUG
void CMySocket::AssertValid() const
{
	CAsyncSocket::AssertValid();
}

void CMySocket::Dump(CDumpContext& dc) const
{
	CAsyncSocket::Dump(dc);
	dc << "m_pbBlocking = " << m_pbBlocking <<"\n";
	dc << "m_nConnectError = " << m_nConnectError <<"\n";
}
#endif //_DEBUG

IMPLEMENT_DYNAMIC(CMySocket, CAsyncSocket)
#pragma warning(disable: 4074)

