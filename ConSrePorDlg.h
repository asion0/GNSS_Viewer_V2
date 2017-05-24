#pragma once
#include "afxwin.h"


// CConSrePorDlg

class CConSrePorDlg : public CDialog
{
	DECLARE_DYNAMIC(CConSrePorDlg)

public:
	CConSrePorDlg(CWnd* pParent = NULL);
	virtual ~CConSrePorDlg();

	enum { IDD = IDD_CON_SRE_POR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_comport;
	CComboBox m_baudrate;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	U08 messages[10];  
	CComboBox m_attribute;
};
