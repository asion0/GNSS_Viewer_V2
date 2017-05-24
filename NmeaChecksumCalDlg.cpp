// NmeaChecksumCalDlg.cpp 
//

#include "stdafx.h"
#include "NmeaChecksumCalDlg.h"
#include "Resource.h"
#include "GPSDlg.h"
#include "Serial.h"


// NmeaChecksumCalDlg 

IMPLEMENT_DYNAMIC(NmeaChecksumCalDlg, CDialog)

NmeaChecksumCalDlg::NmeaChecksumCalDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_NMEA_CHECKSUM_CAL, pParent)
{

}

NmeaChecksumCalDlg::~NmeaChecksumCalDlg()
{
}

void NmeaChecksumCalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(NmeaChecksumCalDlg, CDialog)
	ON_EN_CHANGE(IDC_INPUT, &NmeaChecksumCalDlg::OnEnChangeInput)
	ON_BN_CLICKED(IDC_SEND, &NmeaChecksumCalDlg::OnBnClickedSend)
END_MESSAGE_MAP()


// NmeaChecksumCalDlg 

BOOL NmeaChecksumCalDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	GetDlgItem(IDC_INPUT)->SetWindowText("");
	GetDlgItem(IDC_RESULT)->SetWindowText("$*00");
	return TRUE;  // return TRUE unless you set the focus to a control
}

CString NmeaChecksumCalDlg::GetFullNmeaString(const CString& s)
{	
	char checkSum = 0;
	for(int i=0; i<s.GetLength(); ++i)
	{
		checkSum ^= s.GetAt(i);
	}
	CString strResult;
	strResult.Format("$%s*%02X\r\n", s, checkSum);
	return strResult;
}

void NmeaChecksumCalDlg::OnEnChangeInput()
{
	CString strInput;
	GetDlgItem(IDC_INPUT)->GetWindowText(strInput);

	GetDlgItem(IDC_RESULT)->SetWindowText(GetFullNmeaString(strInput));
}

void NmeaChecksumCalDlg::OnBnClickedSend()
{
	if(!CGPSDlg::gpsDlg->m_isConnectOn)
	{
		AfxMessageBox("Please connect to GNSS device");
		return;
	}

	CString strInput;
	GetDlgItem(IDC_INPUT)->GetWindowText(strInput);
	CString strCmd = GetFullNmeaString(strInput);
	CGPSDlg::gpsDlg->m_serial->SendData((LPCSTR)strCmd, strCmd.GetLength());
}
