#pragma once


// CSetupDialog 對話方塊
struct Setting;

class CSetupDialog : public CDialog
{
	DECLARE_DYNAMIC(CSetupDialog)

public:
	CSetupDialog(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CSetupDialog();

// 對話方塊資料
//	enum { IDD = IDD_SETUP };

	void SetSetting(Setting* p)
	{ setting = p; }
protected:
	Setting* setting;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedBrowse();
	virtual BOOL OnInitDialog();
};
