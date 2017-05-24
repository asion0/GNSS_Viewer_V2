// ClipboardListBox.cpp
//

#include "stdafx.h"
#include "ClipboardListBox.h"


// CClipboardListBox

IMPLEMENT_DYNAMIC(CClipboardListBox, CListBox)

CClipboardListBox::CClipboardListBox()
{

}

CClipboardListBox::~CClipboardListBox()
{
}


BEGIN_MESSAGE_MAP(CClipboardListBox, CListBox)
    ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
    ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
    ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CClipboardListBox 
void CClipboardListBox::DoCopy()
{
	CArray<int,int> sels;
	int n = CListBox::GetSelCount();
	if(n <= 0)
	{
		return; // nothing to copy
	}

	sels.SetSize(n);
	CListBox::GetSelItems(n, sels.GetData());

	CString s;
	//*****************************************************************************
	// This segment of code only works if the listbox is non-owner-draw,          *
	// or is owner-draw with LBS_HASSTRINGS                                       *
	// So first check to make sure this is true                                   *
	//*****************************************************************************
	ASSERT( (GetStyle() & (LBS_OWNERDRAWFIXED | LBS_OWNERDRAWVARIABLE)) == 0 || //*
		(GetStyle() & LBS_HASSTRINGS) == LBS_HASSTRINGS);						//*
																				//*
	// Extract the text                                                         //*
	for(int i = 0; i < n; i++)                                                  //*
	{ /* copy items */															//*
		CString t;                                                              //*
		CListBox::GetText(sels[i], t);                                          //*
		t += _T("\r\n");                                                        //*
		s += t;                                                                 //*
	} /* copy items */															//*
	//*****************************************************************************

	HGLOBAL g = ::GlobalAlloc(GMEM_MOVEABLE, (s.GetLength() + 1) * sizeof(TCHAR));
	if(g == NULL)
	{ /* alloc failed */
		ASSERT(FALSE);  // failed to allocate memory
		return;
	} /* alloc failed */

	LPTSTR p = (LPTSTR)::GlobalLock(g);
	if(p == NULL)
	{ /* lock failed */
		ASSERT(FALSE);
		return;
	} /* lock failed */

	strcpy_s(p, s.GetLength() + 1, (LPCTSTR)s);
	//::StringCchCopy(p, s.GetLength() + 1, (LPCTSTR)s);

	::GlobalUnlock(g);

	if(!OpenClipboard())
	{ /* clipboard open failed */
		ASSERT(FALSE);
		GlobalFree(g);
		return;
	} /* clipboard open failed */

	if(!EmptyClipboard())
	{ /* empty failed */
		ASSERT(FALSE);
		GlobalFree(g);
		return;
	} /* empty failed */

	if(::SetClipboardData(CF_TEXT, g) == NULL)
	{ /* SetClipboardData failed */
		ASSERT(FALSE); //
		::CloseClipboard();
		::GlobalFree(g);
		return;
	} /* SetClipboardData failed */
	::CloseClipboard();
}

void CClipboardListBox::SelectAll()
{		
	for(int i = GetCount()-1; i>=0; --i) 
	{
		SetSel(i);
	}	
}

void CClipboardListBox::OnRButtonUp(UINT nFlags, CPoint point)
{
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
		DoCopy();
	}
	else if(IDM_CLEAR==cmd)
	{
		ResetContent();
	}	
	else if(IDM_SELECTALL==cmd)
	{
		SelectAll();	
	}	
	return;
	//CListBox::OnRButtonUp(nFlags, point);
}

int CClipboardListBox::SetCurSel(int nSelect)
{
	if(nSelect == GetCount()-1)
	{
		SetSel(-1, FALSE);
		return SetSel(nSelect);
	}
	return CListBox::SetCurSel(nSelect);
}

BOOL CClipboardListBox::PreTranslateMessage(MSG* pMsg)
{
	if( WM_KEYDOWN==pMsg->message && 
		(GetKeyState(VK_CONTROL) & 0x8000) > 0 )
	{	
		if('A' == pMsg->wParam || 'a' == pMsg->wParam)
		{
			SelectAll();	
			return TRUE;
		}
		else if('C' == pMsg->wParam || 'c' == pMsg->wParam)
		{
			DoCopy();
			return TRUE;
		}
	}
	return CListBox::PreTranslateMessage(pMsg);
}

void CClipboardListBox::PreSubclassWindow() 
{
    // TODO: Add your specialized code here and/or call the base class
    

    CListBox::PreSubclassWindow();
    EnableToolTips(TRUE);
}

int CClipboardListBox::OnToolHitTest(CPoint point, TOOLINFO * pTI) const
{
    int row;
    RECT cellrect;   // cellrect        - to hold the bounding rect
    BOOL tmp = FALSE;
    row  = ItemFromPoint(point,tmp);  //we call the ItemFromPoint function to determine the row,
    //note that in NT this function may fail  use the ItemFromPointNT member function

    if ( row == -1 ) 
        return -1;

    //set up the TOOLINFO structure. GetItemRect(row,&cellrect);
    GetItemRect(row,&cellrect);
    pTI->rect = cellrect;
    pTI->hwnd = m_hWnd;
    pTI->uId = (UINT)((row));   //The ¡¥uId¡¦ is assigned a value according to the row value.
    pTI->lpszText = LPSTR_TEXTCALLBACK;
    return pTI->uId;
}

//Define OnToolTipText(). This is the handler for the TTN_NEEDTEXT notification from 
//support ansi and unicode 
BOOL CClipboardListBox::OnToolTipText( UINT id, NMHDR * pNMHDR, LRESULT * pResult )
{
	// need to handle both ANSI and UNICODE versions of the message
	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
	CString strTipText;
	UINT nID = pNMHDR->idFrom;

	
	GetText( nID ,strTipText);

#ifndef _UNICODE
	if (pNMHDR->code == TTN_NEEDTEXTA)
		lstrcpyn(pTTTA->szText, strTipText, 80);
	else
		_mbstowcsz(pTTTW->szText, strTipText, 80);
#else
	if (pNMHDR->code == TTN_NEEDTEXTA)
		_wcstombsz(pTTTA->szText, strTipText, 80);
	else
		lstrcpyn(pTTTW->szText, strTipText, 80);
#endif
	*pResult = 0;

	return TRUE;    
}

