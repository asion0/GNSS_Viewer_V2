#pragma once

/////////////////////////////////////////////////////////////////////////////
// CMySocket

class CMySocket : public CAsyncSocket
{
	DECLARE_DYNAMIC(CMySocket);
private:
	CMySocket(const CMySocket& rSrc);         // no implementation
	void operator=(const CMySocket& rSrc);  // no implementation

// Construction
public:
	CMySocket();
	//BOOL Create(UINT nSocketPort = 0, int nSocketType=SOCK_STREAM,
	//	LPCTSTR lpszSocketAddress = NULL);

// Attributes
public:
	BOOL IsBlocking();
	static CMySocket* PASCAL FromHandle(SOCKET hSocket);
	BOOL Attach(SOCKET hSocket);

// Operations
public:
	void CancelBlockingCall();

// Overridable callbacks
protected:
	virtual BOOL OnMessagePending();

// Implementation
public:
	int m_nTimeOut;

	virtual ~CMySocket();

	static int PASCAL ProcessAuxQueue();

	virtual BOOL Accept(CAsyncSocket& rConnectedSocket,
		SOCKADDR* lpSockAddr = NULL, int* lpSockAddrLen = NULL);
	virtual void Close();
	virtual int Receive(void* lpBuf, int nBufLen, int nFlags = 0);
	virtual int Send(const void* lpBuf, int nBufLen, int nFlags = 0);

	int SendChunk(const void* lpBuf, int nBufLen, int nFlags);

protected:
	friend class CSocketWnd;

	BOOL* m_pbBlocking;
	int m_nConnectError;

	virtual BOOL ConnectHelper(const SOCKADDR* lpSockAddr, int nSockAddrLen);
	virtual int ReceiveFromHelper(void* lpBuf, int nBufLen,
		SOCKADDR* lpSockAddr, int* lpSockAddrLen, int nFlags);
	virtual int SendToHelper(const void* lpBuf, int nBufLen,
		const SOCKADDR* lpSockAddr, int nSockAddrLen, int nFlags);

	static void PASCAL AuxQueueAdd(UINT message, WPARAM wParam, LPARAM lParam);

	virtual BOOL PumpMessages(UINT uStopFlag);

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};