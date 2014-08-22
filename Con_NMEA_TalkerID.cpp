// Con_NMEA_TalkerID.cpp : 實作檔
//

#include "stdafx.h"
#include "GPS.h"
#include "Con_NMEA_TalkerID.h"


// CCon_NMEA_TalkerID 對話方塊

IMPLEMENT_DYNAMIC(CCon_NMEA_TalkerID, CDialog)

CCon_NMEA_TalkerID::CCon_NMEA_TalkerID(CWnd* pParent /*=NULL*/)
	: CDialog(CCon_NMEA_TalkerID::IDD, pParent)
	, talkerid(0)
	, attr(0)
{

}

CCon_NMEA_TalkerID::~CCon_NMEA_TalkerID()
{
}

void CCon_NMEA_TalkerID::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_CBIndex(pDX, IDC_NMEA_COM_ATTR2, talkerid);
	DDV_MinMaxInt(pDX, talkerid, 0, 1);
	DDX_CBIndex(pDX, IDC_NMEA_COM_ATTR, attr);
	DDV_MinMaxInt(pDX, attr, 0, 1);
}


BEGIN_MESSAGE_MAP(CCon_NMEA_TalkerID, CDialog)
END_MESSAGE_MAP()


// CCon_NMEA_TalkerID 訊息處理常式
