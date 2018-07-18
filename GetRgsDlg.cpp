// GetRgsDlg.cpp 
//

#include "stdafx.h"
#include "GPS.h"
#include "GetRgsDlg.h"


// CGetRgsDlg 
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


// CGetRgsDlg 

BOOL CGetRgsDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CString str;
	str.Format("%08x", lastAddress);
	m_address.SetLimitText(8);
	m_address.SetWindowText(str);


	return TRUE;  // return TRUE unless you set the focus to a control
}

void CGetRgsDlg::OnBnClickedOk()
{
	CString strText;
	//CString& temp = temp1;
	//char buff[50];
	
	m_address.GetWindowText(strText);
	//sprintf_s(buff, sizeof(buff),"%s",temp);
	address = ConvertCharToU32(strText);
	lastAddress = address;
	OnOK();
}
