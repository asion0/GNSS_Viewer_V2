// IoTester.cpp
//

#include "stdafx.h"
#include "DcdcRegister.h"
#include "GPSDlg.h"

#define TIMER_ID    0

// CDcdcRegister

IMPLEMENT_DYNAMIC(CDcdcRegister, CDialog)

CDcdcRegister::CDcdcRegister(CWnd* pParent /*=NULL*/)
	: CDialog(CDcdcRegister::IDD, pParent)
{
  autoUpdateMode = FALSE;
}

CDcdcRegister::~CDcdcRegister()
{
}

void CDcdcRegister::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CDcdcRegister, CDialog)
  ON_CONTROL_RANGE(BN_CLICKED, IDC_CHECK_M1_00, IDC_CHECK_M1_05, OnBnClickedCheckM1)
  ON_BN_CLICKED(IDC_CONFIG_REG_BTN, &CDcdcRegister::OnBnClickedConfigRegBtn)
  ON_BN_CLICKED(IDC_GET_AGAIN_BTN, &CDcdcRegister::OnBnClickedGetAgainBtn)
END_MESSAGE_MAP()


// CDcdcRegister
BOOL CDcdcRegister::OnInitDialog()
{
	CDialog::OnInitDialog();

  if(!GetAllRegister())
  {
    EndDialog(IDCANCEL);
    return TRUE;
  }

  if(!GetAllRegister())
  {
    EndDialog(IDCANCEL);
    return TRUE;
  }

	return TRUE;  // return TRUE unless you set the focus to a control
}

BOOL CDcdcRegister::GetAllRegister()
{
  BOOL ret = TRUE;
  ret = SetOneRegister(0x20014C2C, IDC_STATIC_4C2C, IDC_CHECK_M1_00, IDC_EDIT_4C2C);
  if(!ret) return ret;
  
  return TRUE;
}

BOOL CDcdcRegister::SetOneRegister(U32 addr, U32 showId, U32 checkId, U32 editId)
{
  CString strText;
  U32 data;
  CGPSDlg::CmdErrorCode ack = CGPSDlg::Timeout;

  CGPSDlg::gpsDlg->m_regAddress = addr;
  ack = CGPSDlg::gpsDlg->QueryRegister(CGPSDlg::Return, &data);
  if(CGPSDlg::Ack == ack)
  {
    strText.Format("0x%08X", data);
    GetDlgItem(showId)->SetWindowText(strText);
    strText.Format("%08X", data);
    GetDlgItem(editId)->SetWindowText(strText);

    if(checkId)
    {
      for(int i = 0; i < 32; ++i)
      {
        if((CButton*)GetDlgItem(checkId + i) != NULL)
        {
          ((CButton*)GetDlgItem(checkId + i))->SetCheck(((data >> i) & 0x01) ? TRUE : FALSE);
        }
      }
    }
    return TRUE;
  }
  return FALSE;
}

void CDcdcRegister::OnBnClickedCheckM1(UINT nID)  
{
  UpdateOneRegister(IDC_CHECK_M1_00, IDC_EDIT_4C2C);
}


void CDcdcRegister::UpdateOneRegister(U32 checkId, U32 editId)
{
  U32 data = 0;
  for(int i = 0; i < 32; ++i)
  {
    int b = ((CButton*)GetDlgItem(checkId + i))->GetCheck();
    data |= (b << i);
  }
  CString strText;
  strText.Format("%08X", data);
  GetDlgItem(editId)->SetWindowText(strText);
}

void CDcdcRegister::OnBnClickedConfigRegBtn()
{
  ConfigureOneRegister(0x20014C2C, IDC_STATIC_4C2C, IDC_EDIT_4C2C, IDC_CHECK_M1_00);
}

void CDcdcRegister::ConfigureOneRegister(U32 addr, U32 showId, U32 editId, U32 checkId)
{
  CString strText;
  U32 v1, v2;

  GetDlgItem(showId)->GetWindowText(strText);
  v1 = ConvertCharToU32(strText);

  GetDlgItem(editId)->GetWindowText(strText);
  v2 = ConvertCharToU32(strText);
  
  if(v1 == v2)
    return;

  if(CGPSDlg::gpsDlg->DoCConfigRegisterDirect(addr, v2))
  {
    SetOneRegister(addr, showId, checkId, editId);
  }
  return;
}

void CDcdcRegister::OnBnClickedGetAgainBtn()
{
  GetAllRegister();
}

void CDcdcRegister::OnBnClickedConfigReg2Btn()
{
  ConfigureOneRegister(0x20001008, IDC_STATIC_1008, IDC_EDIT_1008, IDC_CHECK_MV_00);
}

void CDcdcRegister::OnBnClickedAutoUpdate()
{
  if(SwitchAutoUpdate())
  {
    SetTimer(TIMER_ID, 1000, NULL);
  }
  else
  {
    KillTimer(TIMER_ID);
  }
}

BOOL CDcdcRegister::SwitchAutoUpdate()
{
  if(autoUpdateMode)
  {
    GetDlgItem(IDC_AUTO_UPDATE)->SetWindowText("Auto Update");
    GetDlgItem(IDC_CONFIG_REG2_BTN)->EnableWindow(TRUE);
    autoUpdateMode = FALSE;
  }
  else
  {
    GetDlgItem(IDC_AUTO_UPDATE)->SetWindowText("Stop Update");
    GetDlgItem(IDC_CONFIG_REG2_BTN)->EnableWindow(FALSE);
    autoUpdateMode = TRUE;
  }
  return autoUpdateMode;
}

void CDcdcRegister::OnTimer(UINT_PTR nIDEvent)
{
    if(TIMER_ID == nIDEvent)
    {
      SetOneRegister(0x20001008, IDC_STATIC_1008, IDC_CHECK_MV_00, IDC_EDIT_1008);
    }
 
    CDialog::OnTimer(nIDEvent);
}
