// CConfig1ppsPulseWidthDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "Resource.h"
#include "Config1ppsPulseWidthDlg.h"


// CConfig1ppsPulseWidthDlg 對話方塊

IMPLEMENT_DYNAMIC(CConfig1ppsPulseWidthDlg, CDialog)

CConfig1ppsPulseWidthDlg::CConfig1ppsPulseWidthDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_CONFIG_1PPS_PULSE_WIDTH, pParent)
{

	m_nPulseWidth = 0;
	m_nAttribute = 0;
}

CConfig1ppsPulseWidthDlg::~CConfig1ppsPulseWidthDlg()
{
}

void CConfig1ppsPulseWidthDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CConfig1ppsPulseWidthDlg, CDialog)
	ON_BN_CLICKED(IDOK, &CConfig1ppsPulseWidthDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CConfig1ppsPulseWidthDlg 訊息處理常式
void CConfig1ppsPulseWidthDlg::OnBnClickedOk()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	CString txt;

	GetDlgItem(IDC_PULSE_WIDTH)->GetWindowText(txt);
	m_nPulseWidth = atoi(txt);
	if(m_nPulseWidth<1 || m_nPulseWidth>100000)
	{
		AfxMessageBox("Number of Pulse Width must be between 1 and 100000.");
		return;
	}
	m_nAttribute = ((CComboBox*)GetDlgItem(IDC_BINARY_ATTRI))->GetCurSel();;

	OnOK();
}

BOOL CConfig1ppsPulseWidthDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此加入額外的初始化
	GetDlgItem(IDC_PULSE_WIDTH)->SetWindowText("1");
	((CComboBox*)GetDlgItem(IDC_BINARY_ATTRI))->SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX 屬性頁應傳回 FALSE
}
