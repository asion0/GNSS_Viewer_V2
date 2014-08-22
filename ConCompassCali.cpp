// ConCompassCali.cpp : 實作檔
//

#include "stdafx.h"
#include "GPS.h"
#include "ConCompassCali.h"
#include "GPSDlg.h"


// CConCompassCali 對話方塊

IMPLEMENT_DYNAMIC(CConCompassCali, CDialog)
CConCompassCali::CConCompassCali(CWnd* pParent /*=NULL*/)
	: CDialog(CConCompassCali::IDD, pParent)
{
}

CConCompassCali::~CConCompassCali()
{
}

void CConCompassCali::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX,IDC_COMPASS_OFFSETX,m_offset_x);
	DDX_Control(pDX,IDC_COMPASS_OFFSETY,m_offset_y);
	DDX_Control(pDX,IDC_COMPASS_OFFSETZ,m_offset_z);
	
	DDX_Control(pDX,IDC_COMPASS_SENSX,m_sens_x);
	DDX_Control(pDX,IDC_COMPASS_SENSY,m_sens_y);
	DDX_Control(pDX,IDC_COMPASS_SENSZ,m_sens_z);
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CConCompassCali, CDialog)
	ON_BN_CLICKED(IDC_GET, OnBnClickedCompassGet)
	ON_BN_CLICKED(IDC_SET, OnBnClickedCompassSet)
END_MESSAGE_MAP()


// CConCompassCali 訊息處理常式

void CConCompassCali::OnBnClickedCompassGet()
{
	// TODO: 在此加入控制項告知處理常式程式碼
}

void CConCompassCali::OnBnClickedCompassSet()
{
	// TODO: 在此加入控制項告知處理常式程式碼
}
