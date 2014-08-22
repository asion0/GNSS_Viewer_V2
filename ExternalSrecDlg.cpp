// ExternalSrecDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "ExternalSrecDlg.h"
#include "Serial.h"
#include "GPSDlg.h"

#define TIMER_ID 1
// CExternalSrecDlg 對話方塊
IMPLEMENT_DYNAMIC(CExternalSrecDlg, CDialog)

CExternalSrecDlg::CExternalSrecDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CExternalSrecDlg::IDD, pParent)
{

}

CExternalSrecDlg::~CExternalSrecDlg()
{
}

void CExternalSrecDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CExternalSrecDlg, CDialog)
	ON_BN_CLICKED(IDC_BROWSE, &CExternalSrecDlg::OnBnClickedBrowse)
	ON_BN_CLICKED(IDC_GO, &CExternalSrecDlg::OnBnClickedGo)
	ON_WM_TIMER()
	ON_WM_CLOSE()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

// CExternalSrecDlg 訊息處理常式
BOOL CExternalSrecDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRegistry reg;
	reg.SetRootKey(HKEY_CURRENT_USER);
	if(reg.SetKey("Software\\GNSSViewer\\GPS",true))
	{
		CString strPath = reg.ReadString("extern_srec_path", "");
		GetDlgItem(IDC_PATH)->SetWindowText(strPath);
		//GetDlgItem(IDC_PATH)->Invalidate();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX 屬性頁應傳回 FALSE
}

void CExternalSrecDlg::OnBnClickedBrowse()
{
	CFileDialog fd(true,"*.srec",NULL,OFN_FILEMUSTEXIST| OFN_HIDEREADONLY,"*.srec|*.srec||");
	if(fd.DoModal() == IDOK)
	{
		CString strPath = fd.GetPathName();
		GetDlgItem(IDC_PATH)->SetWindowText(strPath);
		//m_lbl_firmware_path.Invalidate();

		CRegistry reg;
		reg.SetRootKey(HKEY_CURRENT_USER);
		if(reg.SetKey("Software\\GNSSViewer\\GPS", false))
		{
			reg.WriteString("extern_srec_path", strPath);
		}
	}
}

CWinThread* g_srecThread = NULL;
bool g_stopThread = false;
UINT ConnectSrec(LPVOID pParam)
{	
	U08 buffer[1024] = {0};
	CExternalSrecDlg* pDlg = (CExternalSrecDlg*)pParam;
	while(!g_stopThread)
	{
		DWORD length = CGPSDlg::gpsDlg->m_serial->GetBinary(buffer, sizeof(buffer));
		if(ReadOK(length))
		{
			//Doesn't save skytraq binary data
			if(length > 2 && buffer[0] != 0xA0 && buffer[1] != 0xA1)
			{
				pDlg->AddMsgToList((LPCSTR)buffer);
			}
		}
	}
	return 0;
}

void CExternalSrecDlg::OnBnClickedGo()
{
	CString externalSrecFile;
	GetDlgItem(IDC_PATH)->GetWindowText(externalSrecFile);
	if(!DownloadLoader(externalSrecFile))
	{
		//AfxMessageBox("");				
		return;
	}
	//g_stopThread = false;
	//g_srecThread = ::AfxBeginThread(ConnectSrec, this);	
	//SetTimer(TIMER_ID, 100, NULL);
	OnOK();
}

WlfResult WaitingLoaderFeedback(CSerial* serial, int TimeoutLimit, CWnd* msgWnd);
bool CExternalSrecDlg::DownloadLoader(CString externalSrecFile)
{
	ScopeTimer t("DownloadLoader()");
	const int bufferSize = 256;
	char messages[100] = "$LOADER DOWNLOAD";

	//GetLoaderDownloadCmd(messages, sizeof(messages));
	const int retryCount = 3;
	for (int i=1; i<retryCount; i++)
	{
		CGPSDlg::gpsDlg->SendToTargetNoAck((U08*)messages, (U16)strlen(messages) + 1);
		Sleep(300);

		switch(WaitingLoaderFeedback(CGPSDlg::gpsDlg->m_serial, 5000, NULL))
		{
		case wlf_timeout:
			AfxMessageBox("Target doesn't reply, please power cycle the target!");				
			return false;
			break;
		case wlf_ok:
			i = retryCount;
			break;
		default:
			//Utility::LogFatal(__FUNCTION__, messages, __LINE__);
			break;
		}	
	}
	
	BinaryData srec;
	srec.ReadFromFile(externalSrecFile);

	const U08* sData = srec.Ptr();
	long leftSize = srec.Size();
	int needdelay = 0;
	int totalByte = 0;
	char buff[bufferSize] = {0};

	while(leftSize>0)
	{
		int ProgressPos = 0;
		int packetSize = 0;
		const U08 *tmp = sData;

		memset(buff, 0, sizeof(buff));
		while(1)
		{
			++packetSize;
			if(*tmp == '\n')
			{
				break;
			}
			++tmp;
		}
		memcpy(buff, sData, packetSize - 1);
		buff[packetSize - 2] = 0x0a;
		CGPSDlg::gpsDlg->SendToTargetNoAck((U08*)buff, packetSize);

		leftSize -= packetSize;
		sData += packetSize;
		totalByte += packetSize;//deduct by end of string character in sending
	}			

	memset(buff, 0, sizeof(buff));
	switch(WaitingLoaderFeedback(CGPSDlg::gpsDlg->m_serial, TIME_OUT_MS, NULL))
	{
	case wlf_end:
		//sprintf_s(messages, sizeof(messages), "The total bytes transferred = %d", totalByte);
		//AfxMessageBox(messages);				
		//add_msgtolist(messages);	
		break;
	case wlf_timeout:
		AfxMessageBox("Timeout!");				
		return false;
		break;
	default:
		//Utility::LogFatal(__FUNCTION__, messages, __LINE__);
		break;
	}	

	for(int i=0; i<100; ++i)
	{
		memset(buff, 0, sizeof(buff));
		CGPSDlg::gpsDlg->m_serial->GetString(buff, sizeof(buff), 10*1000);
	}
	return true;
}

//增加訊息到response list
void CExternalSrecDlg::AddMsgToList(LPCTSTR msg)
{
	CListBox* list = (CListBox*)GetDlgItem(IDC_LIST);
	list->AddString(msg);	
	list->SetCurSel(list->GetCount()-1);
}

void CExternalSrecDlg::OnTimer(UINT_PTR nIDEvent)
{
	CDialog::OnTimer(nIDEvent);
}

void CExternalSrecDlg::OnClose()
{
	if(g_srecThread)
	{
		g_stopThread = true;
		::WaitForSingleObject(g_srecThread->m_hThread, 2000);
	}
	CDialog::OnClose();
}

void CExternalSrecDlg::OnOK()
{
	if(g_srecThread)
	{
		g_stopThread = true;
		::WaitForSingleObject(g_srecThread->m_hThread, 2000);
	}
	CDialog::OnOK();
}

void CExternalSrecDlg::OnDestroy()
{

	CDialog::OnDestroy();
}
