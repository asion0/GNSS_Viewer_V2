#pragma once
#include "afxwin.h"


// CLogFilterDlg 對話方塊

class CLogFilterDlg : public CDialog
{
	DECLARE_DYNAMIC(CLogFilterDlg)

public:
	CLogFilterDlg(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CLogFilterDlg();

// 對話方塊資料
	enum { IDD = IDD_LOGFILTERDLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

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
};
