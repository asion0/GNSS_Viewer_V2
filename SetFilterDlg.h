#pragma once
#include "afxwin.h"


// CSetFilterDlg 

class CSetFilterDlg : public CDialog
{
	DECLARE_DYNAMIC(CSetFilterDlg)

public:
	CSetFilterDlg(CWnd* pParent = NULL);   
	virtual ~CSetFilterDlg();

	enum { IDD = IDD_DLGSETFILTER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_tmin;
	CEdit m_tmax;
	CEdit m_dmin;
	CEdit m_dmax;
	CEdit m_vmin;
	CEdit m_vmax;
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
