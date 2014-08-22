// GetRgsDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "GPS.h"
#include "GetRgsDlg.h"


// CGetRgsDlg 對話方塊
static unsigned int lastAddress = 0;

IMPLEMENT_DYNAMIC(CGetRgsDlg, CDialog)
CGetRgsDlg::CGetRgsDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGetRgsDlg::IDD, pParent)
{
}

CGetRgsDlg::~CGetRgsDlg()
{
}

void CGetRgsDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_address);
}


BEGIN_MESSAGE_MAP(CGetRgsDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CGetRgsDlg 訊息處理常式

BOOL CGetRgsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CString str;
	str.Format("%08x", lastAddress);
	m_address.SetLimitText(8);
	m_address.SetWindowText(str);


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX 屬性頁應傳回 FALSE
}

void CGetRgsDlg::OnBnClickedOk()
{
	CString temp1;
	CString& temp = temp1;
	char buff[50];
	
	m_address.GetWindowText(temp);
	sprintf_s(buff, sizeof(buff),"%s",temp);
	address = ConvertCharToU32(buff);
	lastAddress = address;
	OnOK();
}
