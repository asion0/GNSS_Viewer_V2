// ConPosPinning.cpp 
//

#include "stdafx.h"
#include "GPS.h"
#include "ConPosPinning.h"

IMPLEMENT_DYNAMIC(CConPosPinning, CDialog)
CConPosPinning::CConPosPinning(CWnd* pParent /*=NULL*/)
	: CDialog(CConPosPinning::IDD, pParent)
{
	m_position_pinning = 0;
	m_attributes = 0;
}

CConPosPinning::~CConPosPinning()
{
}

void CConPosPinning::DoDataExchange(CDataExchange* pDX)
{
	DDX_CBIndex(pDX,IDC_POS_PINNING,m_position_pinning);
	DDX_CBIndex(pDX,IDC_PINN_ATTRIBUTE,m_attributes);

	DDX_Control(pDX,IDC_POS_PINNING,position_pinning);
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CConPosPinning, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

void CConPosPinning::OnBnClickedOk()
{
	OnOK();
}

BOOL CConPosPinning::OnInitDialog()
{
	CDialog::OnInitDialog();

	// disable;enable;
	position_pinning.ResetContent();
	//Old fw only has two mode, new has three.
#ifdef OPEN_PINNING_RESERVE
	position_pinning.AddString("Default");
	position_pinning.AddString("Enable");
	position_pinning.AddString("Disable");
#else
	position_pinning.AddString("Default");
	position_pinning.AddString("Enable");
#endif
	position_pinning.SetCurSel(0);
	return TRUE;  // return TRUE unless you set the focus to a control
}
