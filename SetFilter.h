#pragma once


// CSetFilter 對話方塊

class CSetFilter : public CDialog
{
	DECLARE_DYNAMIC(CSetFilter)

public:
	CSetFilter(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CSetFilter();

// 對話方塊資料
	enum { IDD = IDD_DIALOG6 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
};
