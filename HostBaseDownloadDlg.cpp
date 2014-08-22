// HostBaseDownloadDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "HostBaseDownloadDlg.h"
#include "Registry.h"
#include "GPS.h"
#include "GPSDlg.h"
#include "Serial.h"

extern CGPSApp theApp;
// CHostBaseDownloadDlg 對話方塊

IMPLEMENT_DYNAMIC(CHostBaseDownloadDlg, CDialog)

CHostBaseDownloadDlg::CHostBaseDownloadDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_HOSTBASE_DL, pParent)
{

}

CHostBaseDownloadDlg::~CHostBaseDownloadDlg()
{
}

void CHostBaseDownloadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CHostBaseDownloadDlg, CDialog)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BROWSE, OnBnClickedBrowse)
	ON_BN_CLICKED(IDC_SENT_CMD, OnBnClickedSendCmd)
	ON_BN_CLICKED(IDC_SENT_BIN, OnBnClickedSendBin)
END_MESSAGE_MAP()


// CHostBaseDownloadDlg 訊息處理常式

BOOL CHostBaseDownloadDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	CComboBox* baudrateCombo = (CComboBox*)GetDlgItem(IDC_BAUDRATE_IDX);
	CComboBox* bufferCombo = (CComboBox*)GetDlgItem(IDC_BUFFER_IDX);

	if(_V8_SUPPORT)
	{
		baudrateCombo->ResetContent();
		for(int i=0; i<9; ++i)
		{
			CString strIdx;
			strIdx.Format("%d", CSerial::BaudrateTable[i]);
			baudrateCombo->AddString(strIdx);
		}
	}
	baudrateCombo->AddString("SPI");

	CRegistry reg;
	reg.SetRootKey(HKEY_CURRENT_USER);
	if(reg.SetKey("Software\\GNSSViewer\\GPS", true))
	{
		m_nBaudrateIdx = reg.ReadInt("hb_baudrate", g_setting.boostBaudIndex);
		m_strPath = reg.ReadString("hb_firmware", "");
		m_nBufferIdx = reg.ReadInt("hb_buffer", 0);
	}
	else
	{
		m_nBaudrateIdx = BAUDRATE_DEFAULT;
	}

	if(m_strPath.IsEmpty())
	{
		m_strPath = theApp.GetCurrrentDir();
		m_strPath += "\\host.bin";
	}

	baudrateCombo->SetCurSel(m_nBaudrateIdx);
	GetDlgItem(IDC_IMG_PATH)->SetWindowText(m_strPath);
	bufferCombo->SetCurSel(m_nBufferIdx);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX 屬性頁應傳回 FALSE
}
void CHostBaseDownloadDlg::GetValue()
{
	m_nBaudrateIdx = ((CComboBox*)GetDlgItem(IDC_BAUDRATE_IDX))->GetCurSel();
	GetDlgItem(IDC_IMG_PATH)->GetWindowText(m_strPath);
	m_nBufferIdx = ((CComboBox*)GetDlgItem(IDC_BUFFER_IDX))->GetCurSel();

	CRegistry reg;
	reg.SetRootKey(HKEY_CURRENT_USER);
	if(reg.SetKey("Software\\GNSSViewer\\GPS", false))
	{
		reg.WriteInt("hb_baudrate", m_nBaudrateIdx);
		reg.WriteString("hb_firmware", m_strPath);
		reg.WriteInt("hb_buffer", m_nBufferIdx);
	}

}

void CHostBaseDownloadDlg::OnOK()
{
	GetValue();
	CDialog::OnOK();
}

void CHostBaseDownloadDlg::OnBnClickedBrowse()
{
	CFileDialog fd(TRUE, "*.bin", NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, "*.bin|*.bin||");
	if(fd.DoModal() == IDOK)
	{
		m_strPath = fd.GetPathName();
		GetDlgItem(IDC_IMG_PATH)->SetWindowText(m_strPath);
	}

}
UINT DownloadThread(LPVOID pParam);
void CHostBaseDownloadDlg::OnBnClickedSendCmd()
{
	GetValue();

//	CGPSDlg::gpsDlg->m_bHostBasedMode = TRUE;
//	CGPSDlg::gpsDlg->m_isHostOnlySendCmd = TRUE;
//	CGPSDlg::gpsDlg->m_isHostOnlySendBin = FALSE;
//	CGPSDlg::gpsDlg->m_isUsingV8InternalLoader = FALSE;
	CGPSDlg::gpsDlg->m_DownloadMode = CGPSDlg::HostBasedCmdOnly;

	CGPSDlg::gpsDlg->m_nDownloadBaudIdx = GetBaudrateIndex();
	CGPSDlg::gpsDlg->m_strDownloadImage = GetFilePath();
	CGPSDlg::gpsDlg->m_nDownloadBufferIdx = GetBufferIndex();

	GetDlgItem(IDOK)->EnableWindow(FALSE);
	::AfxBeginThread(DownloadThread, 0);
}

void CHostBaseDownloadDlg::OnBnClickedSendBin()
{
	GetValue();
	
//	CGPSDlg::gpsDlg->m_bHostBasedMode = TRUE;
//	CGPSDlg::gpsDlg->m_isHostOnlySendCmd = FALSE;
//	CGPSDlg::gpsDlg->m_isHostOnlySendBin = TRUE;
//	CGPSDlg::gpsDlg->m_isUsingV8InternalLoader = FALSE;
	CGPSDlg::gpsDlg->m_DownloadMode = CGPSDlg::HostBasedBinOnly;

	CGPSDlg::gpsDlg->m_nDownloadBaudIdx = GetBaudrateIndex();
	CGPSDlg::gpsDlg->m_strDownloadImage = GetFilePath();
	CGPSDlg::gpsDlg->m_nDownloadBufferIdx = GetBufferIndex();

	GetDlgItem(IDOK)->EnableWindow(FALSE);
	::AfxBeginThread(DownloadThread, 0);
}

void CHostBaseDownloadDlg::OnClose()
{
	CDialog::OnCancel();		
}