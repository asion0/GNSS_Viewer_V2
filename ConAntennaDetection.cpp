// ConAntennaDetection.cpp 
//

#include "stdafx.h"
#include "GPS.h"
#include "ConAntennaDetection.h"


// CConAntennaDetection 

IMPLEMENT_DYNAMIC(CConAntennaDetection, CDialog)

CConAntennaDetection::CConAntennaDetection(CWnd* pParent /*=NULL*/)
	: CDialog(CConAntennaDetection::IDD, pParent)
{
	attr = 0;
	antenna_control = 0;
}

CConAntennaDetection::~CConAntennaDetection()
{
}

void CConAntennaDetection::DoDataExchange(CDataExchange* pDX)
{
	DDX_CBIndex(pDX,IDC_ANTENNA_DETECT_ATTR,attr);
	DDX_Control(pDX,IDC_DETECT_SHORT,m_chk_short);
	DDX_Control(pDX,IDC_DETECT_ANTENNA,m_chk_antenna);
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CConAntennaDetection, CDialog)
	ON_BN_CLICKED(IDOK, &CConAntennaDetection::OnBnClickedOk)
END_MESSAGE_MAP()


// CConAntennaDetection 

BOOL CConAntennaDetection::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_chk_short.ShowWindow(SW_HIDE);
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CConAntennaDetection::OnBnClickedOk()
{
	//20150709 modify for fw spec, just ON/OFF.
	//if (m_chk_short.GetCheck() == 1)
	//	antenna_control |= 0x01;
	//if (m_chk_antenna.GetCheck() == 1)
	//	antenna_control |= 0x02;
	antenna_control = m_chk_antenna.GetCheck();

	OnOK();
}
