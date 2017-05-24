#pragma once
#include "afxcmn.h"
#include "afxwin.h"


// CScanDlg 

class CScanDlg : public CDialog
{
	DECLARE_DYNAMIC(CScanDlg)

public:
	CScanDlg(CWnd* pParent = NULL);  
	virtual ~CScanDlg();

	enum { IDD = IDD_SCANGPS_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 

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
