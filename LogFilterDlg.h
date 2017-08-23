#pragma once
#include "afxwin.h"


// CLogFilterDlg 

class CLogFilterDlg : public CDialog
{
	DECLARE_DYNAMIC(CLogFilterDlg)

public:
	CLogFilterDlg(CWnd* pParent = NULL);   
	virtual ~CLogFilterDlg();

	enum { IDD = IDD_LOG_FILTER_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 

	DECLARE_MESSAGE_MAP()
public:

	CEdit m_maxt;
	CEdit m_mint;
	CEdit m_maxd;
	CEdit m_mind;
	CEdit m_maxv;
	CEdit m_minv;
	CComboBox m_logenable;
	CComboBox m_fifomode;

	CEdit m_mic;
	CStatic m_lbl_mic;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
  afx_msg void OnBnClickedEnable();
};
