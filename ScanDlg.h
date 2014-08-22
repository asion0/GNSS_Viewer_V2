#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CScanDlg 對話方塊

class CScanDlg : public CDialog
{
	DECLARE_DYNAMIC(CScanDlg)

public:
	CScanDlg(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CScanDlg();

// 對話方塊資料
	enum { IDD = IDD_SCANGPS_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	CProgressCtrl m_progress;
	CEdit m_msg;
	bool IsFinish;
	virtual BOOL OnInitDialog();
	afx_msg void OnTimer(UINT nIDEvent);
	U08 Pos;
	afx_msg void OnBnClickedButton1();
};
