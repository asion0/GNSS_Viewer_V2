// Proprietary_nmea.cpp : 實作檔
//

#include "stdafx.h"
#include "GPS.h"
#include "Proprietary_nmea.h"


// CProprietary_nmea 對話方塊

IMPLEMENT_DYNAMIC(CProprietary_nmea, CDialog)
CProprietary_nmea::CProprietary_nmea(CWnd* pParent /*=NULL*/)
	: CDialog(CProprietary_nmea::IDD, pParent)
{

}

CProprietary_nmea::~CProprietary_nmea()
{
}

void CProprietary_nmea::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX,IDC_PSTI_ID,m_psti_id);
	DDX_Control(pDX,IDC_PSTI_INTERVAL,m_psti);

	DDX_Control(pDX,IDC_PROPRIETARY_ATTR,m_attr);

	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CProprietary_nmea, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CProprietary_nmea 訊息處理常式

void CProprietary_nmea::OnBnClickedOk()
{
	psti_id = m_psti_id.GetCurSel();
	psti_interval = m_psti.GetCurSel();
	attr = m_attr.GetCurSel();
	OnOK();
}

BOOL CProprietary_nmea::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此加入額外的初始化
	CString temp;

	for (int i=0;i<256;i++)
	{
		temp.Format("%d",i);
		m_psti_id.AddString(temp);
		m_psti.AddString(temp);
	}
	m_psti_id.SetCurSel(0);
	m_psti.SetCurSel(0);

	m_attr.SetCurSel(0);


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX 屬性頁應傳回 FALSE
}
