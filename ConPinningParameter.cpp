// ConPinningParameter.cpp : 實作檔
//

#include "stdafx.h"
#include "GPS.h"
#include "ConPinningParameter.h"


// CConPinningParameter 對話方塊

IMPLEMENT_DYNAMIC(CConPinningParameter, CDialog)
CConPinningParameter::CConPinningParameter(CWnd* pParent /*=NULL*/)
	: CDialog(CConPinningParameter::IDD, pParent)
{
	pin_speed = 2;
	pin_cnt = 10;
	upin_speed = 8;
	upin_threshold = 45;
	upin_distance = 500;
	//pin_speed = pin_cnt = upin_speed = upin_threshold = upin_distance = 0;
	attr = 0;
}

CConPinningParameter::~CConPinningParameter()
{
}

void CConPinningParameter::DoDataExchange(CDataExchange* pDX)
{
	DDX_Text(pDX,IDC_PIN_SP,pin_speed);
	DDX_Text(pDX,IDC_PIN_CNT,pin_cnt);
	DDX_Text(pDX,IDC_UPIN_SP,upin_speed);
	DDX_Text(pDX,IDC_UPIN_TH,upin_threshold);
	DDX_Text(pDX,IDC_UPIN_DIS,upin_distance);
	DDX_CBIndex(pDX,IDC_PINNING_PARAM_ATTR,attr);
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CConPinningParameter, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CConPinningParameter 訊息處理常式

void CConPinningParameter::OnBnClickedOk()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	OnOK();
}
