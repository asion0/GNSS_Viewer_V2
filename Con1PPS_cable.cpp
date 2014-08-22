// Con1PPS_cable.cpp : 實作檔
//

#include "stdafx.h"
#include "GPS.h"
#include "Con1PPS_cable.h"


// CCon1PPS_cable 對話方塊

IMPLEMENT_DYNAMIC(CCon1PPS_cable, CDialog)
CCon1PPS_cable::CCon1PPS_cable(CWnd* pParent /*=NULL*/)
	: CDialog(CCon1PPS_cable::IDD, pParent)
{
	attr = 0;
	cable_delay = 0;
}

CCon1PPS_cable::~CCon1PPS_cable()
{
}

void CCon1PPS_cable::DoDataExchange(CDataExchange* pDX)
{
	DDX_Text(pDX,IDC_1PPS_CABLE,cable_delay);
	DDX_CBIndex(pDX,IDC_1PPS_CABLE_ATTRI,attr);
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCon1PPS_cable, CDialog)
END_MESSAGE_MAP()


// CCon1PPS_cable 訊息處理常式
