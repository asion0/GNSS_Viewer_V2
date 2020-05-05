// CompressDlg.cpp 
//

#include "stdafx.h"
#include "GPS.h"
#include "CompressDlg.h"
#include "DataLog.h"
#include "SetFilterDlg.h"
#include "NMEA.h"
#include "GPSDlg.h"

CCompressDlg *pComDlg;

int CCompressDlg::GET_NMEA_SENTENCE(CFile& file, unsigned char* buffer)
{
	unsigned char nmea[2];
	unsigned int   length=0;
	int nBytesRead = 0;
    do
	{
		memset(nmea,0,2);
		nBytesRead = file.Read(nmea,1);		
		*(buffer+length) = nmea[0];
		length += nBytesRead;
		if(nBytesRead == 0 || *(buffer+length-1)=='\n' && *(buffer+length-2)=='\r')
			break;		
	}while(1);
	return length;
}

bool CCompressDlg::NMEA_PROC(const char* buffer,int offset)
{
	offset = NMEA::TrimTail(buffer, offset);
	U08 MsgType = NMEA::MessageType(buffer, offset + 1);
	
	switch(MsgType)
	{
	case MSG_ERROR:
		return false;
		break;
	case MSG_GLL:
		nmea.ShowGPGLLmsg(msg_gpgll,buffer,offset);
		break;
	case MSG_GPGSA:
		nmea.ShowGNGSAmsg(msg_gpgsa, msg_glgsa, msg_bdgsa, msg_gagsa, msg_gigsa, buffer, offset);	
    break;
	case MSG_GLGSA:
		nmea.ShowGLGSAmsg(msg_glgsa,buffer,offset);	
		break;
	case MSG_GNGSA:
		nmea.ShowGNGSAmsg(msg_gpgsa, msg_glgsa, msg_bdgsa, msg_gagsa, msg_gigsa, buffer, offset);	
		break;
	case MSG_GGA:
		nmea.ShowGPGGAmsg(msg_gpgga,buffer,offset);		
    break;
	case MSG_ZDA:
    nmea.ShowGPZDAmsg(msg_gpzda,buffer,offset);			
    break;
	case MSG_GPGSV:
		//nmea.ShowGPGSVmsg(msg_gpgsv,buffer,offset);
		break;
	case MSG_GLGSV:
		nmea.ShowGLGSVmsg(msg_glgsv,buffer,offset);
		break;
	case MSG_RMC:
		nmea.ShowGPRMCmsg(msg_gprmc,buffer,offset);
		break;
	case MSG_VTG:
    nmea.ShowGPVTGmsg(msg_gpvtg,buffer,offset);
		break;
	//case MSG_THS:
 //   nmea.ShowGPTHSmsg(m_fTrueHeading,buffer,offset);
	//	break;
	default :
		break;
	}
	return true;	
}

UINT ReadNMEA(LPVOID pParam)
{	
	int    nBytesRead;
	UINT   retCode = 0;	
	U08    nmea[200];	
	ULONGLONG  dwBytesRemaining = pComDlg->Fsource.GetLength();
	
	int write_count = 0;
	int file_tail = 0;

	while(dwBytesRemaining)
	{	
		CString tmp_file;
		tmp_file.Format("%s%d%s", pComDlg->kml_file_name,file_tail, ".kml");
		pComDlg->kml.Init(tmp_file,0x0000ff);
		while(dwBytesRemaining)
		{
			memset(nmea,0,200);		
			nBytesRead = pComDlg->GET_NMEA_SENTENCE(pComDlg->Fsource,nmea);						
			pComDlg->NMEA_PROC((const char*)nmea,nBytesRead-1);			    	
			dwBytesRemaining-=nBytesRead;
			//	TRACE("%d ",dwBytesRemaining);		
			pComDlg->PosFixAlgorithm();	

			write_count++;
			if (write_count > 65000)
			{
				write_count = 0;
				file_tail++;
				break;
			}
		}
		pComDlg->kml.Finish();
	}	

	AfxMessageBox("Compress is completed!");
	pComDlg->Fsource.Close();
	pComDlg->Flog.Close();
	//pComDlg->Fnmea.Close();
	pComDlg->IsFlogOpen    = false;
	pComDlg->IsFSourceOpen = false;
	if(!CGPSDlg::gpsDlg->m_isPressCloseButton)
    CGPSDlg::gpsDlg->CreateGPSThread();
//	AfxEndThread(0);	
	return retCode;
}


// CCompressDlg 

IMPLEMENT_DYNAMIC(CCompressDlg, CDialog)
CCompressDlg::CCompressDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCompressDlg::IDD, pParent)
{
}

CCompressDlg::~CCompressDlg()
{
}

void CCompressDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_source);
	DDX_Control(pDX, IDC_EDIT2, m_datacom);
}


BEGIN_MESSAGE_MAP(CCompressDlg, CDialog)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedButton1)
	ON_BN_CLICKED(IDC_BUTTON2, OnBnClickedButton2)
	ON_BN_CLICKED(IDC_BUTTON4, OnBnClickedButton4)
	ON_BN_CLICKED(IDC_BUTTON3, OnBnClickedButton3)
END_MESSAGE_MAP()


// CCompressDlg 

void CCompressDlg::OnBnClickedButton1()
{
	CString fileName("Source.txt");	
	CFileDialog dlgFile(false, _T("txt"), fileName, OFN_HIDEREADONLY, _T("ALL Files (*.*)|*.*||"), this);
  	INT_PTR nResult = dlgFile.DoModal();
	fileName = dlgFile.GetPathName();	
	CFileException ef;
	try
	{
		if(nResult == IDOK)
		{          
		    if(!Fsource.Open(fileName,CFile::modeReadWrite,&ef))
			{
				ef.ReportError();
				return;
			}
			IsFSourceOpen = true;
			m_source.SetWindowText(fileName);
			fileName = dlgFile.GetFileTitle();
			kml_file_name = fileName;
	        //fileName+=".kml";	
	        //Fnmea.Open(fileName,CFile::modeReadWrite|CFile::modeCreate);		       
		}
	}
	catch(CFileException *fe)
	{
		fe->ReportError();
		fe->Delete();
	}		
	fileName.ReleaseBuffer();	
}

void CCompressDlg::OnBnClickedButton2()
{
		CString fileName("Data.log");	
	CFileDialog dlgFile(false, _T("log"), fileName, OFN_HIDEREADONLY, _T("ALL Files (*.*)|*.*||"), this);
   	INT_PTR nResult = dlgFile.DoModal();	
	fileName = dlgFile.GetPathName();	
	try
	{
		if(nResult == IDOK)
		{          
		    Flog.Open(fileName,CFile::modeReadWrite|CFile::modeCreate);	
			IsFlogOpen = true;
			m_datacom.SetWindowText(fileName);			
		}else if(nResult == IDCANCEL)
		{  			
		    return;			
		}
	}
	catch(CFileException *fe)
	{
		fe->ReportError();
		fe->Delete();
	}	
	fileName.ReleaseBuffer();	
}

void CCompressDlg::OnBnClickedButton4()
{
	CSetFilterDlg* pDlg = new  CSetFilterDlg;
	pDlg->DoModal();

	_cprintf("%d %d %d %d %d %d\n",	        
	        LogFlashInfo.min_time,
			LogFlashInfo.max_time,
	        LogFlashInfo.min_distance,
	        LogFlashInfo.max_distance,
	        LogFlashInfo.min_speed,
	        LogFlashInfo.max_speed);
	delete pDlg;
	pDlg = NULL;
}

void CCompressDlg::OnBnClickedButton3()
{
	if(!IsFlogOpen )
	{
		AfxMessageBox("Plz create a compress file");
		return;
	}
	if(!IsFSourceOpen)
	{
		AfxMessageBox("Plz select a source file to compress");
		return;
	}	

	memset(&msg_gpgll,0,sizeof(GPGLL));
	memset(&msg_gpgsa,0,sizeof(GPGSA));
	memset(&msg_gpgga,0,sizeof(GPGGA));
	//memset(&msg_gpgsv,0,sizeof(GPGSV));
	memset(&msg_glgsv,0,sizeof(GPGSV));
	memset(&msg_gpzda,0,sizeof(GPZDA));
	memset(&msg_gprmc,0,sizeof(GPRMC));
	memset(&msg_gpvtg,0,sizeof(GPVTG));

	AfxBeginThread(ReadNMEA,0);	
	inilog=true;
	
	OnOK();
}


BOOL CCompressDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	pComDlg =this;
	IsFlogOpen=false;
	IsFSourceOpen=false;

	LogFlashInfo.max_time=3600;
	LogFlashInfo.min_time=5;
	LogFlashInfo.min_distance=20;
	LogFlashInfo.max_distance=255;
	LogFlashInfo.min_speed=0;
	LogFlashInfo.max_speed=511;

	return TRUE;  // return TRUE unless you set the focus to a control
}


int Abs(int x)
{
	if(x<0)
		return -x;
	else
		return x;
}

void CCompressDlg::PosFixAlgorithm()
{
	if(!IsFlogOpen)
	{
		return;
	}

	U16 X_DIFF , Y_DIFF, Z_DIFF;
	if((msg_gpgga.GPSQualityIndicator==49 || msg_gpgga.GPSQualityIndicator==50) && X!=0 && Y!=0 && Z!=0)
	{
		LLA2ECEF();
		m_lon = Rad2Deg(m_lon);
		m_lat = Rad2Deg(m_lat);		
		CString timeStr;
		timeStr.Format("%02d:%02d:%05.2f", msg_gpgga.Hour, msg_gpgga.Min, msg_gpgga.Sec);
		kml.PushOnePoint(m_lon, m_lat, m_alt, timeStr, GetGnssQualityMode(msg_gpgga.GPSQualityIndicator));

		Current.TOW     = (U32)(msg_gpgga.Hour*3600 + msg_gpgga.Min*60 + msg_gpgga.Sec);
		Current.WNO     = 120;
		Current.V       = (U16)msg_gpvtg.SpeedKmPerHur;
		Current.ECEF_X  = (S32)X;
		Current.ECEF_Y  = (S32)Y;
		Current.ECEF_Z  = (S32)Z;
		//caculate the difference between now and the last storage time  
		Current.DTOW    = (U16)(Current.TOW - Last.TOW);

		//Caculate the difference between now and the last storage time

		X_DIFF = Abs(Current.ECEF_X - Last.ECEF_X);
		Y_DIFF = Abs(Current.ECEF_Y - Last.ECEF_Y);
		Z_DIFF = Abs(Current.ECEF_Z - Last.ECEF_Z);

		Current.DECEF_X = (S16)(Current.ECEF_X - Last.ECEF_X);
		Current.DECEF_Y = (S16)(Current.ECEF_Y - Last.ECEF_Y);
		Current.DECEF_Z = (S16)(Current.ECEF_Z - Last.ECEF_Z);
        if(inilog)
		{		
			FULL_DATA();
			inilog = false;
			Last.TOW = Current.TOW;
	        Last.ECEF_X = Current.ECEF_X;
	        Last.ECEF_Y = Current.ECEF_Y;
	        Last.ECEF_Z = Current.ECEF_Z;

			kml.PushOnePoint(m_lon, m_lat, m_alt, timeStr, GetGnssQualityMode(msg_gpgga.GPSQualityIndicator));
		}
		else
		{				
	        //WriteKMLPath();
			
			if((Current.DTOW > LogFlashInfo.min_time    ) &&
		       (X_DIFF       >= LogFlashInfo.min_distance) &&
		       (Y_DIFF       >= LogFlashInfo.min_distance) &&
		       (Z_DIFF       >= LogFlashInfo.min_distance) &&
               (Current.V    >= LogFlashInfo.min_speed   ) ||
		       (Current.DTOW > LogFlashInfo.max_time    ) ||
		       (X_DIFF       > LogFlashInfo.max_distance) ||
		       (Y_DIFF       > LogFlashInfo.max_distance) ||
		       (Z_DIFF       > LogFlashInfo.max_distance) ||
		       (Current.V    > LogFlashInfo.max_speed   )
		      )	   
		    {	
				_cprintf("%d %d %d %d %d\n\n",Current.DTOW,X_DIFF,Y_DIFF,Z_DIFF,Current.V);
				if((X_DIFF >511)||
			       (Y_DIFF >511)||
			       (Z_DIFF >511)||
			       (Current.DTOW   >65535))
			    {
					//store a FIX_FULL record to the flash
                    FULL_DATA();
			    }
			    else
				{
					//store a FIX_INC record to the flash
					INC_DATA();
				}
                //Backup storage time and position 
				Last.TOW = Current.TOW;
	            Last.ECEF_X = Current.ECEF_X;
	            Last.ECEF_Y = Current.ECEF_Y;
	            Last.ECEF_Z = Current.ECEF_Z;
		    }
		}		
	}
}


void CCompressDlg::FULL_DATA()
{
	memset(&FixFull,0,sizeof(FixFull));
	U16 word0;	
	word0            = (U16)(Current.V)           &0x3ff;
    word0           |= (1<<14);
	FixFull.word[0]  = word0>>8            &0xff;
	FixFull.word[0] |= word0<<8            &0xff00;
	FixFull.word[1]  = Current.TOW         &0xf;
    FixFull.word[1]  = FixFull.word[1]<<12;
    FixFull.word[1] |= Current.WNO         &0x3ff;
/*	FixFull.word[1]  = Current.WNO         &0x3ff;
	FixFull.word[1]  = FixFull.word[1]<<6;
	FixFull.word[1] |= Current.TOW         &0xf;*/
	FixFull.word[2]  = Current.TOW>>4      &0xffff;
	FixFull.word[3]  = Current.ECEF_X      &0xffff;
	FixFull.word[4]  = Current.ECEF_X>>16  &0xffff;
	FixFull.word[5]  = Current.ECEF_Y      &0xffff;
	FixFull.word[6]  = Current.ECEF_Y>>16  &0xffff;
	FixFull.word[7]  = Current.ECEF_Z      &0xffff;
	FixFull.word[8]  = Current.ECEF_Z>>16  &0xffff;
	Flog.Write(&FixFull.word[0],sizeof(FixFull));
//	for(int i=0;i<9;i++)_cprintf("%x ",FixFull.word[i]);
}
void CCompressDlg::INC_DATA()
{
	if(Current.DECEF_X<0) Current.DECEF_X = 511+Current.DECEF_X*(-1);
	if(Current.DECEF_Y<0) Current.DECEF_Y = 511+Current.DECEF_Y*(-1);
	if(Current.DECEF_Z<0) Current.DECEF_Z = 511+Current.DECEF_Z*(-1);
    U16 word0;
	memset(&FixInc,0,sizeof(FixInc));
	word0           = (U16)(Current.V)            &0x3ff;
    word0          |= (1<<15);
	FixInc.word[0]  = word0>>8             &0xff;
	FixInc.word[0] |= word0<<8             &0xff00;
	FixInc.word[1]  = Current.DTOW         &0xffff;
	
	FixInc.word[2]  = Current.DECEF_X      &0x3ff;
    FixInc.word[2]  = FixInc.word[2]  << 6;
    FixInc.word[2] |= Current.DECEF_Y       &0x3f;
    FixInc.word[3]  = Current.DECEF_Y >> 6  &0xf;
    FixInc.word[3]  = FixInc.word[3]  << 12;
    FixInc.word[3] |= Current.DECEF_Z       &0x3ff;  


	Flog.Write(&FixInc.word[0],sizeof(FixInc));
}

void CCompressDlg::LLA2ECEF(void)
{
	h=msg_gpgga.GeoidalSeparation;

	m_lon = int(msg_gpgga.Longitude/100);
	m_lon += double(msg_gpgga.Longitude-int(msg_gpgga.Longitude/100)*100)/60;
	if (msg_gpgga.Longitude_E_W == 'W')
		m_lon *= -1;
	m_lat = int(msg_gpgga.Latitude/100);
	m_lat += double(msg_gpgga.Latitude-int(msg_gpgga.Latitude/100)*100)/60;
	if (msg_gpgga.Latitude_N_S == 'S')
		m_lat *= -1;

	m_alt = msg_gpgga.Altitude;

	m_lon = Deg2Rad(m_lon);
	m_lat = Deg2Rad(m_lat);
	N=WGS84_RA/(sqrt(1-WGS84_E2*sin(m_lat)*sin(m_lat)));		
	X=(N+h)*cos(m_lat)*cos(m_lon);
	Y=(N+h)*cos(m_lat)*sin(m_lon);
	Z=(N*(1-WGS84_E2)+h)*sin(m_lat);	
}
