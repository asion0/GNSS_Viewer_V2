// ConfigureRfIc.cpp
//

#include "stdafx.h"
#include "ConfigureRfIc.h"
#include "GPSDlg.h"

// CConfigureRfIc
IMPLEMENT_DYNAMIC(CConfigureRfIc, CDialog)

CConfigureRfIc::CConfigureRfIc(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_CFG_RF_IC, pParent)
{

}

BEGIN_MESSAGE_MAP(CConfigureRfIc, CDialog)
	ON_BN_CLICKED(IDC_WRITE, &CConfigureRfIc::OnBnClickedWrite)
	ON_BN_CLICKED(IDC_READ, &CConfigureRfIc::OnBnClickedRead)
	ON_BN_CLICKED(IDC_CLOSE, &CConfigureRfIc::OnBnClickedClose)
	ON_CBN_SELCHANGE(IDC_TYPE, OnCbnSelChangeRtkMode)
END_MESSAGE_MAP()

// CConfigureRfIc
UINT CConfigureRfIc::staticWCtrlList[Setting::RFIC_REG_SIZE] = 
  { IDC_T_W_REG0, IDC_T_W_REG1, IDC_T_W_REG2, IDC_T_W_REG3, IDC_T_W_REG4, IDC_T_W_REG5,
    IDC_T_W_REG6, IDC_T_W_REG7, IDC_T_W_REG8, IDC_T_W_REG9, IDC_T_W_REG10, IDC_T_W_REG11,
    IDC_T_W_REG12, IDC_T_W_REG13, IDC_T_W_REG14, IDC_T_W_REG15, IDC_T_W_REG16, IDC_T_W_REG17 };

UINT CConfigureRfIc::editWCtrlList[Setting::RFIC_REG_SIZE] = 
  { IDC_W_REG0, IDC_W_REG1, IDC_W_REG2, IDC_W_REG3, IDC_W_REG4, IDC_W_REG5,
    IDC_W_REG6, IDC_W_REG7, IDC_W_REG8, IDC_W_REG9, IDC_W_REG10, IDC_W_REG11,
    IDC_W_REG12, IDC_W_REG13, IDC_W_REG14, IDC_W_REG15, IDC_W_REG16, IDC_W_REG17 };

UINT CConfigureRfIc::staticRCtrlList[Setting::RFIC_REG_SIZE] = 
  { IDC_T_R_REG0, IDC_T_R_REG1, IDC_T_R_REG2, IDC_T_R_REG3, IDC_T_R_REG4, IDC_T_R_REG5,
    IDC_T_R_REG6, IDC_T_R_REG7, IDC_T_R_REG8, IDC_T_R_REG9, IDC_T_R_REG10, IDC_T_R_REG11,
    IDC_T_R_REG12, IDC_T_R_REG13, IDC_T_R_REG14, IDC_T_R_REG15, IDC_T_R_REG16, IDC_T_R_REG17 };

UINT CConfigureRfIc::editRCtrlList[Setting::RFIC_REG_SIZE] = 
  { IDC_R_REG0, IDC_R_REG1, IDC_R_REG2, IDC_R_REG3, IDC_R_REG4, IDC_R_REG5,
    IDC_R_REG6, IDC_R_REG7, IDC_R_REG8, IDC_R_REG9, IDC_R_REG10, IDC_R_REG11,
    IDC_R_REG12, IDC_R_REG13, IDC_R_REG14, IDC_R_REG15, IDC_R_REG16, IDC_R_REG17 };

const BOOL type0_show[Setting::RFIC_REG_SIZE] = 
  { TRUE, TRUE, TRUE, FALSE, FALSE, FALSE,
    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE,
    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE };

const BOOL type1_show[Setting::RFIC_REG_SIZE] = 
  { TRUE, TRUE, TRUE, TRUE, TRUE, TRUE,
    TRUE, FALSE, FALSE, FALSE, FALSE, FALSE,
    FALSE, FALSE, FALSE, FALSE, FALSE, FALSE };

BOOL CConfigureRfIc::OnInitDialog()
{
	CDialog::OnInitDialog();

  for(int i = 0; i < Setting::RFIC_REG_SIZE; ++i)
  {
	  ((CEdit*)GetDlgItem(editWCtrlList[i]))->SetLimitText(8);
  }

  ((CComboBox*)GetDlgItem(IDC_TYPE))->SetCurSel(g_setting.configRfIcType);
  OnCbnSelChangeRtkMode();

  CString txt;
  for(int i = 0; i < Setting::RFIC_REG_SIZE; ++i)
  {
    txt.Format("%08X", g_setting.configRfIcReg[i]);
    GetDlgItem(editWCtrlList[i])->SetWindowText(txt);
  }

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CConfigureRfIc::OnBnClickedWrite()
{	
  CString txt;
	U32 reg[Setting::RFIC_REG_SIZE];
  int type = ((CComboBox*)GetDlgItem(IDC_TYPE))->GetCurSel();

  g_setting.configRfIcType = type;
  for(int i = 0; i < Setting::RFIC_REG_SIZE; ++i)
  {
    GetDlgItem(editWCtrlList[i])->GetWindowText(txt);
	  reg[i] = ConvertCharToU32(txt);
    g_setting.configRfIcReg[i] = reg[i];
  }

  if(CGPSDlg::gpsDlg->DoCConfigRfIcDirect(type, reg, Setting::RFIC_REG_SIZE))
  {
    g_setting.Save();
    ::AfxMessageBox("Configure RF IC successfully");
  }
  return;
}

void CConfigureRfIc::OnBnClickedRead()
{
  BinaryData ackCmd;
  int type = ((CComboBox*)GetDlgItem(IDC_TYPE))->GetCurSel();

  CGPSDlg::CmdErrorCode err = CGPSDlg::gpsDlg->QueryRfIc(CGPSDlg::Return, &ackCmd, type);
  if(err != CGPSDlg::Ack)
	{
    ::AfxMessageBox("Read Rf IC register failed!");
    return;
  }

  int idx = 6;
  CString txt;
  const BOOL* showTable = (type == 0) ? type0_show : type1_show;
  for(int i = 0; i < Setting::RFIC_REG_SIZE; ++i)
  {
    if(showTable[i])
    {
      
      txt.Format("%08X", ConvertLeonU32(ackCmd.Ptr(idx)));
      GetDlgItem(editRCtrlList[i])->SetWindowText(txt);
      idx += 4;
    }
  }
}

void CConfigureRfIc::OnBnClickedClose()
{
  CDialog::OnOK();
}

void CConfigureRfIc::OnCbnSelChangeRtkMode()
{
  const char* type0_name[Setting::RFIC_REG_SIZE] = 
    { "R00  0x", "R01  0x", "R02  0x", "---", "---", "---",
      "---", "---", "---", "---", "---", "---",
      "---", "---", "---", "---", "---", "---" };

  const char* type1_name[Setting::RFIC_REG_SIZE] = 
    { "R02  0x", "R04  0x", "R06  0x", "R10  0x", "R12  0x", "R13  0x",
      "R14  0x", "R16  0x", "R17  0x", "R18  0x", "R20  0x", "R49  0x",
      "R15  0x", "R15  0x", "R19  0x", "R19  0x", "R23  0x", "R23  0x" };

  int t = ((CComboBox*)GetDlgItem(IDC_TYPE))->GetCurSel();

  for(int i = 0; i < Setting::RFIC_REG_SIZE; ++i)
  {
    if(t == 0)
    {
      GetDlgItem(staticWCtrlList[i])->SetWindowText(type0_name[i]);
      GetDlgItem(staticWCtrlList[i])->EnableWindow(type0_name[i][0] != '-');
      GetDlgItem(staticRCtrlList[i])->SetWindowText(type0_name[i]);
      GetDlgItem(staticRCtrlList[i])->EnableWindow(type0_name[i][0] != '-');
      GetDlgItem(staticRCtrlList[i])->ShowWindow((type0_show[i]) ? SW_SHOW : SW_HIDE);
      GetDlgItem(editRCtrlList[i])->ShowWindow((type0_show[i]) ? SW_SHOW : SW_HIDE);
      GetDlgItem(IDC_READ)->EnableWindow(FALSE);
     }
    else if(t == 1)
    {
      GetDlgItem(staticWCtrlList[i])->SetWindowText(type1_name[i]);
      GetDlgItem(staticWCtrlList[i])->EnableWindow(TRUE);
      GetDlgItem(staticRCtrlList[i])->SetWindowText(type1_name[i]);
      GetDlgItem(staticRCtrlList[i])->EnableWindow(TRUE);
      GetDlgItem(staticRCtrlList[i])->ShowWindow((type1_show[i]) ? SW_SHOW : SW_HIDE);
      GetDlgItem(editRCtrlList[i])->ShowWindow((type1_show[i]) ? SW_SHOW : SW_HIDE);
      GetDlgItem(IDC_READ)->EnableWindow(TRUE);
    }
  }
}

