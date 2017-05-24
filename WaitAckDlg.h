#pragma once
#include "afxwin.h"


// CWaitAckDlg 

class CWaitAckDlg : public CDialog
{
	DECLARE_DYNAMIC(CWaitAckDlg)

public:
	CWaitAckDlg(CWnd* pParent = NULL);   
	virtual ~CWaitAckDlg();


	enum { IDD = IDD_WAITACK };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 

	DECLARE_MESSAGE_MAP()
public:	
	afx_msg void OnTimer(UINT nIDEvent);
	virtual BOOL OnInitDialog();
	bool IsFinish;
	CEdit m_msg;
	
};
