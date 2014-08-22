// CigRgsDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "GPS.h"
#include "CigRgsDlg.h"


// CCigRgsDlg 對話方塊

IMPLEMENT_DYNAMIC(CCigRgsDlg, CDialog)
CCigRgsDlg::CCigRgsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCigRgsDlg::IDD, pParent)
{
}

CCigRgsDlg::~CCigRgsDlg()
{
}

void CCigRgsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_ms);
	DDX_Control(pDX, IDC_EDIT2, m_ns);
	DDX_Control(pDX, IDC_EDIT3, m_plldiv);
}


BEGIN_MESSAGE_MAP(CCigRgsDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CCigRgsDlg 訊息處理常式

BOOL CCigRgsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_ms.SetWindowText("0");
	m_ns.SetWindowText("0");
	m_plldiv.SetWindowText("0");

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX 屬性頁應傳回 FALSE
}

void CCigRgsDlg::OnBnClickedOk()
{
	CString temp1;
	CString& temp = temp1;
	char buff[50];
	
	m_ms.GetWindowText(temp);
	sprintf_s(buff, sizeof(buff), "%s", temp);
	MS = atoi(buff);
	if(MS > 63 || MS < 0)
	{
		AfxMessageBox("MS must be between 0 and 63");
		return;
	}
	m_ns.GetWindowText(temp);
	sprintf_s(buff, sizeof(buff), "%s", temp);
	NS = atoi(buff);
	if(NS > 63 || NS < 0)
	{
		AfxMessageBox("NS must be between 0 and 63");
		return;
	}
	m_plldiv.GetWindowText(temp);
	sprintf_s(buff, sizeof(buff),"%s",temp);
	PLLDIV = atoi(buff);
	if(PLLDIV > 7 || PLLDIV < 0)
	{
		AfxMessageBox("MS must be between 0 and 7");
		return;
	}
	OnOK();
}
