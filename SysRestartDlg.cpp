// SysRestartDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "GPS.h"
#include "SysRestartDlg.h"
#include "GPSDlg.h"

//CSysRestartDlg* CSysRestartDlg::pSysRestartDlg = NULL;	

unsigned char CSysRestartDlg::restartMode = 0;
unsigned char CSysRestartDlg::SRutcmonth = 0;
unsigned char CSysRestartDlg::SRutcday = 0;
unsigned char CSysRestartDlg::SRutchour = 0;
unsigned char CSysRestartDlg::SRutcmin = 0;
unsigned char CSysRestartDlg::SRutcsec = 0;
unsigned short int CSysRestartDlg::SRutcyear = 0;
signed short int CSysRestartDlg::SRlat = 0;
signed short int CSysRestartDlg::SRlon = 0;
signed short int CSysRestartDlg::SRalt = 0;

UINT ResetThread(LPVOID pParam)
{		
	((CSysRestartDlg*)pParam)->Restart();
	return 0;
}

// CSysRestartDlg 對話方塊
IMPLEMENT_DYNAMIC(CSysRestartDlg, CDialog)
CSysRestartDlg::CSysRestartDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSysRestartDlg::IDD, pParent)
{
//	pSysRestartDlg = this;
}

CSysRestartDlg::~CSysRestartDlg()
{
//	pSysRestartDlg = NULL;
}

void CSysRestartDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SRMODE, m_startmode);
	DDX_Control(pDX, IDC_SRYEAR, m_utcyear);
	DDX_Control(pDX, IDC_SRMONTH, m_utcmonth);
	DDX_Control(pDX, IDC_SRDAY, m_utcday);
	DDX_Control(pDX, IDC_SRHOUR, m_utchour);
	DDX_Control(pDX, IDC_SRMIN, m_utcminute);
	DDX_Control(pDX, IDC_SRSEC, m_utcsecond);
	DDX_Control(pDX, IDC_SRLAT, m_latitude);
	DDX_Control(pDX, IDC_SRLON, m_longitude);
	DDX_Control(pDX, IDC_SRALT, m_altitude);
}


BEGIN_MESSAGE_MAP(CSysRestartDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_CBN_CLOSEUP(IDC_SRMODE, OnCbnCloseupSrmode)
END_MESSAGE_MAP()


// CSysRestartDlg 訊息處理常式

BOOL CSysRestartDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	SYSTEMTIME	now; 
	char str[10];	
	GetSystemTime(&now);	
	m_startmode.SetCurSel(0);
	sprintf_s(str, sizeof(str), "%d",now.wYear);
	m_utcyear.SetWindowText(str);
	m_utcmonth.SetCurSel(now.wMonth-1);
	m_utcday.SetCurSel(now.wDay-1);
	m_utchour.SetCurSel(now.wHour);
	m_utcminute.SetCurSel(now.wMinute);
	m_utcsecond.SetCurSel(now.wSecond);

	if(GPS_VIEWER)
	{	//GPS FW的Cold Start不會清除全部星曆資料，而Entirely Cold Start才會。
		//GNSS FW的Cold Start會清除全部星曆資料，不需Entirely Cold Start。
		m_startmode.AddString("Entirely Cold Start");
	}

//	DWORD GetRegValue(HKEY hKeyHandle, LPCTSTR lpSubkey, LPCTSTR lpRegName, CString &csRegvalue);
//	DWORD GetRegValue(HKEY hKeyHandle, LPCTSTR lpSubkey, LPCTSTR lpRegName, int &nRegvalue);
	CString tmpString;
	Utility::GetRegValue(HKEY_CURRENT_USER, "Software\\GNSSViewer\\GPS", "lat", tmpString);
	if(tmpString.IsEmpty()) 
	{
		tmpString = "25";
	}
	m_latitude.SetWindowText(tmpString);

	Utility::GetRegValue(HKEY_CURRENT_USER, "Software\\GNSSViewer\\GPS", "lon", tmpString);
	if(tmpString.IsEmpty()) 
	{
		tmpString = "124";
	}
	m_longitude.SetWindowText(tmpString);

	Utility::GetRegValue(HKEY_CURRENT_USER, "Software\\GNSSViewer\\GPS", "alt", tmpString);
	if(tmpString.IsEmpty()) 
	{
		tmpString = "100";
	}
	m_altitude.SetWindowText(tmpString);

	switch (ReStartType)
	{
	case 0:	
		m_utcyear.EnableWindow(0);
		m_utcmonth.EnableWindow(0);
		m_utcday.EnableWindow(0);
		m_utchour.EnableWindow(0);
		m_utcminute.EnableWindow(0);
		m_utcsecond.EnableWindow(0);	
		m_latitude.EnableWindow(0);
		m_longitude.EnableWindow(0);
		m_altitude.EnableWindow(0);
		break;
		
	case 1:		
		m_startmode.EnableWindow(0);
		m_startmode.SetCurSel(1);
		break;
	case 2:
		m_startmode.EnableWindow(0);
		m_startmode.SetCurSel(2);
		break;
	case 3:
		m_startmode.EnableWindow(0);
		m_startmode.SetCurSel(3);
		break;
	case 4:
		m_startmode.EnableWindow(0);
		m_startmode.SetCurSel(3);
		break;
	default:
		break;
	}
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX 屬性頁應傳回 FALSE
}

void CSysRestartDlg::OnBnClickedOk()
{		
	if(!CheckMessage())
	{		
		return;
	}

	restartMode = m_startmode.GetCurSel();
    SRutcmonth= m_utcmonth.GetCurSel()+1;
    SRutcday  = m_utcday.GetCurSel()+1;
    SRutchour = m_utchour.GetCurSel();
    SRutcmin  = m_utcminute.GetCurSel();
    SRutcsec  = m_utcsecond.GetCurSel();
    SRutcyear = utcyear;
    SRlat     = lat;
    SRlon     = lon;
    SRalt     = alt;	

	CString tmp;
	m_latitude.GetWindowText(tmp);
	Utility::SetRegValue(HKEY_CURRENT_USER, "Software\\GNSSViewer\\GPS", "lat", tmp);
	m_longitude.GetWindowText(tmp);
	Utility::SetRegValue(HKEY_CURRENT_USER, "Software\\GNSSViewer\\GPS", "lon", tmp);
	m_altitude.GetWindowText(tmp);
	Utility::SetRegValue(HKEY_CURRENT_USER, "Software\\GNSSViewer\\GPS", "alt", tmp);

	AfxBeginThread(ResetThread, this);	
	OnOK();
}

bool CSysRestartDlg::CheckMessage()
{	
	CString data;
	m_utcyear.GetWindowText(data);
    utcyear = atoi(data); 
	_cprintf("UTC Year: %d\n",utcyear);
	if(utcyear < 1980 )
	{		
		AfxMessageBox("UTC Year must be greater than 1980");
			return false;
	}
	
	m_latitude.GetWindowText(data);
    lat  = (int)(atof(data)*100);
	_cprintf("Latitude :%d\n",lat);
	if(lat <-9000 || lat >9000)
	{		
		AfxMessageBox("Latitude must be between -90 and 90");
			return false;
	}
	m_longitude.GetWindowText(data);
	lon  = (int)(atof(data)*100);
	_cprintf("Longitude: %d\n",lon);
	if(lon <-18000 || lon >18000)
	{		
		AfxMessageBox("Longitude must be between -180 and 180");
			return false;
	}

	m_altitude.GetWindowText(data);
    alt  = atoi(data);
	_cprintf("Altitude: %d\n",alt);
	if(alt <-1000 || alt >18300)
	{		
		AfxMessageBox("Altitude must be between -1000 and 18300");
			return false;
	}
	return true;
}

void CSysRestartDlg::OnCbnCloseupSrmode()
{
	int index = m_startmode.GetCurSel();
	if(index!=LB_ERR)
	{
		if(index == 0)
		{
			m_utcyear.EnableWindow(0);
			m_utcmonth.EnableWindow(0);
			m_utcday.EnableWindow(0);
			m_utchour.EnableWindow(0);
			m_utcminute.EnableWindow(0);
			m_utcsecond.EnableWindow(0);	
			m_latitude.EnableWindow(0);
			m_longitude.EnableWindow(0);
			m_altitude.EnableWindow(0);
		}
		else
		{
			m_utcyear.EnableWindow(1);
			m_utcmonth.EnableWindow(1);
			m_utcday.EnableWindow(1);
			m_utchour.EnableWindow(1);
			m_utcminute.EnableWindow(1);
			m_utcsecond.EnableWindow(1);	
			m_latitude.EnableWindow(1);
			m_longitude.EnableWindow(1);
			m_altitude.EnableWindow(1);
		}
	}
}


void CSysRestartDlg::Restart()
{
	CGPSDlg::gpsDlg->m_CloseBtn.ShowWindow(0);
	int i;
	U08 messages[22] = {0};

	messages[0] = (U08)0xa0;
	messages[1] = (U08)0xa1;
	messages[2] = 0;
	messages[3] = 15;
	messages[4] = 1; //msgid
	messages[5] = restartMode; //msgid
	if(restartMode != 3 && restartMode != 5)
	{
		messages[6] = SRutcyear>>8 &0xff;
		messages[7] = SRutcyear    &0xff;		
		messages[8] = SRutcmonth;
		messages[9] = SRutcday;
		messages[10]= SRutchour;
		messages[11]= SRutcmin;
		messages[12]= SRutcsec;
		messages[13]= SRlat>>8     &0xff;
		messages[14]= SRlat        &0xff;		
		messages[15]= SRlon>>8     &0xff;
		messages[16]= SRlon        &0xff;		
		messages[17]= SRalt>>8     &0xff;
		messages[18]= SRalt        &0xff;	
	}
	unsigned char checksum = 0;
	for(i=0;i<15;i++)
		checksum^=messages[i+4];

	messages[19]=checksum; //checksum right	    
	messages[20]=(unsigned char)0x0d;
	messages[21]=(unsigned char)0x0a;
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(messages, 22, "System Restart Successful...");

	//for(int i=0;i<22;i++)	_cprintf("%x ",messages[i]);	
	/*
	CGPSDlg::gpsDlg->ClearQue();
	CGPSDlg::gpsDlg->SendToTarget(messages, 22, "System Restart Successful...");	
	CGPSDlg::gpsDlg->SetMode(); 
	//CGPSDlg::gpsDlg->InvalidateRect(CRect(330,210,330+700,363),TRUE);
	//CGPSDlg::gpsDlg->InvalidateRect(CRect(332,20,497,210),TRUE);	
	CGPSDlg::gpsDlg->CreateGPSThread();
	*/
	CGPSDlg::gpsDlg->m_CloseBtn.ShowWindow(1);
}
