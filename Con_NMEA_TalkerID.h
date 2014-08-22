#pragma once


// CCon_NMEA_TalkerID 對話方塊

class CCon_NMEA_TalkerID : public CDialog
{
	DECLARE_DYNAMIC(CCon_NMEA_TalkerID)

public:
	CCon_NMEA_TalkerID(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CCon_NMEA_TalkerID();

// 對話方塊資料
	enum { IDD = IDD_CON_NMEA_TALKER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	int talkerid;
	int attr;
};
