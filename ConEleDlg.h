#pragma once
#include "afxwin.h"
#include "afxcmn.h"


// CConEleDlg 對話方塊

class CConEleDlg : public CDialog
{
	DECLARE_DYNAMIC(CConEleDlg)

public:
	CConEleDlg(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CConEleDlg();

// 對話方塊資料
	enum { IDD = IDD_CFG_ELE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	CEdit m_eele;
	CSpinButtonCtrl m_spele;
	CSliderCtrl m_slele;
	virtual BOOL OnInitDialog();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedOk();
};
