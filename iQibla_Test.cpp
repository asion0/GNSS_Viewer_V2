// iQibla_Test.cpp : 實作檔
//

#include "stdafx.h"
#include "GPS.h"
#include "iQibla_Test.h"


// CiQibla_Test 對話方塊

IMPLEMENT_DYNAMIC(CiQibla_Test, CDialog)

CiQibla_Test::CiQibla_Test(CWnd* pParent /*=NULL*/)
	: CDialog(CiQibla_Test::IDD, pParent)
{
	using_gps = 0;
	lat = lon = timezone = 0;
}

CiQibla_Test::~CiQibla_Test()
{
}

void CiQibla_Test::DoDataExchange(CDataExchange* pDX)
{
	DDX_Check(pDX,IDC_CHK_USING_GPS,using_gps);
	DDX_Text(pDX,IDC_IQIBLA_LAT,lat);
	DDX_Text(pDX,IDC_IQIBLA_LON,lon);
	DDX_Text(pDX,IDC_IQIBLA_TIMEZONE,timezone);

	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CiQibla_Test, CDialog)
	ON_BN_CLICKED(IDOK, &CiQibla_Test::OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHK_USING_GPS, &CiQibla_Test::OnBnClickedChkUsingGps)
END_MESSAGE_MAP()


// CiQibla_Test 訊息處理常式

void CiQibla_Test::OnBnClickedOk()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	OnOK();
}

void CiQibla_Test::OnBnClickedChkUsingGps()
{
	
}
