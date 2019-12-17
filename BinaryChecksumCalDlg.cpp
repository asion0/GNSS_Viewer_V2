// BinaryChecksumCalDlg.cpp
//

#include "stdafx.h"
#include "BinaryChecksumCalDlg.h"
#include "Resource.h"
#include "GPSDlg.h"
#include "Serial.h"


// BinaryChecksumCalDlg 
IMPLEMENT_DYNAMIC(BinaryChecksumCalDlg, CDialog)

BinaryChecksumCalDlg::BinaryChecksumCalDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_BIN_CHECKSUM_CAL, pParent)
{
  m_mode = CalcCheckSum;
}

BinaryChecksumCalDlg::~BinaryChecksumCalDlg()
{
}

void BinaryChecksumCalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(BinaryChecksumCalDlg, CDialog)
	ON_EN_CHANGE(IDC_CONTEXT, &BinaryChecksumCalDlg::OnEnChangeInput)
	ON_BN_CLICKED(IDC_SEND, &BinaryChecksumCalDlg::OnBnClickedSend)
END_MESSAGE_MAP()


// BinaryChecksumCalDlg 

BOOL BinaryChecksumCalDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
  if(m_mode == RawData)
  {
      GetDlgItem(IDC_STATIC1)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_STATIC2)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_STATIC3)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_STATIC4)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_STATIC5)->ShowWindow(SW_HIDE);
      GetDlgItem(IDC_CHECKSUM)->ShowWindow(SW_HIDE);
      SetWindowText("Binary command Tester");
  }
	return TRUE;  // return TRUE unless you set the focus to a control
}

void BinaryChecksumCalDlg::OnOK()
{
	CDialog::OnOK();
}

void BinaryChecksumCalDlg::OnEnChangeInput()
{
 	CString strInput;
	GetDlgItem(IDC_CONTEXT)->GetWindowText(strInput);

	//GetDlgItem(IDC_CHECKSUM)->SetWindowText(strInput + "_");
	BinaryData binData;
	if(!Utility::ConvertHexToBinary(strInput, binData))
	{
		GetDlgItem(IDC_BIN_SIZE)->SetWindowText("Invalidate Format!");
		GetDlgItem(IDC_CHECKSUM)->SetWindowText("Invalidate Format!");
		return;
	}

	U08 checkSum = 0;
	for(int i=0; i<binData.Size(); ++i)
	{
		checkSum ^= binData[i];
	}

	CString strOutput;
	strOutput.Format("%02X %02X", HIBYTE(binData.Size()), LOBYTE(binData.Size()));
	GetDlgItem(IDC_BIN_SIZE)->SetWindowText(strOutput);
	strOutput.Format("%02X", checkSum);
	GetDlgItem(IDC_CHECKSUM)->SetWindowText(strOutput);
}



void BinaryChecksumCalDlg::OnBnClickedSend()
{
	if(!CGPSDlg::gpsDlg->m_isConnectOn)
	{
		AfxMessageBox("Please connect to GNSS device");
		return;
	}

	CString strInput;
	GetDlgItem(IDC_CONTEXT)->GetWindowText(strInput);

	BinaryData binData;
	if(!Utility::ConvertHexToBinary(strInput, binData))
	{
		::AfxMessageBox("Invalidate Format!");
		return;
	}

  if(m_mode == RawData)
  {
	  BinaryCommand cmd(binData);
	  U08* pCmd = cmd.GetBuffer();
	  int inSize = cmd.Size();
	  CGPSDlg::gpsDlg->m_serial->SendData(binData.Ptr(), binData.Size());
    return;
  }

	BinaryCommand cmd(binData);
	U08* pCmd = cmd.GetBuffer();
	int inSize = cmd.Size();
	CGPSDlg::gpsDlg->m_serial->SendData(pCmd, inSize);
}
