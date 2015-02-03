// Config1ppsFrequenceOutput.cpp : 實作檔
//

#include "stdafx.h"
#include "Resource.h"
#include "Config1ppsFrequenceOutput.h"


// CConfig1ppsFrequenceOutput 對話方塊

IMPLEMENT_DYNAMIC(CConfig1ppsFrequenceOutput, CDialog)

CConfig1ppsFrequenceOutput::CConfig1ppsFrequenceOutput(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_CONFIG_1PPS_FREQ_OUTPUT, pParent)
{

	EnableAutomation();

}

CConfig1ppsFrequenceOutput::~CConfig1ppsFrequenceOutput()
{
}

void CConfig1ppsFrequenceOutput::OnFinalRelease()
{
	// 當釋放 Automation 物件最後的參考時，
	// 會呼叫 OnFinalRelease。基底類別會自動
	// 刪除物件。呼叫基底類別前，請先加入您物件所需的額外清除 (Cleanup)
	// 程式碼。

	CDialog::OnFinalRelease();
}

void CConfig1ppsFrequenceOutput::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CConfig1ppsFrequenceOutput, CDialog)
	ON_BN_CLICKED(IDOK, &CConfig1ppsFrequenceOutput::OnBnClickedOk)
END_MESSAGE_MAP()

BEGIN_DISPATCH_MAP(CConfig1ppsFrequenceOutput, CDialog)
END_DISPATCH_MAP()

// 注意: 我們加入了對 IID_IConfig1ppsFrequenceOutput 的支援
// 以便從 VBA 支援型別安全繫結。此 IID 必須與 .IDL 檔中，
// 附加至分配介面 (Dispinterface) 的 GUID 相符。

// {D1A372EB-FC2B-4847-AE3B-31031DB44A83}
static const IID IID_IConfig1ppsFrequenceOutput =
{ 0xD1A372EB, 0xFC2B, 0x4847, { 0xAE, 0x3B, 0x31, 0x3, 0x1D, 0xB4, 0x4A, 0x83 } };

BEGIN_INTERFACE_MAP(CConfig1ppsFrequenceOutput, CDialog)
	INTERFACE_PART(CConfig1ppsFrequenceOutput, IID_IConfig1ppsFrequenceOutput, Dispatch)
END_INTERFACE_MAP()


// CConfig1ppsFrequenceOutput 訊息處理常式

BOOL CConfig1ppsFrequenceOutput::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此加入額外的初始化
	GetDlgItem(IDC_OUTPUT)->SetWindowText("10000000");
	((CComboBox*)GetDlgItem(IDC_ATTR))->SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX 屬性頁應傳回 FALSE
}

void CConfig1ppsFrequenceOutput::OnBnClickedOk()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	CString strValue;
	GetDlgItem(IDC_OUTPUT)->GetWindowText(strValue);
	int n = atoi(strValue);
	if(n < 1 || n > 19200000)
	{
		::AfxMessageBox("Frequency must be between 1 and 19200000.");
		return;
	}
	freqOutput = n;
	freqOutputAttr = ((CComboBox*)GetDlgItem(IDC_ATTR))->GetCurSel();

	OnOK();
}
