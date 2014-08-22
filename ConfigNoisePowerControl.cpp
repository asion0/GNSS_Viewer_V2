// ConfigSBAS.cpp : 實作檔
//

#include "stdafx.h"
#include "Resource.h"
#include "ConfigNoisePowerControl.h"


// CConfigNoisePowerControl 對話方塊

IMPLEMENT_DYNAMIC(CConfigNoisePowerControl, CDialog)

CConfigNoisePowerControl::CConfigNoisePowerControl(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_CONFIG_NOISE_PWR_CTRL, pParent)
{
	m_nMode = 0;
	m_nAttribute = 0;
}

CConfigNoisePowerControl::~CConfigNoisePowerControl()
{
}

void CConfigNoisePowerControl::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CConfigNoisePowerControl, CDialog)
	ON_BN_CLICKED(IDOK, &CConfigNoisePowerControl::OnBnClickedOk)
END_MESSAGE_MAP()


// CConfigNoisePowerControl 訊息處理常式
void CConfigNoisePowerControl::OnBnClickedOk()
{
	m_nMode = ((CComboBox*)GetDlgItem(IDC_MODE))->GetCurSel();
	m_nAttribute = ((CComboBox*)GetDlgItem(IDC_BINARY_ATTRI))->GetCurSel();

	OnOK();
}

BOOL CConfigNoisePowerControl::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此加入額外的初始化
	((CComboBox*)GetDlgItem(IDC_MODE))->SetCurSel(0);;
	((CComboBox*)GetDlgItem(IDC_BINARY_ATTRI))->SetCurSel(0);;

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX 屬性頁應傳回 FALSE
}
