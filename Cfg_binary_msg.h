#pragma once


// CCfg_binary_msg 對話方塊

class CCfg_binary_msg : public CDialog
{
	DECLARE_DYNAMIC(CCfg_binary_msg)

public:
	CCfg_binary_msg(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CCfg_binary_msg();

// 對話方塊資料
	enum { IDD = IDD_CON_BINARY_MSG };

	CComboBox m_rate;
	CComboBox m_mt;
	CComboBox m_rm;
	CComboBox m_sc;
	CComboBox m_rs;
	CComboBox m_sub;
	CComboBox m_attr;

	CComboBox m_gsb;
	int m_gsb_sel;


	int m_mt_sel,m_rm_sel,m_sc_sel,m_rs_sel,m_sub_sel,m_attr_sel;
	int m_rate_sel;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();
};
