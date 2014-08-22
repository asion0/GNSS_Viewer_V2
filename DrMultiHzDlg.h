#pragma once


// DrMultiHzDlg 對話方塊

class DrMultiHzDlg : public CDialog
{
	DECLARE_DYNAMIC(DrMultiHzDlg)

public:
	DrMultiHzDlg(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~DrMultiHzDlg();

// 對話方塊資料
//	enum { IDD = IDD_CONFIG_DRUPDATERATE };
	
	int rate;
	int attr;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援
	
	CComboBox cbo_rate;
	CComboBox cbo_attr;
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
};
