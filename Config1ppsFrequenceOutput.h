#pragma once


// CConfig1ppsFrequenceOutput

class CConfig1ppsFrequenceOutput : public CDialog
{
	DECLARE_DYNAMIC(CConfig1ppsFrequenceOutput)

public:
	CConfig1ppsFrequenceOutput(CWnd* pParent = NULL);
	virtual ~CConfig1ppsFrequenceOutput();

	virtual void OnFinalRelease();

//	enum { IDD = IDD_CONFIG_1PPS_FREQ_OUTPUT };
	long int freqOutput;
	U08 freqOutputAttr;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 

	DECLARE_MESSAGE_MAP()
	DECLARE_DISPATCH_MAP()
	DECLARE_INTERFACE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
