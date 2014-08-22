// FirmwareDownloadDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "ParallelDownloadDlg.h"
//#include "resource.h"
#include "Registry.h"
#include "GPS.h"
#include "Serial.h"

extern CGPSApp theApp;
// CParallelDownloadDlg 對話方塊

IMPLEMENT_DYNAMIC(CParallelDownloadDlg, CDialog)

CParallelDownloadDlg::CParallelDownloadDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_PARALLEL_DL, pParent)
{
	m_nBaudrateIdx = 5;	//Only has 115200 loader
}

CParallelDownloadDlg::~CParallelDownloadDlg()
{

}

void CParallelDownloadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CParallelDownloadDlg, CDialog)
	ON_BN_CLICKED(IDC_BROWSE, OnBnClickedBrowse)
END_MESSAGE_MAP()

// CParallelDownloadDlg 訊息處理常式
BOOL CParallelDownloadDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	//CComboBox* baudrateCombo = (CComboBox*)GetDlgItem(IDC_BAUDRATE_IDX);
	CComboBox* flashTypeCombo = (CComboBox*)GetDlgItem(IDC_FLASH_TYPE);
	CComboBox* bufferCombo = (CComboBox*)GetDlgItem(IDC_BUFFER_IDX);

	/*
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
	*/

	CRegistry reg;
	reg.SetRootKey(HKEY_CURRENT_USER);
	if(reg.SetKey("Software\\GNSSViewer\\GPS", true))
	{
		//m_nBaudrateIdx = reg.ReadInt("pl_baudrate", setting->boostBaudIndex);
		//m_nBaudrateIdx = setting->boostBaudIndex;
		m_nFlashType = reg.ReadInt("pl_flashtype", 0);
		m_strPath = reg.ReadString("pl_firmware", "");
		//m_nBufferIdx = reg.ReadInt("pl_buffer", 0);
	}
	else
	{
		m_nBaudrateIdx = g_setting.boostBaudIndex;
		m_nFlashType = 0;
	}

	if(m_strPath.IsEmpty())
	{
		m_strPath = theApp.GetCurrrentDir();
		m_strPath += "\\prom.bin";
	}

	//baudrateCombo->SetCurSel(m_nBaudrateIdx);
	flashTypeCombo->SetCurSel(m_nFlashType);
	
	GetDlgItem(IDC_IMG_PATH)->SetWindowText(m_strPath);
	//((CButton*)GetDlgItem(IDC_IN_LOADER))->SetCheck(m_isInternal);
	//bufferCombo->SetCurSel(m_nBufferIdx);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX 屬性頁應傳回 FALSE
}

void CParallelDownloadDlg::OnOK()
{
	// TODO: 在此加入特定的程式碼和 (或) 呼叫基底類別

	//m_nBaudrateIdx = ((CComboBox*)GetDlgItem(IDC_BAUDRATE_IDX))->GetCurSel();
	m_nFlashType = ((CComboBox*)GetDlgItem(IDC_FLASH_TYPE))->GetCurSel();
	GetDlgItem(IDC_IMG_PATH)->GetWindowText(m_strPath);
//	m_isInternal = ((CButton*)GetDlgItem(IDC_IN_LOADER))->GetCheck();
//	m_nBufferIdx = ((CComboBox*)GetDlgItem(IDC_BUFFER_IDX))->GetCurSel();

	CRegistry reg;
	reg.SetRootKey(HKEY_CURRENT_USER);
	if(reg.SetKey("Software\\GNSSViewer\\GPS", false))
	{
		//reg.WriteInt("pl_baudrate", m_nBaudrateIdx);
		reg.WriteInt("pl_flashtype", m_nFlashType);
		reg.WriteString("pl_firmware", m_strPath);
		//reg.WriteInt("pl_internal", m_isInternal);
		//reg.WriteInt("pl_buffer", m_nBufferIdx);
	}

	CDialog::OnOK();
}

void CParallelDownloadDlg::OnBnClickedBrowse()
{
	CFileDialog fd(TRUE, "*.bin", NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, "*.bin|*.bin||");
	if(fd.DoModal() == IDOK)
	{
		m_strPath = fd.GetPathName();
		GetDlgItem(IDC_IMG_PATH)->SetWindowText(m_strPath);
	}

}