#pragma once
#include "afxwin.h"


// CCfgMsg 對話方塊

class CCfgMsg : public CDialog
{
	DECLARE_DYNAMIC(CCfgMsg)

public:
	CCfgMsg(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CCfgMsg();

// 對話方塊資料
	enum { IDD = IDD_CFGMSG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	int msg_id;
	CComboBox m_type;
	CComboBox m_attribute;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
