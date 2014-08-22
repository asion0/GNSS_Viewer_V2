// MsgList.cpp : 實作檔
//

#include "stdafx.h"
#include "MsgList.h"


// CMsgList
#define SCAN_TIMER		1
IMPLEMENT_DYNAMIC(CMsgList, CListCtrl)

CMsgList::CMsgList()
{
	m_scanTimer = false;
	m_maxLine = 5000;
}

CMsgList::~CMsgList()
{
}


BEGIN_MESSAGE_MAP(CMsgList, CListCtrl)
	ON_WM_TIMER()
	ON_WM_DESTROY()
	ON_WM_SIZE()
END_MESSAGE_MAP()

// CMsgList 訊息處理常式
void CALLBACK CMsgList::AsyncMessageAdd(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	((CMsgList*)CWnd::FromHandle(hwnd))->ProcessTextBuffer();
}

void CMsgList::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	//Utility::Log(__FUNCTION__, "OnTimer", nIDEvent);

	CListCtrl::OnTimer(nIDEvent);
}

void CMsgList::OnDestroy()
{
	CListCtrl::OnDestroy();
	if(m_scanTimer)
	{
		KillTimer(SCAN_TIMER);
		m_scanTimer = false;
	}
}

void CMsgList::OnSize(UINT nType, int cx, int cy)
{
	CListCtrl::OnSize(nType, cx, cy);
	if(!m_scanTimer)
	{
		SetTimer(SCAN_TIMER, 20, AsyncMessageAdd);
		m_scanTimer = true;
	}
}

int CMsgList::AddTextAsync(LPCSTR text)
{
	m_textBufferCritSect.Lock();
	m_textBuffer.Add(text);
	INT_PTR bufferCount = m_textBuffer.GetCount();
	m_textBufferCritSect.Unlock();
	return bufferCount;
}

void CMsgList::ClearAllText()
{
	m_textBufferCritSect.Lock();
	m_textBuffer.RemoveAll();
	m_textBufferCritSect.Unlock();
	return;
}

void CMsgList::ProcessTextBuffer()
{
	if(0 == m_textBuffer.GetCount())
	{
		return;
	}
	//const int MaxCount = 300;
	if(m_maxLine < GetItemCount())
	{
		DeleteAllItems();
		//for(int i=m_maxLine/2; i>0; --i)
		//{
		//	DeleteItem(0);
		//}
	}

	m_textBufferCritSect.Lock();
	for(int i=0; i<m_textBuffer.GetCount(); ++i)
	{
		InsertItem(GetItemCount(), m_textBuffer.GetAt(i));
	}
	m_textBuffer.RemoveAll();
	m_textBufferCritSect.Unlock();

	RECT r;
	GetItemRect(0, &r, LVIR_BOUNDS);
	Scroll(CSize(0, (r.bottom - r.top) * GetItemCount()));	
}
