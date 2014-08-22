#pragma once


// CConfig_Powersaving_param 對話方塊

class CConfig_Powersaving_param : public CDialog
{
	DECLARE_DYNAMIC(CConfig_Powersaving_param)

public:
	CConfig_Powersaving_param(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CConfig_Powersaving_param();

	int param[9];
	int attr;

// 對話方塊資料
	enum { IDD = IDD_POWERSAVING_PARAM };

	void SetRomMode(bool b) { romMode = b; }
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援
	bool romMode;

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
