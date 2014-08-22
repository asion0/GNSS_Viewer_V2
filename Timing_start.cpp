// Timing_start.cpp : 實作檔
//

#include "stdafx.h"
#include "GPS.h"
#include "Serial.h"
#include "Timing_start.h"
#include "GPSDlg.h"

CTiming_start* pDlg_timing;

// CTiming_start 對話方塊

IMPLEMENT_DYNAMIC(CTiming_start, CDialog)

CTiming_start::CTiming_start(CWnd* pParent /*=NULL*/)
	: CDialog(CTiming_start::IDD, pParent)
{
	latitude = 0;
	longitude = 0;

}

CTiming_start::~CTiming_start()
{
}

void CTiming_start::DoDataExchange(CDataExchange* pDX)
{
	DDX_Text(pDX,IDC_TXT_LAT,latitude);
	DDX_Text(pDX,IDC_TXT_LON,longitude);

	DDX_Control(pDX, IDC_HOST, m_host);
	DDX_Control(pDX, IDC_NAME, m_name);
	DDX_Control(pDX, IDC_PWD, m_pwd);
	DDX_Control(pDX, IDC_TEXT, m_text);
	DDX_Control(pDX, IDC_PROGRESS1, m_progress);

	DDX_Control(pDX, IDC_START, m_start);
	DDX_Control(pDX, IDCANCEL, m_exit);

	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CTiming_start, CDialog)
	ON_BN_CLICKED(IDC_START, &CTiming_start::OnBnClickedStart)
END_MESSAGE_MAP()


// CTiming_start 訊息處理常式

U08 CTiming_start::Ftp_File(char *remote_file, const char *local_file)
{
	unsigned long time_out ;
	const int textSize = 100;
	char text[textSize];
	//float fpos;
	CString sHost;
	CString sUsername; 
	CString sPassword; 
	U08 ret = TRUE;

	m_host.GetWindowText(sHost);
	m_name.GetWindowText(sUsername);
	m_pwd.GetWindowText(sPassword);

	BOOL bPASV = true;
	CInternetSession sess(0,0,INTERNET_OPEN_TYPE_DIRECT);
	CFtpConnection* pConnect = NULL;

	time_out = 300000;
	sess.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT,time_out);
	sess.SetOption(INTERNET_OPTION_CONTROL_RECEIVE_TIMEOUT,time_out);    
	sess.SetOption(INTERNET_OPTION_CONTROL_SEND_TIMEOUT,time_out);    
	sess.SetOption(INTERNET_OPTION_DATA_RECEIVE_TIMEOUT,time_out*10);    
	sess.SetOption(INTERNET_OPTION_DATA_SEND_TIMEOUT,time_out*10);    
	sess.SetOption(INTERNET_OPTION_SEND_TIMEOUT,time_out);  
	sess.SetOption(INTERNET_OPTION_RECEIVE_TIMEOUT,time_out*10);  
	sess.SetOption(INTERNET_OPTION_DISCONNECTED_TIMEOUT,time_out);  
	sess.SetOption(INTERNET_OPTION_FROM_CACHE_TIMEOUT,time_out);  
	sess.SetOption(INTERNET_OPTION_LISTEN_TIMEOUT,time_out);  

	try
	{      
#ifdef FTP_PASSIVE
		pConnect = sess.GetFtpConnection(sHost, sUsername, sPassword, 21, false);
#else
		pConnect = sess.GetFtpConnection(sHost, sUsername, sPassword, 21, true);
#endif
		sprintf_s(text, textSize, "connect to %s successful!", sHost);
		m_text.SetWindowText(text);

		sprintf_s(text, textSize, "Downloading %s from %s", remote_file,sHost);
		m_text.SetWindowText(text);

		if(!pConnect->GetFile(remote_file,local_file,false))
		{
			int err = GetLastError();
			if(err==12002)
			{
				AfxMessageBox("Download file time out!");
			}
			else if(err==12003)
			{
				AfxMessageBox("Server connection lost!");
			}
			else
			{
				sprintf_s(text, textSize, "Download file failed with error %d",err);
				AfxMessageBox(text);
			}	
			ret = FALSE;
			goto TheLast;
		}
	}
	catch (CInternetException* pEx)
	{
		TCHAR sz[1024];
		pEx->GetErrorMessage(sz, 1024);
		printf("ERROR!  %s\n", sz);
		pEx->Delete();
		AfxMessageBox(sz);
		ret = FALSE;
		goto TheLast;
	}


	ret = TRUE;
TheLast:

	if(pConnect != NULL)
		pConnect->Close();
	delete pConnect;	

	return ret;	
}

U08 CTiming_start::FTP_utc()
{
	U08 ret ;
	const char file_name[] = "utc.dat";
	char remote_path[100];

	sprintf_s(remote_path, 100, "ephemeris\\%s", file_name);
	ret = Ftp_File(remote_path, file_name);
	
	return ret;
}

U08 CTiming_start::FTP_almanac()
{
	U08 ret ;
	const char file_name[] = "almanac.dat";
	char remote_path[100];

	sprintf_s(remote_path, 100, "ephemeris\\%s", file_name);
	ret = Ftp_File(remote_path, file_name);

	return ret;
}

U08 CTiming_start::FTP_Eph()
{
	U08 ret ;
	const char file_name[] = "Eph_4.dat";
	char remote_path[100];

	sprintf_s(remote_path, 100, "ephemeris\\%s", file_name);
	ret = Ftp_File(remote_path, file_name);

	return ret;
}

U08 CTiming_start::set_eph_4()
{
	U08 buff[SATELLITE_EPH_SIZE];
	long file_size;
	int i,j;
	int RETRY_COUNT=10;
//	char msg[100];
	int svid;
	FILE* f = NULL;

	U08 ret = FTP_Eph();
	if(ret == FALSE)
		return FALSE;

	fopen_s(&f, EPH_4_FILE_NAME, "rb");
	fseek(f, 0, SEEK_END);
	file_size = ftell(f);
	fseek(f, 0, SEEK_SET);

	if(f != NULL)
	{
		for(i = 0 ; i< SATELLITES_COUNT ;i++)
		{
			int size = fread(buff,1,SATELLITE_EPH_SIZE,f);
			if(size == SATELLITE_EPH_SIZE)
			{
				int ack = 0;

				svid = buff[5]<<8 | buff[6];
				TRACE("set ephemeris svid=%d\r\n",svid);

				//sprintf(msg,"set eph %d",)
				for(j=0;j<RETRY_COUNT;j++)
				{
					ack = CGPSDlg::gpsDlg->SendToTarget(buff,SATELLITE_EPH_SIZE,"");
					if(ack == 1) break;
				}
				if(j == RETRY_COUNT)
				{
					fclose(f);
					return FALSE;
				}
			}
		}
		fclose(f);
		return TRUE;
	}
	return FALSE;
}

U08 CTiming_start::gps_set_almanac(U08 *almanac)
{
	U08 message[0x3B];		//=len(msg)+7
	U08 msg[0x34];
//	int msg_len;


	memset(msg,0,sizeof(msg));
	msg[0] = 0x51;		//msgid
	memcpy(&msg[1],&almanac[1],0x32);
	msg[0x33] = 0;

	CGPSDlg::gpsDlg->ClearQue();
	int len = CGPSDlg::gpsDlg->SetMessage2(message, msg, sizeof(msg));
	return CGPSDlg::gpsDlg->SendToTarget(message, len, "");
}


U08 CTiming_start::set_almanac()
{
	FILE *f = NULL;
	int i,j;
	U08 ret;
	U08 buff[64];
//	int ret_len;
	U16 svid;

	ret = FTP_almanac();
	if(ret == FALSE)
	{
		return FALSE;
	}

	fopen_s(&f, ALMANAC_FILE_NAME, "rb");
	if(f == NULL)	return FALSE;


	for(i=0;i<32;i++)
	{
		fread(buff,1,ALMANAC_ENTRY_SIZE,f);

		svid = buff[1]<<8 | buff[2];
		TRACE("set almanac svid=%d\r\n",svid);

		for(j=0;j<3;j++)
		{
			ret = gps_set_almanac(buff);

			if(ret == CMD_ACK)
				break;
		}

		if(ret != CMD_ACK)
			break;
	}

	fclose(f);

	return ret;
}

U08 CTiming_start::target_warmstart(int year,U08 mon,U08 day,U08 hour,U08 minute,U08 sec,D64 dlat,D64 dlon)
{
	S16 lat,lon;

	lat = (S16)(dlat * 100 + 0.5);
	lon = (S16)(dlon * 100 + 0.5);

//	int i;
	U08 msg[15];
	U08 messages[22];
	
	msg[0] = 0x1; //msgid
	msg[1] = 0x02; //mode
	msg[2] = year >> 8 &0xff;
	msg[3] = year ;	
	msg[4] = mon ;
	msg[5] = day ;
	msg[6] = hour ;
	msg[7] = minute ;
	msg[8] = sec ;
	msg[9]= lat >>8     &0xff;
	msg[10]= lat        &0xff;	
	msg[11]= lon >>8     &0xff;
	msg[12]= lon        &0xff;	
	msg[13]= 0x00;
	msg[14]= 0x00;	

	CGPSDlg::gpsDlg->ClearQue();
	int len = CGPSDlg::gpsDlg->SetMessage2(messages, msg, sizeof(msg));
	return CGPSDlg::gpsDlg->SendToTarget(messages, len,"System Restart Successful...");
}

U08 CTiming_start::device_warmstart()
{
	FILE *f = NULL;
	U08 buff[7];

	U08 ret = FTP_utc();
	if(ret == FALSE)
		return FALSE;

	fopen_s(&f, "utc.dat", "rb");
	if(f == NULL)
	{
		return FALSE;
	}
	fread(buff, 1, 7, f);
	fclose(f);

	return target_warmstart(buff[0]<<8|buff[1],buff[2],buff[3],buff[4],buff[5],buff[6],latitude,longitude);
}

UINT timing_thread(LPVOID param)
{
	U08 ret;
	char text[100];
	int buad = 0;
	
	pDlg_timing->m_progress.SetPos(0);

	buad = CGPSDlg::gpsDlg->GetBaudrate();
	CGPSDlg::gpsDlg->SetPort(g_setting.boostBaudIndex, 2);

	ret = pDlg_timing->set_almanac();
	if(ret)
	{
		sprintf_s(text, sizeof(text),"set almanac successful.");
	}
	else
	{
		sprintf_s(text, sizeof(text), "set almanac fail.");
	}
	pDlg_timing->m_text.SetWindowText(text);
	
	pDlg_timing->m_progress.SetPos(40);
	
	if(ret)
	{
		ret = pDlg_timing->set_eph_4();
		if(ret)
		{
			sprintf_s(text, sizeof(text), "set ephemeris successful.");
		}
		else
		{
			sprintf_s(text, sizeof(text), "set ephemeris fail.");
		}
		pDlg_timing->m_text.SetWindowText(text);
	}

	pDlg_timing->m_progress.SetPos(70);

	if(ret)
	{
		ret = pDlg_timing->device_warmstart();
		if(ret)
		{
			sprintf_s(text, sizeof(text), "device warmstart successful.");
		}
		else
		{
			sprintf_s(text, sizeof(text), "device warmstart fail.");
		}
		pDlg_timing->m_text.SetWindowText(text);
	}

	pDlg_timing->m_progress.SetPos(100);

	if(ret)
	{
		sprintf_s(text, sizeof(text), "Timing setup successful.");
		pDlg_timing->m_text.SetWindowText(text);
	}

	if(TIMING_MODE)
	{
		CGPSDlg::gpsDlg->SetPort(buad, 2);
	}
	CGPSDlg::gpsDlg->SetBaudrate(buad);


	CGPSDlg::gpsDlg->m_ttffCount = 0;
	CGPSDlg::gpsDlg->m_initTtff = false;
	CGPSDlg::gpsDlg->SetTTFF(0);
	CGPSDlg::gpsDlg->DeleteNmeaMemery();

	pDlg_timing->m_start.EnableWindow(1);
	pDlg_timing->m_exit.EnableWindow(1);

	return 0;
}

void CTiming_start::OnBnClickedStart()
{
	UpdateData();
	m_start.EnableWindow(0);
	m_exit.EnableWindow(0);

	::AfxBeginThread(timing_thread, 0);
}

BOOL CTiming_start::OnInitDialog()
{
	CDialog::OnInitDialog();

	pDlg_timing = this;

	m_host.SetWindowText("60.250.205.31");
	m_name.SetWindowText("skytraq");
	m_pwd.SetWindowText("skytraq");

	// TODO:  在此加入額外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX 屬性頁應傳回 FALSE
}
