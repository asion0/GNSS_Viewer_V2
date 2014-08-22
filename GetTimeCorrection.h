#pragma once


// CGetTimeCorrection 對話方塊

class CGetTimeCorrection : public CDialog
{
	DECLARE_DYNAMIC(CGetTimeCorrection)

public:
	CGetTimeCorrection(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CGetTimeCorrection();

// 對話方塊資料
	enum { IDD = IDD_GETTIMECORR };

	CString _filename;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBrowse();
};
