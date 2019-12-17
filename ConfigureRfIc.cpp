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
/*
BEGIN_MESSAGE_MAP(CConfigureRfIc, CDialog)
  ON_WM_CLOSE()
  ON_COMMAND_RANGE(IDC_WRITE00, IDC_WRITE22, &CConfigureRfIc::OnBnClickedSet)
	ON_BN_CLICKED(IDC_DEFAULT, &CConfigureRfIc::OnBnClickedDefault)
	ON_BN_CLICKED(IDC_WRITE, &CConfigureRfIc::OnBnClickedWrite)
	ON_BN_CLICKED(IDC_READ, &CConfigureRfIc::OnBnClickedRead)
	ON_BN_CLICKED(IDC_CLOSE, &CConfigureRfIc::OnBnClickedClose)
	ON_BN_CLICKED(IDC_EXPORT, &CConfigureRfIc::OnBnClickedExport)
	ON_CBN_SELCHANGE(IDC_TYPE, OnCbnSelChangeType)
END_MESSAGE_MAP()


// CConfigureRfIc
UINT CConfigureRfIc::staticWCtrlList[Setting::RFIC_W_REG_SIZE] = 
  { IDC_T_W_REG0, IDC_T_W_REG1, IDC_T_W_REG2, IDC_T_W_REG3, IDC_T_W_REG4, IDC_T_W_REG5,
    IDC_T_W_REG6, IDC_T_W_REG7, IDC_T_W_REG8, IDC_T_W_REG9, IDC_T_W_REG10, IDC_T_W_REG11,
    IDC_T_W_REG12, IDC_T_W_REG13, IDC_T_W_REG14, IDC_T_W_REG15, IDC_T_W_REG16, IDC_T_W_REG17,
    IDC_T_W_REG18, IDC_T_W_REG19, IDC_T_W_REG20, IDC_T_W_REG21, IDC_T_W_REG22, IDC_T_W_REG23 };

UINT CConfigureRfIc::editWCtrlList[Setting::RFIC_W_REG_SIZE] = 
  { IDC_W_REG0, IDC_W_REG1, IDC_W_REG2, IDC_W_REG3, IDC_W_REG4, IDC_W_REG5,
    IDC_W_REG6, IDC_W_REG7, IDC_W_REG8, IDC_W_REG9, IDC_W_REG10, IDC_W_REG11,
    IDC_W_REG12, IDC_W_REG13, IDC_W_REG14, IDC_W_REG15, IDC_W_REG16, IDC_W_REG17,
    IDC_W_REG18, IDC_W_REG19, IDC_W_REG20, IDC_W_REG21, IDC_W_REG22, IDC_W_REG23 };

UINT CConfigureRfIc::staticRCtrlList[Setting::RFIC_R_REG_SIZE] = 
  { IDC_T_R_REG0, IDC_T_R_REG1, IDC_T_R_REG2, IDC_T_R_REG3, IDC_T_R_REG4, IDC_T_R_REG5,
    IDC_T_R_REG6, IDC_T_R_REG7, IDC_T_R_REG8, IDC_T_R_REG9, IDC_T_R_REG10, IDC_T_R_REG11,
    IDC_T_R_REG12, IDC_T_R_REG13, IDC_T_R_REG14, IDC_T_R_REG15, IDC_T_R_REG16, IDC_T_R_REG17,
    IDC_T_R_REG18, IDC_T_R_REG19, IDC_T_R_REG20, IDC_T_R_REG21, IDC_T_R_REG22, IDC_T_R_REG23,
    IDC_T_R_REG24, IDC_T_R_REG25};

UINT CConfigureRfIc::editRCtrlList[Setting::RFIC_R_REG_SIZE] = 
  { IDC_R_REG0, IDC_R_REG1, IDC_R_REG2, IDC_R_REG3, IDC_R_REG4, IDC_R_REG5,
    IDC_R_REG6, IDC_R_REG7, IDC_R_REG8, IDC_R_REG9, IDC_R_REG10, IDC_R_REG11,
    IDC_R_REG12, IDC_R_REG13, IDC_R_REG14, IDC_R_REG15, IDC_R_REG16, IDC_R_REG17,
    IDC_R_REG18, IDC_R_REG19, IDC_R_REG20, IDC_R_REG21, IDC_R_REG22, IDC_R_REG23, 
    IDC_R_REG24, IDC_R_REG25 };

BOOL CConfigureRfIc::OnInitDialog()
{
	CDialog::OnInitDialog();
  GetWindowText(m_title);
  for(int i = 0; i < Setting::RFIC_W_REG_SIZE; ++i)
  {
	  ((CEdit*)GetDlgItem(editWCtrlList[i]))->SetLimitText(8);
  }

  ((CComboBox*)GetDlgItem(IDC_TYPE))->SetCurSel(g_setting.configRfIcType);
  OnCbnSelChangeType();

	return TRUE;  // return TRUE unless you set the focus to a control
}
void CConfigureRfIc::OnBnClickedDefault()
{	
  CString txt;
  //int defaultValue[] = { 
  //  0x0053FF, 0x800D02, 0x260115, 0x177313, 0x320200,
  //  0x0E6D30, 0x7E51FF, 0x800D02, 0x060115, 0x177313,
  //  0x339B4C, 0x52D800, 0x01E142, 0x6F8982, 0x000004,
  //  0xA91110, 0x089448, 0x089448, 0x005182, 0x4D298A, 
  //  0x080040, 0x080040, 0x81A00C };  
  int defaultValue[] = { 
    0xC0F162, 0x37C4C2, 0x0053FF, 0x840D02, 0x220111,
    0x111333, 0x34444C, 0x0E6D30, 0x7E51FF, 0x840D02,
    0x420111, 0x177333, 0x339B4E, 0x52D800, 0x000000,
    0x081430, 0xB06582, 0x4B290A, 0x080040, 0x080040, 
    0x80200C, 0xD12EA0, 0xD12E80, 0x37C4C3 };

  for(int i = 0; i < Setting::RFIC_W_REG_SIZE; ++i)
  {
    txt.Format("%06X", defaultValue[i]);
    GetDlgItem(editWCtrlList[i])->SetWindowText(txt);
  }
  return;
}

void CConfigureRfIc::OnBnClickedSet(UINT uid)
{
  UINT id = uid - IDC_WRITE00;
  U08 addr;
  U32 data;
  CString txt;
  GetDlgItem(staticWCtrlList[id])->GetWindowTextA(txt);
  txt.Replace("  0x", "");
  txt.Replace("R", "");
  addr = atoi(txt);

  GetDlgItem(editWCtrlList[id])->GetWindowText(txt);
	data = ConvertCharToU32(txt);

  if(!SetRfRegister(addr, data, false))
  {
    ::AfxMessageBox("Set Rf register failed!");
  }
}

void CConfigureRfIc::UpdateUiSetting()
{
  CString txt;
	U32 reg[Setting::RFIC_W_REG_SIZE];
  int type = ((CComboBox*)GetDlgItem(IDC_TYPE))->GetCurSel();

  g_setting.configRfIcType = type;
  for(int i = 0; i < Setting::RFIC_W_REG_SIZE; ++i)
  {
    GetDlgItem(editWCtrlList[i])->GetWindowText(txt);
	  reg[i] = ConvertCharToU32(txt);
    g_setting.configRfIcReg[i] = reg[i];
  }
  g_setting.Save();
}

void CConfigureRfIc::OnBnClickedWrite()
{	
  CString txt;
	U32 reg[Setting::RFIC_W_REG_SIZE];
  int type = ((CComboBox*)GetDlgItem(IDC_TYPE))->GetCurSel();

  g_setting.configRfIcType = type;
  for(int i = 0; i < Setting::RFIC_W_REG_SIZE; ++i)
  {
    GetDlgItem(editWCtrlList[i])->GetWindowText(txt);
	  reg[i] = ConvertCharToU32(txt);
    g_setting.configRfIcReg[i] = reg[i];
  }
  g_setting.Save();

  if(type == 2)
  {
    U08 addr;
    bool allSuccessful = true;
    for(int i = 0; i < Setting::RFIC_W_REG_SIZE; ++i)
    {
      GetDlgItem(staticWCtrlList[i])->GetWindowText(txt);
      txt.Replace("  0x", "");
      txt.Replace("R", "");
      addr = atoi(txt);
      if(!SetRfRegister(addr, reg[i], true))
      {
        allSuccessful = false;
        break;
      }
    }
    ::AfxMessageBox((allSuccessful) ? "Set RF IC registers successfully" : "Set RF IC registers failed!");
  }
  else
  {
    if(CGPSDlg::gpsDlg->DoCConfigRfIcDirect(type, reg, Setting::RFIC_W_REG_SIZE))
    {
      ::AfxMessageBox("Set RF IC register successfully");
    }
    else
    {
      ::AfxMessageBox("Set RF IC register failed!");
    }
  }
  return;
}

void CConfigureRfIc::OnBnClickedRead()
{
  BinaryData ackCmd;
  CString txt;
  int type = ((CComboBox*)GetDlgItem(IDC_TYPE))->GetCurSel();

  if(type == 2)
  {
    for(int i = 0; i < Setting::RFIC_R_REG_SIZE; ++i)
    {
        if(!GetDlgItem(editRCtrlList[i])->IsWindowVisible())
        {
          return;
        }
        GetDlgItem(staticRCtrlList[i])->GetWindowText(txt);
        txt.Replace("  0x", "");
        txt.Replace("R", "");
        U08 id = atoi(txt);

        CGPSDlg::CmdErrorCode err = CGPSDlg::gpsDlg->QueryOneRfRegister(CGPSDlg::Return, &ackCmd, id);
        if(err != CGPSDlg::Ack)
	      {
          ::AfxMessageBox("Get RF IC registers failed!");
          return;
        }

        txt.Format("%06X", ConvertLeonU32(ackCmd.Ptr(6)) & 0xFFFFFF);
        GetDlgItem(editRCtrlList[i])->SetWindowText(txt);
    }
    ::AfxMessageBox("Get RF IC registers successfully.");
  }
  else
  {
    CGPSDlg::CmdErrorCode err = CGPSDlg::gpsDlg->QueryRfIc(CGPSDlg::Return, &ackCmd, type);
    if(err != CGPSDlg::Ack)
	  {
      ::AfxMessageBox("Get Rf IC register failed!");
      return;
    }

    int idx = 6;
    for(int i = 0; i < Setting::RFIC_R_REG_SIZE; ++i)
    {
        if(!GetDlgItem(editRCtrlList[i])->IsWindowVisible())
        {
          break;
        }
        txt.Format("%08X", ConvertLeonU32(ackCmd.Ptr(idx)));
        GetDlgItem(editRCtrlList[i])->SetWindowText(txt);
        idx += 4;
    }
  }
}

void CConfigureRfIc::OnClose()
{
  UpdateUiSetting();
	CDialog::OnClose();
}

void CConfigureRfIc::OnBnClickedClose()
{
  UpdateUiSetting();
  CDialog::OnOK();
}

void CConfigureRfIc::OnBnClickedExport()
{
	CTime t = CTime::GetCurrentTime();
	CString fileName;
	fileName.Format("rf_reg%02d%02d%02d_%02d%02d%02d.txt", 
    t.GetYear(), t.GetMonth(), t.GetDay(),
		t.GetHour(), t.GetMinute(), t.GetSecond());

	CFileDialog dlgFile(FALSE, _T("txt"), fileName, 
    OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("ALL Files (*.*)|*.*||"), this);

	dlgFile.GetOFN().lpstrFile = fileName.GetBuffer(MyMaxPath);
	dlgFile.GetOFN().nMaxFile = MyMaxPath;
	INT_PTR nResult = dlgFile.DoModal();
	fileName.ReleaseBuffer();

	if(nResult != IDOK)
	{
		return;
	}

	CFile f;
	f.Open(fileName, CFile::modeReadWrite | CFile::modeCreate | CFile::shareDenyWrite);
  CString name, reg, txt;
  //const BOOL* showTable = (type == 0) ? type0_show : type1_show;
  for(int i = 0; i < Setting::RFIC_R_REG_SIZE; ++i)
  {
      if(!GetDlgItem(editRCtrlList[i])->IsWindowVisible())
      {
        continue;
      }
      GetDlgItem(editRCtrlList[i])->GetWindowText(reg);
      GetDlgItem(staticRCtrlList[i])->GetWindowText(name);
      name.Replace("  ", ":");
      txt.Format("%s%06X\r\n", name, ConvertCharToU32(reg));
  	  f.Write(txt, txt.GetLength());
  }
	f.Close();
}

void CConfigureRfIc::OnCbnSelChangeType()
{
  const char* type0_w_name[] = 
    { "R00  0x", "R01  0x", "R02  0x"};

  const char* type1_w_name[] = 
    { "R02  0x", "R04  0x", "R06  0x", "R10  0x", "R12  0x", "R13  0x",
      "R14  0x", "R16  0x", "R17  0x", "R18  0x", "R20  0x", "R49  0x",
      "R15  0x", "R15  0x", "R19  0x", "R19  0x", "R23  0x", "R23  0x" };

  //const char* type2_w_name[] = 
  //  { "R00  0x", "R01  0x", "R02  0x", "R03  0x", "R04  0x", "R05  0x",
  //    "R06  0x", "R07  0x", "R08  0x", "R09  0x", "R10  0x", "R11  0x",
  //    "R12  0x", "R13  0x", "R14  0x", "R15  0x", "R16  0x", "R16  0x",
  //    "R17  0x", "R18  0x", "R19  0x", "R20  0x", "R21  0x" };
  const char* type2_w_name[] = 
    { "R12  0x", "R13  0x", "R00  0x", "R01  0x", "R02  0x", "R03  0x",
      "R04  0x", "R05  0x", "R06  0x", "R07  0x", "R08  0x", "R09  0x",
      "R10  0x", "R11  0x", "R14  0x", "R16  0x", "R17  0x", "R18  0x",
      "R19  0x", "R20  0x", "R21  0x", "R22  0x", "R22  0x", "R13  0x"};

  int t = ((CComboBox*)GetDlgItem(IDC_TYPE))->GetCurSel();

  int i1 = 0;
  int i1Size;
  const char** typeName = NULL;
  if(t == 0)
  {
    i1Size = sizeof(type0_w_name) / sizeof(type0_w_name[0]);
    typeName = type0_w_name;
    GetDlgItem(IDC_DEFAULT)->EnableWindow(FALSE);
  }
  else if(t == 1)
  {
    i1Size = sizeof(type1_w_name) / sizeof(type1_w_name[0]);
    typeName = type1_w_name;
    GetDlgItem(IDC_DEFAULT)->EnableWindow(FALSE);
  }
  else if(t == 2)
  {
    i1Size = sizeof(type2_w_name) / sizeof(type2_w_name[0]);
    typeName = type2_w_name;
    GetDlgItem(IDC_DEFAULT)->EnableWindow(TRUE);
  }  
    
  for(; i1 < i1Size; ++i1)
  {
    GetDlgItem(staticWCtrlList[i1])->SetWindowText(typeName[i1]);
    GetDlgItem(staticWCtrlList[i1])->ShowWindow(SW_SHOW);
    GetDlgItem(editWCtrlList[i1])->ShowWindow(SW_SHOW);
  }
  
  for(int i2 = i1; i2 < Setting::RFIC_W_REG_SIZE; ++i2)
  {
    GetDlgItem(staticWCtrlList[i2])->ShowWindow(SW_HIDE);
    GetDlgItem(editWCtrlList[i2])->ShowWindow(SW_HIDE);
  }

  CString txt;
  BOOL hasValue = FALSE;
  SetWindowText(m_title);
  if(t == 2)
  {
      for(int i = 0; i < Setting::RFIC_W_REG_SIZE; ++i)
      {
          //txt.Format("%06X", ConvertLeonU32(ackCmd.Ptr(idx)));
          txt.Format("%06X", g_setting.configRfIcReg[i]);
          GetDlgItem(editWCtrlList[i])->SetWindowText(txt);
          //idx += 4;
      }
      SetWindowText(m_title + " (Get successfully)");
      hasValue = TRUE;
  }
  
  if(hasValue == FALSE)
  {
    for(int i = 0; i < Setting::RFIC_W_REG_SIZE; ++i)
    {
      txt.Format("%08X", g_setting.configRfIcReg[i]);
      GetDlgItem(editWCtrlList[i])->SetWindowText(txt);
    }
  }

  const char* type1_r_name[] = 
    { "R02  0x", "R04  0x", "R06  0x", "R10  0x", "R12  0x", "R13  0x",
      "R14  0x", "R16  0x", "R17  0x", "R18  0x", "R20  0x", "R49  0x",
      "R15  0x", "R15  0x", "R19  0x", "R19  0x", "R23  0x", "R23  0x" };

  //const char* type2_r_name[] = 
  //  { "R00  0x", "R01  0x", "R02  0x", "R03  0x", "R04  0x", "R05  0x",
  //    "R06  0x", "R07  0x", "R08  0x", "R09  0x", "R10  0x", "R11  0x",
  //    "R12  0x", "R13  0x", "R14  0x", "R15  0x", "R16  0x", "R17  0x", 
  //    "R18  0x", "R19  0x", "R20  0x", "R21  0x", "R61  0x", "R62  0x", 
  //    "R63  0x" };  
    const char* type2_r_name[] = 
    { "R00  0x", "R01  0x", "R02  0x", "R03  0x", "R04  0x", "R05  0x",
      "R06  0x", "R07  0x", "R08  0x", "R09  0x", "R10  0x", "R11  0x",
      "R12  0x", "R13  0x", "R14  0x", "R15  0x", "R16  0x", "R17  0x", 
      "R18  0x", "R19  0x", "R20  0x", "R21  0x", "R22  0x", "R61  0x", 
      "R62  0x", "R63  0x" };

  i1 = 0;
  if(t == 0)
  {
    i1Size = 0;
    typeName = type0_w_name;
    GetDlgItem(IDC_READ)->EnableWindow(FALSE);
    GetDlgItem(IDC_EXPORT)->EnableWindow(FALSE);
  }
  else if(t == 1)
  {
    i1Size = sizeof(type1_r_name) / sizeof(type1_r_name[0]);
    typeName = type1_r_name;
    GetDlgItem(IDC_READ)->EnableWindow(TRUE);
    GetDlgItem(IDC_EXPORT)->EnableWindow(TRUE);
  }
  else if(t == 2)
  {
    i1Size = sizeof(type2_r_name) / sizeof(type2_r_name[0]);
    typeName = type2_r_name;
    GetDlgItem(IDC_READ)->EnableWindow(TRUE);
    GetDlgItem(IDC_EXPORT)->EnableWindow(TRUE);
  }  
    
  for(; i1 < i1Size; ++i1)
  {
    GetDlgItem(staticRCtrlList[i1])->SetWindowText(typeName[i1]);
    GetDlgItem(staticRCtrlList[i1])->ShowWindow(SW_SHOW);
    GetDlgItem(editRCtrlList[i1])->ShowWindow(SW_SHOW);
  }
  
  for(int i2 = i1; i2 < Setting::RFIC_R_REG_SIZE; ++i2)
  {
    GetDlgItem(staticRCtrlList[i2])->ShowWindow(SW_HIDE);
    GetDlgItem(editRCtrlList[i2])->ShowWindow(SW_HIDE);
  }
}

bool CConfigureRfIc::SetRfRegister(U08 addr, U32 data, bool silence)
{
	CWaitCursor wait;

	BinaryData cmd(6);
  *cmd.GetBuffer(0) = 0x64;
  *cmd.GetBuffer(1) = 0x6F;
	*cmd.GetBuffer(2) = addr;
	*cmd.GetBuffer(3) = data >> 16 & 0xFF;
	*cmd.GetBuffer(4) = data >> 8 & 0xFF;
	*cmd.GetBuffer(5) = data & 0xFF;

  BinaryCommand configCmd;
  configCmd.SetData(cmd);

  CString txt;
  txt.Format("Set RF register(0x%02X) successfully", addr);
  return CGPSDlg::gpsDlg->ExecuteConfigureCommand(configCmd.GetBuffer(), configCmd.Size(), (silence) ? NULL : txt, false);
}
*/

BEGIN_MESSAGE_MAP(CConfigureRfIc, CDialog)
  ON_WM_CLOSE()
  ON_COMMAND_RANGE(IDC_WRITE00, IDC_WRITE23, &CConfigureRfIc::OnBnClickedSet)
	//ON_BN_CLICKED(IDC_DEFAULT, &CConfigureRfIc::OnBnClickedDefault)
	ON_BN_CLICKED(IDC_WRITE, &CConfigureRfIc::OnBnClickedWrite)
	ON_BN_CLICKED(IDC_READ, &CConfigureRfIc::OnBnClickedRead)
	ON_BN_CLICKED(IDC_CLOSE, &CConfigureRfIc::OnBnClickedClose)
	ON_BN_CLICKED(IDC_EXPORT, &CConfigureRfIc::OnBnClickedExport)
	//ON_CBN_SELCHANGE(IDC_TYPE, OnCbnSelChangeType)
END_MESSAGE_MAP()


// CConfigureRfIc
UINT CConfigureRfIc::staticWCtrlList[Setting::RFIC_W_REG_SIZE] = 
  { IDC_T_W_REG0, IDC_T_W_REG1, IDC_T_W_REG2, IDC_T_W_REG3, IDC_T_W_REG4, IDC_T_W_REG5,
    IDC_T_W_REG6, IDC_T_W_REG7, IDC_T_W_REG8, IDC_T_W_REG9, IDC_T_W_REG10, IDC_T_W_REG11,
    IDC_T_W_REG12, IDC_T_W_REG13, IDC_T_W_REG14, IDC_T_W_REG15, IDC_T_W_REG16, IDC_T_W_REG17,
    IDC_T_W_REG18, IDC_T_W_REG19, IDC_T_W_REG20, IDC_T_W_REG21, IDC_T_W_REG22 };

UINT CConfigureRfIc::editWCtrlList[Setting::RFIC_W_REG_SIZE] = 
  { IDC_W_REG0, IDC_W_REG1, IDC_W_REG2, IDC_W_REG3, IDC_W_REG4, IDC_W_REG5,
    IDC_W_REG6, IDC_W_REG7, IDC_W_REG8, IDC_W_REG9, IDC_W_REG10, IDC_W_REG11,
    IDC_W_REG12, IDC_W_REG13, IDC_W_REG14, IDC_W_REG15, IDC_W_REG16, IDC_W_REG17,
    IDC_W_REG18, IDC_W_REG19, IDC_W_REG20, IDC_W_REG21, IDC_W_REG22 };

UINT CConfigureRfIc::staticRCtrlList[Setting::RFIC_R_REG_SIZE] = 
  { IDC_T_R_REG0, IDC_T_R_REG1, IDC_T_R_REG2 };

UINT CConfigureRfIc::editRCtrlList[Setting::RFIC_R_REG_SIZE] = 
  { IDC_R_REG0, IDC_R_REG1, IDC_R_REG2 };

BOOL CConfigureRfIc::OnInitDialog()
{
	CDialog::OnInitDialog();
  GetWindowText(m_title);
  for(int i = 0; i < Setting::RFIC_W_REG_SIZE; ++i)
  {
	  ((CEdit*)GetDlgItem(editWCtrlList[i]))->SetLimitText(6);
  }

  //((CComboBox*)GetDlgItem(IDC_TYPE))->SetCurSel(g_setting.configRfIcType);
  OnCbnSelChangeType();

	return TRUE;  // return TRUE unless you set the focus to a control
}

//void CConfigureRfIc::OnBnClickedDefault()
//{	
//  CString txt;
//  //int defaultValue[] = { 
//  //  0x0053FF, 0x800D02, 0x260115, 0x177313, 0x320200,
//  //  0x0E6D30, 0x7E51FF, 0x800D02, 0x060115, 0x177313,
//  //  0x339B4C, 0x52D800, 0x01E142, 0x6F8982, 0x000004,
//  //  0xA91110, 0x089448, 0x089448, 0x005182, 0x4D298A, 
//  //  0x080040, 0x080040, 0x81A00C };  
//  int defaultValue[] = { 
//    0xC0F162, 0x37C4C2, 0x0053FF, 0x840D02, 0x220111,
//    0x111333, 0x34444C, 0x0E6D30, 0x7E51FF, 0x840D02,
//    0x420111, 0x177333, 0x339B4E, 0x52D800, 0x000000,
//    0x081430, 0xB06582, 0x4B290A, 0x080040, 0x080040, 
//    0x80200C, 0xD12EA0, 0xD12E80, 0x37C4C3 };
//
//  for(int i = 0; i < Setting::RFIC_W_REG_SIZE; ++i)
//  {
//    txt.Format("%06X", defaultValue[i]);
//    GetDlgItem(editWCtrlList[i])->SetWindowText(txt);
//  }
//  return;
//}

void CConfigureRfIc::OnBnClickedSet(UINT uid)
{
  UINT id = uid - IDC_WRITE00;
  U08 addr;
  U32 data;
  CString txt;
  GetDlgItem(staticWCtrlList[id])->GetWindowTextA(txt);
  txt.Replace("  0x", "");
  txt.Replace("R", "");
  addr = atoi(txt);

  GetDlgItem(editWCtrlList[id])->GetWindowText(txt);
	data = ConvertCharToU32(txt);

  if(!SetRfRegister(addr, data, false))
  {
    ::AfxMessageBox("Set RF register failed!");
  }
}

void CConfigureRfIc::UpdateUiSetting()
{
  CString txt;
	U32 reg[Setting::RFIC_W_REG_SIZE];
  for(int i = 0; i < Setting::RFIC_W_REG_SIZE; ++i)
  {
    GetDlgItem(editWCtrlList[i])->GetWindowText(txt);
	  reg[i] = ConvertCharToU32(txt);
    g_setting.configRfIcReg[i] = reg[i];
  }
  g_setting.Save();
}

void CConfigureRfIc::OnBnClickedWrite()
{	
  CString txt;
	//U32 reg[Setting::RFIC_W_REG_SIZE];
  UpdateUiSetting();

  U08 addr;
  bool allSuccessful = true;
  for(int i = 0; i < Setting::RFIC_W_REG_SIZE; ++i)
  {
    GetDlgItem(staticWCtrlList[i])->GetWindowText(txt);
    txt.Replace("  0x", "");
    txt.Replace("R", "");
    addr = atoi(txt);
    if(!SetRfRegister(addr, g_setting.configRfIcReg[i], true))
    {
      allSuccessful = false;
      break;
    }
  }
  ::AfxMessageBox((allSuccessful) ? "Set RF IC registers successfully" : "Set RF IC registers failed!");

  return;
}

void CConfigureRfIc::OnBnClickedRead()
{
  BinaryData ackCmd;
  CString txt;
  for(int i = 0; i < Setting::RFIC_W_REG_SIZE; ++i)
  {
      if(!GetDlgItem(editWCtrlList[i])->IsWindowVisible())
      {
        return;
      }
      GetDlgItem(staticWCtrlList[i])->GetWindowText(txt);
      txt.Replace("  0x", "");
      txt.Replace("R", "");
      U08 id = atoi(txt);

      CGPSDlg::CmdErrorCode err = CGPSDlg::gpsDlg->QueryOneRfRegister(CGPSDlg::Return, &ackCmd, id);
      if(err != CGPSDlg::Ack)
      {
        ::AfxMessageBox("Get RF IC registers failed!");
        return;
      }

      txt.Format("%06X", ConvertLeonU32(ackCmd.Ptr(6)) & 0xFFFFFF);
      GetDlgItem(editWCtrlList[i])->SetWindowText(txt);
  }

  for(int i = 0; i < Setting::RFIC_R_REG_SIZE; ++i)
  {
      if(!GetDlgItem(editRCtrlList[i])->IsWindowVisible())
      {
        return;
      }
      GetDlgItem(staticRCtrlList[i])->GetWindowText(txt);
      txt.Replace("  0x", "");
      txt.Replace("R", "");
      U08 id = atoi(txt);

      CGPSDlg::CmdErrorCode err = CGPSDlg::gpsDlg->QueryOneRfRegister(CGPSDlg::Return, &ackCmd, id);
      if(err != CGPSDlg::Ack)
      {
        ::AfxMessageBox("Get RF IC registers failed!");
        return;
      }

      txt.Format("%06X", ConvertLeonU32(ackCmd.Ptr(6)) & 0xFFFFFF);
      GetDlgItem(editRCtrlList[i])->SetWindowText(txt);
  }
  ::AfxMessageBox("Get RF IC registers successfully.");

}

void CConfigureRfIc::OnClose()
{
  UpdateUiSetting();
	CDialog::OnClose();
}

void CConfigureRfIc::OnBnClickedClose()
{
  UpdateUiSetting();
  CDialog::OnOK();
}

void CConfigureRfIc::OnBnClickedExport()
{
	CTime t = CTime::GetCurrentTime();
	CString fileName;
	fileName.Format("rf_reg%02d%02d%02d_%02d%02d%02d.txt", 
    t.GetYear(), t.GetMonth(), t.GetDay(),
		t.GetHour(), t.GetMinute(), t.GetSecond());

	CFileDialog dlgFile(FALSE, _T("txt"), fileName, 
    OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("ALL Files (*.*)|*.*||"), this);

	dlgFile.GetOFN().lpstrFile = fileName.GetBuffer(MyMaxPath);
	dlgFile.GetOFN().nMaxFile = MyMaxPath;
	INT_PTR nResult = dlgFile.DoModal();
	fileName.ReleaseBuffer();

	if(nResult != IDOK)
	{
		return;
	}

	CFile f;
	f.Open(fileName, CFile::modeReadWrite | CFile::modeCreate | CFile::shareDenyWrite);
  CString name, reg, txt;
  //const BOOL* showTable = (type == 0) ? type0_show : type1_show;

  for(int i = 0; i < Setting::RFIC_W_REG_SIZE; ++i)
  {
      if(!GetDlgItem(editWCtrlList[i])->IsWindowVisible())
      {
        continue;
      }
      GetDlgItem(editWCtrlList[i])->GetWindowText(reg);
      GetDlgItem(staticWCtrlList[i])->GetWindowText(name);
      name.Replace("R0", "");
      name.Replace("R", "");
      name.Replace("  0x", ",");
      txt.Format("%s%06X\r\n", name, ConvertCharToU32(reg));
  	  f.Write(txt, txt.GetLength());
  }

  for(int i = 0; i < Setting::RFIC_R_REG_SIZE; ++i)
  {
      if(!GetDlgItem(editRCtrlList[i])->IsWindowVisible())
      {
        continue;
      }
      GetDlgItem(editRCtrlList[i])->GetWindowText(reg);
      GetDlgItem(staticRCtrlList[i])->GetWindowText(name);
      name.Replace("R0", "");
      name.Replace("R", "");
      name.Replace("  0x", ",");
      txt.Format("%s%06X\r\n", name, ConvertCharToU32(reg));
  	  f.Write(txt, txt.GetLength());
  }
	f.Close();
}

void CConfigureRfIc::OnCbnSelChangeType()
{

  const char* type_w_name[] = 
    { "R12  0x", "R13  0x", "R00  0x", "R01  0x", "R02  0x", "R03  0x",
      "R04  0x", "R05  0x", "R06  0x", "R07  0x", "R08  0x", "R09  0x",
      "R10  0x", "R11  0x", "R14  0x", "R16  0x", "R17  0x", "R18  0x",
      "R19  0x", "R20  0x", "R21  0x", "R22  0x", "R15  0x" };

  int i1 = 0;
  int i1Size = sizeof(type_w_name) / sizeof(type_w_name[0]);
  const char** typeName = type_w_name;

  for(; i1 < i1Size; ++i1)
  {
    GetDlgItem(staticWCtrlList[i1])->SetWindowText(typeName[i1]);
    GetDlgItem(staticWCtrlList[i1])->ShowWindow(SW_SHOW);
    GetDlgItem(editWCtrlList[i1])->ShowWindow(SW_SHOW);
  }
  
  for(int i2 = i1; i2 < Setting::RFIC_W_REG_SIZE; ++i2)
  {
    GetDlgItem(staticWCtrlList[i2])->ShowWindow(SW_HIDE);
    GetDlgItem(editWCtrlList[i2])->ShowWindow(SW_HIDE);
  }

  CString txt;
  BOOL hasValue = FALSE;
  //SetWindowText(m_title);

  for(int i = 0; i < Setting::RFIC_W_REG_SIZE; ++i)
  {
      txt.Format("%06X", g_setting.configRfIcReg[i]);
      GetDlgItem(editWCtrlList[i])->SetWindowText(txt);
  }
  //SetWindowText(m_title + " (Get successfully)");
  hasValue = TRUE;

  if(hasValue == FALSE)
  {
    for(int i = 0; i < Setting::RFIC_W_REG_SIZE; ++i)
    {
      txt.Format("%06X", g_setting.configRfIcReg[i]);
      GetDlgItem(editWCtrlList[i])->SetWindowText(txt);
    }
  }

  const char* type_r_name[] = 
    { "R61  0x", "R62  0x", "R63  0x" };

  i1 = 0;
  i1Size = sizeof(type_r_name) / sizeof(type_r_name[0]);
  typeName = type_r_name;
  GetDlgItem(IDC_READ)->EnableWindow(TRUE);
  GetDlgItem(IDC_EXPORT)->EnableWindow(TRUE);
    
  for(; i1 < i1Size; ++i1)
  {
    GetDlgItem(staticRCtrlList[i1])->SetWindowText(typeName[i1]);
    GetDlgItem(staticRCtrlList[i1])->ShowWindow(SW_SHOW);
    GetDlgItem(editRCtrlList[i1])->ShowWindow(SW_SHOW);
  }
  
  for(int i2 = i1; i2 < Setting::RFIC_R_REG_SIZE; ++i2)
  {
    GetDlgItem(staticRCtrlList[i2])->ShowWindow(SW_HIDE);
    GetDlgItem(editRCtrlList[i2])->ShowWindow(SW_HIDE);
  }
}

bool CConfigureRfIc::SetRfRegister(U08 addr, U32 data, bool silence)
{
	CWaitCursor wait;

	BinaryData cmd(6);
  *cmd.GetBuffer(0) = 0x64;
  *cmd.GetBuffer(1) = 0x6F;
	*cmd.GetBuffer(2) = addr;
	*cmd.GetBuffer(3) = data >> 16 & 0xFF;
	*cmd.GetBuffer(4) = data >> 8 & 0xFF;
	*cmd.GetBuffer(5) = data & 0xFF;

  BinaryCommand configCmd;
  configCmd.SetData(cmd);

  CString txt;
  txt.Format("Set RF register(%02d) successfully", addr);
  return CGPSDlg::gpsDlg->ExecuteConfigureCommand(configCmd.GetBuffer(), configCmd.Size(), (silence) ? NULL : txt, false);
}
