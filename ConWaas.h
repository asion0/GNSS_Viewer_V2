#pragma once


// CConWaas 對話方塊

class CConWaas : public CDialog
{
	DECLARE_DYNAMIC(CConWaas)

public:
	CConWaas(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CConWaas();
	int waas ;
	int attribute ;
// 對話方塊資料
	enum { IDD = IDD_CON_WAAS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};
