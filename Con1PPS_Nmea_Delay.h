
#pragma once


// CCon1PPS_Nmea_Delay 對話方塊

class CCon1PPS_Nmea_Delay : public CDialog
{
	DECLARE_DYNAMIC(CCon1PPS_Nmea_Delay)

public:
	CCon1PPS_Nmea_Delay(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CCon1PPS_Nmea_Delay();

// 對話方塊資料
	enum { IDD = IDD_CON_1PPS_NMEA_DELAY };

	int nmea_delay;
	int attr;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
};
