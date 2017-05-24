#pragma once


// CCon_NMEA_TalkerID 

class CCon_NMEA_TalkerID : public CDialog
{
	DECLARE_DYNAMIC(CCon_NMEA_TalkerID)

public:
	CCon_NMEA_TalkerID(CWnd* pParent = NULL); 
	virtual ~CCon_NMEA_TalkerID();

	enum { IDD = IDD_CON_NMEA_TALKER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 

	DECLARE_MESSAGE_MAP()
public:
	int talkerid;
	int attr;
};
