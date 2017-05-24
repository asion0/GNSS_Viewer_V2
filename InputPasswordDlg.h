#pragma once

// CInputPassword
class CInputPassword : public CDialog
{
	DECLARE_DYNAMIC(CInputPassword)

public:
	CInputPassword(CWnd* pParent = NULL);
	virtual ~CInputPassword();


//	enum { IDD = IDD_PASSWD };
	CString GetPassword() { return password; };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV

	DECLARE_MESSAGE_MAP()

	CString password;

public:
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
};
