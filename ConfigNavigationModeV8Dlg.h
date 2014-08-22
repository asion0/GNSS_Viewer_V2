#pragma once


// ConfigNavigationModeV8Dlg 對話方塊

class ConfigNavigationModeV8Dlg : public CDialog
{
	DECLARE_DYNAMIC(ConfigNavigationModeV8Dlg)

public:
	ConfigNavigationModeV8Dlg(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~ConfigNavigationModeV8Dlg();

// 對話方塊資料
//	enum { IDD = IDD_CONFIG_NAV_MODE_V8 };
	int m_nMode;
	int m_nAttribute;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
