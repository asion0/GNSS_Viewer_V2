#pragma once


// CConPowerMode 對話方塊

class CConPowerMode : public CDialog
{
	DECLARE_DYNAMIC(CConPowerMode)

public:
	CConPowerMode(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CConPowerMode();

	int mode;
	int attribute;
// 對話方塊資料
	enum { IDD = IDD_CON_POWERMODE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
