#pragma once
#include "afxwin.h"


// CGetRgsDlg 

class CGetRgsDlg : public CDialog
{
	DECLARE_DYNAMIC(CGetRgsDlg)

public:
	CGetRgsDlg(CWnd* pParent = NULL);   
	virtual ~CGetRgsDlg();

	enum { IDD = IDD_GETRGSDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_address;
	unsigned int address;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
