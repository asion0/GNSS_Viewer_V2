// ConfigureIir.cpp
//

#include "stdafx.h"
#include "ConfigureIir.h"
#include "GPSDlg.h"

// CConfigureIir
IMPLEMENT_DYNAMIC(CConfigureIir, CDialog)

CConfigureIir::CConfigureIir(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_CFG_IIR, pParent)
{

}

BEGIN_MESSAGE_MAP(CConfigureIir, CDialog)
	ON_BN_CLICKED(IDC_WRITE, &CConfigureIir::OnBnClickedWrite)
	ON_BN_CLICKED(IDC_CLOSE, &CConfigureIir::OnBnClickedClose)
END_MESSAGE_MAP()

// CConfigureIir
UINT CConfigureIir::staticWCtrlList[] = 
  { IDC_T_W_REG0, IDC_T_W_REG1, IDC_T_W_REG2, IDC_T_W_REG3, IDC_T_W_REG4, IDC_T_W_REG5,
    IDC_T_W_REG6, IDC_T_W_REG7, IDC_T_W_REG8, IDC_T_W_REG9, IDC_T_W_REG10, IDC_T_W_REG11,
    IDC_T_W_REG12, IDC_T_W_REG13, IDC_T_W_REG14, IDC_T_W_REG15, IDC_T_W_REG16 };

UINT CConfigureIir::editWCtrlList[] = 
  { IDC_W_REG0, IDC_W_REG1, IDC_W_REG2, IDC_W_REG3, IDC_W_REG4, IDC_W_REG5,
    IDC_W_REG6, IDC_W_REG7, IDC_W_REG8, IDC_W_REG9, IDC_W_REG10, IDC_W_REG11,
    IDC_W_REG12, IDC_W_REG13, IDC_W_REG14, IDC_W_REG15, IDC_W_REG16 };


BOOL CConfigureIir::OnInitDialog()
{
	CDialog::OnInitDialog();

  CString txt;
  U32 data;
  for(int i = 0; i < Dim(staticWCtrlList); ++i)
  {
	  ((CEdit*)GetDlgItem(editWCtrlList[i]))->SetLimitText(8);
    GetDlgItem(staticWCtrlList[i])->GetWindowText(txt);
    CGPSDlg::gpsDlg->m_regAddress = ConvertCharToU32(txt.Right(8));
    CGPSDlg::CmdErrorCode ack = CGPSDlg::Timeout;
    ack = CGPSDlg::gpsDlg->QueryRegister(CGPSDlg::Return, &data);
    if(CGPSDlg::Ack == ack)
    {
      txt.Format("%08X", data);
      GetDlgItem(editWCtrlList[i])->SetWindowText(txt);
    }
    else
    {
      GetDlgItem(editWCtrlList[i])->SetWindowText("");
    }
  }
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CConfigureIir::OnBnClickedWrite()
{	
  CString txt;
  CString errString;
  U32 addr, data;
  for(int i = 0; i < Dim(staticWCtrlList); ++i)
  {
    GetDlgItem(staticWCtrlList[i])->GetWindowText(txt);
    addr = ConvertCharToU32(txt.Right(8));
    GetDlgItem(editWCtrlList[i])->GetWindowText(txt);
    data = ConvertCharToU32(txt);

    if(!CGPSDlg::gpsDlg->DoCConfigRegisterDirect(addr, data))
    {
      GetDlgItem(staticWCtrlList[i])->GetWindowText(txt);
      errString += txt;
      errString += ", ";
    }
  }
  if(errString.GetLength() == 0)
  {
    ::AfxMessageBox("Configure IIR successfully");
  }
  else
  {
    txt.Format("Register: %s configure failed!", errString.Right(errString.GetLength() - 2));
    ::AfxMessageBox(txt);
  }
  return;
}

void CConfigureIir::OnBnClickedClose()
{
  CDialog::OnOK();
}


