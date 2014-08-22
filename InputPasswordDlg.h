#pragma once

// CInputPassword 對話方塊
class CInputPassword : public CDialog
{
	DECLARE_DYNAMIC(CInputPassword)

public:
	CInputPassword(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CInputPassword();

// 對話方塊資料
//	enum { IDD = IDD_PASSWD };
	CString GetPassword() { return password; };
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()

	CString password;

public:
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void OnOK();
};
