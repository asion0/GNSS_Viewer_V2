// IoTester.cpp
//

#include "stdafx.h"
#include "IoTester.h"
#include "GPSDlg.h"

#define TIMER_ID    0

// CIoTester

IMPLEMENT_DYNAMIC(CIoTester, CDialog)

CIoTester::CIoTester(CWnd* pParent /*=NULL*/)
	: CDialog(CIoTester::IDD, pParent)
{
  autoUpdateMode = FALSE;
}

CIoTester::~CIoTester()
{
}

void CIoTester::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CIoTester, CDialog)
  ON_CONTROL_RANGE(BN_CLICKED, IDC_CHECK_M1_00, IDC_CHECK_M1_31, OnBnClickedCheckM1)
  ON_CONTROL_RANGE(BN_CLICKED, IDC_CHECK_M2_00, IDC_CHECK_M2_31, OnBnClickedCheckM2)
  ON_CONTROL_RANGE(BN_CLICKED, IDC_CHECK_M3_00, IDC_CHECK_M3_31, OnBnClickedCheckM3)
  ON_CONTROL_RANGE(BN_CLICKED, IDC_CHECK_M4_00, IDC_CHECK_M4_31, OnBnClickedCheckM4)
  ON_CONTROL_RANGE(BN_CLICKED, IDC_CHECK_M5_00, IDC_CHECK_M5_31, OnBnClickedCheckM5)
  ON_CONTROL_RANGE(BN_CLICKED, IDC_CHECK_M6_00, IDC_CHECK_M6_31, OnBnClickedCheckM6)
  ON_CONTROL_RANGE(BN_CLICKED, IDC_CHECK_M7_00, IDC_CHECK_M7_31, OnBnClickedCheckM7)
  ON_CONTROL_RANGE(BN_CLICKED, IDC_CHECK_MC_00, IDC_CHECK_MC_31, OnBnClickedCheckMC)
  ON_CONTROL_RANGE(BN_CLICKED, IDC_CHECK_MV_00, IDC_CHECK_MV_31, OnBnClickedCheckMV)
  ON_BN_CLICKED(IDC_CONFIG_REG_BTN, &CIoTester::OnBnClickedConfigRegBtn)

  ON_BN_CLICKED(IDC_GET_AGAIN_BTN, &CIoTester::OnBnClickedGetAgainBtn)
  ON_BN_CLICKED(IDC_CONFIG_REG2_BTN, &CIoTester::OnBnClickedConfigReg2Btn)
  ON_BN_CLICKED(IDC_AUTO_UPDATE, &CIoTester::OnBnClickedAutoUpdate)

  ON_WM_TIMER()
END_MESSAGE_MAP()


// CIoTester
BOOL CIoTester::OnInitDialog()
{
	CDialog::OnInitDialog();

  if(!GetAllRegister())
  {
    EndDialog(IDCANCEL);
    return TRUE;
  }

  if(!SetOneRegister(0x20001008, IDC_STATIC_1008, IDC_CHECK_MV_00, IDC_EDIT_1008))
  {
    EndDialog(IDCANCEL);
    return TRUE;
  }

	return TRUE;  // return TRUE unless you set the focus to a control
}

BOOL CIoTester::GetAllRegister()
{
  BOOL ret = TRUE;
  ret = SetOneRegister(0x2000F078, IDC_STATIC_F078, IDC_CHECK_M1_00, IDC_EDIT_F078);
  if(!ret) return ret;
  ret = SetOneRegister(0x2000F014, IDC_STATIC_F014, IDC_CHECK_M2_00, IDC_EDIT_F014);
  if(!ret) return ret;
  ret = SetOneRegister(0x2000F004, IDC_STATIC_F004, IDC_CHECK_M3_00, IDC_EDIT_F004);
  if(!ret) return ret;
  ret = SetOneRegister(0x2000F00C, IDC_STATIC_F00C, IDC_CHECK_M4_00, IDC_EDIT_F00C);
  if(!ret) return ret;
  ret = SetOneRegister(0x2000F0BC, IDC_STATIC_F0BC, IDC_CHECK_M5_00, IDC_EDIT_F0BC);
  if(!ret) return ret;
  ret = SetOneRegister(0x2000F02C, IDC_STATIC_F02C, IDC_CHECK_M6_00, IDC_EDIT_F02C);
  if(!ret) return ret;
  ret = SetOneRegister(0x2000F034, IDC_STATIC_F034, IDC_CHECK_M7_00, IDC_EDIT_F034);
  if(!ret) return ret;

  ret = SetOneRegister(0x2000F090, IDC_STATIC_F090, 0, IDC_EDIT_F090);
  if(!ret) return ret;
  ret = SetOneRegister(0x2000F094, IDC_STATIC_F094, 0, IDC_EDIT_F094);
  if(!ret) return ret;
  ret = SetOneRegister(0x2000F098, IDC_STATIC_F098, 0, IDC_EDIT_F098);
  if(!ret) return ret;
  ret = SetOneRegister(0x2000F09C, IDC_STATIC_F09C, 0, IDC_EDIT_F09C);
  if(!ret) return ret;

  ret = SetOneRegister(0x2000F0A0, IDC_STATIC_F0A0, 0, IDC_EDIT_F0A0);
  if(!ret) return ret;
  ret = SetOneRegister(0x2000F0A4, IDC_STATIC_F0A4, 0, IDC_EDIT_F0A4);
  if(!ret) return ret;
  ret = SetOneRegister(0x2000F0A8, IDC_STATIC_F0A8, 0, IDC_EDIT_F0A8);
  if(!ret) return ret;
  ret = SetOneRegister(0x2000F0AC, IDC_STATIC_F0AC, 0, IDC_EDIT_F0AC);
  if(!ret) return ret;

  ret = SetOneRegister(0x2000F028, IDC_STATIC_F028, 0, IDC_EDIT_F028);
  if(!ret) return ret;
  ret = SetOneRegister(0x2000F030, IDC_STATIC_F030, 0, IDC_EDIT_F030);
  if(!ret) return ret;

  ret = SetOneRegister(0x2000100C, IDC_STATIC_100C, IDC_CHECK_MC_00, IDC_EDIT_100C);
  if(!ret) return ret;
  
  return TRUE;
}

BOOL CIoTester::SetOneRegister(U32 addr, U32 showId, U32 checkId, U32 editId)
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
        ((CButton*)GetDlgItem(checkId + i))->SetCheck(((data >> i) & 0x01) ? TRUE : FALSE);
      }
    }
    return TRUE;
  }
  return FALSE;
}

void CIoTester::OnBnClickedCheckM1(UINT nID)  
{
  UpdateOneRegister(IDC_CHECK_M1_00, IDC_EDIT_F078);
}

void CIoTester::OnBnClickedCheckM2(UINT nID)  
{
  UpdateOneRegister(IDC_CHECK_M2_00, IDC_EDIT_F014);
}

void CIoTester::OnBnClickedCheckM3(UINT nID)  
{
  UpdateOneRegister(IDC_CHECK_M3_00, IDC_EDIT_F004);
}

void CIoTester::OnBnClickedCheckM4(UINT nID)  
{
  UpdateOneRegister(IDC_CHECK_M4_00, IDC_EDIT_F00C);
}

void CIoTester::OnBnClickedCheckM5(UINT nID)  
{
  UpdateOneRegister(IDC_CHECK_M5_00, IDC_EDIT_F0BC);
}

void CIoTester::OnBnClickedCheckM6(UINT nID)  
{
  UpdateOneRegister(IDC_CHECK_M6_00, IDC_EDIT_F02C);
}

void CIoTester::OnBnClickedCheckM7(UINT nID)  
{
  UpdateOneRegister(IDC_CHECK_M7_00, IDC_EDIT_F034);
}

void CIoTester::OnBnClickedCheckMC(UINT nID)  
{
  UpdateOneRegister(IDC_CHECK_MC_00, IDC_EDIT_100C);
}

void CIoTester::OnBnClickedCheckMV(UINT nID)  
{
  UpdateOneRegister(IDC_CHECK_MV_00, IDC_EDIT_1008);
}

void CIoTester::UpdateOneRegister(U32 checkId, U32 editId)
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
void CIoTester::OnBnClickedConfigRegBtn()
{
  ConfigureOneRegister(0x2000F078, IDC_STATIC_F078, IDC_EDIT_F078, IDC_CHECK_M1_00);
  ConfigureOneRegister(0x2000F014, IDC_STATIC_F014, IDC_EDIT_F014, IDC_CHECK_M2_00);
  ConfigureOneRegister(0x2000F004, IDC_STATIC_F004, IDC_EDIT_F004, IDC_CHECK_M3_00);
  ConfigureOneRegister(0x2000F00C, IDC_STATIC_F00C, IDC_EDIT_F00C, IDC_CHECK_M4_00);
  ConfigureOneRegister(0x2000F0BC, IDC_STATIC_F0BC, IDC_EDIT_F0BC, IDC_CHECK_M5_00);
  ConfigureOneRegister(0x2000F02C, IDC_STATIC_F02C, IDC_EDIT_F02C, IDC_CHECK_M6_00);
  ConfigureOneRegister(0x2000F034, IDC_STATIC_F034, IDC_EDIT_F034, IDC_CHECK_M7_00);
  ConfigureOneRegister(0x2000100C, IDC_STATIC_100C, IDC_EDIT_100C, IDC_CHECK_MC_00);

  ConfigureOneRegister(0x2000F090, IDC_STATIC_F090, IDC_EDIT_F090, 0);
  ConfigureOneRegister(0x2000F094, IDC_STATIC_F094, IDC_EDIT_F094, 0);
  ConfigureOneRegister(0x2000F098, IDC_STATIC_F098, IDC_EDIT_F098, 0);
  ConfigureOneRegister(0x2000F09C, IDC_STATIC_F09C, IDC_EDIT_F09C, 0);

  ConfigureOneRegister(0x2000F0A0, IDC_STATIC_F0A0, IDC_EDIT_F0A0, 0);
  ConfigureOneRegister(0x2000F0A4, IDC_STATIC_F0A4, IDC_EDIT_F0A4, 0);
  ConfigureOneRegister(0x2000F0A8, IDC_STATIC_F0A8, IDC_EDIT_F0A8, 0);
  ConfigureOneRegister(0x2000F0AC, IDC_STATIC_F0AC, IDC_EDIT_F0AC, 0);
  
  ConfigureOneRegister(0x2000F028, IDC_STATIC_F028, IDC_EDIT_F028, 0);
  ConfigureOneRegister(0x2000F030, IDC_STATIC_F030, IDC_EDIT_F030, 0);
}

void CIoTester::ConfigureOneRegister(U32 addr, U32 showId, U32 editId, U32 checkId)
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

void CIoTester::OnBnClickedGetAgainBtn()
{
  GetAllRegister();
}

void CIoTester::OnBnClickedConfigReg2Btn()
{
  ConfigureOneRegister(0x20001008, IDC_STATIC_1008, IDC_EDIT_1008, IDC_CHECK_MV_00);
}

void CIoTester::OnBnClickedAutoUpdate()
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

BOOL CIoTester::SwitchAutoUpdate()
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

void CIoTester::OnTimer(UINT_PTR nIDEvent)
{
    if(TIMER_ID == nIDEvent)
    {
      SetOneRegister(0x20001008, IDC_STATIC_1008, IDC_CHECK_MV_00, IDC_EDIT_1008);
    }
 
    CDialog::OnTimer(nIDEvent);
}
