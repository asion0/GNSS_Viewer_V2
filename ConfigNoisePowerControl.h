#pragma once


// CConfigNoisePowerControl 對話方塊

class CConfigNoisePowerControl : public CDialog
{
	DECLARE_DYNAMIC(CConfigNoisePowerControl)

public:
	CConfigNoisePowerControl(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CConfigNoisePowerControl();

// 對話方塊資料
//	enum { IDD = IDD_CONFIG_NOISE_PWR_CTRL };
	BOOL m_nMode;
	int m_nAttribute;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
