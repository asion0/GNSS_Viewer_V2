#pragma once


// CSetTimeCorrections 對話方塊

class CSetTimeCorrections : public CDialog
{
	DECLARE_DYNAMIC(CSetTimeCorrections)

public:
	CSetTimeCorrections(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CSetTimeCorrections();

// 對話方塊資料
	enum { IDD = IDD_SETTIMECORR };

	CString _filename;
	int _attr;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBrowse();
	virtual BOOL OnInitDialog();
};
