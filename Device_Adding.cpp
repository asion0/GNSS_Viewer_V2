// Device_Adding.cpp : 實作檔
//

#include "stdafx.h"
#include "GPS.h"
#include "Device_Adding.h"


// CDevice_Adding 對話方塊

IMPLEMENT_DYNAMIC(CDevice_Adding, CDialog)
CDevice_Adding::CDevice_Adding(CWnd* pParent /*=NULL*/)
	: CDialog(CDevice_Adding::IDD, pParent)
{
}

CDevice_Adding::~CDevice_Adding()
{
}

void CDevice_Adding::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX,IDC_COMPORT,m_cbo_com);
	DDX_Control(pDX,IDC_BAUDRATE,m_cbo_baudrate);
	CDialog::DoDataExchange(pDX);
}



BEGIN_MESSAGE_MAP(CDevice_Adding, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()

void CDevice_Adding::setPort_Baudrate(CString port,int baudrate)
{
	m_comport = port;
	m_baudrate = baudrate;
}
// CDevice_Adding 訊息處理常式

void CDevice_Adding::OnBnClickedOk()
{
	CString tmp;

	m_cbo_baudrate.GetLBText(m_cbo_baudrate.GetCurSel(),tmp);
	m_baudrate = atoi(tmp);
	OnOK();
}

BOOL CDevice_Adding::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此加入額外的初始化
	CString cbo_portname,now_baudrate,cbo_baudrate;

	m_cbo_com.EnableWindow(0);
	for (int i=0;i<m_cbo_com.GetCount();i++)
	{
		m_cbo_com.GetLBText(i,cbo_portname);
		if (cbo_portname.Compare(m_comport) == 0 )
		{
			m_cbo_com.SetCurSel(i);
			break;
		}
	}

	now_baudrate.Format("%d",m_baudrate);

	for (int i=0;i<m_cbo_baudrate.GetCount();i++)
	{
		m_cbo_baudrate.GetLBText(i,cbo_baudrate);
		if (cbo_baudrate.Compare(now_baudrate) == 0 )
		{
			m_cbo_baudrate.SetCurSel(i);
			break;
		}
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX 屬性頁應傳回 FALSE
}
