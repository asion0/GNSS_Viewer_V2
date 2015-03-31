// GpsdoDownload.cpp : 實作檔
//

#include "stdafx.h"
#include "GpsdoDownload.h"
#include "resource.h"


// CGpsdoDownload 對話方塊

IMPLEMENT_DYNAMIC(CGpsdoDownload, CDialog)

CGpsdoDownload::CGpsdoDownload(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_GPSDO_DOWNLOAD, pParent)
{

}

CGpsdoDownload::~CGpsdoDownload()
{
}

void CGpsdoDownload::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CGpsdoDownload, CDialog)
	ON_BN_CLICKED(IDC_BROWSE_MASTER, &CGpsdoDownload::OnBnClickedBrowseMaster)
	ON_BN_CLICKED(IDC_BROWSE_SLAVE, &CGpsdoDownload::OnBnClickedBrowseSlave)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_DOWNLOAD, &CGpsdoDownload::OnBnClickedDownload)
END_MESSAGE_MAP()


// CGpsdoDownload 訊息處理常式
void CGpsdoDownload::OnBnClickedBrowseMaster()
{
	CFileDialog fd(TRUE, "*.bin", m_strMasterPath, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, "*.bin|*.bin||");
	if(fd.DoModal() == IDOK)
	{
		m_strMasterPath = fd.GetPathName();
		GetDlgItem(IDC_MASTER_PATH)->SetWindowText(m_strMasterPath);
	}
	SaveSetting();
}

void CGpsdoDownload::OnBnClickedBrowseSlave()
{
	CFileDialog fd(TRUE, "*.bin", m_strSlavePath, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, "*.bin|*.bin||");
	if(fd.DoModal() == IDOK)
	{
		m_strSlavePath = fd.GetPathName();
		GetDlgItem(IDC_SLAVE_PATH)->SetWindowText(m_strSlavePath);
	}
	SaveSetting();
}

void CGpsdoDownload::SaveSetting()
{
	//m_nBaudrateIdx = ((CComboBox*)GetDlgItem(IDC_BAUDRATE_IDX))->GetCurSel();

	CRegistry reg;
	reg.SetRootKey(HKEY_CURRENT_USER);
	if(reg.SetKey("Software\\GNSSViewer\\GPS", false))
	{
		reg.WriteString("gpsdo_fw_dn_master_path", m_strMasterPath);
		reg.WriteString("gpsdo_fw_dn_slave_path", m_strSlavePath);
		//reg.WriteInt("fw_baudrate", m_nBaudrateIdx);
	}
}

void CGpsdoDownload::LoadSetting()
{
	CRegistry reg;
	reg.SetRootKey(HKEY_CURRENT_USER);
	if(reg.SetKey("Software\\GNSSViewer\\GPS", true))
	{
		m_strMasterPath = reg.ReadString("gpsdo_fw_dn_master_path", "");
		m_strSlavePath = reg.ReadString("gpsdo_fw_dn_slave_path", "");
	}
}

BOOL CGpsdoDownload::OnInitDialog()
{
	CDialog::OnInitDialog();

	LoadSetting();
	GetDlgItem(IDC_MASTER_PATH)->SetWindowText(m_strMasterPath);
	GetDlgItem(IDC_SLAVE_PATH)->SetWindowText(m_strSlavePath);

	return TRUE;  // return TRUE unless you set the focus to a control
}

void CGpsdoDownload::OnClose()
{

	CDialog::OnClose();
}

void CGpsdoDownload::OnBnClickedDownload()
{
	OnOK();
}
