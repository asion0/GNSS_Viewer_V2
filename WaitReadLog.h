#pragma once

// CWaitReadLog 

class CWaitReadLog : public CDialog
{
	DECLARE_DYNAMIC(CWaitReadLog)

public:
	CWaitReadLog(CWnd* pParent = NULL);
	virtual ~CWaitReadLog();

//	enum { IDD = IDD_WAITLOGREAD };

//	CGPSDlg *pdlg;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 

	DECLARE_MESSAGE_MAP()
public:
	CEdit msg;
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL OnInitDialog();
	bool IsFinish;
	//afx_msg void OnBnClickedButton1();
	//afx_msg void OnClose();
};
