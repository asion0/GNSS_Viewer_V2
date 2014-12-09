// GPSDlg.cpp : implementation file
//
#include "stdafx.h"
#include "GPS.h"
#include "GPSDlg.h"

//#include "DownloadDlg.h"
//#include "RomWriterDlg.h"
#include "LogFilterDlg.h"
#include "KmlDlg.h"
#include "log2nmea.h"
#include "PositionFinderDlg.h"
#include "skytraqkml.h"
#include "ConPinningParameter.h"
#include "ConMultiMode.h"
#include "Con_register.h"
#include "Con1PPS.h"
#include "ConMultiPath.h"
#include "ConWaas.h"
#include "GetAlmanac.h"
#include "Cfg_binary_msg.h"
#include "Con1PPS_Timing.h"
#include "Config_binary_interval.h"
#include "Con1PPS_cable.h"
#include "Con1PPS_DOP.h"
#include "Con1PPS_ElevCNR.h"
#include "Proprietary_nmea.h"
#include "Device_Adding.h"
#include "geoid.h"
#include "Config_USB.h"
#include "ConGNSSSelectionforNAV.h"
#include "ConNMEAComport.h"
#include "Con_NMEA_TalkerID.h"
#include "Con_acquisition.h"
#include "GetGNSSEphemeris.h"
#include "SetGNSSEphemeris.h"
#include "GetTimeCorrection.h"
#include "SetTimeCorrections.h"
#include "WaitAckDlg.h"
#include "CompressDlg.h"

#include "SoftImDwDlg.h"
#include "ConPosPinning.h"
#include "BinaryMSG.h"
#include "Ephems.h"
#include "Serial.h"
#include "WaitReadLog.h"
#include "MaskedBitmap.h"
#include "Monitor_1PPS.h"
#include "SnrBarChart.h"
#include "Pic_Earth.h"
#include "Pic_Scatter.h"
#include "CigRgsDlg.h"
#include "GetRgsDlg.h"
#include "HostBaseDownloadDlg.h"
#include "FirmwareDownloadDlg.h"
#include "ParallelDownloadDlg.h"
#include "ExternalSrecDlg.h"

#include "Con1PPS_OutputMode.h"
//#include "Timing_start.h"
//#include "Con1PPS_PulseClkSource.h"
#include "NmeaChecksumCalDlg.h"
#include "BinaryChecksumCalDlg.h"
#include "PanelBackground.h"
#include "RawMeasmentOutputConvertDlg.h"
#include "VerifyFwDlg.h"

//extern Satellite satellites[MAX_SATELLITE];
//extern Satellite satellites_gnss[MAX_SATELLITE];
//extern Satellite satellites_bd[MAX_SATELLITE];
extern U08 slgsv;
#define UWM_KERNEL_REBOOT	(WM_USER + 0x134)
#define UWM_FIRST_NMEA		(WM_USER + 0x135)
#define UWM_SHOW_TIME		(WM_USER + 0x136)
#define UWM_UPDATE_UI		(WM_USER + 0x137)


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

ECEF_USER_PVT              ecef_user_pvt;
GEODETIC_USER_PVT          geod_user_pvt;
USER_SATELLITE_INFOMATION  sv_info;
SATELLITE_MEASUREMENT_DATA sm_data;

HANDLE  hScanGPS;
//HANDLE g_thread;
CWinThread* g_gpsThread = NULL;
U08 g_nmeaInputTerminate = 0;

//int CGPSDlg::Baudrate = 0;
CGPSDlg* CGPSDlg::gpsDlg = NULL;
UINT CGPSDlg::UWM_PLAYNMEA_EVENT = 0;
UINT CGPSDlg::UWM_SAVENMEA_EVENT = 0;
UINT CGPSDlg::UWM_UPDATE_EVENT = 0;
U08 CGPSDlg::m_inputMsg[200] = {0};
CFont CGPSDlg::m_textFont;
CFont CGPSDlg::m_infoFontS;
CFont CGPSDlg::m_infoFontL;
CFont CGPSDlg::comboFont;;	
CFont CGPSDlg::messageFont;

static void Log(CString f, int line, CString name = "", int data = 0)
{
	return;
	static char dbg_buf[64];
	sprintf_s(dbg_buf, "%s(%d) %s - %d\r\n", f, line, name, data);
	::OutputDebugString(dbg_buf);
}

static void Log2(CString f, int line, CString name = "", int data = 0)
{
	return;
	static char dbg_buf[64];
	sprintf_s(dbg_buf, "%s(%d) %s - %d\r\n", f, line, name, data);
	::OutputDebugString(dbg_buf);
}

void add2message(char* buffer, int offset)
{
	//if(CSaveNmea::IsNmeaFileOpen())
	//{			
	//	buffer[offset] = 13;
	//	buffer[offset+1] = 10;
	//	CGPSDlg::gpsDlg->m_nmeaFile.Write(buffer, offset + 2);
	//	CGPSDlg::gpsDlg->m_nmeaFileSize += offset;
	//	CGPSDlg::gpsDlg->m_saveNmeaDlg->DisplaySize(CGPSDlg::gpsDlg->m_nmeaFileSize);
	//}
	static char msg[1024];
	memcpy(msg, buffer, offset);
	msg[offset] = 0;
	CGPSDlg::gpsDlg->m_nmeaList.AddTextAsync(msg);
	if(offset > 2)
	{
		CSaveNmea::SaveText(buffer, offset);
		CSaveNmea::SaveText("\r\n", 2);
	}
}

void U32toBuff(U08 *buf,U32 v)
{
	buf[0] = (U08)(v>>24) &0xff;
	buf[1] = (U08)(v>>16) &0xff;
	buf[2] = (U08)(v>>8)  &0xff;
	buf[3] = (U08)(v)     &0xff;
}

void CGPSDlg::DeleteNmeaMemery()
{
	memset(&m_gpgllMsg, 0 ,sizeof(GPGLL));
	memset(&m_gpgllMsgCopy1, 0, sizeof(GPGLL));
	memset(&m_gpgllMsgCopy, 0, sizeof(GPGLL));

	memset(&m_gpgsaMsg, 0, sizeof(GPGSA));
	memset(&m_glgsaMsg, 0, sizeof(GPGSA));
	memset(&m_bdgsaMsg, 0, sizeof(GPGSA));
	memset(&m_gagsaMsg, 0, sizeof(GPGSA));

	memset(&m_gpgsaMsgCopy1, 0, sizeof(GPGSA));
	memset(&m_glgsaMsgCopy1, 0, sizeof(GPGSA));
	memset(&m_bdgsaMsgCopy1, 0, sizeof(GPGSA));
	memset(&m_gagsaMsgCopy1, 0, sizeof(GPGSA));

	memset(&m_gpgsaMsgCopy, 0, sizeof(GPGSA));
	memset(&m_glgsaMsgCopy, 0, sizeof(GPGSA));
	memset(&m_bdgsaMsgCopy, 0, sizeof(GPGSA));
	memset(&m_gagsaMsgCopy, 0, sizeof(GPGSA));

	memset(&m_gpggaMsg, 0, sizeof(GPGGA));
	memset(&m_gpggaMsgCopy1, 0, sizeof(GPGGA));
	memset(&m_gpggaMsgCopy, 0, sizeof(GPGGA));

	memset(&m_gpzdaMsg, 0, sizeof(GPZDA));
	memset(&m_gpzdaMsgCopy1, 0, sizeof(GPZDA));
	memset(&m_gpzdaMsgCopy, 0, sizeof(GPZDA));

	memset(&m_gprmcMsg, 0, sizeof(GPRMC));
	memset(&m_gprmcMsgCopy1, 0, sizeof(GPRMC));
	memset(&m_gprmcMsgCopy, 0, sizeof(GPRMC));

	memset(&m_gpvtgMsg, 0, sizeof(GPVTG));
	memset(&m_gpvtgMsgCopy1, 0, sizeof(GPVTG));
	memset(&m_gpvtgMsgCopy, 0, sizeof(GPVTG));

#if(_MODULE_SUP_800_)
	memset(&m_psti004001, 0, sizeof(PSTI004001));
	memset(&m_psti004001Copy1, 0, sizeof(PSTI004001));
	memset(&m_psti004001Copy, 0, sizeof(PSTI004001));
#endif

	memset(&m_gpgsvMsg, 0, sizeof(GPGSV));	
	memset(&m_glgsvMsg, 0, sizeof(GPGSV));	
	memset(&m_bdgsvMsg, 0, sizeof(GPGSV));	
	memset(&m_gagsvMsg, 0, sizeof(GPGSV));	

	memset(&m_gpgsvMsgCopy1, 0, sizeof(GPGSV));
	memset(&m_glgsvMsgCopy1, 0, sizeof(GPGSV));
	memset(&m_bdgsvMsgCopy1, 0, sizeof(GPGSV));
	memset(&m_gagsvMsgCopy1, 0, sizeof(GPGSV));

	memset(&m_gpgsvMsgCopy, 0, sizeof(GPGSV));
	memset(&m_glgsvMsgCopy, 0, sizeof(GPGSV));
	memset(&m_bdgsvMsgCopy, 0, sizeof(GPGSV));
	memset(&m_gagsvMsgCopy, 0, sizeof(GPGSV));

	memset(&satellites_gps, 0, sizeof(satellites_gps));
	memset(&satellites_gnss, 0, sizeof(satellites_gnss));
	memset(&satellites_bd, 0, sizeof(satellites_bd));
	memset(&satellites_ga, 0, sizeof(satellites_ga));

	memset(&satecopy_gps, 0, sizeof(satecopy_gps));
	memset(&satecopy_gnss, 0, sizeof(satecopy_gnss));
	memset(&satecopy_bd, 0, sizeof(satecopy_bd));
	memset(&satecopy_ga, 0, sizeof(satecopy_ga));

	csSatelliteStruct.Lock();
	memset(&sate_gps, 0, sizeof(sate_gps));
	memset(&sate_gnss, 0, sizeof(sate_gnss));
	memset(&sate_bd, 0, sizeof(sate_bd));
	memset(&sate_ga, 0, sizeof(sate_ga));
	csSatelliteStruct.Unlock();

#if GNSS_VIEWER
	ClearGlonass();
#endif

	m_versionInfo.Free();
	m_bootInfo.Free();
}

void CGPSDlg::Copy_NMEA_Memery()
{
	static bool copygsv = false;
	static bool copygsv_gnss = false;
	static bool copygsv_bd = false;
	static bool copygsv_ga = false;

	memcpy(&m_gpgllMsgCopy1, &m_gpgllMsgCopy, sizeof(GPGLL));

	memcpy(&m_gpgsaMsgCopy1, &m_gpgsaMsgCopy, sizeof(GPGSA));
	memcpy(&m_glgsaMsgCopy1, &m_glgsaMsgCopy, sizeof(GPGSA));
	memcpy(&m_bdgsaMsgCopy1, &m_bdgsaMsgCopy, sizeof(GPGSA));
	memcpy(&m_gagsaMsgCopy1, &m_gagsaMsgCopy, sizeof(GPGSA));

	memcpy(&m_gpggaMsgCopy1, &m_gpggaMsgCopy, sizeof(GPGGA));

	memcpy(&m_gpgsvMsgCopy1, &m_gpgsvMsgCopy, sizeof(GPGSV));
	memcpy(&m_glgsvMsgCopy1, &m_glgsvMsgCopy, sizeof(GPGSV));
	memcpy(&m_bdgsvMsgCopy1, &m_bdgsvMsgCopy, sizeof(GPGSV));
	memcpy(&m_gagsvMsgCopy1, &m_gagsvMsgCopy, sizeof(GPGSV));

	memcpy(&m_gpzdaMsgCopy1, &m_gpzdaMsgCopy, sizeof(GPZDA));
	memcpy(&m_gprmcMsgCopy1, &m_gprmcMsgCopy, sizeof(GPRMC));
	memcpy(&m_gpvtgMsgCopy1, &m_gpvtgMsgCopy, sizeof(GPVTG));

#if(_MODULE_SUP_800_)
	memcpy(&m_psti004001Copy, &m_psti004001, sizeof(PSTI004001));
#endif

	if(m_gpgsvMsgCopy.NumOfMessage && m_gpgsvMsgCopy.NumOfMessage == m_gpgsvMsgCopy.SequenceNum)
	{
		if(!copygsv || m_gpgsvMsgCopy.NumOfMessage == m_gpgsvMsgCopy.SequenceNum)
		{				
			memcpy(&satecopy_gps, &satellites_gps, sizeof(Satellite) * MAX_SATELLITE);
			copygsv = true;
		}
		else
		{
			copygsv = false;
		}
	}
	else	
	{
		copygsv = false;
	}

	if(m_glgsvMsgCopy.NumOfMessage && m_glgsvMsgCopy.NumOfMessage == m_glgsvMsgCopy.SequenceNum)
	{
		if(!copygsv_gnss || m_glgsvMsgCopy.NumOfMessage == m_glgsvMsgCopy.SequenceNum)
		{
			memcpy(&satecopy_gnss, &satellites_gnss, sizeof(Satellite)*MAX_SATELLITE);
			copygsv_gnss = true;
		}
		else
		{
			copygsv_gnss = false;
		}
	}
	else	
	{
		copygsv_gnss = false;
	}

	if(m_bdgsvMsgCopy.NumOfMessage && m_bdgsvMsgCopy.NumOfMessage == m_bdgsvMsgCopy.SequenceNum)
	{
		if(!copygsv_bd || m_bdgsvMsgCopy.NumOfMessage == m_bdgsvMsgCopy.SequenceNum)
		{
			memcpy(&satecopy_bd, &satellites_bd, sizeof(Satellite)*MAX_SATELLITE);
			copygsv_bd = true;
		}
		else
		{
			copygsv_bd = false;
		}
	}
	else	
	{
		copygsv_bd = false;
	}

	if(m_gagsvMsgCopy.NumOfMessage && m_gagsvMsgCopy.NumOfMessage == m_gagsvMsgCopy.SequenceNum)
	{
		if(!copygsv_ga || m_gagsvMsgCopy.NumOfMessage == m_gagsvMsgCopy.SequenceNum)
		{
			memcpy(&satecopy_ga, &satellites_ga, sizeof(Satellite)*MAX_SATELLITE);
			copygsv_ga = true;
		}
		else
		{
			copygsv_ga = false;
		}
	}
	else	
	{
		copygsv_ga = false;
	}

	csSatelliteStruct.Lock();
	if(copygsv)
	{
		memcpy(&sate_gps, &satecopy_gps, sizeof(Satellite) * MAX_SATELLITE);
	}
	if(copygsv_gnss)
	{
		memcpy(&sate_gnss, &satecopy_gnss, sizeof(Satellite) * MAX_SATELLITE);
	}
	if(copygsv_bd)
	{
		memcpy(&sate_bd, &satecopy_bd, sizeof(Satellite) * MAX_SATELLITE);
	}
	if(copygsv_ga)
	{
		memcpy(&sate_ga, &satecopy_ga, sizeof(Satellite) * MAX_SATELLITE);
	}	
	csSatelliteStruct.Unlock();

	if(copygsv || copygsv_gnss || copygsv_bd || copygsv_ga)
	{
		CopyNmeaToUse();
		PostMessage(UWM_UPDATE_UI, 0, 0);
	}
}

void CGPSDlg::CopyNmeaToUse()
{
	memcpy(&m_gpgllMsg, &m_gpgllMsgCopy1, sizeof(GPGLL));

	memcpy(&m_gpggaMsg, &m_gpggaMsgCopy1, sizeof(GPGGA));
	memcpy(&m_gpzdaMsg, &m_gpzdaMsgCopy1, sizeof(GPZDA));
	memcpy(&m_gprmcMsg, &m_gprmcMsgCopy1, sizeof(GPRMC));
	memcpy(&m_gpvtgMsg, &m_gpvtgMsgCopy1, sizeof(GPVTG));
#if(_MODULE_SUP_800_)
	memcpy(&m_psti004001, &m_psti004001Copy1, sizeof(PSTI004001));
#endif
	memcpy(&m_gpgsaMsg, &m_gpgsaMsgCopy1, sizeof(GPGSA));
	memcpy(&m_glgsaMsg, &m_glgsaMsgCopy1, sizeof(GPGSA));
	memcpy(&m_bdgsaMsg, &m_bdgsaMsgCopy1, sizeof(GPGSA));
	memcpy(&m_gagsaMsg, &m_gagsaMsgCopy1, sizeof(GPGSA));

	memcpy(&m_gpgsvMsg, &m_gpgsvMsgCopy1, sizeof(GPGSV));
	memcpy(&m_glgsvMsg, &m_glgsvMsgCopy1, sizeof(GPGSV));
	memcpy(&m_bdgsvMsg, &m_bdgsvMsgCopy1, sizeof(GPGSV));
	memcpy(&m_gagsvMsg, &m_gagsvMsgCopy1, sizeof(GPGSV));

//	csSatelliteStruct.Lock();
//	memcpy(&sate_gps, &satecopy_gps, sizeof(Satellite) * MAX_SATELLITE);
//	memcpy(&sate_gnss, &satecopy_gnss, sizeof(Satellite) * MAX_SATELLITE);
//	memcpy(&sate_bd, &satecopy_bd, sizeof(Satellite) * MAX_SATELLITE);
//	csSatelliteStruct.Unlock();

	//if(m_gpggaMsg.GPSQualityIndicator == 0x31 || m_gpggaMsg.GPSQualityIndicator == 0x32)
	if(::IsFixed(m_gpggaMsg.GPSQualityIndicator))
	{
		memcpy(&m_gpggaMsgBk, &m_gpggaMsgCopy1, sizeof(GPGGA));
	}
}

void CGPSDlg::CLEAR_NMEA_TO_USE()
{
	memset(&m_gpgllMsg, 0, sizeof(GPGLL));

	memset(&m_gpgsaMsg, 0, sizeof(GPGSA));
	memset(&m_glgsaMsg, 0, sizeof(GPGSA));
	memset(&m_bdgsaMsg, 0, sizeof(GPGSA));

	memset(&m_gpggaMsg, 0, sizeof(GPGGA));

	memset(&m_gpgsvMsg, 0, sizeof(GPGSV));
	memset(&m_glgsvMsg, 0, sizeof(GPGSV));
	memset(&m_bdgsvMsg, 0, sizeof(GPGSV));

	memset(&m_gpzdaMsg, 0, sizeof(GPZDA));
	memset(&m_gprmcMsg, 0, sizeof(GPRMC));
	memset(&m_gpvtgMsg, 0, sizeof(GPVTG));
#if(_MODULE_SUP_800_)
	memset(&m_psti004001, 0, sizeof(PSTI004001));
#endif
	csSatelliteStruct.Lock();
	memset(&sate_gps, 0, sizeof(Satellite) * MAX_SATELLITE);
	memset(&sate_gnss, 0, sizeof(Satellite) * MAX_SATELLITE);
	memset(&sate_bd, 0, sizeof(Satellite) * MAX_SATELLITE);
	csSatelliteStruct.Unlock();
}

bool CGPSDlg::NmeaProc(const char* buffer, int offset, NmeaType& nmeaType)
{	
	offset = NMEA::TrimTail(buffer, offset);
	nmeaType = NMEA::MessageType(buffer, offset);	
	NmeaOutput(buffer, offset);
	
	if(nmeaType==MSG_REBOOT)
	{
		PostMessage(UWM_FIRST_NMEA, 1000, 1);
	}

	NMEA nmea;	
	switch(nmeaType)
	{
	case MSG_Unknown:
		return false;
		break;
	case MSG_GLL:
		nmea.ShowGPGLLmsg(m_gpgllMsgCopy, buffer, offset);		
		break;
	case MSG_GLGSA:
		nmea.ShowGLGSAmsg(m_glgsaMsgCopy, buffer, offset);	
		break;
	case MSG_GNGSA:
		nmea.ShowGNGSAmsg(m_gpgsaMsgCopy, m_glgsaMsgCopy, m_bdgsaMsgCopy, m_gagsaMsgCopy, buffer, offset);	
		break;
	case MSG_BDGSA:
		nmea.ShowBDGSAmsg(m_bdgsaMsgCopy, buffer, offset);	
		break;
	case MSG_GAGSA:
		nmea.ShowGAGSAmsg(m_gagsaMsgCopy, buffer, offset);	
		break;
	case MSG_GPGSA:		
		nmea.ShowGNGSAmsg(m_gpgsaMsgCopy, m_glgsaMsgCopy, m_bdgsaMsgCopy, m_gagsaMsgCopy, buffer, offset);	
		break;
	case MSG_GGA:		
		nmea.ShowGPGGAmsg(m_gpggaMsgCopy, buffer, offset);
		PostMessage(UWM_SHOW_TIME, 0, 0);
		break; 
	case MSG_GNS:		
		nmea.ShowGNSmsg(m_gpggaMsgCopy, buffer, offset);
		break; 
	case MSG_ZDA:			
		nmea.ShowGPZDAmsg(m_gpzdaMsgCopy, buffer, offset);						
		break; 		
	case MSG_GPGSV:		
		nmea.ShowGPGSVmsg2(m_gpgsvMsgCopy, m_glgsvMsgCopy, m_bdgsvMsgCopy, m_gagsvMsgCopy, buffer, offset);	
		break; 	
	case MSG_RMC:		
		nmea.ShowGPRMCmsg(m_gprmcMsgCopy, buffer, offset);		
		PostMessage(UWM_SHOW_TIME, 0, 0);
		break; 	
	case MSG_VTG:				
		nmea.ShowGPVTGmsg(m_gpvtgMsgCopy ,buffer, offset);
		break;
	case MSG_STI:
		parse_sti_message(buffer, offset);
		break;
	case MSG_GLGSV:
		nmea.ShowGLGSVmsg(m_glgsvMsgCopy, buffer, offset);		
		break;
	case MSG_BDGSV:
		nmea.ShowBDGSVmsg(m_bdgsvMsgCopy, buffer, offset);		
		break;
	case MSG_GAGSV:
		nmea.ShowGAGSVmsg(m_gagsvMsgCopy, buffer, offset);		
		break;
	case MSG_REBOOT:
		this->DeleteNmeaMemery();		
		break;
	default:
		break;
	}

	if(nmea.GetFirstGsaIn())
	{
		return false;
	}
	return true;
}

void CGPSDlg::BinaryProc(U08* buffer, int len)
{
	if(!m_isConnectOn)
	{
		return;
	}
	if(len <= 7)
	{
		return;
	}

	int packetLen = MAKEWORD(buffer[3], buffer[2]);
	if(packetLen + 7 > len) 
	{
		return;
	}

	U08 msgType = Cal_Checksum(buffer);	
	switch(msgType)
	{
	case 0xDC:		// measurement time
		//Show_Message(buffer,len);
		ShowMeasurementTime(buffer);
		ShowTime();
		//NMEA_Output(message, strlen(message));
		break;
	case 0xDD:		// raw measurement
		ShowMeasurementChannel(buffer);
		//Show_Message(buffer,len);
		break;
	case 0xDE:		// SV_CH status
		ShowMeasurementSv(buffer);
		//Show_Message(buffer,len);
		break;
	case 0xDF:		// receiver state
		ShowReceiverNav(buffer);
		//Show_Message(buffer,len);
		break;
	case 0xE0:		// sub frame data
	case 0xE1:		// sub frame data
	case 0xE2:		// sub frame data
	case 0xE3:		// sub frame data
		//Show_Message(buffer,len);
		ShowSubframe(buffer);
		break;
	case BINMSG_ECEF_USER_PVT:
		ShowBinaryOutput(buffer);
		ShowTime();
		break;
	case BINMSG_ERROR:
		add_msgtolist("Unknown: " + theApp.GetHexString(buffer, len));	
		break;
	default:
		add_msgtolist("Unknown: " + theApp.GetHexString(buffer, len));	
		break;
	}
}

UINT ConnectGPS(LPVOID pParam)
{		
	UINT retCode = 0;	
	DWORD success;
	while(WaitForSingleObjectEx(g_connectEvent, 10, false) != WAIT_OBJECT_0);
	success = SetThreadPriority(GetCurrentThread(), ABOVE_NORMAL_PRIORITY_CLASS);
	CGPSDlg::gpsDlg->MSG_PROC();

	success = SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL );

	g_gpsThread = NULL;
	if(!SetEvent(g_closeEvent))	
	{
		DWORD error = GetLastError();
	}
	return retCode;
}

void PreprocessInputLine(char *buf, int bufLen)
{
	if(strlen(buf) < 12)
	{
		return;
	}
	CString str = buf;
	if(buf[0] != '$')
	{
		str.TrimRight();
	}

	if(buf[str.GetLength() - 2] != 0x0D && buf[str.GetLength() - 1] != 0x0A)
	{
		str.TrimLeft();
	}

	strcpy_s(buf, bufLen, str);
}

UINT NMEA_Parsing_Thread(LPVOID pParam)
{
	FILE *f = NULL;
	fopen_s(&f, (LPCSTR)pParam, "r");

	if(f==NULL)
	{
		return 0;
	}

	fseek(f, 0L, SEEK_END);
	long total = ftell(f);
	fseek(f, 0L, SEEK_SET);


	bool hasGGA = false;
	bool hasRMC = false;
	bool needSleep = false;
	char nmeaBuff[256] = {0};
	CGPSDlg::gpsDlg->SetTimer(SHOW_STATUS_TIMER, 1000, NULL);
	CGPSDlg::gpsDlg->m_isConnectOn = true;
	CGPSDlg::gpsDlg->SetInputMode(2);
	int lineCount = 0;
	g_nmeaInputTerminate = 0;
	DWORD startTick = ::GetTickCount();
	DWORD currentTick = 0;
	while(fgets(nmeaBuff, sizeof(nmeaBuff), f))
	{
		long current = ftell(f);

		PreprocessInputLine(nmeaBuff, 256);
		if(strncmp(nmeaBuff, "$GPGGA,", 7)==0 || 
		   strncmp(nmeaBuff, "$GNGGA,", 7)==0 || 
		   strncmp(nmeaBuff, "$BDGGA,", 7)==0)
		{
			if(!hasRMC)
			{
				hasGGA = true;
				needSleep = true;
			}		
		}
		if(strncmp(nmeaBuff, "$GPRMC,", 7)==0 || 
		   strncmp(nmeaBuff, "$GNRMC,", 7)==0 || 
		   strncmp(nmeaBuff, "$BDRMC,", 7)==0)
		{
			if(!hasGGA)
			{
				hasRMC = true;
				needSleep = true;
			}
		}

		currentTick = ::GetTickCount();
		if(needSleep)
		{
			CGPSDlg::gpsDlg->SetNmeaUpdated(true);
			const int MaxSleepMs = 50;
			int nDuration = 0;
			needSleep = false;
			do
			{
				CGPSDlg::gpsDlg->_nmeaPlayInterval.Lock();
				int nGap = CGPSDlg::gpsDlg->m_nmeaPlayInterval;
				CGPSDlg::gpsDlg->_nmeaPlayInterval.Unlock();

				if(currentTick < (startTick + nGap))
				{
					Sleep((startTick + nGap) - currentTick);
					startTick = ::GetTickCount();
				}
				if(g_nmeaInputTerminate) 
				{
					break;
				}
			} while(nDuration);
		}

		if(g_nmeaInputTerminate) 
		{
			break;
		}
		while(CGPSDlg::gpsDlg->m_nmeaPlayPause)
		{
			if(g_nmeaInputTerminate) 
			{
				break;
			}
			Sleep(50);
			startTick = ::GetTickCount();
		}

		CGPSDlg::gpsDlg->m_playNmea->SetLineCount(++lineCount, current, total);
		NmeaType nmeaType;
		if(lineCount == 138)
		{
			int a = 0;
		}
		if(CGPSDlg::gpsDlg->NmeaProc(nmeaBuff, strlen(nmeaBuff), nmeaType))
		{		
			CGPSDlg::gpsDlg->Copy_NMEA_Memery();		    
		}
		if(MSG_ERROR==nmeaType && g_setting.checkNmeaError)
		{
			CGPSDlg::gpsDlg->add_msgtolist("Detect NMEA checksum error :");
			CGPSDlg::gpsDlg->add_msgtolist(nmeaBuff);
		}

	}
	fclose(f);
	CGPSDlg::gpsDlg->m_isConnectOn = false;
	CGPSDlg::gpsDlg->KillTimer(SHOW_STATUS_TIMER);
	CGPSDlg::gpsDlg->SetInputMode(0);
	CGPSDlg::gpsDlg->SetNmeaUpdated(false);
	CGPSDlg::gpsDlg->m_nmeaPlayThread = NULL;
	CGPSDlg::gpsDlg->m_nmeaPlayPause = false; 
	return 0;
}

// CAboutDlg dialog used for App About
class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

	CStatic m_version;
	// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Implementation
protected:
	CxImage logoImg;

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX,IDC_ABOUT_VERSION,m_version);
	CDialog::DoDataExchange(pDX);
}

BOOL CAboutDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	CString strCaption;
	strCaption.Format("%s %s %s", APP_CAPTION, APP_VERSION, APP_TITLE);
	this->SetWindowText(strCaption);
	m_version.SetWindowText(strCaption);

	HRSRC hRsrc = FindResource(0, MAKEINTRESOURCE(IDB_ICON256), "PNG");	
	if(hRsrc)	
	{
		logoImg.LoadResource(hRsrc, CXIMAGE_FORMAT_PNG);
		FreeResource(hRsrc);
	}
	return TRUE;
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CAboutDlg::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	logoImg.Draw(dc, 0, 10, 160, 160);
}

// CGPSDlg dialog
CGPSDlg::CGPSDlg(CWnd* pParent /*=NULL*/)
: CDialog(CGPSDlg::IDD, pParent)
{
	dia_monitor_1pps = NULL;
	m_hIcon = AfxGetApp()->LoadIcon(IDI_MAINFRAME);
	m_serial = NULL;
	m_bShowBinaryCmdData = FALSE;
	gpsSnrBar = new CSnrBarChartGpsGlonass;
	//gnssSnrBar = new CSnrBarChartGlonass;
	bdSnrBar = new CSnrBarChartBeidou;
	gaSnrBar = new CSnrBarChartGalileo;
	pic_scatter = new CPic_Scatter;
	pic_earth = new CPic_Earth;
	m_noisePower = 0;

	m_DownloadMode = EnternalLoader;

	m_nDownloadBaudIdx = g_setting.boostBaudIndex;
	m_psoftImgDlDlg = NULL;
	m_infoPanel = new CPanelBackground;
	m_earthPanel = new CPanelBackground;
	m_scatterPanel = new CPanelBackground;
	m_downloadPanel = new CPanelBackground;
	m_firstDataIn = false;
	m_customerID = 0;
	m_saveNmeaDlg = NULL;
	m_playNmea = NULL;
	m_nmeaPlayPause = false;
	m_nmeaPlayThread = NULL;
	DeleteNmeaMemery();
	m_dataLogDecompressMode = 1;
	m_customerId = CUSTOMER_ID;
	usedEarth = g_setting.earthBitmap;
}

void CGPSDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TTFF, m_ttff);
	DDX_Control(pDX, IDC_DATE, m_date);
	DDX_Control(pDX, IDC_TIME, m_time);
	DDX_Control(pDX, IDC_BOOT_STATUS, m_bootStatus);
	DDX_Control(pDX, IDC_SW_VER, m_swKernel);
	DDX_Control(pDX, IDC_SW_REV, m_swRev);
	DDX_Control(pDX, IDC_LONGITUDE, m_longitude);
	DDX_Control(pDX, IDC_LATITUDE, m_latitude);
	DDX_Control(pDX, IDC_ALTITUDE, m_altitude);
	DDX_Control(pDX, IDC_SPEED, m_speed);
	DDX_Control(pDX, IDC_DIRECTION, m_direction);
	DDX_Control(pDX, IDC_PDOP, m_hdop);

	DDX_Control(pDX, IDC_COMPORT, m_ComPortCombo);
	DDX_Control(pDX, IDC_BAUDRATE_IDX, m_BaudRateCombo);	
	DDX_Control(pDX, IDC_COOR, m_coordinate);
	DDX_Control(pDX, IDC_ENUSCALE, m_scale);
	DDX_Control(pDX, IDC_MAPSCALE, m_mapscale);
	DDX_Control(pDX, IDC_2DRMS, m_twodrms);
	DDX_Control(pDX, IDC_CEP50, m_cep);	
	DDX_Control(pDX, IDC_RESPONSE, m_responseList);
	DDX_Control(pDX, IDC_MESSAGE, m_nmeaList);
	DDX_Control(pDX, IDC_OPEN_CLOSE_T, m_connectT);
	DDX_Control(pDX, IDC_NO_OUTPUT, m_no_output);
	DDX_Control(pDX, IDC_NMEA_OUTPUT, m_nmea0183msg);
	DDX_Control(pDX, IDC_BIN_OUTPUT, m_binarymsg);
	DDX_Control(pDX, IDC_FIRMWARE_PATH, m_lbl_firmware_path);
	DDX_Control(pDX, IDC_INFO_PANEL, *m_infoPanel);
	DDX_Control(pDX, IDC_EARTH_PANEL, *m_earthPanel);
	DDX_Control(pDX, IDC_SCATTER_PANEL, *m_scatterPanel);
	DDX_Control(pDX, IDC_DOWNLOAD_PANEL, *m_downloadPanel);

	DDX_Control(pDX,IDC_ROM_MODE,m_rom_mode);
	//#if FIRMWARE_DOWNLOAD
//	DDX_Control(pDX, IDC_DOWNLOAD_T, m_lbl_download);
	DDX_Control(pDX, IDC_BROWSE, m_btn_browse);

	DDX_Control(pDX, IDC_WARMSTART, m_bnt_warmstart);
	DDX_Control(pDX, IDC_COLDSTART, m_btn_coldstart);
//	DDX_Control(pDX, IDC_AGPS_ENABLE, m_agps_enable);
//	DDX_Control(pDX, IDC_AGPS_DISABLE, m_agps_disable);	

//	DDX_Control(pDX, IDC_NOISE_T, lbl_noise);	
//	DDX_Control(pDX, IDC_CLOCK_T, lbl_clock);	
	DDX_Control(pDX, IDC_NOISE, m_noise);	
	DDX_Control(pDX, IDC_CLOCK, m_clock_offset);	
	DDX_Control(pDX, IDC_KNUM_LIST,m_kNumList);
	
	DDX_Control(pDX, IDC_WGS84_X, m_wgs84_x);	
	DDX_Control(pDX, IDC_WGS84_Y, m_wgs84_y);	
	DDX_Control(pDX, IDC_WGS84_Z, m_wgs84_z);

	DDX_Control(pDX, IDC_ENU_E, m_enu_e);	
	DDX_Control(pDX, IDC_ENU_N, m_enu_n);	
	DDX_Control(pDX, IDC_ENU_U, m_enu_u);	

	DDX_Control(pDX, IDC_GPS_BAR, *gpsSnrBar);	
	//DDX_Control(pDX, IDC_GNSS_BAR, *gnssSnrBar);	
	DDX_Control(pDX, IDC_BD_BAR, *bdSnrBar);	
	DDX_Control(pDX, IDC_GA_BAR, *gaSnrBar);	
	DDX_Control(pDX, IDC_EARTH, *pic_earth);	
	DDX_Control(pDX, IDC_SCATTER, *pic_scatter);	
	DDX_Control(pDX, IDC_FIXED_STATUS, m_fixed_status);	

	DDX_Control(pDX, IDC_ODO_METER, m_odo_meter);	
	DDX_Control(pDX, IDC_GYRO_DATA, m_gyro_data);	
	DDX_Control(pDX, IDC_BK_INDICATOR, m_backward_indicator);	

	DDX_Control(pDX, IDC_ODO_METER_T, m_lbl_odo_meter);	
	DDX_Control(pDX, IDC_GYRO_DATA_T, m_lbl_gyro_data);	
	DDX_Control(pDX, IDC_BK_INDICATOR_T, m_lbl_backward_indicator);	

	//DDX_Control(pDX,IDC_SETORIGIN_USER,m_setorigin_user);
	//#endif	
}

BEGIN_MESSAGE_MAP(CGPSDlg, CDialog)
	ON_WM_CLOSE()
	ON_WM_DRAWITEM()
	ON_WM_INITMENUPOPUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SYSCOMMAND()
	ON_WM_TIMER()

	ON_BN_CLICKED(IDC_BROWSE, OnBnClickedBrowse)
	ON_BN_CLICKED(IDC_NO_OUTPUT, OnBnClickedNoOutput)
	ON_BN_CLICKED(IDC_NMEA_OUTPUT, OnBnClickedNmeaOutput)
	ON_BN_CLICKED(IDC_BIN_OUTPUT, OnBnClickedBinaryOutput)
	ON_BN_CLICKED(IDC_CLEAR, OnBnClickedClear)
	ON_BN_CLICKED(IDC_CLOSE, OnBnClickedClose)
	ON_BN_CLICKED(IDC_STOP, OnBnClickedStop)
	ON_BN_CLICKED(IDC_COLDSTART, OnBnClickedColdstart)
	ON_BN_CLICKED(IDC_CONNECT, OnBnClickedConnect)
	ON_BN_CLICKED(IDC_PLAY, OnBnClickedPlay)
	ON_BN_CLICKED(IDC_RECORD, OnBnClickedRecord)
	ON_BN_CLICKED(IDC_DOWNLOAD, OnBnClickedDownload)
	ON_BN_CLICKED(IDC_ECOM_CALIB, OnBnClickedECompassCalibration)
	ON_BN_CLICKED(IDC_HOTSTART, OnBnClickedHotstart)
	ON_BN_CLICKED(IDC_KNUM_DISABLE, &CGPSDlg::OnBnClickedKnumDisable)
	ON_BN_CLICKED(IDC_KNUM_ENABLE, &CGPSDlg::OnBnClickedKnumEnable)
	ON_BN_CLICKED(IDC_ROM_MODE, &CGPSDlg::OnBnClickedRomMode)
	ON_BN_CLICKED(IDC_SCANALL, OnBnClickedScanall)
	ON_BN_CLICKED(IDC_SCANBAUDRATE, OnBnClickedScanbaudrate)
	ON_BN_CLICKED(IDC_SCANPORT, OnBnClickedScanport)
	ON_BN_CLICKED(IDC_SETORIGIN, OnBnClickedSetorigin)	
//	ON_BN_CLICKED(IDC_SETORIGIN_USER, OnBnClickedSetoriginUser)
	ON_BN_CLICKED(IDC_WARMSTART, OnBnClickedWarmstart)

	ON_CBN_CLOSEUP(IDC_COOR, OnCbnCloseupCoordinate)
	ON_CBN_CLOSEUP(IDC_ENUSCALE, OnCbnCloseupEnuscale)
	ON_CBN_CLOSEUP(IDC_MAPSCALE, OnCbnCloseupMapscale)

	ON_COMMAND(ID_CONFIG_1PPS_FREQ_OUTPUT, &CGPSDlg::OnConfig1ppsFrequencyOutput)
//	ON_COMMAND(ID_1PPSTIMING_CONFIGUREPPSCLKSOURCE, &CGPSDlg::On1ppstimingConfigureppspulseclksrc)
	ON_COMMAND(ID_1PPSTIMING_CONFIGUREPPSOUTPUTMODE, &CGPSDlg::On1ppstimingConfigureppsoutputmode)
	ON_COMMAND(ID_1PPSTIMING_CONFIGUREPROPRIETARYNMEA, On1ppstimingConfigureproprietarynmea)
	ON_COMMAND(ID_1PPSTIMING_ENTERREFERENCEPOSITION32977, &CGPSDlg::On1ppstimingEnterreferenceposition32977)
//	ON_COMMAND(ID_1PPSTIMING_ENTERREFERENCEPOSITION32961, &CGPSDlg::On1ppstimingEnterreferenceposition32961)
	ON_COMMAND(ID_1PPSTIMING_MONITORING1PPS, On1ppstimingMonitoring1pps)
	ON_COMMAND(ID_1PPSTIMING_QUERYPPSCLKSOURCE, &CGPSDlg::On1ppstimingQueryppspulseclksrc)
	ON_COMMAND(ID_1PPSTIMING_QUERYPPSOUTPUTMODE, &CGPSDlg::On1ppstimingQueryppsoutputmode)
	ON_COMMAND(ID_AGPS_CONFIG, OnAgpsConfig)
//	ON_COMMAND(ID_AGPS_FTP, OnAgpsFtp)
	ON_COMMAND(ID_BINARY_CONFIGURE1PPS, OnBinaryConfigure1pps)
	ON_COMMAND(ID_BINARY_CONFIGUREBINARYDATA, OnBinaryConfigurebinarydata)
	ON_COMMAND(ID_BINARY_CONFIGUREBINARYINTERVAL, OnBinaryConfigureBinaryInterval)
	ON_COMMAND(ID_BINARY_CONFIGUREDATUM, OnBinaryConfiguredatum)
	ON_COMMAND(ID_BINARY_CONFIGUREDOPMASK, OnBinaryConfiguredopmask)
	ON_COMMAND(ID_BINARY_CONFIGUREELEVATIONMASK, OnBinaryConfigureelevationmask)
//	ON_COMMAND(ID_CFG_GL_ACQUISITION_MODE, &CGPSDlg::OnCfgGlonassAcquisitionMode)
//	ON_COMMAND(ID_CONFIG_GNSS_SEL_FOR_NAV, &CGPSDlg::OnBinaryConfiguregnssselectionfornavigationsystem)

	ON_COMMAND(ID_BINARY_CONFIGUREMESSAGETYPE, OnBinaryConfiguremessagetype)
	ON_COMMAND(ID_BINARY_CONFIGUREMESSAGE_TYPE, OnBinaryConfiguremessageType)
	ON_COMMAND(ID_BINARY_CONFIGUREMULTIPATH, OnBinaryConfiguremultipath)
	ON_COMMAND(ID_BINARY_CONFIGURENMEAOUTPUT, OnBinaryConfigurenmeaoutput)
	ON_COMMAND(ID_CONFIG_NMEA_INTERVAL_V8, OnConfigureNmeaIntervalV8)
	ON_COMMAND(ID_CONFIG_ERICSSON_STC_ITV, OnConfigureEricssonSentecneInterval)

	ON_COMMAND(ID_BINARY_CONFIGURENMEAOUTPUT32953, &CGPSDlg::OnBinaryConfigurenmeaoutput32953)
	ON_COMMAND(ID_BINARY_CONFIGURENMEATALKERID, &CGPSDlg::OnBinaryConfigurenmeatalkerid)
	ON_COMMAND(ID_BINARY_CONFIGUREPINNINGPARAMETERS, OnBinaryConfigurepinningparameters)
	ON_COMMAND(ID_BINARY_CONFIGUREPOSITIONPINNING, OnBinaryConfigurepositionpinning)
	ON_COMMAND(ID_BINARY_CONFIGUREPOSITIONRATE, OnBinaryConfigurepositionrate)
	ON_COMMAND(ID_CONFIG_DR_MULTIHZ, OnBinaryConfigDrMultiHz)
	ON_COMMAND(ID_BINARY_CONFIGUREPOWERMODE, OnBinaryConfigurepowermode)
	ON_COMMAND(ID_BINARY_CONFIGUREPOWERSAVINGPARAMETERS, &CGPSDlg::OnConfigPowerSavingParameters)
	ON_COMMAND(ID_CONFIG_V8_POWER_SV_PARAM_ROM, &CGPSDlg::OnConfigPowerSavingParametersRom)
	ON_COMMAND(ID_CONFIG_PROPRIETARY_MESSAGE, &CGPSDlg::OnConfigProprietaryMessage)
	ON_COMMAND(ID_CONFIG_DOZE_MODE, &CGPSDlg::OnConfigGnssDozeMode)

	ON_COMMAND(ID_BINARY_CONFIGUREREGISTER, OnBinaryConfigureregister)
	ON_COMMAND(ID_CONFIGURE_SERIAL_PORT, OnConfigureSerialPort)
	ON_COMMAND(ID_BINARY_CONFIGURESUBSECREGISTER, OnBinaryConfiguresubsecregister)
	ON_COMMAND(ID_BINARY_DUMPDATA, OnBinaryDumpData)
	ON_COMMAND(ID_BINARY_GETRGISTER, OnBinaryGetrgister)
	ON_COMMAND(ID_BINARY_POSITIONFINDER, OnBinaryPositionfinder)

	ON_COMMAND(ID_BINARY_RESETODOMETER, OnBinaryResetodometer)
	ON_COMMAND(ID_BINARY_SYSTEMRESTART, OnBinarySystemRestart)
	ON_COMMAND(ID_CONFIGURE1PPSTIMING_CONFIGURE1PPS, OnConfigure1ppstimingConfigure1pps)
	ON_COMMAND(ID_CONFIGURE1PPSTIMING_CONFIGURE1PPSCABLEDELAY, OnConfigure1ppstimingConfigure1ppscabledelay)
	ON_COMMAND(ID_CONFIGURE1PPSTIMING_CONFIGURE1PPSTIMING, OnConfigure1ppstimingConfigure1ppstiming)
	ON_COMMAND(ID_CONFIG_ELEV_AND_CNR_MASK, OnConfigElevationAndCnrMask)
	ON_COMMAND(ID_CONFIGUREOUTPUTMESSAGETYPE_BINARYMESSAGE, OnConfigureoutputmessagetypeBinarymessage)
	ON_COMMAND(ID_CONFIGUREOUTPUTMESSAGETYPE_NMEAMESSAGE, OnConfigureoutputmessagetypeNmeamessage)
	ON_COMMAND(ID_CONFIGUREOUTPUTMESSAGETYPE_NOOUTPUT, OnConfigureoutputmessagetypeNooutput)
	ON_COMMAND(ID_CONVERTER_COMPRESS, OnConverterCompress)
	ON_COMMAND(ID_CONVERTER_DECOMPRESS, OnCovDecopre)
	ON_COMMAND(ID_CONVERTER_KML, OnConverterKml)
	ON_COMMAND(ID_RAW_MEAS_OUT_CONVERT, OnRawMeasurementOutputConvert)
//	ON_COMMAND(ID_DATALOG_CLEARLOGINPASSWORD, &CGPSDlg::OnDatalogClearloginpassword)
	ON_COMMAND(ID_DATALOG_LOGCLEARCONTROL, OnDatalogClearControl)
	ON_COMMAND(ID_DATALOG_LOGCONFIGURECONTROL, OnDatalogLogconfigurecontrol)
	ON_COMMAND(ID_NMEA_CHECKSUM_CAL, OnNmeaChecksumCalculator)
	ON_COMMAND(ID_BIN_CHECKSUM_CAL, OnBinaryChecksumCalculator)
	ON_COMMAND(ID_TEST_EXTERNAL_SREC, OnTestExternalSrec)
	ON_COMMAND(ID_DATALOG_LOGREADBATCH, OnDatalogLogReadBatch)
//	ON_COMMAND(ID_DATALOG_LOGREADCONTROL, OnDatalogLogreadcontrol)	
//	ON_COMMAND(ID_DATALOG_LOGREADSREC, OnDatalogLogreadsrec)
//	ON_COMMAND(ID_DOWNLOAD_DOWNLOAD, OnDownloadDownload)
//	ON_COMMAND(ID_DOWNLOAD_ROMWRITE, OnDownloadRomwrite)
	ON_COMMAND(ID_EPHEMERIS_GETALMANAC, OnEphemerisGetalmanac)
	ON_COMMAND(ID_EPHEMERIS_GETEPHEMERIS, OnEphemerisGetephemeris)
	ON_COMMAND(ID_EPHEMERIS_GETGLONASSALMANAC, &CGPSDlg::OnEphemerisGetglonassalmanac)
	ON_COMMAND(ID_EPHEMERIS_GETGPSGLONASS, &CGPSDlg::OnEphemerisGetgpsglonass)
	ON_COMMAND(ID_EPHEMERIS_GETGPSGLONASSALMANAC, &CGPSDlg::OnEphemerisGetgpsglonassalmanac)
	ON_COMMAND(ID_EPHEMERIS_GETTIMECORRECTIONS, &CGPSDlg::OnEphemerisGettimecorrections)
	ON_COMMAND(ID_EPHEMERIS_SETALMANAC, OnEphemerisSetalmanac)
	ON_COMMAND(ID_EPHEMERIS_SETEPHEMERIS, OnEphemerisSetephemeris)
	ON_COMMAND(ID_EPHEMERIS_SETGLONASSALMANAC, &CGPSDlg::OnEphemerisSetglonassalmanac)
	ON_COMMAND(ID_EPHEMERIS_SETGPSGLONASS, &CGPSDlg::OnEphemerisSetgpsglonass)
	ON_COMMAND(ID_EPHEMERIS_SETGPSGLONASS_ALMANAC, &CGPSDlg::OnEphemerisSetgpsglonassAlmanac)
	ON_COMMAND(ID_EPHEMERIS_SETTIMECORRECTIONS, &CGPSDlg::OnEphemerisSettimecorrections)
	ON_COMMAND(ID_FILE_CLEANNEMA, OnFileCleannema)
	ON_COMMAND(ID_FILE_SETUP, OnFileSetup)
	ON_COMMAND(ID_FILE_EXIT, OnFileExit)
	ON_COMMAND(ID_FILE_SAVENMEA, OnFileSaveNmea)
	ON_COMMAND(ID_FILE_BINARY, OnFileSaveBinary)
	ON_COMMAND(ID_VERIFY_FIRMWARE, OnVerifyFirmware)
	ON_COMMAND(ID_FILE_PLAYNMEA, OnFilePlayNmea)
	ON_COMMAND(ID_GLONASS_CONFIGUREUSBDRIVER, &CGPSDlg::OnGlonassConfigureusbdriver)
	ON_COMMAND(ID_HELP_ABOUT, OnHelpAbout)
	ON_COMMAND(ID_LOGGER_CONVERT, OnLoggerConvert)
	ON_COMMAND(ID_MINIHOMER_ACTIVATE, OnMinihomerActivate)
	ON_COMMAND(ID_MINIHOMER_QUERYTAG, OnMinihomerQuerytag)
	ON_COMMAND(ID_MINIHOMER_SETTAGECCO, OnMinihomerSettagecco)
	ON_COMMAND(ID_MULTIMODE_CONFIGUREMODE, OnMultimodeConfiguremode)
	ON_COMMAND(ID_QUERY1PPSTIMING_QUERYCABLEDELAY, OnQuery1ppstimingQuerycabledelay)

	ON_COMMAND(ID_SETFACTORYDEFAULT_NOREBOOT, OnSetFactoryDefaultNoReboot)
	ON_COMMAND(ID_SETFACTORYDEFAULT_REBOOT, OnSetFactoryDefaultReboot)
//	ON_COMMAND(ID_SOARCOMM_CONFIGPASSWORD, OnSoarcommConfigpassword)
//	ON_COMMAND(ID_SOARCOMM_LOGIN, OnSoarcommLogin)
//	ON_COMMAND(ID_SOARCOMM_LOGOUT, OnSoarcommLogout)
//	ON_COMMAND(ID_SOFTWAREIMAGEDOWNLOAD_LOADERIMAGE, OnSoftwareimagedownloadLoaderimage)
	ON_COMMAND(ID_WAAS_WAAS, OnWaasWaas)
	
	ON_MESSAGE(WM_DEVICECHANGE, OnMyDeviceChange)
	ON_MESSAGE(UWM_KERNEL_REBOOT, OnKernelReboot)
	ON_MESSAGE(UWM_FIRST_NMEA, OnFirstNmea)
	ON_MESSAGE(UWM_SHOW_TIME, OnShowTime)
	ON_MESSAGE(UWM_UPDATE_UI, OnUpdateUI)
//	ON_UPDATE_COMMAND_UI(ID_DOWNLOAD_ROMWRITE, OnUpdateDownloadRomwrite)


	ON_COMMAND(ID_BINARY_CONFIGURESAGPS, OnBinaryConfigureSAGPS)
	ON_COMMAND(ID_BINARY_CONFIGURESBAS, OnBinaryConfigureSBAS)
	ON_COMMAND(ID_BINARY_CONFIGUREQZSS, OnBinaryConfigureQZSS)
	ON_COMMAND(ID_BINARY_CONFIG_DGPS, OnBinaryConfigureDGPS)
	ON_COMMAND(ID_BINARY_CONFIG_SMOOTH_MODE, OnBinaryConfigureSmoothMode)
	ON_COMMAND(ID_BINARY_CONFIG_TIME_STAMPING, OnBinaryConfigTimeStamping)
	ON_COMMAND(ID_CONFIG_LEAP_SECONDS, OnConfigLeapSeconds)
	ON_COMMAND(ID_CONFIG_PARAM_SRCH_ENG_SLP_CRT, OnConfigParamSearchEngineSleepCriteria)
	ON_COMMAND(ID_CONFIG_DATUM_INDEX, OnConfigDatumIndex)
	ON_COMMAND(ID_BINARY_CONFIGURE_NOISE_PW_CTL, OnConfigureNoisePowerControl)
	ON_COMMAND(ID_BINARY_CONFIGURE_ITF_DET_CTL, OnConfigureInterferenceDetectControl)
	ON_COMMAND(ID_BINARY_CONFIGURE_NMBI_OUT_DES, OnConfigNMEABinaryOutputDestination)
	ON_COMMAND(ID_CONFIGURE_PARAM_SEARCH_ENG_NUM, OnConfigParameterSearchEngineNumber)
	ON_COMMAND(ID_CONFIGURE_ANTENNA_DETECTION, &CGPSDlg::OnBinaryConfigureantennadetection)
	ON_COMMAND(ID_AGPS_FTP_SREC, OnAgpsFtpSrec)
	ON_COMMAND(ID_ROMAGPS_FTP_SREC, OnRomAgpsFtpSrec)
	ON_COMMAND(ID_ROMAGPS_FTP_NEW, OnRomAgpsFtpNew)
	ON_COMMAND(ID_CLOCK_OFFSET_PREDICT, OnClockOffsetPredict)
	ON_COMMAND(ID_CLOCK_OFFSET_PREDICT_OLD, OnClockOffsetPredictOld)
	ON_COMMAND(ID_HOSTBASED_DOWNLOAD, OnHostBasedDownload)
	ON_COMMAND(ID_FIRMWARE_DOWNLOAD, OnFiremareDownload)
	ON_COMMAND(ID_CONFIGURE_POS_FIX_NAV_MASK, OnConfigPositionFixNavigationMask)
	ON_COMMAND(ID_PARALLEL_DOWNLOAD, OnParallelDownload)
	ON_COMMAND(ID_CONFIG_REF_TIME_TO_GPS, OnConfigRefTimeSyncToGpsTime)
	ON_COMMAND(ID_1PPSTIMING_CONFIGURE1PPSPULSEWIDTH, On1ppstimingConfigurePulseWidth)
	ON_COMMAND(ID_1PPSTIMING_QUERY1PPSPULSEWIDTH, On1ppsTimingQuery1ppsPulseWidth)
	ON_COMMAND(ID_CONFIG_GNSS_NAV_SOL, OnConfigQueryGnssNavSol)
	ON_COMMAND(ID_CONFIG_BIN_MEA_DAT_OUT, OnConfigBinaryMeasurementDataOut)
	ON_COMMAND(ID_QUERY_BIN_MEA_DAT_OUT, OnQueryBinaryMeasurementDataOut)

	//New type
	ON_COMMAND(ID_BINARY_QUERYPOSITIONRATE, OnQueryPositionRate)
	ON_COMMAND(ID_BINARY_QUERYDATUM, OnQueryDatum)
	ON_COMMAND(ID_QUERYSOFTWAREVERSION_ROMCODE, OnQuerySoftwareVersionRomCode)
	ON_COMMAND(ID_QUERY_SHA1, OnQuerySha1String)
	ON_COMMAND(ID_QUERY_CON_CAP, OnQueryConstellationCapability)
	ON_COMMAND(ID_QUERYSOFTWAREVERSION_SYSTEMCODE, OnQuerySoftwareVersionSystemCode)
	ON_COMMAND(ID_QUERYSOFTWARECRC_ROMCODE, OnQuerySoftwareCrcRomCode)
	ON_COMMAND(ID_QUERYSOFTWARECRC_SYSTEMCODE, OnQuerySoftwareCrcSystemCode)
	ON_COMMAND(ID_WAAS_WAASSTATUS, OnQueryWaasStatus)
	ON_COMMAND(ID_BINARY_QUERYPOSITIONPINNING, OnQueryPositionPinning)
	ON_COMMAND(ID_BINARY_QUERY1PPS, OnQuery1ppsMode)
	ON_COMMAND(ID_BINARY_QUERYPOWERMODE, OnQueryPowerMode)
	ON_COMMAND(ID_BINARY_QUERYPOWERSAVINGPARAMETERS, OnQueryPowerSavingParameters)
	ON_COMMAND(ID_QUERY_V8_POWER_SV_PARAM, OnQueryV8PowerSavingParameters)
	ON_COMMAND(ID_QUERY_V8_POWER_SV_PARAM_ROM, OnQueryV8PowerSavingParametersRom)
	ON_COMMAND(ID_QUERY_1PPS_FREQ_OUTPUT, OnQuery1ppsFreqencyOutput)
	ON_COMMAND(ID_BINARY_QUERYPROPRIETARYMESSAGE, OnQueryProprietaryMessage)
	ON_COMMAND(ID_QUERY1PPSTIMING_QUERYTIMING, OnQueryTiming) 
	ON_COMMAND(ID_QUERY1PPSTIMING_QUERY, OnQueryDopMask)
	ON_COMMAND(ID_QUERY1PPSTIMING_QUERYELEVATIONANDCNRMASK, OnQueryElevationAndCnrMask)
	ON_COMMAND(ID_BINARY_QUERYANTENNADETECTION, OnQueryAntennaDetection)
	ON_COMMAND(ID_BINARY_QUERYNOISEPOWER, OnQueryNoisePower)
	ON_COMMAND(ID_BINARY_QUERYDRINFO, OnQueryDrInfo)		//Not test
	ON_COMMAND(ID_BINARY_QUERYDRHWPARAMETER, OnQueryDrHwParameter)	//Not test
	//ON_COMMAND(ID_QUERY_GNSS_SEL_FOR_NAV, OnQueryGnssSelectionForNavigationSystem)//Not test
	ON_COMMAND(ID_QUERY_GNSS_SEL_FOR_NAV, OnQueryGnssSelectionForNavigationSystem2)//Not test
	//ON_COMMAND(ID_BINARY_QUERYGNSSKNUMBERSLOTCNR, OnQueryGnssKnumberSlotCnr)//Not test
	ON_COMMAND(ID_BINARY_QUERYNMEATALKERID, OnQueryGnssNmeaTalkId)
	ON_COMMAND(ID_BINARY_QUERYGNSSKNUMBERSLOTCNR, OnQueryGnssKnumberSlotCnr2)//Not test
	ON_COMMAND(ID_BINARY_QUERYSBAS, OnQuerySbas)
	ON_COMMAND(ID_BINARY_QUERYSAGPS, OnQuerySagps)
	ON_COMMAND(ID_BINARY_QUERYQZSS, OnQueryQzss)
	ON_COMMAND(ID_BINARY_QUERY_DGPS, OnQueryDgps)
	ON_COMMAND(ID_BINARY_QUERY_SMOOTH_MODE, OnQuerySmoothMode)
	ON_COMMAND(ID_BINARY_QUERY_TIME_STAMPING, OnQueryTimeStamping)
	ON_COMMAND(ID_QUERY_GPS_TIME, OnQueryGpsTime)
	ON_COMMAND(ID_BINARY_QUERY_NOISE_PW_CTL, OnQueryNoisePowerControl)
	ON_COMMAND(ID_BINARY_QUERY_ITF_DET_CTL, OnQueryInterferenceDetectControl)
	ON_COMMAND(ID_BINARY_QUERY_NMBI_OUT_DES, OnQueryNmeaBinaryOutputDestination)
	ON_COMMAND(ID_BINARY_QUERY_PARAM_SEARCH_ENG_NUM, OnQueryParameterSearchEngineNumber)
	ON_COMMAND(ID_AGPS_STATUS, OnQueryAgpsStatus)
	ON_COMMAND(ID_DATALOG_LOGSTATUSCONTROL, OnQueryDatalogLogStatus)
	ON_COMMAND(ID_QUERY_POS_FIX_NAV_MASK, OnQueryPositionFixNavigationMask)
	ON_COMMAND(ID_MULTIMODE_QUERYMODE, OnQueryNavigationMode)
	ON_COMMAND(ID_QUERY_NMEA_INTERVAL_V8, OnQueryNmeaIntervalV8)
	ON_COMMAND(ID_QUERY_ERICSSON_STC_ITV, OnQueryEricssonInterval)
	ON_COMMAND(ID_QUERY_REF_TIME_TO_GPS, OnQueryRefTimeSyncToGpsTime)
	ON_COMMAND(ID_QUERY_PARAM_SRCH_ENG_SLP_CRT, OnQuerySearchEngineSleepCriteria)
	ON_COMMAND(ID_QUERY_NAV_MODE_V8, OnQueryNavigationModeV8)
	ON_COMMAND(ID_QUERY_BOOT_STATUS, OnQueryGnssBootStatus)
	ON_COMMAND(ID_QUERY_DR_MULTIHZ, OnQueryDrMultiHz)
	ON_COMMAND(ID_QUERY_GNSS_NAV_SOL, OnQueryGnssNavSol)
	ON_COMMAND(ID_QUERY_CUSTOMER_ID, OnQueryCustomerID)
	ON_COMMAND(ID_QUERY_SERIAL_NUMBER, OnQuerySerialNumber)
	ON_COMMAND(ID_CONFIG_SERIAL_NUMBER, OnConfigureSerialNumber)
	ON_COMMAND(ID_QUERY_DATUM_INDEX, OnQueryDatumIndex)
	ON_COMMAND(ID_QUERY_UARTPASS, OnQueryUartPass)
	ON_COMMAND(ID_GPSDO_RESET_SLAVE, OnGpsdoResetSlave)
	ON_COMMAND(ID_GPSDO_ENTER_ROM, OnGpsdoEnterRom)
	ON_COMMAND(ID_GPSDO_LEAVE_ROM, OnGpsdoLeaveRom)
	ON_COMMAND(ID_GPSDO_ENTER_DWN, OnGpsdoEnterDownload)
	ON_COMMAND(ID_GPSDO_LEAVE_DWN, OnGpsdoLeaveDownload)
	ON_COMMAND(ID_GPSDO_ENTER_UART, OnGpsdoEnterUart)
	ON_COMMAND(ID_GPSDO_LEAVE_UART, OnGpsdoLeaveUart)

	ON_COMMAND(ID_SUP800_ERASE_DATA, OnSup800EraseData)	
	ON_COMMAND(ID_SUP800_WRITE_DATA, OnSup800WriteData)	
	ON_COMMAND(ID_SUP800_READ_DATA, OnSup800ReadData)	

	ON_COMMAND(ID_QUERY_SIG_DISTUR_DATA, OnQuerySignalDisturbanceData)	
	ON_COMMAND(ID_QUERY_SIG_DISTUR_STATUS, OnQuerySignalDisturbanceStatus)	
	ON_COMMAND(ID_CONFIG_SIG_DISTUR_STATUS, OnConfigureSignalDisturbanceStatus)	

	ON_REGISTERED_MESSAGE(UWM_PLAYNMEA_EVENT, OnPlayNmeaEvent)
	ON_REGISTERED_MESSAGE(UWM_SAVENMEA_EVENT, OnSaveNmeaEvent)
	ON_REGISTERED_MESSAGE(UWM_UPDATE_EVENT, OnUpdateEvent)

	ON_COMMAND(ID_GET_GLONASS_EPHEMERIS, &CGPSDlg::OnGetGlonassEphemeris)
	ON_COMMAND(ID_SET_GLONASS_EPHEMERIS, &CGPSDlg::OnSetGlonassEphemeris)
	ON_COMMAND(ID_GET_BEIDOU_EPHEMERIS, &CGPSDlg::OnGetBeidouEphemeris)
	ON_COMMAND(ID_SET_BEIDOU_EPHEMERIS, &CGPSDlg::OnSetBeidouEphemeris)

END_MESSAGE_MAP()

// CGPSDlg message handlers
BOOL CGPSDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if(pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if(!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	NMEA::gnssData.SetNotify(this->GetSafeHwnd());
	Initialization();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

CGPSDlg::~CGPSDlg()
{
	Sleep(10);

	delete gpsSnrBar;
	//delete gnssSnrBar;
	delete bdSnrBar;
	delete gaSnrBar;
	delete pic_scatter;
	delete pic_earth;
	delete m_pScanDlg;
	m_pScanDlg = NULL;
	if(hScanGPS)
	{
		CloseHandle(hScanGPS);
		hScanGPS = NULL;
	}
	delete m_infoPanel;
	delete m_earthPanel;
	delete m_scatterPanel;
	delete m_downloadPanel;

	if(m_saveNmeaDlg)
	{
		//m_saveNmea->DestroyWindow();
		delete m_saveNmeaDlg;
	}
	if(m_playNmea)
	{
		//m_playNmea->DestroyWindow();
		delete m_playNmea;
	}
}

void CGPSDlg::Initialization()
{	
	Load_Menu();
	RescaleDialog();

	m_pScanDlg = new CScanDlg;
	gpsDlg = this;

	VERIFY(m_ConnectBtn.AutoLoad(IDC_CONNECT, this));
	VERIFY(m_CloseBtn.AutoLoad(IDC_CLOSE, this));   
	VERIFY(m_PlayBtn.AutoLoad(IDC_PLAY, this));
	VERIFY(m_StopBtn.AutoLoad(IDC_STOP, this));
	VERIFY(m_RecordBtn.AutoLoad(IDC_RECORD, this));   
	VERIFY(m_SetOriginBtn.AutoLoad(IDC_SETORIGIN, this));
	VERIFY(m_ClearBtn.AutoLoad(IDC_CLEAR, this));
	VERIFY(m_DownloadBtn.AutoLoad(IDC_DOWNLOAD, this));	
	VERIFY(m_EarthSettingBtn.AutoLoad(IDC_EARTHSETTING, this));	
	VERIFY(m_ScatterSettingBtn.AutoLoad(IDC_SCATTERSETTING, this));	

	m_SetOriginBtn.EnableWindow(!g_setting.specifyCenter);
	m_scale.ResetContent();
	CString enuItems[] = { "0.1m", "0.2m", "0.5m", "1m", "2m", "3m", "5m", "10m", 
				"20m", "30m", "40m", "50m", "100m", "150m", "200m", "300m", "" };
	int c = 0;
	while(1)
	{
		if(enuItems[c].IsEmpty())
		{
			break;
		}
		m_scale.AddString(enuItems[c++]);
	} 
	
	
	SwitchToConnectedStatus(FALSE);
	m_scale.SetCurSel(DefauleEnuScale);
	m_mapscale.SetCurSel(0);


	m_mapscale.ShowWindow(0);
	m_coordinate.SetCurSel(0);	
	m_StopBtn.EnableWindow(FALSE);

	int dlBaudIdx = theApp.GetIntSetting("dl_baudIdx", g_setting.boostBaudIndex);
	dlBaudIdx -= 5;	//Download Baudrate start in 115200
	((CComboBox*)GetDlgItem(IDC_DL_BAUDRATE))->SetCurSel(dlBaudIdx);


	m_textFont.CreatePointFont(100, "Arial");	
	m_connectT.SetFont(&m_textFont);
	m_infoFontS.CreateFont(-13, 0, 0,
			0, FW_NORMAL, 0, 0,
			0, DEFAULT_CHARSET , OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH,
			"Arial");
	m_infoFontL.CreateFont(-16, 0, 0,
			0, FW_NORMAL, 0, 0,
			0, DEFAULT_CHARSET , OUT_DEFAULT_PRECIS,
			CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH,
			"Arial");

	m_ttff.SetFont(&m_infoFontL);
	m_date.SetFont(&m_infoFontL);
	m_time.SetFont(&m_infoFontL);
	m_bootStatus.SetFont(&m_infoFontL);
	m_swKernel.SetFont(&m_infoFontL);
	m_swRev.SetFont(&m_infoFontL);
	m_longitude.SetFont(&m_infoFontS);
	m_latitude.SetFont(&m_infoFontS);
	m_altitude.SetFont(&m_infoFontL);
	m_direction.SetFont(&m_infoFontL);
	m_speed.SetFont(&m_infoFontL);
	m_hdop.SetFont(&m_infoFontL);

	m_BaudRateCombo.SetCurSel(BAUDRATE);
	UWM_UPDATE_EVENT = NMEA::gnssData.RegisterEventMessage();

	m_playNmea = new CPlayNmea();
	m_playNmea->Create(IDD_PLAY_NMEA);
	UWM_PLAYNMEA_EVENT = m_playNmea->RegisterEventMessage();
	m_playNmea->SetNotifyWindow(this->GetSafeHwnd());

	
	m_fixed_status.SetTransparent(TRUE);
	m_fixed_status.SetBkColor(RGB(206,204,194));
	m_fixed_status.SetTextColor(RGB(0,0,255));
	m_fixed_status.ModifyStyle(0,SS_CENTERIMAGE);

	m_wgs84_x.SetBkColor(RGB(255,255,255));
	m_wgs84_x.SetTextColor(RGB(0,0,255));

	m_wgs84_y.SetBkColor(RGB(255,255,255));
	m_wgs84_y.SetTextColor(RGB(0,0,255));

	m_wgs84_z.SetBkColor(RGB(255,255,255));
	m_wgs84_z.SetTextColor(RGB(0,0,255));

	m_enu_e.SetBkColor(RGB(255,255,255));
	m_enu_e.SetTextColor(RGB(0,0,255));

	m_enu_n.SetBkColor(RGB(255,255,255));
	m_enu_n.SetTextColor(RGB(0,0,255));

	m_enu_u.SetBkColor(RGB(255,255,255));
	m_enu_u.SetTextColor(RGB(0,0,255));

	//-----Init UI Controls End-----------------------------------
	GetCurrentDirectory(MyMaxPath, m_currentDir);	

	m_nmeaFileSize = 0;
	m_nmeaList.InsertColumn(0,"Message");
	m_nmeaList.SetColumnWidth(0, 1550);	

	comboFont.CreatePointFont(100, "Times New Roman");
	m_nmeaList.SetFont(&comboFont, TRUE);

	messageFont.CreatePointFont(80, "Courier New");
	m_responseList.SetFont(&messageFont, TRUE);

	m_kNumList.InsertColumn(0,"K-Num");
	m_kNumList.SetColumnWidth(0,55);	
	m_kNumList.SetFont(&comboFont, 1);

	m_kNumList.InsertColumn(1,"Slot");
	m_kNumList.SetColumnWidth(1,40);	
	m_kNumList.SetFont(&comboFont, 1);

	m_kNumList.InsertColumn(2,"CN");
	m_kNumList.SetColumnWidth(2,40);	
	m_kNumList.SetFont(&comboFont, 1);
	
	SetConnectTitle(false);
	//m_connectT.SetWindowText("Close");

//	m_isNmeaFileOpen        = false;	
//	m_isPressNmeaCommend    = false;
	m_isNmeaUpdated         = false;
	m_isConnectOn = false;		
	m_isFlogOpen            = false;
	m_isPressCloseButton    = true;

	m_inputMode             = Nooutput_Mode;
	memset(MSG_TYPE_STORAGE,0,1275);
	SetMsgType(NMEA_Mode);

	memset(&m_logFlashInfo,0,sizeof(m_logFlashInfo));
	m_logFlashInfo.max_time=3600;
	m_logFlashInfo.min_time=5;
	m_logFlashInfo.max_distance=100;
	m_logFlashInfo.min_distance=0;
	m_logFlashInfo.max_speed=100;
	m_logFlashInfo.min_speed=0;
	m_logFlashInfo.datalog_enable = 0;
	m_logFlashInfo.fifo_mode = 0;
	slgsv = 6;

	m_tip.Create(this);
	m_tip.AddTool(GetDlgItem(IDC_CONNECT), _T("Disconnected"));
	m_tip.AddTool(GetDlgItem(IDC_CLOSE), _T("Connected"));
	m_tip.AddTool(GetDlgItem(IDC_SETORIGIN), _T("Set current as origin."));
	m_tip.AddTool(GetDlgItem(IDC_DOWNLOAD), _T(" Download firmware to target."));

	hScanGPS = CreateEvent(NULL, TRUE, FALSE, NULL);	
	if(!ResetEvent(hScanGPS))   DWORD error = GetLastError();

	CRegistry reg;
	reg.SetRootKey(HKEY_CURRENT_USER);
	if(reg.SetKey("Software\\GNSSViewer\\GPS",true))
	{
		m_comPort = reg.ReadInt("comport",0);
		m_baudrate = reg.ReadInt("baudrate",0);
		m_ComPortCombo.SetCurSel(GetComPort());
		m_BaudRateCombo.SetCurSel(GetBaudrate());

		CString strPath = reg.ReadString("firmware", "");
		if(strPath.IsEmpty())
		{
			strPath.Format("%s%s", theApp.GetCurrrentDir(), "\\prom.bin");
		}
		m_lbl_firmware_path.SetWindowText(strPath);
		m_lbl_firmware_path.Invalidate();
	}

	m_gpgsvMsg.NumOfSate = 0;
	m_glgsvMsg.NumOfSate = 0;
	m_bdgsvMsg.NumOfSate = 0;

//	m_setorigin_user.ShowWindow(0);
/*
	if ((SOFTWARE_FUNCTION & SW_FUN_DR) == 0)
	{
		m_odo_meter.ShowWindow(0);
		m_gyro_data.ShowWindow(0);
		m_backward_indicator.ShowWindow(0);
		m_lbl_odo_meter.ShowWindow(0);
		m_lbl_gyro_data.ShowWindow(0);
		m_lbl_backward_indicator.ShowWindow(0);
	}
*/
	char local_path[1024];
	GetCurrentDirectory(1024,local_path);
	strcat_s(local_path, sizeof(local_path), "\\GNSSViewer.ini");
	if(Utility::IsFileExist(local_path))
	{
		warmstart_latitude = Utility::GetPrivateProfileDouble("AGPS", "Latitude", "2400", local_path);
		warmstart_longitude = Utility::GetPrivateProfileDouble("AGPS", "Lontitude", "12100", local_path);
		warmstart_altitude = Utility::GetPrivateProfileDouble("AGPS", "Altitude", "100", local_path);
	}

	HDEVNOTIFY hDevNotify;
	DEV_BROADCAST_DEVICEINTERFACE NotificationFilter;
	ZeroMemory( &NotificationFilter, sizeof(NotificationFilter) );
	NotificationFilter.dbcc_size = sizeof(DEV_BROADCAST_DEVICEINTERFACE);
	NotificationFilter.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
	for(int i=0; i<sizeof(GUID_DEVINTERFACE_LIST)/sizeof(GUID); i++) {
		NotificationFilter.dbcc_classguid = GUID_DEVINTERFACE_LIST[i];
		hDevNotify = RegisterDeviceNotification(this->GetSafeHwnd(), &NotificationFilter, DEVICE_NOTIFY_WINDOW_HANDLE);

	}

#if WITH_CONFIG_USB_BAUDRATE
	m_DefaultBaudConfigData[0].BaudRate = 15000000;
	m_DefaultBaudConfigData[0].BaudGen = 0xFFF0;
	m_DefaultBaudConfigData[0].Prescaler = 1;
	m_DefaultBaudConfigData[0].Timer0Reload = 0xFFFA;

	m_DefaultBaudConfigData[1].BaudRate = 15000000;
	m_DefaultBaudConfigData[1].BaudGen = 0xFFF0;
	m_DefaultBaudConfigData[1].Prescaler = 1;
	m_DefaultBaudConfigData[1].Timer0Reload = 0xFFFA;

	m_DefaultBaudConfigData[2].BaudRate = 12000000;
	m_DefaultBaudConfigData[2].BaudGen = 0xFFEC;
	m_DefaultBaudConfigData[2].Prescaler = 1;
	m_DefaultBaudConfigData[2].Timer0Reload = 0xFFF8;

	m_DefaultBaudConfigData[3].BaudRate = 921600;
	m_DefaultBaudConfigData[3].BaudGen = 0xFFCE;	// skytraq glonass modify
	m_DefaultBaudConfigData[3].Prescaler = 1;
	m_DefaultBaudConfigData[3].Timer0Reload = 0xFFF6;

	m_DefaultBaudConfigData[4].BaudRate = 576000;
	m_DefaultBaudConfigData[4].BaudGen = 0xFFD6;
	m_DefaultBaudConfigData[4].Prescaler = 1;
	m_DefaultBaudConfigData[4].Timer0Reload = 0xFFF0;

	m_DefaultBaudConfigData[5].BaudRate = 500000;
	m_DefaultBaudConfigData[5].BaudGen = 0xFFD0;
	m_DefaultBaudConfigData[5].Prescaler = 1;
	m_DefaultBaudConfigData[5].Timer0Reload = 0xFFEE;

	m_DefaultBaudConfigData[6].BaudRate = 460800;
	m_DefaultBaudConfigData[6].BaudGen = 0xFFCE;
	m_DefaultBaudConfigData[6].Prescaler = 1;
	//m_DefaultBaudConfigData[6].Timer0Reload = 0xFFEC;
	m_DefaultBaudConfigData[6].Timer0Reload = 0xFFE0;

	m_DefaultBaudConfigData[7].BaudRate = 256000;
	m_DefaultBaudConfigData[7].BaudGen = 0xFFA2;
	m_DefaultBaudConfigData[7].Prescaler = 1;
	m_DefaultBaudConfigData[7].Timer0Reload = 0xFFDC;

	m_DefaultBaudConfigData[8].BaudRate = 250000;
	m_DefaultBaudConfigData[8].BaudGen = 0xFFA0;
	m_DefaultBaudConfigData[8].Prescaler = 1;
	m_DefaultBaudConfigData[8].Timer0Reload = 0xFFDC;

	m_DefaultBaudConfigData[9].BaudRate = 230400;
	m_DefaultBaudConfigData[9].BaudGen = 0xFF98;
	m_DefaultBaudConfigData[9].Prescaler = 1;
	m_DefaultBaudConfigData[9].Timer0Reload = 0xFFD9;

	m_DefaultBaudConfigData[10].BaudRate = 153600;
	m_DefaultBaudConfigData[10].BaudGen = 0xFF64;
	m_DefaultBaudConfigData[10].Prescaler = 1;
	m_DefaultBaudConfigData[10].Timer0Reload = 0xFFC5;

	m_DefaultBaudConfigData[11].BaudRate = 128000;
	m_DefaultBaudConfigData[11].BaudGen = 0xFF44; //Originally FF45 in firmware, but changed to FF44 Sep 7 2004
	m_DefaultBaudConfigData[11].Prescaler = 1;
	m_DefaultBaudConfigData[11].Timer0Reload = 0xFFB9;

	m_DefaultBaudConfigData[12].BaudRate = 115200;
	m_DefaultBaudConfigData[12].BaudGen = 0xFF30;
	m_DefaultBaudConfigData[12].Prescaler = 1;
	m_DefaultBaudConfigData[12].Timer0Reload = 0xFFB2;

	m_DefaultBaudConfigData[13].BaudRate = 76800;
	m_DefaultBaudConfigData[13].BaudGen = 0xFEC8;
	m_DefaultBaudConfigData[13].Prescaler = 1;
	m_DefaultBaudConfigData[13].Timer0Reload = 0xFF8B;

	m_DefaultBaudConfigData[14].BaudRate = 64000;
	m_DefaultBaudConfigData[14].BaudGen = 0xFE89;
	m_DefaultBaudConfigData[14].Prescaler = 1;
	m_DefaultBaudConfigData[14].Timer0Reload = 0xFF73;

	m_DefaultBaudConfigData[15].BaudRate = 57600;
	m_DefaultBaudConfigData[15].BaudGen = 0xFE5F;
	m_DefaultBaudConfigData[15].Prescaler = 1;
	m_DefaultBaudConfigData[15].Timer0Reload = 0xFF63;

	m_DefaultBaudConfigData[16].BaudRate = 56000;
	m_DefaultBaudConfigData[16].BaudGen = 0xFE53;
	m_DefaultBaudConfigData[16].Prescaler = 1;
	m_DefaultBaudConfigData[16].Timer0Reload = 0xFF5F;

	m_DefaultBaudConfigData[17].BaudRate = 51200;
	m_DefaultBaudConfigData[17].BaudGen = 0xFE2B;
	m_DefaultBaudConfigData[17].Prescaler = 1;
	m_DefaultBaudConfigData[17].Timer0Reload = 0xFF50;

	m_DefaultBaudConfigData[18].BaudRate = 38400;
	m_DefaultBaudConfigData[18].BaudGen = 0xFD8F;
	m_DefaultBaudConfigData[18].Prescaler = 1;
	m_DefaultBaudConfigData[18].Timer0Reload = 0xFF15;

	m_DefaultBaudConfigData[19].BaudRate = 28800;
	m_DefaultBaudConfigData[19].BaudGen = 0xFCBF;
	m_DefaultBaudConfigData[19].Prescaler = 1;
	m_DefaultBaudConfigData[19].Timer0Reload = 0xFEC7;

	m_DefaultBaudConfigData[20].BaudRate = 19200;
	m_DefaultBaudConfigData[20].BaudGen = 0xFB1E;
	m_DefaultBaudConfigData[20].Prescaler = 1;
	m_DefaultBaudConfigData[20].Timer0Reload = 0xFE2B;

	m_DefaultBaudConfigData[21].BaudRate = 16000;
	m_DefaultBaudConfigData[21].BaudGen = 0xFA24;
	m_DefaultBaudConfigData[21].Prescaler = 1;
	m_DefaultBaudConfigData[21].Timer0Reload = 0xFDCD;

	m_DefaultBaudConfigData[22].BaudRate = 14400;
	m_DefaultBaudConfigData[22].BaudGen = 0xF97D;
	m_DefaultBaudConfigData[22].Prescaler = 1;
	m_DefaultBaudConfigData[22].Timer0Reload = 0xFD8E;

	m_DefaultBaudConfigData[23].BaudRate = 9600;
	m_DefaultBaudConfigData[23].BaudGen = 0xF63C;
	m_DefaultBaudConfigData[23].Prescaler = 1;
	m_DefaultBaudConfigData[23].Timer0Reload = 0xFC56;

	m_DefaultBaudConfigData[24].BaudRate = 7200;
	m_DefaultBaudConfigData[24].BaudGen = 0xF2FB;
	m_DefaultBaudConfigData[24].Prescaler = 1;
	m_DefaultBaudConfigData[24].Timer0Reload = 0xFB1E;

	m_DefaultBaudConfigData[25].BaudRate = 4800;
	m_DefaultBaudConfigData[25].BaudGen = 0xEC78;
	m_DefaultBaudConfigData[25].Prescaler = 1;
	m_DefaultBaudConfigData[25].Timer0Reload = 0xF8AD;

	m_DefaultBaudConfigData[26].BaudRate = 4000;
	m_DefaultBaudConfigData[26].BaudGen = 0xE890;
	m_DefaultBaudConfigData[26].Prescaler = 1;
	m_DefaultBaudConfigData[26].Timer0Reload = 0xF736;

	m_DefaultBaudConfigData[27].BaudRate = 2400;
	m_DefaultBaudConfigData[27].BaudGen = 0xD8F0;
	m_DefaultBaudConfigData[27].Prescaler = 1;
	m_DefaultBaudConfigData[27].Timer0Reload = 0xF15A;

	m_DefaultBaudConfigData[28].BaudRate = 1800;
	m_DefaultBaudConfigData[28].BaudGen = 0xCBEB;
	m_DefaultBaudConfigData[28].Prescaler = 1;
	m_DefaultBaudConfigData[28].Timer0Reload = 0xEC78;

	m_DefaultBaudConfigData[29].BaudRate = 1200;
	m_DefaultBaudConfigData[29].BaudGen = 0xB1E0;
	m_DefaultBaudConfigData[29].Prescaler = 1;
	m_DefaultBaudConfigData[29].Timer0Reload = 0xE2B4;

	m_DefaultBaudConfigData[30].BaudRate = 600;
	m_DefaultBaudConfigData[30].BaudGen = 0x63C0;
	m_DefaultBaudConfigData[30].Prescaler = 1;
	m_DefaultBaudConfigData[30].Timer0Reload = 0xC568;

	m_DefaultBaudConfigData[31].BaudRate = 300;
	m_DefaultBaudConfigData[31].BaudGen = 0xB1E0;
	m_DefaultBaudConfigData[31].Prescaler = 4;
	m_DefaultBaudConfigData[31].Timer0Reload = 0x8AD0;

	memcpy(&m_ROM_BaudConfigData[0],&m_DefaultBaudConfigData[0],sizeof(m_ROM_BaudConfigData));

	m_ROM_BaudConfigData[3].BaudGen = 0xFFD8;		// 891200 ,
	m_ROM_BaudConfigData[6].BaudGen = 0xFFD8;
	m_ROM_BaudConfigData[12].BaudGen = 0xFF51;		// 115200
	m_ROM_BaudConfigData[15].BaudGen = 0xFEA2;		// 57600
	m_ROM_BaudConfigData[18].BaudGen = 0xFDF3;		// 38400
	m_ROM_BaudConfigData[20].BaudGen = 0xFBE6;		// 19200
	m_ROM_BaudConfigData[23].BaudGen = 0xF7CD;		// 9600
	m_ROM_BaudConfigData[25].BaudGen = 0xEF9B;		// 4800
#endif


#ifdef RTC_CHECK
	//CBitmap bitmap;
	//HBITMAP   hBmp   =   (HBITMAP)bitmap;
	//bitmap.LoadBitmap(IDB_RTC);
	//lbl_noise.SetBitmap((HBITMAP)bitmap);
	m_noise.ShowWindow(1);
	lbl_noise.ShowWindow(1);
#endif

	this->SetWindowText(theApp.GetTitle());

	gpsSnrBar->SetGsvData(&m_gpgsvMsg); 
	gpsSnrBar->SetGsaData(&m_gpgsaMsg); 
	gpsSnrBar->SetGgaData(&m_gpggaMsg); 
	gpsSnrBar->SetSateStatus(sate_gps); 
//	gpsSnrBar->SetSnrBarChartMode(CSnrBarChart::Gps);
	gpsSnrBar->SetGsvData2(&m_glgsvMsg); 
	gpsSnrBar->SetGsaData2(&m_glgsaMsg); 
	gpsSnrBar->SetGgaData2(&m_gpggaMsg); 
	gpsSnrBar->SetSateStatus2(sate_gnss); 

	if(_V8_SUPPORT)
	{
		m_BaudRateCombo.ResetContent();
		for(int i=0; i<9; ++i)
		{
			CString strIdx;
			strIdx.Format("%d", CSerial::BaudrateTable[i]);
			m_BaudRateCombo.AddString(strIdx);
		}
		m_BaudRateCombo.SetCurSel(1);
		((CButton*)GetDlgItem(IDC_IN_LOADER))->SetCheck(TRUE);

	}
	else
	{
		CSerial::BaudrateTable[6] = _BAUTRATE_IDX_6_;
		m_BaudRateCombo.ResetContent();
		for(int i=0; i<6; ++i)
		{
			CString strIdx;
			strIdx.Format("%d", CSerial::BaudrateTable[i]);
			m_BaudRateCombo.AddString(strIdx);
		}
		m_BaudRateCombo.SetCurSel(1);
	}

//	if(!ECOMPASS_CALIBRATION)
//	{
//		GetDlgItem(IDC_ECOM_CALIB)->ShowWindow(SW_HIDE);
//		GetDlgItem(IDC_ECOM_COUNTER)->ShowWindow(SW_HIDE);
//	}

#if GG12A
	bdSnrBar->SetGsvData(NULL); 
	bdSnrBar->SetGsaData(NULL); 
	bdSnrBar->SetGgaData(NULL); 
	bdSnrBar->SetSateStatus(NULL); 
	gaSnrBar->SetGsvData(NULL); 
	gaSnrBar->SetGsaData(NULL); 
	gaSnrBar->SetGgaData(NULL); 
	gaSnrBar->SetSateStatus(NULL); 
#else
	bdSnrBar->SetGsvData(&m_bdgsvMsg); 
	bdSnrBar->SetGsaData(&m_bdgsaMsg); 
	bdSnrBar->SetGgaData(&m_gpggaMsg); 
	bdSnrBar->SetSateStatus(sate_bd); 
	gaSnrBar->SetGsvData(&m_gagsvMsg); 
	gaSnrBar->SetGsaData(&m_gagsaMsg); 
	gaSnrBar->SetGgaData(&m_gpggaMsg); 
	gaSnrBar->SetSateStatus(sate_ga); 
#endif
//	gnssSnrBar->SetSnrBarChartMode(CSnrBarChart::Gnss);
}
/*
void CGPSDlg::change_agps_status(U08 agps)
{
	if(agps)
	{
		m_agps_enable.SetWindowText("AGPS Enabled");
		m_agps_enable.EnableWindow(1);
		m_agps_disable.ShowWindow(0);
	}
	else
	{
		m_agps_enable.SetWindowText("AGPS Disabled");
		m_agps_enable.EnableWindow(1);
		//m_agps_enable.EnableWindow(1);
		m_agps_disable.ShowWindow(0);
	}
}
*/
void CGPSDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CGPSDlg::OnPaint() 
{
	if(IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CGPSDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CGPSDlg::ClearInformation(bool onlyQueryInfo)
{
	NMEA::gnssData.ClearData();
	DisplayLongitude(0, 0, 0.0, 0);
	//DisplayAltitude(0.0);
	DisplayLatitude(0, 0, 0.0, 0);
	m_gpggaMsg.Altitude = -9999.9F;
	ShowAltitude();
	CLEAR_NMEA_TO_USE();

	SetTTFF(0);
	DisplayDate(0, 0, 0);
	m_date.SetWindowText("");
	DisplayTime(0, 0, 0);
	m_time.SetWindowText("");

	m_bootStatus.SetWindowText("");
	m_swKernel.SetWindowText("");
	m_swRev.SetWindowText("");

	m_longitude.SetWindowText("");
	m_latitude.SetWindowText("");

	m_altitude.SetWindowText("");
	ShowAltitude(true);
	m_direction.SetWindowText("0.0");
	ShowDirection(true);
#if(!_MODULE_SUP_800_)
	m_speed.SetWindowText("0.0");
	ShowSpeed(true);
	m_hdop.SetWindowText("0.0");
	ShowPDOP(true);
#else
	m_speed.SetWindowText("");
	m_hdop.SetWindowText("");
#endif
	gpsSnrBar->Invalidate(FALSE);
	bdSnrBar->Invalidate(FALSE);
	gaSnrBar->Invalidate(FALSE);

	pic_earth->Invalidate(FALSE);
	pic_scatter->Invalidate(FALSE);//	Sleep(1000);
}

bool CGPSDlg::NmeaInput()
{
	CFileDialog fd(true, "*.txt", NULL, OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, "*.txt|*.txt||");
	if(fd.DoModal() != IDOK)
	{
		return false;
	}
	m_nmeaPlayFilePath = fd.GetPathName();

	m_playNmea->ShowWindow(SW_SHOW);
	m_playNmea->Initialize(m_nmeaPlayFilePath);
	m_nmeaPlayInterval = m_playNmea->GetPlayInterval();
	m_playNmea->SetFocus();
	m_nmeaPlayThread = ::AfxBeginThread(NMEA_Parsing_Thread, (LPVOID)(LPCSTR)m_nmeaPlayFilePath);
	SetConnectTitle(true);
	return true;
}

bool CGPSDlg::ComPortInput()
{
	m_comPort = m_ComPortCombo.GetCurSel();
	m_baudrate = m_BaudRateCombo.GetCurSel();
	m_serial = new CSerial;

	char buff[100];
	if(!m_serial->Open(GetComPort()+1, GetBaudrate()))
	{		
		sprintf_s(buff, sizeof(buff), "Failed to open port %d!", GetComPort()+1);		
		add_msgtolist(buff);	
		delete m_serial;
		m_serial = NULL;

		SwitchToConnectedStatus(FALSE);
		AfxMessageBox(buff);
		return false;
	}
	m_firstDataIn = false;
	
	if(m_isConnectOn) 
	{
		OnBnClickedClose();
	}
	CreateGPSThread();

	m_inputMode = NMEA_Mode;	
	//m_connectT.SetWindowText("Connect");
	SetConnectTitle(true);

	CRegistry reg;
	reg.SetRootKey(HKEY_CURRENT_USER);
	if(reg.SetKey("Software\\GNSSViewer\\GPS", true))
	{
		reg.WriteInt("comport", GetComPort());
		reg.WriteInt("baudrate", GetBaudrate());
	}
	return true;
}

void CGPSDlg::OnBnClickedRecord()
{
	OnFileSaveNmea();
	//if(!m_isPressNmeaCommend)
	//{
	//	return;
	//}
	OnBnClickedConnect();
	//if(m_serial==NULL)
	//{
	//	close_nmea_file();
	//	m_saveNmeaDlg->ShowWindow(SW_HIDE);
	//	return;
	//}

	m_StopBtn.EnableWindow(TRUE);
	m_PlayBtn.EnableWindow(FALSE);
	m_RecordBtn.EnableWindow(FALSE);
}

void CGPSDlg::OnBnClickedPlay()
{
	if(!NmeaInput())
	{
		return;
	}
	m_StopBtn.EnableWindow(TRUE);
	m_PlayBtn.EnableWindow(FALSE);
	m_RecordBtn.EnableWindow(FALSE);

	SwitchToConnectedStatus(TRUE);
	DeleteNmeaMemery();	
	ClearInformation();

	m_isPressCloseButton = false;	  
	m_gpgsvMsg.NumOfSate = 0;
	m_glgsvMsg.NumOfSate = 0;
	m_gpgsvMsg.NumOfMessage = 0;	
	m_glgsvMsg.NumOfMessage = 0;	
	m_nmeaList.ClearAllText();
	m_nmeaList.DeleteAllItems();   

	g_scatterData.ClearData();
	OnCbnCloseupEnuscale();
	OnCbnCloseupMapscale();

	m_ttffCount = 0;
	m_initTtff = false;
	SetTTFF(0);
	ClearGlonass();
}

void CGPSDlg::OnBnClickedConnect()
{
	bool connectOk = ComPortInput();
	if(!connectOk)
	{
		return;
	}

	SwitchToConnectedStatus(TRUE);
	DeleteNmeaMemery();	
	ClearInformation();

	m_isPressCloseButton = false;	  
	m_gpgsvMsg.NumOfSate = 0;
	m_glgsvMsg.NumOfSate = 0;
	m_gpgsvMsg.NumOfMessage = 0;	
	m_glgsvMsg.NumOfMessage = 0;	
	m_nmeaList.ClearAllText();
	m_nmeaList.DeleteAllItems();   

	g_scatterData.ClearData();
	OnCbnCloseupEnuscale();
	OnCbnCloseupMapscale();
//	m_scale.SetCurSel(DefauleEnuScale);
//	m_mapscale.SetCurSel(0);

	m_ttffCount = 0;
	m_initTtff = false;
	SetTTFF(0);
	ClearGlonass();
}

//HANDLE g_thread = NULL;
void CGPSDlg::CreateGPSThread()
{
	if(!NMEA_INPUT)
	{
		SetTimer(SHOW_STATUS_TIMER, 1000, NULL);
	}
	m_isConnectOn = true;
	NMEA::nmeaType = NMEA::NtUnknown;
	//Creat Connect Event
	g_connectEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	g_closeEvent   = CreateEvent(NULL, TRUE, FALSE, NULL);
	waitlog        = CreateEvent(NULL, TRUE, FALSE, NULL);

	if(!ResetEvent(g_connectEvent))  
	{
		DWORD error = GetLastError();
	}
	if(!ResetEvent(g_closeEvent))  
	{
		DWORD error = GetLastError();
	}

	if(!ResetEvent(waitlog))  
	{
		DWORD error = GetLastError();
	}

	HWND msgWindow = ::GetDlgItem(this->m_hWnd, IDOK);

	int com,baudrate;
	com = m_serial->GetComPort();
	baudrate = m_serial->GetBaudRate();

	if(!m_serial->IsOpened())
	{
		m_serial->Open(com,baudrate);
	}

	//g_thread = (HANDLE)_beginthread(ConnectGPS, 0, msgWindow);
	g_gpsThread = ::AfxBeginThread(ConnectGPS, 0);
	if(!SetEvent(g_connectEvent)) 
	{
		DWORD error = GetLastError();	
	}
//	GetDlgItem(IDC_CONNECT)->EnableWindow(TRUE);

}

void CGPSDlg::TerminateGPSThread()
{
	SetNmeaUpdated(false);
	m_isConnectOn = false;  
	m_serial->CancelTransmission();
	//Sleep(100);
	WaitForSingleObject(g_closeEvent, 1000);
	CloseHandle(g_closeEvent);
	g_closeEvent = NULL;

	KillTimer(SHOW_STATUS_TIMER);
	if(g_connectEvent)
	{
		CloseHandle(g_connectEvent);
		g_connectEvent = NULL;
	}
	//TerminateThread(g_thread, 0);
	if(g_gpsThread)
	{
		TerminateThread(g_gpsThread, 0);
	}

}

void CGPSDlg::OnTimer(UINT nIDEvent)
{	
	if(nIDEvent==ECOM_CALIB_TIMER)
	{
		CString strSeconds;
		GetDlgItem(IDC_ECOM_COUNTER)->GetWindowText(strSeconds);
		int nSec = atoi(strSeconds);

		if(--nSec <= 0)
		{
			KillTimer(nIDEvent);
		}
		strSeconds.Format("%d", nSec);
		GetDlgItem(IDC_ECOM_COUNTER)->SetWindowText(strSeconds);
		CDialog::OnTimer(nIDEvent);
		return;
	}
	else if(nIDEvent==TEST_KERNEL_TIMER)
	{

	}
	else if(SHOW_STATUS_TIMER==nIDEvent)
	{
	
		switch(m_inputMode)
		{
		case NMEA_Mode:

			if(m_isNmeaUpdated)	
			{	
				CopyNmeaToUse();
		
				ShowStatus();
				ShowDate();			
				ShowLongitudeLatitude();		
				ShowAltitude();	
				ShowDirection();
				ShowKNumber();
#if(!_MODULE_SUP_800_)
				ShowVersion();
				ShowBootStatus();
				ShowSpeed();	
				ShowPDOP();		
#else
				ShowPsti004001();	
#endif
				SetNmeaUpdated(false);
			}	
			break;
		case Binary_Mode:	
			_BINMSGCS.Lock();	
			_BINMSGCS.Unlock();
			break;
		default:
			break;
		}
		pic_scatter->Invalidate(FALSE);
		m_wgs84_x.Invalidate(FALSE);
		m_wgs84_y.Invalidate(FALSE);
		m_wgs84_z.Invalidate(FALSE);

		m_enu_e.Invalidate(FALSE);
		m_enu_n.Invalidate(FALSE);
		m_enu_u.Invalidate(FALSE);
		UpdateCooridate();
	}
	else if(DELAY_QUERY_TIMER==nIDEvent)
	{
		KillTimer(nIDEvent);
		GetGPSStatus();
	}
	else if(DELAY_PLUGIN_TIMER==nIDEvent)
	{
		KillTimer(nIDEvent);
		Close_Open_Port(plugin_wParam, plugin_port_name);
	}
	CDialog::OnTimer(nIDEvent);
}

void CGPSDlg::UpdateCooridate()
{
	CString str;
	
	str.Format("%.3f", g_scatterData.X);
	m_wgs84_x.SetWindowText(str);
//Log2(__FUNCTION__, __LINE__, "m_wgs84_x");

	str.Format("%.3f", g_scatterData.Y);
	m_wgs84_y.SetWindowText(str);
//Log2(__FUNCTION__, __LINE__, "m_wgs84_y");

	str.Format("%.3f", g_scatterData.Z);
	m_wgs84_z.SetWindowText(str);
//Log2(__FUNCTION__, __LINE__, "m_wgs84_z");

	if(g_scatterData.IniPos)
	{
		return;
	}
	str.Format("%.3f", g_scatterData.ENU(0,0));
	m_enu_e.SetWindowText(str);

	str.Format("%.3f", g_scatterData.ENU(1,0));
	m_enu_n.SetWindowText(str);

	str.Format("%.3f", g_scatterData.ENU(2,0));
	m_enu_u.SetWindowText(str);

	str.Format("%.4f", g_scatterData.TwoDrms);
	m_twodrms.SetWindowText(str);

	str.Format("%.4f", g_scatterData.CEP);
	m_cep.SetWindowText(str);
}

void CGPSDlg::OnBnClickedStop()
{		
	m_StopBtn.EnableWindow(FALSE);
	if(m_nmeaPlayThread)
	{
		g_nmeaInputTerminate = 1;
		m_playNmea->ShowWindow(SW_HIDE);
		::WaitForSingleObject(m_nmeaPlayThread, INFINITE);
	}
	
	m_isPressCloseButton = true;
	m_BaudRateCombo.EnableWindow(TRUE);	
	m_ComPortCombo.EnableWindow(TRUE);
	m_PlayBtn.EnableWindow(TRUE);
	m_RecordBtn.EnableWindow(TRUE);
}

void CGPSDlg::OnBnClickedClose()
{		
#if NMEA_INPUT
	g_nmeaInputTerminate = 1;
	m_playNmea->ShowWindow(SW_HIDE);
	::WaitForSingleObject(m_nmeaPlayThread, INFINITE);

	m_BaudRateCombo.EnableWindow(1);	
	m_ComPortCombo.EnableWindow(1);
	GetDlgItem(IDC_CONNECT)->ShowWindow(1);    
	m_CloseBtn.ShowWindow(0);	
#else
	Terminate();	
	m_CloseBtn.EnableWindow(FALSE);
#endif
	//m_connectT.SetWindowText("Close");
	SetConnectTitle(false);
}

void CGPSDlg::Terminate(void)
{	
	SwitchToConnectedStatus(FALSE);

	m_serial->CancelTransmission();
	Sleep(100);

	TerminateGPSThread();
	//WaitForSingleObject(close_thread,500);	
	//
	m_serial->Close();

	delete m_serial;
	m_serial = NULL;			
	m_isPressCloseButton = true;
//	Reconnect = true;

	m_inputMode = 0;	
	if(m_isFlogOpen)
	{
		dataLogFile.Close();
		m_isFlogOpen = false;
	} 
}

void CGPSDlg::OnCbnCloseupCoordinate()
{
	int coorIdx = m_coordinate.GetCurSel();	
	if(coorIdx==1)
	{
		//int LLAindex = m_mapscale.GetCurSel(); 
		m_scale.ShowWindow(0);
		m_mapscale.ShowWindow(1);
		m_SetOriginBtn.ShowWindow(0);
		g_scatterData.ChangeCoordinateLLA();
	}
	else if(coorIdx==0)
	{
		m_mapscale.ShowWindow(0);
		m_scale.ShowWindow(1);
		m_SetOriginBtn.ShowWindow(1);		
	}
}

void CGPSDlg::OnCbnCloseupEnuscale()
{
	int index = m_scale.GetCurSel();
	if(index!=LB_ERR)
	{
		g_scatterData.ChangeENUScale(index);
	}
}

void CGPSDlg::OnCbnCloseupMapscale()
{
	int index = m_mapscale.GetCurSel();
	if(index!=LB_ERR)
	{
		g_scatterData.ChangeLLAScale(index);
	}
}

void CGPSDlg::DisplayTime(int h, int m, D64 s)
{
	CString txt;
	txt.Format("%02d:%02d:%05.2lf", h, m, s);
	m_time.SetWindowText(txt);
	m_time.Invalidate(TRUE);
}

void CGPSDlg::DisplayTime(int h, int m, int s)
{
	static int lastH = 0, lastM = 0, lastS = 0;
	if(s != lastS || m != lastM || h != lastH)
	{
		CString txt;
		txt.Format("%02d:%02d:%02d", h, m, s);
		m_time.SetWindowText(txt);
		m_time.Invalidate(TRUE);
		lastH = h;
		lastM = m;
		lastS = s;
	}
}

void CGPSDlg::ShowRMCTime()
{
	DisplayTime(m_gprmcMsgCopy.Hour, m_gprmcMsgCopy.Min, (int)m_gprmcMsgCopy.Sec);
}

void CGPSDlg::ShowTime()
{
	DisplayTime(m_gpggaMsgCopy.Hour, m_gpggaMsgCopy.Min, (int)m_gpggaMsgCopy.Sec);
}

void CGPSDlg::DisplayDate(int y, int m, int d)
{
//	static int lastY = 0, lastM = 0, lastD = 0;
//	if(d != lastD || m != lastM || y != lastY)
	{
		CString txt;
		txt.Format("%02d/%02d/%02d", y, m, d);
		m_date.SetWindowText(txt);
		//m_date.Invalidate(TRUE);
		//lastY = y;
		//lastM = m;
		//lastD = d;
	}
}

void CGPSDlg::ShowDate(void)
{	
	if(m_gpzdaMsg.Year && m_gpzdaMsg.Month && m_gpzdaMsg.Day)
	{
		DisplayDate(m_gpzdaMsg.Year, m_gpzdaMsg.Month, m_gpzdaMsg.Day);
	}
	else if(m_gprmcMsg.Year && m_gprmcMsg.Month && m_gprmcMsg.Day)
	{
		DisplayDate(m_gprmcMsg.Year, m_gprmcMsg.Month, m_gprmcMsg.Day);
	}
}

void CGPSDlg::ShowVersion(void)
{	//m_versionInfo IDC_SW_VER IDC_SW_REV m_swKernel m_swRev
	if(m_versionInfo.Size()>=21 && m_versionInfo[4]==0x80)
	{
		CString txt;
		txt.Format("%d.%d.%d", m_versionInfo[7], m_versionInfo[8], m_versionInfo[9]);
		m_swKernel.SetWindowText(txt);
		m_swKernel.Invalidate(TRUE);
		txt.Format("%d.%d.%d", m_versionInfo[15] + 2000, m_versionInfo[16], m_versionInfo[17]);
		m_swRev.SetWindowText(txt);
		m_swRev.Invalidate(TRUE);
		m_versionInfo.Free();
	}
}

#if(_MODULE_SUP_800_)
void CGPSDlg::ShowPsti004001(void)
{	
	if(0==m_psti004001Copy.Valide)
	{
		m_bootStatus.SetBgColor(RGB(255,0,0));
		m_swKernel.SetBgColor(RGB(255,0,0));
		m_swRev.SetBgColor(RGB(255,0,0));
		//m_speed.SetTextColor(RGB(255,0,0));
		//m_hdop.SetTextColor(RGB(255,0,0));
	}
	else
	{
		m_bootStatus.SetBgColor(RGB(255,255,255));
		m_swKernel.SetBgColor(RGB(255,255,255));
		m_swRev.SetBgColor(RGB(255,255,255));
		//m_speed.SetTextColor(RGB(255,255,255));
		//m_hdop.SetTextColor(RGB(255,255,255));
	}

	CString txt;
	txt.Format("%.2f", m_psti004001Copy.Pitch);
	m_bootStatus.SetWindowText(txt);
	txt.Format("%.2f", m_psti004001Copy.Roll);
	m_swKernel.SetWindowText(txt);
	txt.Format("%.2f", m_psti004001Copy.Yaw);
	m_swRev.SetWindowText(txt);
	txt.Format("%d", m_psti004001Copy.Pressure);
	m_speed.SetWindowText(txt);
	txt.Format("%.2f", m_psti004001Copy.Temperature);
	m_hdop.SetWindowText(txt);
}
#endif
void CGPSDlg::ShowBootStatus(void)
{	//m_versionInfo IDC_SW_VER IDC_SW_REV m_swKernel m_swRev
	if(m_bootInfo.Size()>=11 && m_bootInfo[4]==0x64 && m_bootInfo[5]==0x80)
	{
		CString txt;
		switch(m_bootInfo[7])
		{
		case 0:
			txt += "ROM ";
			break;
		case 1:
			txt += "QSPI ";
			break;
		case 2:
			txt += "QSPI ";
			break;
		case 4:
			txt += "Parallel ";
			break;
		default:
			txt += "Unknown ";
			break;
		}

		switch(m_bootInfo[6])
		{
		case 0:
			txt += "(OK)";
			break;
		case 1:
			txt += "(Fail)";
			break;
		default:
			txt += "(Fail!)";
			break;
		}
		m_bootStatus.SetWindowText(txt);
		m_bootStatus.Invalidate(TRUE);
		m_bootInfo.Free();
	}
}
void CGPSDlg::DisplayLongitude(int h, int m, double s, char c)
{
	static int lastH= 0, lastM = 0;
	static double lastS = 0.0;
	static char lastC = ' ';
	if(s != lastS || m != lastM || h != lastH || c != lastC)
	{
		CString txt;
		txt.Format("%d %d' %.2f'' %c", h, m, s, c);
		m_longitude.SetWindowText(txt);
		m_longitude.Invalidate(TRUE);
		lastH = h;
		lastM = m;
		lastS = s;
		lastC = c;
	}
}

void CGPSDlg::DisplayLongitude(D64 lon, U08 c)
{
	CString txt;
	txt.Format("%d %d' %.2f'' %c", (int)(lon / 100.0), (int)lon - (int)(lon / 100.0) * 100,
		(lon - (int)lon) * 60.0, c);
	m_longitude.SetWindowText(txt);
	//m_longitude.Invalidate(TRUE);
}

void CGPSDlg::DisplayLatitude(D64 lat, U08 c)
{
	CString txt;
	txt.Format("%d %d' %.2f'' %c", (int)(lat / 100.0), (int)lat - (int)(lat / 100.0) * 100,
		(lat - (int)lat) * 60.0, c);
	m_latitude.SetWindowText(txt);
	m_latitude.Invalidate(TRUE);
}

void CGPSDlg::DisplayAltitude(D64 alt)
{
	CString txt;
	txt.Format("%5.2lf", alt);
	m_altitude.SetWindowText(txt);
	m_altitude.Invalidate(TRUE);
}

void CGPSDlg::DisplayHdop(D64 hdop)
{
	CString txt;
	txt.Format("%5.2lf", hdop);
	m_hdop.SetWindowText(txt);
	m_hdop.Invalidate(TRUE);
}

void CGPSDlg::DisplayLatitude(int h, int m, double s, char c)
{
	static int lastH= 0, lastM = 0;
	static double lastS = 0.0;
	static char lastC = ' ';
	if(s != lastS || m != lastM || h != lastH || c != lastC)
	{
		CString txt;
		txt.Format("%d %d' %.2f'' %c", h, m, s, c);
		m_latitude.SetWindowText(txt);
		//m_latitude.Invalidate(TRUE);
		lastH = h;
		lastM = m;
		lastS = s;
		lastC = c;
	}
}

void CGPSDlg::ShowLongitudeLatitude(void)
{
	DisplayLongitude((int)( m_gpggaMsg.Longitude / 100.0), 
			(int)m_gpggaMsg.Longitude - (int)(m_gpggaMsg.Longitude / 100.0) * 100,
			((m_gpggaMsg.Longitude - (int)m_gpggaMsg.Longitude) * 60.0),
			m_gpggaMsg.Longitude_E_W);
	DisplayLatitude((int)( m_gpggaMsg.Latitude / 100.0), 
			(int)m_gpggaMsg.Latitude - (int)(m_gpggaMsg.Latitude / 100.0) * 100,
			((m_gpggaMsg.Latitude - (int)m_gpggaMsg.Latitude) * 60.0),
			m_gpggaMsg.Latitude_N_S);
}

void CGPSDlg::ShowPDOP(bool reset)
{
	static F32 lastPdop = -99999.0F;
	if(reset)
	{
		lastPdop = -99999.0F;
	}

	F32 pdop = m_gpggaMsg.HDOP;
	if(pdop != lastPdop)
	{
		CString txt;
		txt.Format("%.2f", pdop);
		m_hdop.SetWindowText(txt);
		m_hdop.Invalidate(TRUE);
		lastPdop = pdop;
	}
}

void CGPSDlg::ShowSpeed(bool reset)
{
	static F32 lastSpeed = -99999.0F;
	F32 speed = 0.0F;
	if(reset)
	{
		lastSpeed = -99999.0F;
	}
	if(m_gpvtgMsg.SpeedKmPerHur != 0.0F)
	{
		speed = m_gpvtgMsg.SpeedKmPerHur;
	}
	else if(m_gprmcMsg.SpeedKnots != 0.0F)
	{
		speed = (F32)(m_gprmcMsg.SpeedKnots * 1.85567);
	}

	if(speed != lastSpeed)
	{
		CString txt;
		txt.Format("%.2f", speed);
		m_speed.SetWindowText(txt);
		m_speed.Invalidate(TRUE);
		lastSpeed = speed;
	}
}

void CGPSDlg::DisplaySpeed(D64 speed)
{
	CString txt;
	txt.Format("%.2lf", speed);
	m_speed.SetWindowText(txt);
	m_speed.Invalidate(TRUE);
}

void CGPSDlg::DisplayDirection(D64 direction)
{
	CString txt;
	txt.Format("%.2lf", direction);
	m_direction.SetWindowText(txt);
	m_direction.Invalidate(TRUE);
}

void CGPSDlg::DisplayStatus(GnssData::QualityMode m)
{
	if(GnssData::Unlocated == m)
	{
		if(m_initTtff)
		{
			m_ttffCount = 0;
			m_initTtff = false;
		}	
		else
		{
			m_ttffCount++;
		}
		SetTTFF(m_ttffCount);
		m_setTtff = true;
	}

	const char * statusStrings[] = {
		"",
		"Position fix unavailable!",
		"Estimated mode!",
		"DGPS mode",
		"PPS mode, fix valid!",
		"Position fix 2D.",
		"Position fix 3D.",
		"Survey-in",
		"Static-Mode",
		"Data not Valid!",
		"Autonomous mode!",
		"DGPS mode!",
	};

	CString txt = statusStrings[(int)m];
	m_fixed_status.SetRedraw(FALSE);
	switch(m)
	{
	case GnssData::EstimatedMode:
	case GnssData::DgpsMode:
	case GnssData::PpsMode:
	case GnssData::PositionFix2d:
	case GnssData::PositionFix3d:
	case GnssData::SurveyIn:
	case GnssData::StaticMode:
	case GnssData::AutonomousMode:
	case GnssData::DgpsMode2:
		m_fixed_status.SetTextColor(RGB(0, 0, 255));
		if(m_setTtff)
		{				
			m_setTtff = false;
			m_initTtff = true;
		}
		break;
	case GnssData::DataNotValid:
		m_fixed_status.SetTextColor(RGB(255,0,0));
		break;
	case GnssData::Unlocated:
		m_fixed_status.SetTextColor(RGB(255, 0, 0));
		break;
	default:
		ASSERT(FALSE);
		break;
	}
	m_fixed_status.SetRedraw(TRUE);
	m_fixed_status.SetText(txt);
	m_fixed_status.Invalidate(FALSE);
}

void CGPSDlg::ShowAltitude(bool reset)
{
	static F32 lastAlt = -99999.0F;
	if(reset)
	{
		lastAlt = -99999.0F;
	}

	F32 alt = m_gpggaMsg.Altitude;
	if(alt != lastAlt)
	{
		CString txt;
		txt.Format("%.2f", alt);
		m_altitude.SetWindowText(txt);
		m_altitude.Invalidate(TRUE);
		lastAlt = alt;
	}
}

void CGPSDlg::ShowKNumber()
{
	CString temp;
	m_kNumList.DeleteAllItems();
	//m_kNumList
	if(m_gnss.gnss_in_view >0)
	{
		for(int i=0;i<m_gnss.gnss_in_view;i++)
		{
			temp.Format("%d",m_gnss.sate[i].k_num);
			m_kNumList.InsertItem(m_kNumList.GetItemCount(),temp);
			temp.Format("%d",m_gnss.sate[i].slot_num);
			m_kNumList.SetItemText(m_kNumList.GetItemCount()-1,1,temp);
			temp.Format("%d",m_gnss.sate[i].snr);
			m_kNumList.SetItemText(m_kNumList.GetItemCount()-1,2,temp);
		}
	}
}

void CGPSDlg::ShowDirection(bool reset)
{
	static F32 lastDir = -99999.0F;
	F32 dir = 0.0F;
	
	if(reset)
	{
		lastDir = -99999.0F;
	}
	if(m_gprmcMsg.TrueCourse != 0.0F)
	{
		dir = m_gprmcMsg.TrueCourse;
	}
	else if(m_gpvtgMsg.TrueCourse != 0.0F)
	{
		dir = m_gpvtgMsg.TrueCourse;
	}

	if(dir != lastDir)
	{
		CString txt;
		txt.Format("%.2f", dir);
		m_direction.SetWindowText(txt);
		m_direction.Invalidate(TRUE);
		lastDir = dir;
	}
}

void CGPSDlg::ShowStatus()
{ 
	enum QualityMode {
		Uninitial = 0,
		Unlocated,
		EstimatedMode,
		DgpsMode,
		PpsMode,
		PositionFix2d,
		PositionFix3d,

		SurveyIn,
		StaticMode,
		DataNotValid,
		AutonomousMode,
		DgpsMode2,
	};
	static QualityMode lastMode = Uninitial;
	QualityMode mode = Unlocated;
	U08 gpsInd = 0, gnssInd = 0;

	if(m_gpggaMsg.GPSQualityIndicator > 0xFF)
	{
		gpsInd = m_gpggaMsg.GPSQualityIndicator >> 8;
		gnssInd = m_gpggaMsg.GPSQualityIndicator & 0xFF;
	}
	else
	{
		gpsInd = m_gpggaMsg.GPSQualityIndicator & 0xFF;
		gnssInd = 0;
	}

	if(gpsInd || gnssInd)
	{		
		if(gpsInd=='E' || gnssInd=='E')
		{
			mode = EstimatedMode;	
		}
		else if(gpsInd=='D' || gnssInd=='D')
		{
			mode = DgpsMode;	
		}
		else if(gpsInd=='3')
		{
		}
		else if(gpsInd=='2')
		{
			mode = DgpsMode;		
		}
		else if(gpsInd == 'A' || gnssInd == 'A' || gpsInd == '1')
		{				
	
			if(m_gpgsaMsg.Mode==2 || m_glgsaMsg.Mode==2 || m_bdgsaMsg.Mode==2)
			{
				mode = PositionFix2d;	
			}
			else if(m_gpgsaMsg.Mode==3 || m_glgsaMsg.Mode==3 || m_bdgsaMsg.Mode==3)
			{
				mode = PositionFix3d;	
			}
			else if(m_gpgsaMsg.Mode==4 || m_glgsaMsg.Mode==4 || m_bdgsaMsg.Mode==4)
			{
				mode = SurveyIn;	
			}
			else if(m_gpgsaMsg.Mode==5 || m_glgsaMsg.Mode==5 || m_bdgsaMsg.Mode==5)
			{
				mode = StaticMode;	
			}
		}
	}
	else if(m_gprmcMsg.ModeIndicator)
	{		
		if(m_gprmcMsg.ModeIndicator=='N')
		{
				mode = DataNotValid;	
		}
		else if(m_gprmcMsg.ModeIndicator=='A')
		{
				mode = AutonomousMode;	
		}
		else if(m_gprmcMsg.ModeIndicator=='D')
		{
				mode = DgpsMode2;	
		}
		else if(m_gprmcMsg.ModeIndicator=='E')
		{
				mode = EstimatedMode;	
		}
	}
	if(Unlocated==mode)
	{
		if(m_initTtff)
		{
			m_ttffCount = 0;
			m_initTtff = false;
		}	
		else
		{
			m_ttffCount++;
		}
		SetTTFF(m_ttffCount);
		m_setTtff = true;
	}


	if(mode != lastMode)
	{
		const char * statusStrings[] = {
			"",
			"Position fix unavailable!",
			"Estimated mode!",
			"DGPS mode",
			"PPS mode, fix valid!",
			"Position fix 2D.",
			"Position fix 3D.",
			"Survey-in",
			"Static-Mode",
			"Data not Valid!",
			"Autonomous mode!",
			"DGPS mode!",
		};
		CString txt = statusStrings[mode];
		lastMode = mode;
		m_fixed_status.SetRedraw(FALSE);
		switch(mode)
		{
		case EstimatedMode:
		case DgpsMode:
		case PpsMode:
		case PositionFix2d:
		case PositionFix3d:
		case SurveyIn:
		case StaticMode:
		case AutonomousMode:
		case DgpsMode2:
			m_fixed_status.SetTextColor(RGB(0, 0, 255));
			if(m_setTtff)
			{				
				m_setTtff = false;
				m_initTtff = true;
			}
			break;
		case DataNotValid:
			m_fixed_status.SetTextColor(RGB(255,0,0));
			break;
		case Unlocated:
			m_fixed_status.SetTextColor(RGB(255, 0, 0));
			break;
		default:
			ASSERT(FALSE);
			break;
		}
		m_fixed_status.SetRedraw(TRUE);
		m_fixed_status.SetText(txt);
		m_fixed_status.Invalidate(FALSE);
	}
}

#include "SetupDialog.h"
void CGPSDlg::OnFileSetup()
{	
	CSetupDialog setupDlg;
	setupDlg.SetSetting(&g_setting);
	if(setupDlg.DoModal()==IDOK)
	{
		g_setting.Save();
		m_nDownloadBaudIdx = g_setting.boostBaudIndex;
		m_SetOriginBtn.EnableWindow(!g_setting.specifyCenter);
		if(g_setting.specifyCenter)
		{
			g_scatterData.SetOrigin();
		}
	}
}

void CGPSDlg::OnFileExit()
{	
	if(!m_isPressCloseButton)
	{
		Terminate();	
	}
	CDialog::OnCancel();		
}

void CGPSDlg::OnClose()
{
#if NMEA_INPUT
	g_nmeaInputTerminate = 1;
	::WaitForSingleObject(m_nmeaPlayThread, INFINITE);
	//Sleep(1000);
#else
	if(!m_isPressCloseButton)
		Terminate();	
#endif
	g_setting.Save();
	CDialog::OnClose();
}

void CGPSDlg::OnHelpAbout()
{
	CAboutDlg dlg;
	dlg.DoModal();
}

bool CGPSDlg::CloseOpenUart()
{	
	m_serial->Close();
	delete m_serial;
	m_serial = NULL;	
	Sleep(200);
	m_serial = new CSerial;
	if(!m_serial->Open(GetComPort()+1, GetBaudrate()))
	{
		AfxMessageBox("Open uart Failed!");
		return false;
	}
	return true;
}

void CGPSDlg::OnBinaryDumpData()
{
	//CMenu::GetMenuItemID
	CMenu *pMenu = CMenu::FromHandle(::GetMenu(this->m_hWnd));
	UINT checkFlag = 0;
	if(m_bShowBinaryCmdData)
	{
		m_bShowBinaryCmdData = FALSE; 
		checkFlag = MF_BYCOMMAND | MF_UNCHECKED;
	}
	else
	{
		m_bShowBinaryCmdData = TRUE; 
		checkFlag = MF_BYCOMMAND | MF_CHECKED;
	}
	UINT n =pMenu->CheckMenuItem(ID_BINARY_DUMPDATA, checkFlag);
	
	pMenu->Detach();
}

void CGPSDlg::ClearQue()
{	
	m_serial->ClearQueue();
}

CGPSDlg::DataLogType CGPSDlg::GetDataLogType(U16 word0)
{
	U08 type = word0 >> 12 ;
	switch(type)
	{
	case 0x2:
		return FIXMULTI;
	case 0x0c:
		return FIXPOIMULTI;
	case 0x4:
		return FIXFULL;
	case 0x5:
		return FIXFULL2;
	case 0x8:
		return FIXINC;
	case 0x9:
		return FIXINC2;
	case 0x6:
		return FIXPOI;
	case 0x7:
		return FIXPOI2;
	default:
		return FIXNONE;
	}
}

UINT ConvertLogThread(LPVOID pParam)
{
	CGPSDlg::gpsDlg->DataLogDecompress(1);
	return 0;
	//_endthread();
}

void CGPSDlg::OnLoggerConvert()
{
	bool old_status = m_isConnectOn;
	if(old_status)
	{
		TerminateGPSThread();
	}

	CString fileName("Data.log");	
	CFileDialog dlgFile(false, _T("log"), fileName, OFN_HIDEREADONLY, _T("ALL Files (*.*)|*.*||"), this);
	INT_PTR nResult = dlgFile.DoModal();
	fileName = dlgFile.GetPathName();	
	CFileException ef;
	if(Utility::IsFileExist(fileName))
	{
		try
		{
			if(nResult == IDOK)
			{  	
				if(!m_convertFile.Open(fileName, CFile::modeRead,&ef))
				{
					ef.ReportError();
					return;
				}		   	
				::AfxBeginThread(ConvertLogThread, 0);	
			}
			else if(nResult == IDCANCEL)
			{  		
				if(!m_isPressCloseButton)
				{
					if(old_status)
						CreateGPSThread();
				}
				return;			
			}
		}
		catch(CFileException *fe)
		{
			fe->ReportError();
			fe->Delete();
			if(old_status)
				CreateGPSThread();
			return;
		}	
	}
	else
	{
		if(old_status)
			CreateGPSThread();
	}
	fileName.ReleaseBuffer();  
}

//datalog decompress
void CGPSDlg::DataLogDecompress(bool mode)
{
	CString strTxt;
	U16   word, word0;
	char  Log[1024] = {0};
	DataLogType   type;
	BYTE  buffer[0x1000];
	ULONGLONG dwBytesRemaining = m_convertFile.GetLength();	
	ULONGLONG startAddress = 0;
	CFile fDecompress;	
	mode = (m_dataLogDecompressMode==1);

	CSkyTraqKml1 kml;
#if TWIN_DATALOG
	CSkyTraqKml1 kml2;
#endif

	CString filename = m_convertFile.GetFileName() + 'g';
	//filename+="g";
	fDecompress.Open(filename,CFile::modeReadWrite|CFile::modeCreate);

	CString logg_filename = fDecompress.GetFilePath();
	//filename+=".kml";	
	//FKml.Open(filename,CFile::modeReadWrite|CFile::modeCreate);

	//sprintf_s(Log, sizeof(Log), "WNO TOW Time Date DECEF_X DECEF_Y DECEF_Z ECEF_X ECEF_Y ECEF_Z Speed Longitude Latitude Altitude Mode\r\n");
	//fDecompress.Write(Log,(U16)strlen(Log)-1);

	strTxt = "WNO TOW Time Date DECEF_X DECEF_Y DECEF_Z ECEF_X ECEF_Y ECEF_Z Speed Longitude Latitude Altitude Mode\r\n";
	fDecompress.Write(strTxt, strTxt.GetLength() - 1);

	FIX_FULL    fix_full;
	FIX_INC     fix_inc;
	FIX_FULL_MULTI_HZ_DATA fix_multi;

	POS_FIX_REC DataLog;
	UTC_T       utc;

	int write_count = 0;
	int file_tail = 0;


	int _10Hz_count=0;
	int start_tow=0;

	double lat,lon,alt;
	U32 temp_lat,temp_lon,temp_alt;

	geoid geo_id;
	D64 tow;

	while(dwBytesRemaining)
	{

		CString tmp_file;
		CString tmp_name = m_convertFile.GetFileName();
		int find = tmp_name.Find(".");
#if TWIN_DATALOG
		tmp_file.Format("%s%d_datalogger1%s",tmp_name.Mid(0,find),file_tail,".kml");
		kml.init(tmp_file,0x0000ff);
		tmp_file.Format("%s%d_datalogger2%s",tmp_name.Mid(0,find),file_tail,".kml");
		kml2.init(tmp_file,0xff0000);
#else
		tmp_file.Format("%s%d%s",tmp_name.Mid(0,find),file_tail,".kml");
		kml.init(tmp_file,0x0000ff);
		//FKml.Open(tmp_file,CFile::modeReadWrite|CFile::modeCreate);	
#endif		
		while(dwBytesRemaining)
		{
			startAddress = m_convertFile.GetPosition();
			UINT nBytesRead = m_convertFile.Read(buffer,2);
			memcpy(&word,buffer,nBytesRead);
			if(word == 0xffff)
			{
				//It is Empty! 
				break;
			}
			word0  = word>>8 &0xff;
			word0 |= word<<8 &0xff00;
			_cprintf( "%x " , word0 );

			dwBytesRemaining-=nBytesRead;

			type = GetDataLogType(word0);
			switch(type)
			{
			case FIXMULTI:
			case FIXPOIMULTI:
				memcpy(&fix_multi,&word0,sizeof(word0));	
				nBytesRead = m_convertFile.Read(buffer, 18); 
				memcpy(&fix_multi.word[1], buffer, nBytesRead);	
				if(mode)
				{
					for(int i=1;i<10;i++)
					{
						word0 = fix_multi.word[i];
						fix_multi.word[i]  = word0>>8 &0xff;
						fix_multi.word[i] |= word0<<8 &0xff00;
					}
				}
				DataLog.V    = (fix_multi.word[1] + ((fix_multi.word[3] & 0xC000) << 2)) / 100.0f;				  
				DataLog.WNO  = fix_multi.word[0] & 0x3ff;

				tow = ((fix_multi.word[3] & 0x3fff)<<16 | fix_multi.word[2])/1000.0 + 0.005;
				DataLog.TOW  = (U32)tow;		
				if(tow > 186615.9)
				{
					int a = 0;
				}

				temp_lat = fix_multi.word[5]<<16 | fix_multi.word[4];
				//lat = FixedPointToSingle(temp_lat,20);
				temp_lon = fix_multi.word[7]<<16 | fix_multi.word[6];
				//lon = FixedPointToSingle(temp_lon,20);
				temp_alt = fix_multi.word[9]<<16 | fix_multi.word[8];
				//alt = FixedPointToSingle(temp_alt,7);
				break;
			case FIXFULL:
			case FIXPOI:
				memcpy(&fix_full,&word0,sizeof(word0));			
				nBytesRead = m_convertFile.Read(buffer,16); 
				memcpy(&fix_full.word[1], buffer, nBytesRead);	

				if(mode)
				{
					for(int i=1;i<9;i++)
					{
						word0 = fix_full.word[i];
						fix_full.word[i]  = word0>>8 &0xff;
						fix_full.word[i] |= word0<<8 &0xff00;
					}
				}

				DataLog.V    = (float)(fix_full.word[0] & 0x7ff);
				DataLog.WNO  = fix_full.word[1]     &0x3ff;
				DataLog.TOW  = fix_full.word[1]>>12 &0xf;		
				DataLog.TOW |= fix_full.word[2]<<4;

				tow = DataLog.TOW;
				//
				DataLog.ECEF_X = buffer[6]<<24 | buffer[7]<<16 | buffer[4]<<8 | buffer[5] ;
				DataLog.ECEF_Y = buffer[10]<<24 | buffer[11]<<16 | buffer[8]<<8 | buffer[9] ;
				DataLog.ECEF_Z = buffer[14]<<24 | buffer[15]<<16 | buffer[12]<<8 | buffer[13] ;

				DataLog.DECEF_X=0;
				DataLog.DECEF_Y=0;
				DataLog.DECEF_Z=0;

				break;
			case FIXINC:
				memcpy(&fix_inc,&word0,sizeof(word0));		
				nBytesRead = m_convertFile.Read(buffer, 6);
				memcpy(&fix_inc.word[1], buffer, nBytesRead);

				if(mode)
				{			
					for(int i=1;i<4;i++)
					{			
						word0 = fix_inc.word[i];			
						fix_inc.word[i]  = word0>>8 &0xff;
						fix_inc.word[i] |= word0<<8 &0xff00;
					}
				}			
				DataLog.V        = (float)(fix_inc.word[0] & 0x7ff);
				DataLog.DTOW     = fix_inc.word[1]; 			
				DataLog.DECEF_X  = fix_inc.word[2]>>6    &0x3ff;
				DataLog.DECEF_Y  = fix_inc.word[2]       &0x3f;
				DataLog.DECEF_Y |= (fix_inc.word[3] >>12  &0xf)<<6;
				DataLog.DECEF_Z  = fix_inc.word[3]       &0x3ff;			
				if(DataLog.DECEF_X>511)	DataLog.DECEF_X = 511 - DataLog.DECEF_X;
				if(DataLog.DECEF_Y>511)	DataLog.DECEF_Y = 511 - DataLog.DECEF_Y;
				if(DataLog.DECEF_Z>511)	DataLog.DECEF_Z = 511 - DataLog.DECEF_Z;

				DataLog.TOW+=DataLog.DTOW;

				tow = DataLog.TOW;
				DataLog.ECEF_X+=DataLog.DECEF_X;
				DataLog.ECEF_Y+=DataLog.DECEF_Y;
				DataLog.ECEF_Z+=DataLog.DECEF_Z;
				break;
			default:
				break;
			}

			int X,Y,Z;
			if (type == FIXMULTI || type == FIXPOIMULTI)
			{
				LLA_T lla;
				POS_T pos;

				lat = FixedPointToSingle(temp_lat,20);
				lon = FixedPointToSingle(temp_lon,20);
				alt = FixedPointToSingle(temp_alt,7);

				lla.lat = lat / R2D;
				lla.lon = lon / R2D;
				lla.alt = (F32)alt;
				COO_geodetic_to_cartesian(&lla, &pos);

				DataLog.ECEF_X = (S32)pos.px;
				DataLog.ECEF_Y = (S32)pos.py;
				DataLog.ECEF_Z = (S32)pos.pz;

				DataLog.DECEF_X=0;
				DataLog.DECEF_Y=0;
				DataLog.DECEF_Z=0;

				X=DataLog.ECEF_X;
				Y=DataLog.ECEF_Y;
				Z=DataLog.ECEF_Z;

			}else
			{
				
			X=DataLog.ECEF_X;
			Y=DataLog.ECEF_Y;
			Z=DataLog.ECEF_Z;

			double p = sqrt(pow((double)X,2.0)+pow((double)Y,2.0));
			double theta = atan(Z*WGS84a/(p*WGS84b));
			// e square
			double e2 = (pow(WGS84a,2.0)-pow(WGS84b,2.0))/pow(WGS84a,2.0);
			// e' square
			double e2p = (pow(WGS84a,2.0)-pow(WGS84b,2.0))/pow(WGS84b,2.0);

			// latitude : phi (rad.)
			lat = atan2((Z+e2p*WGS84b*pow(sin(theta),3.0)),(p-e2*WGS84a*pow(cos(theta),3.0)));
			//double lat = atan((Z+e2p*WGS84b*pow(sin(theta),3.0))/(p-e2*WGS84a*pow(cos(theta),3.0)));
			// longitude : lambda (rad.)
			//double lon = atan((double)Y / (double)X )+PI;
			lon = atan2((double)Y , (double)X );

			double N = WGS84a/(sqrt(1-e*e*sin(lat)*sin(lat)));
			alt = p/cos(lat)-N;

			lat = lat*180/PI;
			lon = lon*180/PI;
		}


			alt = alt - geo_id.GEO_calc_geoid_height(lat,lon);

			if(lon >= 130 || lon <= 120)
				TRACE("%.4f,%.4f\r\n",lat,lon);


			convert_gps_time_to_utc((S16) DataLog.WNO+1024,tow, &utc );

			LL kml_lla;
			kml_lla.lat = lat;
			kml_lla.lon = lon;
			kml_lla.alt = alt;
			kml_lla.utc.hour = utc.hour;
			kml_lla.utc.minute = utc.minute;
			kml_lla.utc.sec = utc.sec;
			kml_lla.speed = DataLog.V;

			if(type == FIXPOI)
			{
				kml.push_one_poi(lon,lat);
			}
#if TWIN_DATALOG
			else if(type == FIXPOI2)
			{
				kml2.push_one_poi(lon,lat);
			}
			else if(type == FIXFULL_POI || type == FIXINC2)
			{
				kml2.push_one_point(lon,lat);
			}
#endif
			else
			{
				kml.push_one_point(lon,lat);

			}
			//TRACE("type=%d\r\n",type);
			//ConvertGpsTimeToUtc((S16) DataLog.WNO+1024,(D64) DataLog.TOW, &utc );
			//	_cprintf("%d %d %d %d %d %.0f\n\n", utc.year, utc.month, utc.day, utc.hour, utc.minute, utc.sec);

			//sprintf_s(Log, sizeof(Log), "%d %.2f %d:%d:%.2f %d/%d/%d %d %d %d %d %d %d %.2f %.6f %.6f %.6f %d\r\n",
			//	DataLog.WNO+1024,tow,utc.hour,utc.minute,utc.sec,utc.month,utc.day,utc.year,
			//	DataLog.DECEF_X,DataLog.DECEF_Y,DataLog.DECEF_Z,X,Y,Z,DataLog.V, lon,lat,alt,type);
			strTxt.Format("%.2f", utc.sec);
			if(strTxt=="60.00")
			{
				strTxt.Format("%.6f", utc.sec);
			}
			strTxt.Format("%d %.2f %02d:%02d:%02.2f %2d/%2d/%d %d %d %d %d %d %d %.2f %.6f %.6f %.6f %d 0x%08X\r\n",

				DataLog.WNO+1024,	//%d 
				tow,		//%.2f
				utc.hour,	//%d:
				utc.minute,	//%d:
				utc.sec,	//%.2f
				utc.month,
				utc.day,
				utc.year,
				DataLog.DECEF_X,DataLog.DECEF_Y,DataLog.DECEF_Z,X,Y,Z,DataLog.V, lon,lat,alt,type, (U32)startAddress);
			fDecompress.Write(strTxt, strTxt.GetLength() - 1);
			dwBytesRemaining -= nBytesRead;	
			// ---------------------------------------------
			write_count++;
			if( write_count > 65000)
			{
				write_count = 0;
				file_tail++;
				break;
			}
		}

		kml.finish();
#if TWIN_DATALOG
		kml2.finish();
#endif
		if(word == 0xffff)
		{
			//It is Empty! 
			break;
		}
	}

	m_convertFile.Close();
	fDecompress.Close();
	//FKml.Close();
	//delete fDecompress;
	Sleep(100);
#ifndef SOARCOMM
	log2nmea conv2rmc;
	conv2rmc.set_LoggFile(logg_filename);
	conv2rmc.convert2nmea();
#endif
	AfxMessageBox("Decompress is completed!");	
	if(!m_isPressCloseButton) 
	{
		SetMode();
		CreateGPSThread();
	}
}
/*
void CGPSDlg::WritePOIPath(CFile& nmeaFile ,vector<LLA_T> *lst )
{

	vector<LLA_T>::iterator iter;
	int id = 1;
	char buff[1024];

	for(iter = lst->begin();iter != lst->end();iter++)
	{
		LLA_T lla = *iter;
		sprintf_s(buff, sizeof(buff), "<Placemark id=\"POI%d\">\r\n    <styleUrl>#POI_STYLE</styleUrl>\r\n    <name>POI%d</name>\r\n    <LookAt>\r\n",id,id++);
		nmeaFile.Write(buff,(U16)strlen(buff));
		sprintf_s(buff, sizeof(buff), "    <longitude>%.6f</longitude>\r\n    <latitude>%.6f</latitude>\r\n    </LookAt>\r\n        <Point>\r\n",lla.lon,lla.lat);
		nmeaFile.Write(buff,(U16)strlen(buff));
		sprintf_s(buff, sizeof(buff), "        <coordinates>%.6f,%.6f</coordinates>\r\n    </Point>\r\n</Placemark>\r\n",lla.lon,lla.lat),
			m_nmeaFile.Write(buff,(U16)strlen(buff));
	}
	strcpy_s(buff, sizeof(buff),"</Folder>\r\n");
	nmeaFile.Write(buff,(U16)strlen(buff));
}
*/
UINT DecompressThread(LPVOID pParam)
{
	CGPSDlg::gpsDlg->DataLogDecompress(0);
	return 0;
	//_endthread();	
}

void CGPSDlg::OnCovDecopre()
{
	if(m_isConnectOn)
	{
		TerminateGPSThread();
	}

	CString fileName("Data.log");	
	CFileDialog dlgFile(true, _T("*.log"), NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, _T("*.log|*.log||"), this);
	INT_PTR nResult = dlgFile.DoModal();
	try
	{
		if(nResult == IDOK)
		{  			
			fileName = dlgFile.GetPathName();	
			m_convertFile.Open(fileName, CFile::modeRead);	
			::AfxBeginThread(DecompressThread, 0);	
		}
		else if(nResult == IDCANCEL)
		{  
			if(!m_isPressCloseButton)
				CreateGPSThread();
			return;			
		}
	}
	catch(CFileException *fe)
	{
		fe->ReportError();
		fe->Delete();
		return;
	}	
}

void  CGPSDlg::ConvertGpsTimeToUtc( S16 wn, D64 tow, UTC_T* utc_p )
{
	// default leap secs has passed away
	//const S16 DEFAULT_PRIOR_LEAP_SECS = 15;
	// GPS Time start at 1980 Jan. 5/6 mid-night
	const S16 INIT_UTC_YEAR = 1980;     

	const S16 DAYS_PER_YEAR = 365;
	const S16 DAYS_PER_4_YEARS = (DAYS_PER_YEAR*4 + 1);  // plus one day for leap year

	const S16 day_of_year_month_table[] = 
	{ 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };
	const S16 day_of_leap_year_month_table[] = 
	{ 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 };

	S16 i;
	S32 tow_int;
	D64 tow_frac;
	S32 total_utc_sec, total_utc_day;
	S32 passed_leap_days, passed_utc_years,
		day_of_utc_year, leap_days_of_prev_years;
	S32 sec_of_day;
	const S16* month_tbl_p = day_of_year_month_table;

	tow_int  = (U32)floor( tow );
	tow_frac = tow - (D64) tow_int;
	total_utc_sec = 604800L*wn + tow_int - DefaultLeapSeconds;
	total_utc_day = total_utc_sec / 86400L;
	sec_of_day = total_utc_sec - 86400L * total_utc_day;

	passed_leap_days = (total_utc_day + DAYS_PER_4_YEARS 
		- day_of_leap_year_month_table[2] + 5) / DAYS_PER_4_YEARS;
	passed_utc_years = (total_utc_day + 5 - passed_leap_days) / 365;
	leap_days_of_prev_years = (passed_utc_years + 3) / 4;

	day_of_utc_year = total_utc_day + 5 - passed_utc_years*DAYS_PER_YEAR
		- leap_days_of_prev_years;

	utc_p->year = 1980 + (S16)passed_utc_years;
	if( (utc_p->year & 0x3) == 0 )  // utc->year % 4
		month_tbl_p = day_of_leap_year_month_table;  // this year is leap year

	for( i=1; i<13; i++ )
		if( day_of_utc_year < month_tbl_p[i] )
			break;

	utc_p->month = i;
	utc_p->day   = (S16)day_of_utc_year - month_tbl_p[i-1] + 1;

	utc_p->hour = (U08)(sec_of_day / 3600);
	if( utc_p->hour > 23 )
		utc_p->hour = 23;

	utc_p->minute = (U08)((sec_of_day - utc_p->hour * 3600) / 60);
	if( utc_p->minute > 59 )
		utc_p->minute = 59;

	utc_p->sec = (F32)(sec_of_day - utc_p->hour*3600L - utc_p->minute*60L)
		+ (F32)tow_frac;
}

UINT LogClearControlThread(LPVOID pParam)
{
	U08 message[8];
	memset(message, 0, 8);   		    
	message[0]=(U08)0xa0;
	message[1]=(U08)0xa1;
	message[2]=0;
	message[3]=1;
	message[4]=0x19; //msgid
	unsigned char checksum = 0;
	for(int i=0;i<(int)message[3];i++)
		checksum^=message[i+4];	
	message[5]=checksum;
	message[6]=(U08)0x0d;
	message[7]=(U08)0x0a;
//	CGPSDlg::gpsDlg->LogClear( message );
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(message, 8, "Log Clear Control Successful...");

	return 0;

}

void CGPSDlg::OnDatalogClearControl()
{
	if(!CheckConnect())
	{
		return;
	}

	m_inputMode = 0;
	if(AfxMessageBox("Clear datalog?", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) == IDYES)
	{
		AfxBeginThread(LogClearControlThread, 0);
	}
	else
	{
		SetMode();
		CreateGPSThread();
	}
}

void CGPSDlg::ExecuteConfigureCommand(U08 *cmd, int size, LPCSTR msg, bool restoreConnect/* = true*/)
{
	ClearQue();
	SendToTarget(cmd, size, msg, true);
	if(restoreConnect)
	{
		SetMode();
		CreateGPSThread();
	}
}

UINT LogConfigureControlThread(LPVOID pParam)
{	
	CGPSDlg::gpsDlg->LogConfigure();
	return 0;
}

void CGPSDlg::OnDatalogLogconfigurecontrol()
{	
	if(!CheckConnect())return;
	m_inputMode = 0 ;
	CLogFilterDlg* pLFDlg = new CLogFilterDlg;
	INT_PTR ret = pLFDlg->DoModal();
	_cprintf("%d %d %d %d %d %d %d %d\n",	        
		m_logFlashInfo.min_time,
		m_logFlashInfo.max_time,
		m_logFlashInfo.min_distance,
		m_logFlashInfo.max_distance,
		m_logFlashInfo.min_speed,
		m_logFlashInfo.max_speed,
		m_logFlashInfo.datalog_enable,
		m_logFlashInfo.fifo_mode);
	if(ret == IDOK)
	{	
		::AfxBeginThread(LogConfigureControlThread, 0);
	}
	else
	{
		SetMode();
		CreateGPSThread();
	}
	delete pLFDlg;
	pLFDlg = NULL;

}

void CGPSDlg::LogConfigure()
{	
	//WaitEvent();	
	U08 message[34];
	U08 msg[27];
	msg[0] = 0x18;

	U32toBuff(&msg[1],m_logFlashInfo.max_time);
	U32toBuff(&msg[5],m_logFlashInfo.min_time);
	U32toBuff(&msg[9],m_logFlashInfo.max_distance);
	U32toBuff(&msg[13],m_logFlashInfo.min_distance);
	U32toBuff(&msg[17],m_logFlashInfo.max_speed);
	U32toBuff(&msg[21],m_logFlashInfo.min_speed);
	//enable
	msg[25]= m_logFlashInfo.datalog_enable;
	//FIFO
	msg[26]= m_logFlashInfo.fifo_mode;

	int len = SetMessage2(message, msg, sizeof(msg));
	ExecuteConfigureCommand(message, len, "Log Configure Control Successful");
/*
	ClearQue();
	SendToTarget(message,len,"Log Configure Control Successful");	
	SetMode();
	CreateGPSThread();
*/
}

//設定送出去的封包
int CGPSDlg::SetMessage(U08* msg, int len)
{
	return SetMessage2(m_inputMsg, msg, len);
/*
	memset(m_inputMsg, 0, len+7); 
	m_inputMsg[0]=(U08)0xa0;
	m_inputMsg[1]=(U08)0xa1;
	m_inputMsg[2]=0;
	m_inputMsg[3]=len;
	for(int i = 0 ; i < len ; i++)
	{
		m_inputMsg[4+i] = *msg;
		msg++ ;
	}
	U08 checksum = 0;
	for(int j=0;j<(int)m_inputMsg[3];j++)
		checksum^=m_inputMsg[j+4];
	m_inputMsg[len+4]= checksum;
	m_inputMsg[len+5]=(U08)0x0d;
	m_inputMsg[len+6]=(U08)0x0a;

	return len+7 ;
*/
}

int CGPSDlg::SetMessage2(U08* dst, U08* src, int srcLen)
{
	dst[0] = 0xa0;
	dst[1] = 0xa1;
	dst[2] = 0;
	dst[3] = srcLen;
	U08 checkSum = 0;
	for(int i=0; i<srcLen; ++i)
	{
		dst[4 + i] = *src;
		checkSum ^= *src;
		++src;
	}

	dst[srcLen+4] = checkSum;
	dst[srcLen+5] = 0x0d;
	dst[srcLen+6] = 0x0a;
	return srcLen + 7;
}

void CGPSDlg::SetPort(U08 port, int mode)
{   		
	U08 messages[11] = {0};    

	messages[0] = 0xa0;
	messages[1] = 0xa1;
	messages[2] = 0;
	messages[3] = 4;
	messages[4] = 5; //msgid		
	messages[5] = 0; 
	messages[6] = port; 
	messages[7] = (U08)mode;

	U08 checksum = 0;
	for(int i=0; i<(int)messages[3]; ++i)
	{
		checksum ^= messages[i + 4];	
	}
	messages[8] = checksum;
	messages[9] = 0x0d;
	messages[10] = 0x0a;	
	for(int i=0; i<10; ++i)
	{
		if(SendToTarget(messages, 11, "", 1))
		{
			break;
		}
	}
	CloseOpenUart();
	m_serial->ResetPort(port);	
	m_BaudRateCombo.SetCurSel(port);
	Sleep(100);
}
/*
void CGPSDlg::SetPort_noAck(U08 port,int mode)
{   		
	U08 messages[11];    
	U08 i;	  
	memset(messages, 0, 11);
	messages[0]=(U08)0xa0;
	messages[1]=(U08)0xa1;
	messages[2]=0;
	messages[3]=4;
	messages[4]=5; //msgid		
	messages[5]=0; 
	messages[6]=port; 
	messages[7]=mode;
	U08 checksum = 0;
	for(i=0;i<(int)messages[3];i++)	checksum^=messages[i+4];	
	messages[8]=checksum;
	messages[9]=(U08)0x0d;
	messages[10]=(U08)0x0a;	
	SendToTargetNoAck(messages, 11);
	//#endif
	CloseOpenUart();

	m_serial->ResetPort(port);	
	m_BaudRateCombo.SetCurSel(port);
	Sleep(100);
}

void CGPSDlg::OnSoftwareimagedownloadLoaderimage()
{
	OnBnClickedDownload();
}
*/

UINT CigRgsThread(LPVOID pParam)
{
	U08 message[11];
	memset(message, 0, 11);   		    
	message[0]=(U08)0xa0;
	message[1]=(U08)0xa1;
	message[2]=0;
	message[3]=4;
	message[4]=0x70; //msgid
	message[5]=CGPSDlg::gpsDlg->m_ms;
	message[6]=CGPSDlg::gpsDlg->m_ns;
	message[7]=CGPSDlg::gpsDlg->m_pllDiv;
	U08 checksum = 0;
	for(int i=0;i<(int)message[3];i++)
		checksum^=message[i+4];	
	message[8]=checksum;
	message[9]=(U08)0x0d;
	message[10]=(U08)0x0a;		
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(message, 11, "Configure Register Successful...");
	return 0;
}
/* 
void CGPSDlg::ConfigureRegister(U08* message)
{	  
	//WaitEvent();
	SendToTarget(message, 11, "Configure Register Successful...");	
	SetMode();
	CreateGPSThread();
}

UINT GetRgsThread(LPVOID pParam)
{
	U08 message[12];
	memset(message, 0, 12);   		    
	message[0]=(U08)0xa0;
	message[1]=(U08)0xa1;
	message[2]=0;
	message[3]=5;
	message[4]=0x71; //msgid
	message[5]=(U08)(CGPSDlg::gpsDlg->m_regAddress>>24) &0xff;
	message[6]=(U08)(CGPSDlg::gpsDlg->m_regAddress>>16) &0xff;
	message[7]=(U08)(CGPSDlg::gpsDlg->m_regAddress>>8) &0xff;
	message[8]=(U08)CGPSDlg::gpsDlg->m_regAddress &0xff;
	U08 checksum = 0;
	for(int i=0;i<(int)message[3];i++)
		checksum^=message[i+4];	
	message[9]=checksum;
	message[10]=(U08)0x0d;
	message[11]=(U08)0x0a;		
	CGPSDlg::gpsDlg->GetRegister( message );
	//AfxEndThread(0);	
	return 0;
}
*/
//get register value
void CGPSDlg::OnBinaryGetrgister()
{	
	if(!m_isConnectOn)
	{
		AfxMessageBox("Please connect to Sky Traq GPS");
		return;
	}

	CGetRgsDlg dlg;
	INT_PTR ret = dlg.DoModal();
	if(ret == IDOK)
	{
		m_regAddress = dlg.address;			
		//AfxBeginThread(GetRgsThread, 0);	
		GenericQuery(&CGPSDlg::QueryRegister);
	}

}

bool CGPSDlg::TIMEOUT_METHOD(time_t start, time_t end)
{	
	if((end-start) > TIME_OUT_MS )
	{	
		AfxMessageBox("Timeout: GPS device no response.");
		return true;
	}
	return false;
}

bool CGPSDlg::TIMEOUT_METHOD_QUICK(time_t start,time_t end)
{	
	//TIMEOUT =  (U32)(end-start);
	return (end-start) > TIME_OUT_QUICK_MS;
}

void CGPSDlg::SetMode()
{
	switch(GetMsgType())
	{
	case Nooutput_Mode:
		m_inputMode = Nooutput_Mode;
		break;
	case Binary_Mode:
		m_inputMode = NMEA_Mode;
		break;
	case NMEA_Mode:
		m_inputMode = NMEA_Mode;
		break;
	default:
		break;
	}
}

bool CGPSDlg::CheckConnect()
{
	if(!m_isConnectOn)
	{
		AfxMessageBox("Please connect to Sky Traq GPS");
		return false;
	}
	else
	{
		TerminateGPSThread();		
		return true;
	}
}

void CGPSDlg::OnBnClickedClear()
{
	g_scatterData.Clear();
}

UINT demoagps_thread(LPVOID param)
{
	if(CGPSDlg::gpsDlg->CheckEphAndDownload())
	{
		CGPSDlg::gpsDlg->target_only_restart(2);
	}
	else
	{
		CGPSDlg::gpsDlg->SetMode(); 
		Sleep(200);
		CGPSDlg::gpsDlg->CreateGPSThread();
	}

	CGPSDlg::gpsDlg->m_bnt_warmstart.EnableWindow(1);
	CGPSDlg::gpsDlg->m_btn_coldstart.EnableWindow(1);
	return TRUE;
}


void CGPSDlg::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
{
	CDialog::OnInitMenuPopup(pPopupMenu, nIndex, bSysMenu);
	switch(m_inputMode)
	{
	case 0:
		pPopupMenu->EnableMenuItem(ID_CONFIGUREOUTPUTMESSAGETYPE_NOOUTPUT     ,1);
		pPopupMenu->EnableMenuItem(ID_CONFIGUREOUTPUTMESSAGETYPE_NMEAMESSAGE  ,0);
		pPopupMenu->EnableMenuItem(ID_CONFIGUREOUTPUTMESSAGETYPE_BINARYMESSAGE,0);
		break;
	case 1:
		pPopupMenu->EnableMenuItem(ID_CONFIGUREOUTPUTMESSAGETYPE_NOOUTPUT     ,0);
		pPopupMenu->EnableMenuItem(ID_CONFIGUREOUTPUTMESSAGETYPE_NMEAMESSAGE  ,0);
		pPopupMenu->EnableMenuItem(ID_CONFIGUREOUTPUTMESSAGETYPE_BINARYMESSAGE,1);
		break;
	case 2:
		pPopupMenu->EnableMenuItem(ID_CONFIGUREOUTPUTMESSAGETYPE_NOOUTPUT     ,0);
		pPopupMenu->EnableMenuItem(ID_CONFIGUREOUTPUTMESSAGETYPE_NMEAMESSAGE  ,1);
		pPopupMenu->EnableMenuItem(ID_CONFIGUREOUTPUTMESSAGETYPE_BINARYMESSAGE,0);
		break;
	default:
		break;
	}	
}

void CGPSDlg::OnBnClickedNoOutput()
{
	OnConfigureoutputmessagetypeNooutput();
	m_no_output.EnableWindow(0);
	m_nmea0183msg.EnableWindow(1);
	m_binarymsg.EnableWindow(1);
}

void CGPSDlg::OnBnClickedNmeaOutput()
{
	OnConfigureoutputmessagetypeNmeamessage();
	m_no_output.EnableWindow(1);
	m_nmea0183msg.EnableWindow(0);
	m_binarymsg.EnableWindow(1);
}

void CGPSDlg::OnBnClickedBinaryOutput()
{
	OnConfigureoutputmessagetypeBinarymessage();
	m_no_output.EnableWindow(1);
	m_nmea0183msg.EnableWindow(1);
	m_binarymsg.EnableWindow(0);
}

UINT DownloadThread(LPVOID pParam)
{
	//WaitForSingleObject(close_thread,INFINITE);	
	CGPSDlg::gpsDlg->Download();
	return 0;
}

//firmware dowmload

#ifdef GG12A
bool CGPSDlg::check_gg12a_format(const char *file_path)
{
	U08 check_patern[8] = {0xF4,0x0E,0xE0,0xB8,0x16,0xA0,0x01,0xA2};
	FILE *f = NULL;
	fopen_s(&f, file_path,"rb");
	if(f == NULL) return FALSE;

	fseek(f,0,SEEK_END);
	long file_size = ftell(f);

	if(file_size<8) return FALSE;

	fseek(f,file_size-8,SEEK_SET);

	U08 check_packet[8];
	fread(check_packet,1,sizeof(check_packet),f);
	fclose(f);

	if(memcmp(check_packet,check_patern,sizeof(check_packet)) == 0)
		return TRUE;
	
	return FALSE;
}
#endif

void CGPSDlg::OnFileSaveNmea()
{
	if(m_saveNmeaDlg)
	{
		::AfxMessageBox("Save function has been activated!");
		return;
	}

	CTime t = CTime::GetCurrentTime();
	CString fileName;
	fileName.Format("NMEA%02d-%02d-%02d_%02d%02d%02d.txt", t.GetYear(), t.GetMonth(), t.GetDay(),
		t.GetHour(), t.GetMinute(), t.GetSecond());

	CFileDialog dlgFile(FALSE, _T("txt"), fileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
		_T("ALL Files (*.*)|*.*||"), this);
	
	dlgFile.GetOFN().lpstrFile = fileName.GetBuffer(MyMaxPath);
	dlgFile.GetOFN().nMaxFile = MyMaxPath;
	INT_PTR nResult = dlgFile.DoModal();
	fileName.ReleaseBuffer();

	if(nResult != IDOK)
	{
		return;
	}

	m_saveNmeaDlg = new CSaveNmea(this);
	m_saveNmeaDlg->Create(IDD_SAVENMEA);
	UWM_SAVENMEA_EVENT = m_saveNmeaDlg->RegisterEventMessage();
	m_saveNmeaDlg->SetNotifyWindow(this->GetSafeHwnd());
	m_saveNmeaDlg->StartSave(fileName, fileName);
	m_saveNmeaDlg->ShowWindow(SW_SHOW);
	m_saveNmeaDlg->SetFocus();
	m_saveNmeaDlg->SetBinaryMode(false);
}

void CGPSDlg::OnFileSaveBinary()
{
	if(m_saveNmeaDlg)
	{
		::AfxMessageBox("Save function has been activated!");
		return;
	}

	CTime t = CTime::GetCurrentTime();
	CString fileName;
	fileName.Format("Binary%02d-%02d-%02d_%02d%02d%02d.out", t.GetYear(), t.GetMonth(), t.GetDay(),
		t.GetHour(), t.GetMinute(), t.GetSecond());

	CFileDialog dlgFile(FALSE, _T("txt"), fileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, 
		_T("ALL Files (*.*)|*.*||"), this);
	
	dlgFile.GetOFN().lpstrFile = fileName.GetBuffer(MyMaxPath);
	dlgFile.GetOFN().nMaxFile = MyMaxPath;
	INT_PTR nResult = dlgFile.DoModal();
	fileName.ReleaseBuffer();

	if(nResult != IDOK)
	{
		return;
	}

	m_saveNmeaDlg = new CSaveNmea(this);
	m_saveNmeaDlg->Create(IDD_SAVENMEA);
	UWM_SAVENMEA_EVENT = m_saveNmeaDlg->RegisterEventMessage();
	m_saveNmeaDlg->SetNotifyWindow(this->GetSafeHwnd());
	m_saveNmeaDlg->StartSave(fileName, fileName);
	m_saveNmeaDlg->ShowWindow(SW_SHOW);
	m_saveNmeaDlg->SetFocus();
	m_saveNmeaDlg->SetBinaryMode(true);
}

void CGPSDlg::OnVerifyFirmware()
{
	if(!CheckConnect())
	{
		return;
	}

	m_inputMode = 0;

	CVerifyFwDlg dlg;
	INT_PTR nResult = dlg.DoModal();	
	if(nResult!=IDOK)
	{
		SetMode();  
		CreateGPSThread();
	}
}

void CGPSDlg::OnFilePlayNmea()
{
	OnBnClickedPlay();
}

void CGPSDlg::OnConverterKml()
{
	CKmlDlg dlg;
	dlg.DoModal();
}

void CGPSDlg::OnRawMeasurementOutputConvert()
{
	CRawMeasmentOutputConvertDlg dlg;
	dlg.DoModal();
}

UINT ScanGPSThread(LPVOID pParam)
{
	CGPSDlg::gpsDlg->ScanGPS();
	return 0;	
}

UINT ScanGPS1Thread(LPVOID pParam)
{
	CGPSDlg::gpsDlg->ScanGPS1();
	return 0;	
}

UINT ScanGPS2Thread(LPVOID pParam)
{
	CGPSDlg::gpsDlg->ScanGPS2();
	return 0;	
}

void CGPSDlg::ScanGPS()
{
	WaitForSingleObject(hScanGPS,INFINITE);
	DeleteNmeaMemery();		

	for(int comPort=0; comPort<30; ++comPort)
	{				
		for(int i=0; i<CSerial::BaudrateTableSize; ++i)
		{		
			if(m_pScanDlg->IsFinish)
			{
				return;
			}
			CString strMsg;
			strMsg.Format("Scanning Com%d Baudrate %d", comPort+1, CSerial::BaudrateTable[i]);
			//sprintf_s(binmsg,"Scanning Com%d Baudrate %d",ComPort+1,baudrate[i]);
			m_pScanDlg->m_msg.SetWindowText(strMsg);

			m_serial = new CSerial;
			m_BaudRateCombo.SetCurSel(i);	
			m_ComPortCombo.SetCurSel(comPort);	
			if(!m_serial->Open(comPort+1, i))
			{		
				delete m_serial;
				m_serial = NULL;
				break;				
			}
			else
			{
				Sleep(100);
				if(SendMsg())
				{
					m_pScanDlg->IsFinish = true;
					return;
				}
			}		
			delete m_serial;
			m_serial = NULL;		
		}	
	} 
	m_pScanDlg->IsFinish = true;
	AfxMessageBox("Sorry! Can't find supported GNSS device.");
}

bool CGPSDlg::SendMsg()
{
	m_inputMode = 0;	
	memset(m_inputMsg, 0, 9);   		    
	m_inputMsg[0]=(U08)0xa0;
	m_inputMsg[1]=(U08)0xa1;
	m_inputMsg[2]=0;
	m_inputMsg[3]=2;
	m_inputMsg[4]=9; //msgid
	m_inputMsg[5]=1;
	m_inputMsg[6]=8; //checksum right	    
	m_inputMsg[7]=(U08)0x0d;
	m_inputMsg[8]=(U08)0x0a;
	SetMsgType(NMEA_Mode);
	ClearQue();
#ifdef JICEN
	if(CheckGPS(m_inputMsg,9,"Find Jicen Hitech GPS Device..."))
#else
	if(CheckGPS(m_inputMsg,9,"Find supported GNSS device..."))
#endif
	{					
		if(m_isConnectOn)
		{
			OnBnClickedClose();
		}

		SwitchToConnectedStatus(TRUE);

		m_comPort  = m_ComPortCombo.GetCurSel();
		m_baudrate = m_BaudRateCombo.GetCurSel();

		CreateGPSThread();
		m_inputMode = NMEA_Mode;	
		//m_connectT.SetWindowText("Connect");
		SetConnectTitle(true);


		CRegistry reg;
		reg.SetRootKey(HKEY_CURRENT_USER);
		if(reg.SetKey("Software\\GNSSViewer\\GPS",true))
		{
			reg.WriteInt("comport",GetComPort());
			reg.WriteInt("baudrate",GetBaudrate());
		}

		m_isPressCloseButton      = false;	  
		m_gpgsvMsg.NumOfSate    = 0;
		m_gpgsvMsg.NumOfMessage = 0;	
		m_glgsvMsg.NumOfSate    = 0;
		m_glgsvMsg.NumOfMessage = 0;	
		m_nmeaList.DeleteAllItems();   
		g_scatterData.ClearData();
		m_ttffCount        = 0;	
		SetTTFF(0);
//		if(m_isPressNmeaCommend && !m_isNmeaFileOpen)
//		{		
//			m_nmeaFile.Open(m_nmeaFilePath, CFile::modeReadWrite|CFile::modeCreate);	
//			m_isNmeaFileOpen = true;	
//		}	
		return true;
	}
	return false;
}

void CGPSDlg::ScanGPS1()
{
	WaitForSingleObject(hScanGPS,INFINITE);
	DeleteNmeaMemery();		
	U08 b = m_BaudRateCombo.GetCurSel();
//	U32 baudrate[9]={4800,9600,19200,38400,57600,115200};	
	//char buff[100];	
	for(int ComPort = 0;ComPort<30;ComPort++)
	{					
		if(m_pScanDlg->IsFinish)
			return;
		CString strMsg;
		strMsg.Format("Scanning Com%d Baudrate %d", ComPort+1, CSerial::BaudrateTable[b]);
		//sprintf_s(binmsg,"Scanning Com%d Baudrate %d",ComPort+1,baudrate[b]);
		m_pScanDlg->m_msg.SetWindowText(strMsg);

		m_serial = new CSerial;
		m_BaudRateCombo.SetCurSel(b);	m_ComPortCombo.SetCurSel(ComPort);	
		if(!m_serial->Open(ComPort+1,b))
		{		
			delete m_serial;
			m_serial = NULL;				
		}
		else
		{		
			if(SendMsg())
			{	
				Sleep(100);
				m_pScanDlg->IsFinish = true;
				return;
			}
		}			
		delete m_serial;
		m_serial = NULL;			
	} 
	m_pScanDlg->IsFinish = true;
	AfxMessageBox("Sorry! Can't find supported GNSS device.");

}

void CGPSDlg::ScanGPS2()
{
	WaitForSingleObject(hScanGPS,INFINITE);
	DeleteNmeaMemery();		
	//U08 Baudrate[6]={0,1,2,6,7,8};	
//	U32 b[9]={4800,9600,19200,38400,57600,115200};	
	int ComPort = m_ComPortCombo.GetCurSel();	
	//char buff[100];						
	for(int i=0;i<CSerial::BaudrateTableSize;i++)
	{	
		if(m_pScanDlg->IsFinish)
		{
			return;
		}
		CString strMsg;
		strMsg.Format("Scanning Com%d Baudrate %d", ComPort + 1, CSerial::BaudrateTable[i]);
		m_pScanDlg->m_msg.SetWindowText(strMsg);

		m_serial = new CSerial;
		m_BaudRateCombo.SetCurSel(i);	m_ComPortCombo.SetCurSel(ComPort);	
		if(!m_serial->Open(ComPort+1,i))
		{		
			delete m_serial;
			m_serial = NULL;
			break;				
		}
		else
		{	
			if(SendMsg())
			{
				Sleep(100);
				m_pScanDlg->IsFinish = true;
				return;
			}
		}
		delete m_serial;
		m_serial = NULL;		
	} 
	m_pScanDlg->IsFinish = true;
	AfxMessageBox("Sorry! Can't find supported GNSS device.");

}

UINT ShowScanThread(LPVOID pParam)
{	
	CGPSDlg::gpsDlg->m_pScanDlg->DoModal();	
	if(!ResetEvent(hScanGPS))  
	{
		DWORD error = GetLastError();	
	}
	AfxEndThread(0);		
	return 0;	
}

void CGPSDlg::OnBnClickedScanall()
{
	if(m_isConnectOn) OnBnClickedClose();
	AfxBeginThread(ShowScanThread,0);
	AfxBeginThread(ScanGPSThread,0);
}

void CGPSDlg::OnBnClickedScanport()
{
	if(m_isConnectOn) OnBnClickedClose();	
	AfxBeginThread(ShowScanThread,0);
	AfxBeginThread(ScanGPS1Thread,0);
}

void CGPSDlg::OnBnClickedScanbaudrate()
{
	if(m_isConnectOn) OnBnClickedClose();
	AfxBeginThread(ShowScanThread,0);
	AfxBeginThread(ScanGPS2Thread,0);
}

U08 CGPSDlg::wait_res(char* res)
{
	time_t start,end;
	start = clock();

	while(1)
	{
		char buff[1024] ;
		memset(buff,0,1024);
		end=clock();
		if(TIMEOUT_METHOD(start, end))
			return false;

		int len = m_serial->GetString(buff, sizeof(buff), (DWORD)(TIME_OUT_MS - (end-start)));		
		if(len)
		{
			if(!strcmp(buff,res)) 
				return true;
			else if(!strcmp(buff,"Error1"))
				return false;
			else if(!strcmp(buff,"Error4"))
				return false;
			else if(!strcmp(buff,"Error2"))
				return false;
			else if(!strcmp(buff,"Error3"))
				return false;
			else if(!strcmp(buff,"Error5"))
				return false;
			else if(!strcmp(buff,"Error6"))
				return false;
		}

	}
	return false;
}

void CGPSDlg::OnBinaryConfiguremessagetype()
{
	if(!CheckConnect())
	{
		return;
	}

	m_inputMode = 0;	
	CCfgMsg* dlg = new CCfgMsg;
	dlg->msg_id = 0x9;
	INT_PTR nResult = dlg->DoModal();	
	if(nResult!=IDOK)
	{
		SetMode();  
		CreateGPSThread();
	}
	delete dlg;	
	dlg = NULL;	
}

void CGPSDlg::SetBaudrate(int b)
{
	CloseOpenUart();
	m_serial->ResetPort(b);
	m_BaudRateCombo.SetCurSel(b);
	m_baudrate = b;
}

void CGPSDlg::WriteIni()
{
	CRegistry reg;
	reg.SetRootKey(HKEY_CURRENT_USER);
	if(reg.SetKey("Software\\GNSSViewer\\GPS",true))
	{
		reg.WriteInt("comport",GetComPort());
		reg.WriteInt("baudrate",GetBaudrate());
	}
}

bool CGPSDlg::CfgPortSendToTarget(U08* message,U16 length,char* Msg)
{	
	m_serial->SendData(message, length, true);
	Sleep(500);
	add_msgtolist(Msg);
	return true;	
}

void CGPSDlg::OnEphemerisGetephemeris()
{
	if(!CheckConnect())
	{
		return;
	}
	m_inputMode = 0;	
	CGetEphemerisDlg dlg;
	if(dlg.DoModal() != IDOK)
	{
		SetMode();  
		CreateGPSThread();
	}
}

UINT SetEphemerisThread(LPVOID pParam)
{	
	CGPSDlg::gpsDlg->SetEphms(FALSE);	
	return 0;
}

void CGPSDlg::SetEphms(U08 continues)	
{	
	if(g_setting.boostEphemeris)
	{
		CGPSDlg::gpsDlg->m_nDownloadBaudIdx = g_setting.boostBaudIndex;
		BoostBaudrate(FALSE);
	}
	ULONGLONG dwBytesRemaining = m_ephmsFile.GetLength();
	if(dwBytesRemaining == 86 || dwBytesRemaining	== 86*32)
	{
		while(dwBytesRemaining)
		{
			U08 messages[94] = {0};  
			//memset(messages, 0, 94);
			messages[0] = (U08)0xa0;
			messages[1] = (U08)0xa1;
			messages[2] = 0;
			messages[3] = 87;
			//messages[4]  = 0x31; //msgid    
			messages[4] = 0x41; //msgid   

			BYTE buffer[86];
			UINT nBytesRead = m_ephmsFile.Read(buffer, 86);		
			if(IsEphmsEmpty(buffer))
			{
				dwBytesRemaining-=nBytesRead;	
				continue;
			}
			memcpy(&messages[5], buffer, nBytesRead);
			U08 checksum = 0;
			for(int i=0; i<(int)messages[3]; i++)
			{
				checksum ^= messages[i+4];	
			}
			messages[91] = checksum;
			messages[92] = (U08)0x0d;
			messages[93] = (U08)0x0a;	
			dwBytesRemaining -= nBytesRead;
			//memcpy(&SVID,&messages[5],2);
			//SVID = messages[5] << 8 &0xff00 | messages[6]&0xff;
			U16 SVID = messages[6] & 0xff;			
			sprintf_s(m_nmeaBuffer, "Set SV#%d Ephemeris Successful...", SVID);
			if(!SendToTargetNoWait(messages, 94, m_nmeaBuffer))
			{
				sprintf_s(m_nmeaBuffer, "Set SV#%d Ephemeris Fail...", SVID);
				add_msgtolist(m_nmeaBuffer);
			}
			//TRACE("%d %d\n",SVID,dwBytesRemaining);
		}	
	}
	else
	{
		AfxMessageBox("The Ephemeris data Format of the file is wrong");
		//return;
	}	
	m_ephmsFile.Close();	

	if(!continues)
	{
		if(g_setting.boostEphemeris)
		{
		BoostBaudrate(TRUE, ChangeToTemp, true);
		}
		SetMode(); 
		CreateGPSThread();	
	}

}

bool CGPSDlg::IsEphmsEmpty(BYTE* buffer)
{
	U08 ZeroCounter=0;
	for(int i=2;i<86;i++)
	{
		if(*(buffer+i) == 0)
		{
			ZeroCounter++;
			if(ZeroCounter > 60)
				return true;
		}
	}
	return false;
}

bool CGPSDlg::IsGlonassEphmsEmpty(BYTE* buffer)
{
	U08 ZeroCounter=0;
	for(int i=2;i<42;i++)
	{
		if(*(buffer+i) == 0)
		{
			ZeroCounter++;
			if(ZeroCounter > 30)
				return true;
		}
	}
	return false;
}

void CGPSDlg::OnEphemerisSetephemeris()
{
	if(!CheckConnect())
	{
		return;
	}

	m_inputMode  = 0;
	CString fileName = m_lastGpEphFile;
	if(m_lastGpEphFile.IsEmpty())
	{	
		fileName = "GPS_ephemeris.log";	
	}

	CFileDialog dlgFile(TRUE, _T("log"), fileName, 
		OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY, 
		_T("Log Files (*.log)|*.log||"), this);

	if(dlgFile.DoModal() != IDOK)
	{
		SetMode();
		CreateGPSThread();
		return;
	}

	fileName = dlgFile.GetPathName();	
	CFileException ef;
	try
	{
		if(!m_ephmsFile.Open(fileName,CFile::modeRead,&ef))
		{
			ef.ReportError();
			SetMode();
			CreateGPSThread();
			return;
		}
		AfxBeginThread(SetEphemerisThread, 0);			
	}
	catch(CFileException *fe)
	{
		fe->ReportError();
		fe->Delete();
		return;
	}	
}

void CGPSDlg::GetEphms(U08 SV, U08 continues)
{
	const int EphmsRecordSize = 0x5d;
	if(g_setting.boostEphemeris)
	{
		CGPSDlg::gpsDlg->m_nDownloadBaudIdx = g_setting.boostBaudIndex;
		BoostBaudrate(FALSE);
	}

	BinaryCommand cmd(2);
	cmd.SetU08(1, 0x30);
	cmd.SetU08(2, SV);

	if(SendToTarget(cmd.GetBuffer(), cmd.Size(), ""))	
	{	
		if(WRL == NULL )
		{
			WRL = new CWaitReadLog;
		}
		AfxBeginThread(WaitLogRead, 0);
		WaitForSingleObject(waitlog, INFINITE);
		WRL->SetWindowText("Wait for get ephemeris");
		WRL->msg.SetWindowText("Please wait for get ephemeris!");

		U08 NumsOfEphemeris = 0;
		int wait = 0;
		while(1)
		{	
			wait++;
			if(wait == 50)
			{ 
				Sleep(500);
				WRL->msg.SetWindowText("Retrieve GPS Ephemeris data is Failed!");
				Sleep(500);
				WRL->IsFinish = true;
				add_msgtolist("Retrieve GPS Ephemeris Failed...");	
				goto TheLast;
			}

			U08 buff[1024] = {0};
			DWORD nSize = m_serial->GetBinaryBlock(buff, sizeof(buff), EphmsRecordSize);
			if(SAVE_EPHEMRIS(buff, BINMSG_GET_EPHEMERIS))
			{
				if(SV!=0)
				{
					break;
				}
				else
				{
					NumsOfEphemeris++;
				}

				CString txtMsg;
				txtMsg.Format("Retrieve Satellite ID # %d Ephemeris", NumsOfEphemeris);
				WRL->msg.SetWindowText(txtMsg);

				if(NumsOfEphemeris==32)
				{
					break;
				}
			}
		}	
		WRL->msg.SetWindowText("Retrieve GPS Ephemeris data is completed!");
		Sleep(500);
		WRL->IsFinish = true;
		add_msgtolist("Retrieve GPS Ephemeris Successful...");	
		
	} //if(SendToTarget(binMsg, sizeof(binMsg), ""))
TheLast:
	m_lastGpEphFile = m_ephmsFile.GetFilePath();
	m_ephmsFile.Close();	

	if(!continues)
	{
		if(g_setting.boostEphemeris)
		{
			BoostBaudrate(TRUE, ChangeToTemp, true);
		}
		//SetPort(GetBaudrate(),2);	
		SetMode(); 
		CreateGPSThread();
	}

}

void CGPSDlg::GetGlonassEphms(U08 SV,U08 continues)
{
	const int EphmsRecordSize = 0x31;
	if(g_setting.boostEphemeris)
	{
		CGPSDlg::gpsDlg->m_nDownloadBaudIdx = g_setting.boostBaudIndex;
		BoostBaudrate(FALSE);
	}

	U08 binMsg[9] = {0};  

	binMsg[0]  = (U08)0xa0;
	binMsg[1]  = (U08)0xa1;
	binMsg[2]  = 0;
	binMsg[3]  = 2;
	binMsg[4]  = 0x5B; //msgid
	binMsg[5]  = SV;	
	U08 checksum = 0;
	for(int i=0; i<(int)binMsg[3]; i++)
	{
		checksum^=binMsg[i+4];	
	}
	binMsg[6] = checksum;
	binMsg[7] = (U08)0x0d;
	binMsg[8] = (U08)0x0a;
	int wait = 0 ;
	char BINMSG[1024] = {0};
	//	for(int i=0;i<10;i++)	_cprintf("%x ",messages[i]);		
	if(SendToTarget(binMsg, sizeof(binMsg), ""))	
	{	
		if(WRL == NULL )
		{
			WRL = new CWaitReadLog;
		}
		AfxBeginThread(WaitLogRead, 0);
		WaitForSingleObject(waitlog, INFINITE);
		WRL->SetWindowText("Wait for get Glonass ephemeris");
		WRL->msg.SetWindowText("Please wait for get Glonass ephemeris!");

		U08 NumsOfEphemeris = 0;
		while(1)
		{	
			wait++;
			if(wait == 50)
			{ 
				Sleep(500);
				WRL->msg.SetWindowText("Retrieve Glonass Ephemeris data is Failed!");
				Sleep(500);
				WRL->IsFinish = true;
				add_msgtolist("Retrieve Glonass Ephemeris Failed...");	
				
				goto TheLast;
			}
			U08 buff[1024] = {0};
			m_serial->GetBinaryBlock(buff, sizeof(buff), EphmsRecordSize);
			if(SAVE_EPHEMRIS(buff, 0x90))
			{
				if(SV!=0)
				{
					break;
				}
				else
				{
					NumsOfEphemeris++;
				}
				sprintf_s(BINMSG,"Retrieve Glonass Satellite ID # %d Ephemeris", NumsOfEphemeris);
				WRL->msg.SetWindowText(BINMSG);

				if(NumsOfEphemeris==24)
				{
					break;
				}
			}
		}	
		WRL->msg.SetWindowText("Retrieve Glonass Ephemeris data is completed!");
		Sleep(500);
		WRL->IsFinish = true;
		add_msgtolist("Retrieve Glonass Ephemeris Successful...");	
	}
TheLast:
	m_lastGlEphFile = m_ephmsFile.GetFilePath();
	m_ephmsFile.Close();

	if(g_setting.boostEphemeris)
	{
		BoostBaudrate(TRUE, ChangeToTemp, true);
	}
	SetMode(); 
	CreateGPSThread();
}

void CGPSDlg::GetBeidouEphms(U08 SV, U08 continues)
{
	if(g_setting.boostEphemeris)
	{
		CGPSDlg::gpsDlg->m_nDownloadBaudIdx = g_setting.boostBaudIndex;
		BoostBaudrate(FALSE);
	}

	BinaryCommand cmd(3);
	cmd.SetU08(1, 0x67);
	cmd.SetU08(2, 0x02);
	cmd.SetU08(3, SV);

	if(SendToTarget(cmd.GetBuffer(), cmd.Size(), ""))	
	{	
		if(WRL == NULL )
		{
			WRL = new CWaitReadLog;
		}
		AfxBeginThread(WaitLogRead, 0);
		WaitForSingleObject(waitlog, INFINITE);
		WRL->SetWindowText("Wait for get Beidou ephemeris");
		WRL->msg.SetWindowText("Please wait for get Beidou ephemeris!");

		U08 NumsOfEphemeris = 0;
		int wait = 0;
		while(1)
		{	
			wait++;
			if(wait == 50)
			{ 
				Sleep(500);
				WRL->msg.SetWindowText("Retrieve Beidou Ephemeris data is Failed!");
				Sleep(500);
				WRL->IsFinish = true;
				add_msgtolist("Retrieve Beidou Ephemeris Failed...");	
				goto TheLast;
			}

			U08 buff[1024] = {0};
			DWORD nSize = m_serial->GetBinary(buff, sizeof(buff));
			if(SaveEphemeris2(buff, MAKEWORD(0x67, 0x80)))
			{
				if(SV != 0)
				{
					break;
				}
				else
				{
					NumsOfEphemeris++;
				}
				
				CString txtMsg;
				txtMsg.Format("Retrieve Beidou Satellite ID # %d Ephemeris",NumsOfEphemeris);
				WRL->msg.SetWindowText(txtMsg);

				if(NumsOfEphemeris==37)
				{
					break;
				}
			}
		}	

		WRL->msg.SetWindowText("Retrieve Beidou Ephemeris data is completed!");
		Sleep(500);
		WRL->IsFinish = true;
		add_msgtolist("Retrieve Beidou Ephemeris Successful...");	
		
	}
TheLast:
	m_lastBdEphFile = m_ephmsFile.GetFilePath();
	m_ephmsFile.Close();	

	if(g_setting.boostEphemeris)
	{
		BoostBaudrate(TRUE, ChangeToTemp, true);
	}
	SetMode(); 
	CreateGPSThread();	
}

void CGPSDlg::Load_Menu()
{
	HMENU hMenu = CreateMenu(); 				// 建立主選單

	//File Menu
	static MenuItemEntry menuItemFile[] =
	{
		{ !NMEA_INPUT, MF_STRING, ID_FILE_SAVENMEA, "&Save NMEA", NULL },
		{ !NMEA_INPUT, MF_STRING, ID_FILE_BINARY, "&Save Binary", NULL },
		{ 1, MF_STRING, ID_FILE_CLEANNEMA, "&Clear Message Screen", NULL },
		{ NMEA_INPUT, MF_STRING, ID_FILE_PLAYNMEA, "&Play NMEA", NULL },
		{ IS_DEBUG, MF_STRING, ID_VERIFY_FIRMWARE, "&Verify Firmware", NULL },
		{ 1, MF_SEPARATOR, 0, NULL, NULL },
		{ IS_DEBUG, MF_STRING, ID_FILE_SETUP, "&Setup", NULL },
		{ 1, MF_STRING, ID_FILE_EXIT, "&Exit", NULL },
		{ 0, 0, 0, NULL, NULL }	//End of table
	};
	CreateSubMenu(hMenu, menuItemFile, "&File");

	//Binary Menu
	static MenuItemEntry QuerySwVerMenu[] =
	{
		{ 1, MF_STRING, ID_QUERYSOFTWAREVERSION_ROMCODE, "Rom Code", NULL },
		{ 1, MF_STRING, ID_QUERYSOFTWAREVERSION_SYSTEMCODE, "System Code", NULL },
		{ 0, 0, 0, NULL, NULL }	//End of table
	};
	static MenuItemEntry QueryCrcChkMenu[] =
	{
		{ 1, MF_STRING, ID_QUERYSOFTWARECRC_ROMCODE, "Rom Code", NULL },
		{ 1, MF_STRING, ID_QUERYSOFTWARECRC_SYSTEMCODE, "System Code", NULL },
		{ 0, 0, 0, NULL, NULL }	//End of table
	};

	static MenuItemEntry SetFactoryDefaultMenu[] =
	{
		{ 1, MF_STRING, ID_SETFACTORYDEFAULT_NOREBOOT, "No Reboot", NULL },
		{ 1, MF_STRING, ID_SETFACTORYDEFAULT_REBOOT, "Reboot after setting to factory defaults", NULL },
		{ 0, 0, 0, NULL, NULL }	//End of table
	};

	static MenuItemEntry menuItemBinary[] =
	{
		{ 1, MF_STRING, ID_BINARY_SYSTEMRESTART, "System Restart", NULL },
		{ _SHOW_BINARY_DATA_, MF_STRING, ID_BINARY_DUMPDATA, "Show Binary Data", NULL },
		{ 1, MF_POPUP, 0, "Set Factory Default", SetFactoryDefaultMenu },
		{ IS_DEBUG, MF_STRING, ID_FIRMWARE_DOWNLOAD, "Firmware Image Download", NULL },
		{ 1, MF_SEPARATOR, 0,NULL,NULL },
		//{ IS_DEBUG, MF_POPUP, 0, "Query Software Version", QuerySwVerMenu },
		//{ IS_DEBUG, MF_POPUP, 0, "Query CRC Checksum", QueryCrcChkMenu },
		{ 1, MF_STRING, ID_QUERYSOFTWAREVERSION_SYSTEMCODE, "Query Software Version", NULL },
		{ 1, MF_STRING, ID_QUERYSOFTWARECRC_SYSTEMCODE, "Query CRC Checksum", NULL },
		{ 1, MF_STRING, ID_QUERY_SHA1, "Query SHA1 String", NULL },
		{ IS_DEBUG, MF_STRING, ID_QUERY_CON_CAP, "Query GNSS Constellation Capability", NULL },
//		{ BINARY_MESSAGE_SUPPORT, MF_STRING, ID_BINARY_QUERYBINARYMSGINTERVAL, "Query Binary Message Measurement", NULL },
		{ _V8_SUPPORT, MF_STRING, ID_QUERY_NMEA_INTERVAL_V8, "Query NMEA Message Interval", NULL },
		{ (CUSTOMER_ID==Ericsson), MF_STRING, ID_QUERY_ERICSSON_STC_ITV, "Query Ericsson Sentence Interval", NULL },
		{ (CUSTOMER_ID==OlinkStar), MF_STRING, ID_QUERY_SERIAL_NUMBER, "Query Serial Number", NULL },

		{ 1, MF_STRING, ID_BINARY_QUERYPOSITIONRATE, "Query Position Update Rate", NULL },
		{ 1, MF_STRING, ID_BINARY_QUERYDATUM, "Query Datum", NULL },
		{ 1, MF_STRING, ID_BINARY_QUERYPOSITIONPINNING, "Query Position Pinning", NULL },
		{ 1, MF_STRING, ID_BINARY_QUERY1PPS, "Query GPS Measurement Mode", NULL },
		{ 1, MF_STRING, ID_BINARY_QUERYPOWERMODE, "Query Power Mode", NULL },//
		{ IS_DEBUG && !_V8_SUPPORT, MF_STRING, ID_BINARY_QUERYPOWERSAVINGPARAMETERS, "Query Power Saving Parameters", NULL },//
		{ IS_DEBUG && _V8_SUPPORT, MF_STRING, ID_QUERY_V8_POWER_SV_PARAM, "Query Power Saving Parameters", NULL },//
		{ IS_DEBUG, MF_STRING, ID_BINARY_QUERYPROPRIETARYMESSAGE, "Query Proprietary Message", NULL },//
		{ 1, MF_STRING, ID_QUERY1PPSTIMING_QUERY, "Query DOP Mask", NULL },//
		{ IS_DEBUG || SHOW_ELEV_AND_CNR_MASK_IN_GEN, MF_STRING, ID_QUERY1PPSTIMING_QUERYELEVATIONANDCNRMASK, "Query Elevation and CNR Mask", NULL },//
		{ IS_DEBUG, MF_STRING, ID_BINARY_QUERYANTENNADETECTION, "Query Antenna Detection", NULL },//
		{ IS_DEBUG, MF_STRING, ID_BINARY_QUERYNOISEPOWER, "Query Noise Power", NULL },//
		{ SOFTWARE_FUNCTION & SW_FUN_DR, MF_STRING, ID_BINARY_QUERYDRINFO, "Query DR Info", NULL },//
		{ SOFTWARE_FUNCTION & SW_FUN_DR, MF_STRING, ID_BINARY_QUERYDRHWPARAMETER, "Query DR HW Parameter", NULL },//
//		{ INTERFERENCE, MF_STRING, ID_BINARY_QUERYJAMMINGINTERFERENCE, "Query Jamming Interference", NULL },
//		{ GNSS_VIEWER, MF_STRING, ID_QUERY_GNSS_SEL_FOR_NAV, "Query GNSS Navigation Selection", NULL },
		{ GNSS_VIEWER, MF_STRING, ID_BINARY_QUERYGNSSKNUMBERSLOTCNR, "Query GLONASS K-Number, Slot, CNR", NULL },
		{ 1, MF_STRING, ID_BINARY_QUERYNMEATALKERID, "Query NMEA Talker ID", NULL },
//		{ 1, MF_STRING, ID_BINARY_QUERYNMEATALKERID, "Query Binary Measurement Data Out", NULL },
//		{ 1, MF_STRING, ID_QUERY_BIN_MEA_DAT_OUT, "Query Binary Measurement Data Out", NULL },

		{ IS_DEBUG, MF_STRING, ID_BINARY_GETRGISTER, "Get Register", NULL },
//		{ AVID, MF_STRING, ID_BINARY_QUERYDISTANCEANGLE, "Query Distance && Angle", NULL },
		{ 1, MF_SEPARATOR, 0,NULL,NULL },
		{ 1, MF_STRING, ID_CONFIGURE_SERIAL_PORT, "Configure Serial Port", NULL },
		{ !_V8_SUPPORT, MF_STRING, ID_BINARY_CONFIGURENMEAOUTPUT, "Configure NMEA Output", NULL },
		{ _V8_SUPPORT, MF_STRING, ID_CONFIG_NMEA_INTERVAL_V8, "Configure NMEA Message Interval", NULL },
		{ (CUSTOMER_ID==Ericsson), MF_STRING, ID_CONFIG_ERICSSON_STC_ITV, "Configure Ericsson Sentence Interval", NULL },
		{ (CUSTOMER_ID==OlinkStar), MF_STRING, ID_CONFIG_SERIAL_NUMBER, "Set Serial Number", NULL },

		{ 1, MF_STRING, ID_BINARY_CONFIGUREMESSAGETYPE, "Configure Message Type", NULL },
		{ BINARY_MESSAGE_INTERVAL, MF_STRING, ID_BINARY_CONFIGUREBINARYINTERVAL, "Configure Binary Message Interval", NULL },
//		{ BINARY_MESSAGE_SUPPORT, MF_STRING, ID_BINARY_CONFIGUREBINARYMSGINTERVAL, "Configure Binary Message Measurement", NULL },
		{ IS_DEBUG, MF_STRING, ID_BINARY_CONFIGUREMULTIPATH, "Configure Multi-path", NULL },	//
		{ 1, MF_STRING, ID_BINARY_CONFIGUREPOSITIONRATE, "Configure Position Update Rate", NULL },
		{ 1, MF_STRING, ID_BINARY_CONFIGUREDATUM, "Configure Datum", NULL },
		{ 1, MF_STRING, ID_BINARY_CONFIGUREPOSITIONPINNING, "Configure Position Pinning", NULL },
		{ 1, MF_STRING, ID_BINARY_CONFIGUREPINNINGPARAMETERS, "Configure Pinning Parameters", NULL },
		{ 1, MF_STRING, ID_BINARY_CONFIGURE1PPS, "Configure GPS Measurement Mode", NULL },
		{ 1, MF_STRING, ID_BINARY_CONFIGUREPOWERMODE, "Configure Power Mode", NULL },
		{ IS_DEBUG, MF_STRING, ID_BINARY_CONFIGUREPOWERSAVINGPARAMETERS, "Configure Power Saving Parameters", NULL },
		{ IS_DEBUG, MF_STRING, ID_CONFIG_PROPRIETARY_MESSAGE, "Configure Proprietary Message", NULL },
		{ 1, MF_STRING, ID_CONFIGURE1PPSTIMING_CONFIGURE1PPS, "Configure DOP Mask", NULL },
		{ IS_DEBUG || SHOW_ELEV_AND_CNR_MASK_IN_GEN, MF_STRING, ID_CONFIG_ELEV_AND_CNR_MASK, "Configure Elevation and CNR Mask", NULL },
		{ IS_DEBUG, MF_STRING, ID_CONFIGURE_ANTENNA_DETECTION, "Configure Antenna Detection", NULL },
		{ IS_DEBUG, MF_STRING, ID_BINARY_CONFIGURESUBSECREGISTER, "Configure SubSec Register", NULL },
		{ GNSS_VIEWER, MF_STRING, ID_BINARY_CONFIGURENMEAOUTPUT32953, "Configure NMEA Output Comport", NULL },
		{ GNSS_VIEWER, MF_STRING, ID_BINARY_CONFIGURENMEATALKERID, "Configure NMEA Talker ID", NULL },
		//2014/03/11, Oliver remove this command in raw measurment version.
		//2014/05/12, Added by customer request.
		{ 1, MF_STRING, ID_CONFIG_BIN_MEA_DAT_OUT, "Configure Binary Measurement Data Out", NULL },

//		{ GNSS_VIEWER, MF_STRING, ID_CONFIG_GNSS_SEL_FOR_NAV, "Configure GNSS Navigation Selection", NULL },
//		{ GG12A, MF_STRING, ID_CFG_GL_ACQUISITION_MODE, "Configure Glonass Acquisition Mode", NULL },
		{ IS_DEBUG, MF_STRING, ID_BINARY_CONFIGUREREGISTER, "Configure Register", NULL },
//		{ SJA, MF_STRING, ID_BINARY_POSITIONFINDER, "Configure Position Finder", NULL },
		{ ODOMETER_SUPPORT, MF_STRING, ID_BINARY_RESETODOMETER, "Reset Odometer", NULL },
//		{ INTERFERENCE, MF_STRING, ID_BINARY_CONFIGUREJAMMINGINTERFERENCE, "Configure Jamming Interference", NULL },
		{ PACIFIC, MF_STRING, ID_BINARY_CONFIGUREMESSAGE_TYPE, "Request Output Message", NULL },
		{ RESET_MOTION_SENSOR, MF_STRING, ID_BINARY_RESETMOTIONSENSOR, "Reset Motion Sensor", NULL },

		{ 0, 0, 0, NULL, NULL }	//End of table
	};
	if(!NMEA_INPUT)
	{
		CreateSubMenu(hMenu, menuItemBinary, "&Binary");
	}

	static MenuItemEntry GpsdoControlMenu[] =
	{
		{ 1, MF_STRING, ID_QUERY_UARTPASS, "Query UART Pass Through Status", NULL },
		{ 1, MF_STRING, ID_GPSDO_RESET_SLAVE, "Reset Slave MCU(Master Only)", NULL },
		{ 1, MF_SEPARATOR, 0, NULL, NULL }	,
		{ 1, MF_STRING, ID_GPSDO_ENTER_ROM, "Enter Slave ROM Download(Master Only)", NULL },
		{ 1, MF_STRING, ID_GPSDO_LEAVE_ROM, "Back To Normal Mode from ROM Download", NULL },
		{ 1, MF_SEPARATOR, 0, NULL, NULL }	,
		{ 1, MF_STRING, ID_GPSDO_ENTER_DWN, "Enter Slave Download(Master Only)", NULL },
		{ 1, MF_STRING, ID_GPSDO_LEAVE_DWN, "Back To Normal Mode from Slave Download", NULL },
		{ 1, MF_SEPARATOR, 0, NULL, NULL }	,
		{ 1, MF_STRING, ID_GPSDO_ENTER_UART, "Enter Slave UART Pass Through(Master Only)", NULL },
		{ 1, MF_STRING, ID_GPSDO_LEAVE_UART, "Back To Normal Mode from UART Pass through", NULL },
		{ 1, MF_SEPARATOR, 0, NULL, NULL }	,
		{ 0, 0, 0, NULL, NULL }	//End of table
	};
	static MenuItemEntry Sup800Menu[] =
	{
		{ 1, MF_STRING, ID_SUP800_ERASE_DATA, "SUP800 Erase User Data", NULL },
		{ 1, MF_STRING, ID_SUP800_WRITE_DATA, "SUP800 Write User Data", NULL },
		{ 1, MF_STRING, ID_SUP800_READ_DATA, "SUP800 Read User Data", NULL },
		{ 0, 0, 0, NULL, NULL }	//End of table
	};	
	static MenuItemEntry SignalDisturbanceMenu[] =
	{
		{ 1, MF_STRING, ID_QUERY_SIG_DISTUR_DATA, "Query Signal Disturbance Data", NULL },
		{ 1, MF_STRING, ID_QUERY_SIG_DISTUR_STATUS, "Query Signal Disturbance Status", NULL },
		{ 1, MF_STRING, ID_CONFIG_SIG_DISTUR_STATUS, "Configure Signal Disturbance Status", NULL },
		{ 0, 0, 0, NULL, NULL }	//End of table
	};

	//Venus 8 Menu
	static MenuItemEntry menuItemVenus8[] =
	{
		{ 1, MF_STRING, ID_QUERY_BOOT_STATUS, "GNSS ROM Boot Status", NULL },
		{ IS_DEBUG, MF_STRING, ID_QUERY_CUSTOMER_ID, "Query Customer ID", NULL },
		{ IS_DEBUG, MF_STRING, ID_CONFIG_DOZE_MODE, "Configure GNSS Doze Mode", NULL },
		{ IS_DEBUG, MF_POPUP, 0, "GPSDO Control", GpsdoControlMenu },
		{ _V8_SUPPORT, MF_POPUP, 0, "SUP800 User Data Storage", Sup800Menu },
		{ IS_DEBUG, MF_POPUP, 0, "Signal Disturbance Test", SignalDisturbanceMenu },

		{ 1, MF_SEPARATOR, 0, NULL, NULL }	,
		{ 1, MF_STRING, ID_BINARY_QUERYSBAS, "Query SBAS", NULL },
		{ 1, MF_STRING, ID_BINARY_QUERYSAGPS, "Query SAEE", NULL },
		{ 1, MF_STRING, ID_BINARY_QUERYQZSS, "Query QZSS", NULL },
		{ 1, MF_STRING, ID_BINARY_QUERY_DGPS, "Query DGPS", NULL },
		{ IS_DEBUG, MF_STRING, ID_BINARY_QUERY_SMOOTH_MODE, "Query Carrier Smooth Mode", NULL },
		{ IS_DEBUG, MF_STRING, ID_BINARY_QUERY_TIME_STAMPING, "Query Time Stamping", NULL },
		{ IS_DEBUG, MF_STRING, ID_BINARY_QUERY_NOISE_PW_CTL, "Query Noise Power Control", NULL },
		{ IS_DEBUG, MF_STRING, ID_BINARY_QUERY_ITF_DET_CTL, "Query Interference Detect Control", NULL },
		{ IS_DEBUG, MF_STRING, ID_BINARY_QUERY_NMBI_OUT_DES, "Query NMEA/Binary Output Destination", NULL },
		{ 1, MF_STRING, ID_BINARY_QUERY_PARAM_SEARCH_ENG_NUM, "Query Parameter Search Engine Number", NULL },
		{ IS_DEBUG, MF_STRING, ID_QUERY_POS_FIX_NAV_MASK, "Query Position Fix Navgation Mask", NULL },
		{ IS_DEBUG, MF_STRING, ID_QUERY_REF_TIME_TO_GPS, "Query Ref Time Sync To GPS Time", NULL },
		{ 1, MF_STRING, ID_QUERY_NAV_MODE_V8, "Query Navigation Mode", NULL },
		{ 1, MF_STRING, ID_QUERY_GNSS_NAV_SOL, "Query GNSS Constellation Type", NULL },
		{ 1, MF_STRING, ID_QUERY_GPS_TIME, "Query GPS Time", NULL },
		{ IS_DEBUG, MF_STRING, ID_QUERY_V8_POWER_SV_PARAM_ROM, "Query Power Saving Parameters(Rom)", NULL },//
		{ 1, MF_STRING, ID_QUERY_PARAM_SRCH_ENG_SLP_CRT, "Query Parameter Search Engine Sleep Criteria", NULL },
		{ 1, MF_STRING, ID_QUERY_DATUM_INDEX, "Query Datum Index", NULL },

		{ 1, MF_SEPARATOR, 0, NULL, NULL }	,
		{ 1, MF_STRING, ID_BINARY_CONFIGURESBAS, "Configure SBAS", NULL },
		{ 1, MF_STRING, ID_BINARY_CONFIGURESAGPS, "Configure SAEE", NULL },
		{ 1, MF_STRING, ID_BINARY_CONFIGUREQZSS, "Configure QZSS", NULL },
		{ 1, MF_STRING, ID_BINARY_CONFIG_DGPS, "Configure DGPS", NULL },
		{ IS_DEBUG, MF_STRING, ID_BINARY_CONFIG_SMOOTH_MODE, "Configure Carrier Smooth Mode", NULL },
		{ IS_DEBUG, MF_STRING, ID_BINARY_CONFIG_TIME_STAMPING, "Configure Time Stamping", NULL },
		{ IS_DEBUG, MF_STRING, ID_BINARY_CONFIGURE_NOISE_PW_CTL, "Configure Noise Power Control", NULL },
		{ IS_DEBUG, MF_STRING, ID_BINARY_CONFIGURE_ITF_DET_CTL, "Configure Interference Detect Control", NULL },
		{ IS_DEBUG, MF_STRING, ID_BINARY_CONFIGURE_NMBI_OUT_DES, "Configure NMEA/Binary Output Destination", NULL },
		{ 1, MF_STRING, ID_CONFIGURE_PARAM_SEARCH_ENG_NUM, "Configure Parameter Search Engine Number", NULL },
		{ IS_DEBUG, MF_STRING, ID_CONFIGURE_POS_FIX_NAV_MASK, "Configure Position Fix Navgation Mask", NULL },
		{ IS_DEBUG, MF_STRING, ID_CONFIG_REF_TIME_TO_GPS, "Configure Ref Time Sync To GPS Time", NULL },
		{ 1, MF_STRING, ID_MULTIMODE_CONFIGUREMODE, "Configure Navigation Mode", NULL },
		{ 1, MF_STRING, ID_CONFIG_GNSS_NAV_SOL, "Configure GNSS Constellation Type", NULL },
		{ 1, MF_STRING, ID_CONFIG_LEAP_SECONDS, "Configure GPS/UTC Leap Seconds", NULL },
		{ IS_DEBUG, MF_STRING, ID_CONFIG_V8_POWER_SV_PARAM_ROM, "Configure Power Saving Parameters(Rom)", NULL },
		{ 1, MF_STRING, ID_CONFIG_PARAM_SRCH_ENG_SLP_CRT, "Configure Parameter Search Engine Sleep Criteria", NULL },
		{ 1, MF_STRING, ID_CONFIG_DATUM_INDEX, "Configure Datum Index", NULL },

		{ IS_DEBUG, MF_SEPARATOR, 0, NULL, NULL }	,
		{ IS_DEBUG, MF_STRING, ID_CLOCK_OFFSET_PREDICT, "Clock Offset Predict(New)", NULL },
		//{ IS_DEBUG, MF_STRING, ID_CLOCK_OFFSET_PREDICT_OLD, "Clock Offset Predict(Old)", NULL },
		{ IS_DEBUG, MF_STRING, ID_HOSTBASED_DOWNLOAD, "Host-Based Image Download", NULL },
		{ IS_DEBUG, MF_STRING, ID_PARALLEL_DOWNLOAD, "Parallel Image Download", NULL },
		{ 0, 0, 0, NULL, NULL }	//End of table
	};
	if(_V8_SUPPORT && !NMEA_INPUT)
	{
		CreateSubMenu(hMenu, menuItemVenus8, "&Venus 8");
	}

	//DRMenu
	static MenuItemEntry menuItemDR[] =
	{
		{ 1, MF_STRING, ID_QUERY_DR_MULTIHZ, "Query DR Multi-Hz", NULL },
		{ 1, MF_SEPARATOR, 0, NULL, NULL }	,
		{ 1, MF_STRING, ID_CONFIG_DR_MULTIHZ, "Configure DR Multi-Hz", NULL },
		{ 0, 0, 0, NULL, NULL }	//End of table
	};
	if ((SOFTWARE_FUNCTION & SW_FUN_DR) && !NMEA_INPUT)
	{
		CreateSubMenu(hMenu, menuItemDR, "&DR");
	}

	// 1PPS Timing Menu
	static MenuItemEntry menuItem1PPSTiming[] =
	{
		{ 1, MF_STRING, ID_QUERY1PPSTIMING_QUERYTIMING, "Query Timing", NULL },
//		{ 1, MF_STRING, ID_1PPSTIMING_QUERY1PPSNMEADELAY, "Query 1PPS NMEA Delay", NULL },
		{ 1, MF_STRING, ID_QUERY1PPSTIMING_QUERYCABLEDELAY, "Query Cable Delay", NULL },
		{ TIMING_MONITORING, MF_STRING, ID_1PPSTIMING_MONITORING1PPS, "Monitoring 1PPS", NULL },
		{ 1, MF_STRING, ID_1PPSTIMING_QUERY1PPSPULSEWIDTH, "Query 1PPS Pulse Width", NULL },
		{ 1, MF_STRING, ID_1PPSTIMING_QUERYPPSOUTPUTMODE, "Query PPS Output Mode", NULL },
//		{ 1, MF_STRING, ID_1PPSTIMING_QUERYPPSCLKSOURCE, "Query PPS Pulse Clock Source", NULL },
		{ 1, MF_STRING, ID_QUERY_1PPS_FREQ_OUTPUT, "Query 1PPS Frequency Output", NULL },
		{ 1, MF_SEPARATOR, 0, NULL, NULL },
		{ 1, MF_STRING, ID_CONFIGURE1PPSTIMING_CONFIGURE1PPSTIMING, "Configure Timing", NULL },
		//{ 1, MF_STRING, ID_1PPSTIMING_CONFIGURE1PPSNMEADELAY, "Configure 1PPS NMEA Delay", NULL },
		{ 1, MF_STRING, ID_CONFIGURE1PPSTIMING_CONFIGURE1PPSCABLEDELAY, "Configure Cable Delay", NULL },
		{ 1, MF_STRING, ID_1PPSTIMING_CONFIGUREPROPRIETARYNMEA, "Configure Proprietary NMEA", NULL },
		{ 1, MF_STRING, ID_1PPSTIMING_CONFIGURE1PPSPULSEWIDTH, "Configure 1PPS Pulse Width", NULL },
		{ 1, MF_STRING, ID_1PPSTIMING_CONFIGUREPPSOUTPUTMODE, "Configure 1PPS Output Mode", NULL },
//		{ 1, MF_STRING, ID_1PPSTIMING_CONFIGUREPPSCLKSOURCE, "Configure 1PPS Pulse Clock Source", NULL },
		{ 1, MF_STRING, ID_1PPSTIMING_ENTERREFERENCEPOSITION32977, "On Line Assistance", NULL },
//		{ 1, MF_STRING, ID_1PPSTIMING_ENTERREFERENCEPOSITION32961, "Enter Reference Position", NULL },
		{ 1, MF_STRING, ID_CONFIG_1PPS_FREQ_OUTPUT, "Configure 1PPS Frequency Output", NULL },
		{ 0, 0, 0, NULL, NULL },
	};
	if(TIMING_MODE && !NMEA_INPUT)
	{
		CreateSubMenu(hMenu, menuItem1PPSTiming, "&1PPS Timing");
	}

	//Ephemeris Menu
	static MenuItemEntry menuItemEphemeris[] =
	{
		{ 1, MF_STRING, ID_EPHEMERIS_GETEPHEMERIS, "Get GPS Ephemeris", NULL },
		{ 1, MF_STRING, ID_EPHEMERIS_SETEPHEMERIS, "Set GPS Ephemeris", NULL },
		{ GNSS_VIEWER, MF_STRING, ID_GET_GLONASS_EPHEMERIS, "Get GLONASS Ephemeris", NULL },
		{ GNSS_VIEWER, MF_STRING, ID_SET_GLONASS_EPHEMERIS, "Set GLONASS Ephemeris", NULL },
		{ GNSS_VIEWER, MF_STRING, ID_GET_BEIDOU_EPHEMERIS, "Get Beidou Ephemeris", NULL },
		{ GNSS_VIEWER, MF_STRING, ID_SET_BEIDOU_EPHEMERIS, "Set Beidou Ephemeris", NULL },
		{ 1, MF_SEPARATOR, 0, NULL, NULL },
		{ NEW_ALMANAC, MF_STRING, ID_EPHEMERIS_GETALMANAC, "Get GPS Almanac", NULL },
		{ NEW_ALMANAC, MF_STRING, ID_EPHEMERIS_SETALMANAC, "Set GPS Almanac", NULL },
		{ GNSS_VIEWER, MF_STRING, ID_EPHEMERIS_GETGLONASSALMANAC, "Get GLONASS Almanac", NULL },
		{ GNSS_VIEWER, MF_STRING, ID_EPHEMERIS_SETGLONASSALMANAC, "Set GLONASS Almanac", NULL },
		{ GNSS_VIEWER, MF_SEPARATOR, 0, NULL, NULL },
		{ GNSS_VIEWER, MF_STRING, ID_EPHEMERIS_GETTIMECORRECTIONS, "Get GLONASS Time Corrections", NULL },
		{ GNSS_VIEWER, MF_STRING, ID_EPHEMERIS_SETTIMECORRECTIONS, "Set GLONASS Time Corrections", NULL },

		{ 0, 0, 0, NULL, NULL },
	};
	if(!NMEA_INPUT)
	{
		CreateSubMenu(hMenu, menuItemEphemeris, "&Ephemeris");
	}

	//Navigation Mode Menu
	static MenuItemEntry menuItemNavMode[] =
	{
		{ 1, MF_STRING, ID_MULTIMODE_QUERYMODE, "Query Navigation Mode", NULL },
		{ 1, MF_STRING, ID_MULTIMODE_CONFIGUREMODE, "Configure Navigation Mode", NULL },
		{ 0, 0, 0, NULL, NULL },
	};
	if(!_V8_SUPPORT && !NMEA_INPUT)
	{
		CreateSubMenu(hMenu, menuItemNavMode, "&Navigation Mode");
	}

	//AGPS Menu
	static MenuItemEntry menuItemAGPS[] =
	{
		{ IS_DEBUG, MF_STRING, ID_AGPS_STATUS, "AGPS Status", NULL },
		{ IS_DEBUG, MF_STRING, ID_AGPS_CONFIG, "AGPS Configure", NULL },
		{ IS_DEBUG, MF_STRING, ID_AGPS_FTP_SREC, "AGPS Download", NULL },
		{ IS_DEBUG, MF_SEPARATOR, 0, NULL, NULL },
		{ IS_DEBUG, MF_STRING, ID_ROMAGPS_FTP_SREC, "Rom AGPS Download(Old Method)", NULL },
		{ 1, MF_STRING, ID_ROMAGPS_FTP_NEW, "Rom AGPS Download", NULL },
		{ 0, 0, 0, NULL, NULL },
	};
	if((SOFTWARE_FUNCTION & SW_FUN_AGPS) && !NMEA_INPUT)
	{
		CreateSubMenu(hMenu, menuItemAGPS, "&AGPS");
	}

	//DataLog Menu
	static MenuItemEntry menuItemDataLog[] =
	{
		{ 1, MF_STRING, ID_DATALOG_LOGSTATUSCONTROL, "Log Status", NULL },
		{ 1, MF_STRING, ID_DATALOG_LOGCONFIGURECONTROL, "Log Configure", NULL },
		{ 1, MF_STRING, ID_DATALOG_LOGCLEARCONTROL, "Log Clear", NULL },
		//{ 1, MF_STRING, ID_LOGGER_CONVERT, "Log Decompress", NULL },
		{ 1, MF_STRING, ID_CONVERTER_DECOMPRESS, "Log Decompress", NULL },
		{ 1, MF_STRING, ID_DATALOG_LOGREADBATCH, "Log Read", NULL },
//		{ SUPPORT_CLEAR_LOGIN_PASSWORD, MF_STRING, ID_DATALOG_CLEARLOGINPASSWORD, "Clear Login Password", NULL },
		{ 0, 0, 0, NULL, NULL },
	};

	if((SOFTWARE_FUNCTION & SW_FUN_DATALOG) && !NMEA_INPUT)
	{
		CreateSubMenu(hMenu, menuItemDataLog, "&DataLog");
	}

	//Converter Menu
	static MenuItemEntry menuItemConvert[] =
	{
		{ IS_DEBUG & SOFTWARE_FUNCTION & SW_FUN_DATALOG, MF_STRING, ID_CONVERTER_COMPRESS, "Compress", NULL },
		{ IS_DEBUG & SOFTWARE_FUNCTION & SW_FUN_DATALOG, MF_STRING, ID_CONVERTER_DECOMPRESS, "Decompress", NULL },
		{ 1, MF_STRING, ID_CONVERTER_KML, "KML", NULL },
		{ 1, MF_STRING, ID_RAW_MEAS_OUT_CONVERT, "Raw Measurement Binary Convert", NULL },
		{ 0, 0, 0, NULL, NULL },
	};
	CreateSubMenu(hMenu, menuItemConvert, "&Converter");

	//WAAS Menu
	static MenuItemEntry menuItemWASS[] =
	{
		{ 1, MF_STRING, ID_WAAS_WAASSTATUS, "&WAAS Status", NULL },
		{ 1, MF_STRING, ID_WAAS_WAAS, "&WAAS Configure", NULL },
		{ 0, 0, 0, NULL, NULL },
	};
	if(!_V8_SUPPORT && !NMEA_INPUT)
	{
		CreateSubMenu(hMenu, menuItemWASS, "&WAAS");
	}

	//miniHomer Menu
	static MenuItemEntry menuItemminiHomer[] =
	{
		{ 1, MF_STRING, ID_MINIHOMER_ACTIVATE, "&Activate", NULL },
		{ 1, MF_STRING, ID_MINIHOMER_SETTAGECCO, "&Set Tag = 0x88 ( ECCO )", NULL },
		{ 1, MF_SEPARATOR, 0, NULL, NULL },
		{ 1, MF_STRING, ID_MINIHOMER_QUERYTAG, "&Query Tag", NULL },
		{ 0, 0, 0, NULL, NULL },
	};
	if(ACTIVATE_MINIHOMER)
	{
		CreateSubMenu(hMenu, menuItemminiHomer, "&miniHomer");
	}

	//Utility Menu
	static MenuItemEntry menuItemUtility[] =
	{
		{ 1, MF_STRING, ID_NMEA_CHECKSUM_CAL, "NMEA checksum calculator", NULL },
		{ 1, MF_STRING, ID_BIN_CHECKSUM_CAL, "Binary checksum calculator", NULL },
		{ IS_DEBUG, MF_STRING, ID_TEST_EXTERNAL_SREC, "Test External SREC", NULL },
//		{ 1, MF_STRING, ID_DATALOG_LOGCLEARCONTROL, "Log Clear", NULL },
//		{ 1, MF_STRING, ID_LOGGER_CONVERT, "Log Decompress", NULL },
//		{ 1, MF_STRING, ID_DATALOG_LOGREADBATCH, "Log Read Batch", NULL },
//		{ SUPPORT_CLEAR_LOGIN_PASSWORD, MF_STRING, ID_DATALOG_CLEARLOGINPASSWORD, "Clear Login Password", NULL },
		{ 0, 0, 0, NULL, NULL },
	};

	if(IS_DEBUG)
	{
		CreateSubMenu(hMenu, menuItemUtility, "&Utility");
	}

	//Help Menu
	static MenuItemEntry menuItemHelp[] =
	{
		{ 1, MF_STRING, ID_HELP_ABOUT, "&About", NULL },
		{ 0, 0, 0, NULL, NULL },
	};
	CreateSubMenu(hMenu, menuItemHelp, "&Help");

	::SetMenu(this->m_hWnd, hMenu);
}

UINT OnAgpsConfig_Thread(LPVOID param)
{
	SYSTEMTIME	now; 
	GetSystemTime(&now);
	U16 cyear = now.wYear;    

	CGPSDlg::gpsDlg->SetInputMode(0);	

	U08 msg[8] ;
	msg[0] = 0x34 ;
	msg[1] = cyear >> 8 &0xff;
	msg[2] = cyear &0xff ;
	msg[3] = (U08)now.wMonth ;
	msg[4] = (U08)now.wDay ;
	msg[5] = (U08)now.wHour ;
	msg[6] = (U08)now.wMinute ;
	msg[7] = (U08)now.wSecond ;

	int len = CGPSDlg::gpsDlg->SetMessage(msg, sizeof(msg));

	CGPSDlg::gpsDlg->//WaitEvent();
	CGPSDlg::gpsDlg->ClearQue();
	if(CGPSDlg::gpsDlg->SendToTarget(CGPSDlg::m_inputMsg, len, ""))
	{
		CAgps_config *dlg = new CAgps_config(CGPSDlg::gpsDlg);
		if(dlg->DoModal() == IDOK)
		{
			U08 msg[3] = {0x33,dlg->enable,dlg->attribute} ;
			int len = CGPSDlg::gpsDlg->SetMessage(msg,sizeof(msg));
			CGPSDlg::gpsDlg->//WaitEvent();
			CGPSDlg::gpsDlg->ClearQue();
			CGPSDlg::gpsDlg->SendToTarget(CGPSDlg::m_inputMsg, len, "Set AGPS Status Successful...");
		}

	}	
	CGPSDlg::gpsDlg->SetMode();  
	CGPSDlg::gpsDlg->CreateGPSThread();
	return 0;
}

//AGPS Configure
void CGPSDlg::OnAgpsConfig()
{
	if(!CheckConnect())
	{
		return;
	}
	::AfxBeginThread(OnAgpsConfig_Thread, 0);
}

//增加訊息到response list
void CGPSDlg::add_msgtolist(LPCTSTR msg)
{
	if(g_setting.responseLog)
	{
		CFile f;
		f.Open(g_setting.responseLogPath, 
			CFile::modeWrite | CFile::modeNoTruncate | CFile::modeCreate);
		f.SeekToEnd();
		CString ts = CTime::GetCurrentTime().Format("%Y-%m-%d %H:%M:%S");
		f.Write(ts, strlen(ts));
		f.Write(" - ", 3);
		f.Write(msg, strlen(msg));
		f.Write("\r\n", 2);
		f.Close();
	}
	m_responseList.AddString(msg);	
	m_responseList.SetCurSel(m_responseList.GetCount()-1);
}
/*
//確認封包的正確性
int CGPSDlg::check_msg_valid(U08 *buff)
{
	U08 checksum=0;
	int len = *(buff+3);
	U08 ID = *(buff+4);
	for(int i=0;i<len;i++)
	{
		checksum^=buff[i+4];
	}	
	if(checksum == buff[len+4])
		return ID ;
	else
		return 0;
}
*/
bool CGPSDlg::send_command_withackString(U08 *rs_buff,int size,char *res)
{
	ClearQue();
	m_serial->SendData(rs_buff, size, true);	

	return (0 != wait_res(res));
}

void CGPSDlg::OnAgpsFtpSrec()
{
	if(!CheckConnect())
	{
		return;
	}
	m_inputMode = 0;
	if(Ack!=QueryAgpsStatus(Return, NULL))
	{
		SetMode();
		CreateGPSThread();
		return;
	}

	SetCurrentDirectory(m_currentDir);

	CFTPDlg ftpdlg;
	ftpdlg.SetMode(1);
	ftpdlg.DoModal();

	SetMode();
	CreateGPSThread();
}

void CGPSDlg::OnRomAgpsFtpSrec()
{
	if(!CheckConnect())
	{
		return;
	}
	m_inputMode = 0;
	SetCurrentDirectory(m_currentDir);

	CFTPDlg ftpdlg;
	ftpdlg.SetMode(2);
	ftpdlg.DoModal();

	SetMode();
	CreateGPSThread();
}

void CGPSDlg::OnRomAgpsFtpNew()
{
	if(!CheckConnect())
	{
		return;
	}
	m_inputMode = 0;
	SetCurrentDirectory(m_currentDir);

	CFTPDlg ftpdlg;
	ftpdlg.SetMode(3);
	ftpdlg.DoModal();

	SetMode();
	CreateGPSThread();
}

void CGPSDlg::OnHostBasedDownload()
{
	if(!CheckConnect())
	{
		return;
	}
	m_inputMode = 0;
//	SetCurrentDirectory(m_currentDir);

	CHostBaseDownloadDlg dlg;
	if(dlg.DoModal()!=IDOK)
	{
		SetMode();
		CreateGPSThread();
		return;
	}
	m_DownloadMode = HostBasedDownload;
//	m_bHostBasedMode = TRUE;
//	m_isHostOnlySendCmd = FALSE;
//	m_isHostOnlySendBin = FALSE;
//	m_isUsingV8InternalLoader = FALSE;

	m_nDownloadBaudIdx = dlg.GetBaudrateIndex();
	m_strDownloadImage = dlg.GetFilePath();
	m_nDownloadBufferIdx = dlg.GetBufferIndex();

	::AfxBeginThread(DownloadThread, 0);
}

void CGPSDlg::OnFiremareDownload()
{
	if(!CheckConnect())
	{
		return;
	}
	m_inputMode = 0;
//	SetCurrentDirectory(m_currentDir);

	CFirmwareDownloadDlg dlg;
	if(dlg.DoModal()!=IDOK)
	{
		SetMode();
		CreateGPSThread();
		return;
	}
//	m_bHostBasedMode = FALSE;
//	m_isHostOnlySendCmd = FALSE;
//	m_isHostOnlySendBin = FALSE;
//	m_isUsingV8InternalLoader = dlg.IsInternal();
//	if(dlg.IsInternal())
//	{
//		m_DownloadMode = InternalLoader;
//	}
//	else
//	{
//		m_DownloadMode = EnternalLoader;
//	}
	switch(dlg.GetLoaderType())
	{
	case CFirmwareDownloadDlg::UsingExternalLoader:
		m_DownloadMode = EnternalLoader;
		break;
	case CFirmwareDownloadDlg::V6GpsSeriesLoader:
		m_DownloadMode = InternalLoaderV6Gps;
		break;
	case CFirmwareDownloadDlg::V6GnssSeriesLoader:
		m_DownloadMode = InternalLoaderV6Gnss;
		break;
	case CFirmwareDownloadDlg::V6Gg12aLoader:
		m_DownloadMode = InternalLoaderV6Gg12a;
		break;
	case CFirmwareDownloadDlg::V8SerialLoader:
		m_DownloadMode = InternalLoaderV8;
		break;
	case CFirmwareDownloadDlg::UsingExternalLoaderInBinCmd:
		m_DownloadMode = EnternalLoaderInBinCmd;
		break;
	case CFirmwareDownloadDlg::OLinkStarDownload:
		m_DownloadMode = CustomerDownload;
		m_customerId = OlinkStar;
		break;
	default:
		ASSERT(FALSE);
		break;
	}

	m_nDownloadBaudIdx = dlg.GetBaudrateIndex();
	m_strDownloadImage = dlg.GetFilePath();
	m_nDownloadBufferIdx = dlg.GetBufferIndex();
	::AfxBeginThread(DownloadThread, 0);
}

void CGPSDlg::OnParallelDownload()
{
	if(!CheckConnect())
	{
		return;
	}
	m_inputMode = 0;
	CParallelDownloadDlg dlg;
	if(dlg.DoModal()!=IDOK)
	{
		SetMode();
		CreateGPSThread();
		return;
	}
	//m_nDownloadBaudIdx = setting->boostBaudIndex;
	m_nDownloadBaudIdx = dlg.GetBaudrateIndex();	//It only has 115200 loader.
	m_strDownloadImage = dlg.GetFilePath();
	m_nDownloadBufferIdx = 0;
	switch(dlg.GetFlashType())
	{
	case 0:	//AMIC A29L160A
		m_DownloadMode = ParallelDownloadType0;
		break;
	case 1:	//;NUMONYX JS28F256P30T
		m_DownloadMode = ParallelDownloadType1;
		break;
	default:
		ASSERT(FALSE);
	}
	::AfxBeginThread(DownloadThread, 0);
}

bool CheckOlinkstarFirmware(LPCSTR pszPath)
{
	BinaryData bin(pszPath);

	if(bin.Size() <= 0)
	{
		return false;
	}

	const char symbol[] = "$OLinkStar";
	BYTE* p = (BYTE*)memchr(bin.GetBuffer(), '$', bin.Size());
	while(p)
	{
		int n = memcmp(p, symbol, strlen(symbol));
		if(n==0)
		{
			return true;
		}
		p = (BYTE*)memchr(p + 1, '$', bin.Size() - (p - bin.GetBuffer()) - 1);
	};
	return false;
}

void CGPSDlg::OnBnClickedDownload()
{
	m_lbl_firmware_path.GetWindowText(m_strDownloadImage);
	if(!Utility::IsFileExist(m_strDownloadImage))
	{
		::AfxMessageBox("PROM file not found!");
		return;
	}

	if(CUSTOMER_DOWNLOAD && CUSTOMER_ID==OlinkStar)	//Olinkstar must check prom.bin
	{
		if(!CheckOlinkstarFirmware(m_strDownloadImage))
		{
			::AfxMessageBox("PROM file not support!");
			return;
		}
	}

	if(!CheckConnect())
	{
		return;
	}

#if GG12A
	if(check_gg12a_format(m_strDownloadImage) == FALSE) 
	{
		return;
	}
#endif
	//Detect Loader Type
	BOOL usingInternal = ((CButton*)GetDlgItem(IDC_IN_LOADER))->GetCheck();
	BOOL isRomMode = ((CButton*)GetDlgItem(IDC_ROM_MODE))->GetCheck();
	BOOL isCheat = ((GetAsyncKeyState(VK_LSHIFT) & 0x8000) && (GetAsyncKeyState(VK_LMENU)& 0x8000));

	//m_DownloadMode = (usingInternal) ? InternalLoaderV8 : EnternalLoader;
	m_DownloadMode = (usingInternal) ? InternalLoaderV8 : EnternalLoaderInBinCmd;
	if(CUSTOMER_DOWNLOAD)
	{
		m_DownloadMode = CustomerDownload;
	}

	CString externalSrecFile;
	if(theApp.CheckExternalSrec(externalSrecFile))
	{	//Not ParallelDownload and external loader is exist. Prompts the user to use external loader.
		if(_ALWAYS_USE_EXTERNAL_SREC_ || IDYES == ::AfxMessageBox("Do you want to use extenal loader?", MB_YESNO))
		{
			m_DownloadMode = EnternalLoader;
			//m_DownloadMode = EnternalLoaderInBinCmd;
		}
	}
	else if(!theApp.CheckExternalSrec(externalSrecFile) && _ALWAYS_USE_EXTERNAL_SREC_)
	{
		::AfxMessageBox("No external loader exist!");
		return;
	}
	else if(_USE_RESOURCE_LOADER_)
	{
		m_DownloadMode = InternalLoaderSpecial;
	}

	int dlBaudIdx = ((CComboBox*)GetDlgItem(IDC_DL_BAUDRATE))->GetCurSel();
	dlBaudIdx += 5;	//Download Baudrate start in 115200
	theApp.SetIntSetting("dl_baudIdx", dlBaudIdx);

	m_nDownloadBaudIdx = dlBaudIdx;
	m_nDownloadBufferIdx = 0;
	m_inputMode = 0;

	::AfxBeginThread(DownloadThread, 0);
}

void CGPSDlg::OnFileCleannema()
{
	m_nmeaList.DeleteAllItems();
}

void CGPSDlg::OnBnClickedBrowse()
{
	CFileDialog fd(true,"*.bin",NULL,OFN_FILEMUSTEXIST| OFN_HIDEREADONLY,"*.bin|*.bin||");
	if(fd.DoModal() == IDOK)
	{
		CString strPath = fd.GetPathName();
		m_lbl_firmware_path.SetWindowText(strPath);
		m_lbl_firmware_path.Invalidate();

		CRegistry reg;
		reg.SetRootKey(HKEY_CURRENT_USER);
		if(reg.SetKey("Software\\GNSSViewer\\GPS", false))
		{
			reg.WriteString("firmware", strPath);
		}
	}
}

void CGPSDlg::OnBinaryConfiguremessageType()
{
	if(!CheckConnect())
	{
		return;
	}

	m_inputMode = 0;	
	CCfgMsg dlg;
	dlg.msg_id = 0xe;
	if(dlg.DoModal() != IDOK)
	{
		SetMode();  
		CreateGPSThread();
	}
}

int m_mt,m_rm,m_sc,m_rs,m_sub,m_gsb,m_binary_interval_attr,m_rate;
UINT set_binarymsg_interval_andrew(LPVOID param)
{

	U08 msg[3] ;
	msg[0] = 0x11 ;
	msg[1] = m_rate;
	msg[2] = m_binary_interval_attr;

	int len = CGPSDlg::gpsDlg->SetMessage(msg,sizeof(msg));
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(CGPSDlg::m_inputMsg, len, "Set binary ouput interval Successful...");
	return 0;	
}

void CGPSDlg::OnBinaryConfigureBinaryInterval()
{
	if(!CheckConnect())
	{
		return;
	}

	m_inputMode = 0;	
	CConfig_binary_interval dlg;

	if(dlg.DoModal() != IDOK)	
	{
		SetMode();  
		CreateGPSThread();
		return;
	}

	m_rate = dlg.m_bin_interval;
	m_binary_interval_attr = dlg.m_bin_attr;
	::AfxBeginThread(set_binarymsg_interval_andrew, 0);
}

UINT set_positon_finder(LPVOID param)
{

	U08 msg[17] ;
	msg[0] = 0x36 ;
	memcpy(&msg[1],&pos_lat,sizeof(double));
	memcpy(&msg[9],&pos_lon,sizeof(double));

	int len = CGPSDlg::gpsDlg->SetMessage(msg,sizeof(msg));
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(CGPSDlg::m_inputMsg, len, "Set Position Finder LLA Successful...");
	return 0;	
}

void CGPSDlg::OnBinaryPositionfinder()
{
	if(!CheckConnect())
	{
		return;
	}

	m_inputMode = 0;	
	CPositionFinderDlg dlg;

	dlg.lat = 0.0;
	dlg.lon = 0.0;
	if(dlg.DoModal() != IDOK)
	{
		SetMode();  
		CreateGPSThread();
		return;
	}

	pos_lat = dlg.lat;
	pos_lon = dlg.lon;
	::AfxBeginThread(set_positon_finder, 0);
}

bool CGPSDlg::download_eph()
{
	int time_out=30000;

	CInternetSession sess(0,0,INTERNET_OPEN_TYPE_DIRECT);
	CFtpConnection* pConnect;
	sess.SetOption(INTERNET_OPTION_CONNECT_TIMEOUT,time_out);
	sess.SetOption(INTERNET_OPTION_CONTROL_RECEIVE_TIMEOUT,time_out);    
	sess.SetOption(INTERNET_OPTION_CONTROL_SEND_TIMEOUT,time_out);    
	sess.SetOption(INTERNET_OPTION_DATA_RECEIVE_TIMEOUT,time_out*6);    
	sess.SetOption(INTERNET_OPTION_DATA_SEND_TIMEOUT,time_out);    
	sess.SetOption(INTERNET_OPTION_SEND_TIMEOUT,time_out);   

	try
	{     
		pConnect = sess.GetFtpConnection("60.250.205.31",
			"skytraq", "skytraq", 21, true);

		//OnAgps_Msg.fire(AGPS_CONNECT_FTP);

		char file_name[100];
		char remote_path[100];
//		char text[100];
		//strcpy_s(file_name,"Eph_4.dat");
		strcpy_s(file_name, sizeof(file_name),"Eph.dat");
		sprintf_s(remote_path,"ephemeris\\%s",file_name);


		if(!pConnect->GetFile(remote_path,file_name,false))
		{
			int err = GetLastError();
			if(err==12002)
			{
				AfxMessageBox("Download Eph.dat Timeout...");
				//OnAgps_Msg.fire(AGPS_DOWNLOAD_TIMEOUT);
			}
			else if(err==12003)
			{
				AfxMessageBox("Server Error...");
				//OnAgps_Msg.fire(AGPS_SERVER_ERROR);
			}
			else
			{
				AfxMessageBox("Eph.dat File Error...");
				//OnAgps_Msg.fire(AGPS_EPH_FILE_ERROR);
			}
			pConnect->Close();
			return false;
		}
		else
		{
			pConnect->Close();
			//sess.Close();
			//OnAgps_Msg.fire(AGPS_DOWNLOAD_EPH_COMPLETE);
			return true;
		}

	}
	catch (CInternetException* pEx)
	{
		TCHAR sz[1024];
		pEx->GetErrorMessage(sz, 1024);
		printf("ERROR!  %s\n", sz);
		pEx->Delete();
		AfxMessageBox(sz);
		return false;
	}	
}

U08 CGPSDlg::CheckEphAndDownload()
{
	U08 buff[86];
	S16 wn;
	D64 tow;

	FILE* f = NULL;
	fopen_s(&f, "Eph.dat", "rb");	
	if( f == NULL)
		return download_eph();

	get_wn_tow(&wn,&tow);

	fseek(f,0,SEEK_END);
	long file_size = ftell(f);
	fseek(f,0,SEEK_SET);

	if(f != NULL)
	{
		int size = fread(buff,1,86,f);

		if(size == 86)
		{
			U16 ephwn;
			S32 ephtoc;
			D64 tk;
			U08 sate_nu = buff[0];

			getBuffWnToc(buff,&ephwn,&ephtoc);
			tk = calculate_tk_double(ephwn, ephtoc, wn, tow ); 

			if(tk < -7200 )
			{
				fclose(f);
				return download_eph();
			}

			S32 abs_tk = (S32)abs(tk);

			S32 offset = abs_tk / 14400 ;

			if(offset > 42)
			{
				fclose(f);
				return download_eph();
			}
		}
		fclose(f);
	}
	return TRUE;
}

D64 CGPSDlg::calculate_tk_double( S16 ref_wn, S32 ref_tow, S16 wn, D64 tow )
{
	S16 dwn = (wn & 0x3FF) - ref_wn;
	if(dwn < -512)
	{
		dwn += 1024;
	}
	else if(dwn > 512)
	{
		dwn -= 1024;
	}

	D64 tk = tow - ref_tow;
	if(dwn != 0)
	{
		tk += dwn * 604800.0;
	}
	return tk;
}

void CGPSDlg::getBuffWnToc(U08* ephptr,U16 *wn,S32 *toc)
{
	U16 ephwn;
	S32 ephtoc;
	ephwn=*(ephptr+6)<<2;
	ephwn=(ephwn) | (*(ephptr+7) >>6);
	ephtoc=*(ephptr+22)<<8;
	ephtoc=(ephtoc) | (*(ephptr+23));
	ephtoc=ephtoc*16L;

	//week1024=ephwn+1024;
	*wn = ephwn+1024;
	*toc = ephtoc;
}

void CGPSDlg::get_wn_tow(S16* wn,D64* tow)
{
	//struct tm *now; 
//	char buff[128]; 

	SYSTEMTIME	now; 
	GetSystemTime(&now);	

	UtcTime utc;
	//strftime(buff,sizeof(buff),"%Y",now);
	utc.year = now.wYear;
	//strftime(buff,sizeof(buff),"%m",now);
	utc.month = now.wMonth;
	//strftime(buff,sizeof(buff),"%d",now);
	utc.day = now.wDay;
	utc.hour = now.wHour;
	utc.minute = now.wMinute;
	utc.sec = now.wSecond;

	printf("PC time %d/%d/%d %d:%d:%d\n",utc.year,utc.month,utc.day,utc.hour,utc.minute,(int)utc.sec);

	UtcConvertUtcToGpsTime(&utc, wn, tow);

	if( *tow >= 604800.0 )
	{
		*tow -= 604800.0;
		wn++;
	}
}



void CGPSDlg::ConfigBaudrate(int baud,int attribute)
{   
	U08 messages[11];    
	U08 i;	  
	memset(messages, 0, 11);
	messages[0]=(U08)0xa0;
	messages[1]=(U08)0xa1;
	messages[2]=0;
	messages[3]=4;
	messages[4]=5; //msgid		
	messages[5]=0; 
	messages[6]=baud; 
	messages[7]=attribute;
	U08 checksum = 0;
	for(i=0;i<(int)messages[3];i++)	checksum^=messages[i+4];	
	messages[8]=checksum;
	messages[9]=(U08)0x0d;
	messages[10]=(U08)0x0a;	
	//for(int i=0;i<10;i++)	_cprintf("%x ",messages[i]);
	SendToTarget(messages, 11,"Configure Serial Port Successful...");	
	SetBaudrate(baud);	
	WriteIni();
	return ;
}

int m_position_pinning;
int m_attributes;
UINT ConfigurePositionPinning(LPVOID param)
{
	U08 msg[3] ;
	msg[0] = 0x39 ;
	msg[1] = m_position_pinning;
	msg[2] = m_attributes;

	int len = CGPSDlg::gpsDlg->SetMessage(msg,sizeof(msg));
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(CGPSDlg::m_inputMsg, len, "Configure Position Pinning Successful...");
/*
	CGPSDlg::gpsDlg->//WaitEvent();
	CGPSDlg::gpsDlg->ClearQue();
	CGPSDlg::gpsDlg->SendToTarget(CGPSDlg::m_inputMsg,len,"Configure Position Pinning Successful...");
	CGPSDlg::gpsDlg->SetMode();  
	CGPSDlg::gpsDlg->CreateGPSThread();
*/
	return 0;	
}

void CGPSDlg::OnBinaryConfigurepositionpinning()
{
	if(!CheckConnect())
	{
		return;
	}

	CConPosPinning dlg;
	if(dlg.DoModal() != IDOK)
	{
		CGPSDlg::gpsDlg->SetMode();  
		CGPSDlg::gpsDlg->CreateGPSThread();
		return;
	}
	
	m_position_pinning = dlg.m_position_pinning;
	m_attributes = dlg.m_attributes;
	::AfxBeginThread(ConfigurePositionPinning, 0);
}

U16 m_pin_speed;
U16 m_pin_cnt;
U16 m_upin_speed;
U16 m_upin_th;
U16 m_upin_dis;
U08 m_pin_attr;

UINT ConfigurePinningParameter(LPVOID param)
{

	U08 msg[12] ;
	msg[0] = 0x3B ;

	msg[1] = m_pin_speed >> 8 & 0xFF;
	msg[2] = m_pin_speed & 0xFF;
	msg[3] = m_pin_cnt >> 8 & 0xFF;
	msg[4] = m_pin_cnt & 0xFF;
	msg[5] = m_upin_speed >> 8 & 0xFF;
	msg[6] = m_upin_speed & 0xFF;
	msg[7] = m_upin_th >> 8 & 0xFF;
	msg[8] = m_upin_th & 0xFF;
	msg[9] = m_upin_dis >> 8 & 0xFF;
	msg[10] = m_upin_dis & 0xFF;
	msg[11] = m_pin_attr;

	int len = CGPSDlg::gpsDlg->SetMessage(msg, sizeof(msg));
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(CGPSDlg::m_inputMsg, len, "Configure Pinning Parameters Successful...");
/*
	CGPSDlg::gpsDlg->//WaitEvent();
	CGPSDlg::gpsDlg->ClearQue();
	CGPSDlg::gpsDlg->SendToTarget(CGPSDlg::m_inputMsg,len,"Configure Pinning Parameters Successful...");
	CGPSDlg::gpsDlg->SetMode();  
	CGPSDlg::gpsDlg->CreateGPSThread();
*/
	return 0;	
}

void CGPSDlg::OnBinaryConfigurepinningparameters()
{
	if(!CheckConnect())
	{
		return;
	}

	CConPinningParameter dlg;

	if(dlg.DoModal() != IDOK)
	{
		CGPSDlg::gpsDlg->SetMode();  
		CGPSDlg::gpsDlg->CreateGPSThread();
		return;
	}
	
	m_pin_speed = dlg.pin_speed;
	m_pin_cnt = dlg.pin_cnt;
	m_upin_speed = dlg.upin_speed;
	m_upin_th = dlg.upin_threshold;
	m_upin_dis = dlg.upin_distance;
	m_pin_attr = dlg.attr;
	::AfxBeginThread(ConfigurePinningParameter, 0);
}

U16 m_multi_mode;
U16 m_multimode_attribute;

UINT ConfigureMultiMode(LPVOID param)
{
	U08 msg[4] = {0};
	int len = 0;
	if(_V8_SUPPORT)
	{
		msg[0] = 0x64;
		msg[1] = 0x17;
		msg[2] = m_multi_mode & 0xFF;
		msg[3] = m_multimode_attribute & 0xFF;
		len = CGPSDlg::gpsDlg->SetMessage(msg, 4);
	}
	else
	{
		msg[0] = 0x3C ;
		msg[1] = m_multi_mode & 0xFF;
		msg[2] = m_multimode_attribute & 0xFF;
		len = CGPSDlg::gpsDlg->SetMessage(msg, 3);
	}
//	int len = CGPSDlg::gpsDlg->SetMessage(msg, sizeof(msg));
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(CGPSDlg::m_inputMsg, len, "Configure Navigation Mode Successful...");
/*
	CGPSDlg::gpsDlg->ClearQue();
	CGPSDlg::gpsDlg->SendToTarget(CGPSDlg::m_inputMsg, len, "Configure Navigation Mode Successful...");
	CGPSDlg::gpsDlg->SetMode();  
	CGPSDlg::gpsDlg->CreateGPSThread();
*/
	return 0;	
}

void CGPSDlg::OnMultimodeConfiguremode()
{
	if(!CheckConnect())	
	{
		return;
	}

	CConMultiMode dlg;
	if(dlg.DoModal() != IDOK)
	{
		CGPSDlg::gpsDlg->SetMode();  
		CGPSDlg::gpsDlg->CreateGPSThread();
		return;
	}

	m_multi_mode = dlg.mode;
	m_multimode_attribute = dlg.attribute;
	::AfxBeginThread(ConfigureMultiMode, 0);

}

U32 m_reg_addr;
U32 m_reg_data;
UINT ConfigureRegister_thread(LPVOID param)
{
	U08 msg[9];
	msg[0] = 0x72;

	msg[1] = m_reg_addr >> 24 & 0xFF;
	msg[2] = m_reg_addr >> 16 & 0xFF;
	msg[3] = m_reg_addr >> 8 & 0xFF;
	msg[4] = m_reg_addr & 0xFF;

	msg[5] = m_reg_data >> 24 & 0xFF;
	msg[6] = m_reg_data >> 16 & 0xFF;
	msg[7] = m_reg_data >> 8 & 0xFF;
	msg[8] = m_reg_data & 0xFF;

	int len = CGPSDlg::gpsDlg->SetMessage(msg,sizeof(msg));
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(CGPSDlg::m_inputMsg, len, "Configure Register Successful...");
/*
	CGPSDlg::gpsDlg->//WaitEvent();
	CGPSDlg::gpsDlg->ClearQue();
	CGPSDlg::gpsDlg->SendToTarget(CGPSDlg::m_inputMsg,len,"Configure Register Successful...");
	CGPSDlg::gpsDlg->SetMode();  
	CGPSDlg::gpsDlg->CreateGPSThread();
*/
	return 0;	
}

void CGPSDlg::OnBinaryConfigureregister()
{	
	if(!CheckConnect())return;

	CCon_register dia;

	dia.m_addr = "00000000";
	dia.m_data = "00000000";

	if(dia.DoModal() == IDOK)
	{
		m_reg_addr = ConvertCharToU32(dia.m_addr);
		m_reg_data = ConvertCharToU32(dia.m_data);
		::AfxBeginThread(ConfigureRegister_thread, 0);
	}else
	{
		SetMode();  
		CreateGPSThread();
	}
}

void CGPSDlg::OnBinaryConfiguresubsecregister()
{
	if(!CheckConnect())
	{
		return;
	}

	m_inputMode = 0;
	CCigRgsDlg dlg;
	INT_PTR ret = dlg.DoModal();

	if(ret == IDOK)
	{
		m_ms     = dlg.MS;
		m_ns     = dlg.NS;
		m_pllDiv = dlg.PLLDIV;		
		AfxBeginThread(CigRgsThread, 0);		
	}
	else
	{
		SetMode();  
		CreateGPSThread();
	}
}

U08 m_1pps_mode;
U32 m_1pps_attr;
UINT Configure1pps_thread(LPVOID param)
{
	U08 msg[3] ;
	msg[0] = 0x3E ;

	msg[1] = m_1pps_mode ;
	msg[2] = (U08)m_1pps_attr;

	int len = CGPSDlg::gpsDlg->SetMessage(msg,sizeof(msg));
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(CGPSDlg::m_inputMsg, len, "Configure GPS Measurement Successful...");
/*
	CGPSDlg::gpsDlg->//WaitEvent();
	CGPSDlg::gpsDlg->ClearQue();
	CGPSDlg::gpsDlg->SendToTarget(CGPSDlg::m_inputMsg,len,"Configure GPS Measurement Successful...");
	CGPSDlg::gpsDlg->SetMode();  
	CGPSDlg::gpsDlg->CreateGPSThread();
*/
	return 0;	
}

void CGPSDlg::OnBinaryConfigure1pps()
{
	CCon1PPS *p1ppsDlg;
	if(!CheckConnect())return;
	m_inputMode = 0;
	p1ppsDlg = new CCon1PPS;
	INT_PTR ret = p1ppsDlg->DoModal();
	if(ret == IDOK)
	{
		m_1pps_mode = p1ppsDlg->_1ppsmode;
		m_1pps_attr = p1ppsDlg->_1ppsattr;

		::AfxBeginThread(Configure1pps_thread, 0);		
	}else
	{
		SetMode();  
		CreateGPSThread();
	}	
}

void CGPSDlg::Show_Noise()
{
	CString tmp;
	tmp.Format("%d", m_noisePower);
	m_noise.SetWindowText(tmp);
}

U16 m_multipath;
U16 m_multipath_attribute;
UINT ConfigureMultipath(LPVOID param)
{

	U08 msg[3] ;
	msg[0] = 0xF ;
	msg[1] = (U08)m_multipath;
	msg[2] = (U08)m_multipath_attribute;

	int len = CGPSDlg::gpsDlg->SetMessage(msg,sizeof(msg));
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(CGPSDlg::m_inputMsg, len, "Configure Multi-Path Successful...");
/*
	CGPSDlg::gpsDlg->//WaitEvent();
	CGPSDlg::gpsDlg->ClearQue();
	CGPSDlg::gpsDlg->SendToTarget(CGPSDlg::m_inputMsg,len,"Configure Multi-Path Successful...");
	CGPSDlg::gpsDlg->SetMode();  
	CGPSDlg::gpsDlg->CreateGPSThread();
*/
	return 0;	
}

void CGPSDlg::OnBinaryConfiguremultipath()
{
	if(!CheckConnect())	
	{
		return;
	}

	CConMultiPath dlg;

	if(dlg.DoModal()== IDOK)
	{
		CGPSDlg::gpsDlg->SetMode();  
		CGPSDlg::gpsDlg->CreateGPSThread();
		return;
	}

	m_multipath = dlg.multipath;
	m_multipath_attribute = dlg.attribute;

	::AfxBeginThread(ConfigureMultipath, 0);

}

U16 m_waas;
U16 m_waas_attribute;

UINT ConfigureWAAS(LPVOID param)
{

	U08 msg[3] ;
	msg[0] = 55 ;
	msg[1] = (U08)m_waas;
	msg[2] = (U08)m_waas_attribute;

	int len = CGPSDlg::gpsDlg->SetMessage(msg,sizeof(msg));
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(CGPSDlg::m_inputMsg, len, "Configure WAAS Successful...");
/*
	CGPSDlg::gpsDlg->//WaitEvent();
	CGPSDlg::gpsDlg->ClearQue();
	CGPSDlg::gpsDlg->SendToTarget(CGPSDlg::m_inputMsg,len,"Configure WAAS Successful...");
	CGPSDlg::gpsDlg->SetMode();  
	CGPSDlg::gpsDlg->CreateGPSThread();
*/
	return 0;	
}

void CGPSDlg::OnWaasWaas()
{
	if(CheckConnect())	
	{
		CConWaas dia;

		if(dia.DoModal()== IDOK)
		{
			m_waas = dia.waas;
			m_waas_attribute = dia.attribute;

			::AfxBeginThread(ConfigureWAAS, 0);
		}
		else
		{
			CGPSDlg::gpsDlg->SetMode();  
			CGPSDlg::gpsDlg->CreateGPSThread();
		}
	}
}

UINT GetAlmanacThread(LPVOID param)
{

#if TMP_ALMANAC
	CGPSDlg::gpsDlg->GetAlmanac_tmp();
#else
#if NEW_ALMANAC
	CGPSDlg::gpsDlg->GetAlmanac_new(m_almanac_filename,m_almanac_no,FALSE);
#else
	CGPSDlg::gpsDlg->GetAlmanac();
#endif
#endif
	return 0;
}

void CGPSDlg::GetAlmanac_new(CString m_almanac_filename,U08 sv,U08 continues)
{
	int wait = 0;
	U08 msg[2] ;
	char BINMSG[1024] = {0};

	msg[0] = 0x50;
	msg[1] = sv;
	int res_len;
	int len = SetMessage(msg,sizeof(msg));
	FILE *f;

	//WaitEvent();
	ClearQue();
	if(SendToTarget(m_inputMsg,len,"Get Almance start...") == 1)
	{
		if(WRL == NULL )
		{
			WRL = new CWaitReadLog;
		}
		AfxBeginThread(WaitLogRead,0);
		WaitForSingleObject(waitlog,INFINITE);
		WRL->SetWindowText("Wait for get almanac");
		WRL->msg.SetWindowText("Please wait for get almanac!");

		U08 NumsOfEphemeris = 0;

		fopen_s(&f, m_almanac_filename, "wb+");
		fclose(f);

		while(1)
		{	
			wait++;
			if(wait == 50 )
			{ 
				Sleep(500);
				WRL->msg.SetWindowText("Retrieve almanac data is Failed!");
				Sleep(500);
				WRL->IsFinish = true;
				add_msgtolist("Retrieve almanac Failed...");	
				//goto TheLast;
			}
			U08 buff[1024] = {0};
			res_len = m_serial->GetBinary(buff, sizeof(buff));

			if(res_len==0x3b)
			{
				fopen_s(&f, m_almanac_filename, "ab");
				fwrite(&buff[5], 1, res_len - 8, f);
				fclose(f);
			}
			else
			{
				break;
			}

			NumsOfEphemeris = buff[7];
			//	}
			sprintf_s(BINMSG,"Retrieve Satellite ID # %d almanac",NumsOfEphemeris);
			WRL->msg.SetWindowText(BINMSG);

			if(NumsOfEphemeris==32)break;
			//}
		}	
		Sleep(500);
		WRL->msg.SetWindowText("Retrieve almanac data is completed!");
		Sleep(500);
		WRL->IsFinish = true;
		add_msgtolist("Retrieve almanac Successful...");	
		

	}

	if(!continues)
	{
		SetMode();  
		CreateGPSThread();
	}


	//	return TRUE;	
}

void CGPSDlg::OnEphemerisGetalmanac()
{
	CGetAlmanac frm;
	if(CheckConnect())	
	{
		if(frm.DoModal()==IDOK)
		{
			m_almanac_filename = frm.fileName;
			m_almanac_no = frm.sv;
			::AfxBeginThread(GetAlmanacThread, 0);
		}else
		{
			SetMode();  
			CreateGPSThread();
		}
	}
}

_1PPS_Timing_T _config_1pps_timing;
UINT configy_1pps_timing_thread(LPVOID param)
{
	U08 msg[31] = {0};
	CString tmp_msg;
	U08 temp[8];
	int i;
    
	if(TIMING_MODE)
	{
		//combo GPS/Glonass
		msg[0] = 0x54;
		msg[1] = _config_1pps_timing.Timing_mode;
		msg[2] = _config_1pps_timing.Survey_Length>>24 & 0xFF;
		msg[3] = _config_1pps_timing.Survey_Length>>16 & 0xFF;
		msg[4] = _config_1pps_timing.Survey_Length>>8 & 0xFF;
		msg[5] = _config_1pps_timing.Survey_Length & 0xFF;
		msg[6] = _config_1pps_timing.Standard_deviation>>24 & 0xFF;
		msg[7] = _config_1pps_timing.Standard_deviation>>16 & 0xFF;
		msg[8] = _config_1pps_timing.Standard_deviation>>8 & 0xFF;
		msg[9] = _config_1pps_timing.Standard_deviation & 0xFF;
		memcpy(temp,&_config_1pps_timing.latitude,8);
		for (i=0;i<8;i++)
			msg[10+i] = temp[7-i];
		memcpy(temp,&_config_1pps_timing.longitude,8);
		for (i=0;i<8;i++)
			msg[18+i] = temp[7-i];
		memcpy(temp,&_config_1pps_timing.altitude,4);
		for (i=0;i<4;i++)
			msg[26+i] = temp[3-i];

		msg[30] = _config_1pps_timing.attributes;
	}
	else
	{
		//old pure GPS
		msg[0] = 0x43;
		msg[1] = _config_1pps_timing.Timing_mode;
		msg[2] = _config_1pps_timing.Survey_Length>>24 & 0xFF;
		msg[3] = _config_1pps_timing.Survey_Length>>16 & 0xFF;
		msg[4] = _config_1pps_timing.Survey_Length>>8 & 0xFF;
		msg[5] = _config_1pps_timing.Survey_Length & 0xFF;
		memcpy(temp,&_config_1pps_timing.latitude,8);
		for (i=0;i<8;i++)
			msg[6+i] = temp[7-i];
		memcpy(temp,&_config_1pps_timing.longitude,8);
		for (i=0;i<8;i++)
			msg[14+i] = temp[7-i];
		memcpy(temp,&_config_1pps_timing.altitude,4);
		for (i=0;i<4;i++)
			msg[22+i] = temp[3-i];

		msg[26] = _config_1pps_timing.attributes;
	}

	int len = CGPSDlg::gpsDlg->SetMessage(msg, sizeof(msg));
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(CGPSDlg::m_inputMsg, len, "Configure 1PPS Timing Successful...");
/*
	CGPSDlg::gpsDlg->//WaitEvent();
	CGPSDlg::gpsDlg->ClearQue();
	CGPSDlg::gpsDlg->SendToTarget(CGPSDlg::m_inputMsg,len,"Configure 1PPS Timing Successful...");
	CGPSDlg::gpsDlg->SetMode();  
	CGPSDlg::gpsDlg->CreateGPSThread();
*/
	return 0;	
}

int m_1pps_delay;
int m_1pps_cable_attr;
UINT configy_1pps_cable_delay_thread(LPVOID param)
{
	U08 msg[6] ;
	CString tmp_msg;

	memset(msg,0,sizeof(msg));

	msg[0] = 0x45;
	msg[1] = m_1pps_delay>>24 & 0xFF;
	msg[2] = m_1pps_delay>>16 & 0xFF;
	msg[3] = m_1pps_delay>>8 & 0xFF;
	msg[4] = m_1pps_delay & 0xFF;
	msg[5] = m_1pps_cable_attr;

	int len = CGPSDlg::gpsDlg->SetMessage(msg,sizeof(msg));
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(CGPSDlg::m_inputMsg, len, "Configure 1PPS Cable Delay Successful...");
	return 0;	
}

void CGPSDlg::OnConfigure1ppstimingConfigure1ppstiming()
{
	CCon1PPS_Timing dlg_1pps_timing;

	if(CheckConnect())	
	{
		if(dlg_1pps_timing.DoModal()==IDOK)
		{
			memcpy(&_config_1pps_timing,&dlg_1pps_timing._1pps_timing,sizeof(_1PPS_Timing_T));
			::AfxBeginThread(configy_1pps_timing_thread, 0);
		}else
		{
			SetMode();  
			CreateGPSThread();
		}
	}
}

void CGPSDlg::OnConfigure1ppstimingConfigure1ppscabledelay()
{
	CCon1PPS_cable dlg_1pps_timing;

	if(CheckConnect())	
	{
		if(dlg_1pps_timing.DoModal()==IDOK)
		{
			m_1pps_delay = dlg_1pps_timing.cable_delay;
			m_1pps_cable_attr = dlg_1pps_timing.attr;
			::AfxBeginThread(configy_1pps_cable_delay_thread, 0);
		}else
		{
			SetMode();  
			CreateGPSThread();
		}
	}
}

int m_dop_sel,m_dop_pdop,m_dop_hdop,m_dop_gdop,m_dop_attr;

UINT configy_1pps_dop_thread(LPVOID param)
{
	U08 msg[9] = {0};

	msg[0] = 0x2A;
	msg[1] = m_dop_sel;
	msg[2] = m_dop_pdop>>8 & 0xFF;
	msg[3] = m_dop_pdop & 0xFF;
	msg[4] = m_dop_hdop>>8 & 0xFF;
	msg[5] = m_dop_hdop & 0xFF;
	msg[6] = m_dop_gdop>>8 & 0xFF;
	msg[7] = m_dop_gdop & 0xFF;
	msg[8] = m_dop_attr;

	int len = CGPSDlg::gpsDlg->SetMessage(msg,sizeof(msg));
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(CGPSDlg::m_inputMsg, len, "Configure DOP Mask Successful...");
/*
	CGPSDlg::gpsDlg->//WaitEvent();
	CGPSDlg::gpsDlg->ClearQue();

	CGPSDlg::gpsDlg->SendToTarget(CGPSDlg::m_inputMsg,len,"Configure DOP Mask Successful...");
	CGPSDlg::gpsDlg->SetMode();  
	CGPSDlg::gpsDlg->CreateGPSThread();
*/
	return 0;	
}

void CGPSDlg::OnConfigure1ppstimingConfigure1pps()
{
	CCon1PPS_DOP dlg;
	if(CheckConnect())	
	{
		if(dlg.DoModal()==IDOK)
		{
			m_dop_sel = dlg.dop_sel;
			m_dop_pdop = (int)(dlg.pdop*10);
			m_dop_hdop = (int)(dlg.hdop*10);
			m_dop_gdop = (int)(dlg.gdop*10);
			m_dop_attr = dlg.attr;
			::AfxBeginThread(configy_1pps_dop_thread, 0);
		}
		else
		{
			SetMode();  
			CreateGPSThread();
		}
	}
}

int m_elev_sel,m_elev_elev,m_elev_cnr,m_elev_attr;
UINT ConfigElevationAndCnrMaskThread(LPVOID param)
{
	U08 msg[5] = {0};
	msg[0] = 0x2B;
	msg[1] = m_elev_sel;
	msg[2] = m_elev_elev;
	msg[3] = m_elev_cnr;
	msg[4] = m_elev_attr;

	int len = CGPSDlg::gpsDlg->SetMessage(msg,sizeof(msg));

	CGPSDlg::gpsDlg->//WaitEvent();
	CGPSDlg::gpsDlg->ClearQue();

	CGPSDlg::gpsDlg->SendToTarget(CGPSDlg::m_inputMsg,len,"Configure Elevation and CNR Mask Successful...");
	CGPSDlg::gpsDlg->SetMode();  
	CGPSDlg::gpsDlg->CreateGPSThread();
	return 0;	
}

void CGPSDlg::OnConfigElevationAndCnrMask()
{
	CCon1PPS_ElevCNR dlg;

	if(CheckConnect())	
	{
		if(dlg.DoModal()==IDOK)
		{
			m_elev_sel = dlg.sel;
			m_elev_elev = dlg.elev;
			m_elev_cnr = dlg.cnr;
			m_elev_attr = dlg.attr;
			//m_1pps_delay = dlg_1pps_timing.cable_delay;
			///m_1pps_cable_attr = dlg_1pps_timing.attr;
			::AfxBeginThread(ConfigElevationAndCnrMaskThread, 0);
		}
		else
		{
			SetMode();  
			CreateGPSThread();
		}
	}
}

inline U08 comma_count(const char *buff, char *comma)
{
	U08 count=0;
	U08 index=0;
	for(count=0;;count++,buff++)
	{
		if(*buff == 0 || *buff=='*') 
		{
			comma[index]=count;
			break;
		}
		else
		if(*buff == ',')
		{
			comma[index]=count;
			index++;
		}
	}
	return index;
}

int m_proprietary_id, m_proprietary_psti, m_proprietary_attr;
UINT configy_proprietary_nmea_thread(LPVOID param)
{
	U08 msg[4] = {0};
	CString tmp_msg;

	msg[0] = 0x14;
	msg[1] = m_proprietary_id;
	msg[2] = m_proprietary_psti;
	msg[3] = m_proprietary_attr;

	int len = CGPSDlg::gpsDlg->SetMessage(msg,sizeof(msg));

	CGPSDlg::gpsDlg->//WaitEvent();
	CGPSDlg::gpsDlg->ClearQue();

	CGPSDlg::gpsDlg->SendToTarget(CGPSDlg::m_inputMsg,len,"Configure Proprietary NMEA Successful...");
	CGPSDlg::gpsDlg->SetMode();  
	CGPSDlg::gpsDlg->CreateGPSThread();

	return 0;	
}

void CGPSDlg::On1ppstimingConfigureproprietarynmea()
{
	CProprietary_nmea dlg;

	if(CheckConnect())	
	{
		if(dlg.DoModal()==IDOK)
		{
			m_proprietary_id = dlg.psti_id;
			m_proprietary_psti = dlg.psti_interval;
			m_proprietary_attr = dlg.attr;
			//m_1pps_delay = dlg_1pps_timing.cable_delay;
			///m_1pps_cable_attr = dlg_1pps_timing.attr;
			::AfxBeginThread(configy_proprietary_nmea_thread, 0);
		}else
		{
			SetMode();  
			CreateGPSThread();
		}
	}
}

UINT SetAlmanacThread(LPVOID pParam)
{	
	CGPSDlg::gpsDlg->SetAlmanac(FALSE);	
	return 0;
}

void CGPSDlg::SetAlmanac(U08 continues)	
{	
	U16 SVID;	
//	int i;	
	U08 messages[100];  
	U08 msg[100];
	BYTE buffer[0x1000];
	ULONGLONG dwBytesRemaining = m_ephmsFile.GetLength();

	if(dwBytesRemaining == 0)
	{
		m_ephmsFile.Close();	
		if(!continues)
		{
			SetMode(); 
			CreateGPSThread();	
		}
		return;
	}

	m_ephmsFile.Read(buffer,1);
	int one_entry_size = buffer[0] + 3;	//Size 1 byte, satellite id 2 bytes.

	if(dwBytesRemaining == 32 * one_entry_size)
	{
		m_ephmsFile.SeekToBegin();

		while(1)
		{
			UINT nBytesRead = m_ephmsFile.Read(buffer,one_entry_size);	
			//TRACE("nBytesRead=%d\r\n",nBytesRead);
			if(nBytesRead<=0)
				break;
			memset(msg, 0, sizeof(msg));
			memset(messages, 0, sizeof(messages));
			msg[0] = 0x51;

			memcpy(&msg[1],&buffer[1],one_entry_size-1);
			SVID = buffer[1]<<8 | buffer[2];	

			if(buffer[0] == 0) 
			{
				TRACE("SVID=%d,continue\r\n",SVID);
				continue;
			}

			if(continues)
			{
				msg[one_entry_size] = 0;
			}else
			{
				if(SVID == 32)
					msg[one_entry_size] = 1;
				else
					msg[one_entry_size] = 0;
			}

			int len = SetMessage2(messages, msg, one_entry_size + 1);
			sprintf_s(m_nmeaBuffer, "Set SV#%d Almanac Successful...",SVID);
			if(!SendToTargetNoWait(messages, len,m_nmeaBuffer))
			{
				sprintf_s(m_nmeaBuffer, "Set SV#%d Almanac Fail...",SVID);
				add_msgtolist(m_nmeaBuffer);
			}

			//Sleep(10);
		}
	}
	else
	{
		AfxMessageBox("The Almanac data Format of the file is wrong");
		//return;
	}	
	m_ephmsFile.Close();	

	if(!continues)
	{
		SetMode(); 
		CreateGPSThread();	
	}

}

void CGPSDlg::OnEphemerisSetalmanac()
{
	if(!CheckConnect())
	{
		return;
	}

	m_inputMode  = 0;
	CString fileName("GPS_Almanac.log");	
	CFileDialog dlgFile(true, _T("log"), fileName, OFN_HIDEREADONLY, _T("Almanac Files (*.log)|*.log||"), this);
	INT_PTR nResult = dlgFile.DoModal();
	fileName = dlgFile.GetPathName();	
	CFileException ef;
	try
	{
		if(nResult == IDOK)
		{  				
			if(!m_ephmsFile.Open(fileName, CFile::modeRead, &ef))
			{
				ef.ReportError();
				SetMode();
				CreateGPSThread();
				return;
			}				
			AfxBeginThread(SetAlmanacThread, 0);			
		}		
		else
		{
			SetMode();
			CreateGPSThread();
		}
	}
	catch(CFileException *fe)
	{
		fe->ReportError();
		fe->Delete();
		return;
	}	
	fileName.ReleaseBuffer();		
}

LRESULT CGPSDlg::OnMyDeviceChange(WPARAM wParam, LPARAM lParam)
{
	if( DBT_DEVICEARRIVAL == wParam || DBT_DEVICEREMOVECOMPLETE == wParam ) 
	{
		PDEV_BROADCAST_HDR pHdr = (PDEV_BROADCAST_HDR)lParam;
		PDEV_BROADCAST_DEVICEINTERFACE pDevInf;
		PDEV_BROADCAST_HANDLE pDevHnd;
		PDEV_BROADCAST_OEM pDevOem;
		PDEV_BROADCAST_PORT pDevPort;
		PDEV_BROADCAST_VOLUME pDevVolume;
		switch( pHdr->dbch_devicetype ) 
		{
		case DBT_DEVTYP_DEVICEINTERFACE:
			pDevInf = (PDEV_BROADCAST_DEVICEINTERFACE)pHdr;
			//UpdateDevice(pDevInf, wParam);
			break;

		case DBT_DEVTYP_HANDLE:
			pDevHnd = (PDEV_BROADCAST_HANDLE)pHdr;
			break;

		case DBT_DEVTYP_OEM:
			pDevOem = (PDEV_BROADCAST_OEM)pHdr;
			break;

		case DBT_DEVTYP_PORT:
			pDevPort = (PDEV_BROADCAST_PORT)pHdr;
			plugin_wParam = wParam;
			plugin_port_name = pDevPort->dbcp_name;
			if( DBT_DEVICEARRIVAL == wParam )	//Plugin
			{
				SetTimer(DELAY_PLUGIN_TIMER, 2000, NULL);
			}
			else	//Remove
			{
				SetTimer(DELAY_PLUGIN_TIMER, 3500, NULL);
			}
			break;

		case DBT_DEVTYP_VOLUME:
			pDevVolume = (PDEV_BROADCAST_VOLUME)pHdr;
			break;
		}
	}
	return 0;
}

LRESULT CGPSDlg::OnSaveNmeaEvent(WPARAM wParam, LPARAM lParam)
{
	if(wParam==CSaveNmea::StopWriteNmea)
	{

	}
	else if(wParam==CSaveNmea::ClickClose)
	{
		delete m_saveNmeaDlg;
		m_saveNmeaDlg = NULL;
	}
	return 0;
}

LRESULT CGPSDlg::OnPlayNmeaEvent(WPARAM wParam, LPARAM lParam)
{
	if(wParam==CPlayNmea::IntervalChange)
	{
		if(m_nmeaPlayInterval!=(int)lParam)
		{
			_nmeaPlayInterval.Lock();
			m_nmeaPlayInterval = (int)lParam;
			_nmeaPlayInterval.Unlock();
		}
	}
	else if(wParam==CPlayNmea::PauseStatus)
	{
		m_nmeaPlayPause = (lParam==FALSE) ? false : true;
	}
	else if(wParam==CPlayNmea::ClickClose)
	{
		OnBnClickedStop();
	}
	return 0;
}

LRESULT CGPSDlg::OnUpdateEvent(WPARAM wParam, LPARAM lParam)
{
	UINT_PTR flag = wParam;
	GnssData& g = NMEA::gnssData;

	if((flag & GnssData::UpdateData) != 0)
	{
		DisplayDate(g.GetYear(), g.GetMonth(), g.GetDay());
	}
	if((flag & GnssData::UpdateTime) != 0)
	{
		DisplayTime(g.GetHour(), g.GetMinute(), g.GetSecond());
	}
	if((flag & GnssData::UpdateSpeed) != 0)
	{
		DisplaySpeed(g.GetSpeed());
	}
	if((flag & GnssData::UpdateDirection) != 0)
	{
		DisplayDirection(g.GetDirection());
	}
	if((flag & GnssData::UpdateFixMode) != 0)
	{
		DisplayStatus(g.GetFixMode());
	}
	if((flag & GnssData::UpdateLongitude) != 0)
	{
		DisplayLongitude(g.GetLongitude(), g.GetLongitudeEW());
	}
	if((flag & GnssData::UpdateLatitude) != 0)
	{
		DisplayLatitude(g.GetLatitude(), g.GetLatitudeNS());
	}	
	if((flag & GnssData::UpdateAltitude) != 0)
	{
		DisplayAltitude(g.GetAltitude());
	}	
	
	if((flag & GnssData::UpdateHdop) != 0)
	{
		DisplayHdop(g.GetHdop());
	}	
	return 0;
}
LRESULT CGPSDlg::OnKernelReboot(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

void CGPSDlg::Close_Open_Port(WPARAM wParam, CString port_name)
{
	CString now_portname,szTmp;

	if( DBT_DEVICEARRIVAL == wParam ) {
		szTmp.Format(_T("Adding %s\r\n"), port_name);
	} else {
		szTmp.Format(_T("Removing %s\r\n"), port_name);
	}

	add_msgtolist(szTmp);

	if( DBT_DEVICEARRIVAL == wParam  && !m_isConnectOn) {
		CDevice_Adding dia_connect;
		CString cbo_portname,tmp,cbo_baudrate;
		int baudrate;

		m_BaudRateCombo.GetLBText(m_BaudRateCombo.GetCurSel(),tmp);
		baudrate = atoi(tmp);
		dia_connect.setPort_Baudrate(port_name,baudrate);
		if(dia_connect.DoModal() == IDOK)
		{
			for (int i=0;i<m_ComPortCombo.GetCount();i++)
			{
				m_ComPortCombo.GetLBText(i,cbo_portname);
				if(cbo_portname.Compare(port_name) == 0 )
				{
					m_ComPortCombo.SetCurSel(i);
					break;
				}
			}

			tmp.Format("%d",dia_connect.m_baudrate);

			for (int i=0;i<m_BaudRateCombo.GetCount();i++)
			{
				m_BaudRateCombo.GetLBText(i,cbo_baudrate);
				if(cbo_baudrate.Compare(tmp) == 0 )
				{
					m_BaudRateCombo.SetCurSel(i);
					break;
				}
			}
			OnBnClickedConnect();

		}
	}else if(m_isConnectOn)
	{
		m_ComPortCombo.GetLBText(m_ComPortCombo.GetCurSel(),now_portname);
		if(now_portname.Compare(port_name) == 0 )
		{
			OnBnClickedClose();
		}
	}
}

UINT MinihomerSettageccoThread(LPVOID pParam)
{	
	CGPSDlg::gpsDlg->MinihomerSettagecco();
	return TRUE;	
}

void CGPSDlg::MinihomerSettagecco()
{
	U08 msg[3] ,checksum=0;
	CString temp;
	U32 data = 0;


	msg[0] = 0x7E; // set device_id;
	msg[1] = 0x01;
	msg[2] = 0x88;

	int len = SetMessage(msg,sizeof(msg));

	//WaitEvent();
	ClearQue();
	if(SendToTarget(m_inputMsg,len,"Set miniHomer Tag Successful.",1) != 1)
		add_msgtolist("Set miniHomer Tag Fail.");	

	SetMode();
	CreateGPSThread();
}

void CGPSDlg::OnMinihomerSettagecco()
{
	if(!CheckConnect())return;

	AfxBeginThread(MinihomerSettageccoThread,0);	

}

void CGPSDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}


void CGPSDlg::OnGlonassConfigureusbdriver()
{
#if WITH_CONFIG_USB_BAUDRATE
	U08 is_open;
	CConfig_USB usb_dia;

	is_open = m_serial->IsOpened();
	if(is_open)
		OnBnClickedClose();

	usb_dia.DoModal();

	if(is_open)
		OnBnClickedConnect();
#endif

}

void CGPSDlg::OnBnClickedRomMode()
{
#if WITH_CONFIG_USB_BAUDRATE
	if(m_serial == NULL)
	{
		m_rom_mode.SetCheck(0);
		return;
	}
	if(m_serial->IsOpened())
	{
		BYTE version;
		if(CP210x_GetPartNumber(m_serial->GetHandle(), &version) == CP210x_SUCCESS)
		{
			if(version == CP210x_CP2101_VERSION)
			{
				MessageBox("CP2101 is not supported in this application", "Error", MB_OK | MB_ICONEXCLAMATION);
			}
			else
			{
				if(m_rom_mode.GetCheck())					
					CP210x_SetBaudRateConfig(m_serial->GetHandle(), m_ROM_BaudConfigData);
				else
					CP210x_SetBaudRateConfig(m_serial->GetHandle(), m_DefaultBaudConfigData);
				OnBnClickedClose();
				OnBnClickedConnect();
			}
			
		}else
		{
			MessageBox("CP210x not Found!", "Error", MB_OK | MB_ICONEXCLAMATION);
		}
	}
#endif
}

int gnss_selforNAV;
int gnss_selattr;
UINT configy_gnss_selectionforNAV_thread(LPVOID param)
{
	U08 msg[3];

	memset(msg,0,sizeof(msg));

	msg[0] = 0x52;
	msg[1] = gnss_selforNAV;
	msg[2] = gnss_selattr;

	CGPSDlg::gpsDlg->DeleteNmeaMemery();	
	int len = CGPSDlg::gpsDlg->SetMessage(msg,sizeof(msg));
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(CGPSDlg::m_inputMsg, len, "Configure GNSS Selection for Navigation System Successful...");
	return 0;
}
/*
UINT ConfigGnssSelectionForNav2Thread(LPVOID param)
{
	U08 msg[4] = {0};

	msg[0] = 0x66;
	msg[1] = 0x01;
	msg[2] = gnss_selforNAV;
	msg[3] = gnss_selattr;

	CGPSDlg::gpsDlg->DeleteNmeaMemery();	
	int len = CGPSDlg::gpsDlg->SetMessage(msg,sizeof(msg));
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(CGPSDlg::m_inputMsg, len, "Configure GNSS Sel for Nav System Successful...");
	return 0;
}
*/
/*
void CGPSDlg::OnBinaryConfiguregnssselectionfornavigationsystem()
{
	if(!CheckConnect())	
	{
		return;
	}	

	CConGNSSSelectionforNAV dlg;
	if(dlg.DoModal() != IDOK)
	{
		SetMode();  
		CreateGPSThread();
		return;
	}

	gnss_selforNAV = dlg.selection+1;
	gnss_selattr = dlg.attr;
	::AfxBeginThread(ConfigGnssSelectionForNav2Thread, 0);
}
*/
void CGPSDlg::ClearGlonass()
{
	if(m_kNumList && m_kNumList.GetSafeHwnd())
	{
		m_kNumList.DeleteAllItems();
	}
	memset(&m_gnssTemp, 0, sizeof(GNSS_T));
	memset(&m_gnss, 0, sizeof(GNSS_T));
}

void CGPSDlg::OnBnClickedKnumEnable()
{
	if(!CheckConnect())return;

	m_reg_addr = 0x90000000;
	m_reg_data = 0x00000001;
	::AfxBeginThread(ConfigureRegister_thread, 0);

	ClearGlonass();
}

void CGPSDlg::OnBnClickedKnumDisable()
{
	if(!CheckConnect())return;

	m_reg_addr = 0x90000000;
	m_reg_data = 0x00000000;
	::AfxBeginThread(ConfigureRegister_thread, 0);

	ClearGlonass();
}

int nmea_com;
int nmea_com_attr;
UINT config_NMEA_Comport_thread(LPVOID param)
{
	U08 msg[3];

	memset(msg, 0, sizeof(msg));

	msg[0] = 0x7D;
	msg[1] = nmea_com;
	msg[2] = nmea_com_attr;

	int len = CGPSDlg::gpsDlg->SetMessage(msg,sizeof(msg));
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(CGPSDlg::m_inputMsg, len, "Configure NMEA Output Comport Successful...");
	return 0;
}

void CGPSDlg::OnBinaryConfigurenmeaoutput32953()
{
	CConNMEAComport dlg;

	if(CheckConnect())	
	{
		if(dlg.DoModal()==IDOK)
		{
			nmea_com = dlg.comport;
			nmea_com_attr = dlg.attr;
			::AfxBeginThread(config_NMEA_Comport_thread, 0);
		}
		else
		{
			SetMode();  
			CreateGPSThread();
		}
	}



	
}

int nmea_talker;
int nmea_talker_attr;
UINT config_NMEA_TalkerID_thread(LPVOID param)
{
	U08 msg[3];

	memset(msg,0,sizeof(msg));

	msg[0] = 0x4B;
	msg[1] = nmea_talker;
	msg[2] = nmea_talker_attr;

	int len = CGPSDlg::gpsDlg->SetMessage(msg,sizeof(msg));
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(CGPSDlg::m_inputMsg, len, "Configure NMEA Talker ID Successful...");
	return 0;
}

void CGPSDlg::OnBinaryConfigurenmeatalkerid()
{
	CCon_NMEA_TalkerID dlg;

	if(CheckConnect())	
	{
		if(dlg.DoModal()==IDOK)
		{
			nmea_talker = dlg.talkerid;
			nmea_talker_attr = dlg.attr;
			::AfxBeginThread(config_NMEA_TalkerID_thread, 0);
		}else
		{
			SetMode();  
			CreateGPSThread();
		}
	}

}
/*
int acquisition_mode;
int acquisition_attr;
UINT config_glonass_acquisition_thread(LPVOID param)
{
	U08 msg[3];

	memset(msg,0,sizeof(msg));

	msg[0] = 0x53;
	msg[1] = acquisition_mode;
	msg[2] = acquisition_attr;

	int len = CGPSDlg::gpsDlg->SetMessage(msg,sizeof(msg));
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(CGPSDlg::m_inputMsg, len, "Configure Acquisition Mode Successful...");
	return 0;
}

UINT config_glonass_acquisition_ascii_thread(LPVOID param)
{
	U08 msg[64];

	memset(msg, 0, sizeof(msg));

	if(acquisition_mode == 0)
	{
		strcpy_s((char*)msg, sizeof(msg), "$PSTI,128,0*15\r\n");
	}
	else
	{
		strcpy_s((char*)msg, sizeof(msg), "$PSTI,128,1*14\r\n");
	}
	CGPSDlg::gpsDlg->//WaitEvent();
	CGPSDlg::gpsDlg->ClearQue();
	CGPSDlg::gpsDlg->m_serial->SendData(msg,strlen((char*)msg));

	CGPSDlg::gpsDlg->add_msgtolist("Configure Acquisition Mode Successful...");	

	CGPSDlg::gpsDlg->SetMode();  
	CGPSDlg::gpsDlg->CreateGPSThread();

	return 0;
}
*/
/*
void CGPSDlg::OnCfgGlonassAcquisitionMode()
{
	CCon_acquisition dlg;

	if(CheckConnect())	
	{
		if(dlg.DoModal()==IDOK)
		{
			acquisition_mode = dlg.auto_power_off;
			acquisition_attr = dlg.attr;
			if(dlg.mode == 0)
				::AfxBeginThread(config_glonass_acquisition_thread, 0);
			else
				::AfxBeginThread(config_glonass_acquisition_ascii_thread, 0);
		}else
		{
			SetMode();  
			CreateGPSThread();
		}
	}
}
*/
void CGPSDlg::OnGetGlonassEphemeris()
{
	if(!CheckConnect())
	{
		return;
	}
	m_inputMode = 0;	
	CGetEphemerisDlg dlg;
	dlg.SetEphemerisType(CGetEphemerisDlg::GlonassEphemeris);
	if(dlg.DoModal() != IDOK) 
	{
		SetMode();  
		CreateGPSThread();
	}
}

void CGPSDlg::OnGetBeidouEphemeris()
{
	if(!CheckConnect())
	{
		return;
	}
	m_inputMode = 0;	
	CGetEphemerisDlg dlg;
	dlg.SetEphemerisType(CGetEphemerisDlg::BeidouEphemeris);
	if(dlg.DoModal() != IDOK) 
	{
		SetMode();  
		CreateGPSThread();
	}
}

void CGPSDlg::SetBeidouEphms(U08 continues)	
{	
	const int PureGeoEphSize = 120;
	const int PureMeoEphSize = 81;
	const int EphCommandSize = 2;
	const int EphExtraSize = 4;
	const int EphHeaderSize = EphCommandSize + EphExtraSize;
	const int GeoRecordSize = PureGeoEphSize + EphExtraSize;
	const int MeoRecordSize = PureMeoEphSize + EphExtraSize;
	const int GeoCount = 5;
	const int MeoCount = 32;
	const int TotalSatelliteCount = GeoCount + MeoCount;

	CString txtMsg;
	if(g_setting.boostEphemeris)
	{
		CGPSDlg::gpsDlg->m_nDownloadBaudIdx = g_setting.boostBaudIndex;
		BoostBaudrate(FALSE);
	}

	int dwBytesRemaining = (int)m_ephmsFile.GetLength();
	if(dwBytesRemaining==GeoRecordSize || dwBytesRemaining==MeoRecordSize || 
		dwBytesRemaining==(GeoRecordSize*GeoCount + MeoRecordSize*MeoCount))
	{
		while(dwBytesRemaining > 0)
		{
			BinaryData cmd;
			BYTE buf[EphExtraSize] = {0};
			UINT nBytesRead = m_ephmsFile.Read(buf, EphExtraSize);		
			U16 svId = MAKEWORD(buf[1], buf[0]);	
			if(buf[2]==0)
			{	//GEO satellite
				cmd.Alloc(PureGeoEphSize + EphHeaderSize);
				*cmd.GetBuffer(0) = 0x67;
				*cmd.GetBuffer(1) = 0x01;
				*cmd.GetBuffer(2) = buf[0];
				*cmd.GetBuffer(3) = buf[1];
				*cmd.GetBuffer(4) = buf[2];
				*cmd.GetBuffer(5) = buf[3];
				nBytesRead += m_ephmsFile.Read(cmd.GetBuffer(EphHeaderSize), PureGeoEphSize);	
			}
			else if(buf[2]==1)
			{	//MEO/IGSO satellite
				cmd.Alloc(PureMeoEphSize + EphHeaderSize);
				*cmd.GetBuffer(0) = 0x67;
				*cmd.GetBuffer(1) = 0x01;
				*cmd.GetBuffer(2) = buf[0];
				*cmd.GetBuffer(3) = buf[1];
				*cmd.GetBuffer(4) = buf[2];
				*cmd.GetBuffer(5) = buf[3];
				nBytesRead += m_ephmsFile.Read(cmd.GetBuffer(EphHeaderSize), PureMeoEphSize);	
			}
			else
			{
				AfxMessageBox("The Beidou Ephemeris data Format of the file is incorrect.");
				break;
			}
			dwBytesRemaining -= nBytesRead;

			if(buf[3]==0)
			{
				continue;
			}
			BinaryCommand commad;
			commad.SetData(cmd);
			txtMsg.Format("Set SV#%d Beidou Ephemeris Successful...", svId);
			if(!SendToTargetNoWait(commad.GetBuffer(), commad.Size(), txtMsg))
			{
				txtMsg.Format("Set SV#%d Beidou Ephemeris Fail...", svId);
				add_msgtolist(txtMsg);
			}
		}	
	}
	else
	{
		AfxMessageBox("The Beidou Ephemeris data Format of the file is wrong");
		//return;
	}	
	m_ephmsFile.Close();	
	if(!continues)
	{
		if(g_setting.boostEphemeris)
		{
			BoostBaudrate(TRUE, ChangeToTemp, true);
		}
		SetMode(); 
		CreateGPSThread();	
	}
}

void CGPSDlg::SetGlonassEphms(U08 continues)	
{	
	if(g_setting.boostEphemeris)
	{
		CGPSDlg::gpsDlg->m_nDownloadBaudIdx = g_setting.boostBaudIndex;
		BoostBaudrate(FALSE);
	}

	U16 SVID;	
	U08 msg[53];  
	BYTE  buffer[0x1000];
	int len;
	ULONGLONG dwBytesRemaining = m_ephmsFile.GetLength();

	if(dwBytesRemaining == 42 || dwBytesRemaining	== 42*24)
	{
		while(dwBytesRemaining)
		{
			memset(msg, 0, 50);
			msg[0]  = 0x5C; //msgid    
			UINT nBytesRead = m_ephmsFile.Read(buffer,42);		
			if(IsGlonassEphmsEmpty(buffer)){dwBytesRemaining-=nBytesRead;	continue;}
			memcpy(&msg[1],buffer,nBytesRead);
			len = SetMessage(msg,43);
			dwBytesRemaining-=nBytesRead;
			SVID = m_inputMsg[5]&0xff;

			sprintf_s(m_nmeaBuffer, sizeof(m_nmeaBuffer), "Set SV#%d Glonass Ephemeris Successful...",SVID);
			if(!SendToTargetNoWait(m_inputMsg, len,m_nmeaBuffer))
			{
				sprintf_s(m_nmeaBuffer, sizeof(m_nmeaBuffer), "Set SV#%d Glonass Ephemeris Fail...",SVID);
				add_msgtolist(m_nmeaBuffer);
			}
			//TRACE("%d %d\n",SVID,dwBytesRemaining);
		}	
	}
	else
	{
		AfxMessageBox("The Glonass Ephemeris data Format of the file is wrong");
		//return;
	}	
	m_ephmsFile.Close();	
	if(!continues)
	{
		if(g_setting.boostEphemeris)
		{
			BoostBaudrate(TRUE, ChangeToTemp, true);
		}
		SetMode(); 
		CreateGPSThread();	
	}
}

UINT SetGlonassEphemerisThread(LPVOID pParam)
{	
	CGPSDlg::gpsDlg->SetGlonassEphms(FALSE);	
	//AfxEndThread(0);
	return 0;
}

UINT SetBeidouEphemerisThread(LPVOID pParam)
{	
	CGPSDlg::gpsDlg->SetBeidouEphms(FALSE);	
	//AfxEndThread(0);
	return 0;
}
void CGPSDlg::OnSetGlonassEphemeris()
{
	if(!CheckConnect())
	{
		return;
	}

	m_inputMode  = 0;
	CString fileName = m_lastGlEphFile;
	if(m_lastGlEphFile.IsEmpty())
	{	
		fileName = "Glonass_ephemeris.log";	
	}

	CFileDialog dlgFile(TRUE, _T("log"), fileName, 
		OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY, 
		_T("Log Files (*.log)|*.log||"), this);

	if(dlgFile.DoModal() != IDOK)
	{
		SetMode();
		CreateGPSThread();
		return;
	}

	fileName = dlgFile.GetPathName();	
	CFileException ef;
	try
	{
		if(!m_ephmsFile.Open(fileName,CFile::modeRead,&ef))
		{
			ef.ReportError();
			SetMode();
			CreateGPSThread();
			return;
		}
		AfxBeginThread(SetGlonassEphemerisThread,0);			
	}
	catch(CFileException *fe)
	{
		fe->ReportError();
		fe->Delete();
		return;
	}	
}

void CGPSDlg::OnSetBeidouEphemeris()
{
	if(!CheckConnect())
	{
		return;
	}

	m_inputMode = 0;
	CString fileName = m_lastBdEphFile;
	if(m_lastBdEphFile.IsEmpty())
	{	
		fileName = "Beidou_ephemeris.log";	
	}

	CFileDialog dlgFile(TRUE, _T("log"), fileName, 
		OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY, 
		_T("Log Files (*.log)|*.log||"), this);

	if(dlgFile.DoModal() != IDOK)
	{
		SetMode();
		CreateGPSThread();
		return;
	}

	fileName = dlgFile.GetPathName();	
	CFileException ef;
	try
	{
		if(!m_ephmsFile.Open(fileName, CFile::modeRead, &ef))
		{
			ef.ReportError();
			SetMode();
			CreateGPSThread();
			return;
		}
		AfxBeginThread(SetBeidouEphemerisThread, 0);			
	}
	catch(CFileException *fe)
	{
		fe->ReportError();
		fe->Delete();
		return;
	}	
}

void CGPSDlg::GetGlonassAlmanac(CString m_almanac_filename,U08 sv,U08 continues)
{
	int wait = 0;
	U08 msg[2];
	char BINMSG[1024] = {0};

	msg[0] = 0x5D;
	msg[1] = sv;
	int res_len;
	int len = SetMessage(msg,sizeof(msg));
	FILE *f = NULL;

	//WaitEvent();
	ClearQue();
	if(SendToTarget(m_inputMsg,len,"Get Glonass Almance start...") == 1)
	{
		if(WRL == NULL )
		{
			WRL = new CWaitReadLog;
		}
		AfxBeginThread(WaitLogRead,0);
		WaitForSingleObject(waitlog,INFINITE);
		WRL->SetWindowText("Wait for get Glonass almanac");
		WRL->msg.SetWindowText("Please wait for get Glonass almanac!");

		U08 NumsOfEphemeris = 0;

		fopen_s(&f, m_almanac_filename, "wb+");
		fclose(f);

		while(1)
		{	
			wait++;
			if(wait == 50 ){ 
				Sleep(500);
				WRL->msg.SetWindowText("Retrieve Glonass almanac data is Failed!");
				Sleep(500);
				WRL->IsFinish = true;
				add_msgtolist("Retrieve Glonass almanac Failed...");	
				//goto TheLast;
			}
			U08 buff[1024] = {0};
			res_len = m_serial->GetBinary(buff, sizeof(buff));

			if(res_len==32)
			{
				fopen_s(&f, m_almanac_filename, "ab");
				//fwrite(&buff[5],1,res_len-7,f);
				fwrite(&buff[5],1,res_len-8,f);
				fclose(f);
			}
			else
			{
				break;
			}


			NumsOfEphemeris = buff[5];
			TRACE("NumsOfEphemeris=%d\r\n",NumsOfEphemeris);
			//	}
			sprintf_s(BINMSG, sizeof(BINMSG), "Retrieve Glonass Satellite ID # %d almanac", NumsOfEphemeris);
			WRL->msg.SetWindowText(BINMSG);

			if(NumsOfEphemeris==24)break;
			//}
		}	
		Sleep(500);
		WRL->msg.SetWindowText("Retrieve Glonass almanac data is completed!");
		Sleep(500);
		WRL->IsFinish = true;
		add_msgtolist("Retrieve Glonass almanac Successful...");	

	}

	if(!continues)
	{
		SetMode();  
		CreateGPSThread();
	}

	//	return TRUE;	


}

UINT GetGlonassAlmanacThread(LPVOID param)
{
	CGPSDlg::gpsDlg->GetGlonassAlmanac(m_almanac_filename,m_almanac_no,FALSE);
	return 0;
}

void CGPSDlg::OnEphemerisGetglonassalmanac()
{
	CGetAlmanac frm;
	if(CheckConnect())	
	{
		frm.isGlonass = 1;
		if(frm.DoModal()==IDOK)
		{
			m_almanac_filename = frm.fileName;
			m_almanac_no = frm.sv;
			::AfxBeginThread(GetGlonassAlmanacThread, 0);
		}else
		{
			SetMode();  
			CreateGPSThread();
		}
	}
}


void CGPSDlg::SetGlonassAlmanac(U08 continues)	
{	
	U16 SVID;	

	U08 messages[100];  
	U08 msg[100];
	BYTE  buffer[0x1000];
	ULONGLONG dwBytesRemaining = m_ephmsFile.GetLength();


	if(dwBytesRemaining == 0)
	{
		m_ephmsFile.Close();	
		if(!continues)
		{
		SetMode(); 
		CreateGPSThread();
		}	
		return;
	}

	m_ephmsFile.Read(buffer,1);
	int one_entry_size = 24;

	//if(dwBytesRemaining == one_entry_size || dwBytesRemaining == 24 * one_entry_size)
	if(dwBytesRemaining == 24 * one_entry_size)
	{
		m_ephmsFile.SeekToBegin();

		while(1)
		{
			UINT nBytesRead = m_ephmsFile.Read(buffer,one_entry_size);	
			//TRACE("nBytesRead=%d\r\n",nBytesRead);
			if(nBytesRead<=0)
				break;
			memset(msg, 0, sizeof(msg));
			memset(messages, 0, sizeof(messages));
			msg[0] = 0x5E;

			memcpy(&msg[1],buffer,one_entry_size);
			SVID = buffer[0];	

			if(buffer[0] == 0) 
			{
				TRACE("SVID=%d,continue\r\n",SVID);
				continue;
			}

			if(continues)
			{
				msg[one_entry_size + 1] = 0;
			}
			else
			{
				msg[one_entry_size + 1] = (SVID == 24) ? 1 : 0;
            }

			int len = SetMessage2(messages, msg, one_entry_size + 2);

			sprintf_s(m_nmeaBuffer, sizeof(m_nmeaBuffer), "Set SV#%d Glonass Almanac Successful...",SVID);
			if(!SendToTargetNoWait(messages, len,m_nmeaBuffer))
			{
				sprintf_s(m_nmeaBuffer, sizeof(m_nmeaBuffer), "Set SV#%d Glonass Almanac Fail...",SVID);
				add_msgtolist(m_nmeaBuffer);
			}

			//Sleep(10);
		}
	}
	else
	{
		AfxMessageBox("The Glonass Almanac data Format of the file is wrong");
		//return;
	}	
	m_ephmsFile.Close();	
	if(!continues)
	{
	SetMode(); 
	CreateGPSThread();
	}	
}

UINT SetGlonassAlmanacThread(LPVOID pParam)
{	
	CGPSDlg::gpsDlg->SetGlonassAlmanac(FALSE);	
	//AfxEndThread(0);
	return 0;
}

void CGPSDlg::OnEphemerisSetglonassalmanac()
{
	if(!CheckConnect())return;
	m_inputMode  = 0;
	CString fileName("Glonass_Almanac.log");	
	CFileDialog dlgFile(true, _T("log"), fileName, OFN_HIDEREADONLY, _T("Almanac Files (*.log)|*.log||"), this);
	INT_PTR nResult = dlgFile.DoModal();
	fileName = dlgFile.GetPathName();	
	CFileException ef;
	try
	{
		if(nResult == IDOK)
		{  				
			if(!m_ephmsFile.Open(fileName,CFile::modeRead,&ef))
			{
				ef.ReportError();
				SetMode();
				CreateGPSThread();
				return;
			}				
			AfxBeginThread(SetGlonassAlmanacThread,0);			
		}		
		else
		{
			SetMode();
			CreateGPSThread();
		}
	}
	catch(CFileException *fe)
	{
		fe->ReportError();
		fe->Delete();
		return;
	}	
	fileName.ReleaseBuffer();		
}

void CGPSDlg::OnEphemerisGetgpsglonass()
{
	if(!CheckConnect())return;
	m_inputMode  = 0;	
	CGetGNSSEphemeris* dlg = new CGetGNSSEphemeris();
	
	INT_PTR nResult = dlg->DoModal();
	if(nResult != IDOK)
	{
		SetMode();  CreateGPSThread();
	}
	delete dlg;	dlg= NULL;
}

void CGPSDlg::OnEphemerisSetgpsglonass()
{
	if(!CheckConnect())return;
	m_inputMode  = 0;	
	CSetGNSSEphemeris* dlg = new CSetGNSSEphemeris();

	INT_PTR nResult = dlg->DoModal();
	if(nResult != IDOK)
	{
		SetMode();  CreateGPSThread();
	}
	delete dlg;	dlg= NULL;
}

void CGPSDlg::OnEphemerisGetgpsglonassalmanac()
{
	if(!CheckConnect())return;
	m_inputMode  = 0;	
	CGetGNSSEphemeris* dlg = new CGetGNSSEphemeris();
	dlg->isAlmanac = TRUE;
	INT_PTR nResult = dlg->DoModal();
	if(nResult != IDOK)
	{
		SetMode();  CreateGPSThread();
	}
	delete dlg;	dlg= NULL;
}

void CGPSDlg::OnEphemerisSetgpsglonassAlmanac()
{
	if(!CheckConnect())return;
	m_inputMode  = 0;	
	CSetGNSSEphemeris* dlg = new CSetGNSSEphemeris();
	dlg->isAlmanac = TRUE;
	INT_PTR nResult = dlg->DoModal();
	if(nResult != IDOK)
	{
		SetMode();  
		CreateGPSThread();
	}
	delete dlg;	dlg= NULL;
}

void CGPSDlg::GetTimeCorrection(CString m_filename)
{
	int wait = 0;
	U08 msg[1] ;
	int res_len;
	int len;
	FILE *f = NULL;
	U08 buff[256];

	msg[0] = 0x5F;

	len = SetMessage(msg,sizeof(msg));

	//WaitEvent();
	ClearQue();
	if(SendToTarget(m_inputMsg,len,"Get Time Correction Successful.") == 1)
	{
		memset(buff, 0, sizeof(buff));
		res_len = m_serial->GetBinary(buff, sizeof(buff));

		if(buff[4]==0x92)
		{
			fopen_s(&f, m_filename, "wb+");
			fwrite(&buff[5],1,8,f);
			fclose(f);
		}
	}
	SetMode();  
	CreateGPSThread();
}

CString m_time_correction_filename;
UINT GetTimeCorrectionThread(LPVOID param)
{
	CGPSDlg::gpsDlg->GetTimeCorrection(m_time_correction_filename);
	return 0;
}

void CGPSDlg::OnEphemerisGettimecorrections()
{
	if(!CheckConnect())return;

	CGetTimeCorrection dlg;

	if(dlg.DoModal() == IDOK)
	{
		m_time_correction_filename = dlg._filename;
		::AfxBeginThread(GetTimeCorrectionThread, 0);
	}else
	{
		SetMode();  CreateGPSThread();
	}

}

int m_time_correction_attr;
void CGPSDlg::SetTimeCorrection(CString m_filename)
{
	int wait = 0;
	U08 msg[10] ;
//	int res_len;
	int len;
	FILE *f = NULL;
//	U08 buff[256];

	msg[0] = 0x60;
	fopen_s(&f, m_filename, "rb");
	fread(&msg[1],1,8,f);
	fclose(f);
	msg[9] = m_time_correction_attr;

	len = SetMessage(msg,sizeof(msg));

	//WaitEvent();
	ClearQue();
	if(SendToTarget(m_inputMsg,len,"Set Time Correction Successful.") != 1)
	{
		sprintf_s(m_nmeaBuffer, sizeof(m_nmeaBuffer), "Set Time Correction Fail.");
		add_msgtolist(m_nmeaBuffer);
			
	}

	SetMode();  
	CreateGPSThread();
}

UINT SetTimeCorrectionThread(LPVOID param)
{
	CGPSDlg::gpsDlg->SetTimeCorrection(m_time_correction_filename);
	return 0;
}

void CGPSDlg::OnEphemerisSettimecorrections()
{
	if(!CheckConnect())
	{
		return;
	}

	m_inputMode = 0;
	CString fileName/* = m_lastBdEphFile*/;
	if(m_lastBdEphFile.IsEmpty())
	{	
		fileName = "GlonassTimeCorrections.log";	
	}

	CFileDialog dlgFile(TRUE, _T("log"), fileName, 
		OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY, 
		_T("Log Files (*.log)|*.log||"), this);

	if(dlgFile.DoModal() != IDOK)
	{
		SetMode();
		CreateGPSThread();
		return;
	}

	m_time_correction_filename = dlgFile.GetPathName();
	m_time_correction_attr = 0;
	::AfxBeginThread(SetTimeCorrectionThread, 0);
}

int m_1pps_output_mode;
int m_1pps_output_mode_attr;
int m_1pps_output_align;

UINT config_1PPS_output_mode_thread(LPVOID param)
{
	U08 msg[4] ;

	msg[0] = 0x55;
	msg[1] = m_1pps_output_mode;
	msg[2] = m_1pps_output_align;
	msg[3] = m_1pps_output_mode_attr;

	int len = CGPSDlg::gpsDlg->SetMessage(msg,sizeof(msg));
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(CGPSDlg::m_inputMsg, len, "Configure PPS Output Mode Successful...");
	return 0;	
} 

void CGPSDlg::On1ppstimingConfigureppsoutputmode()
{
	CCon1PPS_OutputMode dlg;

	if(CheckConnect())	
	{
		if(dlg.DoModal()==IDOK)
		{
			m_1pps_output_mode = dlg.mode;
			m_1pps_output_align =dlg.align_to;
			m_1pps_output_mode_attr = dlg.attr;
			::AfxBeginThread(config_1PPS_output_mode_thread, 0);
		}
		else
		{
			SetMode();  
			CreateGPSThread();
		}
	}	
}

UINT query_1PPS_output_mode_thread(LPVOID param)
{
	U08 buff[100];
	U08 msg[1] ;
	CString tmp;
	time_t start,end;
	int res;

	msg[0] = 0x56;

	int len = CGPSDlg::gpsDlg->SetMessage(msg,sizeof(msg));

	CGPSDlg::gpsDlg->ClearQue();

	if(CGPSDlg::gpsDlg->SendToTarget(CGPSDlg::m_inputMsg,len,"Query 1PPS Output Successful..."))
	{

		start = clock();
		while(1)
		{
			memset(buff, 0, 100);
			res = CGPSDlg::gpsDlg->m_serial->GetBinary(buff, sizeof(buff));	

			if(ReadOK(res) && BINARY_HD1==buff[0] && BINARY_HD2==buff[1])
			{
				if(Cal_Checksum(buff) == 0xC3)
				{
					if(TIMING_MODE)
					{
					if(buff[5] == 0)
						CGPSDlg::gpsDlg->add_msgtolist("PPS No Output");
					else if(buff[5] == 1)
						CGPSDlg::gpsDlg->add_msgtolist("PPS Output if GPS/UTC time is available");
					else if(buff[5] == 2)
					{
						CGPSDlg::gpsDlg->add_msgtolist("PPS Output always and align to GPS/UTC time");
						CGPSDlg::gpsDlg->add_msgtolist("automatically");
					}
                    if(buff[6]==0)
						CGPSDlg::gpsDlg->add_msgtolist("Align to GPS time");
					else if(buff[6] == 1)
						CGPSDlg::gpsDlg->add_msgtolist("Align to UTC time");
					}
					else
					{
					if(buff[5] == 0)
						CGPSDlg::gpsDlg->add_msgtolist("PPS always output");
					else if(buff[5] == 1)
						CGPSDlg::gpsDlg->add_msgtolist("PPS output after 3D fixed");
					}
					break;
				}
			}
			end=clock();	
			if(CGPSDlg::gpsDlg->TIMEOUT_METHOD(start,end))
				break;
		}
	}

	CGPSDlg::gpsDlg->SetMode();  
	CGPSDlg::gpsDlg->CreateGPSThread();
	return 0;	
}

void CGPSDlg::On1ppstimingQueryppsoutputmode()
{
	if(CheckConnect())	
	{
		::AfxBeginThread(query_1PPS_output_mode_thread, 0);
	}
}

U08 CGPSDlg::parse_psti_others(const char *buff, int psti_id)
{
	const char *ptr = buff;
	U08 retVal=TRUE;
	char comma[100];
	U08 comma_number;
//	U08 val;
	comma_number=comma_count(ptr, comma);
	switch (psti_id)
	{
	    case 0x3: //interference flag
		break;
	}

	return retVal;
}

void CGPSDlg::BoostBaudrate(BOOL bRestore, BoostMode mode, bool isForce)
{
	if(bRestore)
	{
		if((isForce || ChangeToTemp != mode) && (GetBaudrate() != m_nDownloadBaudIdx))
		{
			SetPort(GetBaudrate(), (int)mode);
			Sleep(1000);
		}
		CloseOpenUart();
		m_serial->ResetPort(GetBaudrate());
		m_BaudRateCombo.SetCurSel(GetBaudrate());
	}
	else
	{
		if(GetBaudrate() != m_nDownloadBaudIdx)
		{
			SetPort(m_nDownloadBaudIdx, (int)mode);
			Sleep(1000);
		}
	}
}

void CGPSDlg::OnConverterCompress()
{
	if(m_isConnectOn)
	{
		TerminateGPSThread();
	}

	CCompressDlg dlg;
	dlg.DoModal();
}

void CGPSDlg::RescaleDialog()
{	
	const UiLocationEntry UiTable[] = 
	{
		//Left Panel
		TRUE, 0, IDC_COMPORT_T, {6, 7, 80, 18},
		TRUE, 0, IDC_BAUDRATE_T, {98, 7, 80, 18},
		TRUE, 0, IDC_COMPORT, {6, 28, 80, 13},
		TRUE, 0, IDC_BAUDRATE_IDX, {98, 28, 80, 13},
		(!NMEA_INPUT), 0, IDC_OPEN_CLOSE_T, {235, 7, 51, 18},
		(!NMEA_INPUT), 0, IDC_CONNECT, {254, 28, 32, 24},
		(!NMEA_INPUT), 0, IDC_CLOSE, {254, 28, 32, 24},
		NMEA_INPUT, 0, IDC_PLAY, {182, 28, 32, 24},
		NMEA_INPUT, 0, IDC_RECORD, {218, 28, 32, 24},
		NMEA_INPUT, 0, IDC_STOP, {254, 28, 32, 24},
		NMEA_INPUT, 0, IDC_PLAY_T, {180, 7, 32, 18},
		NMEA_INPUT, 0, IDC_RECORD_T, {216, 7, 32, 18},
		NMEA_INPUT, 0, IDC_STOP_T, {252, 7, 32, 18},

		TRUE, 0, IDC_MESSAGE_T, {6, 59, 280, 29},
		TRUE, 0, IDC_FIXED_STATUS, {142, 59, 144, 29},
		TRUE, 0, IDC_MESSAGE, {6, 89, 280, 158},

		TRUE, 0, IDC_RESPONSE_T, {6, 250, 280, 29},
		TRUE, 0, IDC_RESPONSE, {6, 280, 280, 116},

		TRUE, 0, IDC_COORDINATE_T, {6, 399, 280, 29},
		TRUE, 0, IDC_COORDINATE_F, {6, 429, 280, 132},
		TRUE, 0, IDC_WGS84_X_T, {22, 436, 110, 18},
		TRUE, 0, IDC_WGS84_X, {22, 454, 110, 18},
		TRUE, 0, IDC_WGS84_Y_T, {22, 477, 110, 18},
		TRUE, 0, IDC_WGS84_Y, {22, 495, 110, 18},
		TRUE, 0, IDC_WGS84_Z_T, {22, 518, 110, 18},
		TRUE, 0, IDC_WGS84_Z, {22, 536, 110, 18},
		TRUE, 0, IDC_ENU_E_T, {160, 436, 110, 18},
		TRUE, 0, IDC_ENU_E, {160, 454, 110, 18},
		TRUE, 0, IDC_ENU_N_T, {160, 477, 110, 18},
		TRUE, 0, IDC_ENU_N, {160, 495, 110, 18},
		TRUE, 0, IDC_ENU_U_T, {160, 518, 110, 18},
		TRUE, 0, IDC_ENU_U, {160, 536, 110, 18},

		TRUE, 0, IDC_COMMAND_T, {6, 564, 280, 29},
		TRUE, 0, IDC_COMMAND_F, {6, 594, 280, 90},

		TRUE, 0, IDC_HOTSTART, {15, 599, 82, 24},
		TRUE, 0, IDC_WARMSTART, {105, 599, 82, 24},
		TRUE, 0, IDC_COLDSTART, {195, 599, 82, 24},
		TRUE, 0, IDC_NO_OUTPUT, {15, 627, 82, 24},
		TRUE, 0, IDC_NMEA_OUTPUT, {105, 627, 82, 24},
		TRUE, 0, IDC_BIN_OUTPUT, {195, 627, 82, 24},
		TRUE, 0, IDC_SCANALL, {15, 655, 82, 24},
		TRUE, 0, IDC_SCANPORT, {105, 655, 82, 24},
		TRUE, 0, IDC_SCANBAUDRATE, {195, 655, 82, 24},

		//Earth View
		TRUE, 0, IDC_EARTH_PANEL, {301, 352, 343, 253},
		TRUE, 0, IDC_EARTH, {332, 353, 312, 251},

		//Panel Background
		TRUE, 0, IDC_INFO_PANEL, {301, 24, 698, 104},
		TRUE, 0, IDC_SCATTER_PANEL, {656, 352, 343, 253},
		TRUE, 0, IDC_DOWNLOAD_PANEL, {301, 629, 698, 55},

		//Title
		TRUE, 0, IDC_INFORMATION_T, {301, 6, 100, 19},
		TRUE, 0, IDC_EARTH_T, {301, 332, 106, 21},
		TRUE, 0, IDC_SCATTER_SNR_T, {656, 332, 106, 21},
		TRUE, 0, IDC_DOWNLOAD_T, {301, 611, 100, 19},

		//Panel Setting Button
		FALSE, 0, IDC_EARTHSETTING, {387, 334, 18, 18},
		FALSE, 0, IDC_SCATTERSETTING, {742, 334, 18, 18},

		//Information
		TRUE, 0, IDC_TTFF_T,		{320, 35, 100, 37},
		TRUE, 0, IDC_TTFF,			{325, 55,  94, 16},
		TRUE, 0, IDC_DATE_T,		{432, 35, 100, 37},
		TRUE, 0, IDC_DATE,			{437, 55,  94, 16},
		TRUE, 0, IDC_TIME_T,		{544, 35, 100, 37},
		TRUE, 0, IDC_TIME,			{549, 55,  94, 16},
		TRUE, 0, IDC_BOOT_STATUS_T,	{656, 35, 100, 37},
		TRUE, 0, IDC_BOOT_STATUS,	{661, 55,  94, 16},
		TRUE, 0, IDC_SW_VER_T,		{768, 35, 100, 37},
		TRUE, 0, IDC_SW_VER,		{773, 55,  94, 16},
		TRUE, 0, IDC_SW_REV_T,		{880, 35, 100, 37},
		TRUE, 0, IDC_SW_REV,		{885, 55,  94, 16},

		TRUE, 0, IDC_LONGITUDE_T,	{320, 80, 100, 37},
		TRUE, 0, IDC_LONGITUDE,		{325, 100, 94, 16},
		TRUE, 0, IDC_LATITUDE_T,	{432, 80, 100, 37},
		TRUE, 0, IDC_LATITUDE,		{437, 100, 94, 16},
		TRUE, 0, IDC_ALTITUDE_T,	{544, 80, 100, 37},
		TRUE, 0, IDC_ALTITUDE,		{549, 100, 94, 16},
		TRUE, 0, IDC_DIRECTION_T,	{656, 80, 100, 37},
		TRUE, 0, IDC_DIRECTION,		{661, 100, 94, 16},
		TRUE, 0, IDC_SPEED_T,		{768, 80, 100, 37},
		TRUE, 0, IDC_SPEED,			{773, 100, 94, 16},
		TRUE, 0, IDC_PDOP_T,		{880, 80, 100, 37},
		TRUE, 0, IDC_PDOP,			{885, 100, 94, 16},

		//Optional Information
		SHOW_CLOCK_OFFSET, 0, IDC_CLOCK_T, {680, 112, 102, 19},
		SHOW_CLOCK_OFFSET, 0, IDC_CLOCK, {680, 132, 102, 24},
		SHOW_NOISE, 0, IDC_NOISE_T, {680, 112, 102, 19},
		SHOW_NOISE, 0, IDC_NOISE, {680, 132, 102, 24},

		//Glonass Knum
		FALSE, 0, IDC_KNUM_ENABLE, {699, 113, 82, 22},
		FALSE, 0, IDC_KNUM_DISABLE, {699, 137, 82, 22},
		FALSE, 0, IDC_KNUM_LIST, {794, 0, 182, 170},

		//SNR Chart Title
		TRUE, 0, IDC_GPS_SNR_T, {301, 135, 180, 18},
		TRUE, 0, IDC_BEIDOU_SNR_T, {301, 234, 180, 18},
		TRUE, 0, IDC_GA_SNR_T, {656, 234, 180, 18},

		//SNR Chart
		TRUE, 0, IDC_GPS_BAR, {301, 153, 698, 75},
		TRUE, 0, IDC_BD_BAR, {301, 252, 343, 75},
		TRUE, 0, IDC_GA_BAR, {656, 252, 343, 75},

		//Scatter
		//TRUE, 0, IDC_SCATTER, {668, 384, 220, 220},
		TRUE, 0, IDC_SCATTER, {658, 354, 256, 250},
		//TRUE, 0, IDC_SCATTER, {658, 354, 248, 248},

		TRUE, 0, IDC_2DRMS_T, {915, 457, 78, 18},
		TRUE, 0, IDC_2DRMS, {915, 476, 78, 18},
		TRUE, 0, IDC_CEP50_T, {915, 497, 78, 18},
		TRUE, 0, IDC_CEP50, {915, 516, 78, 18},
	
		TRUE, 0, IDC_ENUSCALE_T, {915, 358, 78, 18},
		TRUE, 0, IDC_ENUSCALE, {915, 377, 78, 24},
		TRUE, 0, IDC_MAPSCALE, {915, 377, 78, 24},
		TRUE, 0, IDC_COOR_T, {915, 406, 78, 18},
		TRUE, 0, IDC_COOR, {915, 425, 78, 18},

		TRUE, 0, IDC_SETORIGIN, {915, 540, 67, 24},
		TRUE, 0, IDC_CLEAR, {915, 568, 67, 24},

		//DR 
		SOFTWARE_FUNCTION & SW_FUN_DR, 0, IDC_ODO_METER_T, {848, 454, 80, 18},
		SOFTWARE_FUNCTION & SW_FUN_DR, 0, IDC_ODO_METER, {848, 472, 66, 24},
		SOFTWARE_FUNCTION & SW_FUN_DR, 0, IDC_GYRO_DATA_T, {848, 502, 80, 18},
		SOFTWARE_FUNCTION & SW_FUN_DR, 0, IDC_GYRO_DATA, {848, 520, 66, 24},
		SOFTWARE_FUNCTION & SW_FUN_DR, 0, IDC_BK_INDICATOR_T, {848, 550, 80, 18},
		SOFTWARE_FUNCTION & SW_FUN_DR, 0, IDC_BK_INDICATOR, {848, 568, 66, 24},

		//E-Compass Calibration
		ECOMPASS_CALIBRATION, 0, IDC_ECOM_CALIB, {852, 571, 80, 36},
		ECOMPASS_CALIBRATION, 0, IDC_ECOM_COUNTER, {852, 608, 80, 30},

		//Download
//		0, 0, IDC_DOWNLOAD_T, {300, 600, 146, 16},
		FIRMWARE_DOWNLOAD, 0, IDC_DOWNLOAD, {305, 645, 67, 27},
		FIRMWARE_DOWNLOAD && WITH_CONFIG_USB_BAUDRATE, 0, IDC_ROM_MODE, {300, 618, 146, 15},
		FIRMWARE_DOWNLOAD, 0, IDC_DL_BAUDRATE, {375, 646, 80, 18},

		FIRMWARE_DOWNLOAD, 0, IDC_BROWSE, {456, 646, 24, 24},
		FIRMWARE_DOWNLOAD, 0, IDC_FIRMWARE_PATH, {485, 650, 513, 33},

		0 && _V8_SUPPORT, 0, IDC_IN_LOADER, {300, 618, 146, 15},

		0, 0, 0, {0, 0, 0, 0}
	};

	CRect rcWin, rcClient, rcNewSize;
	CSize szClient(1008, (NMEA_INPUT) ? 614 : 690);

	GetWindowRect(rcWin);
	GetClientRect(rcClient);
	rcNewSize.SetRect(0, 0,
		szClient.cx + rcWin.Width() - rcClient.Width(),
		szClient.cy + rcWin.Height() - rcClient.Height());
	this->MoveWindow(rcNewSize);
	this->CenterWindow();

	const UiLocationEntry *p = UiTable;
	while(p->showOption || p->type || p->id || p->rect.bottom || p->rect.left || 
		  p->rect.right || p->rect.top)
	{

		CWnd *pWnd = this->GetDlgItem(p->id);
		CRect rcWnd(p->rect.left, p->rect.top, p->rect.left + p->rect.right, 
					p->rect.top + p->rect.bottom);

		if(p->showOption)
		{
			pWnd->MoveWindow(&rcWnd);
		}
		pWnd->ShowWindow((p->showOption) ? SW_SHOW : SW_HIDE);
		++p;
	}

	if(NMEA_INPUT)
	{
		GetDlgItem(IDC_HOTSTART)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_WARMSTART)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_COLDSTART)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_NO_OUTPUT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_NMEA_OUTPUT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BIN_OUTPUT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SCANALL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SCANPORT)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SCANBAUDRATE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_COMMAND_T)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_COMMAND_F)->ShowWindow(SW_HIDE);

		GetDlgItem(IDC_DOWNLOAD)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_ROM_MODE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_DL_BAUDRATE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BROWSE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_FIRMWARE_PATH)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_IN_LOADER)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_DOWNLOAD_PANEL)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_DOWNLOAD_T)->ShowWindow(SW_HIDE);
	}
}

void CGPSDlg::SwitchToConnectedStatus(BOOL bSwitch)
{
	if(NMEA_INPUT)
	{
		//m_PlayBtn.ShowWindow((bSwitch) ? SW_HIDE : SW_SHOW);	
		//m_StopBtn.ShowWindow((bSwitch) ? SW_SHOW : SW_HIDE);
	}
	else
	{
		m_ConnectBtn.ShowWindow((bSwitch) ? SW_HIDE : SW_SHOW);	
		m_CloseBtn.ShowWindow((bSwitch) ? SW_SHOW : SW_HIDE);
		m_CloseBtn.EnableWindow(TRUE);
	}
	m_BaudRateCombo.EnableWindow(!bSwitch);	
	m_ComPortCombo.EnableWindow(!bSwitch);
}

bool CGPSDlg::CheckTimeOut(DWORD duration, DWORD timeOut, bool silent)
{	
	if(duration > timeOut )
	{	
		if(!silent)
		{
			AfxMessageBox("Timeout: GPS device no response.");
		}
		return true;
	}
	return false;
}

void CGPSDlg::OnNmeaChecksumCalculator()
{
	NmeaChecksumCalDlg dlg;
	INT_PTR ret = dlg.DoModal();
}

void CGPSDlg::OnBinaryChecksumCalculator()
{
	BinaryChecksumCalDlg dlg;
	INT_PTR ret = dlg.DoModal();
}

void CGPSDlg::OnTestExternalSrec()
{
	if(!CheckConnect())
	{
		return;
	}

	CExternalSrecDlg dlg;
	INT_PTR ret = dlg.DoModal();

	SetMode();  
	CreateGPSThread();
}

void CGPSDlg::SetNmeaUpdated(bool b)
{
//	if(!m_isNmeaUpdated && b)
//	{
//		PostMessage(UWM_FIRST_NMEA, 0, 0);
//	}
	m_isNmeaUpdated = b;
}

bool CGPSDlg::SetFirstDataIn(bool b)
{
	if(!m_firstDataIn && b)
	{
		PostMessage(UWM_FIRST_NMEA, 0, 0);
		m_firstDataIn = true;
		return true;
	}
	return false;
}

LRESULT CGPSDlg::OnFirstNmea(WPARAM wParam, LPARAM lParam)
{
	if(wParam)
	{
		SetTimer(DELAY_QUERY_TIMER, (UINT)wParam, NULL);
		//Sleep((DWORD)wParam);
	}
	else
	{

		GetGPSStatus();

	}
	DeleteNmeaMemery();	
	ClearInformation();
	g_scatterData.ClearData();
	m_ttffCount = 0;
	m_initTtff = false;
	SetTTFF(0);
	ClearGlonass();

	if(lParam!=0 && IS_DEBUG)
	{
		CString strMSg;
		strMSg.Format(_T("Reboot detected in %s"), CTime::GetCurrentTime().Format("%Y-%m-%d %H:%M:%S"));
		add_msgtolist(strMSg);
	}
	return 0;
}

LRESULT CGPSDlg::OnShowTime(WPARAM wParam, LPARAM lParam)
{
	ShowTime();
	return 0;
}

LRESULT CGPSDlg::OnUpdateUI(WPARAM wParam, LPARAM lParam)
{
	/*
	memcpy(&m_gpgsaMsg, &m_gpgsaMsgCopy1, sizeof(GPGSA));
	memcpy(&m_glgsaMsg, &m_glgsaMsgCopy1, sizeof(GPGSA));
	memcpy(&m_bdgsaMsg, &m_bdgsaMsgCopy1, sizeof(GPGSA));

	memcpy(&m_gpgsvMsg, &m_gpgsvMsgCopy1, sizeof(GPGSV));
	memcpy(&m_glgsvMsg, &m_glgsvMsgCopy1, sizeof(GPGSV));
	memcpy(&m_bdgsvMsg, &m_bdgsvMsgCopy1, sizeof(GPGSV));

	memcpy(&sate_gps, &satecopy_gps, sizeof(Satellite) * MAX_SATELLITE);
	memcpy(&sate_gnss, &satecopy_gnss, sizeof(Satellite) * MAX_SATELLITE);
	memcpy(&sate_bd, &satecopy_bd, sizeof(Satellite) * MAX_SATELLITE);
*/
	gpsSnrBar->Invalidate(FALSE);
	//gnssSnrBar->Invalidate(FALSE);
	bdSnrBar->Invalidate(FALSE);
	gaSnrBar->Invalidate(FALSE);
	pic_earth->Invalidate(FALSE);
	return 0;
}

void CGPSDlg::GetGPSStatus()
{
	if(!g_setting.autoQueryVersion)
	{
		return;
	}
	if(m_nmeaPlayThread)
	{
		return;
	}
	QuerySoftwareVersionSystemCode(NoWait, NULL);
	Sleep(60);
	QueryGnssBootStatus(NoWait, NULL);
}

void CGPSDlg::SetConnectTitle(bool isInConnect)
{
	if(isInConnect)
	{
		m_connectT.SetWindowText("Connect");
	}
	else
	{
		m_connectT.SetWindowText("Close");
	}
}

void CGPSDlg::NmeaOutput(LPCTSTR pt, int len)
{
	//static char msg[1024];
	//memcpy(msg, pt, len);
	//msg[len] = 0;
	m_nmeaList.AddTextAsync(pt);
}
