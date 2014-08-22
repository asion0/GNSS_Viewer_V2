#pragma once
#include "afxwin.h"


// CConNMEAComport 對話方塊

class CConNMEAComport : public CDialog
{
	DECLARE_DYNAMIC(CConNMEAComport)

public:
	CConNMEAComport(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CConNMEAComport();

// 對話方塊資料
	enum { IDD = IDD_CON_NMEA_COMPORT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	CButton m_com0;
	CButton m_com1;
	int comport;
	int attr;
	afx_msg void OnBnClickedOk();
};
