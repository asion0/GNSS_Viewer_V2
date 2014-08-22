// Config_Powersaving_param.cpp : 實作檔
//

#include "stdafx.h"
#include "GPS.h"
#include "Config_Powersaving_param.h"


// CConfig_Powersaving_param 對話方塊

IMPLEMENT_DYNAMIC(CConfig_Powersaving_param, CDialog)

CConfig_Powersaving_param::CConfig_Powersaving_param(CWnd* pParent /*=NULL*/)
	: CDialog(CConfig_Powersaving_param::IDD, pParent)
{
	param[0] = 2;
	param[1] = 9;
	param[2] = 0;
	param[3] = 4;
	param[4] = 90;
	param[5] = 20;
	param[6] = 600;
	param[7] = 180;
	param[8] = 15;
	attr = 0;
	romMode = false;
}

CConfig_Powersaving_param::~CConfig_Powersaving_param()
{
}

void CConfig_Powersaving_param::DoDataExchange(CDataExchange* pDX)
{
	DDX_Text(pDX,IDC_POWERSAVING_1,param[0]);
	DDX_Text(pDX,IDC_POWERSAVING_2,param[1]);
	DDX_Text(pDX,IDC_POWERSAVING_3,param[2]);
	DDX_Text(pDX,IDC_POWERSAVING_4,param[3]);
	DDX_Text(pDX,IDC_POWERSAVING_5,param[4]);
	DDX_Text(pDX,IDC_POWERSAVING_6,param[5]);
	DDX_Text(pDX,IDC_POWERSAVING_7,param[6]);
	DDX_Text(pDX,IDC_POWERSAVING_8,param[7]);
	DDX_Text(pDX,IDC_POWERSAVING_9,param[8]);

	DDX_CBIndex(pDX,IDC_POWERSAVING_ATTR,attr);
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CConfig_Powersaving_param, CDialog)
END_MESSAGE_MAP()


// CConfig_Powersaving_param 訊息處理常式

BOOL CConfig_Powersaving_param::OnInitDialog()
{
	CDialog::OnInitDialog();
	if(romMode)
	{
		GetDlgItem(IDC_POWERSAVING_8)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_POWERSAVING_9)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_8)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_STATIC_9)->ShowWindow(SW_HIDE);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
}
