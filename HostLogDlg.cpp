// HostLogDlg.cpp
//

#include "stdafx.h"
#include "HostLogDlg.h"
#include "Resource.h"
#include "GPSDlg.h"
#include "Serial.h"


// HostLogDlg 

IMPLEMENT_DYNAMIC(HostLogDlg, CDialog)

HostLogDlg::HostLogDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_HOSTLOG, pParent)
{

}

HostLogDlg::~HostLogDlg()
{
}

void HostLogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(HostLogDlg, CDialog)
	//ON_EN_CHANGE(IDC_CONTEXT, &HostLogDlg::OnEnChangeInput)
	ON_BN_CLICKED(IDC_BROWSE, &HostLogDlg::OnBnClickedBrowse)
	ON_BN_CLICKED(IDC_START, &HostLogDlg::OnBnClickedStart)
END_MESSAGE_MAP()


// HostLogDlg 

BOOL HostLogDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
  GetDlgItem(IDC_START)->EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void HostLogDlg::OnOK()
{
	CDialog::OnOK();
}

void HostLogDlg::OnBnClickedBrowse()
{
	CTime t = CTime::GetCurrentTime();
	CString fileName;
	fileName.Format("Hostlog%02d-%02d-%02d_%02d%02d%02d.dat", t.GetYear(), t.GetMonth(), t.GetDay(),
		t.GetHour(), t.GetMinute(), t.GetSecond());

	CFileDialog dlgFile(FALSE, _T("txt"), fileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("ALL Files (*.*)|*.*||"), this);

	dlgFile.GetOFN().lpstrFile = fileName.GetBuffer(MyMaxPath);
	dlgFile.GetOFN().nMaxFile = MyMaxPath;
	INT_PTR nResult = dlgFile.DoModal();
	fileName.ReleaseBuffer();

	if(nResult != IDOK)
	{
		return;
	}

  GetDlgItem(IDC_PATH)->SetWindowText(fileName);
  GetDlgItem(IDC_START)->EnableWindow(TRUE);

}

void HostLogDlg::OnBnClickedStart()
{

	CString strInput;
	GetDlgItem(IDC_CONTEXT)->GetWindowText(strInput);

	BinaryData binData;
	if(!Utility::ConvertHexToBinary(strInput, binData))
	{
		::AfxMessageBox("Invalidate Format!");
		return;
	}

	BinaryCommand cmd(binData);
	U08* pCmd = cmd.GetBuffer();
	int inSize = cmd.Size();
	CGPSDlg::gpsDlg->m_serial->SendData(pCmd, inSize);
}
