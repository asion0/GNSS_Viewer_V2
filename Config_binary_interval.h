#pragma once


// CConfig_binary_interval 對話方塊

class CConfig_binary_interval : public CDialog
{
	DECLARE_DYNAMIC(CConfig_binary_interval)

public:
	CConfig_binary_interval(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CConfig_binary_interval();

// 對話方塊資料
	enum { IDD = IDD_CONFIG_BINARY_INTERVAL };

	CComboBox m_interval,m_attr;
	int m_bin_interval,m_bin_attr;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
