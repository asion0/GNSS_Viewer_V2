#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CConDauDlg 對話方塊

class CConDauDlg : public CDialog
{
	DECLARE_DYNAMIC(CConDauDlg)

public:
	CConDauDlg(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CConDauDlg();

// 對話方塊資料
	enum { IDD = IDD_CON_DATUM };

	CButton m_test;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	CComboBox m_ellipsoidlist;
	virtual BOOL OnInitDialog();	
	afx_msg void OnBnClickedOk();
	CComboBox m_attribute;
	afx_msg void OnBnClickedDatumTest();
};
