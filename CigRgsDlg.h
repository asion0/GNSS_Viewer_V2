#pragma once
#include "afxwin.h"


// CCigRgsDlg 對話方塊

class CCigRgsDlg : public CDialog
{
	DECLARE_DYNAMIC(CCigRgsDlg)

public:
	CCigRgsDlg(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CCigRgsDlg();

// 對話方塊資料
	enum { IDD = IDD_CIGRGSDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_ms;
	CEdit m_ns;
	CEdit m_plldiv;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	U08 MS;
	U08 NS;
	U08 PLLDIV;
};
