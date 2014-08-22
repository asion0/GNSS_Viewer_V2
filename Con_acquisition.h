#pragma once


// CCon_acquisition 對話方塊

class CCon_acquisition : public CDialog
{
	DECLARE_DYNAMIC(CCon_acquisition)

public:
	CCon_acquisition(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CCon_acquisition();

// 對話方塊資料
	enum { IDD = IDD_CFG_ACQUISITION_MODE };

	int mode;
	int auto_power_off;
	int attr;

	CComboBox cbo_auto_power_off,cbo_auto_power_off_ascii;
	CButton rdo_binary,rdo_ascii;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
