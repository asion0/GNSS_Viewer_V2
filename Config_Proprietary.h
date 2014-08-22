#pragma once


// CConfig_Proprietary 對話方塊

class CConfig_Proprietary : public CDialog
{
	DECLARE_DYNAMIC(CConfig_Proprietary)

public:
	CConfig_Proprietary(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CConfig_Proprietary();

// 對話方塊資料
	enum { IDD = IDD_CON_PROPRIETARY };

	int enable;
	int attr;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
};
