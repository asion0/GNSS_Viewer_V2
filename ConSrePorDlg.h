#pragma once
#include "afxwin.h"


// CConSrePorDlg 對話方塊

class CConSrePorDlg : public CDialog
{
	DECLARE_DYNAMIC(CConSrePorDlg)

public:
	CConSrePorDlg(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CConSrePorDlg();

// 對話方塊資料
	enum { IDD = IDD_CON_SRE_POR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_comport;
	CComboBox m_baudrate;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	U08 messages[10];  
	CComboBox m_attribute;
};
