// Con_acquisition.cpp : 實作檔
//

#include "stdafx.h"
#include "GPS.h"
#include "Con_acquisition.h"


// CCon_acquisition 對話方塊

IMPLEMENT_DYNAMIC(CCon_acquisition, CDialog)

CCon_acquisition::CCon_acquisition(CWnd* pParent /*=NULL*/)
	: CDialog(CCon_acquisition::IDD, pParent)
{
	mode = 1;
	auto_power_off = attr = 0;
}

CCon_acquisition::~CCon_acquisition()
{
}

void CCon_acquisition::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX,IDC_ACQUISITION_MODE,cbo_auto_power_off);
	DDX_CBIndex(pDX,IDC_ACQUISITION_ATTR,attr);
	DDX_Control(pDX,IDC_RDO_ACQUISITION_BINARY,rdo_binary);
	DDX_Control(pDX,IDC_RDO_ACQUISITION_ASCII,rdo_ascii);
	DDX_Control(pDX,IDC_ACQUISITION_ASCII,cbo_auto_power_off_ascii);

	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCon_acquisition, CDialog)
	ON_BN_CLICKED(IDOK, &CCon_acquisition::OnBnClickedOk)
END_MESSAGE_MAP()


// CCon_acquisition 訊息處理常式

BOOL CCon_acquisition::OnInitDialog()
{
	CDialog::OnInitDialog();

	cbo_auto_power_off.SetCurSel(0);
	cbo_auto_power_off_ascii.SetCurSel(0);
	rdo_binary.SetCheck(!mode);
	rdo_ascii.SetCheck(mode);


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX 屬性頁應傳回 FALSE
}

void CCon_acquisition::OnBnClickedOk()
{
	mode = rdo_ascii.GetCheck();
	
	if (mode == 0)
	{
		auto_power_off = cbo_auto_power_off.GetCurSel();
	}else
	{
		auto_power_off = cbo_auto_power_off_ascii.GetCurSel();
	}

	OnOK();
}
