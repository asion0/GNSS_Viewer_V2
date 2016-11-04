#pragma once


// HostLogDlg
class BinaryData;

class HostLogDlg : public CDialog
{
	DECLARE_DYNAMIC(HostLogDlg)

public:
	HostLogDlg(CWnd* pParent = NULL);   // 
	virtual ~HostLogDlg();
//	enum { IDD = IDD_HOSTLOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 

	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL OnInitDialog();
	//afx_msg void OnEnChangeInput();
	afx_msg void OnBnClickedBrowse();
	afx_msg void OnBnClickedStart();

protected:
	virtual void OnOK();
};
