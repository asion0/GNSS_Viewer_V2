// MsgList.cpp
//

#include "stdafx.h"
#include "MsgList.h"
#include "GPSDlg.h"

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
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CMsgList
void CALLBACK CMsgList::AsyncMessageAdd(HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	((CMsgList*)CWnd::FromHandle(hwnd))->ProcessTextBuffer();
}

void CMsgList::OnTimer(UINT_PTR nIDEvent)
{
	// TODO
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
		SetTimer(SCAN_TIMER, 100, AsyncMessageAdd);
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
  if(!CGPSDlg::gpsDlg->NeedUpdate())
  {
    if(m_textBuffer.GetCount() > 0)
    {
	    m_textBufferCritSect.Lock();
	    m_textBuffer.RemoveAll();
	    m_textBufferCritSect.Unlock();
    }
    return;
  }
  SetRedraw(FALSE);
  if(m_maxLine < GetItemCount())
	{
		DeleteAllItems();
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

  SetRedraw(TRUE);
}

void CMsgList::OnRButtonUp(UINT nFlags, CPoint point)
{
	return;
	// TODO
	const UINT_PTR IDM_SELECTALL = 0x80fb;
	const UINT_PTR IDM_COPY = 0x80fc;
	const UINT_PTR IDM_CLEAR = 0x80fd;
	CMenu menu;
	menu.CreatePopupMenu();
	menu.InsertMenu(0, MF_BYPOSITION|MF_STRING, IDM_SELECTALL, "Select All");
	menu.InsertMenu(1, MF_BYPOSITION|MF_STRING, IDM_COPY, "Copy");
	menu.InsertMenu(2, MF_BYPOSITION|MF_STRING, IDM_CLEAR, "Clear");

	this->ClientToScreen(&point);
	UINT_PTR cmd = menu.TrackPopupMenu(TPM_LEFTALIGN|TPM_TOPALIGN|TPM_RETURNCMD, point.x, point.y, this);

	if(IDM_COPY==cmd)
	{
		//DoCopy();
	}
	else if(IDM_CLEAR==cmd)
	{
		this->ClearAllText();
		//ResetContent();
	}	
	else if(IDM_SELECTALL==cmd)
	{
		//SelectAll();	
	}	
	return;
	//CListBox::OnRButtonUp(nFlags, point);
}

