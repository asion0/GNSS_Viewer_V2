// Con1PPS_Nmea_Delay.cpp : 實作檔
//

#include "stdafx.h"
#include "GPS.h"
#include "Con1PPS_Nmea_Delay.h"


// CCon1PPS_Nmea_Delay 對話方塊

IMPLEMENT_DYNAMIC(CCon1PPS_Nmea_Delay, CDialog)

CCon1PPS_Nmea_Delay::CCon1PPS_Nmea_Delay(CWnd* pParent /*=NULL*/)
	: CDialog(CCon1PPS_Nmea_Delay::IDD, pParent)
{
	nmea_delay = attr = 0;
}	

CCon1PPS_Nmea_Delay::~CCon1PPS_Nmea_Delay()
{
}

void CCon1PPS_Nmea_Delay::DoDataExchange(CDataExchange* pDX)
{
	DDX_Text(pDX,IDC_1PPS_NMEA_DELAY,nmea_delay);
	DDX_CBIndex(pDX,IDC_1PPS_ATTR,attr);
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCon1PPS_Nmea_Delay, CDialog)
END_MESSAGE_MAP()


// CCon1PPS_Nmea_Delay 訊息處理常式
