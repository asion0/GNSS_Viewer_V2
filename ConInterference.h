#pragma once


// CConInterference 對話方塊

class CConInterference : public CDialog
{
	DECLARE_DYNAMIC(CConInterference)

public:
	CConInterference(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CConInterference();

// 對話方塊資料
	enum { IDD = IDD_CON_INTERFERENCE };


	int mode;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
