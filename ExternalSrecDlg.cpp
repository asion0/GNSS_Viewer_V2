// ExternalSrecDlg.cpp
//

#include "stdafx.h"
#include "ExternalSrecDlg.h"
#include "Serial.h"
#include "GPSDlg.h"

#define TIMER_ID 1
// CExternalSrecDlg 
IMPLEMENT_DYNAMIC(CExternalSrecDlg, CDialog)

CExternalSrecDlg::CExternalSrecDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CExternalSrecDlg::IDD, pParent)
{
  m_isV6 = false;
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

// CExternalSrecDlg
BOOL CExternalSrecDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CRegistry reg;
	reg.SetRootKey(HKEY_CURRENT_USER);
	if(reg.SetKey(VIEWER_REG_PATH,true))
	{
		CString str;
		str = reg.ReadString("extern_srec_path", "");
		GetDlgItem(IDC_PATH)->SetWindowText(str);
		str = reg.ReadString("extern_srec_cmd", "");
		GetDlgItem(IDC_CMD)->SetWindowText(str);

		//GetDlgItem(IDC_PATH)->Invalidate();
	}

	return TRUE;  // return TRUE unless you set the focus to a control
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
		if(reg.SetKey(VIEWER_REG_PATH, false))
		{
			reg.WriteString("extern_srec_path", strPath);
		}
	}
}

void CExternalSrecDlg::OnBnClickedGo()
{
	CRegistry reg;

	CString str;
	GetDlgItem(IDC_CMD)->GetWindowText(str);

	if(!str.IsEmpty())
	{
		reg.SetRootKey(HKEY_CURRENT_USER);
		if(reg.SetKey(VIEWER_REG_PATH, false))
		{
			reg.WriteString("extern_srec_cmd", str);
		}
	}
  m_isV6 = ((CButton*)GetDlgItem(IDC_V6))->GetCheck() != 0;
	CString externalSrecFile;
	GetDlgItem(IDC_PATH)->GetWindowText(externalSrecFile);

	if(!DownloadLoader(externalSrecFile))
	{
		return;
	}
	OnOK();
}

WlfResult WaitingLoaderFeedback(CSerial* serial, int TimeoutLimit, CWnd* msgWnd);
bool CExternalSrecDlg::DownloadLoader(CString externalSrecFile)
{
	ScopeTimer t("DownloadLoader()");
	const int bufferSize = 256;
	char messages[100] = "$LOADER DOWNLOAD";

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
  
  int endTimes = (m_isV6) ? 2 : 1;
	memset(buff, 0, sizeof(buff));
  int exit = 0;
  do
  {
	  switch(WaitingLoaderFeedback(CGPSDlg::gpsDlg->m_serial, TIME_OUT_MS, NULL))
	  {
	  case wlf_timeout:
		  AfxMessageBox("Timeout!");				
		  return false;
		  break;
    case wlf_end:
      ++exit;
      break;
	  default:
		  break;
	  }	
  } while (exit < endTimes);
	GetDlgItem(IDC_CMD)->GetWindowText(messages, sizeof(messages));
	CGPSDlg::gpsDlg->m_serial->SendData((U08*)messages, (U16)strlen(messages) + 1);	

	for(int i=0; i<100; ++i)
	{
		memset(buff, 0, sizeof(buff));
		CGPSDlg::gpsDlg->m_serial->GetString(buff, sizeof(buff), 10*1000);
	}
	return true;
}

//add message to response list
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
	//if(g_srecThread)
	//{
	//	g_stopThread = true;
	//	::WaitForSingleObject(g_srecThread->m_hThread, 2000);
	//}
	CDialog::OnClose();
}

void CExternalSrecDlg::OnOK()
{
	//if(g_srecThread)
	//{
	//	g_stopThread = true;
	//	::WaitForSingleObject(g_srecThread->m_hThread, 2000);
	//}
	CDialog::OnOK();
}

void CExternalSrecDlg::OnDestroy()
{
	CDialog::OnDestroy();
}
