#pragma once
#include "afxwin.h"

	
// CConBinOutDlg 對話方塊

class CConBinOutDlg : public CDialog
{
	DECLARE_DYNAMIC(CConBinOutDlg)

public:
	CConBinOutDlg(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CConBinOutDlg();

// 對話方塊資料
	enum { IDD = IDD_CFG_BIN };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	CButton m_ecef;
	CButton m_pvt;
	CButton m_sat;
	CButton m_meas;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();	
};
