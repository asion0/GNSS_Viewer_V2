// ConPowerMode.cpp : 實作檔
//

#include "stdafx.h"
#include "GPS.h"
#include "ConPowerMode.h"


// CConPowerMode 對話方塊

IMPLEMENT_DYNAMIC(CConPowerMode, CDialog)
CConPowerMode::CConPowerMode(CWnd* pParent /*=NULL*/)
	: CDialog(CConPowerMode::IDD, pParent)
{
	mode = 0;
	attribute = 0;
}

CConPowerMode::~CConPowerMode()
{
}

void CConPowerMode::DoDataExchange(CDataExchange* pDX)
{
	DDX_CBIndex(pDX,IDC_MULTI_MODE,mode);
	DDX_CBIndex(pDX,IDC_MULTIMODE_ATTR,attribute);
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CConPowerMode, CDialog)
	ON_BN_CLICKED(IDOK, &CConPowerMode::OnBnClickedOk)
END_MESSAGE_MAP()


// CConPowerMode 訊息處理常式

void CConPowerMode::OnBnClickedOk()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	OnOK();
}
