#pragma once


// CConfig1ppsPulseWidthDlg 對話方塊

class CConfig1ppsPulseWidthDlg : public CDialog
{
	DECLARE_DYNAMIC(CConfig1ppsPulseWidthDlg)

public:
	CConfig1ppsPulseWidthDlg(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CConfig1ppsPulseWidthDlg();

// 對話方塊資料
//	enum { IDD = IDD_CONFIG_1PPS_PULSE_WIDTH };
	UINT32 m_nPulseWidth;
	int m_nAttribute;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
