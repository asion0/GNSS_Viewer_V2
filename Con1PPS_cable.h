#pragma once


// CCon1PPS_cable 對話方塊

class CCon1PPS_cable : public CDialog
{
	DECLARE_DYNAMIC(CCon1PPS_cable)

public:
	CCon1PPS_cable(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CCon1PPS_cable();

// 對話方塊資料
	enum { IDD = IDD_1PPS_CABLE };

	int cable_delay;
	int attr;

	CEdit m_cable_delay;
	

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
};
