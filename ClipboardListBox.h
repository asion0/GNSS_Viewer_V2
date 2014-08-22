#pragma once


// CClipboardListBox

class CClipboardListBox : public CListBox
{
	DECLARE_DYNAMIC(CClipboardListBox)

public:
	CClipboardListBox();
	virtual ~CClipboardListBox();

	BOOL DeleteAllItems()
	{
		while(DeleteString(0)){};
		return TRUE;
	}

	int GetItemCount()
	{
		return GetCount();
	}

	int InsertItem(int nItem, LPCTSTR lpszItem)
	{
		return InsertString(nItem, lpszItem);
	}

protected:
	void DoCopy();
	void SelectAll();
	DECLARE_MESSAGE_MAP()
public:
	int SetCurSel(int nSelect);

	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);

};


