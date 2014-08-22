#pragma once


// CConMultiPath 對話方塊

class CConMultiPath : public CDialog
{
	DECLARE_DYNAMIC(CConMultiPath)

public:
	CConMultiPath(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CConMultiPath();

	int multipath;
	int attribute;
// 對話方塊資料
	enum { IDD = IDD_CON_MULTIPATH };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
};
