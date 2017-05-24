#pragma once


// CVerifyFwDlg 

class CVerifyFwDlg : public CDialog
{
	DECLARE_DYNAMIC(CVerifyFwDlg)

public:
	CVerifyFwDlg(CWnd* pParent = NULL);  
	virtual ~CVerifyFwDlg();

//	enum { IDD = IDD_VERIFY_FW };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 
	bool AutoFill(const CString& s);

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedSelect();
	afx_msg void OnBnClickedGo();
};
