#pragma once

//#include "GPSDlg.h"

// CWaitReadLog 對話方塊

class CWaitReadLog : public CDialog
{
	DECLARE_DYNAMIC(CWaitReadLog)

public:
	CWaitReadLog(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CWaitReadLog();

// 對話方塊資料
//	enum { IDD = IDD_WAITLOGREAD };

//	CGPSDlg *pdlg;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	CEdit msg;
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL OnInitDialog();
	bool IsFinish;
	afx_msg void OnBnClickedButton1();
	afx_msg void OnClose();
};
