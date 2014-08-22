// Con1PPS_PulseClkSource.cpp
//

#include "stdafx.h"
#include "GPS.h"
#include "Con1PPS_PulseClkSource.h"



IMPLEMENT_DYNAMIC(CCon1PPS_PulseClkSource, CDialog)

CCon1PPS_PulseClkSource::CCon1PPS_PulseClkSource(CWnd* pParent /*=NULL*/)
	: CDialog(CCon1PPS_PulseClkSource::IDD, pParent)
{
    clk_source=0;
	attr=0;
}

CCon1PPS_PulseClkSource::~CCon1PPS_PulseClkSource()
{
}

void CCon1PPS_PulseClkSource::DoDataExchange(CDataExchange* pDX)
{
	//DDX_CBIndex(pDX,IDC_1PPS_PULSE_CLOCK_SOURCE_SEL,clk_source);
	//DDX_CBIndex(pDX,IDC_1PPS_PULSE_CLK_SRC_ATTR_SEL,attr);
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_1PPS_PULSE_CLOCK_SOURCE_SEL, m_pulse_clk_src);
	DDX_Control(pDX, IDC_1PPS_PULSE_CLK_SRC_ATTR_SEL, m_attr);
}


BEGIN_MESSAGE_MAP(CCon1PPS_PulseClkSource, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

BOOL CCon1PPS_PulseClkSource::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_pulse_clk_src.SetCurSel(0);
	m_attr.SetCurSel(0);


	return TRUE;  // return TRUE unless you set the focus to a control
}

void CCon1PPS_PulseClkSource::OnBnClickedOk()
{

    clk_source=m_pulse_clk_src.GetCurSel();
	attr = m_attr.GetCurSel();

	OnOK();
}
