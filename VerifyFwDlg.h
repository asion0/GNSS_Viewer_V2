#pragma once


// CVerifyFwDlg 對話方塊

class CVerifyFwDlg : public CDialog
{
	DECLARE_DYNAMIC(CVerifyFwDlg)

public:
	CVerifyFwDlg(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CVerifyFwDlg();

// 對話方塊資料
//	enum { IDD = IDD_VERIFY_FW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援
	bool AutoFill(const CString& s);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSelect();
	afx_msg void OnBnClickedGo();
};
