#pragma once


// CSetupDialog 
struct Setting;

class CSetupDialog : public CDialog
{
	DECLARE_DYNAMIC(CSetupDialog)

public:
	CSetupDialog(CWnd* pParent = NULL);   //
	virtual ~CSetupDialog();
//	enum { IDD = IDD_SETUP };

	void SetSetting(Setting* p)
	{ setting = p; }
protected:
	Setting* setting;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedBrowse();
	virtual BOOL OnInitDialog();
};

class CSetupDialog2 : public CDialog
{
	DECLARE_DYNAMIC(CSetupDialog2)

public:
	CSetupDialog2(CWnd* pParent = NULL);   //
	virtual ~CSetupDialog2();
//	enum { IDD = IDD_SETUP };

	void SetSetting(Setting* p)
	{ setting = p; }
protected:
	Setting* setting;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedBrowse();
	virtual BOOL OnInitDialog();
};