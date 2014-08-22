// FTPDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "GPS.h"
#include "Serial.h"
#include "FTPDlg.h"
#include "GPSDlg.h"

const char EphFile[] = "Eph.dat";
const char EphOldFile[] = "Eph_Old.dat";
const char Eph4File[] = "Eph_4.dat";
const char Eph4RomFile[] = "Eph_4_Rom.dat";

// CFTPDlg 對話方塊
IMPLEMENT_DYNAMIC(CFTPDlg, CDialog)
CFTPDlg::CFTPDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CFTPDlg::IDD, pParent)
{
	m_agpsMode = 0;
	m_ephFileName = EphFile;
}

CFTPDlg::~CFTPDlg()
{

}

void CFTPDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_HOST, m_host);
	DDX_Control(pDX, IDC_NAME, m_name);
	DDX_Control(pDX, IDC_PWD, m_pwd);
	DDX_Control(pDX, IDC_PORT, m_port);
	DDX_Control(pDX, IDC_TEXT, m_text);
	DDX_Control(pDX, IDC_PROGRESS1, m_progress);
	DDX_Control(pDX, IDC_FTP_TITLE, m_ftp_title);

	DDX_Control(pDX, IDC_CLOSE, m_closeBtn);
	DDX_Control(pDX, IDC_CONNECT, m_connectBtn);
}

BEGIN_MESSAGE_MAP(CFTPDlg, CDialog)
	ON_BN_CLICKED(IDC_CONNECT, OnBnClickedConnect)
	ON_BN_CLICKED(IDC_CLOSE, OnBnClickedClose)
	ON_BN_CLICKED(IDC_ERASE, &CFTPDlg::OnBnClickedErase)
	ON_STN_CLICKED(IDC_NAME_T, &CFTPDlg::OnStnClickedNameT)
END_MESSAGE_MAP()

// CFTPDlg 訊息處理常式
BOOL CFTPDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	if(IS_DEBUG && (GetAsyncKeyState(VK_LSHIFT) & 0x8000) && (GetAsyncKeyState(VK_LMENU)& 0x8000))
	{
		GetDlgItem(IDC_ERASE)->ShowWindow(SW_SHOW);
	}
	else
	{
		GetDlgItem(IDC_ERASE)->ShowWindow(SW_HIDE);
	}

	m_host.SetWindowText("60.250.205.31");
	m_ftp_title.SetWindowText("This utility can support to get the predicted ephemeris data.");
	m_name.SetWindowText("skytraq");
	m_pwd.SetWindowText("skytraq");
	m_port.SetWindowText("21");
	m_text.SetWindowText("Please press connect button!");
	m_progress.SetRange(0, 100);
	m_progress.SetPos(0);	
	m_closeBtn.EnableWindow(TRUE);
	if(!IS_DEBUG)
	{	//Customer Release doesn't show ftp information. 2013/11/11
		GetDlgItem(IDC_HOST_T)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_NAME_T)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_PWD_T)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_PORT_T)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_HOST)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_NAME)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_PWD)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_PORT)->ShowWindow(SW_HIDE);
	}
	switch(m_agpsMode)
	{
	case 0:
		break;
	case 1:
		break;
	case 2:
		break;
	case 3:
		this->SetWindowText("ROM AGPS");
		break;
	case 4:
		break;	
	default:
		break;
	}
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CFTPDlg::SetMode(int m) 
{ 
	m_agpsMode = m;
	switch(m_agpsMode)
	{
	case 0:
		m_ephFileName = EphFile;
		break;
	case 1:
		m_ephFileName = EphFile;
		break;
	case 2:
		m_ephFileName = Eph4File;
		break;
	case 3:
		m_ephFileName = Eph4RomFile;
		break;
	case 4:
		m_ephFileName = EphOldFile;
		break;	
	default:
		break;
	}
}

bool CFTPDlg::GetFtpFile()
{
	CInternetSession sess(0, 0, INTERNET_OPEN_TYPE_DIRECT);
	unsigned long timeOut = 300000;
	sess.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT, timeOut);
	sess.SetOption(INTERNET_OPTION_CONTROL_RECEIVE_TIMEOUT, timeOut);    
	sess.SetOption(INTERNET_OPTION_CONTROL_SEND_TIMEOUT, timeOut);    
	sess.SetOption(INTERNET_OPTION_DATA_RECEIVE_TIMEOUT, timeOut * 10);    
	sess.SetOption(INTERNET_OPTION_DATA_SEND_TIMEOUT, timeOut * 10);    
	sess.SetOption(INTERNET_OPTION_SEND_TIMEOUT, timeOut);  
	sess.SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT, timeOut * 10);  
	sess.SetOption(INTERNET_OPTION_DISCONNECTED_TIMEOUT, timeOut);  
	sess.SetOption(INTERNET_OPTION_FROM_CACHE_TIMEOUT, timeOut);  
	sess.SetOption(INTERNET_OPTION_LISTEN_TIMEOUT, timeOut);  

	CString strMsg;
	bool isFileExist = false;
	m_progress.SetPos(0);
	try
	{      
		CString sHost;
		CString sUsername; 
		CString sPassword; 
		CString sPort; 
		m_host.GetWindowText(sHost);
		m_name.GetWindowText(sUsername);
		m_pwd.GetWindowText(sPassword);
		m_port.GetWindowText(sPort);
		int nPort  = atoi(sPort); 
#ifdef FTP_PASSIVE
		CFtpConnection* pConnect = sess.GetFtpConnection(sHost, sUsername, sPassword, nPort, false);
#else
		CFtpConnection* pConnect = sess.GetFtpConnection(sHost, sUsername, sPassword, nPort, true);
#endif
		strMsg.Format("connect to %s successful!", sHost);
		m_text.SetWindowText(strMsg);

		CString strLocalName, strRemotePath;
		strLocalName = m_ephFileName;
		strRemotePath.Format("ephemeris\\%s", strLocalName);

		strMsg.Format("Downloading %s from %s", strLocalName, sHost);
		m_text.SetWindowText(strMsg);
		if(!pConnect->GetFile(strRemotePath, strLocalName, false))
		{
			DWORD err = GetLastError();
			strMsg = Utility::ErrorString(err);
			AfxMessageBox(strMsg);
		}
		else
		{
			isFileExist = true;
		}

		if(pConnect != NULL)
		{
			pConnect->Close();
		}
		delete pConnect;
	}
	catch (CInternetException* pEx)
	{
		TCHAR sz[1024];
		pEx->GetErrorMessage(sz, 1024);
		printf("ERROR!  %s\n", sz);
		pEx->Delete();
		AfxMessageBox(sz);
	}

	return isFileExist;
}

void CFTPDlg::GetAgpsFile()
{	
	if(m_agpsMode==4 || GetFtpFile())
	{
		m_progress.SetPos(100);
		m_text.SetWindowText("Start to set the predicted ephemeris to target!");
		FindEphemerisFile();	
	}
//	if(!SetEvent(m_hClose))  
//	{
//		DWORD error = GetLastError(); 
//	}
	PostMessage(WM_CLOSE, 0, 0);
}

void CFTPDlg::DownloadFileFromFTP(CString& source ,CFtpConnection* m_pFtpConnection)
{
	CFile downloadFile;
	if (downloadFile.Open(source, CFile::modeCreate | CFile::modeWrite, NULL) == FALSE)
	{
		TRACE("Unable to create file %s", source); 		
	}
	CInternetFile* pInternetFile = m_pFtpConnection->OpenFile(source, GENERIC_READ, FTP_TRANSFER_TYPE_BINARY );
	if (!pInternetFile)
	{
		printf("Unable to open remote file %s", source); 		
	}	
	char buffer[BUF_SIZE];
	unsigned int nRead = BUF_SIZE;
	unsigned int nTotalRead = 0;
	while (nRead == BUF_SIZE )
	{
		// read remote data into buffer
		nRead = pInternetFile->Read(buffer, BUF_SIZE);
		// write buffer to data file
		downloadFile.Write(buffer, nRead);
		nTotalRead += nRead;		
	}
	// close the file
	downloadFile.Close();
	// close internet file
	pInternetFile->Close();
	delete pInternetFile;	
}  

UINT DoFtpThread(LPVOID pParam)
{	
    ((CFTPDlg*)pParam)->GetAgpsFile();	
	return 0;	
}

void CFTPDlg::OnBnClickedConnect()
{	
//	if(!ResetEvent(m_hClose)) 
//	{
//		DWORD error = GetLastError();
//	}
	m_connectBtn.EnableWindow(FALSE);
	m_closeBtn.EnableWindow(FALSE);
	CGPSDlg::gpsDlg->m_nDownloadBaudIdx = g_setting.boostBaudIndex;
	AfxBeginThread(DoFtpThread, this);
}

void CFTPDlg::FindEphemerisFile()
{			
	if(m_agpsMode == 0)
	{
		ASSERT(FALSE);
	//	GetAllDatFile();
	}
	else if(m_agpsMode == 1)
	{
		GetAllDatFileSrec();
	}
	else if(m_agpsMode == 2)
	{
		DoRomAgps();
	}
	else if(m_agpsMode == 3)
	{
		DoRomAgps();
	}	
	else if(m_agpsMode == 4)
	{
		GetAllDatFileSrec();
	}	
	remove(m_ephFileName);
}

void CFTPDlg::OnBnClickedClose()
{	
	OnCancel();
	return;
}

void CFTPDlg::OnCancel()
{
//	if(m_hClose)
//	{
//		CloseHandle(m_hClose);
//		m_hClose = NULL;
//	}	
	CDialog::OnCancel();
}

bool CFTPDlg::UploadSrec()
{
	U08 msg[1];
	msg[0] = 0x35;		//msgid
	int len = CGPSDlg::gpsDlg->SetMessage(msg, sizeof(msg));
	return CGPSDlg::gpsDlg->SendToTarget(CGPSDlg::m_inputMsg, len, "");
}

bool CFTPDlg::UploadBin()
{
	U08 mycheckt = 0, mycheckb = 0;
	BinaryData ephData;
	if(m_agpsMode == 4)
	{
		ephData.ReadFromResource(IDR_OLDEPH, "EDAT");
	}
	else
	{
		ephData.ReadFromFile(m_ephFileName);
	}

	for(int i=0; i<ephData.Size(); ++i)
    {
		if(i < 0x10000)
		{
			mycheckb += ephData[i];
		}
        mycheckt = mycheckt + ephData[i];
    }	

	CString strMsg;
	strMsg.Format("BINSIZE = %d Checksum = %d Checksumb = %d ", ephData.Size(), mycheckt, mycheckb);
	if(CGPSDlg::gpsDlg->send_command_withackString((U08*)(LPCSTR)strMsg, 
		strMsg.GetLength() + 1, "OK"))
	{
		m_progress.SetPos(2);	
		return TransferFile(ephData);
	}
	else
	{
		return false;
	}
	return true;
}

bool CFTPDlg::TransferFile(BinaryData& ephData)
{
	bool ret = true;
	int lSize = ephData.Size();
	int totalBytes = 0;
	while(lSize > 0)
	{
		int sentBytes = (lSize >= FlashBytes[0]) ? FlashBytes[0] : lSize;
		CGPSDlg::gpsDlg->SendToTargetNoAck(ephData.GetBuffer(totalBytes), sentBytes);
		totalBytes += sentBytes;
		if((totalBytes % sentBytes) == 0)
		{
			if(!CGPSDlg::gpsDlg->wait_res("OK"))
			{
				ret = false;
				break;
			}
		}
		lSize -= sentBytes;
		float fpos = (((float) totalBytes) / ephData.Size()) * 100;
		m_progress.SetPos((int)fpos);
	}	

	if (!CGPSDlg::gpsDlg->wait_res("END"))
	{
		ret = false;
	}
	Sleep(500);
	return ret;
}

void CFTPDlg::DoRomAgps()
{
	BinaryData eph4Data(m_ephFileName);
	CString strMsg;
     // do rom agps
	if (eph4Data.Size() != 3008)             // 3008 = size of 4 hours ephemeris
	{
		strMsg.Format("%s file size error!", m_ephFileName);
		::AfxMessageBox(strMsg);
		return;
	}
//	CGPSDlg::gpsDlg->BoostBaudrate(FALSE, CGPSDlg::ChangeToTemp);
	m_text.SetWindowText("Warm start the target.");
	if(2==m_agpsMode)
	{
		CGPSDlg::gpsDlg->SendRestartCommand(2);
	}
	else if(3==m_agpsMode)
	{
		CGPSDlg::gpsDlg->SendRestartCommand(4);
	}
	Sleep(2000);
	m_text.SetWindowText("Setting ephemeris to target");
	m_progress.SetPos(0);
	for(int i=0; i<32; ++i)
	{
		m_progress.SetPos((int)((i + 1)* 100 / 32.0));
		if(!CGPSDlg::gpsDlg->SendToTarget(eph4Data.GetBuffer(i*94), 94, ""))
		{
			strMsg.Format("SendToTarget fail!", m_ephFileName);
			::AfxMessageBox(strMsg);
			CGPSDlg::gpsDlg->BoostBaudrate(TRUE, CGPSDlg::ChangeToSram);
			return;
		}
	}
//	Sleep(500);
//	CGPSDlg::gpsDlg->BoostBaudrate(TRUE, CGPSDlg::ChangeToTemp);
}

void CFTPDlg::GetAllDatFileSrec()
{
	m_closeBtn.EnableWindow(FALSE);
	CGPSDlg::gpsDlg->BoostBaudrate(FALSE);
	m_text.SetWindowText("Erase flash of target");
	if(!UploadSrec())
	{
		m_text.SetWindowText("Setting predicted ephemeris data is fail.");
		CGPSDlg::gpsDlg->BoostBaudrate(TRUE);
		return; 
	}
	Sleep(500);
	m_text.SetWindowText("Setting ephemeris to target");
	if(UploadBin())
	{
		m_progress.SetPos(100);
		m_text.SetWindowText("Setting predicted ephemeris data is successful.");
		m_closeBtn.EnableWindow(TRUE);
	}
	else
	{
		::AfxMessageBox("Upload AGPS data fail!");
	}
	CGPSDlg::gpsDlg->BoostBaudrate(TRUE);
}

void CFTPDlg::OnBnClickedErase()
{
	SetMode(4);
	m_connectBtn.EnableWindow(FALSE);
	m_closeBtn.EnableWindow(FALSE);
	AfxBeginThread(DoFtpThread, this);
}

void CFTPDlg::OnStnClickedNameT()
{
	// TODO: 在此加入控制項告知處理常式程式碼
}
