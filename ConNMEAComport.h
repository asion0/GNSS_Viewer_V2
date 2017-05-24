#pragma once
#include "afxwin.h"


// CConNMEAComport 

class CConNMEAComport : public CDialog
{
	DECLARE_DYNAMIC(CConNMEAComport)

public:
	CConNMEAComport(CWnd* pParent = NULL);
	virtual ~CConNMEAComport();

	enum { IDD = IDD_CON_NMEA_COMPORT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CButton m_com0;
	CButton m_com1;
	int comport;
	int attr;
	afx_msg void OnBnClickedOk();
};
