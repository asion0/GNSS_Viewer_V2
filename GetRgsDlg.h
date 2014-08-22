#pragma once
#include "afxwin.h"


// CGetRgsDlg 對話方塊

class CGetRgsDlg : public CDialog
{
	DECLARE_DYNAMIC(CGetRgsDlg)

public:
	CGetRgsDlg(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CGetRgsDlg();

// 對話方塊資料
	enum { IDD = IDD_GETRGSDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_address;
	unsigned int address;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
