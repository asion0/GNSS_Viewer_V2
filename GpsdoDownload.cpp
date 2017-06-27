// GpsdoDownload.cpp 
//

#include "stdafx.h"
#include "GpsdoDownload.h"
#include "resource.h"


// CGpsdoDownload 
const int DefaultDownloadBaudRateIdx = 7;
const int DefaultFirmwareBaudRateIdx = 1;

IMPLEMENT_DYNAMIC(CGpsdoDownload, CDialog)

CGpsdoDownload::CGpsdoDownload(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_GPSDO_DOWNLOAD, pParent)
{
	m_downloadBaudIdx = DefaultDownloadBaudRateIdx;
	m_masterFwBaudIdx = DefaultFirmwareBaudRateIdx;
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


// CGpsdoDownload 
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
		AutoDetectBaudRate(m_strSlavePath, IDC_SLAVE_T);
	}
	SaveSetting();
}

void CGpsdoDownload::SaveSetting()
{
	//m_nBaudrateIdx = ((CComboBox*)GetDlgItem(IDC_BAUDRATE_IDX))->GetCurSel();

	CRegistry reg;
	reg.SetRootKey(HKEY_CURRENT_USER);
	if(reg.SetKey(VIEWER_REG_PATH, false))
	{
		reg.WriteString("msdownload_fw_dn_master_path", m_strMasterPath);
		reg.WriteString("msdownload_fw_dn_slave_path", m_strSlavePath);
		reg.WriteInt("msdownload_downloadBaudIdx", m_downloadBaudIdx);
		reg.WriteInt("msdownload_masterFwBaudIdx", m_masterFwBaudIdx);
	}
}

void CGpsdoDownload::LoadSetting()
{
	CRegistry reg;
	reg.SetRootKey(HKEY_CURRENT_USER);
	if(reg.SetKey(VIEWER_REG_PATH, true))
	{
		m_strMasterPath = reg.ReadString("msdownload_fw_dn_master_path", "");
		m_strSlavePath = reg.ReadString("msdownload_fw_dn_slave_path", "");
		m_downloadBaudIdx = reg.ReadInt("msdownload_downloadBaudIdx", DefaultDownloadBaudRateIdx);
		m_masterFwBaudIdx = reg.ReadInt("msdownload_masterFwBaudIdx", DefaultFirmwareBaudRateIdx);
	}
}

BOOL CGpsdoDownload::OnInitDialog()
{
	CDialog::OnInitDialog();

	LoadSetting();
	GetDlgItem(IDC_MASTER_PATH)->SetWindowText(m_strMasterPath);
	GetDlgItem(IDC_SLAVE_PATH)->SetWindowText(m_strSlavePath);
	((CComboBox*)GetDlgItem(IDC_SLAVE_C))->SetCurSel(m_downloadBaudIdx);
	((CComboBox*)GetDlgItem(IDC_SLAVE_T))->SetCurSel(m_masterFwBaudIdx);
	//AutoDetectBaudRate(m_strSlavePath, IDC_SLAVE_T);
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CGpsdoDownload::AutoDetectBaudRate(CString path, UINT uid)
{
	CComboBox* combo = (CComboBox*)GetDlgItem(uid);
	CString s = Utility::GetFileName(m_strSlavePath);

	if(s.Find("4800") > 0)
	{
		combo->SetCurSel(0);
	}
	else if(s.Find("9600") > 0)
	{
		combo->SetCurSel(1);
	}
	else if(s.Find("19200") > 0)
	{
		combo->SetCurSel(2);
	}
	else if(s.Find("38400") > 0)
	{
		combo->SetCurSel(3);
	}
	else if(s.Find("57600") > 0)
	{
		combo->SetCurSel(4);
	}
	else if(s.Find("115200") > 0)
	{
		combo->SetCurSel(5);
	}
	else if(s.Find("230400") > 0)
	{
		combo->SetCurSel(6);
	}
	else if(s.Find("460800") > 0)
	{
		combo->SetCurSel(7);
	}
	else if(s.Find("921600") > 0)
	{
		combo->SetCurSel(8);
	}
}

void CGpsdoDownload::OnClose()
{

	CDialog::OnClose();
}

void CGpsdoDownload::OnBnClickedDownload()
{
	if(!Utility::IsFileExist(m_strMasterPath) || !Utility::IsFileExist(m_strSlavePath))
	{
		AfxMessageBox("Firmware bin file is not exist!");
	}

	m_downloadBaudIdx = ((CComboBox*)GetDlgItem(IDC_SLAVE_C))->GetCurSel();
	m_masterFwBaudIdx = ((CComboBox*)GetDlgItem(IDC_SLAVE_T))->GetCurSel();
	SaveSetting();

	OnOK();
}
