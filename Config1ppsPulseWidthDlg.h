#pragma once


// CConfig1ppsPulseWidthDlg 
class CConfig1ppsPulseWidthDlg : public CDialog
{
	DECLARE_DYNAMIC(CConfig1ppsPulseWidthDlg)

public:
	CConfig1ppsPulseWidthDlg(CWnd* pParent = NULL);   
	virtual ~CConfig1ppsPulseWidthDlg();


//	enum { IDD = IDD_CONFIG_1PPS_PULSE_WIDTH };
	UINT32 m_nPulseWidth;
	int m_nAttribute;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
