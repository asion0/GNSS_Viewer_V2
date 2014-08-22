#pragma once


// CCon1PPS 對話方塊

class CCon1PPS : public CDialog
{
	DECLARE_DYNAMIC(CCon1PPS)

public:
	CCon1PPS(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CCon1PPS();

// 對話方塊資料
	enum { IDD = IDD_CON_1PPS };
	
	CComboBox m_1pps;
	int _1ppsmode;
	int _1ppsattr;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
