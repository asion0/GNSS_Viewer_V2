#pragma once


// CMsgList

class CMsgList : public CListCtrl
{
	DECLARE_DYNAMIC(CMsgList)

public:
	CMsgList();
	virtual ~CMsgList();

	int AddTextAsync(LPCSTR text);
	void ClearAllText();
	int SetMaxLine(int max) 
	{ 
		int oldMax = m_maxLine;
		m_maxLine = max;
		return oldMax;
	}
  //void SetRedraw(bool b) { this.SetRedraw(FALSE); }
	void ProcessTextBuffer();

protected:
	bool m_scanTimer;
	int m_maxLine;
	CStringArray m_textBuffer;
	CCriticalSection m_textBufferCritSect;
	static void CALLBACK AsyncMessageAdd(HWND hwnd, UINT uMsg, 
				UINT_PTR idEvent, DWORD dwTime);
	void DoCopy();


	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnDestroy();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);

	DECLARE_MESSAGE_MAP()
};


