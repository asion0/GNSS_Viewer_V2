#pragma once
#include "afxwin.h"


// CWaitAckDlg 對話方塊

class CWaitAckDlg : public CDialog
{
	DECLARE_DYNAMIC(CWaitAckDlg)

public:
	CWaitAckDlg(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CWaitAckDlg();

// 對話方塊資料
	enum { IDD = IDD_WAITACK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:	
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL OnInitDialog();
	bool IsFinish;
	CEdit m_msg;
	
};
