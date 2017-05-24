#pragma once


// CCon_register 

class CCon_register : public CDialog
{
	DECLARE_DYNAMIC(CCon_register)

public:
	CCon_register(CWnd* pParent = NULL); 
	virtual ~CCon_register();
	
	CString m_addr;
	CString m_data;

	CEdit m_txt_addr;
	CEdit m_txt_data;

	enum { IDD = IDD_CON_REGISTER };

  int  specialFunction;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnEnUpdateRegAddr();
};
