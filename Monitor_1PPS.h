#pragma once


// CMonitor_1PPS 對話方塊

class CMonitor_1PPS : public CDialog
{
	DECLARE_DYNAMIC(CMonitor_1PPS)

public:
	CMonitor_1PPS(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CMonitor_1PPS();

// 對話方塊資料
//	enum { IDD = IDD_1PPS_MONITOR };

	CEdit m_mode,m_length,m_error;
	CStatic m_lbl_set_std,m_lbl_now_std;
	CEdit m_set_std,m_now_std;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	void Show1PPSTiming(int mode,int surverylen,float quan_err);
	void Show1PPSTiming(int mode,int surveylen,float quan_err,int set_std,int now_std);
protected:
	virtual void OnCancel();
public:
	afx_msg void OnBnClickedCancel();
};
