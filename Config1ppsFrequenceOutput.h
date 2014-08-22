#pragma once


// CConfig1ppsFrequenceOutput 對話方塊

class CConfig1ppsFrequenceOutput : public CDialog
{
	DECLARE_DYNAMIC(CConfig1ppsFrequenceOutput)

public:
	CConfig1ppsFrequenceOutput(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CConfig1ppsFrequenceOutput();

	virtual void OnFinalRelease();

// 對話方塊資料
//	enum { IDD = IDD_CONFIG_1PPS_FREQ_OUTPUT };
	long int freqOutput;
	U08 freqOutputAttr;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
