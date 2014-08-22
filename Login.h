#pragma once


// CLogin 對話方塊

class CLogin : public CDialog
{
	DECLARE_DYNAMIC(CLogin)

public:
	CLogin(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CLogin();

// 對話方塊資料
	enum { IDD = IDD_LOGIN };

	CString password;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
};
