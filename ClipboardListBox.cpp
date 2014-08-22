// ClipboardListBox.cpp : 實作檔
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
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CClipboardListBox 訊息處理常式
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
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
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
