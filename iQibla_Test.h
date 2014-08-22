#pragma once


// CiQibla_Test 對話方塊

class CiQibla_Test : public CDialog
{
	DECLARE_DYNAMIC(CiQibla_Test)

public:
	CiQibla_Test(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CiQibla_Test();

	double lat,lon,timezone;
	int using_gps;

// 對話方塊資料
	enum { IDD = IDD_DIALOG6 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedChkUsingGps();
};
