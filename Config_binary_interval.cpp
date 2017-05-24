// Config_binary_interval.cpp 
//

#include "stdafx.h"
#include "GPS.h"
#include "Config_binary_interval.h"


// CConfig_binary_interval 

IMPLEMENT_DYNAMIC(CConfig_binary_interval, CDialog)
CConfig_binary_interval::CConfig_binary_interval(CWnd* pParent /*=NULL*/)
	: CDialog(CConfig_binary_interval::IDD, pParent)
{
}

CConfig_binary_interval::~CConfig_binary_interval()
{
}

void CConfig_binary_interval::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX,IDC_BINARY_INTERVAL,m_interval);
	DDX_Control(pDX,IDC_BINARY_ATTRI,m_attr);
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CConfig_binary_interval, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CConfig_binary_interval 

void CConfig_binary_interval::OnBnClickedOk()
{
	m_bin_interval = m_interval.GetCurSel();
	m_bin_attr = m_attr.GetCurSel();
	OnOK();
}

BOOL CConfig_binary_interval::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString temp;

	for (int i=0;i<256;i++)
	{
		temp.Format("%d",i);
		m_interval.AddString(temp);
	}
	m_interval.SetCurSel(0);
	m_attr.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
}
