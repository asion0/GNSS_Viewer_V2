#pragma once


// CCon_binary_msg_interval 對話方塊

class CCon_binary_msg_interval : public CDialog
{
	DECLARE_DYNAMIC(CCon_binary_msg_interval)

public:
	CCon_binary_msg_interval(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CCon_binary_msg_interval();

// 對話方塊資料
	enum { IDD = IDD_CON_USERDEFINEMSG_INTERVAL };

public:
	CSliderCtrl m_sliderctrl;
	CEdit m_val;
	CSpinButtonCtrl m_spin;
	CComboBox m_cbo;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	virtual BOOL OnInitDialog();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedOk();
};
