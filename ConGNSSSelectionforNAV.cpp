// ConGNSSSelectionforNAV.cpp : 實作檔
//

#include "stdafx.h"
#include "GPS.h"
#include "ConGNSSSelectionforNAV.h"


// CConGNSSSelectionforNAV 對話方塊

IMPLEMENT_DYNAMIC(CConGNSSSelectionforNAV, CDialog)

CConGNSSSelectionforNAV::CConGNSSSelectionforNAV(CWnd* pParent /*=NULL*/)
	: CDialog(CConGNSSSelectionforNAV::IDD, pParent)
{
	selection = 0;
	attr = 0;
}

CConGNSSSelectionforNAV::~CConGNSSSelectionforNAV()
{
}

void CConGNSSSelectionforNAV::DoDataExchange(CDataExchange* pDX)
{
	DDX_CBIndex(pDX,IDC_COMBO_GNSS_SELECTION,selection);
	DDX_CBIndex(pDX,IDC_GNSS_SEL_ATTR,attr);
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CConGNSSSelectionforNAV, CDialog)
	ON_BN_CLICKED(IDOK, &CConGNSSSelectionforNAV::OnBnClickedOk)
END_MESSAGE_MAP()


// CConGNSSSelectionforNAV 訊息處理常式

void CConGNSSSelectionforNAV::OnBnClickedOk()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	OnOK();
}
