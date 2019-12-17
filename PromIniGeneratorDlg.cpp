// PromIniGeneratorDlg.cpp
//

#include "stdafx.h"
#include "PromIniGeneratorDlg.h"
#include "Resource.h"
#include "GPSDlg.h"
#include "Serial.h"


// PromIniGeneratorDlg
IMPLEMENT_DYNAMIC(PromIniGeneratorDlg, CDialog)

PromIniGeneratorDlg::PromIniGeneratorDlg(LPCSTR pszPromPath, int t/* = 0*/, CWnd* pParent/*=NULL*/)
	: CDialog(IDD_PROM_INI_GEN, pParent)
{
  promPath = pszPromPath;
  type = t;
}

PromIniGeneratorDlg::~PromIniGeneratorDlg()
{
}

void PromIniGeneratorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(PromIniGeneratorDlg, CDialog)
  ON_EN_CHANGE(IDC_TAG_VALUE, &PromIniGeneratorDlg::OnEnChangeInput)
  ON_BN_CLICKED(IDOK, &PromIniGeneratorDlg::OnBnClickedOk)
  ON_BN_CLICKED(IDC_TAG, &PromIniGeneratorDlg::OnBnClickedEnableTag)
	ON_BN_CLICKED(IDC_RENAME, &PromIniGeneratorDlg::OnBnClickedRename)
END_MESSAGE_MAP()


// PromIniGeneratorDlg 
void PromIniGeneratorDlg::OnEnChangeInput()
{
  UpdateUiStatus();
}

void PromIniGeneratorDlg::UpdateUiStatus()
{
  if(((CButton*)GetDlgItem(IDC_TAG))->GetCheck())
  {
    GetDlgItem(IDC_TAG_VALUE)->EnableWindow(TRUE);
  }
  else
  {
    GetDlgItem(IDC_TAG_VALUE)->EnableWindow(FALSE);
  }

  CString txt;
  U32 value;

  GetDlgItem(IDC_TAG_ADDR)->GetWindowText(txt);
	value = ConvertCharToU32(txt);
  txt.Format("%08X", value ^ 0xAAAAAAAA);
  GetDlgItem(IDC_TAG_ADDR2)->SetWindowText(txt);

  GetDlgItem(IDC_TAG_VALUE)->GetWindowText(txt);
	value = ConvertCharToU32(txt);
  txt.Format("%04X", value ^ 0x55555555);
  GetDlgItem(IDC_TAG_VALUE2)->SetWindowText(txt);

  //ta = Convert.ToUInt32(tagAddress.Text, 16) ^ 0xaaaaaaaa;
  //tc = Convert.ToUInt32(tagContent.Text, 16) ^ 0x55555555;

}

BOOL PromIniGeneratorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

  CString txt;
  CGPSDlg::CmdErrorCode ack = CGPSDlg::Timeout;
  BinaryData retVer;
  U16 retCrc;
  
  if(type == 0)
  {
    GetDlgItem(IDC_CRC32_T)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_CRC32)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_GNSSTYPE_T)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_GNSSTYPE)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_GNSSTYPE_C)->ShowWindow(SW_HIDE);
  }

  ((CEdit*)GetDlgItem(IDC_TAG_VALUE))->SetLimitText(4);
  GetDlgItem(IDC_PROM_PATH)->SetWindowText(promPath);
  //if(!Utility::IsFileExist(promPath))
  //{
  //  ::AfxMessageBox("PROM file doesn't exist!");
  //  goto Error;
  //}

  //Get Baud rate
  txt.Format("%0d", g_setting.BaudrateValue(g_setting.GetBaudrateIndex()));
  GetDlgItem(IDC_BAUD_RATE)->SetWindowText(txt);

  //Get Software version
  ack = CGPSDlg::gpsDlg->QuerySoftwareVersionSystemCode(CGPSDlg::Return, &retVer);
  if(CGPSDlg::Ack != ack)
  {
    ::AfxMessageBox("Query software version failed!");
    goto Error;
  }
	txt.Format("%02d.%02d.%02d", retVer[7], retVer[8], retVer[9]);
	GetDlgItem(IDC_K_VER)->SetWindowText(txt);
	txt.Format("%02d.%02d.%02d", retVer[11], retVer[12], retVer[13]);
	GetDlgItem(IDC_S_VER)->SetWindowText(txt);
	txt.Format("%04d%02d%02d", retVer[15] + 2000, retVer[16], retVer[17]);
	GetDlgItem(IDC_R_VER)->SetWindowText(txt);
  
  //Get Software CRC
  ack = CGPSDlg::gpsDlg->QuerySoftwareCrcSystemCode(CGPSDlg::Return, &retCrc);
  if(CGPSDlg::Ack != ack)
  {
    ::AfxMessageBox("Query software CRC failed!");
    goto Error;
  }
  txt.Format("%04X", retCrc);
	GetDlgItem(IDC_CRC)->SetWindowText(txt);

  if(type == 1)
  {
    U32 retCrc32;
    //Get Software CRC
    ack = CGPSDlg::gpsDlg->QuerySoftwareCrc32SystemCode(CGPSDlg::Return, &retCrc32);
    if(CGPSDlg::Ack != ack)
    {
      ::AfxMessageBox("Query software CRC32 failed!");
      goto Error;
    }
    txt.Format("%08X", retCrc32);
	  GetDlgItem(IDC_CRC32)->SetWindowText(txt);
  }

  //STI_02.02.04-01.07.28_V8_2525BD-RTK_Master_CRC_018a_115200_20170712.bin
  txt.Format("STI_%02d.%02d.%02d-%02d.%02d.%02d_%s_PRODUCT_CRC_%04x_%d_%04d%02d%02d.bin", 
    retVer[7], retVer[8], retVer[9],
    retVer[11], retVer[12], retVer[13],
    "V8",
    retCrc,
    g_setting.BaudrateValue(g_setting.GetBaudrateIndex()),
    retVer[15] + 2000, retVer[16], retVer[17]);
  GetDlgItem(IDC_NEW_NAME)->SetWindowText(txt);

  U32 tagValue, tagAddress = 0xFCFFC;
  ack = CGPSDlg::gpsDlg->QueryRegisterx(CGPSDlg::Return, tagAddress, &tagValue);
  if(CGPSDlg::Ack != ack)
  {
    ::AfxMessageBox("Query tag value failed!");
    goto Error;
  }
  txt.Format("%08X", tagAddress);
	GetDlgItem(IDC_TAG_ADDR)->SetWindowText(txt);

  tagValue = (tagValue >> 16) & 0xFFFF;
  txt.Format("%04X", (U16)tagValue);
	GetDlgItem(IDC_TAG_VALUE)->SetWindowText(txt);

  ((CButton*)GetDlgItem(IDC_TAG))->SetCheck(TRUE);
  UpdateUiStatus();
  return TRUE;

Error:
  EndDialog(IDCANCEL);
	return TRUE;  // return TRUE unless you set the focus to a control
}

void PromIniGeneratorDlg::OnOK()
{
	CDialog::OnOK();
}

void PromIniGeneratorDlg::OnBnClickedEnableTag()
{
  UpdateUiStatus();
}

void PromIniGeneratorDlg::OnBnClickedRename()
{
  CString source, newName;

  GetDlgItem(IDC_PROM_PATH)->GetWindowText(source);
  GetDlgItem(IDC_NEW_NAME)->GetWindowText(newName);
  if(newName.IsEmpty())
	{
		AfxMessageBox("Must enter a file name!");
		return;
	}

  BOOL ret = Utility::RenameFile(source, newName, TRUE, FALSE);
	if(!ret)
  {
		AfxMessageBox("Rename file failed!");
		return;
  }
  source = Utility::GetFilePath(source) + "\\" + newName;
  GetDlgItem(IDC_PROM_PATH)->SetWindowText(source);

}

void PromIniGeneratorDlg::OnBnClickedOk()
{
  CString source, newName, txt;

  GetDlgItem(IDC_PROM_PATH)->GetWindowText(source);
  newName = Utility::GetFilePathName(source) + ".ini";

  ::DeleteFile(newName);

  GetDlgItem(IDC_NEW_NAME)->GetWindowText(txt);
	if(!WritePrivateProfileString("Firmware", "Prom", txt, newName))
  {
    goto Error;
  }

  GetDlgItem(IDC_K_VER)->GetWindowText(txt);
	if(!WritePrivateProfileString("Firmware", "K_Version", txt, newName))
  {
    goto Error;
  }

  GetDlgItem(IDC_S_VER)->GetWindowText(txt);
	if(!WritePrivateProfileString("Firmware", "S_Version", txt, newName))
  {
    goto Error;
  }

  GetDlgItem(IDC_R_VER)->GetWindowText(txt);
	if(!WritePrivateProfileString("Firmware", "Rev", txt, newName))
  {
    goto Error;
  }

  GetDlgItem(IDC_CRC)->GetWindowText(txt);
	if(!WritePrivateProfileString("Firmware", "CRC", txt, newName))
  {
    goto Error;
  }

  GetDlgItem(IDC_BAUD_RATE)->GetWindowText(txt);
	if(!WritePrivateProfileString("Firmware", "Baudrate", txt, newName))
  {
    goto Error;
  }

  txt = "00000000";
	if(!WritePrivateProfileString("Firmware", "DopplerAddress", txt, newName))
  {
    goto Error;
  }
	if(!WritePrivateProfileString("Firmware", "DopplerSize", txt, newName))
  {
    goto Error;
  }
  
  if(((CButton*)GetDlgItem(IDC_TAG))->GetCheck())
  {
    GetDlgItem(IDC_TAG_ADDR2)->GetWindowText(txt);
	  if(!WritePrivateProfileString("Firmware", "Address", txt, newName))
    {
      goto Error;
    }
    
    GetDlgItem(IDC_TAG_VALUE2)->GetWindowText(txt);
	  if(!WritePrivateProfileString("Firmware", "Value", txt, newName))
    {
      goto Error;
    }
  }
  else
  {
	  if(!WritePrivateProfileString("Firmware", "Address", txt, newName))
    {
      goto Error;
    }
	  if(!WritePrivateProfileString("Firmware", "Value", txt, newName))
    {
      goto Error;
    }
  }
  ShellExecute(NULL, _T("open"), newName, NULL, NULL, SW_SHOWNORMAL);
  //EndDialog(IDOK);
  return;

Error:
  ::AfxMessageBox("Failed to generate ini file!");
}

