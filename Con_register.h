#pragma once


// CCon_register 對話方塊

class CCon_register : public CDialog
{
	DECLARE_DYNAMIC(CCon_register)

public:
	CCon_register(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CCon_register();
	
	CString m_addr;
	CString m_data;

	CEdit m_txt_addr;
	CEdit m_txt_data;
// 對話方塊資料
	enum { IDD = IDD_CON_REGISTER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnEnUpdateRegAddr();
};
