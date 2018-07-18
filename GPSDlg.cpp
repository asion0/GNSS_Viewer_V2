// GPSDlg.cpp : implementation file
//
#include "stdafx.h"
#include "GPS.h"
#include "GPSDlg.h"

#include "LogFilterDlg.h"
#include "KmlDlg.h"
#include "log2nmea.h"
#include "skytraqkml.h"
#include "ConPinningParameter.h"
#include "ConMultiMode.h"
#include "Con_register.h"
#include "ConMultiPath.h"
#include "ConWaas.h"
#include "Cfg_binary_msg.h"
#include "Config_binary_interval.h"
#include "Con1PPS_DOP.h"
#include "Con1PPS_ElevCNR.h"
#include "Proprietary_nmea.h"
#include "Device_Adding.h"
#include "geoid.h"
#include "ConNMEAComport.h"
#include "Con_NMEA_TalkerID.h"
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
#include "GetRgsDlg.h"
#include "HostBaseDownloadDlg.h"
#include "FirmwareDownloadDlg.h"
#include "ParallelDownloadDlg.h"
#include "ExternalSrecDlg.h"
#include "Con1PPS_OutputMode.h"
#include "NmeaChecksumCalDlg.h"
#include "BinaryChecksumCalDlg.h"
#include "HostLogDlg.h"
#include "PanelBackground.h"
#include "RawMeasmentOutputConvertDlg.h"
#include "VerifyFwDlg.h"
#include "CommonConfigDlg.h"
#include "SaveBinaryNoParsingDlg.h"
#include "MessageParser.h"
#include "IoTester.h"

#include <Dbt.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static CFile playFile;
HANDLE hScanGPS;
CWinThread* g_gpsThread = NULL;
bool g_nmeaInputTerminate = false;

CGPSDlg* CGPSDlg::gpsDlg = NULL;
UINT CGPSDlg::UWM_PLAYNMEA_EVENT = 0;
UINT CGPSDlg::UWM_SAVENMEA_EVENT = 0;
UINT CGPSDlg::UWM_UPDATE_EVENT = 0;
U08 CGPSDlg::m_inputMsg[200] = { 0 };

//UI Fonts
CFont CGPSDlg::m_textFont;
CFont CGPSDlg::m_infoFontS;
CFont CGPSDlg::m_infoFontM;
CFont CGPSDlg::m_infoFontL;
CFont CGPSDlg::comboFont;;
CFont CGPSDlg::messageFont;

void add2message(const char* buffer, int offset)
{
	static char msg[1024] = { 0 };
	memcpy(msg, buffer, offset);
	msg[offset] = 0;
	CGPSDlg::gpsDlg->m_nmeaList.AddTextAsync(msg);
}

void SetListCtrlRedraw(BOOL b) 
{ 
  CGPSDlg::gpsDlg->m_nmeaList.SetRedraw(b); 
}

void U32toBuff(U08 *buf, U32 v)
{
	buf[0] = (U08)(v>>24) &0xff;
	buf[1] = (U08)(v>>16) &0xff;
	buf[2] = (U08)(v>>8)  &0xff;
	buf[3] = (U08)(v)     &0xff;
}

void CGPSDlg::DeleteNmeaMemery()
{
	memset(&m_gpgllMsg, 0, sizeof(GPGLL));
	memset(&m_gpgllMsgCopy1, 0, sizeof(GPGLL));
	memset(&m_gpgllMsgCopy, 0, sizeof(GPGLL));

	memset(&m_gpgsaMsg, 0, sizeof(GPGSA));
	memset(&m_glgsaMsg, 0, sizeof(GPGSA));
	memset(&m_bdgsaMsg, 0, sizeof(GPGSA));
	memset(&m_gagsaMsg, 0, sizeof(GPGSA));
	memset(&m_gigsaMsg, 0, sizeof(GPGSA));

	memset(&m_gpgsaMsgCopy1, 0, sizeof(GPGSA));
	memset(&m_glgsaMsgCopy1, 0, sizeof(GPGSA));
	memset(&m_bdgsaMsgCopy1, 0, sizeof(GPGSA));
	memset(&m_gagsaMsgCopy1, 0, sizeof(GPGSA));
	memset(&m_gigsaMsgCopy1, 0, sizeof(GPGSA));

	memset(&m_gpgsaMsgCopy, 0, sizeof(GPGSA));
	memset(&m_glgsaMsgCopy, 0, sizeof(GPGSA));
	memset(&m_bdgsaMsgCopy, 0, sizeof(GPGSA));
	memset(&m_gagsaMsgCopy, 0, sizeof(GPGSA));
	memset(&m_gigsaMsgCopy, 0, sizeof(GPGSA));

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

#if(MODULE_SUP_800)
	memset(&m_psti004001, 0, sizeof(PSTI004001));
	memset(&m_psti004001Copy1, 0, sizeof(PSTI004001));
	memset(&m_psti004001Copy, 0, sizeof(PSTI004001));
#endif

	memset(&m_gpgsvMsg, 0, sizeof(GPGSV));
	memset(&m_glgsvMsg, 0, sizeof(GPGSV));
	memset(&m_bdgsvMsg, 0, sizeof(GPGSV));
	memset(&m_gagsvMsg, 0, sizeof(GPGSV));
	memset(&m_gigsvMsg, 0, sizeof(GPGSV));

	memset(&m_gpgsvMsgCopy1, 0, sizeof(GPGSV));
	memset(&m_glgsvMsgCopy1, 0, sizeof(GPGSV));
	memset(&m_bdgsvMsgCopy1, 0, sizeof(GPGSV));
	memset(&m_gagsvMsgCopy1, 0, sizeof(GPGSV));
	memset(&m_gigsvMsgCopy1, 0, sizeof(GPGSV));

	memset(&m_gpgsvMsgCopy, 0, sizeof(GPGSV));
	memset(&m_glgsvMsgCopy, 0, sizeof(GPGSV));
	memset(&m_bdgsvMsgCopy, 0, sizeof(GPGSV));
	memset(&m_gagsvMsgCopy, 0, sizeof(GPGSV));
	memset(&m_gigsvMsgCopy, 0, sizeof(GPGSV));

	nmea.ClearSatellites();

	memset(&satecopy_gp, 0, sizeof(satecopy_gp));
	memset(&satecopy_gl, 0, sizeof(satecopy_gl));
	memset(&satecopy_bd, 0, sizeof(satecopy_bd));
	memset(&satecopy_ga, 0, sizeof(satecopy_ga));
	memset(&satecopy_gi, 0, sizeof(satecopy_gi));

	csSatelliteStruct.Lock();
	sate_gp.Clear();
  sate_gl.Clear();
  sate_bd.Clear();
  sate_ga.Clear();

	csSatelliteStruct.Unlock();
	ClearGlonass();

	m_versionInfo.Free();
	m_bootInfo.Free();
}

void CGPSDlg::Copy_NMEA_Memery()
{
	static bool copygsv_gp = false;
	static bool copygsv_gl = false;
	static bool copygsv_bd = false;
	static bool copygsv_ga = false;
	static bool copygsv_gi = false;

	memcpy(&m_gpgllMsgCopy1, &m_gpgllMsgCopy, sizeof(GPGLL));
	memcpy(&m_gpgsaMsgCopy1, &m_gpgsaMsgCopy, sizeof(GPGSA));
	memcpy(&m_glgsaMsgCopy1, &m_glgsaMsgCopy, sizeof(GPGSA));
	memcpy(&m_bdgsaMsgCopy1, &m_bdgsaMsgCopy, sizeof(GPGSA));
	memcpy(&m_gagsaMsgCopy1, &m_gagsaMsgCopy, sizeof(GPGSA));
	memcpy(&m_gigsaMsgCopy1, &m_gigsaMsgCopy, sizeof(GPGSA));

	memcpy(&m_gpggaMsgCopy1, &m_gpggaMsgCopy, sizeof(GPGGA));

	memcpy(&m_gpgsvMsgCopy1, &m_gpgsvMsgCopy, sizeof(GPGSV));
	memcpy(&m_glgsvMsgCopy1, &m_glgsvMsgCopy, sizeof(GPGSV));
	memcpy(&m_bdgsvMsgCopy1, &m_bdgsvMsgCopy, sizeof(GPGSV));
	memcpy(&m_gagsvMsgCopy1, &m_gagsvMsgCopy, sizeof(GPGSV));
	memcpy(&m_gigsvMsgCopy1, &m_gigsvMsgCopy, sizeof(GPGSV));

	memcpy(&m_gpzdaMsgCopy1, &m_gpzdaMsgCopy, sizeof(GPZDA));
	memcpy(&m_gprmcMsgCopy1, &m_gprmcMsgCopy, sizeof(GPRMC));
	memcpy(&m_gpvtgMsgCopy1, &m_gpvtgMsgCopy, sizeof(GPVTG));

#if(MODULE_SUP_800)
	memcpy(&m_psti004001Copy1, &m_psti004001Copy, sizeof(PSTI004001));
#endif

	if(m_gpgsvMsgCopy.NumOfMessage && m_gpgsvMsgCopy.NumOfMessage == m_gpgsvMsgCopy.SequenceNum)
	{
		if(!copygsv_gp || m_gpgsvMsgCopy.NumOfMessage == m_gpgsvMsgCopy.SequenceNum)
		{
      satecopy_gp = nmea.satellites_gp;
		  copygsv_gp = true;
      m_gpgsvMsgCopy.NumOfMessage = 0;
		}
		else
		{
			copygsv_gp = false;
		}
	}
	else
	{
		copygsv_gp = false;
	}

	if(m_glgsvMsgCopy.NumOfMessage && m_glgsvMsgCopy.NumOfMessage == m_glgsvMsgCopy.SequenceNum)
	{
		if(!copygsv_gl || m_glgsvMsgCopy.NumOfMessage == m_glgsvMsgCopy.SequenceNum)
		{
      satecopy_gl = nmea.satellites_gl;
			copygsv_gl = true;
      m_glgsvMsgCopy.NumOfMessage = 0;
		}
		else
		{
			copygsv_gl = false;
		}
	}
	else
	{
		copygsv_gl = false;
	}

	if(m_bdgsvMsgCopy.NumOfMessage && m_bdgsvMsgCopy.NumOfMessage == m_bdgsvMsgCopy.SequenceNum)
	{
		if(!copygsv_bd || m_bdgsvMsgCopy.NumOfMessage == m_bdgsvMsgCopy.SequenceNum)
		{
      satecopy_bd = nmea.satellites_bd;
		  copygsv_bd = true;
      m_bdgsvMsgCopy.NumOfMessage = 0;
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
      satecopy_ga = nmea.satellites_ga;
			copygsv_ga = true;
      m_gagsvMsgCopy.NumOfMessage = 0;
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

	if(m_gigsvMsgCopy.NumOfMessage && m_gigsvMsgCopy.NumOfMessage == m_gigsvMsgCopy.SequenceNum)
	{
		if(!copygsv_gi || m_gigsvMsgCopy.NumOfMessage == m_gigsvMsgCopy.SequenceNum)
		{
      satecopy_gi = nmea.satellites_gi;
			copygsv_gi = true;
      m_gigsvMsgCopy.NumOfMessage = 0;
		}
		else
		{
			copygsv_gi = false;
		}
	}
	else
	{
		copygsv_gi = false;
	}
  
  csSatelliteStruct.Lock();
	if(copygsv_gp)
	{
    sate_gp = satecopy_gp;
	}

	if(copygsv_gl)
	{
    sate_gl = satecopy_gl;
	}
	if(copygsv_bd)
	{
    sate_bd = satecopy_bd;
	}

	if(copygsv_ga)
	{
    sate_ga = satecopy_ga;
	}

	if(copygsv_gi)
	{
    sate_gi = satecopy_gi;
	}

	csSatelliteStruct.Unlock();

	if(copygsv_gp || copygsv_gl || copygsv_bd || copygsv_ga || copygsv_gi)
	{
		CopyNmeaToUse();
    if(NeedUpdate())
    {
		  PostMessage(UWM_UPDATE_UI, 0, 0);
    }
	}
}

void CGPSDlg::CopyNmeaToUse()
{
	memcpy(&m_gpgllMsg, &m_gpgllMsgCopy1, sizeof(GPGLL));

	memcpy(&m_gpggaMsg, &m_gpggaMsgCopy1, sizeof(GPGGA));
	memcpy(&m_gpzdaMsg, &m_gpzdaMsgCopy1, sizeof(GPZDA));
	memcpy(&m_gprmcMsg, &m_gprmcMsgCopy1, sizeof(GPRMC));
	memcpy(&m_gpvtgMsg, &m_gpvtgMsgCopy1, sizeof(GPVTG));
#if(MODULE_SUP_800)
	memcpy(&m_psti004001, &m_psti004001Copy1, sizeof(PSTI004001));
#endif
	memcpy(&m_gpgsaMsg, &m_gpgsaMsgCopy1, sizeof(GPGSA));
	memcpy(&m_glgsaMsg, &m_glgsaMsgCopy1, sizeof(GPGSA));
	memcpy(&m_bdgsaMsg, &m_bdgsaMsgCopy1, sizeof(GPGSA));
	memcpy(&m_gagsaMsg, &m_gagsaMsgCopy1, sizeof(GPGSA));
	memcpy(&m_gigsaMsg, &m_gigsaMsgCopy1, sizeof(GPGSA));

	memcpy(&m_gpgsvMsg, &m_gpgsvMsgCopy1, sizeof(GPGSV));
	memcpy(&m_glgsvMsg, &m_glgsvMsgCopy1, sizeof(GPGSV));
	memcpy(&m_bdgsvMsg, &m_bdgsvMsgCopy1, sizeof(GPGSV));
	memcpy(&m_gagsvMsg, &m_gagsvMsgCopy1, sizeof(GPGSV));
	memcpy(&m_gigsvMsg, &m_gigsvMsgCopy1, sizeof(GPGSV));

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
	memset(&m_gagsaMsg, 0, sizeof(GPGSA));
	memset(&m_gigsaMsg, 0, sizeof(GPGSA));

	memset(&m_gpggaMsg, 0, sizeof(GPGGA));

	memset(&m_gpgsvMsg, 0, sizeof(GPGSV));
	memset(&m_glgsvMsg, 0, sizeof(GPGSV));
	memset(&m_bdgsvMsg, 0, sizeof(GPGSV));
	memset(&m_gagsvMsg, 0, sizeof(GPGSV));
	memset(&m_gigsvMsg, 0, sizeof(GPGSV));

	memset(&m_gpzdaMsg, 0, sizeof(GPZDA));
	memset(&m_gprmcMsg, 0, sizeof(GPRMC));
	memset(&m_gpvtgMsg, 0, sizeof(GPVTG));
#if(MODULE_SUP_800)
	memset(&m_psti004001, 0, sizeof(PSTI004001));
#endif
	csSatelliteStruct.Lock();
  sate_gp.Clear();
  sate_gl.Clear();
  sate_bd.Clear();
  sate_ga.Clear();
  sate_gi.Clear();
	csSatelliteStruct.Unlock();
}


bool CGPSDlg::NmeaProc(const char* buffer, int offset, NmeaType& nmeaType)
{
	offset = NMEA::TrimTail(buffer, offset);
	nmeaType = NMEA::MessageType(buffer, offset);
	NmeaOutput(buffer, offset);

	switch(nmeaType)
	{
	case MSG_GLL:
		nmea.ShowGPGLLmsg(m_gpgllMsgCopy, buffer, offset);
		break;
	case MSG_GLGSA:
		nmea.ShowGLGSAmsg(m_glgsaMsgCopy, buffer, offset);
		break;
	case MSG_GIGSA:
		nmea.ShowGIGSAmsg(m_gigsaMsgCopy, buffer, offset);
		break;
	case MSG_GNGSA:
		nmea.ShowGNGSAmsg(m_gpgsaMsgCopy, m_glgsaMsgCopy, m_bdgsaMsgCopy, m_gagsaMsgCopy, m_gigsaMsgCopy, buffer, offset);
		break;
	case MSG_BDGSA:
		nmea.ShowBDGSAmsg(m_bdgsaMsgCopy, buffer, offset);
		break;
	case MSG_GAGSA:
		nmea.ShowGAGSAmsg(m_gagsaMsgCopy, buffer, offset);
		break;
	case MSG_GPGSA:
		nmea.ShowGNGSAmsg(m_gpgsaMsgCopy, m_glgsaMsgCopy, m_bdgsaMsgCopy, m_gagsaMsgCopy, m_gigsaMsgCopy, buffer, offset);
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
		nmea.ShowGPGSVmsg2(m_gpgsvMsgCopy, m_glgsvMsgCopy, m_bdgsvMsgCopy, m_gagsvMsgCopy, m_gigsvMsgCopy, buffer, offset);
		break;	
#if(SUPPORT_BDL2_GSV2)
	case MSG_GPGSV2:
		nmea.ShowGPGSV2msg(m_gpgsvMsgCopy, buffer, offset);
		break;
#endif
  case MSG_RMC:
		nmea.ShowGPRMCmsg(m_gprmcMsgCopy, buffer, offset);
		PostMessage(UWM_SHOW_RMC_TIME, 0, 0);
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
#if(SUPPORT_BDL2_GSV2)
	case MSG_BDGSV2:
		//nmea.ShowBDGSV2msg(m_bdgsv2MsgCopy, buffer, offset);
		nmea.ShowBDGSV2msg(m_bdgsvMsgCopy, buffer, offset);
		break;
#endif
	case MSG_GAGSV:
		nmea.ShowGAGSVmsg(m_gagsvMsgCopy, buffer, offset);
		break;
	case MSG_GIGSV:
		nmea.ShowGIGSVmsg(m_gigsvMsgCopy, buffer, offset);
		break;
	case MSG_GNGSV:
		nmea.ShowGNGSVmsg(m_gpgsvMsgCopy, m_glgsvMsgCopy, m_bdgsvMsgCopy, m_gagsvMsgCopy, m_gigsvMsgCopy, buffer, offset);
		break;
	case MSG_REBOOT:
		this->DeleteNmeaMemery();
		break;
	case MSG_Unknown:
	case MSG_ERROR:
		return false;
	default:
		return false;
	}

	if(nmea.GetFirstGsaIn())
	{
		return false;
	}
	return true;
}

//void BinaryProc(U08* buffer, int len, CFile& f)
U08 CGPSDlg::BinaryProc(U08* buffer, int len, CFile* fo)
{
if(!m_isConnectOn)
	{
		return BINMSG_ERROR;
	}
	if(len <= 7)
	{
		return BINMSG_ERROR;
	}

	int packetLen = MAKEWORD(buffer[3], buffer[2]);
	if(packetLen + 7 > len)
	{
		return BINMSG_ERROR;
	}

	U08 msgType = Cal_Checksum(buffer);
	U08 id = buffer[5];
	U08 sid = buffer[6];
	CString strOutput;

  //For Liteon Debug Mode convertion
  if(msgType == 0x6A && id == 0xFF && sid == 0x01)
  {
      msgType = buffer[9];
      buffer += 5;
      len -= 5;
  }

	switch(msgType)
	{
	case 0xDC:  // MEAS_TIME�V Measurement time information (0xDC) (Periodic) 
    ShowMeasurementTime(buffer, !(fo == NULL), (fo == NULL) ? NULL : &strOutput);
		ShowTime();
		break;
	case 0xDD:  // RAW_MEAS�V Raw measurements from each channel (0xDD) (Periodic) 
		ShowMeasurementChannel(buffer, !(fo == NULL), (fo == NULL) ? NULL : &strOutput);
		break;
	case 0xDE:  // SV_CH_STATUS�V SV and channel status (0xDE) (Periodic) 
		ShowMeasurementSv(buffer, !(fo == NULL), (fo == NULL) ? NULL : &strOutput);
		break;
	case 0xDF:  // RCV_STATE�V Receiver navigation status (0xDF) (Periodic) 
		ShowReceiverNav(buffer, !(fo == NULL), (fo == NULL) ? NULL : &strOutput);
		break;
	case 0xE0:		// sub frame data
	case 0xE1:		// sub frame data
	case 0xE2:		// sub frame data
	case 0xE3:		// sub frame data
		ShowSubframe(buffer, !(fo == NULL), (fo == NULL) ? NULL : &strOutput);
		break;
	case 0xE4:		// sub frame data
    //20160913 Don't show SBAS subframe data, request from Ryan and Andrew
#if(IS_DEBUG)
		ShowSubframe(buffer, !(fo == NULL), (fo == NULL) ? NULL : &strOutput);
#endif
		break;
	case 0xE5:		// EXT_RAW_MEAS Extended Raw Measurement Data v.1 (0xE5) (Periodic)
		ExtRawMeas(buffer, !(fo == NULL), (fo == NULL) ? NULL : &strOutput);
    ShowTime();
		break;
	case BINMSG_ECEF_USER_PVT:		//0xA8
		ShowBinaryOutput(buffer, !(fo == NULL), (fo == NULL) ? NULL : &strOutput);
		ShowTime();
		break;	
	case 0x7A:		//Customize 0xA8
		if(id == 0x0B && sid == 0x80)
		{
			ShowDjiBinaryOutput(buffer, !(fo == NULL), (fo == NULL) ? NULL : &strOutput);
			ShowTime();
		}
		break;
	default:
    if(g_setting.checkNmeaError)
    {
		  add_msgtolist("Unknown: " + theApp.GetHexString(buffer, len));
    }
    msgType = BINMSG_ERROR;
		break;
	}
	return msgType;
}

//---------------------------------------------------------
static void transform_u08_ary_to_u32_ary(const U08 *dws_p, S32 n_byte, U32 *buf_p, S32* n_bits_p)
{
  S16 i;
  S16 n_word = (S16)(n_byte >> 2);
  S16 remain_n_bytes = n_byte & 0x3;

  // complete 4 bytes
  for( i = 0; i < n_word; ++i )
  {
    buf_p[i] = ( ((((U32)(dws_p[i*4+0]))<<24) & 0xff000000) |
        ((((U32)(dws_p[i*4+1]))<<16) & 0x00ff0000) |
        ((((U32)(dws_p[i*4+2]))<< 8) & 0x0000ff00) |
        ((((U32)(dws_p[i*4+3]))<< 0) & 0x000000ff)   );
    *n_bits_p += 32;
  }

  // remain byte : 0 ~ 3, only deal with 1 ~ 3
  if(remain_n_bytes > 0)
  {
    for(i = 1; i <= remain_n_bytes; ++i)
    {
      //U32 tmp = (U32)dws_p[n_word*4+i-1];
      buf_p[n_word] |= ((((U32)dws_p[n_word*4+i-1]) & 0xff) << (32 - 8*i));
      *n_bits_p += 8;
    }
  }
}

#define CRC24_POLY 0x1864cfbL
const U16 Crc24_Q_table[384] = // total = 768 ( bytes )
{
  0x0000,  0x0086,  0x4cfb,  0x8ad5,  0x0d0c,  0x99f6,  0x93e6,  0xe115,
  0xaa1a,  0x1933,  0xec9f,  0x7f17,  0xa181,  0x3927,  0xcdc2,  0x2b54,
  0x34ad,  0x18cf,  0x3267,  0xd8b4,  0x2b23,  0xb8b2,  0xd53e,  0xfe2e,
  0xc54e,  0x8943,  0x0272,  0x4f9b,  0x84c9,  0xd77f,  0x56a8,  0x68d0,
  0xe493,  0xdc7d,  0x655a,  0x319e,  0x64cf,  0xb0e2,  0x834b,  0xee1a,
  0xbd68,  0x5646,  0xf729,  0x5171,  0x65aa,  0x7dfc,  0x5cfb,  0xb0a7,
  0x0cd1,  0xe98a,  0x9d12,  0x8604,  0xe400,  0x481f,  0x9f37,  0x0819,
  0x7bf3,  0x15e2,  0x0593,  0xaefe,  0xad50,  0xd02b,  0x1c2b,  0x2785,
  0xdda1,  0xc926,  0x3eb6,  0x31b8,  0xfaca,  0xb463,  0x3c32,  0x2fc7,
  0xc99f,  0x604f,  0xd39b,  0x434a,  0x6dc5,  0x0696,  0x5a79,  0x81dc,
  0x357a,  0xd0ac,  0x8c56,  0xe077,  0x681e,  0x59ee,  0x52a2,  0xe2cb,
  0x5464,  0x87af,  0xfbf8,  0xb87d,  0xb443,  0x712d,  0xb5f7,  0x614e,
  0x19a3,  0xd29f,  0xef29,  0x9376,  0xdf15,  0x3a24,  0x8a45,  0x330c,
  0x09c8,  0x0090,  0x3e86,  0xdcc5,  0xb822,  0xeb3e,  0x6e10,  0x32f7,
  0xe6b4,  0xbb1d,  0x2bc4,  0x0aad,  0x88f1,  0xa111,  0x0727,  0x5dfc,
  0xdced,  0x5b5a,  0xa1a0,  0x5638,  0x56d0,  0x74ad,  0x4f0b,  0xbac9,
  0x4741,  0xc5de,  0xb743,  0x924c,  0x7d6c,  0x62fb,  0x2099,  0xf7b9,
  0x6f71,  0xf594,  0xee8a,  0x8368,  0xc678,  0x645f,  0x8ee2,  0x1375,
  0x1572,  0x3b93,  0x3ec0,  0x9fa7,  0x3619,  0xebcd,  0x8694,  0xda00,
  0xd821,  0x0c41,  0xd78a,  0x0d2c,  0xb4f3,  0x0232,  0xbff9,  0x3e26,
  0x0fb8,  0x6af4,  0x2715,  0xe3a1,  0x5918,  0xadc0,  0xee2b,  0x8c15,
  0xd03c,  0xb256,  0x7049,  0x5ae9,  0xbfdc,  0xa544,  0x43da,  0x53c5,
  0x96a8,  0xc90f,  0x5e4f,  0x43a5,  0x71bd,  0x8bf7,  0xf170,  0xfb68,
  0x867d,  0x247d,  0xe25b,  0x6a64,  0x1791,  0x688e,  0x67ee,  0xc29c,
  0x3347,  0xa4b5,  0x0b5f,  0xb992,  0xa93f,  0xde52,  0xa0a1,  0x4526,
  0xedbe,  0x2a74,  0x48ac,  0x38b3,  0x92c6,  0x9d14,  0x8a66,  0x1813,
  0x909e,  0x5f6b,  0x0120,  0x7c87,  0x6c87,  0x8bf5,  0x710d,  0xb98a,
  0xf609,  0x2d70,  0x45d6,  0x7cdc,  0x20fa,  0x90db,  0x65ef,  0xcce3,
  0xa337,  0xef3a,  0xc169,  0x763a,  0x5788,  0x14d1,  0xc4ef,  0xdd5d,
  0x195b,  0x11e2,  0xc46e,  0xf542,  0x220e,  0x4ebb,  0xf8c8,  0xf703,
  0x3f96,  0x4db9,  0xdab6,  0xb543,  0x4033,  0x0fbb,  0xac70,  0xac2a,
  0x3c57,  0x26a5,  0xa1a0,  0xe95a,  0x9e17,  0x7418,  0x5b8f,  0x14c2,
  0x7992,  0x8e82,  0x0df1,  0x958b,  0xbd6e,  0x8724,  0x9801,  0x6863,
  0xfad8,  0xc47c,  0x943f,  0x700d,  0xc9f6,  0x4132,  0x693e,  0x25ef,
  0x72de,  0xe3eb,  0x2865,  0xa7d3,  0x5b59,  0xfddd,  0x1506,  0xd18c,
  0xf057,  0xc00b,  0xc8bf,  0x1c4e,  0xf3e7,  0x426a,  0x11c4,  0x26ea,
  0x2ae4,  0x76ac,  0xa88d,  0xa031,  0x7b26,  0x7d80,  0xb902,  0x973f,
  0x4e6c,  0x33d7,  0x9ab5,  0x9b61,  0x8b65,  0x4f0d,  0x29b4,  0x01b0,
  0x4287,  0xfcb9,  0x1883,  0xae9e,  0xcf55,  0x9256,  0xa314,  0x1a58,
  0xefaa,  0xff69,  0xe604,  0x657f,  0xf2e3,  0x3309,  0x7c4c,  0x1efa,
  0x00e5,  0xf699,  0x1370,  0xd5e8,  0x4e2b,  0xc6c8,  0x673d,  0xc4fe,
  0xcb42,  0xb230,  0xddcd,  0x275b,  0x81dc,  0x5718,  0x2ad1,  0x54d1,
  0x2635,  0x9fa0,  0x7964,  0xace0,  0x922a,  0xac69,  0xb5d3,  0x7e33,
  0x9f85,  0x3f06,  0x73b9,  0x4a88,  0x87b4,  0xa601,  0xf85d,  0x0d61,
  0xab8b,  0x2d50,  0x1452,  0x4792,  0x1ebc,  0x9e87,  0x4a18,  0xcbb1,
  0xe37b,  0x1665,  0x37ed,  0x69ae,  0x1bef,  0xe2e0,  0x709d,  0xf7f6,
  0xd10c,  0xfa48,  0xfa7c,  0x0401,  0x42fa,  0x2fc4,  0xb6d4,  0xc82f,
  0x224e,  0x63d9,  0xd11c,  0xce57,  0x5035,  0x5bc9,  0xc3dd,  0x8538
};
//---------------------------------------------------------
// get_value_from_crc24q_table :
//
// Input : input - index value shifted out from register
// Output : corrosponding value gotten by index value from " U16 Crc24_Q_tab[384] "
//---------------------------------------------------------
static U32 get_value_from_crc24q_table(S16 input)
{
  U32 value = 0;
  S16 a_ndx = 0, b_ndx = 0, c_ndx = 0;

  //  a_ndx  v        0           1    <-- b_ndx
  //          ----------- -----------
  //         |   input   |   input   |
  //          ----------- -----------
  //                    ||
  //                    vv
  //          ------- ------- -------
  //         |  U16  |  U16  |  U16  |   U16 Crc24_Q_tab[384]
  //          ------- ------- -------
  //         ^
  //         c_ndx

  a_ndx = input/2;
  b_ndx = input - a_ndx*2;
  c_ndx = a_ndx*3;

  if( b_ndx == 0 )
    value = ( ((U32)Crc24_Q_table[c_ndx])<<8 ) | ( ((U32)Crc24_Q_table[c_ndx+1])>>8 );
  //value = ( ((U32)Crc24_Q_tab[c_ndx])<<8 ) | ( ((U32)Crc24_Q_tab[c_ndx+1])>>8 );

  if( b_ndx == 1 )
    value = ( ((U32)Crc24_Q_table[c_ndx+1])<<16 ) | ( ((U32)Crc24_Q_table[c_ndx+2])&0x0000ffff );
  //value = ( ((U32)Crc24_Q_tab[c_ndx+1])<<16 ) | ( ((U32)Crc24_Q_tab[c_ndx+2])&0x0000ffff );

  value &= 0x00ffffff;

  return value;
}

//---------------------------------------------------------
// crc24q_parity_calculate_by_tab : calculate data words parity ( use table driven algorithm )
//
// Input: dws_p - data words array, each word contains 32 bits
//        n_bits - total number of bits

// Return : crc 24Q parity (total 24bits which is put in the LSB of a 32bits word)
//---------------------------------------------------------
static U32 crc24q_parity_calculate_by_tab(const U32 *dws_p, S32 n_bits)
{
  S16 i, j;
  U32 value;                     // index value shifted out from register
  U32 reg = 0;                   // only LSB 24 bits

  S16 word_ndx = (S16)(n_bits >> 5);       // each word has 32 bits
  S16 char_ndx = (n_bits & 0x1f)>>3;  // char number of the last uncomplete word ( 0 ~ 3 )
  S16 bit_ndx = (n_bits & 0x07);      // bit number of the last uncomplete char ( 0 ~ 8)

  // step 1 : input complete words(32 bits) into register & calculate one byte at a time
  for(i = 0; i < word_ndx; ++i)
  {
    for(j = 3; j >= 0; --j)
    {
      value = 0;
      reg = ( reg<<8 ) | ( (dws_p[i]>>(j*8))&0x000000ffUL );
      value = ( reg >> 24 );

      if( value )
        reg ^= get_value_from_crc24q_table((S16)value);
    }
  }

  // step 2 : input complete chars(8 bits) into register & calculate one byte at a time
  for( j = 3 ; j >= ( 4 - char_ndx ) ; j-- )
  {
    value = 0;
    reg = ( reg<<8 ) | ( (dws_p[word_ndx]>>(j*8))&0x000000ffUL );
    value = ( reg >> 24 );

    if( value )
      reg ^= get_value_from_crc24q_table((S16)value);
  }

  // step 3 : input last uncompleted word into register, one bit at a time
  for( j = 7 ; j >= ( 8 - bit_ndx ) ; j-- )
  {
    reg <<= 1 ;

    if( ( (dws_p[word_ndx]>>((3-char_ndx)*8))>>j )&( BIT0 ) )
      reg ^= BIT0 ;

    if( reg & BIT24 )
      reg ^= CRC24_POLY;
  }

  // final :
  U32 crc = reg & 0x00ffffffUL;     // only LSB 24 bits
  return crc;
}

//---------------------------------------------------------
// CRC24Q_get_for_rtcm_3_x : get data words parity ( shift one bit once )
//
// Input: dws_p - data words array, each word contains 8 bits
//        n_bytes - total number of bytes
//
// Return : crc 24Q parity (total 24bits which is put in the LSB of a 32bits word)
//---------------------------------------------------------
U32 CRC24Q_get_for_rtcm_3_x(const U08 *dws_p, S32 n_byte)
{
  U32 buf[1024] = {0};
  S32 n_bits = 0;
  //assert( (n_byte >> 2) <= NELEMS( buf ) ); //if > 256, buf[] size need to be changed.

  transform_u08_ary_to_u32_ary(dws_p, n_byte, buf, &n_bits);

  U32 crc = crc24q_parity_calculate_by_tab( buf, n_bits ); //U32 crc = crc24q_parity_calculate( buf, n_bits );
  return crc;
}

U16 CGPSDlg::RtcmProc(U08* buffer, int len)
{
	int packetLen = MAKEWORD(buffer[2], buffer[1] & 0xFC);
	if(packetLen + 6 > len)
	{
		return FALSE;
	}

  U32 crc24 = MAKELONG(MAKEWORD(buffer[packetLen + 5], buffer[packetLen + 4]), MAKEWORD(buffer[packetLen + 3], 0));
  buffer[packetLen + 3] = 0;
  buffer[packetLen + 4] = 0;
  buffer[packetLen + 5] = 0;
  U32 crc24Check = CRC24Q_get_for_rtcm_3_x(buffer, packetLen + 6);
	if(crc24 != crc24Check)
	{
		return FALSE;
	}

	U16 msgType = (buffer[3] << 4) | (buffer[4] >> 4);
	switch(msgType)
	{
  case 1005:
    ShowRtcm1005(buffer);
		break;
  case 1077:
    ShowRtcm1077(buffer);
		break;
  case 1107:
    ShowRtcm1107(buffer);
		break;
  case 1117:
    ShowRtcm1117(buffer);
		break;
  case 1127:
    ShowRtcm1127(buffer);
		break;
  case 1087:
    ShowRtcm1087(buffer);
		break;
	default:
		break;
	}
	return msgType;
}

U16 CGPSDlg::UbloxProc(U08* buffer, int len, CFile* fo)
{
	int packetLen = MAKEWORD(buffer[4], buffer[5]);
	U16 ubClassId = MAKEWORD(buffer[3], buffer[2]);
	CString strOutput;

	switch(ubClassId)
	{
  case 0x0D01:
    ShowUbxTimTp(buffer, !(fo == NULL), (fo == NULL) ? NULL : &strOutput);
    ShowTime();
		break;
  case 0x0106:
    ShowUbxNavSol(buffer, !(fo == NULL), (fo == NULL) ? NULL : &strOutput);
    ShowTime();
		break;
  case 0x0130:
    ShowUbxNavSvInfo(buffer, !(fo == NULL), (fo == NULL) ? NULL : &strOutput);
		break;
  case 0x0103:
    ShowUbxNavSvStatus(buffer, !(fo == NULL), (fo == NULL) ? NULL : &strOutput);
		break;
  case 0x0102:
    ShowUbxNavPosllh(buffer, !(fo == NULL), (fo == NULL) ? NULL : &strOutput);
		break;
  case 0x0104:
    ShowUbxNavDop(buffer, !(fo == NULL), (fo == NULL) ? NULL : &strOutput);
		break;
  case 0x0112:
    ShowUbxNavVelned(buffer, !(fo == NULL), (fo == NULL) ? NULL : &strOutput);
		break;
	default:
		break;
	}
	if((fo != NULL) && (strOutput.GetLength() > 0))
	{
		fo->Write((LPCTSTR)strOutput, strOutput.GetLength());
	}

	return ubClassId;
}

UINT ConnectGPS(LPVOID pParam)
{
	UINT retCode = 0;
	DWORD success;
	while(WaitForSingleObjectEx(g_connectEvent, 10, false) != WAIT_OBJECT_0);
	success = SetThreadPriority(GetCurrentThread(), ABOVE_NORMAL_PRIORITY_CLASS);
#if(RTCM_NEW_PARSER)
	CGPSDlg::gpsDlg->ParsingMessage();
#else
	CGPSDlg::gpsDlg->MSG_PROC();
#endif
	success = SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL );

	g_gpsThread = NULL;
	if(!SetEvent(g_closeEvent))
	{
		DWORD error = GetLastError();
	}
	return retCode;
}

UINT NmeaPlayThread(LPVOID pParam)
{
  CFile f;
	CString ext = Utility::GetFileExt((LPCSTR)pParam).MakeUpper();

  if(FALSE == f.Open((LPCSTR)pParam, CFile::modeRead))
  {
    return 0;
  }

  ULONGLONG total = f.GetLength();
	bool hasGGA = false;
	bool hasRMC = false;
	bool needSleep = false;

	CGPSDlg::gpsDlg->SetTimer(SHOW_STATUS_TIMER, 1000, NULL);
	CGPSDlg::gpsDlg->m_isConnectOn = true;
	CGPSDlg::gpsDlg->SetInputMode(CGPSDlg::NmeaMessageMode);
	int lineCount = 0;
	g_nmeaInputTerminate = false;
	DWORD startTick = ::GetTickCount();
	DWORD currentTick = 0;
  
  const int NmeaBufferSize = COM_BUFFER_SIZE;
  static char nmeaBuff[NmeaBufferSize] = {0};
  int bufferSize = NmeaBufferSize;
	//while(fgets(nmeaBuff, sizeof(nmeaBuff), f))
	while(ReadOneLineInFile(f, nmeaBuff, NmeaBufferSize))
	{
		//long current = ftell(f);
    ULONGLONG current = f.GetPosition();
    bufferSize = NmeaBufferSize;
		if(!PreprocessInputLine((U08*)nmeaBuff, bufferSize))
    {
      continue;
    }

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
				}
				startTick = ::GetTickCount();
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

		CGPSDlg::gpsDlg->m_playNmea->SetLineCount(++lineCount, (LONG)current, (LONG)total);
		NmeaType nmeaType;
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
  f.Close();

	CGPSDlg::gpsDlg->m_isConnectOn = false;
	CGPSDlg::gpsDlg->KillTimer(SHOW_STATUS_TIMER);
	CGPSDlg::gpsDlg->SetInputMode(CGPSDlg::NoOutputMode);
	CGPSDlg::gpsDlg->SetNmeaUpdated(false);
	CGPSDlg::gpsDlg->m_nmeaPlayThread = NULL;
	CGPSDlg::gpsDlg->m_nmeaPlayPause = false;
	return 0;
}

DWORD ReadOneLine(void *buffer, DWORD bufferSize, CFile *f)
{	
	U08* bufferIter = (U08*)buffer;
	DWORD totalSize = 0;

	bool cmdHeaderCome = false;
	while(totalSize < bufferSize - 1)
	{ 

		DWORD dwBytesDoRead = 0;
		DWORD dwErrorFlags = 0;
		DWORD bytesinbuff = 1;
		while(bytesinbuff)
		{
			dwBytesDoRead = f->Read(bufferIter, 1);
			//BOOL bb = ReadFile(m_comDeviceHandle, bufferIter, 1, &dwBytesDoRead, 0);
			if(dwBytesDoRead == 0)
			{	//Read fail.
				DWORD dwErr = ::GetLastError();
				return READ_ERROR;
			}

			if(totalSize > 0)
			{	//not first char.
				if(!cmdHeaderCome && *bufferIter==0xa1 && *(bufferIter-1)==0xa0)
				{
					bufferIter -= totalSize;
					*bufferIter = 0xa0; 
					++bufferIter;
					*bufferIter = 0xa1; 
					++bufferIter;
					totalSize = 2;
					cmdHeaderCome = true;
					continue;
				}
				else if(*bufferIter==0x0a && *(bufferIter-1)==0x0d)
				{
					unsigned char *chk_ptr = bufferIter - totalSize;
					
					if (*chk_ptr == 0xa0)
					{
						int tmp_len = *(chk_ptr + 2);
						tmp_len = tmp_len << 8 | *(chk_ptr+3);
						if (totalSize == tmp_len + 6) 
						{
							*(bufferIter+1) = 0;
							return totalSize + 1;
						}
						cmdHeaderCome = false;
					}
					else
					{
						return totalSize;
					}
				}
			}
			++totalSize;
			if (totalSize >=  bufferSize - 1)
			{	//Check 
				*(bufferIter+1) = 0;
				break;
			}
				
			++bufferIter;
			--bytesinbuff;
		} //while(bytesinbuff)
	} //while(total < size - 1)
	return totalSize;
}

CTime GetPlayTime()
{
	int y = 0, m = 0, d = 0;
	if(CGPSDlg::gpsDlg->m_gpzdaMsg.Year && CGPSDlg::gpsDlg->m_gpzdaMsg.Month && CGPSDlg::gpsDlg->m_gpzdaMsg.Day)
	{
		y = CGPSDlg::gpsDlg->m_gpzdaMsg.Year;
		m = CGPSDlg::gpsDlg->m_gpzdaMsg.Month;
		d = CGPSDlg::gpsDlg->m_gpzdaMsg.Day;
	}
	else if(CGPSDlg::gpsDlg->m_gprmcMsg.Year && CGPSDlg::gpsDlg->m_gprmcMsg.Month && CGPSDlg::gpsDlg->m_gprmcMsg.Day)
	{
		y = CGPSDlg::gpsDlg->m_gprmcMsg.Year;
		m = CGPSDlg::gpsDlg->m_gprmcMsg.Month;
		d = CGPSDlg::gpsDlg->m_gprmcMsg.Day;
	}
	else
	{
		return CTime(2006, 6, 28, 12, 00, 00);
	}

	CTime rmcTime(y, m, d, CGPSDlg::gpsDlg->m_gprmcMsgCopy.Hour, CGPSDlg::gpsDlg->m_gprmcMsgCopy.Min, (int)(CGPSDlg::gpsDlg->m_gprmcMsgCopy.Sec));
	CTime rggaTime(y, m, d, CGPSDlg::gpsDlg->m_gpggaMsgCopy.Hour, CGPSDlg::gpsDlg->m_gpggaMsgCopy.Min, (int)(CGPSDlg::gpsDlg->m_gpggaMsgCopy.Sec));
	if(rmcTime > rggaTime)
	{
		return rmcTime;
	}
	else
	{
		return rggaTime;
	}
}

UINT BinaryPlayThread(LPVOID pParam)
{
	CFile f;
	CString ext = Utility::GetFileExt((LPCSTR)pParam).MakeUpper();
	if(!f.Open((LPCSTR)pParam, CFile::modeRead))
	{
		return 0;
	}

	DWORD total = (DWORD)f.GetLength();
	bool needSleep = false;
	const int BufferSize = 2 * 1024;
	static U08 nmeaBuff[BufferSize] = { 0 };
	CTime playTime(2000, 1, 1, 12, 0, 0);

	CGPSDlg::gpsDlg->SetTimer(SHOW_STATUS_TIMER, 1000, NULL);
	CGPSDlg::gpsDlg->m_isConnectOn = true;
	CGPSDlg::gpsDlg->SetInputMode(CGPSDlg::NmeaMessageMode);

	int lineCount = 0;
	g_nmeaInputTerminate = false;
	DWORD startTick = ::GetTickCount();
	DWORD currentTick = 0;
	DWORD length = ReadOneLine(nmeaBuff, sizeof(nmeaBuff), &f);

	while(length != READ_ERROR)
	{
		DWORD current = (DWORD)f.GetPosition();
		U08 type = CGPSDlg::gpsDlg->BinaryProc(nmeaBuff, length);
		if(BINMSG_ERROR != type)
		{
			CGPSDlg::gpsDlg->Copy_NMEA_Memery();
			CGPSDlg::gpsDlg->SetNmeaUpdated(true);
		}

		CTime t = GetPlayTime();
		if(t > playTime)
		{
			needSleep = true;
			playTime = t;
		}

		currentTick = ::GetTickCount();
		if(needSleep)
		{
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
				}
				startTick = ::GetTickCount();
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
		length = ReadOneLine(nmeaBuff, sizeof(nmeaBuff), &f);
	}
	f.Close();
	CGPSDlg::gpsDlg->m_isConnectOn = false;
	CGPSDlg::gpsDlg->KillTimer(SHOW_STATUS_TIMER);
	CGPSDlg::gpsDlg->SetInputMode(CGPSDlg::NoOutputMode);
	CGPSDlg::gpsDlg->SetNmeaUpdated(false);
	CGPSDlg::gpsDlg->m_nmeaPlayThread = NULL;
	CGPSDlg::gpsDlg->m_nmeaPlayPause = false;
	return 0;
}

UINT NewPlayThread(LPVOID pParam)
{
	//CFile f;
	CString ext = Utility::GetFileExt((LPCSTR)pParam).MakeUpper();
	if(!playFile.Open((LPCSTR)pParam, CFile::modeRead))
	{
		return 0;
	}
  
	CGPSDlg::gpsDlg->SetTimer(SHOW_STATUS_TIMER, 1000, NULL);
	CGPSDlg::gpsDlg->m_isConnectOn = true;
	CGPSDlg::gpsDlg->SetInputMode(CGPSDlg::NmeaMessageMode);

	g_nmeaInputTerminate = false;
  CGPSDlg::gpsDlg->ParsingMessage(TRUE);
	playFile.Close();

/*
	while(length != READ_ERROR)
	{
		DWORD current = (DWORD)f.GetPosition();
		U08 type = CGPSDlg::gpsDlg->BinaryProc(nmeaBuff, length);
		if(BINMSG_ERROR != type)
		{
			CGPSDlg::gpsDlg->Copy_NMEA_Memery();
			CGPSDlg::gpsDlg->SetNmeaUpdated(true);
		}

		CTime t = GetPlayTime();
		if(t > playTime)
		{
			needSleep = true;
			playTime = t;
		}

		currentTick = ::GetTickCount();
		if(needSleep)
		{
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
				}
				startTick = ::GetTickCount();
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
		length = ReadOneLine(nmeaBuff, sizeof(nmeaBuff), &f);
	}
	f.Close();
*/
	CGPSDlg::gpsDlg->m_isConnectOn = false;
	CGPSDlg::gpsDlg->KillTimer(SHOW_STATUS_TIMER);
	CGPSDlg::gpsDlg->SetInputMode(CGPSDlg::NoOutputMode);
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
	m_InfoTabStat = BasicInfo;
	m_hIcon = AfxGetApp()->LoadIcon(IDI_MAINFRAME);
	m_serial = NULL;
	m_bShowBinaryCmdData = FALSE;
	gaSnrBar = new CSnrBarChartGalileo;
	giSnrBar = new CSnrBarChartNavic;
#if(SUPPORT_BDL2_GSV2)
	gpsSnrBar = new CSnrBarChartDualL2;
	bdSnrBar = new CSnrBarChartL2(1);
#else
	gpsSnrBar = new CSnrBarChartDual;
	bdSnrBar = new CSnrBarChartBeidou;
#endif

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
	//m_gpsdoInProgress = false;
	m_nDefaultTimeout = g_setting.defaultTimeout;
	m_coorFormat = DegreeMinuteSecond;
	m_copyLatLon = FALSE;
	m_bClearPsti032 = FALSE;
#if (SPECIAL_TEST)
	specCmd = NULL;
	specSize = 0;

	CString filePath = theApp.GetCurrrentDir() + "\\specialcmd.bin";
	CFile f;
	if(f.Open(filePath, CFile::modeRead | CFile::typeBinary))
	{
		specSize = (U32)f.GetLength();
		if(specSize > 0)
		{
			specCmd = new U08[specSize];
			f.Read(specCmd, specSize);
		}
		f.Close();
	}
#endif
  m_nDoFlag = DO_NOTHING;
  m_mouseNoMoving = FALSE;
  m_mouseMouingTick = GetTickCount();
#if defined(XN120_TESTER)
  m_nXn120TestSatus = 0;
#endif
  downloadAddTag = FALSE;
  tagAddress = 0;
  tagValue = 0;
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
	DDX_Control(pDX, IDC_HDOP, m_hdop);
	DDX_Control(pDX, IDC_RTK_AGE, m_rtkAge);
	DDX_Control(pDX, IDC_RTK_RATIO, m_rtkRatio);
#if(_TAB_LAYOUT_)
	//DDX_Control(pDX, IDC_DATE2, m_date2);
	DDX_Control(pDX, IDC_CYCLE_SLIP, m_cycleSlip);
	DDX_Control(pDX, IDC_TIME2, m_time2);
	DDX_Control(pDX, IDC_EAST_PROJECTION, m_eastProjection);
	DDX_Control(pDX, IDC_BASELINE_LENGTH, m_baselineLength);
	DDX_Control(pDX, IDC_NORTH_PROJECTION, m_northProjection);
	DDX_Control(pDX, IDC_BASELINE_COURSE, m_baselineCourse);
	DDX_Control(pDX, IDC_UP_PROJECTION, m_upProjection);
#endif
	DDX_Control(pDX, IDC_COMPORT, m_ComPortCombo);
	DDX_Control(pDX, IDC_BAUDRATE_IDX, m_BaudRateCombo);
	DDX_Control(pDX, IDC_COOR, m_coordinate);
	DDX_Control(pDX, IDC_ENUSCALE, m_scale);
	DDX_Control(pDX, IDC_MAPSCALE, m_mapscale);
	DDX_Control(pDX, IDC_2DRMS, m_twodrms);
	DDX_Control(pDX, IDC_2DRMS_2, m_twodrms2);
	DDX_Control(pDX, IDC_CEP50, m_cep);
	DDX_Control(pDX, IDC_CEP50_2, m_cep2);
	DDX_Control(pDX, IDC_CENTER_ALT, m_centerAlt);
	DDX_Control(pDX, IDC_RESPONSE, m_responseList);
	DDX_Control(pDX, IDC_MESSAGE, m_nmeaList);
	DDX_Control(pDX, IDC_OPEN_CLOSE_T, m_connectT);
	DDX_Control(pDX, IDC_FIRMWARE_PATH, m_lbl_firmware_path);
	DDX_Control(pDX, IDC_INFO_PANEL, *m_infoPanel);
	DDX_Control(pDX, IDC_EARTH_PANEL, *m_earthPanel);
	DDX_Control(pDX, IDC_SCATTER_PANEL, *m_scatterPanel);
	DDX_Control(pDX, IDC_DOWNLOAD_PANEL, *m_downloadPanel);
	DDX_Control(pDX, IDC_BROWSE, m_btn_browse);
	DDX_Control(pDX, IDC_WARMSTART, m_bnt_warmstart);
	DDX_Control(pDX, IDC_COLDSTART, m_btn_coldstart);
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
	DDX_Control(pDX, IDC_BD_BAR, *bdSnrBar);
	DDX_Control(pDX, IDC_GA_BAR, *gaSnrBar);
	DDX_Control(pDX, IDC_GI_BAR, *giSnrBar);
	DDX_Control(pDX, IDC_EARTH, *pic_earth);
	DDX_Control(pDX, IDC_SCATTER, *pic_scatter);
	DDX_Control(pDX, IDC_FIXED_STATUS, m_fixed_status);
	DDX_Control(pDX, IDC_ODO_METER, m_odo_meter);
	//DDX_Control(pDX, IDC_GYRO_DATA, m_gyro_data);
	DDX_Control(pDX, IDC_BK_INDICATOR, m_backward_indicator);
	DDX_Control(pDX, IDC_ODO_METER_T, m_lbl_odo_meter);
	DDX_Control(pDX, IDC_GYRO_DATA_T, m_lbl_gyro_data);
	DDX_Control(pDX, IDC_BK_INDICATOR_T, m_lbl_backward_indicator);
}

BEGIN_MESSAGE_MAP(CGPSDlg, CDialog)
	ON_WM_CLOSE()
	ON_WM_DRAWITEM()
	ON_WM_INITMENUPOPUP()
	ON_WM_LBUTTONDOWN()
	//ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SYSCOMMAND()
	ON_WM_TIMER()
  ON_WM_MOUSEMOVE()

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
	ON_BN_CLICKED(IDC_SCANALL, OnBnClickedScanAll)
	ON_BN_CLICKED(IDC_SCANBAUDRATE, OnBnClickedScanBaudrate)
	ON_BN_CLICKED(IDC_SCANPORT, OnBnClickedScanPort)
	ON_BN_CLICKED(IDC_SETORIGIN, OnBnClickedSetOrigin)
	ON_BN_CLICKED(IDC_WARMSTART, OnBnClickedWarmstart)

	ON_CBN_CLOSEUP(IDC_COOR, OnCbnCloseupCoordinate)
	ON_CBN_CLOSEUP(IDC_ENUSCALE, OnCbnCloseupEnuscale)
	ON_CBN_CLOSEUP(IDC_MAPSCALE, OnCbnCloseupMapscale)

	ON_COMMAND(ID_CONFIG_1PPS_FREQ_OUTPUT, &CGPSDlg::OnConfig1ppsFrequencyOutput)
	ON_COMMAND(ID_CFG_1PPS_OUTPUT_MODE, &CGPSDlg::OnConfigure1PpsOutputMode)
	ON_COMMAND(ID_CFG_PRPTY_NMEA, OnConfigureProprietaryNmea)
	ON_COMMAND(ID_ON_LINE_ASSIST, &CGPSDlg::OnLineAssistance)
	ON_COMMAND(ID_MONITORING_1PPS, OnMonitoring1Pps)
	//ON_COMMAND(ID_1PPSTIMING_QUERYPPSCLKSOURCE, &CGPSDlg::On1ppstimingQueryppspulseclksrc)
	ON_COMMAND(ID_QUERY_1PPS_OUTPUT_MODE, &CGPSDlg::OnQueryPpsOutputMode)
	ON_COMMAND(ID_AGPS_CONFIG, OnAgpsConfig)
	ON_COMMAND(ID_CFG_GPS_MEAS_MODE, OnConfigGpsMeasurementMode)
	ON_COMMAND(ID_CFG_BIN_ITV, OnBinaryConfigureBinaryInterval)
	//ON_COMMAND(ID_CFG_DATUM, OnBinaryConfiguredatum)
	//ON_COMMAND(ID_BINARY_CONFIGUREDOPMASK, OnBinaryConfiguredopmask)
	ON_COMMAND(ID_CFG_MESSAGE_TYPE, OnConfigMessageOut)
	ON_COMMAND(ID_CFG_MULTI_PATH, OnBinaryConfiguremultipath)
  //ON_COMMAND(ID_BINARY_CONFIGURENMEAOUTPUT, OnBinaryConfigurenmeaoutput)
	ON_COMMAND(ID_CFG_NMEA_INTERVAL_V8, OnConfigureNmeaIntervalV8)
	ON_COMMAND(ID_CONFIG_ERICSSON_STC_ITV, OnConfigureEricssonSentecneInterval)

	ON_COMMAND(ID_CFG_NMEA_OUTPUT_COM, &CGPSDlg::OnConfigNmeaOutputComPort)
	ON_COMMAND(ID_CFG_NMEA_TALKER_ID, &CGPSDlg::OnBinaryConfigurenmeatalkerid)
	ON_COMMAND(ID_CFG_PING_PRMTR, OnBinaryConfigurepinningparameters)
	ON_COMMAND(ID_CFG_PST_PING, OnBinaryConfigurepositionpinning)
	ON_COMMAND(ID_CFG_POSITION_UPDATE_RATE, OnConfigurePositionUpdateRate)
	//ON_COMMAND(ID_CONFIG_DR_MULTIHZ, OnBinaryConfigDrMultiHz)
	ON_COMMAND(ID_CFG_POWER_MODE, OnBinaryConfigurepowermode)
	ON_COMMAND(ID_CFG_PWR_SAV_PAR, &CGPSDlg::OnConfigPowerSavingParameters)
	//ON_COMMAND(ID_CONFIG_V8_POWER_SV_PARAM_ROM, &CGPSDlg::OnConfigPowerSavingParametersRom)
	ON_COMMAND(ID_CFG_PROPRIETARY_MESSAGE, &CGPSDlg::OnConfigProprietaryMessage)
	ON_COMMAND(ID_CONFIG_DOZE_MODE, &CGPSDlg::OnConfigGnssDozeMode)

	ON_COMMAND(ID_CFG_REGISTER, OnBinaryConfigureregister)
	ON_COMMAND(ID_CFG_REGISTER16, OnBinaryConfigureregister16)
	ON_COMMAND(ID_QUERY_CLOCK_OFFSET, OnBinaryQueryClockOffset)
	ON_COMMAND(ID_SET_CLOCK_OFFSET, OnBinaryConfigureClockOffset)
	ON_COMMAND(ID_CFG_SERIAL_PORT, OnConfigureSerialPort)
	ON_COMMAND(ID_CFG_SUBSEC_REG, OnConfigSubSecRegister)
	ON_COMMAND(ID_BINARY_DUMP_DATA, OnBinaryDumpData)
	ON_COMMAND(ID_GET_RGISTER, OnBinaryGetrgister)
	ON_COMMAND(ID_GET_RGISTER16, OnBinaryGetrgister16)
	//ON_COMMAND(ID_BINARY_POSITIONFINDER, OnBinaryPositionfinder)

	ON_COMMAND(ID_RESET_ODOMETER, OnResetOdometer)
	ON_COMMAND(ID_SYSTEM_RESTART, OnBinarySystemRestart)
	ON_COMMAND(ID_CFG_DOP_MASK, OnConfigureDopMask)
	ON_COMMAND(ID_CFG_TIMING_CABLE_DELAY, OnConfigTimingCableDelay)
	ON_COMMAND(ID_CFG_TIMING, OnConfigTiming)
	ON_COMMAND(ID_CONFIG_ELEV_AND_CNR_MASK, OnConfigElevationAndCnrMask)
	ON_COMMAND(ID_CONVERTER_COMPRESS, OnConverterCompress)
	ON_COMMAND(ID_LOG_DECOMPRESS, OnCovDecopre)
	ON_COMMAND(ID_DECODE_GP_ALMANAC, OnDecodeGpsAlmanac)
	ON_COMMAND(ID_CONVERTER_KML, OnConverterKml)
	ON_COMMAND(ID_RAW_MEAS_OUT_CONVERT, OnRawMeasurementOutputConvert)
	ON_COMMAND(ID_UBLOX_OUT_CONVERT, OnUbloxBinaryOutputConvert)
	ON_COMMAND(ID_HOSTLOG_NMEA, OnHosLogToNmea)
	ON_COMMAND(ID_LOG_CONFIGURE, OnDatalogLogConfigureControl)
	ON_COMMAND(ID_LOGW_CONFIGURE, OnDatalogWatchLogConfigureControl)
	ON_COMMAND(ID_NMEA_CHECKSUM_CAL, OnNmeaChecksumCalculator)
	ON_COMMAND(ID_BIN_CHECKSUM_CAL, OnBinaryChecksumCalculator)
	ON_COMMAND(ID_HOSTLOG, OnHostLog)
	ON_COMMAND(ID_TEST_EXTERNAL_SREC, OnTestExternalSrec)
	ON_COMMAND(ID_IQ_PLOT, OnIqPlot)
	ON_COMMAND(ID_READ_MEM_TO_FILE, OnReadMemToFile)
	ON_COMMAND(ID_READ_MEM_TO_FILE2, OnReadMemToFile2)
	ON_COMMAND(ID_PATCH_ROM_EPH, OnPatchRom20130221Ephemeris)
	ON_COMMAND(ID_IO_TESTER, OnIoTester)
  
	ON_COMMAND(ID_WRITE_MEM_TO_FILE, OnWriteMemToFile)
	ON_COMMAND(ID_UPGRADE_DOWNLOAD, OnUpgradeDownload)
	ON_COMMAND(ID_PATCH, OnPatch)
	ON_COMMAND(ID_LOG_READ, OnDatalogLogReadBatch)
	ON_COMMAND(ID_CONFIG_WATCH_TRACKBACK, OnConfigWatchTrackback)
	ON_COMMAND(ID_SHOW_GP_ALMANAC, OnShowGpsAlmanac)
	ON_COMMAND(ID_SHOW_BD_ALMANAC, OnShowBeidouAlmanac)
	ON_COMMAND(ID_GET_GP_ALMANAC, OnGetGpsAlmanac)
	ON_COMMAND(ID_SET_GP_ALMANAC, OnSetGpsAlmanac)
	ON_COMMAND(ID_GET_GL_ALMANAC, &CGPSDlg::OnGetGlonassAlmanac)
	ON_COMMAND(ID_SET_GL_ALMANAC, &CGPSDlg::OnSetGlonassAlmanac)
	ON_COMMAND(ID_GET_BD_ALMANAC, &CGPSDlg::OnGetBeidouAlmanac)
	ON_COMMAND(ID_SET_BD_ALMANAC, &CGPSDlg::OnSetBeidouAlmanac)
	ON_COMMAND(ID_GET_GP_EPHEMERIS, OnEphemerisGetephemeris)
	//ON_COMMAND(ID_EPHEMERIS_GETGPSGLONASS, &CGPSDlg::OnEphemerisGetgpsglonass)
	//ON_COMMAND(ID_EPHEMERIS_GETGPSGLONASSALMANAC, &CGPSDlg::OnEphemerisGetgpsglonassalmanac)
	ON_COMMAND(ID_GET_GL_TIME_CORRECTIONS, &CGPSDlg::OnEphemerisGettimecorrections)
	ON_COMMAND(ID_SET_GP_EPHEMERIS, OnEphemerisSetephemeris)
	//ON_COMMAND(ID_EPHEMERIS_SETGPSGLONASS, &CGPSDlg::OnEphemerisSetgpsglonass)
	//ON_COMMAND(ID_EPHEMERIS_SETGPSGLONASS_ALMANAC, &CGPSDlg::OnEphemerisSetgpsglonassAlmanac)
	ON_COMMAND(ID_SET_GL_TIME_CORRECTIONS, &CGPSDlg::OnEphemerisSettimecorrections)
	ON_COMMAND(ID_FILE_CLEANNEMA, OnFileCleannema)
	ON_COMMAND(ID_FILE_SETUP, OnFileSetup)
	ON_COMMAND(ID_FILE_EXIT, OnFileExit)
	ON_COMMAND(ID_FILE_SAVENMEA, OnFileSaveNmea)
	ON_COMMAND(ID_FILE_BINARY, OnFileSaveBinary)
	ON_COMMAND(ID_SAVE_BINARY_NOPARSING, OnSaveBinaryNoParsing)
	ON_COMMAND(ID_SAVE_TELIT_NOPARSING, OnSaveTelitNoParsing)
	ON_COMMAND(ID_VERIFY_FIRMWARE, OnVerifyFirmware)
	ON_COMMAND(ID_FILE_PLAYNMEA, OnFilePlayNmea)
	ON_COMMAND(ID_HELP_ABOUT, OnHelpAbout)
	//ON_COMMAND(ID_LOGGER_CONVERT, OnLoggerConvert)
	//ON_COMMAND(ID_MINIHOMER_ACTIVATE, OnMinihomerActivate)
	//ON_COMMAND(ID_MINIHOMER_QUERYTAG, OnMinihomerQuerytag)
	//ON_COMMAND(ID_MINIHOMER_SETTAGECCO, OnMinihomerSettagecco)
	ON_COMMAND(ID_CFG_NVGTN_MODE, OnMultimodeConfiguremode)
	ON_COMMAND(ID_QUERY_CABLEDELAY, OnQueryCableDelay)

	ON_COMMAND(ID_FCTRY_DFLT_NO_RBT, OnSetFactoryDefaultNoReboot)
	ON_COMMAND(ID_FCTRY_DFLT_RBT, OnSetFactoryDefaultReboot)
	//ON_COMMAND(ID_WAAS_WAAS, OnWaasWaas)

	ON_MESSAGE(WM_DEVICECHANGE, OnMyDeviceChange)
	ON_MESSAGE(UWM_KERNEL_REBOOT, OnKernelReboot)
	ON_MESSAGE(UWM_FIRST_NMEA, OnFirstNmea)
	ON_MESSAGE(UWM_SHOW_TIME, OnShowTime)
	ON_MESSAGE(UWM_SHOW_RMC_TIME, OnShowRMCTime)
	ON_MESSAGE(UWM_UPDATE_UI, OnUpdateUI)
	//ON_MESSAGE(UWM_GPSDO_HI_DOWNLOAD, OnGpsdoHiDownload)
	ON_MESSAGE(UWM_DO_ZENLANE_CMD, OnDoZenlaneCmd)
#if defined(XN120_TESTER)
	ON_MESSAGE(UWM_TEST_XN112_START, OnTestXn112Start)
#endif

	ON_COMMAND(ID_CFG_SAEE, OnBinaryConfigureSAGPS)
	ON_COMMAND(ID_CFG_SBAS, OnBinaryConfigureSBAS)
	ON_COMMAND(ID_CFG_SBAS2, OnBinaryConfigureSBAS2)
	ON_COMMAND(ID_CFG_QZSS, OnBinaryConfigureQZSS)
	ON_COMMAND(ID_CFG_DGPS, OnBinaryConfigureDGPS)
	ON_COMMAND(ID_CFG_SMOOTH_MODE, OnBinaryConfigureSmoothMode)
	ON_COMMAND(ID_CFG_TIME_STAMPING, OnBinaryConfigTimeStamping)
	ON_COMMAND(ID_CONFIG_LEAP_SECONDS, OnConfigLeapSeconds)
	ON_COMMAND(ID_CFG_PARAM_SRCH_ENG_SLP_CRT, OnConfigParamSearchEngineSleepCriteria)
	ON_COMMAND(ID_CONFIG_DATUM_INDEX, OnConfigDatumIndex)
	ON_COMMAND(ID_CONFIG_VERY_LOW, OnConfigVeryLowSpeed)
	ON_COMMAND(ID_CONFIG_DF_UNIQUE_ID, OnConfigDofunUniqueId)
	ON_COMMAND(ID_ERASE_DF_UNIQUE_ID, OnEraseDofunUniqueId)

	ON_COMMAND(ID_CFG_NOISE_PWR_CTRL, OnConfigureNoisePowerControl)
	ON_COMMAND(ID_CFG_ITRFRC_DTCT_CTRL, OnConfigureInterferenceDetectControl)
	ON_COMMAND(ID_CFG_NMBI_OUT_DES, OnConfigNMEABinaryOutputDestination)
	ON_COMMAND(ID_CFG_PARAM_SEARCH_ENG_NUM, OnConfigParameterSearchEngineNumber)
	ON_COMMAND(ID_CFG_ANTENNA_DETECTION, &CGPSDlg::OnBinaryConfigureantennadetection)
	ON_COMMAND(ID_AGPS_FTP_SREC, OnAgpsFtpSrec)
	ON_COMMAND(ID_ROMAGPS_FTP_SREC, OnRomAgpsFtpSrec)
	ON_COMMAND(ID_ROMAGPS_FTP_NEW, OnRomAgpsFtpNew)
	ON_COMMAND(ID_CLOCK_OFFSET_PREDICT, OnClockOffsetPredict)
	ON_COMMAND(ID_CLOCK_OFFSET_PREDICT_OLD, OnClockOffsetPredictOld)
	ON_COMMAND(ID_HOSTBASED_DOWNLOAD, OnHostBasedDownload)
	ON_COMMAND(ID_FIRMWARE_DOWNLOAD, OnFiremareDownload)
	ON_COMMAND(ID_CFG_POS_FIX_NAV_MASK, OnConfigPositionFixNavigationMask)
	ON_COMMAND(ID_PARALLEL_DOWNLOAD, OnParallelDownload)
	ON_COMMAND(ID_CONFIG_REF_TIME_TO_GPS, OnConfigRefTimeSyncToGpsTime)
	ON_COMMAND(ID_CFG_1PPS_PULSE_WIDTH, OnConfigure1PpsPulseWidth)
	ON_COMMAND(ID_QUERY_1PPS_PULSE_WIDTH, OnQuery1ppsPulseWidth)
	ON_COMMAND(ID_CONFIG_GNSS_NAV_SOL, OnConfigQueryGnssNavSol)
	ON_COMMAND(ID_CONFIG_BIN_MEA_DAT_OUT, OnConfigBinaryMeasurementDataOut)
	ON_COMMAND(ID_CONFIG_RTCM_MEA_DAT_OUT, OnConfigRtcmMeasurementDataOut)
	ON_COMMAND(ID_QUERY_BIN_MEA_DAT_OUT, OnQueryBinaryMeasurementDataOut)
	ON_COMMAND(ID_QUERY_RTCM_MEA_DAT_OUT, OnQueryRtcmMeasurementDataOut)

	//New type
	ON_COMMAND(ID_QUERY_POSITION_UPDATE_RATE, OnQueryPositionRate)
	ON_COMMAND(ID_QUERY_DATUM, OnQueryDatum)
	ON_COMMAND(ID_QUERY_SHA1, OnQuerySha1String)
	ON_COMMAND(ID_QUERY_CON_CAP, OnQueryConstellationCapability)
	ON_COMMAND(ID_QUERY_SW_VERSION, OnQuerySoftwareVersionSystemCode)
	ON_COMMAND(ID_QUERY_SW_CRC, OnQuerySoftwareCrcSystemCode)
	ON_COMMAND(ID_QUERY_POSITION_PINNING, OnQueryPositionPinning)
	ON_COMMAND(ID_QUERY_GPS_MSRMNT_MODE, OnQuery1ppsMode)
	ON_COMMAND(ID_QUERY_POWER_MODE, OnQueryPowerMode)
	ON_COMMAND(ID_QUERY_V8_POWER_SV_PARAM, OnQueryV8PowerSavingParameters)
	ON_COMMAND(ID_QUERY_1PPS_FREQ_OUTPUT, OnQuery1ppsFreqencyOutput)
	ON_COMMAND(ID_QUERY_PROPRIETARY_MSG, OnQueryProprietaryMessage)
	ON_COMMAND(ID_QUERY_TIMING, OnQueryTiming)
	ON_COMMAND(ID_QUERY_DOP_MASK, OnQueryDopMask)
	ON_COMMAND(ID_QUERY_ELE_CNR_MSK, OnQueryElevationAndCnrMask)
	ON_COMMAND(ID_QUERY_ANTENNA_DETECTION, OnQueryAntennaDetection)
	ON_COMMAND(ID_QUERY_NOISE_POWER, OnQueryNoisePower)
	ON_COMMAND(ID_QUERY_DR_INFO, OnQueryDrInfo)		//Not test
	ON_COMMAND(ID_QUERY_DR_HW_PARAMETER, OnQueryDrHwParameter)	//Not test
	ON_COMMAND(ID_QUERY_NMEA_TALKER_ID, OnQueryGnssNmeaTalkId)
	ON_COMMAND(ID_QUERY_GNSS_KNUM_SLT_CNR, OnQueryGnssKnumberSlotCnr2)//Not test
	ON_COMMAND(ID_QUERY_SBAS_DEFAULT, OnQuerySbasDefault)
	ON_COMMAND(ID_QUERY_SBAS, OnQuerySbas)
	ON_COMMAND(ID_QUERY_SBAS2, OnQuerySbas2)
	ON_COMMAND(ID_QUERY_SAEE, OnQuerySagps)
	ON_COMMAND(ID_QUERY_QZSS, OnQueryQzss)
	ON_COMMAND(ID_QUERY_DGPS, OnQueryDgps)
	ON_COMMAND(ID_QUERY_SMOOTH_MODE, OnQuerySmoothMode)
	ON_COMMAND(ID_QUERY_TIME_STAMPING, OnQueryTimeStamping)
	ON_COMMAND(ID_QUERY_GPS_TIME, OnQueryGpsTime)
	ON_COMMAND(ID_QUERY_NOISE_PW_CTL, OnQueryNoisePowerControl)
	ON_COMMAND(ID_QUERY_ITRFRC_DTCT_CTRL, OnQueryInterferenceDetectControl)
	ON_COMMAND(ID_QUERY_NMBI_OUT_DES, OnQueryNmeaBinaryOutputDestination)
	ON_COMMAND(ID_QUERY_PARAM_SEARCH_ENG_NUM, OnQueryParameterSearchEngineNumber)
	ON_COMMAND(ID_AGPS_STATUS, OnQueryAgpsStatus)
	ON_COMMAND(ID_LOG_STATUS, OnQueryDatalogLogStatus)
	ON_COMMAND(ID_LOGW_STATUS, OnQueryDatalogWatchLogStatus)
  ON_COMMAND(ID_LOG_CLEAR, OnDatalogClear)
  ON_COMMAND(ID_LOGW_CLEAR, OnDatalogWatchClear)
	ON_COMMAND(ID_QUERY_POS_FIX_NAV_MASK, OnQueryPositionFixNavigationMask)
	ON_COMMAND(ID_QUERY_NMEA_INTERVAL_V8, OnQueryNmeaIntervalV8)
	ON_COMMAND(ID_QUERY_ERICSSON_STC_ITV, OnQueryEricssonInterval)
	ON_COMMAND(ID_QUERY_REF_TIME_TO_GPS, OnQueryRefTimeSyncToGpsTime)
	ON_COMMAND(ID_QUERY_PARAM_SRCH_ENG_SLP_CRT, OnQuerySearchEngineSleepCriteria)
	ON_COMMAND(ID_QUERY_NAV_MODE_V8, OnQueryNavigationModeV8)
	ON_COMMAND(ID_QUERY_BOOT_STATUS, OnQueryGnssBootStatus)
	//ON_COMMAND(ID_QUERY_DR_MULTIHZ, OnQueryDrMultiHz)
	ON_COMMAND(ID_QUERY_GNSS_NAV_SOL, OnQueryGnssNavSol)
	ON_COMMAND(ID_QUERY_CUSTOMER_ID, OnQueryCustomerID)
	ON_COMMAND(ID_QUERY_SERIAL_NUMBER, OnQuerySerialNumber)
	ON_COMMAND(ID_CONFIG_SERIAL_NUMBER, OnConfigureSerialNumber)
	ON_COMMAND(ID_QUERY_DATUM_INDEX, OnQueryDatumIndex)
	ON_COMMAND(ID_QUERY_VERY_LOW, OnQueryVeryLowSpeed)
	ON_COMMAND(ID_QUERY_DF_UNIQUE_ID, OnQueryDofunUniqueId)

	ON_COMMAND(ID_QUERY_UARTPASS, OnQueryUartPass)
	ON_COMMAND(ID_GPSDO_RESET_SLAVE, OnGpsdoResetSlave)
	ON_COMMAND(ID_GPSDO_ENTER_ROM, OnGpsdoEnterRom)
	ON_COMMAND(ID_GPSDO_LEAVE_ROM, OnGpsdoLeaveRom)
	ON_COMMAND(ID_GPSDO_ENTER_DWN, OnGpsdoEnterDownload)
	ON_COMMAND(ID_GPSDO_LEAVE_DWN, OnGpsdoLeaveDownload)
	ON_COMMAND(ID_GPSDO_ENTER_DWN_H, OnGpsdoEnterDownloadHigh)
	ON_COMMAND(ID_GPSDO_FW_DOWNLOAD, OnGpsdoFirmwareDownload)
	ON_COMMAND(ID_GPSDO_ENTER_UART, OnGpsdoEnterUart)
	ON_COMMAND(ID_GPSDO_LEAVE_UART, OnGpsdoLeaveUart)

	ON_COMMAND(ID_INSDR_ENTER_UART, OnInsdrEnterUart)
	ON_COMMAND(ID_INSDR_ENTER_DWN, OnInsdrEnterDownload)
	ON_COMMAND(ID_INSDR_LEAVE_UART, OnInsdrLeaveUart)
	ON_COMMAND(ID_INSDR_TEST, OnInsdrTest)
	ON_COMMAND(ID_INSDR_ACC_SELF_TEST, OnInsdrAccelerometerSelfTest)
	ON_COMMAND(ID_INSDR_GYRO_SELF_TEST, OnInsdrGyroscopeSelfTest)
	ON_COMMAND(ID_INSDR_ACCU_ANGLE_START, OnInsdrAccumulateAngleStart)
	ON_COMMAND(ID_INSDR_ACCU_ANGLE_STOP, OnInsdrAccumulateAngleStop)
	ON_COMMAND(ID_QUERY_DR_RATE, OnQueryDrRate)
	ON_COMMAND(ID_CONFIG_DR_RATE, OnConfigDrRate)
	ON_COMMAND(ID_QUERY_DR_RAW_RATE, OnQueryDrRawRate)
	ON_COMMAND(ID_CONFIG_DR_RAW_RATE, OnConfigDrRawRate)

	ON_COMMAND(ID_SUP800_ERASE_DATA, OnSup800EraseData)
	ON_COMMAND(ID_SUP800_WRITE_DATA, OnSup800WriteData)
	ON_COMMAND(ID_SUP800_READ_DATA, OnSup800ReadData)

	ON_COMMAND(ID_CONFIG_GEOFENCE, OnConfigGeofence)
	ON_COMMAND(ID_CONFIG_GEOFENCE1, OnConfigGeofence1)
	ON_COMMAND(ID_CONFIG_GEOFENCE2, OnConfigGeofence2)
	ON_COMMAND(ID_CONFIG_GEOFENCE3, OnConfigGeofence3)
	ON_COMMAND(ID_CONFIG_GEOFENCE4, OnConfigGeofence4)
	//ON_COMMAND(ID_QUERY_GEOFENCE, OnQueryGeofence)
	ON_COMMAND(ID_QUERY_GEOFENCE1, OnQueryGeofence1)
	ON_COMMAND(ID_QUERY_GEOFENCE2, OnQueryGeofence2)
	ON_COMMAND(ID_QUERY_GEOFENCE3, OnQueryGeofence3)
	ON_COMMAND(ID_QUERY_GEOFENCE4, OnQueryGeofence4)
	ON_COMMAND(ID_QUERY_GEOFENCE_RESULT, OnQueryGeofenceResult)
	ON_COMMAND(ID_QUERY_GEOFENCE_RESULTEX, OnQueryGeofenceResultEx)

	ON_COMMAND(ID_ENTER_RTK_DEBUG_MODE, OnEnterRtkDebugMode)
	ON_COMMAND(ID_BACK_RTK_DEBUG_MODE, OnBackRtkDebugMode)

	ON_COMMAND(ID_QUERY_RTK_MODE, OnQueryRtkMode)
	ON_COMMAND(ID_QUERY_RTK_MODE2, OnQueryRtkMode2)
	ON_COMMAND(ID_QUERY_RTK_SLAVE_BAUD, OnQueryRtkSlaveBaud)
	ON_COMMAND(ID_CLEAR_RTK_SLAVE_DATA, OnClearRtkSlaveData)

	ON_COMMAND(ID_QUERY_BASE_POSITION, OnQueryBasePosition)
	ON_COMMAND(ID_CONFIG_RTK_MODE, OnConfigRtkMode)
	ON_COMMAND(ID_CONFIG_RTK_MODE2, OnConfigRtkMode2)
	ON_COMMAND(ID_CFG_RTK_SLAVE_BAUD, OnConfigRtkSlaveBaud)
  ON_COMMAND(ID_CONFIG_BASE_POSITION, OnConfigBasePosition)
  
	ON_COMMAND(ID_QUERY_RTK_PARAM, OnQueryRtkParameters)
	ON_COMMAND(ID_CONFIG_RTK_PARAM, OnConfigRtkParameters)
	ON_COMMAND(ID_RTK_RESET, OnRtkReset)
	ON_COMMAND(ID_RTK_ONOFF_GP_SV, OnRtkOnOffGpsSv)
	ON_COMMAND(ID_RTK_ONOFF_SQ_SV, OnRtkOnOffSbasQzssSv)
	ON_COMMAND(ID_RTK_ONOFF_GL_SV, OnRtkOnOffGlonassSv)
	ON_COMMAND(ID_RTK_ONOFF_BD_SV, OnRtkOnOffBeidouSv)
	ON_COMMAND(ID_QUERY_RTK_REF_POSITION, OnQueryRtkReferencePosition)
 
	ON_COMMAND(ID_QUERY_PSCM_DEV_ADDR, OnQueryPstmDeviceAddress)
	ON_COMMAND(ID_QUERY_PSCM_LAT_LON, OnQueryPstnLatLonDigits)
	ON_COMMAND(ID_CONFIG_PSCM_DEV_ADDR, OnConfigPstmDeviceAddress)
	ON_COMMAND(ID_CONFIG_PSCM_LAT_LON, OnConfigPstmLatLonDigits)

	ON_COMMAND(ID_QUERY_SIG_DISTUR_DATA, OnQuerySignalDisturbanceData)
	ON_COMMAND(ID_QUERY_SIG_DISTUR_STATUS, OnQuerySignalDisturbanceStatus)
	ON_COMMAND(ID_CONFIG_SIG_DISTUR_STATUS, OnConfigureSignalDisturbanceStatus)
	ON_COMMAND(ID_CONFIG_GPS_LEAP_IN_UTC, OnConfigureGpsUtcLeapSecondsInUtc)

	ON_REGISTERED_MESSAGE(UWM_PLAYNMEA_EVENT, OnPlayNmeaEvent)
	ON_REGISTERED_MESSAGE(UWM_SAVENMEA_EVENT, OnSaveNmeaEvent)
	ON_REGISTERED_MESSAGE(UWM_UPDATE_EVENT, OnUpdateEvent)

	ON_COMMAND(ID_GET_GL_EPHEMERIS, &CGPSDlg::OnGetGlonassEphemeris)
	ON_COMMAND(ID_SET_GL_EPHEMERIS, &CGPSDlg::OnSetGlonassEphemeris)
	ON_COMMAND(ID_GET_BD_EPHEMERIS, &CGPSDlg::OnGetBeidouEphemeris)
	ON_COMMAND(ID_SET_BD_EPHEMERIS, &CGPSDlg::OnSetBeidouEphemeris)

	ON_MESSAGE(UWM_UPDATE_RTK_INFO, OnUpdateRtkInfo)
	ON_MESSAGE(UWM_UPDATE_PSTI030, OnUpdatePsti030)
	ON_MESSAGE(UWM_UPDATE_PSTI031, OnUpdatePsti031)
	ON_MESSAGE(UWM_UPDATE_PSTI032, OnUpdatePsti032)
	ON_MESSAGE(UWM_UPDATE_PSTI033, OnUpdatePsti033)
	//ON_STN_CLICKED(IDC_INFORMATION_T, &CGPSDlg::OnStnClickedInformationT)
	ON_STN_CLICKED(IDC_INFORMATION_B, &CGPSDlg::OnStnClickedInformationB)
	//ON_STN_CLICKED(IDC_RTK_INFO_T, &CGPSDlg::OnStnClickedRtkInfoT)
	ON_STN_CLICKED(IDC_RTK_INFO_B, &CGPSDlg::OnStnClickedRtkInfoB)
	ON_BN_CLICKED(IDC_COOR_SWITCH1, OnBnClickedCoorSwitch)
	ON_BN_CLICKED(IDC_COOR_SWITCH2, OnBnClickedCoorSwitch)
	ON_BN_CLICKED(IDC_ALT_SWITCH, OnBnClickedAltitudeSwitch)

	ON_COMMAND(ID_QUERY_PSTI030, OnQueryPsti030)
	ON_COMMAND(ID_QUERY_PSTI032, OnQueryPsti032)
	ON_COMMAND(ID_QUERY_PSTI033, OnQueryPsti033)
	ON_COMMAND(ID_QUERY_PSTI063, OnQueryPsti063)
	ON_COMMAND(ID_QUERY_PSTI065, OnQueryPsti065)
	ON_COMMAND(ID_QUERY_PSTI067, OnQueryPsti067)
	ON_COMMAND(ID_QUERY_PSTI068, OnQueryPsti068)
	ON_COMMAND(ID_QUERY_PSTI070, OnQueryPsti070)

	ON_COMMAND(ID_CONFIG_PSTI030, OnConfigPsti030)
	ON_COMMAND(ID_CONFIG_PSTI032, OnConfigPsti032)
	ON_COMMAND(ID_CONFIG_PSTI033, OnConfigPsti033)
	ON_COMMAND(ID_CONFIG_PSTI063, OnConfigPsti063)
	ON_COMMAND(ID_CONFIG_PSTI065, OnConfigPsti065)
	ON_COMMAND(ID_CONFIG_PSTI067, OnConfigPsti067)
	ON_COMMAND(ID_CONFIG_PSTI068, OnConfigPsti068)
	ON_COMMAND(ID_CONFIG_PSTI070, OnConfigPsti070)
	//20160906 Added
	ON_COMMAND(ID_QUERY_PSTI004, OnQueryPsti004)
	ON_COMMAND(ID_CONFIG_PSTI004, OnConfigPsti004)
  //20160914 Added
	ON_COMMAND(ID_RECALC_GLONASS_IFB, OnReCalcuteGlonassIfb)

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

	NMEA::gnssData.SetNotify(this->GetSafeHwnd());
	Initialization();
#if(_TAB_LAYOUT_)
	SwitchInfoTab();
#endif
	GetDlgItem(IDC_COOR_SWITCH1)->Invalidate(TRUE);
	GetDlgItem(IDC_COOR_SWITCH2)->Invalidate(TRUE);
	GetDlgItem(IDC_ALT_SWITCH)->Invalidate(TRUE);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

CGPSDlg::~CGPSDlg()
{
	Sleep(10);

	SafelyDelPtr(m_serial);
	SafelyDelPtr(gpsSnrBar);
	SafelyDelPtr(bdSnrBar);
	SafelyDelPtr(gaSnrBar);
	SafelyDelPtr(giSnrBar);
  SafelyDelPtr(pic_scatter);
	SafelyDelPtr(pic_earth);
	SafelyDelPtr(m_infoPanel);
	SafelyDelPtr(m_earthPanel);
	SafelyDelPtr(m_scatterPanel);
	SafelyDelPtr(m_downloadPanel);
	SafelyDelWnd(m_pScanDlg);

	if(hScanGPS)
	{
		CloseHandle(hScanGPS);
		hScanGPS = NULL;
	}

	if(m_saveNmeaDlg)
	{
		delete m_saveNmeaDlg;
	}
	if(m_playNmea)
	{
		delete m_playNmea;
	}
#if (SPECIAL_TEST)
	delete [] specCmd;
	specCmd = NULL;
#endif
}

void CGPSDlg::InitDownloadBaudRate()
{
	int dlBaudIdx = theApp.GetIntSetting("dl_baudIdx", g_setting.boostBaudIndex);
	//dlBaudIdx -= 5;	//Download Baudrate start in 115200
  CComboBox* pDownloadBaudCombo = ((CComboBox*)GetDlgItem(IDC_DL_BAUDRATE));
  pDownloadBaudCombo->ResetContent();
  pDownloadBaudCombo->AddString("4800");
  pDownloadBaudCombo->AddString("9600");
  pDownloadBaudCombo->AddString("19200");
  pDownloadBaudCombo->AddString("38400");
  pDownloadBaudCombo->AddString("57600");
  pDownloadBaudCombo->AddString("115200");
  pDownloadBaudCombo->AddString("230400");
  pDownloadBaudCombo->AddString("460800");
  pDownloadBaudCombo->AddString("921600");
  if(FIX_DOWNLOAD_115200)
  {
    pDownloadBaudCombo->EnableWindow(FALSE);
    pDownloadBaudCombo->SetCurSel(5);
  }
  else
  {
	  pDownloadBaudCombo->SetCurSel(dlBaudIdx);
  }
}

void CGPSDlg::Initialization()
{
#if (!SMALL_UI)
	LoadMenu();
#endif
#if defined(PRODUCTION_OLIVER20161128) && defined(CUSTOMIZE_COSDSTART_BUTTON)
	GetDlgItem(IDC_COLDSTART)->SetWindowText("Set Default Clock Offset");
#elif defined(XN120_TESTER)
	GetDlgItem(IDC_COLDSTART)->SetWindowText("Test IC");
#endif
	RescaleDialog();
#if (SOFTWARE_FUNCTION & SW_FUN_DR)
    GetDlgItem(IDC_COOR_T)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_COOR)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_2DRMS_T)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_2DRMS)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_CEP50_T)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_CEP50)->ShowWindow(SW_HIDE);
    m_lbl_odo_meter.SetWindowText("Odo Pulse");
    m_lbl_gyro_data.SetWindowText("Odo Calib.");
    m_lbl_backward_indicator.SetWindowText("Odo FW/BW");
#endif

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
	VERIFY(m_CoorSwitch1Btn.AutoLoad(IDC_COOR_SWITCH1, this));
	VERIFY(m_CoorSwitch2Btn.AutoLoad(IDC_COOR_SWITCH2, this));
	VERIFY(m_AltitudeSwitchBtn.AutoLoad(IDC_ALT_SWITCH, this));

	m_SetOriginBtn.EnableWindow(!g_setting.specifyCenter);
	m_scale.ResetContent();
	CString enuItems[] = { "0.01m", "0.05m", "0.1m", "0.2m", "0.5m", "1m", "2m", "3m", "5m", "10m",
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

  InitDownloadBaudRate();

	m_textFont.CreatePointFont(100, "Arial");
	m_connectT.SetFont(&m_textFont);
	m_infoFontS.CreateFont(-13, 0, 0,
		0, FW_NORMAL, 0, 0,
		0, DEFAULT_CHARSET , OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH,
		"Arial");
	m_infoFontM.CreateFont(-15, 0, 0,
		0, FW_NORMAL, 0, 0,
		0, DEFAULT_CHARSET , OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH,
		"Arial");
	m_infoFontL.CreateFont(-16, 0, 0,
		0, FW_NORMAL, 0, 0,
		0, DEFAULT_CHARSET , OUT_DEFAULT_PRECIS,
		CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH,
		"Arial");
#if (_TAB_LAYOUT_)
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

	//m_date2.SetFont(&m_infoFontL);
	m_cycleSlip.SetFont(&m_infoFontL);
	m_time2.SetFont(&m_infoFontL);
	m_eastProjection.SetFont(&m_infoFontL);
	m_baselineLength.SetFont(&m_infoFontL);
	m_northProjection.SetFont(&m_infoFontL);
	m_baselineCourse.SetFont(&m_infoFontL);
	m_upProjection.SetFont(&m_infoFontL);
	m_rtkAge.SetFont(&m_infoFontL);
	m_rtkRatio.SetFont(&m_infoFontL);

	m_twodrms2.SetFont(&m_infoFontL);
	m_cep2.SetFont(&m_infoFontL);
#elif (_MORE_INFO_)
	m_ttff.SetFont(&m_infoFontM);
	m_date.SetFont(&m_infoFontM);
	m_time.SetFont(&m_infoFontM);
	m_bootStatus.SetFont(&m_infoFontM);
	m_swKernel.SetFont(&m_infoFontM);
	m_swRev.SetFont(&m_infoFontM);
	m_longitude.SetFont(&m_infoFontS);
	m_latitude.SetFont(&m_infoFontS);
	m_altitude.SetFont(&m_infoFontM);
	m_direction.SetFont(&m_infoFontM);
	m_speed.SetFont(&m_infoFontM);
	m_hdop.SetFont(&m_infoFontM);
	m_rtkAge.SetFont(&m_infoFontM);
	m_rtkRatio.SetFont(&m_infoFontM);
	m_twodrms2.SetFont(&m_infoFontS);
	m_cep2.SetFont(&m_infoFontS);
#else
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

	m_twodrms2.SetFont(&m_infoFontL);
	m_cep2.SetFont(&m_infoFontL);
#endif

	m_centerAlt.SetFont(&m_infoFontS);

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
  InitMessageBox(NmeaMessageMode);

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
	m_isNmeaUpdated         = false;
	m_isConnectOn = false;
	m_isFlogOpen            = false;
	m_isPressCloseButton    = true;

	m_inputMode = NoOutputMode;
	//memset(MSG_TYPE_STORAGE,0,1275);
	SetMsgType(NmeaMessageMode);

	memset(&m_logFlashInfo,0,sizeof(m_logFlashInfo));
	m_logFlashInfo.max_time=3600;
	m_logFlashInfo.min_time=5;
	m_logFlashInfo.max_distance=100;
	m_logFlashInfo.min_distance=0;
	m_logFlashInfo.max_speed=100;
	m_logFlashInfo.min_speed=0;
	m_logFlashInfo.datalog_enable = 0;
	m_logFlashInfo.fifo_mode = 0;
	//slgsv = 6;

	m_tip.Create(this);
	m_tip.AddTool(GetDlgItem(IDC_CONNECT), _T("Disconnected"));
	m_tip.AddTool(GetDlgItem(IDC_CLOSE), _T("Connected"));
	m_tip.AddTool(GetDlgItem(IDC_SETORIGIN), _T("Set current as origin"));
	m_tip.AddTool(GetDlgItem(IDC_DOWNLOAD), _T(" Download firmware to target"));
	m_tip.AddTool(GetDlgItem(IDC_COOR_SWITCH1), _T("Switch coordinate format"));
	m_tip.AddTool(GetDlgItem(IDC_COOR_SWITCH2), _T("Switch coordinate format"));
	m_tip.AddTool(GetDlgItem(IDC_ALT_SWITCH), _T("Switch Altitude and Ellipsoid Height"));
	//Default attributes, Manual Reset, Initial disactived, No name
	hScanGPS = CreateEvent(NULL, TRUE, FALSE, NULL);
	if(!ResetEvent(hScanGPS))   
	{
		DWORD error = GetLastError();
	}

	m_lbl_firmware_path.SetWindowText(g_setting.mainFwPath);
	m_lbl_firmware_path.Invalidate();

	m_gpgsvMsg.NumOfSate = 0;
	m_glgsvMsg.NumOfSate = 0;
	m_bdgsvMsg.NumOfSate = 0;

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

	SetWindowText(theApp.GetTitle());

	gpsSnrBar->SetGsvData(&m_gpgsvMsg);
	gpsSnrBar->SetGsaData(&m_gpgsaMsg);
	gpsSnrBar->SetGgaData(&m_gpggaMsg);
	gpsSnrBar->SetSateStatus(&sate_gp);

	gpsSnrBar->SetGsvData2(&m_glgsvMsg);
	gpsSnrBar->SetGsaData2(&m_glgsaMsg);
	gpsSnrBar->SetGgaData2(&m_gpggaMsg);
	gpsSnrBar->SetSateStatus2(&sate_gl);

  g_setting.InitBaudrateCombo(&m_BaudRateCombo);

	m_ComPortCombo.SetCurSel(g_setting.GetComPortIndex());
	m_BaudRateCombo.SetCurSel(g_setting.GetBaudrateIndex());
	((CButton*)GetDlgItem(IDC_IN_LOADER))->SetCheck(TRUE);

  bdSnrBar->SetGsvData(&m_bdgsvMsg);
	bdSnrBar->SetGsaData(&m_bdgsaMsg);
	bdSnrBar->SetGgaData(&m_gpggaMsg);
	bdSnrBar->SetSateStatus(&sate_bd);

#if(SUPPORT_BDL2_GSV2)
  GetDlgItem(IDC_GA_SNR_T)->ShowWindow(SW_HIDE);
  GetDlgItem(IDC_GI_SNR_T)->ShowWindow(SW_HIDE);
#else
	gaSnrBar->SetGsvData(&m_gagsvMsg);
	gaSnrBar->SetGsaData(&m_gagsaMsg);
	gaSnrBar->SetGgaData(&m_gpggaMsg);
	gaSnrBar->SetSateStatus(&sate_ga);
	giSnrBar->SetGsvData(&m_gigsvMsg);
	giSnrBar->SetGsaData(&m_gigsaMsg);
	giSnrBar->SetGgaData(&m_gpggaMsg);
	giSnrBar->SetSateStatus(&sate_gi);
#endif

#if GG12A
	bdSnrBar->SetGsvData(NULL);
	bdSnrBar->SetGsaData(NULL);
	bdSnrBar->SetGgaData(NULL);
	bdSnrBar->SetSateStatus(NULL);
	gaSnrBar->SetGsvData(NULL);
	gaSnrBar->SetGsaData(NULL);
	gaSnrBar->SetGgaData(NULL);
	gaSnrBar->SetSateStatus(NULL);
	giSnrBar->SetGsvData(NULL);
	giSnrBar->SetGsaData(NULL);
	giSnrBar->SetGgaData(NULL);
	giSnrBar->SetSateStatus(NULL);
#endif
	m_altFormat = EllipsoidHeight;
  OnBnClickedAltitudeSwitch();

	m_CoorSwitch1Btn.Invalidate();
	m_CoorSwitch2Btn.Invalidate();
	m_AltitudeSwitchBtn.Invalidate();
}

void CGPSDlg::InitMessageBox(MsgMode mode)
{
  if(NmeaMessageMode == mode)
  {
    comboFont.DeleteObject();
	  comboFont.CreatePointFont(100, "Times New Roman");
	  m_nmeaList.SetFont(&comboFont, TRUE);
  }
  else if(BinaryMessageMode == mode)
  {
    comboFont.DeleteObject();
	  comboFont.CreatePointFont(90, "Courier New");
	  m_nmeaList.SetFont(&comboFont, TRUE);
  }
}

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

//void CGPSDlg::OnPaint()
//{
//	if(IsIconic())
//	{
//		CPaintDC dc(this); // device context for painting
//
//		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);
//
//		// Center icon in client rectangle
//		int cxIcon = GetSystemMetrics(SM_CXICON);
//		int cyIcon = GetSystemMetrics(SM_CYICON);
//		CRect rect;
//		GetClientRect(&rect);
//		int x = (rect.Width() - cxIcon + 1) / 2;
//		int y = (rect.Height() - cyIcon + 1) / 2;
//
//		// Draw the icon
//		dc.DrawIcon(x, y, m_hIcon);
//	}
//	else
//	{
//		CDialog::OnPaint();
//	}
//}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CGPSDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CGPSDlg::ClearInformation(bool onlyQueryInfo)
{
	NMEA::gnssData.ClearData();
	DisplayLongitude(L"");
	DisplayLatitude(L"");
	m_gpggaMsg.Altitude = -9999.9F;
	ShowAltitude();
	CLEAR_NMEA_TO_USE();

	SetTTFF(0);
	DisplayDate(0, 0, 0);
	m_date.SetWindowText("");
	DisplayTime(0, 0, 0.0);
	m_time.SetWindowText("");

	m_bootStatus.SetWindowText("");
	m_swKernel.SetWindowText("");
	m_swRev.SetWindowText("");

	m_longitude.SetWindowText("");
	m_latitude.SetWindowText("");

#if(_TAB_LAYOUT_)
	//m_date2.SetWindowText("");
	m_cycleSlip.SetWindowText("");
	m_time2.SetWindowText("");
	m_eastProjection.SetWindowText("");
	m_baselineLength.SetWindowText("");
	m_northProjection.SetWindowText("");
	m_baselineCourse.SetWindowText("");
	m_upProjection.SetWindowText("");
#endif

	m_altitude.SetWindowText("");
	ShowAltitude(true);
	m_direction.SetWindowText("0.0");
	ShowDirection(true);

#if(!MODULE_SUP_800)
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

#if(!SUPPORT_BDL2_GSV2)
	gaSnrBar->Invalidate(FALSE);
	giSnrBar->Invalidate(FALSE);
#endif

	pic_earth->Invalidate(FALSE);
	pic_scatter->Invalidate(FALSE);
}

bool CGPSDlg::NmeaInput()
{
	CFileDialog fd(true, NULL, NULL, OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, "All Suppored Files (*.txt;*.out;*.dat)|*.txt; *.out; *.dat|NMEA Files (*.txt)|*.txt|Binary Files (*.out)|*.out|Data Files (*.dat)|*.dat||");
	if(fd.DoModal() != IDOK)
	{
		return false;
	}
	m_nmeaPlayFilePath = fd.GetPathName();

	m_playNmea->ShowWindow(SW_SHOW);
	m_playNmea->Initialize(m_nmeaPlayFilePath);
	m_nmeaPlayInterval = m_playNmea->GetPlayInterval();
	m_playNmea->SetFocus();
#if(RTCM_NEW_PARSER)
		m_nmeaPlayThread = ::AfxBeginThread(NewPlayThread, (LPVOID)(LPCSTR)m_nmeaPlayFilePath);

#else
	if(0 == Utility::GetFileExt(m_nmeaPlayFilePath).CompareNoCase("txt") || 0 == Utility::GetFileExt(m_nmeaPlayFilePath).CompareNoCase("dat"))
	{
		m_nmeaPlayThread = ::AfxBeginThread(NmeaPlayThread, (LPVOID)(LPCSTR)m_nmeaPlayFilePath);
	}
	else
	{
		m_nmeaPlayThread = ::AfxBeginThread(BinaryPlayThread, (LPVOID)(LPCSTR)m_nmeaPlayFilePath);
	}
#endif
	SetConnectTitle(true);
	return true;
}

void CGPSDlg::DisplayComportError(int com, DWORD errorCode)
{
	CString err, strMsg;
	Utility::GetErrorString(err, errorCode);

	strMsg.Format("Unable to open COM%d! Error code: %d\r\n%s", com, errorCode, err);
	add_msgtolist(strMsg);
	::AfxMessageBox(strMsg);
}

int CGPSDlg::GetSelectComNumber()
{
  int n = m_ComPortCombo.GetCurSel();
  if(-1 != n)
  {
    return n;
  }

  CString t;
  m_ComPortCombo.GetWindowText(t);
  t.Replace("COM", "");
  n = atoi(t);

  return n - 1;
}

bool CGPSDlg::ComPortInput()
{
  if(!g_setting.IsValidBaudrateIndex(m_BaudRateCombo.GetCurSel()))
  {
    ::AfxMessageBox("Not supported baud rate!");
    return false;
  }

  int baudIdx = GetSelectComNumber();
  if(baudIdx < 0)
  {
    ::AfxMessageBox("Incorrect COM port!");
    return false;
  }

	g_setting.SetComPortIndex(baudIdx);
	g_setting.SetBaudrateIndex(m_BaudRateCombo.GetCurSel());
  if(m_serial == NULL)
  {
	  m_serial = new CSerial;
  }
	Utility::Log(__FUNCTION__, CTime::GetCurrentTime().Format("%Y%m%d%H%M%S"), __LINE__);
	if(!m_serial->Open(g_setting.GetComPort(), g_setting.GetBaudrateIndex()))
	{
		DisplayComportError(g_setting.GetComPort(), m_serial->errorCode);
		SafelyDelPtr(m_serial);
		SwitchToConnectedStatus(FALSE);
		return false;
	}

  m_firstDataIn = false;

	if(m_isConnectOn)
	{
		OnBnClickedClose();
	}
	CreateGPSThread();

	m_inputMode = NmeaMessageMode;
	SetConnectTitle(true);

	g_setting.Save();
	return true;
}

void CGPSDlg::OnBnClickedRecord()
{
	OnFileSaveNmea();
	OnBnClickedConnect();

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

	OnCbnCloseupEnuscale();
	OnCbnCloseupMapscale();

	m_ttffCount = 0;
	m_initTtff = false;
	SetTTFF(0);
	ClearGlonass();

  if(CUSTOMER_ZENLANE_160808)
  {
		SetTimer(ZENLANE_INIT_TIMER, 1000, NULL);
  }
}

void CGPSDlg::CreateGPSThread()
{
	if(NULL == m_serial) return;
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
	if(!m_serial->IsOpened())
	{
		m_serial->Open(g_setting.GetComPort(), g_setting.GetBaudrateIndex());
	}

	g_gpsThread = ::AfxBeginThread(ConnectGPS, 0);
	if(!SetEvent(g_connectEvent))
	{
		DWORD error = GetLastError();
	}
}

void CGPSDlg::TerminateGPSThread()
{
	SetNmeaUpdated(false);
	m_isConnectOn = false;
	m_serial->CancelTransmission();

	WaitForSingleObject(g_closeEvent, 1000);
	CloseHandle(g_closeEvent);
	g_closeEvent = NULL;

	KillTimer(SHOW_STATUS_TIMER);
	if(g_connectEvent)
	{
		CloseHandle(g_connectEvent);
		g_connectEvent = NULL;
	}

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
    if(!NeedUpdate())
    {
      CDialog::OnTimer(nIDEvent);
      return;
    }
#if (SPECIAL_TEST)
		if(specCmd != NULL && specSize > 0)
		{
			m_serial->SendData(specCmd, specSize);
			for(U32 i = 0; i < specSize; ++i)
			{
				specCmd[i] = rand() % 0xFF;
			}

		}
#endif
		switch(m_inputMode)
		{
		case NmeaMessageMode:
			if(m_isNmeaUpdated)
			{
				CopyNmeaToUse();

				ShowStatus();
				ShowDate();
				ShowLongitudeLatitude();
				ShowAltitude();
				ShowDirection();
				ShowKNumber();
#if(!MODULE_SUP_800)
				ShowVersion();
				ShowBootStatus();
				ShowSpeed();
				ShowPDOP();
#else
				ShowPsti004001();
#endif
				SetNmeaUpdated(false);
			}
			else
			{
				ShowStatus();
			}
			break;
		case BinaryMessageMode:
			//_BINMSGCS.Lock();
			//_BINMSGCS.Unlock();
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
    m_nDoFlag = DO_QUERY_VERSION;
	}
  else if(ZENLANE_INIT_TIMER==nIDEvent)
  {
		KillTimer(nIDEvent);
    m_nDoFlag = DO_ZENLANE_INIT;
  }
  else if(ZENLANE_QUERY_TIMER==nIDEvent)
  {
		KillTimer(nIDEvent);
    m_nDoFlag = DO_ZENLANE_QUERY;
  }
  else if(DELAY_PLUGIN_TIMER==nIDEvent)
	{
		KillTimer(nIDEvent);
		Close_Open_Port(plugin_wParam, plugin_port_name);
	}
#if defined(XN120_TESTER)
  else if(XN116_TESTER_TIMER==nIDEvent)
  {
    Xn116TesterEvent();
  }
#endif
	CDialog::OnTimer(nIDEvent);
}

void CGPSDlg::UpdateCooridate()
{
	CString str;

	str.Format("%.3f", g_scatterData.wgs84_X);
	m_wgs84_x.SetWindowText(str);

	str.Format("%.3f", g_scatterData.wgs84_Y);
	m_wgs84_y.SetWindowText(str);

	str.Format("%.3f", g_scatterData.wgs84_Z);
	m_wgs84_z.SetWindowText(str);

	str.Format("%.2f\r\n(%.2f)", m_gpggaMsg.Altitude + m_gpggaMsg.GeoidalSeparation, g_scatterData.ini_h);
	m_centerAlt.SetWindowText(str);

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
#if (_MORE_INFO_)
	if(g_scatterData.TwoDrms < 1.0)
	{
		str.Format("%.3f cm", g_scatterData.TwoDrms * 100);
	}
	else
	{
		str.Format("%.4f m", g_scatterData.TwoDrms);
	}
	m_twodrms2.SetWindowText(str);

	if(g_scatterData.CEP < 1.0)
	{
		str.Format("%.3f cm", g_scatterData.CEP * 100);
	}
	else
	{
		str.Format("%.4f m", g_scatterData.CEP);
	}
	m_cep2.SetWindowText(str);
#else
	str.Format("%.4f m", g_scatterData.TwoDrms);
	m_twodrms.SetWindowText(str);
	str.Format("%.4f m", g_scatterData.CEP);
	m_cep.SetWindowText(str);
#endif

}

void CGPSDlg::OnBnClickedStop()
{
	m_StopBtn.EnableWindow(FALSE);
	if(m_nmeaPlayThread)
	{
		g_nmeaInputTerminate = true;
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
	g_nmeaInputTerminate = true;
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
	SetConnectTitle(false);
}

void CGPSDlg::Terminate(void)
{
	SwitchToConnectedStatus(FALSE);

	if(m_serial!=NULL)
	{
		m_serial->CancelTransmission();
		Sleep(100);

		TerminateGPSThread();
		m_serial->Close();

		delete m_serial;
		m_serial = NULL;
	}
	m_isPressCloseButton = true;

	SetInputMode(NoOutputMode);
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

bool CGPSDlg::CheckTimeContinuity(const CString& txt, int lastH, int lastM, int lastS, int h, int m, int s)
{
  static int timeGap = 0;
  static int errorCount = 0;
  CString txtLast;

  //int tt = timeGap;
  //int ee = errorCount;
  if(timeGap == 0 && lastS != 0)
  { //First time
    timeGap = s - lastS;
  }
  else if(s > lastS)
  {
    if(timeGap == 0)
    { //First time
      //timeGap = lastS - s;
    }
    else
    {
     if(s - lastS != timeGap)
      {
        if(++errorCount < 5)
        {
          txtLast.Format("%02d:%02d:%05.2lf", lastH, lastM, lastS / 1000.0);
          add_msgtolist("Time lost in : " + txt + "," + txtLast);
        }
        else
        {
          errorCount = 0;
          timeGap = s - lastS;
        }
      }
    }
  }
  else
  {
    if((m == lastM + 1) || ((m == 0) && (h == lastH + 1)))
    {
      s += 60000;
      if(s - lastS != timeGap)
      {
        if(++errorCount < 5)
        {
          txtLast.Format("%02d:%02d:%05.2lf", lastH, lastM, lastS / 1000.0);
          add_msgtolist("Time lost in : " + txt + "," + txtLast);
        }
        else
        {
          errorCount = 0;
          timeGap = s - lastS;
        }
      }
      s -= 60000;
    }
    else
    {
      txtLast.Format("%02d:%02d:%05.2lf", lastH, lastM, lastS / 1000.0);
      add_msgtolist("Back in time : " + txt + "," + txtLast);
    }
  }
  return true;
}

void CGPSDlg::DisplayTime(int h, int m, D64 sec)
{
	static int lastH = 0, lastM = 0;
  static int lastS = 0;
  int s = (int)((sec  + 0.0005) * 1000);
  if(s >= 60000)
  {
    s -= 60000;
    ++m;
    if(m >= 60)
    {
      m -= 60;
      ++h;
      if(h >= 24)
      {
        h -= 24;
      }
    }
  }

	if(s != lastS || m != lastM || h != lastH)
	{
    CString txt;
    if((s == (int)((s / 1000) * 1000)) && ((s - lastS) == 1000 || (s - lastS) == -59000))
    {
	    txt.Format("%02d:%02d:%02.0lf", h, m, s / 1000.0);
    }
    else
    {
	    txt.Format("%02d:%02d:%05.2lf", h, m, s / 1000.0);
    }

	  m_time.SetWindowText(txt);
	  m_time.Invalidate(TRUE);
#if(_TAB_LAYOUT_)
	  m_time2.SetWindowText(txt);
	  m_time2.Invalidate(TRUE);
#endif

    if(g_setting.checkTimeError)
    { 
      CheckTimeContinuity(txt, lastH, lastM, lastS, h, m, s);
    }
		lastH = h;
		lastM = m;
		lastS = s;
  }
}
/*
void CGPSDlg::DisplayTime(int h, int m, int s)
{
	static int lastH = 0, lastM = 0, lastS = 0;
	if(s != lastS || m != lastM || h != lastH)
	{
		CString txt;
		txt.Format("%02d:%02d:%02d", h, m, s);
		m_time.SetWindowText(txt);
		m_time.Invalidate(TRUE);
#if(_TAB_LAYOUT_)
		m_time2.SetWindowText(txt);
		m_time2.Invalidate(TRUE);
#endif
		lastH = h;
		lastM = m;
		lastS = s;
	}
}
*/
void CGPSDlg::ShowRMCTime()
{
	DisplayTime(m_gprmcMsgCopy.Hour, m_gprmcMsgCopy.Min, (D64)(m_gprmcMsgCopy.Sec));
}

void CGPSDlg::ShowTime()
{
  //static int last = 0;
  //static double lastD = 0;
  //double nowD = m_gpggaMsgCopy.Sec;

  //int hh = m_gpggaMsgCopy.Hour;
  //int mm = m_gpggaMsgCopy.Min;
  //int ss =(int)(m_gpggaMsgCopy.Sec + 0.5);
  //if(ss >= 60)
  //{
  //  ss = 0;
  //  if(++mm >= 60)
  //  {
  //    mm = 0;
  //    if(++hh >= 24)
  //    {
  //      hh = 0;
  //    }
  //  }
  //}

	//DisplayTime(hh, mm, ss);
	DisplayTime(m_gpggaMsgCopy.Hour, m_gpggaMsgCopy.Min, m_gpggaMsgCopy.Sec);
  //if(last != 0 && last + 1 != ss)
  //{
  //  if(last != 59 || ss != 0)
  //  {
  //    CString dbg;
  //    dbg.Format("Last(%d,%f) ->Now(%d,%f)\r\n", last, lastD, ss, nowD);
  //    ::OutputDebugString(dbg);
  //  }
  //}
  //last = ss;
  //lastD = nowD;
}

void CGPSDlg::DisplayDate(int y, int m, int d)
{
	CString txt;
	txt.Format("%02d/%02d/%02d", y, m, d);
	m_date.SetWindowText(txt);
#if(_TAB_LAYOUT_)
	//m_date2.SetWindowText(txt);
#endif
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
{	
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

void CGPSDlg::DisplayCycleSliped()
{
	CString txt;
	txt.Format("%5d/%5d", m_psti033R.numCycleSlippedTotal, m_psti033B.numCycleSlippedTotal);
#if(_TAB_LAYOUT_)
	m_cycleSlip.SetWindowText(txt);
#endif
}

#if(MODULE_SUP_800)
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

void CGPSDlg::DisplayHdop(D64 hdop)
{
	CString txt;
	txt.Format("%5.2lf", hdop);
	m_hdop.SetWindowText(txt);
	m_hdop.Invalidate(TRUE);
}
/*
void CGPSDlg::DisplayLatitude(int h, int m, double s, char c)
{
	static int lastH= 0, lastM = 0;
	static double lastS = 0.0;
	static char lastC = ' ';
	if(s != lastS || m != lastM || h != lastH || c != lastC)
	{
		CStringW txt;
		txt.Format(L"%d�X%d'%.5f\" %c", h, m, s, c);
		::SetWindowTextW(m_latitude, txt);
		m_latitude.Invalidate(TRUE);
		lastH = h;
		lastM = m;
		lastS = s;
		lastC = c;
	}
}
*/
void CGPSDlg::DisplayLongitude(LPCWSTR txt)
{
	CString lastTxt;
	if(txt != lastTxt)
	{
		::SetWindowTextW(m_longitude, txt);
		m_longitude.Invalidate(TRUE);
		lastTxt = txt;
	}
}
/*
void CGPSDlg::DisplayLongitude(int h, int m, double s, char c)
{
	static int lastH= 0, lastM = 0;
	static double lastS = 0.0;
	static char lastC = ' ';
	if(s != lastS || m != lastM || h != lastH || c != lastC)
	{
		CStringW txt;
		txt.Format(L"%d�X%d'%.5f\" %c", h, m, s, c);
		::SetWindowTextW(m_lbl_firmware_path, txt);
		m_lbl_firmware_path.Invalidate(TRUE);
		lastH = h;
		lastM = m;
		lastS = s;
		lastC = c;
	}
}
*/
void CGPSDlg::DisplayLongitude(D64 lon, U08 c)
{
	CStringW txt;
	txt.Format(L"%d�X%d'%.5f\" %c", (int)(lon / 100.0), (int)lon - (int)(lon / 100.0) * 100,
		(lon - (int)lon) * 60.0, c);
	::SetWindowTextW(m_longitude, txt);
}

void CGPSDlg::DisplayLatitude(LPCWSTR txt)
{
	CString lastTxt;
	if(txt != lastTxt)
	{
		::SetWindowTextW(m_latitude, txt);
		m_latitude.Invalidate(TRUE);
		lastTxt = txt;
	}
}

void CGPSDlg::DisplayLatitude(D64 lat, U08 c)
{
	CStringW txt;
	txt.Format(L"%d�X%d'%.5f\" %c", (int)(lat / 100.0), (int)lat - (int)(lat / 100.0) * 100,
		(lat - (int)lat) * 60.0, c);
	::SetWindowTextW(m_latitude, txt);
	m_latitude.Invalidate(TRUE);
}

void CGPSDlg::DisplayAltitude(D64 alt)
{
	CString txt;
	txt.Format("%5.2lf", alt);
	m_altitude.SetWindowText(txt);
	m_altitude.Invalidate(TRUE);
}

void CGPSDlg::ShowLongitudeLatitude(void)
{
	int d = (int)(m_gpggaMsg.Longitude / 100.0);
	int m = (int)m_gpggaMsg.Longitude - d * 100;
	D64 s = (m_gpggaMsg.Longitude - (int)m_gpggaMsg.Longitude) * 60.0;
	char c = m_gpggaMsg.Longitude_E_W;
	CStringW txt;
	CString msg;
  if(d == 0 && m == 0 && s == 0.0 && c == 0)
  {
    d = (int)(m_gprmcMsg.Longitude / 100.0);
    m = (int)m_gprmcMsg.Longitude - d * 100;
    s = (m_gprmcMsg.Longitude - (int)m_gpggaMsg.Longitude) * 60.0;
    c = m_gprmcMsg.Longitude_E_W;
  }


	switch(m_coorFormat)
	{
	case DegreeMinuteSecond:
		txt.Format(L"%d�X%d'%.5f\" %c", d, m, s, c);
		break;
	case DegreeMinute:
		txt.Format(L"%d�X%.6f' %c", d, (double)m + s / 60, c);
		break;
	case Degree:
		txt.Format(L"%.7f�X %c", (double)d + (double)m / 60 + s / 3600, c);
		break;
	}
	DisplayLongitude(txt);
	if(m_copyLatLon)
	{
		msg = txt;
		msg += ", ";
	}
	//DisplayLongitude((int)( m_gpggaMsg.Longitude / 100.0),
	//	(int)m_gpggaMsg.Longitude - (int)(m_gpggaMsg.Longitude / 100.0) * 100,
	//	((m_gpggaMsg.Longitude - (int)m_gpggaMsg.Longitude) * 60.0),
	//	m_gpggaMsg.Longitude_E_W);
	d = (int)(m_gpggaMsg.Latitude / 100.0);
	m = (int)m_gpggaMsg.Latitude - d * 100;
	s = (m_gpggaMsg.Latitude - (int)m_gpggaMsg.Latitude) * 60.0;
	c = m_gpggaMsg.Latitude_N_S;

  if(d == 0 && m == 0 && s == 0.0 && c == 0)
  {
    d = (int)(m_gprmcMsg.Latitude / 100.0);
    m = (int)m_gprmcMsg.Latitude - d * 100;
    s = (m_gprmcMsg.Latitude - (int)m_gpggaMsg.Longitude) * 60.0;
    c = m_gprmcMsg.Latitude_N_S;
  }
	switch(m_coorFormat)
	{
	case DegreeMinuteSecond:
		txt.Format(L"%d�X%d'%.5f\" %c", d, m, s, c);
		break;
	case DegreeMinute:
		txt.Format(L"%d�X%.5f' %c", d, (double)m + s / 60, c);
		break;
	case Degree:
		txt.Format(L"%.7f�X %c", (double)d + (double)m / 60 + s / 3600, c);
		break;
	}
	DisplayLatitude(txt);
	if(m_copyLatLon)
	{
		msg += txt;
		m_copyLatLon = FALSE;
		add_msgtolist(msg);
	}
	//DisplayLatitude((int)( m_gpggaMsg.Latitude / 100.0),
	//	(int)m_gpggaMsg.Latitude - (int)(m_gpggaMsg.Latitude / 100.0) * 100,
	//	((m_gpggaMsg.Latitude - (int)m_gpggaMsg.Latitude) * 60.0),
	//	m_gpggaMsg.Latitude_N_S);
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

	if(m_gprmcMsg.SpeedKnots != 0.0F)
	{
		speed = (F32)(m_gprmcMsg.SpeedKnots * KNOTS2KMHR);
	}
	else if(m_gpvtgMsg.SpeedKmPerHur != 0.0F)
	{
		speed = m_gpvtgMsg.SpeedKmPerHur;
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
	switch(m_altFormat)
	{
	case Altitude:
		alt = m_gpggaMsg.Altitude;
		break;
	case EllipsoidHeight:
		alt = m_gpggaMsg.Altitude + m_gpggaMsg.GeoidalSeparation;
		break;
  }
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
	static QualityMode lastMode = Uninitial;
	QualityMode mode = Unlocated;

	if(m_gpggaMsg.GPSQualityIndicator != 0)
	{
		mode = GetGnssQualityMode(m_gpggaMsg.GPSQualityIndicator, (U08)m_gpgsaMsg.Mode, (U08)m_glgsaMsg.Mode, (U08)m_gagsaMsg.Mode, (U08)m_bdgsaMsg.Mode, (U08)m_gigsaMsg.Mode);
	}
	else
	{
		mode = GetGnssQualityMode(m_gprmcMsg.ModeIndicator);
	}

	if(Unlocated == mode)
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
		const char *statusStrings[] = {
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
			"Fix RTK",
			"Float RTK",
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
		case FixRTK:
		case FloatRTK:
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
#if IS_DEBUG
	CSetupDialog setupDlg;
#else
	CSetupDialog2 setupDlg;
#endif

	setupDlg.SetSetting(&g_setting);
	if(setupDlg.DoModal() == IDOK)
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
	g_nmeaInputTerminate = true;
	::WaitForSingleObject(m_nmeaPlayThread, INFINITE);
	//Sleep(1000);
#else
	if(!m_isPressCloseButton)
	{
		Terminate();
	}
	//TerminateGPSThread();

	CString pipeName = Utility::GetNameAttachPid("SkytraqIQPlotPipe");
	if(Utility::IsNamedPipeUsing(pipeName))
	{
		CString pipeName;
		pipeName.Format("//./pipe/%s", pipeName);
		HANDLE h = CreateFile(pipeName, GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE , NULL, OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL, NULL);
		if(h != INVALID_HANDLE_VALUE)
		{
			DWORD dwWrite = 0;
			const char * szCmd = "#QUIT\r\n";
			WriteFile(h, szCmd, strlen(szCmd), &dwWrite, NULL);
			::CloseHandle(h);
		}
	}
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
  if(NULL == m_serial) 
  {
    m_serial = new CSerial;
  }
  m_serial->Close();
	Sleep(200);
	//m_serial = new CSerial;
	if(!m_serial->Open(g_setting.GetComPort(), g_setting.GetBaudrateIndex()))
	{
		DisplayComportError(g_setting.GetComPort(), m_serial->errorCode);
		return false;
	}
	return true;
}

void CGPSDlg::OnBinaryDumpData()
{
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
	UINT n =pMenu->CheckMenuItem(ID_BINARY_DUMP_DATA, checkFlag);

	pMenu->Detach();
}

void CGPSDlg::ClearQue()
{
	m_serial->ClearQueue();
}

CGPSDlg::DataLogType CGPSDlg::GetDataLogType(U16 word0)
{
	U08 type = word0 >> 12;
	switch(type)
	{
	case 0x4:   //0100b
		return FIX_FULL;
	case 0x5:   //0101b
		return FIX_FULL_C;
	case 0x8:   //1000b
		return FIX_COMPACT;
	case 0x9:   //1001b
		return FIX_COMPACT_C;
	case 0x6:   //0110b
		return FIX_FULL_POI;
	case 0x7:   //0111b
		return FIX_FULL_POI_C;
	case 0x2:   //0010b
		return FIX_MULTI_HZ;
	case 0x3:   //0011b
		return FIX_MULTI_HZ_C;
	case 0xC:  //1100b
		return FIX_MULTI_HZ_POI;
	case 0xD:  //1101b
		return FIX_MULTI_HZ_POI_C;
	default:
		return FIXNONE;
	}
}

//datalog decompress
void CGPSDlg::DataLogDecompress(bool mode)
{
	CString strTxt;
	U16   word, word0;
	char  Log[1024] = {0};
	DataLogType type;
	BYTE  buffer[0x1000];
	ULONGLONG dwBytesRemaining = m_convertFile.GetLength();
	ULONGLONG startAddress = 0;
	CFile fDecompress;
	mode = (m_dataLogDecompressMode==1);

	CSkyTraqKml kml;

  CString filename = Utility::GetFilePathName(m_convertFile.GetFilePath()) + ".csv";
  BOOL openOk = FALSE;
  while(1)
  {
	  openOk = fDecompress.Open(filename, CFile::modeReadWrite|CFile::modeCreate);
    if(openOk)
    {
      break;
    }
    int r = ::AfxMessageBox("File is open in another program, try again?", MB_YESNO);
    if(IDYES != r)
    {
      break;
    }
  }
  if(!openOk)
  {
    m_convertFile.Close();
    return;
  }
	CString logg_filename = fDecompress.GetFilePath();

	strTxt = "WNO,TOW,Time,Date,DECEF_X,DECEF_Y,DECEF_Z,ECEF_X,ECEF_Y,ECEF_Z,Speed,Lon,Lat,Alt,Type,Address,CRC16,Valid\r\n";
	fDecompress.Write(strTxt, strTxt.GetLength() - 1);

	FixFull_T fixFull;
	FixCompact_T fix_inc;
	FixMultiHz_T fix_multi;

	POS_FIX_REC DataLog;
	UtcTime       utc;

	int write_count = 0;
	int file_tail = 0;
	int _10Hz_count=0;
	int start_tow=0;

	double lat,lon,alt;
	U32 temp_lat,temp_lon,temp_alt;

	geoid geo_id;
	D64 tow;
  U16 crc16 = 0, chkCrc16 = 0;
  BOOL hasCrc16 = FALSE;
	while(dwBytesRemaining)
	{
		CString tmp_file;
		CString tmp_name = m_convertFile.GetFilePath();
		int find = tmp_name.ReverseFind('.');

    tmp_file.Format("%s%d%s",tmp_name.Mid(0,find),file_tail,".kml");
		kml.Init(tmp_file, 0x0000ff);
		while(dwBytesRemaining)
		{
			startAddress = m_convertFile.GetPosition();
			UINT nBytesRead = m_convertFile.Read(buffer, 2);
			memcpy(&word, buffer, nBytesRead);
			if(word == 0xffff)
			{
				//It is Empty!
				break;
			}
			word0  = word >> 8 &0xFF;
			word0 |= word << 8 & 0xFF00;
			dwBytesRemaining -= nBytesRead;

			type = GetDataLogType(word0);
      hasCrc16 = FALSE;
			switch(type)
			{
			case FIX_MULTI_HZ:
			case FIX_MULTI_HZ_POI:
			case FIX_MULTI_HZ_C:
			case FIX_MULTI_HZ_POI_C:
				memcpy(&fix_multi, &word, sizeof(word));
				nBytesRead = m_convertFile.Read(buffer, 18);
				memcpy(&fix_multi.word[1], buffer, nBytesRead);
        if(type == FIX_MULTI_HZ_C || type == FIX_MULTI_HZ_POI_C)
        {
          crc16 = GetCrc16(&fix_multi, sizeof(fix_multi));
				  nBytesRead += m_convertFile.Read(&chkCrc16, 2);
          U16 tmpCrc = ((chkCrc16 & 0xFF) << 8) | (chkCrc16 >> 8);
          chkCrc16 = tmpCrc;
          hasCrc16 = TRUE;
        }
				if(mode)
				{
					for(int i = 0; i < 10; ++i)
					{
						word0 = fix_multi.word[i];
						fix_multi.word[i] = (word0 >> 8) & 0xFF;
						fix_multi.word[i] |= (word0 << 8) & 0xFF00;
					}
				}
				DataLog.V    = (fix_multi.word[1] + ((fix_multi.word[3] & 0xC000) << 2)) / 100.0f;
				DataLog.WNO  = fix_multi.word[0] & 0x3ff;

				tow = ((fix_multi.word[3] & 0x3fff)<<16 | fix_multi.word[2])/1000.0 + 0.005;
				DataLog.TOW  = (U32)tow;
				temp_lat = fix_multi.word[5]<<16 | fix_multi.word[4];
				temp_lon = fix_multi.word[7]<<16 | fix_multi.word[6];
				temp_alt = fix_multi.word[9]<<16 | fix_multi.word[8];
				break;
			case FIX_FULL:
			case FIX_FULL_POI:
			case FIX_FULL_C:
			case FIX_FULL_POI_C:
				memcpy(&fixFull, &word, sizeof(word));
				nBytesRead = m_convertFile.Read(buffer, 16);
				memcpy(&fixFull.word[1], buffer, nBytesRead);
        if(type == FIX_FULL_C || type == FIX_FULL_POI_C)
        {
          crc16 = GetCrc16(&fixFull, sizeof(fixFull));
				  nBytesRead += m_convertFile.Read(&chkCrc16, 2);
          U16 tmpCrc = ((chkCrc16 & 0xFF) << 8) | (chkCrc16 >> 8);
          chkCrc16 = tmpCrc;
          hasCrc16 = TRUE;
        }
				if(mode)
				{
					for(int i = 0; i < 9; ++i)
					{
						word0 = fixFull.word[i];
						fixFull.word[i]  = word0 >> 8 & 0xff;
						fixFull.word[i] |= word0 << 8 & 0xff00;
					}
				}

				DataLog.V    = (float)(fixFull.word[0] & 0x7FF);
				DataLog.WNO  = fixFull.word[1] & 0x3FF;
				DataLog.TOW  = fixFull.word[1] >> 12 & 0xF;
				DataLog.TOW |= fixFull.word[2] << 4;
				tow = DataLog.TOW;
				//
				DataLog.ECEF_X = buffer[6] << 24 | buffer[7] << 16 | buffer[4] << 8 | buffer[5];
				DataLog.ECEF_Y = buffer[10] << 24 | buffer[11] << 16 | buffer[8] << 8 | buffer[9];
				DataLog.ECEF_Z = buffer[14] << 24 | buffer[15] << 16 | buffer[12] << 8 | buffer[13];

				DataLog.DECEF_X=0;
				DataLog.DECEF_Y=0;
				DataLog.DECEF_Z=0;
				break;
			case FIX_COMPACT:
			case FIX_COMPACT_C:
				memcpy(&fix_inc, &word, sizeof(word));
				nBytesRead = m_convertFile.Read(buffer, 6);
				memcpy(&fix_inc.word[1], buffer, nBytesRead);
        if(type == FIX_COMPACT_C)
        {
          crc16 = GetCrc16(&fix_inc, sizeof(fix_inc));
				  nBytesRead += m_convertFile.Read(&chkCrc16, 2);
          U16 tmpCrc = ((chkCrc16 & 0xFF) << 8) | (chkCrc16 >> 8);
          chkCrc16 = tmpCrc;
          hasCrc16 = TRUE;
        }				
        if(mode)
				{
					for(int i=0; i < 4; ++i)
					{
						word0 = fix_inc.word[i];
						fix_inc.word[i] = word0 >> 8 & 0xFF;
						fix_inc.word[i] |= word0 << 8 & 0xFF00;
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
			if (type == FIX_MULTI_HZ || type == FIX_MULTI_HZ_POI ||
          type == FIX_MULTI_HZ_C || type == FIX_MULTI_HZ_POI_C)
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

				X = DataLog.ECEF_X;
				Y = DataLog.ECEF_Y;
				Z = DataLog.ECEF_Z;
			}
			else
			{
				X=DataLog.ECEF_X;
				Y=DataLog.ECEF_Y;
				Z=DataLog.ECEF_Z;

				double p = sqrt(pow((double)X,2.0)+pow((double)Y,2.0));
				double theta = atan(Z*WGS84_RA/(p*WGS84_RB));
				// e square
				double e2 = (pow(WGS84_RA,2.0)-pow(WGS84_RB,2.0))/pow(WGS84_RA,2.0);
				// e' square
				double e2p = (pow(WGS84_RA,2.0)-pow(WGS84_RB,2.0))/pow(WGS84_RB,2.0);

				// latitude : phi (rad.)
				lat = atan2((Z+e2p*WGS84_RB*pow(sin(theta),3.0)),(p-e2*WGS84_RA*pow(cos(theta),3.0)));
				lon = atan2((double)Y , (double)X );

				double N = WGS84_RA/(sqrt(1-WGS84_E2*sin(lat)*sin(lat)));
				alt = p/cos(lat)-N;

				lat = lat*180/PI;
				lon = lon*180/PI;
			}


			alt = alt - geo_id.GEO_calc_geoid_height(lat,lon);

			if(lon >= 130 || lon <= 120)
				TRACE("%.4f,%.4f\r\n",lat,lon);


			UtcConvertGpsToUtcTime((S16) DataLog.WNO+1024,tow, &utc);

			LL kml_lla;
			kml_lla.lat = lat;
			kml_lla.lon = lon;
			kml_lla.alt = alt;
			kml_lla.utc.hour = utc.hour;
			kml_lla.utc.minute = utc.minute;
			kml_lla.utc.sec = utc.sec;
			kml_lla.speed = DataLog.V;

			if(type == FIX_FULL_POI || (type == FIX_FULL_POI_C && (crc16 == chkCrc16)))
			{
				kml.PushOnePoi(lon, lat, alt);
			}
			else if(!hasCrc16 || (crc16 == chkCrc16))
			{
				kml.PushOnePoint(lon, lat, alt, "", PositionFix3d);
			}

			strTxt.Format("%.2f", utc.sec);
			if(strTxt=="60.00")
			{
				strTxt.Format("%.6f", utc.sec);
			}
			strTxt.Format(
      "%d,%.2f,%02d:%02d:%02.2f,%02d/%02d/%04d,%d,%d,%d,%d,%d,%d,%.2f,%.6f,%.6f,%.6f,%02X(%sb),%08Xh,%sh,%s\r\n",
				DataLog.WNO + 1024,	//%d
				tow,		//%.2f
				utc.hour,	//%d:
				utc.minute,	//%d:
				utc.sec,	//%.2f
				utc.month,
				utc.day,
				utc.year,
				DataLog.DECEF_X,DataLog.DECEF_Y,DataLog.DECEF_Z,
        X,Y,Z,
        DataLog.V, 
        lon,lat,alt, 
        type, GetBinString(type, 4), 
        (U32)startAddress,
        (hasCrc16) ? GetU16HexString(crc16) : "",
        (crc16 == chkCrc16) ? "V" : "X");
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

		kml.Finish();
		if(word == 0xffff)
		{
			//It is Empty!
			break;
		}
	}

	m_convertFile.Close();
	fDecompress.Close();
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

UINT DecompressThread(LPVOID pParam)
{
	CGPSDlg::gpsDlg->DataLogDecompress(0);
	return 0;
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

void DecodeGpsAlmanac(LPCSTR pathname)
{
  CFile f;
  U08 buf[4];
  f.Open(pathname, CFile::modeRead);
  f.Read(buf, 4);



  f.Close();
}

void CGPSDlg::OnDecodeGpsAlmanac()
{
	//CString fileName("GPS_Ephemeris.log");
	//CFileDialog dlgFile(true, _T("*.log"), NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, _T("*.log|*.log||"), this);
	//INT_PTR nResult = dlgFile.DoModal();
	//try
	//{
	//	if(nResult == IDOK)
	//	{
	//		fileName = dlgFile.GetPathName();
	//		m_convertFile.Open(fileName, CFile::modeRead);
	//		DecodeGpsAlmanac(fileName);
	//	}
 // }
	//catch(CFileException *fe)
	//{
	//}
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
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(message, 8, "Log Clear Control successfully");
	return 0;
}

void CGPSDlg::OnDatalogClearControl()
{
	if(!CheckConnect())
	{
		return;
	}

	SetInputMode(NoOutputMode);
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

bool CGPSDlg::ExecuteConfigureCommand(U08 *cmd, int size, LPCSTR msg, bool restoreConnect/* = true*/)
{
	ClearQue();
	bool b = SendToTarget(cmd, size, (msg == NULL) ? "" : msg, true);

	if(b && m_bClearPsti032)
	{
		PostMessage(UWM_UPDATE_PSTI032, 0, 0);
	}
	if(restoreConnect)
	{
		SetMode();
		CreateGPSThread();
	}
	return b;
}

void CGPSDlg::OnDatalogLogConfigureControl()
{
  CLogConfigureControlDlg dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnDatalogWatchLogConfigureControl()
{
  CLogConfigureControlDlg dlg(NULL, CLogConfigureControlDlg::Cmd740Ch);
	DoCommonConfig(&dlg);
}

int CGPSDlg::SetMessage(U08* msg, int len)
{
	return SetMessage2(m_inputMsg, msg, len);
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

CGPSDlg::CmdErrorCode CGPSDlg::SetPort0Baud(U08 baudIdx, U08 mode)
{	    
	BinaryCommand cmd(4);
	cmd.SetU08(1, 0x05);    //Command Id
	cmd.SetU08(2, 0x00);    //Port number 0 or 1
	cmd.SetU08(3, baudIdx); //Baud rate index
	cmd.SetU08(4, mode);    //Mode : 0 - SRAM, 1 - Flash, 2 - Temp

	ClearQue();
  CString strMsg;
  strMsg.Format("Change baud rate to %d...", g_setting.BaudrateValue(baudIdx));
	bool r = SendToTarget(cmd.GetBuffer(), cmd.Size(), strMsg, true);	
  if(r)
  {
    m_serial->ResetPort(baudIdx);
	  m_BaudRateCombo.SetCurSel(baudIdx);
	  Sleep(100);
  }
	return (r) ? Ack : Timeout;
}

bool CGPSDlg::SetPort(U08 port, int mode)
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
//	for(int i=0; i<10; ++i)
//	{
		//if(SendToTarget(messages, 11, "", true))
		//{
		//	break;
		//}
//	}
  bool r = SendToTarget(messages, 11, "", true);
  if(!r)
  {
	  CloseOpenUart();
	  m_BaudRateCombo.SetCurSel(port);
	  Sleep(100);
    return r;
  }
	CloseOpenUart();
	m_serial->ResetPort(port);
	m_BaudRateCombo.SetCurSel(port);
	Sleep(100);
  return r;
}

void CGPSDlg::OnBinaryGetrgister()
{
	if(!m_isConnectOn)
	{
		AfxMessageBox("Please connect to GNSS device");
		return;
	}

	CGetRgsDlg dlg;
	INT_PTR ret = dlg.DoModal();
	if(ret == IDOK)
	{
		m_regAddress = dlg.address;
		GenericQuery(&CGPSDlg::QueryRegister);
	}
}

void CGPSDlg::OnBinaryGetrgister16()
{
	if(!m_isConnectOn)
	{
		AfxMessageBox("Please connect to GNSS device");
		return;
	}

	CGetRgsDlg dlg;
	INT_PTR ret = dlg.DoModal();
	if(ret == IDOK)
	{
		m_regAddress = dlg.address;
		GenericQuery(&CGPSDlg::QueryRegister16);
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
	return (end-start) > TIME_OUT_QUICK_MS;
}

bool CGPSDlg::CheckConnect()
{
	if(!m_isConnectOn)
	{
		AfxMessageBox("Please connect to GNSS device");
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

void CGPSDlg::OnBnClickedNoOutput()
{
	CConfigMessageOut dlg;
	DoCommonConfigDirect(&dlg, 0);

	return;
}

void CGPSDlg::OnBnClickedNmeaOutput()
{
	CConfigMessageOut dlg;
	DoCommonConfigDirect(&dlg, 1);

	return;
}

void CGPSDlg::OnBnClickedBinaryOutput()
{
	CConfigMessageOut dlg;
	DoCommonConfigDirect(&dlg, 2);

	return;
}

UINT DownloadThread(LPVOID pParam)
{
	CGPSDlg::gpsDlg->Download();
	return 0;
}

#ifdef GG12A
bool CGPSDlg::check_gg12a_format(const char *file_path)
{
	U08 check_patern[8] = {0xF4,0x0E,0xE0,0xB8,0x16,0xA0,0x01,0xA2};
	FILE *f = NULL;
	fopen_s(&f, file_path, "rb");
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
	m_saveNmeaDlg->StartSave(CSaveNmea::NMEA_Mode, fileName, fileName);
	m_saveNmeaDlg->ShowWindow(SW_SHOW);
	m_saveNmeaDlg->SetFocus();
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

	CFileDialog dlgFile(FALSE, _T("out"), fileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
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
	m_saveNmeaDlg->StartSave(CSaveNmea::Binary_Mode, fileName, fileName);
	m_saveNmeaDlg->ShowWindow(SW_SHOW);
	m_saveNmeaDlg->SetFocus();
}

void CGPSDlg::OnSaveTelitNoParsing()
{
	if(m_saveNmeaDlg)
	{
		::AfxMessageBox("Save function has been activated!");
		return;
	}
  
  if(!CheckConnect())
	{
		return;
	}
  m_serial->Close();
	SetInputMode(NoOutputMode);

	CTime t = CTime::GetCurrentTime();
	CString fileName;
	fileName.Format("Host%02d-%02d-%02d_%02d%02d%02d.dat", t.GetYear(), t.GetMonth(), t.GetDay(),
		t.GetHour(), t.GetMinute(), t.GetSecond());

	CFileDialog dlgFile(FALSE, _T("*.dat"), fileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("ALL Files (*.*)|*.*||"), this);

	dlgFile.GetOFN().lpstrFile = fileName.GetBuffer(MyMaxPath);
	dlgFile.GetOFN().nMaxFile = MyMaxPath;
	INT_PTR nResult = dlgFile.DoModal();
	fileName.ReleaseBuffer();
  if(nResult != IDOK)
  {
	  m_serial->Open(g_setting.GetComPort(), g_setting.GetBaudrateIndex());
		SetMode();
		CreateGPSThread();
    return;
  }

	SaveBinaryNoParsingDlg dlg;
  dlg.SetType(SaveBinaryNoParsingDlg::Telit);
  dlg.SetFilePath(fileName);
  dlg.SetComPort(g_setting.GetComPort());
  dlg.SetBaudRate(g_setting.GetBaudrate());
	nResult = dlg.DoModal();

	m_serial->Open(g_setting.GetComPort(), g_setting.GetBaudrateIndex());
	SetMode();
	CreateGPSThread();
}

void CGPSDlg::OnSaveBinaryNoParsing()
{
	if(m_saveNmeaDlg)
	{
		::AfxMessageBox("Save function has been activated!");
		return;
	}
  
  if(!CheckConnect())
	{
		return;
	}
  m_serial->Close();
	SetInputMode(NoOutputMode);

	CTime t = CTime::GetCurrentTime();
	CString fileName;
	fileName.Format("Host%02d-%02d-%02d_%02d%02d%02d.dat", t.GetYear(), t.GetMonth(), t.GetDay(),
		t.GetHour(), t.GetMinute(), t.GetSecond());

	CFileDialog dlgFile(FALSE, _T("*.dat"), fileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("ALL Files (*.*)|*.*||"), this);

	dlgFile.GetOFN().lpstrFile = fileName.GetBuffer(MyMaxPath);
	dlgFile.GetOFN().nMaxFile = MyMaxPath;
	INT_PTR nResult = dlgFile.DoModal();
	fileName.ReleaseBuffer();
  if(nResult != IDOK)
  {
	  m_serial->Open(g_setting.GetComPort(), g_setting.GetBaudrateIndex());
		SetMode();
		CreateGPSThread();
    return;
  }

	SaveBinaryNoParsingDlg dlg;
  dlg.SetFilePath(fileName);
  dlg.SetComPort(g_setting.GetComPort());
  dlg.SetBaudRate(g_setting.GetBaudrate());
	nResult = dlg.DoModal();

	m_serial->Open(g_setting.GetComPort(), g_setting.GetBaudrateIndex());
	SetMode();
	CreateGPSThread();
}

void CGPSDlg::OnVerifyFirmware()
{
	if(!CheckConnect())
	{
		return;
	}

	SetInputMode(NoOutputMode);

	SaveBinaryNoParsingDlg dlg;
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

void CGPSDlg::OnUbloxBinaryOutputConvert()
{
	CRawMeasmentOutputConvertDlg dlg;
  dlg.SetMode(CRawMeasmentOutputConvertDlg::UbloxBinary);
	dlg.DoModal();
}

void CGPSDlg::OnHosLogToNmea()
{
	CRawMeasmentOutputConvertDlg dlg;
  dlg.SetMode(CRawMeasmentOutputConvertDlg::HostLog);
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
	WaitForSingleObject(hScanGPS, INFINITE);
	DeleteNmeaMemery();

	for(int c=0; c<30; ++c)
	{
		for(int i=0; i<g_setting.GetBaudrateTableSize(); ++i)
		{
			if(m_pScanDlg->IsFinish)
			{
				return;
			}
			CString strMsg;
			strMsg.Format("Scanning COM%d Baudrate %d", c + 1, g_setting.BaudrateValue(i));
			m_pScanDlg->m_msg.SetWindowText(strMsg);

			m_serial = new CSerial;
			m_BaudRateCombo.SetCurSel(i);
			m_ComPortCombo.SetCurSel(c);
			if(!m_serial->Open(c + 1, i))
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
	SetInputMode(NoOutputMode);
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
	SetMsgType(GetMsgType());
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
		g_setting.SetComPortIndex(m_ComPortCombo.GetCurSel());
		g_setting.SetBaudrateIndex(m_BaudRateCombo.GetCurSel());

		CreateGPSThread();
		m_inputMode = GetMsgType();
		SetConnectTitle(true);
		g_setting.Save();

		m_isPressCloseButton = false;
		m_gpgsvMsg.NumOfSate = 0;
		m_gpgsvMsg.NumOfMessage = 0;
		m_glgsvMsg.NumOfSate = 0;
		m_glgsvMsg.NumOfMessage = 0;
		m_nmeaList.DeleteAllItems();
		g_scatterData.ClearData();
		m_ttffCount = 0;
		SetTTFF(0);
		return true;
	}
	return false;
}

void CGPSDlg::ScanGPS1()
{
	WaitForSingleObject(hScanGPS, INFINITE);
	DeleteNmeaMemery();
	int b = m_BaudRateCombo.GetCurSel();
	for(int c = 0; c < 30; ++c)
	{
		if(m_pScanDlg->IsFinish)
			return;
		CString strMsg;
		strMsg.Format("Scanning COM%d Baudrate %d", c + 1, g_setting.BaudrateValue(b));
		m_pScanDlg->m_msg.SetWindowText(strMsg);

		m_serial = new CSerial;
		m_BaudRateCombo.SetCurSel(b);	
		m_ComPortCombo.SetCurSel(c);
		if(!m_serial->Open(c + 1, b))
		{
			delete m_serial;
			m_serial = NULL;
			continue;
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
	WaitForSingleObject(hScanGPS, INFINITE);
	DeleteNmeaMemery();
	int c = m_ComPortCombo.GetCurSel();
	for(int i = 0; i < g_setting.GetBaudrateTableSize(); ++i)
	{
		if(m_pScanDlg->IsFinish)
		{
			return;
		}
		CString strMsg;
		strMsg.Format("Scanning COM%d in baudrate %d", c + 1, g_setting.BaudrateValue(i));
		m_pScanDlg->m_msg.SetWindowText(strMsg);

		m_serial = new CSerial;
		m_BaudRateCombo.SetCurSel(i);	
		m_ComPortCombo.SetCurSel(c);
		if(!m_serial->Open(c + 1, i))
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

void CGPSDlg::OnBnClickedScanAll()
{
	if(m_isConnectOn) 
	{
		OnBnClickedClose();
	}
	AfxBeginThread(ShowScanThread, 0);
	AfxBeginThread(ScanGPSThread, 0);
}

void CGPSDlg::OnBnClickedScanPort()
{
	if(m_isConnectOn) 
	{
		OnBnClickedClose();
	}

	AfxBeginThread(ShowScanThread, 0);
	AfxBeginThread(ScanGPS1Thread, 0);
}

void CGPSDlg::OnBnClickedScanBaudrate()
{
	if(m_isConnectOn) 
	{
		OnBnClickedClose();
	}
	AfxBeginThread(ShowScanThread, 0);
	AfxBeginThread(ScanGPS2Thread, 0);
}

void CGPSDlg::SetBaudrate(int b)
{
	CloseOpenUart();
	m_serial->ResetPort(b);
	m_BaudRateCombo.SetCurSel(b);
	g_setting.SetBaudrateIndex(b);
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
	SetInputMode(NoOutputMode);
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
			messages[0] = (U08)0xa0;
			messages[1] = (U08)0xa1;
			messages[2] = 0;
			messages[3] = 87;
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
			U16 SVID = messages[6] & 0xff;
			sprintf_s(m_nmeaBuffer, "Set SV#%d ephemeris successfully", SVID);
			if(!SendToTargetNoWait(messages, 94, m_nmeaBuffer))
			{
				sprintf_s(m_nmeaBuffer, "Set SV#%d ephemeris failed...", SVID);
				add_msgtolist(m_nmeaBuffer);
			}
		}
	}
	else
	{
		AfxMessageBox("File of the ephemeris data format wrong!");
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

	SetInputMode(NoOutputMode);
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

	if(SendToTarget(cmd.GetBuffer(), cmd.Size(), "", true))
	{
		if(g_waitReadDialog == NULL )
		{
			g_waitReadDialog = new CWaitReadLog;
		}
		AfxBeginThread(WaitLogRead, 0);
		WaitForSingleObject(waitlog, INFINITE);
		g_waitReadDialog->SetWindowText("Wait for get ephemeris");
		g_waitReadDialog->msg.SetWindowText("Please wait for get ephemeris!");

		U08 NumsOfEphemeris = 0;
		int wait = 0;
		while(1)
		{
			wait++;
			if(wait == 50)
			{
				Sleep(500);
				g_waitReadDialog->msg.SetWindowText("Retrieve GPS Ephemeris data is Failed!");
				Sleep(500);
				g_waitReadDialog->IsFinish = true;
				add_msgtolist("Retrieve GPS Ephemeris Failed...");
				goto TheLast;
			}

			U08 buff[1024] = {0};
			DWORD nSize = m_serial->GetBinaryBlock(buff, sizeof(buff), EphmsRecordSize);
			if(SaveEphemeris(buff, BINMSG_GET_EPHEMERIS))
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
				g_waitReadDialog->msg.SetWindowText(txtMsg);

				if(NumsOfEphemeris==32)
				{
					break;
				}
			}
		}
		g_waitReadDialog->msg.SetWindowText("Retrieve GPS Ephemeris data is completed!");
		Sleep(500);
		g_waitReadDialog->IsFinish = true;
		add_msgtolist("Retrieve GPS Ephemeris successfully");

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
	int wait = 0;
	char BINMSG[1024] = {0};
	if(SendToTarget(binMsg, sizeof(binMsg), "", true))
	{
		if(g_waitReadDialog == NULL )
		{
			g_waitReadDialog = new CWaitReadLog;
		}
		AfxBeginThread(WaitLogRead, 0);
		WaitForSingleObject(waitlog, INFINITE);
		g_waitReadDialog->SetWindowText("Wait for get Glonass ephemeris");
		g_waitReadDialog->msg.SetWindowText("Please wait for get Glonass ephemeris!");

		U08 NumsOfEphemeris = 0;
		while(1)
		{
			wait++;
			if(wait == 50)
			{
				Sleep(500);
				g_waitReadDialog->msg.SetWindowText("Retrieve Glonass Ephemeris data is Failed!");
				Sleep(500);
				g_waitReadDialog->IsFinish = true;
				add_msgtolist("Retrieve Glonass Ephemeris Failed...");

				goto TheLast;
			}
			U08 buff[1024] = {0};
			m_serial->GetBinaryBlock(buff, sizeof(buff), EphmsRecordSize);
			if(SaveEphemeris(buff, 0x90))
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
				g_waitReadDialog->msg.SetWindowText(BINMSG);

				if(NumsOfEphemeris==24)
				{
					break;
				}
			}
		}
		g_waitReadDialog->msg.SetWindowText("Retrieve Glonass Ephemeris data is completed!");
		Sleep(500);
		g_waitReadDialog->IsFinish = true;
		add_msgtolist("Retrieve Glonass Ephemeris successfully");
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

	if(SendToTarget(cmd.GetBuffer(), cmd.Size(), "Get Beidou2 ephemeris start...", true))
	{
		if(g_waitReadDialog == NULL )
		{
			g_waitReadDialog = new CWaitReadLog;
		}
		AfxBeginThread(WaitLogRead, 0);
		WaitForSingleObject(waitlog, INFINITE);
		g_waitReadDialog->SetWindowText("Waiting for get Beidou2 ephemeris...");
		g_waitReadDialog->msg.SetWindowText("Please wait for the Beidou2 ephemeris.");

		U08 NumsOfEphemeris = 0;
		int wait = 0;
		while(1)
		{
			wait++;
			if(wait == 50)
			{
				Sleep(500);
				g_waitReadDialog->msg.SetWindowText("Retrieving the Beidou2 ephemeris data fails!");
				Sleep(500);
				g_waitReadDialog->IsFinish = true;
				add_msgtolist("Retrieving the Beidou2 ephemeris fails!");
				goto TheLast;
			}

			U08 buff[1024] = {0};
			DWORD nSize = m_serial->GetBinaryAck(buff, sizeof(buff));
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
				txtMsg.Format("Retrieving thr Beidou2 Satellite ID # %d ephemeris.",NumsOfEphemeris);
				g_waitReadDialog->msg.SetWindowText(txtMsg);

				if(NumsOfEphemeris==37)
				{
					break;
				}
			}
		}

		g_waitReadDialog->msg.SetWindowText("Retrieving the Beidou2 ephemeris data complete.");
		Sleep(500);
		g_waitReadDialog->IsFinish = true;
		add_msgtolist("Retrieving thr Beidou2 ephemeris successfully.");

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

void CGPSDlg::LoadMenu()
{
	HMENU hMenu = CreateMenu();

	//File Menu
	static MenuItemEntry menuItemFile[] =
	{
		{ !NMEA_INPUT && !SHOW_PATCH_MENU, MF_STRING, ID_FILE_SAVENMEA, "&Save Message", NULL },
		{ !NMEA_INPUT && !SHOW_PATCH_MENU, MF_STRING, ID_FILE_BINARY, "Save &Device Output", NULL },
    //{ !NMEA_INPUT, MF_STRING, ID_HOSTLOG, "Save &Host Log", NULL },
		{ !NMEA_INPUT && !SHOW_PATCH_MENU, MF_STRING, ID_SAVE_BINARY_NOPARSING, "Save HostLog without Parsing", NULL },
		{ 1 && !NMEA_INPUT && !SHOW_PATCH_MENU, MF_STRING, ID_SAVE_TELIT_NOPARSING, "Save DR Debug HostLog", NULL },
		{ 1, MF_STRING, ID_FILE_CLEANNEMA, "&Clear Message Screen", NULL },
		{ NMEA_INPUT, MF_STRING, ID_FILE_PLAYNMEA, "&Play NMEA", NULL },
		//{ IS_DEBUG, MF_STRING, ID_VERIFY_FIRMWARE, "&Verify Firmware", NULL },
		{ 1, MF_SEPARATOR, 0, NULL, NULL },
		{ UPGRADE_DOWNLOAD, MF_STRING, ID_UPGRADE_DOWNLOAD, "Upgrade", NULL },
		{ SHOW_PATCH_MENU, MF_STRING, ID_PATCH, "Patch!", NULL },

		{ !NMEA_INPUT && !SHOW_PATCH_MENU, MF_STRING, ID_GPSDO_FW_DOWNLOAD, "Master/Slave Firmware Download", NULL },
		{ 1, MF_SEPARATOR, 0, NULL, NULL },
		{ !SHOW_PATCH_MENU, MF_STRING, ID_FILE_SETUP, "S&etup", NULL },
		{ 1, MF_STRING, ID_FILE_EXIT, "E&xit", NULL },
		{ 0, 0, 0, NULL, NULL }	//End of table
	};
	CreateSubMenu(hMenu, menuItemFile, "&File");

	//Binary Menu
	static MenuItemEntry SetFactoryDefaultMenu[] =
	{
		{ 1, MF_STRING, ID_FCTRY_DFLT_NO_RBT, "No Reboot", NULL },
		{ 1, MF_STRING, ID_FCTRY_DFLT_RBT, "Reboot after setting to factory defaults", NULL },
		{ 0, 0, 0, NULL, NULL }	//End of table
	};

	static MenuItemEntry menuItemBinary[] =
	{
		{ 1, MF_STRING, ID_SYSTEM_RESTART, "System Restart", NULL },
		{ SHOW_BINARY_DATA, MF_STRING, ID_BINARY_DUMP_DATA, "Show Binary Data", NULL },
		{ 1, MF_POPUP, 0, "Set Factory Default", SetFactoryDefaultMenu },
		{ IS_DEBUG, MF_STRING, ID_FIRMWARE_DOWNLOAD, "Firmware Image Download", NULL },
    { 1, MF_STRING, ID_QUERY_CLOCK_OFFSET, "Query Clock Offset", NULL },
    { 1, MF_STRING, ID_SET_CLOCK_OFFSET, "Set Default Clock Offset", NULL },
		{ 1, MF_SEPARATOR, 0,NULL,NULL },
		{ 1, MF_STRING, ID_QUERY_SW_VERSION, "Query Software Version", NULL },
		{ 1, MF_STRING, ID_QUERY_SW_CRC, "Query CRC Checksum", NULL },
		{ 1, MF_STRING, ID_QUERY_SHA1, "Query SHA1 String", NULL },
		{ IS_DEBUG, MF_STRING, ID_QUERY_CON_CAP, "Query GNSS Constellation Capability", NULL },
		{ 1, MF_STRING, ID_QUERY_NMEA_INTERVAL_V8, "Query NMEA Message Interval", NULL },
		{ (CUSTOMER_ID == Ericsson), MF_STRING, ID_QUERY_ERICSSON_STC_ITV, "Query Ericsson Sentence Interval", NULL },
		{ (CUSTOMER_ID == OlinkStar), MF_STRING, ID_QUERY_SERIAL_NUMBER, "Query Serial Number", NULL },

		{ 1, MF_STRING, ID_QUERY_POSITION_UPDATE_RATE, "Query Position Update Rate", NULL },
		//Remove in 20160516, Already has Query Datum Index in Venus 8, Request from Andrew
		//{ 0, MF_STRING, ID_QUERY_DATUM, "Query Datum", NULL },
		{ 1, MF_STRING, ID_QUERY_POSITION_PINNING, "Query Position Pinning", NULL },
    //Remove in 20160422, V8 doesn't need this cmd, Request from Andrew
		//{ 0, MF_STRING, ID_QUERY_GPS_MSRMNT_MODE, "Query GPS Measurement Mode", NULL },	
		{ 1, MF_STRING, ID_QUERY_POWER_MODE, "Query Power Mode", NULL },//
		{ IS_DEBUG, MF_STRING, ID_QUERY_V8_POWER_SV_PARAM, "Query Power Saving Parameters", NULL },//
		{ IS_DEBUG, MF_STRING, ID_QUERY_PROPRIETARY_MSG, "Query Proprietary Message", NULL },//
		{ 1, MF_STRING, ID_QUERY_DOP_MASK, "Query DOP Mask", NULL },//
		{ 1, MF_STRING, ID_QUERY_ELE_CNR_MSK, "Query Elevation and CNR Mask", NULL },//
		{ IS_DEBUG, MF_STRING, ID_QUERY_ANTENNA_DETECTION, "Query Antenna Detection", NULL },//
		{ IS_DEBUG, MF_STRING, ID_QUERY_NOISE_POWER, "Query Noise Power", NULL },//
		{ SOFTWARE_FUNCTION & SW_FUN_DR, MF_STRING, ID_QUERY_DR_INFO, "Query DR Info", NULL },//
		{ SOFTWARE_FUNCTION & SW_FUN_DR, MF_STRING, ID_QUERY_DR_HW_PARAMETER, "Query DR HW Parameter", NULL },//
		{ 1, MF_STRING, ID_QUERY_GNSS_KNUM_SLT_CNR, "Query GLONASS K-Number, Slot, CNR", NULL },
		{ 1, MF_STRING, ID_QUERY_NMEA_TALKER_ID, "Query NMEA Talker ID", NULL },

		{ IS_DEBUG, MF_STRING, ID_GET_RGISTER, "Get Register", NULL },
		{ IS_DEBUG, MF_STRING, ID_GET_RGISTER16, "Get 16-IO Register", NULL },
		{ 1, MF_SEPARATOR, 0,NULL,NULL },
		{ 1, MF_STRING, ID_CFG_SERIAL_PORT, "Configure Serial Port", NULL },
		{ 1, MF_STRING, ID_CFG_NMEA_INTERVAL_V8, "Configure NMEA Message Interval", NULL },
		{ (CUSTOMER_ID == Ericsson), MF_STRING, ID_CONFIG_ERICSSON_STC_ITV, "Configure Ericsson Sentence Interval", NULL },
		{ (CUSTOMER_ID == OlinkStar), MF_STRING, ID_CONFIG_SERIAL_NUMBER, "Set Serial Number", NULL },

		{ 1, MF_STRING, ID_CFG_MESSAGE_TYPE, "Configure Message Type", NULL },
		{ 1, MF_STRING, ID_CFG_BIN_ITV, "Configure Binary Message Interval", NULL },
		{ IS_DEBUG, MF_STRING, ID_CFG_MULTI_PATH, "Configure Multi-path", NULL },	//
		{ 1, MF_STRING, ID_CFG_POSITION_UPDATE_RATE, "Configure Position Update Rate", NULL },
		//Remove in 20160516, Already has Configure Datum Index in Venus 8, request from Andrew
		//{ 0, MF_STRING, ID_CFG_DATUM, "Configure Datum", NULL },
		{ 1, MF_STRING, ID_CFG_PST_PING, "Configure Position Pinning", NULL },
		{ 1, MF_STRING, ID_CFG_PING_PRMTR, "Configure Pinning Parameters", NULL },
    //Remove in 20160422, V8 doesn't need this cmd
		//{ 0, MF_STRING, ID_CFG_GPS_MEAS_MODE, "Configure GPS Measurement Mode", NULL },	
		{ 1, MF_STRING, ID_CFG_POWER_MODE, "Configure Power Mode", NULL },
		{ IS_DEBUG, MF_STRING, ID_CFG_PWR_SAV_PAR, "Configure Power Saving Parameters", NULL },
		{ IS_DEBUG, MF_STRING, ID_CFG_PROPRIETARY_MESSAGE, "Configure Proprietary Message", NULL },
		{ 1, MF_STRING, ID_CFG_DOP_MASK, "Configure DOP Mask", NULL },
		{ 1, MF_STRING, ID_CONFIG_ELEV_AND_CNR_MASK, "Configure Elevation and CNR Mask", NULL },
		{ IS_DEBUG, MF_STRING, ID_CFG_ANTENNA_DETECTION, "Configure Antenna Detection", NULL },
		{ IS_DEBUG, MF_STRING, ID_CFG_SUBSEC_REG, "Configure SubSec Register", NULL },
		{ 1, MF_STRING, ID_CFG_NMEA_OUTPUT_COM, "Configure NMEA Output Comport", NULL },
		{ 1, MF_STRING, ID_CFG_NMEA_TALKER_ID, "Configure NMEA Talker ID", NULL },

		{ IS_DEBUG, MF_STRING, ID_CFG_REGISTER, "Configure Register", NULL },
		{ IS_DEBUG, MF_STRING, ID_CFG_REGISTER16, "Configure 16-IO Register", NULL },
		{ ODOMETER_SUPPORT, MF_STRING, ID_RESET_ODOMETER, "Reset Odometer", NULL },

		{ 0, 0, 0, NULL, NULL }	//End of table
	};
	if(!NMEA_INPUT)
	{
		CreateSubMenu(hMenu, menuItemBinary, "&Binary");
	}
	
#ifdef SWCFG_VENDOR_NSHP_FIX_TOOL
	::SetMenu(this->m_hWnd, hMenu);
	return;
#endif

	static MenuItemEntry GpsdoControlMenu[] =
	{
		//{ 1, MF_STRING, ID_GPSDO_ENTER_DWN_H, "High-Speed Slave Download(Master Only)!", NULL },
		{ 1, MF_STRING, ID_QUERY_UARTPASS, "Query UART Pass Through Status", NULL },
		{ 1, MF_STRING, ID_GPSDO_RESET_SLAVE, "Reset Slave MCU(Master Only)", NULL },
		{ 1, MF_SEPARATOR, 0, NULL, NULL },
		{ 1, MF_STRING, ID_GPSDO_ENTER_ROM, "Enter Slave ROM Download(Master Only)", NULL },
		{ 1, MF_STRING, ID_GPSDO_LEAVE_ROM, "Back To Normal Mode from ROM Download", NULL },
		{ 1, MF_SEPARATOR, 0, NULL, NULL },
		{ 1, MF_STRING, ID_GPSDO_ENTER_DWN, "Enter Slave Download(Master Only)", NULL },
		{ 1, MF_STRING, ID_GPSDO_LEAVE_DWN, "Back To Normal Mode from Slave Download", NULL },
		//{ 1, MF_SEPARATOR, 0, NULL, NULL },
		//{ 1, MF_STRING, ID_GPSDO_LEAVE_DWN_H, "Back To Normal Mode from Slave Download", NULL },
		{ 1, MF_SEPARATOR, 0, NULL, NULL },
		{ 1, MF_STRING, ID_GPSDO_ENTER_UART, "Enter Slave UART Pass Through(Master Only)", NULL },
		{ 1, MF_STRING, ID_GPSDO_LEAVE_UART, "Back To Normal Mode from UART Pass through", NULL },
		{ 1, MF_SEPARATOR, 0, NULL, NULL },
		{ 0, 0, 0, NULL, NULL }	//End of table
	};
	static MenuItemEntry Sup800Menu[] =
	{
  	{ 1, MF_STRING, ID_SUP800_ERASE_DATA, "SUP800 Erase User Data", NULL },
		{ 1, MF_STRING, ID_SUP800_WRITE_DATA, "SUP800 Write User Data", NULL },
		{ 1, MF_STRING, ID_SUP800_READ_DATA, "SUP800 Read User Data", NULL },
		{ 0, 0, 0, NULL, NULL }	//End of table
	};

	static MenuItemEntry GeoFencingConfigureMenu[] =
	{
		{ 1, MF_STRING, ID_CONFIG_GEOFENCE1, "Geo-fencing data NO.1", NULL },
		{ 1, MF_STRING, ID_CONFIG_GEOFENCE2, "Geo-fencing data NO.2", NULL },
		{ 1, MF_STRING, ID_CONFIG_GEOFENCE3, "Geo-fencing data NO.3", NULL },
		{ 1, MF_STRING, ID_CONFIG_GEOFENCE4, "Geo-fencing data NO.4", NULL },
		{ 0, 0, 0, NULL, NULL }	//End of table
	};

	static MenuItemEntry GeoFencingQueryMenu[] =
	{
		{ 1, MF_STRING, ID_QUERY_GEOFENCE1, "Geo-fencing data NO.1", NULL },
		{ 1, MF_STRING, ID_QUERY_GEOFENCE2, "Geo-fencing data NO.2", NULL },
		{ 1, MF_STRING, ID_QUERY_GEOFENCE3, "Geo-fencing data NO.3", NULL },
		{ 1, MF_STRING, ID_QUERY_GEOFENCE4, "Geo-fencing data NO.4", NULL },
		{ 0, 0, 0, NULL, NULL }	//End of table
	};

	static MenuItemEntry GeoFencingMenu[] =
	{
#if (GEO_FENCING_CMD==0)
		{ 1, MF_STRING, ID_CONFIG_GEOFENCE, "Configure geo-fencing data", NULL },
		{ 1, MF_STRING, ID_QUERY_GEOFENCE, "Query geo-fencing data", NULL },
		{ 1, MF_STRING, ID_QUERY_GEOFENCE_RESULT, "Query geo-fencing result ", NULL },
#else
		{ 1, MF_POPUP, 0, "Configure geo-fencing data", GeoFencingConfigureMenu },
		{ 1, MF_POPUP, 0, "Query geo-fencing data", GeoFencingQueryMenu },
		{ 1, MF_STRING, ID_QUERY_GEOFENCE_RESULTEX, "Query geo-fencing result ", NULL },
#endif
		{ 0, 0, 0, NULL, NULL }	//End of table
	};

	static MenuItemEntry SignalDisturbanceMenu[] =
	{
		{ 1, MF_STRING, ID_QUERY_SIG_DISTUR_DATA, "Query Signal Disturbance Data", NULL },
		{ 1, MF_STRING, ID_QUERY_SIG_DISTUR_STATUS, "Query Signal Disturbance Status", NULL },
		{ 1, MF_STRING, ID_CONFIG_SIG_DISTUR_STATUS, "Configure Signal Disturbance Status", NULL },
		{ 0, 0, 0, NULL, NULL }	//End of table
	};

	static MenuItemEntry QueryPstiInterval[] =
	{
		{ 1, MF_STRING, ID_QUERY_PSTI004, "Query PSTI004 Interval", NULL },
		{ 1, MF_STRING, ID_QUERY_PSTI030, "Query PSTI030 Interval", NULL },
		{ 1, MF_STRING, ID_QUERY_PSTI032, "Query PSTI032 Interval", NULL },
		{ 1, MF_STRING, ID_QUERY_PSTI033, "Query PSTI033 Interval", NULL },
		{ 1, MF_STRING, ID_QUERY_PSTI063, "Query PSTI063 Interval", NULL },
		{ 1, MF_STRING, ID_QUERY_PSTI065, "Query PSTI065 Interval", NULL },
		{ 1, MF_STRING, ID_QUERY_PSTI067, "Query PSTI067 Interval", NULL },
		{ 1, MF_STRING, ID_QUERY_PSTI068, "Query PSTI068 Interval", NULL },
		{ 1, MF_STRING, ID_QUERY_PSTI070, "Query PSTI070 Interval", NULL },
		{ 0, 0, 0, NULL, NULL }	//End of table
	};

	static MenuItemEntry ConfigPstiInterval[] =
	{
		{ 1, MF_STRING, ID_CONFIG_PSTI004, "Configure PSTI004 Interval", NULL },
  	{ 1, MF_STRING, ID_CONFIG_PSTI030, "Configure PSTI030 Interval", NULL },
		{ 1, MF_STRING, ID_CONFIG_PSTI032, "Configure PSTI032 Interval", NULL },
		{ 1, MF_STRING, ID_CONFIG_PSTI033, "Configure PSTI033 Interval", NULL },
		{ 1, MF_STRING, ID_CONFIG_PSTI063, "Configure PSTI063 Interval", NULL },
		{ 1, MF_STRING, ID_CONFIG_PSTI065, "Configure PSTI065 Interval", NULL },
		{ 1, MF_STRING, ID_CONFIG_PSTI067, "Configure PSTI067 Interval", NULL },
		{ 1, MF_STRING, ID_CONFIG_PSTI068, "Configure PSTI068 Interval", NULL },
		{ 1, MF_STRING, ID_CONFIG_PSTI070, "Configure PSTI070 Interval", NULL },
		{ 0, 0, 0, NULL, NULL }	//End of table
	};

	//Venus 8 Menu
	static MenuItemEntry menuItemVenus8[] =
	{
		{ 1, MF_STRING, ID_QUERY_BOOT_STATUS, "GNSS ROM Boot Status", NULL },
		{ IS_DEBUG, MF_STRING, ID_QUERY_CUSTOMER_ID, "Query Customer ID", NULL },
		{ IS_DEBUG, MF_STRING, ID_CONFIG_DOZE_MODE, "Configure GNSS Doze Mode", NULL },
		//20160408 Add GPSDO menu to customer release version for Patrick's customer.
		{ !LITEON_CUSTOMIZE, MF_POPUP, 0, "GPSDO Control", GpsdoControlMenu },
		{ !LITEON_CUSTOMIZE, MF_POPUP, 0, "SUP800 User Data Storage", Sup800Menu },
		{ IS_DEBUG && !LITEON_CUSTOMIZE, MF_POPUP, 0, "Geofencing", GeoFencingMenu },
		{ IS_DEBUG , MF_POPUP, 0, "Signal Disturbance Test", SignalDisturbanceMenu },
		{ IS_DEBUG, MF_STRING, ID_QUERY_SBAS_DEFAULT, "Query SBAS Default", NULL },
		{ 1, MF_SEPARATOR, 0, NULL, NULL },
		{ 1, MF_STRING, ID_QUERY_SBAS, "Query SBAS", NULL },
		{ 1, MF_STRING, ID_QUERY_SBAS2, "Query SBAS Advance", NULL },
		{ 1, MF_STRING, ID_QUERY_SAEE, "Query SAEE", NULL },
		{ 1, MF_STRING, ID_QUERY_QZSS, "Query QZSS", NULL },
		{ 1, MF_STRING, ID_QUERY_DGPS, "Query DGPS", NULL },
		{ IS_DEBUG, MF_STRING, ID_QUERY_SMOOTH_MODE, "Query Carrier Smooth Mode", NULL },
		{ IS_DEBUG, MF_STRING, ID_QUERY_TIME_STAMPING, "Query Time Stamping", NULL },
		{ 1, MF_STRING, ID_QUERY_NOISE_PW_CTL, "Query Noise Power Control", NULL },
		{ 1, MF_STRING, ID_QUERY_ITRFRC_DTCT_CTRL, "Query Interference Detect Control", NULL },
		{ IS_DEBUG, MF_STRING, ID_QUERY_NMBI_OUT_DES, "Query NMEA/Binary Output Destination", NULL },
		{ 1, MF_STRING, ID_QUERY_PARAM_SEARCH_ENG_NUM, "Query Parameter Search Engine Number", NULL },
		{ 1, MF_STRING, ID_QUERY_POS_FIX_NAV_MASK, "Query Position Fix Navigation Mask", NULL },
		{ IS_DEBUG, MF_STRING, ID_QUERY_REF_TIME_TO_GPS, "Query Ref Time Sync To GPS Time", NULL },
		{ 1, MF_STRING, ID_QUERY_NAV_MODE_V8, "Query Navigation Mode", NULL },
		{ 1, MF_STRING, ID_QUERY_GNSS_NAV_SOL, "Query GNSS Constellation Type", NULL },
		{ 1, MF_STRING, ID_QUERY_GPS_TIME, "Query GPS Time", NULL },
		//20150520 Remove this command from Andrew's request
		//{ IS_DEBUG, MF_STRING, ID_QUERY_V8_POWER_SV_PARAM_ROM, "Query Power Saving Parameters(Rom)", NULL },//
		{ 1, MF_STRING, ID_QUERY_PARAM_SRCH_ENG_SLP_CRT, "Query Parameter Search Engine Sleep Criteria", NULL },
		{ 1, MF_STRING, ID_QUERY_DATUM_INDEX, "Query Datum Index", NULL },
		{ 1, MF_STRING, ID_QUERY_VERY_LOW, "Query Kernel Very Low Speed", NULL },
		{ 1, MF_POPUP, 0, "Query PSTI Interval", QueryPstiInterval },

		{ 1, MF_SEPARATOR, 0, NULL, NULL },
		{ 1, MF_STRING, ID_CFG_SBAS, "Configure SBAS", NULL },
		{ 1, MF_STRING, ID_CFG_SBAS2, "Configure SBAS Advance", NULL },
		{ 1, MF_STRING, ID_CFG_SAEE, "Configure SAEE", NULL },
		{ 1, MF_STRING, ID_CFG_QZSS, "Configure QZSS", NULL },
		{ 1, MF_STRING, ID_CFG_DGPS, "Configure DGPS", NULL },
		{ IS_DEBUG, MF_STRING, ID_CFG_SMOOTH_MODE, "Configure Carrier Smooth Mode", NULL },
		{ IS_DEBUG, MF_STRING, ID_CFG_TIME_STAMPING, "Configure Time Stamping", NULL },
		{ 1, MF_STRING, ID_CFG_NOISE_PWR_CTRL, "Configure Noise Power Control", NULL },
		{ 1, MF_STRING, ID_CFG_ITRFRC_DTCT_CTRL, "Configure Interference Detect Control", NULL },
		{ IS_DEBUG, MF_STRING, ID_CFG_NMBI_OUT_DES, "Configure NMEA/Binary Output Destination", NULL },
		{ 1, MF_STRING, ID_CFG_PARAM_SEARCH_ENG_NUM, "Configure Parameter Search Engine Number", NULL },
		{ 1, MF_STRING, ID_CFG_POS_FIX_NAV_MASK, "Configure Position Fix Navigation Mask", NULL },
		{ IS_DEBUG, MF_STRING, ID_CONFIG_REF_TIME_TO_GPS, "Configure Ref Time Sync To GPS Time", NULL },
		{ 1, MF_STRING, ID_CFG_NVGTN_MODE, "Configure Navigation Mode", NULL },
		{ 1, MF_STRING, ID_CONFIG_GNSS_NAV_SOL, "Configure GNSS Constellation Type", NULL },
		{ 1, MF_STRING, ID_CONFIG_LEAP_SECONDS, "Configure GPS/UTC Leap Seconds", NULL },
		//20150520 Remove this command from Andrew's request
		//{ IS_DEBUG, MF_STRING, ID_CONFIG_V8_POWER_SV_PARAM_ROM, "Configure Power Saving Parameters(Rom)", NULL },
		{ 1, MF_STRING, ID_CFG_PARAM_SRCH_ENG_SLP_CRT, "Configure Parameter Search Engine Sleep Criteria", NULL },
		{ 1, MF_STRING, ID_CONFIG_DATUM_INDEX, "Configure Datum Index", NULL },
		{ 1, MF_STRING, ID_CONFIG_VERY_LOW, "Configure Kernel Very Low Speed", NULL },
		{ 1, MF_POPUP, 0, "Configure PSTI Interval", ConfigPstiInterval },

		{ IS_DEBUG, MF_SEPARATOR, 0, NULL, NULL },
		{ IS_DEBUG, MF_STRING, ID_CONFIG_GPS_LEAP_IN_UTC, "Configure GPS/UTC Leap Seconds In UTC", NULL },
		{ IS_DEBUG, MF_STRING, ID_CLOCK_OFFSET_PREDICT, "Clock Offset Predict(New)", NULL },
		//{ IS_DEBUG, MF_STRING, ID_CLOCK_OFFSET_PREDICT_OLD, "Clock Offset Predict(Old)", NULL },
		{ IS_DEBUG, MF_STRING, ID_HOSTBASED_DOWNLOAD, "Host-Based Image Download", NULL },
		{ IS_DEBUG, MF_STRING, ID_PARALLEL_DOWNLOAD, "Parallel Image Download", NULL },
		{ 0, 0, 0, NULL, NULL }	//End of table
	};
	if(!NMEA_INPUT && !SHOW_PATCH_MENU)
	{
		CreateSubMenu(hMenu, menuItemVenus8, "&Venus 8");
	}

	static MenuItemEntry menuItemRaw[] =
	{
		{ 1, MF_STRING, ID_QUERY_BASE_POSITION, "Query Base Position", NULL },
		{ 1, MF_STRING, ID_QUERY_BIN_MEA_DAT_OUT, "Query Binary Measurement Data Out", NULL },
    //2016/11/24, Add from Ryan request.
		{ 1, MF_STRING, ID_QUERY_RTCM_MEA_DAT_OUT, "Query RTCM Measurement Data Out", NULL },
		{ 1, MF_SEPARATOR, 0,NULL,NULL },
		{ 1, MF_STRING, ID_CONFIG_BASE_POSITION, "Configure Base Position", NULL },
		//2014/03/11, Oliver remove this command in raw measurment version.
		//2014/05/12, Added by customer request.
		{ 1, MF_STRING, ID_CONFIG_BIN_MEA_DAT_OUT, "Configure Binary Measurement Data Out", NULL },
    //2016/11/24, Add from Ryan request.
		{ 1, MF_STRING, ID_CONFIG_RTCM_MEA_DAT_OUT, "Configure RTCM Measurement Data Out", NULL },
		{ 0, 0, 0, NULL, NULL }	//End of table
	};
	if(!NMEA_INPUT && RTK_MENU && !SHOW_PATCH_MENU)
	{
		CreateSubMenu(hMenu, menuItemRaw, "&RAW");
	}

	static MenuItemEntry RtkPassThroughMenu[] =
	{
		{ 1, MF_STRING, ID_GPSDO_ENTER_UART, "Enter Slave UART Pass Through(Master Only)", NULL },
		{ 1, MF_STRING, ID_GPSDO_LEAVE_UART, "Back To Normal Mode from UART Pass through", NULL },
		{ IS_DEBUG, MF_SEPARATOR, 0, NULL, NULL },
		{ IS_DEBUG, MF_STRING, ID_INSDR_ENTER_UART, "Enter UART Pass Through for Slave ROM Code(New Architecture)", NULL },
		{ IS_DEBUG, MF_STRING, ID_INSDR_ENTER_DWN, "Enter UART Pass Through for Slave Flash Code(New Architecture)", NULL },
		{ IS_DEBUG, MF_STRING, ID_INSDR_LEAVE_UART, "Back To Master(New Architecture)", NULL },

		{ 0, 0, 0, NULL, NULL }	//End of table
	};

	static MenuItemEntry RtkDebugModeMenu[] =
	{
		{ 1, MF_STRING, ID_ENTER_RTK_DEBUG_MODE, "Enter RTK Debug Mode", NULL },
		{ 1, MF_STRING, ID_BACK_RTK_DEBUG_MODE, "Back from RTK Debug Mode", NULL },
		{ 0, 0, 0, NULL, NULL }	//End of table
	};

  static MenuItemEntry menuItemRtk[] =
	{
		{ IS_DEBUG, MF_STRING, ID_RTK_RESET, "Reset RTK engine", NULL },
		{ 1, MF_STRING, ID_RECALC_GLONASS_IFB, "Re-calculate GLONASS IFB", NULL },
		{ 1, MF_POPUP, 0, "RTK Pass-Through", RtkPassThroughMenu },
		{ IS_DEBUG || LITEON_CUSTOMIZE, MF_POPUP, 0, "RTK Debug Mode", RtkDebugModeMenu },

		{ IS_DEBUG, MF_STRING, ID_RTK_ONOFF_GP_SV, "Enable/Disable GPS SV mechanism", NULL },
		{ IS_DEBUG, MF_STRING, ID_RTK_ONOFF_SQ_SV, "Enable/Disable SBAS + QZSS SV mechanism", NULL },
		{ IS_DEBUG, MF_STRING, ID_RTK_ONOFF_GL_SV, "Enable/Disable GLONASS SV mechanism", NULL },
		{ IS_DEBUG, MF_STRING, ID_RTK_ONOFF_BD_SV, "Enable/Disable BEIDOU2 SV mechanism", NULL },
		{ 1, MF_STRING, ID_QUERY_RTK_REF_POSITION, "Configure RTK Reference Static Started Position", NULL },
		{ 1, MF_STRING, ID_CLEAR_RTK_SLAVE_DATA, "Clear RTK Slave Backup Data", NULL },

		{ 1, MF_SEPARATOR, 0, NULL, NULL },
		{ 1, MF_STRING, ID_QUERY_RTK_MODE, "Query RTK Mode", NULL },
		{ 1, MF_STRING, ID_QUERY_RTK_MODE2, "Query RTK Mode And Operational Function", NULL },
		{ 1, MF_STRING, ID_QUERY_RTK_SLAVE_BAUD, "Query RTK Slave Serial Port Baud Rate", NULL },
		//{ IS_DEBUG, MF_STRING, ID_QUERY_RTK_PARAM, "Query RTK Parameters", NULL },
		{ 1, MF_SEPARATOR, 0, NULL, NULL },
		{ 1, MF_STRING, ID_CONFIG_RTK_MODE, "Configure RTK Mode", NULL },
		{ 1, MF_STRING, ID_CONFIG_RTK_MODE2, "Configure RTK Mode And Operational Function", NULL },
		{ 1, MF_STRING, ID_CFG_RTK_SLAVE_BAUD, "Configure RTK Slave Serial Port Baud Rate", NULL },
		//{ IS_DEBUG, MF_STRING, ID_CONFIG_RTK_PARAM, "Configure RTK Parameters", NULL },
		{ 0, 0, 0, NULL, NULL }	//End of table
	};
	if(!NMEA_INPUT && RTK_MENU && !SHOW_PATCH_MENU)
	{
		CreateSubMenu(hMenu, menuItemRtk, "&RTK");
	}

	static MenuItemEntry menuItemInsDr[] =
	{
		{ 1, MF_STRING, ID_INSDR_ENTER_UART, "Enter UART Pass Through for Slave ROM Code", NULL },
		{ 1, MF_STRING, ID_INSDR_ENTER_DWN, "Enter UART Pass Through for Slave Flash Code", NULL },
		{ 1, MF_STRING, ID_INSDR_LEAVE_UART, "Back To Master", NULL },
		{ IS_DEBUG, MF_STRING, ID_INSDR_ACC_SELF_TEST, "BMP160 Accelerometer Self-test", NULL },
		{ IS_DEBUG, MF_STRING, ID_INSDR_GYRO_SELF_TEST, "BMP160 Gyroscope Self-test", NULL },
		{ IS_DEBUG, MF_STRING, ID_INSDR_ACCU_ANGLE_START, "BMP160 Accumulate Angle Start", NULL },
		{ IS_DEBUG, MF_STRING, ID_INSDR_ACCU_ANGLE_STOP, "BMP160 Accumulate Angle Stop", NULL },
		{ 1, MF_STRING, ID_INSDR_TEST, "DR Test", NULL },
		{ 1, MF_SEPARATOR, 0, NULL, NULL },
		{ 1, MF_STRING, ID_QUERY_DR_RATE, "Query DR Predict Update Rate", NULL },
		{ 1, MF_STRING, ID_QUERY_DR_RAW_RATE, "Query DR RAW Sensor Data Output Rate", NULL },
		{ 1, MF_SEPARATOR, 0, NULL, NULL },
		{ 1, MF_STRING, ID_CONFIG_DR_RATE, "Configure DR Predict Update Rate", NULL },
		{ 1, MF_STRING, ID_CONFIG_DR_RAW_RATE, "Configure DR RAW Sensor Data Output Rate", NULL },
		{ 0, 0, 0, NULL, NULL }	//End of table
	};
	if(!NMEA_INPUT && INS_DR_MENU && !SHOW_PATCH_MENU)
	{
		CreateSubMenu(hMenu, menuItemInsDr, "&INS DR");
	}

	static MenuItemEntry menuItemEten[] =
	{
		{ 1, MF_SEPARATOR, 0, NULL, NULL },
		{ 1, MF_STRING, ID_QUERY_PSCM_DEV_ADDR, "Query PSCM Device Address", NULL },
		{ 1, MF_STRING, ID_QUERY_PSCM_LAT_LON, "Query PSCM Longitude/Latitude Fractional Digits", NULL },
		{ 1, MF_SEPARATOR, 0, NULL, NULL },
		{ 1, MF_STRING, ID_CONFIG_PSCM_DEV_ADDR, "Configure PSCM Device Address", NULL },
		{ 1, MF_STRING, ID_CONFIG_PSCM_LAT_LON, "Configure PSCM Longitude/Latitude Fractional Digits", NULL },
		{ 0, 0, 0, NULL, NULL }	//End of table
	};
	if(!NMEA_INPUT && CUSTOMER_ID == Eten)
	{
		CreateSubMenu(hMenu, menuItemEten, "&PSCM ");
	}

	//DRMenu
  /*
	static MenuItemEntry menuItemDR[] =
	{
		{ 1, MF_STRING, ID_QUERY_DR_MULTIHZ, "Query DR Multi-Hz", NULL },
		{ 1, MF_SEPARATOR, 0, NULL, NULL },
		{ 1, MF_STRING, ID_CONFIG_DR_MULTIHZ, "Configure DR Multi-Hz", NULL },
		{ 0, 0, 0, NULL, NULL }	//End of table
	};
	if ((SOFTWARE_FUNCTION & SW_FUN_DR) && !NMEA_INPUT)
	{
		CreateSubMenu(hMenu, menuItemDR, "&DR");
	}
  */
	//DF_UNIQUE_IDMenu
	static MenuItemEntry menuItemDofunUniqueId[] =
	{
		{ 1, MF_STRING, ID_ERASE_DF_UNIQUE_ID, "Erase Device Unique ID", NULL },
		{ 1, MF_STRING, ID_QUERY_DF_UNIQUE_ID, "Query Device Unique ID", NULL },
		{ 1, MF_SEPARATOR, 0, NULL, NULL },
		{ 1, MF_STRING, ID_CONFIG_DF_UNIQUE_ID, "Configure Device Unique ID", NULL },
		{ 0, 0, 0, NULL, NULL }	//End of table
	};
	if ((DOFUN_UNIQUE_ID) && !NMEA_INPUT)
	{
		CreateSubMenu(hMenu, menuItemDofunUniqueId, "&Unique ID");
	}

	// 1PPS Timing Menu
	static MenuItemEntry menuItem1PPSTiming[] =
	{
		{ TIMING_MONITORING, MF_STRING, ID_MONITORING_1PPS, "Monitoring 1PPS", NULL },
		{ IS_DEBUG, MF_STRING, ID_CFG_PRPTY_NMEA, "Configure Proprietary NMEA", NULL },
		{ 1, MF_STRING, ID_ON_LINE_ASSIST, "On Line Assistance", NULL },
		{ 1, MF_SEPARATOR, 0, NULL, NULL },
		{ 1, MF_STRING, ID_QUERY_TIMING, "Query Timing", NULL },
		{ 1, MF_STRING, ID_QUERY_CABLEDELAY, "Query Cable Delay", NULL },
		{ 1, MF_STRING, ID_QUERY_1PPS_PULSE_WIDTH, "Query 1PPS Pulse Width", NULL },
		{ 1, MF_STRING, ID_QUERY_1PPS_OUTPUT_MODE, "Query 1PPS Output Mode", NULL },
		{ 1, MF_STRING, ID_QUERY_1PPS_FREQ_OUTPUT, "Query 1PPS Frequency Output", NULL },
		{ 1, MF_SEPARATOR, 0, NULL, NULL },
		{ 1, MF_STRING, ID_CFG_TIMING, "Configure Timing", NULL },
		{ 1, MF_STRING, ID_CFG_TIMING_CABLE_DELAY, "Configure Cable Delay", NULL },
		{ 1, MF_STRING, ID_CFG_1PPS_PULSE_WIDTH, "Configure 1PPS Pulse Width", NULL },
		{ 1, MF_STRING, ID_CFG_1PPS_OUTPUT_MODE, "Configure 1PPS Output Mode", NULL },
		{ 1, MF_STRING, ID_CONFIG_1PPS_FREQ_OUTPUT, "Configure 1PPS Frequency Output", NULL },
		{ 0, 0, 0, NULL, NULL },
	};
	if(TIMING_MODE && !NMEA_INPUT && !SHOW_PATCH_MENU)
	{
		CreateSubMenu(hMenu, menuItem1PPSTiming, "&1PPS Timing");
	}

	//Ephemeris Menu
	static MenuItemEntry menuItemEphemeris[] =
	{
		{ 1, MF_STRING, ID_SHOW_GP_ALMANAC, "Show GPS Almanac", NULL },
		{ 1, MF_STRING, ID_SHOW_BD_ALMANAC, "Show Beidou Almanac", NULL },
		{ 1, MF_SEPARATOR, 0, NULL, NULL },
		{ 1, MF_STRING, ID_GET_GP_EPHEMERIS, "Get GPS Ephemeris", NULL },
		{ 1, MF_STRING, ID_SET_GP_EPHEMERIS, "Set GPS Ephemeris", NULL },
		{ 1, MF_STRING, ID_GET_GL_EPHEMERIS, "Get GLONASS Ephemeris", NULL },
		{ 1, MF_STRING, ID_SET_GL_EPHEMERIS, "Set GLONASS Ephemeris", NULL },
		{ 1, MF_STRING, ID_GET_BD_EPHEMERIS, "Get Beidou Ephemeris", NULL },
		{ 1, MF_STRING, ID_SET_BD_EPHEMERIS, "Set Beidou Ephemeris", NULL },
		{ 1, MF_SEPARATOR, 0, NULL, NULL },
		{ 1, MF_STRING, ID_GET_GP_ALMANAC, "Get GPS Almanac", NULL },
		{ 1, MF_STRING, ID_SET_GP_ALMANAC, "Set GPS Almanac", NULL },
		{ 1, MF_STRING, ID_GET_GL_ALMANAC, "Get GLONASS Almanac", NULL },
		{ 1, MF_STRING, ID_SET_GL_ALMANAC, "Set GLONASS Almanac", NULL },
		{ 1, MF_STRING, ID_GET_BD_ALMANAC, "Get Beidou Almanac", NULL },
		{ 1, MF_STRING, ID_SET_BD_ALMANAC, "Set Beidou Almanac", NULL },

		{ 1, MF_SEPARATOR, 0, NULL, NULL },
		{ 1, MF_STRING, ID_GET_GL_TIME_CORRECTIONS, "Get GLONASS Time Corrections", NULL },
		{ 1, MF_STRING, ID_SET_GL_TIME_CORRECTIONS, "Set GLONASS Time Corrections", NULL },

		{ 0, 0, 0, NULL, NULL },
	};
	if(!NMEA_INPUT && !SHOW_PATCH_MENU)
	{
		CreateSubMenu(hMenu, menuItemEphemeris, "&Ephemeris");
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
	if((SOFTWARE_FUNCTION & SW_FUN_AGPS) && !NMEA_INPUT && !SHOW_PATCH_MENU)
	{
		CreateSubMenu(hMenu, menuItemAGPS, "&AGPS");
	}

	//DataLog Menu
	static MenuItemEntry menuItemDataLog[] =
	{
		{ 1, MF_STRING, ID_LOG_STATUS, "Log Status", NULL },
		{ 1, MF_STRING, ID_LOG_CONFIGURE, "Log Configure", NULL },
		{ 1, MF_STRING, ID_LOG_CLEAR, "Log Clear", NULL },
		//{ 1, MF_STRING, ID_LOGGER_CONVERT, "Log Decompress", NULL },
		{ 1, MF_STRING, ID_LOG_DECOMPRESS, "Log Decompress", NULL },
		{ 1, MF_STRING, ID_LOG_READ, "Log Read", NULL },
		//		{ SUPPORT_CLEAR_LOGIN_PASSWORD, MF_STRING, ID_DATALOG_CLEARLOGINPASSWORD, "Clear Login Password", NULL },
		{ 0, 0, 0, NULL, NULL },
	};

	if((SOFTWARE_FUNCTION & SW_FUN_DATALOG) && !NMEA_INPUT && !SHOW_PATCH_MENU)
	{
		CreateSubMenu(hMenu, menuItemDataLog, "&DataLog");
	}

  //Watch DataLog Menu
	static MenuItemEntry menuItemWatchDataLog[] =
	{
		{ 1, MF_STRING, ID_LOGW_STATUS, "Log Status", NULL },
		{ 1, MF_STRING, ID_LOGW_CONFIGURE, "Log Configure", NULL },
		{ 1, MF_STRING, ID_LOGW_CLEAR, "Log Clear", NULL },
		{ 1, MF_STRING, ID_LOGW_DECOMPRESS, "Log Decompress", NULL },
		{ 1, MF_STRING, ID_LOGW_READ, "Log Read", NULL },
    { 1, MF_SEPARATOR, 0, NULL, NULL },
		{ IS_DEBUG, MF_STRING, ID_CONFIG_WATCH_TRACKBACK, "Watch Trackback Configure", NULL },

		{ 0, 0, 0, NULL, NULL },
	};
	if(IS_DEBUG && !NMEA_INPUT && !SHOW_PATCH_MENU)
	{
		CreateSubMenu(hMenu, menuItemWatchDataLog, "&Watch DataLog");
	}

	//Converter Menu
	static MenuItemEntry menuItemConvert[] =
	{
		//{ IS_DEBUG & SOFTWARE_FUNCTION & SW_FUN_DATALOG, MF_STRING, ID_CONVERTER_COMPRESS, "Compress", NULL },
		{ IS_DEBUG & SOFTWARE_FUNCTION & SW_FUN_DATALOG, MF_STRING, ID_LOG_DECOMPRESS, "DataLog Decompress", NULL },
		{ 1, MF_STRING, ID_CONVERTER_KML, "KML", NULL },
		{ 1, MF_STRING, ID_RAW_MEAS_OUT_CONVERT, "Raw Measurement Binary Convert", NULL },
		{ IS_DEBUG, MF_STRING, ID_UBLOX_OUT_CONVERT, "Ublox Binary Convert", NULL },
		{ 1, MF_STRING, ID_HOSTLOG_NMEA, "HostLog Output To NMEA", NULL },
		//{ IS_DEBUG, MF_STRING, ID_DECODE_GP_ALMANAC, "Decode GPS Almanac", NULL },
		{ 0, 0, 0, NULL, NULL },
	};

	if(!SHOW_PATCH_MENU)
	{
    CreateSubMenu(hMenu, menuItemConvert, "&Converter");
  }

	//Utility Menu
	static MenuItemEntry menuItemUtility[] =
	{
		{ IS_DEBUG || DEVELOPER_VERSION, MF_STRING, ID_NMEA_CHECKSUM_CAL, "NMEA checksum calculator", NULL },
		{ IS_DEBUG || DEVELOPER_VERSION, MF_STRING, ID_BIN_CHECKSUM_CAL, "Binary checksum calculator", NULL },
		{ IS_DEBUG, MF_STRING, ID_TEST_EXTERNAL_SREC, "Test External SREC", NULL },
		{ IS_DEBUG, MF_STRING, ID_IQ_PLOT, "IQ Plot", NULL },
		{ IS_DEBUG, MF_STRING, ID_READ_MEM_TO_FILE, "Read 0x50000000 to a File (20KB)", NULL },
		{ IS_DEBUG, MF_STRING, ID_WRITE_MEM_TO_FILE, "Write a File to 0x50000000", NULL },
		{ IS_DEBUG, MF_STRING, ID_READ_MEM_TO_FILE2, "Read 0x60000000 to a File (1KB)", NULL },
		{ IS_DEBUG, MF_STRING, ID_PATCH_ROM_EPH, "Patch ROM 20130221 Ephemeris", NULL },
		{ IS_DEBUG, MF_STRING, ID_IO_TESTER, "V9 IO Tester", NULL },

		{ 0, 0, 0, NULL, NULL },
	};

	if(IS_DEBUG | DEVELOPER_VERSION && !SHOW_PATCH_MENU)
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

	CGPSDlg::gpsDlg->SetInputMode(CGPSDlg::NoOutputMode);

	U08 msg[8];
	msg[0] = 0x34;
	msg[1] = cyear >> 8 &0xff;
	msg[2] = cyear &0xff;
	msg[3] = (U08)now.wMonth;
	msg[4] = (U08)now.wDay;
	msg[5] = (U08)now.wHour;
	msg[6] = (U08)now.wMinute;
	msg[7] = (U08)now.wSecond;

	int len = CGPSDlg::gpsDlg->SetMessage(msg, sizeof(msg));

	CGPSDlg::gpsDlg->ClearQue();
	if(CGPSDlg::gpsDlg->SendToTarget(CGPSDlg::m_inputMsg, len, "", true))
	{
		CAgps_config *dlg = new CAgps_config(CGPSDlg::gpsDlg);
		if(dlg->DoModal() == IDOK)
		{
			U08 msg[3] = {0x33,dlg->enable,dlg->attribute};
			int len = CGPSDlg::gpsDlg->SetMessage(msg,sizeof(msg));
			CGPSDlg::gpsDlg->//WaitEvent();
				CGPSDlg::gpsDlg->ClearQue();
			CGPSDlg::gpsDlg->SendToTarget(CGPSDlg::m_inputMsg, len, "Set AGPS Status successfully", true);
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

void CGPSDlg::OnAgpsFtpSrec()
{
	if(!CheckConnect())
	{
		return;
	}
	SetInputMode(NoOutputMode);
	if(Ack != QueryAgpsStatus(Return, NULL))
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
	SetInputMode(NoOutputMode);
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
	SetInputMode(NoOutputMode);
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
	SetInputMode(NoOutputMode);

	CHostBaseDownloadDlg dlg;
	if(dlg.DoModal()!=IDOK)
	{
		SetMode();
		CreateGPSThread();
		return;
	}
	m_DownloadMode = HostBasedDownload;
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
	SetInputMode(NoOutputMode);

	CFirmwareDownloadDlg dlg;
	if(dlg.DoModal()!=IDOK)
	{
		SetMode();
		CreateGPSThread();
		return;
	}

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
	case CFirmwareDownloadDlg::V8InternalLoader:
		m_DownloadMode = ForceInternalLoaderV8;
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
	SetInputMode(NoOutputMode);
	CParallelDownloadDlg dlg;
	if(dlg.DoModal()!=IDOK)
	{
		SetMode();
		CreateGPSThread();
		return;
	}
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
		if(n == 0)
		{
			return true;
		}
		p = (BYTE*)memchr(p + 1, '$', bin.Size() - (p - bin.GetBuffer()) - 1);
	};
	return false;
}

bool CGPSDlg::DoDownload(int dlBaudIdx)
{
	m_lbl_firmware_path.GetWindowText(m_strDownloadImage);
	if(!Utility::IsFileExist(m_strDownloadImage))
	{
		::AfxMessageBox("PROM file not found!");
		return false;
	}

	if(CUSTOMER_DOWNLOAD && CUSTOMER_ID == OlinkStar)	//Olinkstar must check prom.bin
	{
		if(!CheckOlinkstarFirmware(m_strDownloadImage))
		{
			::AfxMessageBox("PROM file not support!");
			return false;
		}
	}

	if(!CheckConnect())
	{
		return false;
	}

#if GG12A
	if(check_gg12a_format(m_strDownloadImage) == FALSE)
	{
		return false;
	}
#endif
	//Detect Loader Type
	BOOL usingInternal = ((CButton*)GetDlgItem(IDC_IN_LOADER))->GetCheck();
	BOOL isCheat = IS_DEBUG && (GetAsyncKeyState(VK_LSHIFT) & 0x8000) && (GetAsyncKeyState(VK_LMENU)& 0x8000);

	m_DownloadMode = (usingInternal) ? InternalLoaderV8 : EnternalLoaderInBinCmd;
	if(CUSTOMER_DOWNLOAD)
	{
		m_DownloadMode = CustomerDownload;
	}
  //Press Shift + Alt to enter production download
  if(isCheat)
  {
    CString imageIni = Utility::GetFilePathName(m_strDownloadImage) + ".ini";
    if(Utility::IsFileExist(imageIni))
    {
      tagAddress = Utility::GetPrivateProfileHex("Firmware", "Address", 0x00000000, imageIni);
      tagValue = Utility::GetPrivateProfileHex("Firmware", "Value", 0x00000000, imageIni);
      downloadAddTag = TRUE;
    }
    else
    {
      isCheat = FALSE;
      downloadAddTag = FALSE;
    }
  }

	CString externalSrecFile;
	if(theApp.CheckExternalSrec(externalSrecFile))
	{	//Not ParallelDownload and external loader is exist. Prompts the user to use external loader.
		if(ALWAYS_USE_EXTERNAL_SREC || IDYES == ::AfxMessageBox("Do you want to use extenal loader?", MB_YESNO))
		{
      m_DownloadMode = (g_setting.downloadUseBinExternal) ? FileLoaderInBinCmd : FileLoader;
		}
	}
	else if(!theApp.CheckExternalSrec(externalSrecFile) && ALWAYS_USE_EXTERNAL_SREC)
	{
		::AfxMessageBox("No external loader exist!");
		return false;
	}
	else if(RESOURCE_LOADER_ID)
	{
		m_DownloadMode = InternalLoaderSpecial;
	}
  else if(isCheat && m_DownloadMode == InternalLoaderV8)
  {
    m_DownloadMode = EnternalLoader;
  }
	m_nDownloadBaudIdx = dlBaudIdx;
	m_nDownloadBufferIdx = 0;
	SetInputMode(NoOutputMode);

	::AfxBeginThread(DownloadThread, 0);
	return true;
}

bool CGPSDlg::DoDownload(int dlBaudIdx, UINT rid)
{
	if(!CheckConnect())
	{
		return false;
	}

	m_DownloadMode = CustomerUpgrade;
	m_nDownloadBaudIdx = dlBaudIdx;
	m_nDownloadBufferIdx = 0;
	SetInputMode(NoOutputMode);
	m_nDownloadResource = rid;

	::AfxBeginThread(DownloadThread, 0);
	return true;
}

void CGPSDlg::OnBnClickedDownload()
{
	int dlBaudIdx = ((CComboBox*)GetDlgItem(IDC_DL_BAUDRATE))->GetCurSel();
	//dlBaudIdx += 5;	//Download Baudrate start in 115200
  if(SPECIAL_BAUD_RATE)
  {
    dlBaudIdx = 3;
  }
	theApp.SetIntSetting("dl_baudIdx", dlBaudIdx);

	DoDownload(dlBaudIdx);
}

void CGPSDlg::OnUpgradeDownload()
{
#if defined (UPGRADE_DOWNLOAD)
	DoDownload(6, IDR_UPGRADE_DOWNLOAD_PROM);
#endif
}

void CGPSDlg::OnPatch()
{
	DoDownload(6, IDR_UPGRADE_DOWNLOAD_PROM2);
}

void CGPSDlg::OnFileCleannema()
{
	m_nmeaList.DeleteAllItems();
}

void CGPSDlg::OnBnClickedBrowse()
{
	CString strPath;
	m_lbl_firmware_path.GetWindowText(strPath);
	if(!Utility::IsFileExist(strPath))
	{
		strPath.Empty();
	}
	CFileDialog fd(true, "*.bin", strPath, OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, "*.bin|*.bin||");
	if(fd.DoModal() == IDOK)
	{
		strPath = fd.GetPathName();
		m_lbl_firmware_path.SetWindowText(strPath);
		m_lbl_firmware_path.Invalidate();

		g_setting.mainFwPath = strPath;
		g_setting.Save();
	}
}

int m_binary_interval_attr,m_rate;
UINT set_binarymsg_interval_andrew(LPVOID param)
{
	U08 msg[3];
	msg[0] = 0x11;
	msg[1] = m_rate;
	msg[2] = m_binary_interval_attr;

	int len = CGPSDlg::gpsDlg->SetMessage(msg,sizeof(msg));
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(CGPSDlg::m_inputMsg, len, "Set Binary Ouput Interval successfully");
	return 0;
}

void CGPSDlg::OnBinaryConfigureBinaryInterval()
{
	if(!CheckConnect())
	{
		return;
	}

	SetInputMode(NoOutputMode);
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

		char file_name[100];
		char remote_path[100];
		strcpy_s(file_name, sizeof(file_name),"Eph.dat");
		sprintf_s(remote_path,"ephemeris\\%s",file_name);

		if(!pConnect->GetFile(remote_path,file_name,false))
		{
			int err = GetLastError();
			if(err==12002)
			{
				AfxMessageBox("Download Eph.dat Timeout...");
			}
			else if(err==12003)
			{
				AfxMessageBox("Server Error...");
			}
			else
			{
				AfxMessageBox("Eph.dat File Error...");
			}
			pConnect->Close();
			return false;
		}
		else
		{
			pConnect->Close();
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
	if(f == NULL)
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

			if(tk < -7200)
			{
				fclose(f);
				return download_eph();
			}

			S32 abs_tk = (S32)abs(tk);

			S32 offset = abs_tk / 14400;

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

D64 CGPSDlg::calculate_tk_double(S16 ref_wn, S32 ref_tow, S16 wn, D64 tow)
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

	*wn = ephwn+1024;
	*toc = ephtoc;
}

void CGPSDlg::get_wn_tow(S16* wn,D64* tow)
{
	SYSTEMTIME	now;
	GetSystemTime(&now);

	UtcTime utc;
	utc.year = now.wYear;
	utc.month = now.wMonth;
	utc.day = now.wDay;
	utc.hour = now.wHour;
	utc.minute = now.wMinute;
	utc.sec = now.wSecond;

	//printf("PC time %d/%d/%d %d:%d:%d\n",utc.year,utc.month,utc.day,utc.hour,utc.minute,(int)utc.sec);
	UtcConvertUtcToGpsTime(&utc, wn, tow);

	if(*tow >= 604800.0)
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
	SendToTarget(messages, 11,"Configure Serial Port successfully", true);
	SetBaudrate(baud);
	g_setting.Save();
	return;
}

int m_position_pinning;
int m_attributes;
UINT ConfigurePositionPinning(LPVOID param)
{
	U08 msg[3];
	msg[0] = 0x39;
	msg[1] = m_position_pinning;
	msg[2] = m_attributes;

	int len = CGPSDlg::gpsDlg->SetMessage(msg,sizeof(msg));
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(CGPSDlg::m_inputMsg, len, "Configure Position Pinning successfully");

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
	U08 msg[12];
	msg[0] = 0x3B;

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
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(CGPSDlg::m_inputMsg, len, "Configure Pinning Parameters successfully");
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

		msg[0] = 0x64;
		msg[1] = 0x17;
		msg[2] = m_multi_mode & 0xFF;
		msg[3] = m_multimode_attribute & 0xFF;
		len = CGPSDlg::gpsDlg->SetMessage(msg, 4);
    //V6 style
		//msg[0] = 0x3C;
		//msg[1] = m_multi_mode & 0xFF;
		//msg[2] = m_multimode_attribute & 0xFF;
		//len = CGPSDlg::gpsDlg->SetMessage(msg, 3);

	CGPSDlg::gpsDlg->ExecuteConfigureCommand(CGPSDlg::m_inputMsg, len, "Configure Navigation Mode successfully");
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

static U32 m_reg_addr;
static U32 m_reg_data;
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
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(CGPSDlg::m_inputMsg, len, "Configure Register successfully");
	return 0;
}

UINT ConfigureRegister16_thread(LPVOID param)
{
	U08 msg[9];
	msg[0] = 0x74;

	msg[1] = m_reg_addr >> 24 & 0xFF;
	msg[2] = m_reg_addr >> 16 & 0xFF;
	msg[3] = m_reg_addr >> 8 & 0xFF;
	msg[4] = m_reg_addr & 0xFF;

	msg[5] = m_reg_data >> 24 & 0xFF;
	msg[6] = m_reg_data >> 16 & 0xFF;
	msg[7] = m_reg_data >> 8 & 0xFF;
	msg[8] = m_reg_data & 0xFF;

	int len = CGPSDlg::gpsDlg->SetMessage(msg,sizeof(msg));
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(CGPSDlg::m_inputMsg, len, "Configure 16-IO Register successfully");
	return 0;
}

bool CGPSDlg::WriteRegister(U32 addr, U32 data, LPCSTR prompt)
{
	U08 msg[9];
	msg[0] = 0x72;

	msg[1] = addr >> 24 & 0xFF;
	msg[2] = addr >> 16 & 0xFF;
	msg[3] = addr >> 8 & 0xFF;
	msg[4] = addr & 0xFF;

	msg[5] = data >> 24 & 0xFF;
	msg[6] = data >> 16 & 0xFF;
	msg[7] = data >> 8 & 0xFF;
	msg[8] = data & 0xFF;

	int len = CGPSDlg::gpsDlg->SetMessage(msg, sizeof(msg));
	return CGPSDlg::gpsDlg->ExecuteConfigureCommand(CGPSDlg::m_inputMsg, len, prompt, false);
}

void CGPSDlg::OnBinaryConfigureregister()
{
	if(!CheckConnect())
	{
		return;
	}
	static CString addr = "00000000";
	static CString m_data = "00000000";
	CCon_register dia;

	dia.m_addr = addr;
	dia.m_data = m_data;

	if(dia.DoModal() == IDOK)
	{
		m_reg_addr = ConvertCharToU32(dia.m_addr);
		m_reg_data = ConvertCharToU32(dia.m_data);
		addr = dia.m_addr;
		m_data = dia.m_data;
		::AfxBeginThread(ConfigureRegister_thread, 0);
	}
	else
	{
		SetMode();
		CreateGPSThread();
	}
}

void CGPSDlg::OnBinaryConfigureregister16()
{
	if(!CheckConnect())
	{
		return;
	}
	static CString addr = "00000000";
	static CString m_data = "00000000";
	CCon_register dia;

	dia.m_addr = addr;
	dia.m_data = m_data;

	if(dia.DoModal() == IDOK)
	{
		m_reg_addr = ConvertCharToU32(dia.m_addr);
		m_reg_data = ConvertCharToU32(dia.m_data);
		addr = dia.m_addr;
		m_data = dia.m_data;
		::AfxBeginThread(ConfigureRegister16_thread, 0);
	}
	else
	{
		SetMode();
		CreateGPSThread();
	}
}

void CGPSDlg::OnBinaryConfigureClockOffset()
{
	if(!CheckConnect())
	{
		return;
	}
	static CString addr = "870000CD";
	static CString m_data = "00000000";
	CCon_register dia;
  dia.specialFunction = 1;
	dia.m_addr = addr;
	dia.m_data = m_data;

	if(dia.DoModal() == IDOK)
	{
		m_reg_addr = ConvertCharToU32(dia.m_addr);
		m_reg_data = ConvertCharToU32(dia.m_data);
		addr = dia.m_addr;
		m_data = dia.m_data;
		::AfxBeginThread(ConfigureRegister_thread, 0);
	}
	else
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

	U08 msg[3];
	msg[0] = 0xF;
	msg[1] = (U08)m_multipath;
	msg[2] = (U08)m_multipath_attribute;

	int len = CGPSDlg::gpsDlg->SetMessage(msg,sizeof(msg));
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(CGPSDlg::m_inputMsg, len, "Configure Multi-Path successfully");
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
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(CGPSDlg::m_inputMsg, len, "Configure 1PPS Timing successfully");
	return 0;
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
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(CGPSDlg::m_inputMsg, len, "Configure DOP Mask successfully");
	return 0;
}

void CGPSDlg::OnConfigureDopMask()
{
	if(!CheckConnect())
	{
    return;
  }
    
  CCon1PPS_DOP dlg;
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

	CGPSDlg::gpsDlg->SendToTarget(CGPSDlg::m_inputMsg,len,"Configure Elevation and CNR Mask successfully", true);
	CGPSDlg::gpsDlg->SetMode();
	CGPSDlg::gpsDlg->CreateGPSThread();
	return 0;
}

void CGPSDlg::OnConfigElevationAndCnrMask()
{
	if(!CheckConnect())
	{
    return;
  }

  CCon1PPS_ElevCNR dlg;
	if(dlg.DoModal()==IDOK)
	{
		m_elev_sel = dlg.sel;
		m_elev_elev = dlg.elev;
		m_elev_cnr = dlg.cnr;
		m_elev_attr = dlg.attr;
		::AfxBeginThread(ConfigElevationAndCnrMaskThread, 0);
	}
	else
	{
		SetMode();
		CreateGPSThread();
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

	CGPSDlg::gpsDlg->ClearQue();
	CGPSDlg::gpsDlg->SendToTarget(CGPSDlg::m_inputMsg,len,"Configure Proprietary NMEA successfully", true);
	CGPSDlg::gpsDlg->SetMode();
	CGPSDlg::gpsDlg->CreateGPSThread();

	return 0;
}

void CGPSDlg::OnConfigureProprietaryNmea()
{
	if(!CheckConnect())
	{
    return;
  }

  CProprietary_nmea dlg;
	if(dlg.DoModal()==IDOK)
	{
		m_proprietary_id = dlg.psti_id;
		m_proprietary_psti = dlg.psti_interval;
		m_proprietary_attr = dlg.attr;
		::AfxBeginThread(configy_proprietary_nmea_thread, 0);
	}
	else
	{
		SetMode();
		CreateGPSThread();
	}
}

LRESULT CGPSDlg::OnMyDeviceChange(WPARAM wParam, LPARAM lParam)
{
	if(DBT_DEVICEARRIVAL == wParam || DBT_DEVICEREMOVECOMPLETE == wParam)
	{
		PDEV_BROADCAST_HDR pHdr = (PDEV_BROADCAST_HDR)lParam;
		PDEV_BROADCAST_DEVICEINTERFACE pDevInf;
		PDEV_BROADCAST_HANDLE pDevHnd;
		PDEV_BROADCAST_OEM pDevOem;
		PDEV_BROADCAST_PORT pDevPort;
		PDEV_BROADCAST_VOLUME pDevVolume;
		switch(pHdr->dbch_devicetype)
		{
		case DBT_DEVTYP_DEVICEINTERFACE:
			pDevInf = (PDEV_BROADCAST_DEVICEINTERFACE)pHdr;
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
			if(DBT_DEVICEARRIVAL == wParam)	
			{	//Plugin
				SetTimer(DELAY_PLUGIN_TIMER, 2000, NULL);
			}
			else
			{	//Remove
				SetTimer(DELAY_PLUGIN_TIMER, 1000, NULL);
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

	if(DBT_DEVICEARRIVAL == wParam) 
	{
		szTmp.Format(_T("Adding %s\r\n"), port_name);
	}
	else 
	{
		szTmp.Format(_T("Removing %s\r\n"), port_name);
	}

	add_msgtolist(szTmp);
	if(DBT_DEVICEARRIVAL == wParam  && !m_isConnectOn) 
	{
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
	}
	else if(m_isConnectOn)
	{
		m_ComPortCombo.GetLBText(m_ComPortCombo.GetCurSel(),now_portname);
		if(now_portname.Compare(port_name) == 0)
		{
			OnBnClickedClose();
		}
	}
}

//UINT MinihomerSettageccoThread(LPVOID pParam)
//{
//	CGPSDlg::gpsDlg->MinihomerSettagecco();
//	return TRUE;
//}
//
//void CGPSDlg::MinihomerSettagecco()
//{
//	U08 msg[3] ,checksum=0;
//	CString temp;
//	U32 data = 0;
//
//	msg[0] = 0x7E; // set device_id;
//	msg[1] = 0x01;
//	msg[2] = 0x88;
//
//	int len = SetMessage(msg,sizeof(msg));
//
//	ClearQue();
//	if(SendToTarget(m_inputMsg,len, "Set miniHomer Tag successfully", true) != 1)
//		add_msgtolist("Set miniHomer Tag Fail.");
//
//	SetMode();
//	CreateGPSThread();
//}

//void CGPSDlg::OnMinihomerSettagecco()
//{
//	if(!CheckConnect())
//		return;
//
//	AfxBeginThread(MinihomerSettageccoThread,0);
//
//}

void CGPSDlg::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
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
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(CGPSDlg::m_inputMsg, len, "Configure GNSS Selection for Navigation System successfully");
	return 0;
}

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
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(CGPSDlg::m_inputMsg, len, "Configure NMEA Output Comport successfully");
	return 0;
}

void CGPSDlg::OnConfigNmeaOutputComPort()
{
	if(!CheckConnect())
	{
    return;
  }

	CConNMEAComport dlg;
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
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(CGPSDlg::m_inputMsg, len, "Configure NMEA Talker ID successfully");
	return 0;
}

void CGPSDlg::OnBinaryConfigurenmeatalkerid()
{
	if(!CheckConnect())
	{
    return;
  }

	CCon_NMEA_TalkerID dlg;
	if(dlg.DoModal()==IDOK)
	{
		nmea_talker = dlg.talkerid;
		nmea_talker_attr = dlg.attr;
		::AfxBeginThread(config_NMEA_TalkerID_thread, 0);
	}
  else
	{
		SetMode();
		CreateGPSThread();
	}
}

void CGPSDlg::OnGetGlonassEphemeris()
{
	if(!CheckConnect())
	{
		return;
	}
	SetInputMode(NoOutputMode);
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
	SetInputMode(NoOutputMode);
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
				AfxMessageBox("The Beidou2 ephemeris data file format is incorrect.");
				break;
			}
			dwBytesRemaining -= nBytesRead;

			if(buf[3]==0)
			{
				continue;
			}
			BinaryCommand commad;
			commad.SetData(cmd);
			txtMsg.Format("Set SV#%d Beidou2 ephemeris successfully", svId);
			if(!SendToTargetNoWait(commad.GetBuffer(), commad.Size(), txtMsg))
			{
				txtMsg.Format("Set SV#%d Beidou2 ephemeris failed...", svId);
				add_msgtolist(txtMsg);
			}
		}
	}
	else
	{
		AfxMessageBox("File of the Beidou2 ephemeris data format wrong!");
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

			sprintf_s(m_nmeaBuffer, sizeof(m_nmeaBuffer), "Set SV#%d Glonass ephemeris successfully.",SVID);
			if(!SendToTargetNoWait(m_inputMsg, len,m_nmeaBuffer))
			{
				sprintf_s(m_nmeaBuffer, sizeof(m_nmeaBuffer), "Set SV#%d Glonass ephemeris fails!",SVID);
				add_msgtolist(m_nmeaBuffer);
			}
			//TRACE("%d %d\n",SVID,dwBytesRemaining);
		}
	}
	else
	{
		AfxMessageBox("File of the Glonass ephemeris data format wrong!");
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
	return 0;
}

UINT SetBeidouEphemerisThread(LPVOID pParam)
{
	CGPSDlg::gpsDlg->SetBeidouEphms(FALSE);
	return 0;
}
void CGPSDlg::OnSetGlonassEphemeris()
{
	if(!CheckConnect())
	{
		return;
	}

	SetInputMode(NoOutputMode);
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

	SetInputMode(NoOutputMode);
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

void CGPSDlg::GetTimeCorrection(CString m_filename)
{
	int wait = 0;
	U08 msg[1];
	int res_len;
	int len;
	FILE *f = NULL;
	U08 buff[256];

	msg[0] = 0x5F;
	len = SetMessage(msg,sizeof(msg));

	ClearQue();
	if(SendToTarget(m_inputMsg,len,"Get Time Correction successfully", true) == 1)
	{
		memset(buff, 0, sizeof(buff));
		res_len = m_serial->GetBinaryAck(buff, sizeof(buff));

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
	U08 msg[10];
	int len;
	FILE *f = NULL;

	msg[0] = 0x60;
	fopen_s(&f, m_filename, "rb");
	fread(&msg[1],1,8,f);
	fclose(f);
	msg[9] = m_time_correction_attr;

	len = SetMessage(msg,sizeof(msg));

	ClearQue();
	if(SendToTarget(m_inputMsg,len,"Set Time Correction successfully", true) != 1)
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

	SetInputMode(NoOutputMode);
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
/*
int m_1pps_output_mode;
int m_1pps_output_mode_attr;
int m_1pps_output_align;

UINT config_1PPS_output_mode_thread(LPVOID param)
{
	U08 msg[4];

	msg[0] = 0x55;
	msg[1] = m_1pps_output_mode;
	msg[2] = m_1pps_output_align;
	msg[3] = m_1pps_output_mode_attr;

	int len = CGPSDlg::gpsDlg->SetMessage(msg,sizeof(msg));
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(CGPSDlg::m_inputMsg, len, "Configure 1PPS Output Mode successfully");
	return 0;
}

void CGPSDlg::OnConfigure1PpsOutputMode()
{
	if(!CheckConnect())
	{
    return;
  }

  CCon1PPS_OutputMode dlg;
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
*/
/*
UINT query_1PPS_output_mode_thread(LPVOID param)
{
	U08 buff[100];
	U08 msg[1];
	CString tmp;
	time_t start,end;
	int res;

	msg[0] = 0x56;

	int len = CGPSDlg::gpsDlg->SetMessage(msg,sizeof(msg));

	CGPSDlg::gpsDlg->ClearQue();

	if(CGPSDlg::gpsDlg->SendToTarget(CGPSDlg::m_inputMsg,len,"Query 1PPS Output successfully", true))
	{

		start = clock();
		while(1)
		{
			memset(buff, 0, 100);
			res = CGPSDlg::gpsDlg->m_serial->GetBinaryAck(buff, sizeof(buff));

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
*/
/*
void CGPSDlg::OnQuery1PpsOutputMode()
{
	if(CheckConnect())
	{
		::AfxBeginThread(query_1PPS_output_mode_thread, 0);
	}
}
*/
bool CGPSDlg::BoostBaudrate(BOOL bRestore, BoostMode mode, bool isForce)
{
  bool r = true;
	if(bRestore)
	{
		if((isForce || ChangeToTemp != mode) && (g_setting.GetBaudrateIndex() != m_nDownloadBaudIdx))
		{
			r = SetPort(g_setting.GetBaudrateIndex(), (int)mode);
			Sleep(1000);
		}
		CloseOpenUart();
		m_serial->ResetPort(g_setting.GetBaudrateIndex());
		m_BaudRateCombo.SetCurSel(g_setting.GetBaudrateIndex());
	}
	else
	{
		if(g_setting.GetBaudrateIndex() != m_nDownloadBaudIdx)
		{
			r = SetPort(m_nDownloadBaudIdx, (int)mode);
			Sleep(1000);
		}
	}
  return r;
}

void CGPSDlg::TempBoostBaudrate(BOOL bRestore, int boostBaudIndex, int timeout)
{
	if(bRestore)
	{
		SetPort(g_setting.GetBaudrateIndex(), (int)ChangeToTemp);
		Sleep(timeout);
		CloseOpenUart();
		m_serial->ResetPort(g_setting.GetBaudrateIndex());
		m_BaudRateCombo.SetCurSel(g_setting.GetBaudrateIndex());
	}
  else
	{
		if(g_setting.GetBaudrateIndex() != boostBaudIndex)
		{
			SetPort(boostBaudIndex, (int)ChangeToTemp);
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
	//struct UiLocationEntry {
	//	BOOL showOption;	//TRUE-Show, FALSE-Hide
	//	UINT type;			//MF_STRING(0), MF_SEPARATOR, MF_POPUP
	//	UINT id;
	//	RECT rect;
	//};
#if (SMALL_UI)
	const UiLocationEntry UiTable[] =
	{
		//Connect Panel
		TRUE, 0, IDC_COMPORT_T, {6, 1, 80, 15},
		TRUE, 0, IDC_BAUDRATE_T, {92, 1, 80, 15},
		TRUE, 0, IDC_OPEN_CLOSE_T, {170, 1, 51, 15},
		TRUE, 0, IDC_COMPORT, {6, 17, 80, 13},
		TRUE, 0, IDC_BAUDRATE_IDX, {92, 17, 80, 13},
		TRUE, 0, IDC_CONNECT, {189, 17, 32, 24},
		TRUE, 0, IDC_CLOSE, {189, 17, 32, 24},
		FALSE, 0, IDC_PLAY, {182, 28, 32, 24},
		FALSE, 0, IDC_RECORD, {218, 28, 32, 24},
		FALSE, 0, IDC_STOP, {254, 28, 32, 24},
		FALSE, 0, IDC_PLAY_T, {180, 7, 32, 18},
		FALSE, 0, IDC_RECORD_T, {216, 7, 32, 18},
		FALSE, 0, IDC_STOP_T, {252, 7, 32, 18},

		//Message Panel
		TRUE, 0, IDC_MESSAGE_T, {6, 45, 215, 29},
		TRUE, 0, IDC_FIXED_STATUS, {77, 45, 144, 29},
		TRUE, 0, IDC_MESSAGE, {6, 75, 215, 100},

		//Response Panel
		FALSE, 0, IDC_RESPONSE_T, {6, 192, 215, 29},
		TRUE, 0, IDC_RESPONSE, {6, 178, 215, 100},

		//Coordinate Panel
		FALSE, 0, IDC_COORDINATE_T, {6, 399, 280, 29},
		FALSE, 0, IDC_COORDINATE_F, {6, 429, 280, 132},
		FALSE, 0, IDC_WGS84_X_T, {22, 436, 110, 18},
		FALSE, 0, IDC_WGS84_X, {22, 454, 110, 18},
		FALSE, 0, IDC_WGS84_Y_T, {22, 477, 110, 18},
		FALSE, 0, IDC_WGS84_Y, {22, 495, 110, 18},
		FALSE, 0, IDC_WGS84_Z_T, {22, 518, 110, 18},
		FALSE, 0, IDC_WGS84_Z, {22, 536, 110, 18},
		FALSE, 0, IDC_ENU_E_T, {160, 436, 110, 18},
		FALSE, 0, IDC_ENU_E, {160, 454, 110, 18},
		FALSE, 0, IDC_ENU_N_T, {160, 477, 110, 18},
		FALSE, 0, IDC_ENU_N, {160, 495, 110, 18},
		FALSE, 0, IDC_ENU_U_T, {160, 518, 110, 18},
		FALSE, 0, IDC_ENU_U, {160, 536, 110, 18},

		//Command Panel
#if defined(SAINTMAX_UI)
		TRUE, 0, IDC_NO_OUTPUT, {5, 268, 107, 71},
		TRUE, 0, IDC_NMEA_OUTPUT, {116, 268, 107, 71},
		FALSE, 0, IDC_COLDSTART, {5, 268, 218, 71},
#elif defined(XN120_TESTER)
		FALSE, 0, IDC_NO_OUTPUT, {5, 268, 107, 71},
		FALSE, 0, IDC_NMEA_OUTPUT, {116, 268, 107, 71},
		TRUE, 0, IDC_COLDSTART, {5, 268, 218, 71},
#endif
		FALSE, 0, IDC_COMMAND_T, {6, 325, 280, 29},
		FALSE, 0, IDC_COMMAND_F, {6, 355, 280, 132},
		FALSE, 0, IDC_HOTSTART, {15, 599, 82, 24},
		FALSE, 0, IDC_WARMSTART, {105, 599, 82, 24},
		FALSE, 0, IDC_BIN_OUTPUT, {195, 627, 82, 24},
		FALSE, 0, IDC_SCANALL, {15, 655, 82, 24},
		FALSE, 0, IDC_SCANPORT, {105, 655, 82, 24},
		FALSE, 0, IDC_SCANBAUDRATE, {195, 655, 82, 24},

		//Information
		TRUE, 0, IDC_INFORMATION_T, {228, 17, 100, 19},
		FALSE, 0, IDC_RTK_INFO_T, {401, 6, 100, 19},
		FALSE, 0, IDC_RTK_INFO_B, {401, 6, 100, 19},

		TRUE, 0, IDC_INFO_PANEL,	{228, 35, 343, 104},
		TRUE, 0, IDC_TTFF_T,		{238, 46, 100, 37},
		TRUE, 0, IDC_TTFF,			{243, 66,  94, 16},
		TRUE, 0, IDC_LONGITUDE_T,	{238, 91, 100, 37},
		TRUE, 0, IDC_LONGITUDE,		{243, 111, 94, 16},
		TRUE, 0, IDC_DATE_T,		{350, 46, 100, 37},
		TRUE, 0, IDC_DATE,			{355, 66,  94, 16},
		TRUE, 0, IDC_LATITUDE_T,	{350, 91, 100, 37},
		TRUE, 0, IDC_LATITUDE,		{355, 111, 94, 16},
		TRUE, 0, IDC_TIME_T,		{462, 46, 100, 37},
		TRUE, 0, IDC_TIME,			{467, 66,  94, 16},
		TRUE, 0, IDC_SW_REV_T,		{462, 91, 100, 37},
		TRUE, 0, IDC_SW_REV,		{467, 111, 94, 16},

		FALSE, 0, IDC_BOOT_STATUS_T,	{656, 35, 100, 37},
		FALSE, 0, IDC_BOOT_STATUS,	{661, 55,  94, 16},
		FALSE, 0, IDC_DIRECTION_T,	{656, 80, 100, 37},
		FALSE, 0, IDC_DIRECTION,		{661, 100, 94, 16},
		FALSE, 0, IDC_SW_VER_T,		{768, 35, 100, 37},
		FALSE, 0, IDC_SW_VER,		{773, 55,  94, 16},
		FALSE, 0, IDC_SPEED_T,		{768, 80, 100, 37},
		FALSE, 0, IDC_SPEED,			{773, 100, 94, 16},
		FALSE, 0, IDC_ALTITUDE_T,	{880, 35, 100, 37},
		FALSE, 0, IDC_ALTITUDE2_T,	{880, 35, 100, 37},
		FALSE, 0, IDC_ALTITUDE,		{885, 55,  94, 16},
		FALSE, 0, IDC_HDOP_T,		{880, 80, 100, 37},
		FALSE, 0, IDC_HDOP,			{885, 100, 94, 16},
		FALSE, 0, IDC_RTK_AGE_T,		{907, 35, 80, 37},
		FALSE, 0, IDC_RTK_AGE,		{912, 55,  74, 16},
		FALSE, 0, IDC_RTK_RATIO_T,	{907, 80, 80, 37},
		FALSE, 0, IDC_RTK_RATIO,		{912, 100, 74, 16},
		FALSE, 0, IDC_CLOCK_T, {680, 112, 102, 19},
		FALSE, 0, IDC_CLOCK, {680, 132, 102, 24},
		FALSE, 0, IDC_NOISE_T, {680, 112, 102, 19},
		FALSE, 0, IDC_NOISE, {680, 132, 102, 24},

		//Glonass Knum
		FALSE, 0, IDC_KNUM_ENABLE, {699, 113, 82, 22},
		FALSE, 0, IDC_KNUM_DISABLE, {699, 137, 82, 22},
		FALSE, 0, IDC_KNUM_LIST, {794, 0, 182, 170},

		//SNR Chart
		TRUE, 0, IDC_GPS_SNR_T, {228, 146, 180, 18},
		TRUE, 0, IDC_GPS_BAR, {228, 164, 343, 75},

		TRUE, 0, IDC_BEIDOU_SNR_T, {228, 246, 180, 18},
		TRUE, 0, IDC_BD_BAR, {228, 263, 343, 75},
		FALSE, 0, IDC_GA_SNR_T, {656, 234, 180, 18},
		FALSE, 0, IDC_GI_SNR_T, {656, 234, 180, 18},
		FALSE, 0, IDC_GA_BAR, {656, 252, 343, 75},
		FALSE, 0, IDC_GI_BAR, {656, 252, 343, 75},

		//Earth View
		FALSE, 0, IDC_EARTH_T, {301, 332, 106, 21},
		FALSE, 0, IDC_EARTHSETTING, {387, 334, 18, 18},
		FALSE, 0, IDC_EARTH_PANEL, {301, 352, 343, 253},
		FALSE, 0, IDC_EARTH, {332, 353, 312, 251},

		//Scatter View
		FALSE, 0, IDC_SCATTER_SNR_T, {656, 332, 106, 21},
		FALSE, 0, IDC_SCATTER_PANEL, {656, 352, 343, 253},
		FALSE, 0, IDC_SCATTERSETTING, {742, 334, 18, 18},
		FALSE, 0, IDC_DOWNLOAD_PANEL, {301, 629, 698, 55},

		//Title
		FALSE, 0, IDC_DOWNLOAD_T, {301, 611, 100, 19},

		//Scatter
		FALSE, 0, IDC_SCATTER, {657, 354, 258, 250},
		FALSE, 0, IDC_2DRMS_T, {916, 457, 78, 18},
		FALSE, 0, IDC_2DRMS, {915, 476, 78, 18},
		FALSE, 0, IDC_CEP50_T, {916, 497, 78, 18},
		FALSE, 0, IDC_CEP50, {915, 516, 78, 18},
		FALSE, 0, IDC_SETORIGIN, {925, 540, 67, 24},
		FALSE, 0, IDC_CLEAR, {925, 568, 67, 24},
		FALSE, 0, IDC_2DRMS_2, {915, 476, 78, 18},
		FALSE, 0, IDC_CEP50_2, {915, 516, 78, 18},
		FALSE, 0, IDC_CENTER_ALT, {916, 453, 78, 32},
		FALSE, 0, IDC_SCATTER_ALT, {915, 450, 78, 30},

		FALSE, 0, IDC_ENUSCALE_T, {915, 358, 78, 18},
		FALSE, 0, IDC_ENUSCALE, {915, 377, 78, 24},
		FALSE, 0, IDC_MAPSCALE, {915, 377, 78, 24},
		FALSE, 0, IDC_COOR_T, {915, 406, 78, 18},
		FALSE, 0, IDC_COOR, {915, 425, 78, 18},

		//DR
		FALSE, 0, IDC_ODO_METER_T, {848, 454, 80, 18},
		FALSE, 0, IDC_ODO_METER, {848, 472, 66, 24},
		FALSE, 0, IDC_GYRO_DATA_T, {848, 502, 80, 18},
		FALSE, 0, IDC_GYRO_DATA, {848, 520, 66, 24},
		FALSE, 0, IDC_BK_INDICATOR_T, {848, 550, 80, 18},
		FALSE, 0, IDC_BK_INDICATOR, {848, 568, 66, 24},

		//E-Compass Calibration
		FALSE, 0, IDC_ECOM_CALIB, {852, 571, 80, 36},
		FALSE, 0, IDC_ECOM_COUNTER, {852, 608, 80, 30},

		//Download
		FALSE, 0, IDC_DOWNLOAD, {305, 645, 67, 27},
		FALSE, 0, IDC_DL_BAUDRATE, {375, 646, 80, 18},
		FALSE, 0, IDC_BROWSE, {456, 646, 24, 24},
		FALSE, 0, IDC_FIRMWARE_PATH, {485, 650, 513, 33},
		FALSE, 0, IDC_IN_LOADER, {300, 618, 146, 15},
		0, 0, 0, {0, 0, 0, 0}
	};
#else
	const UiLocationEntry UiTable[] =
	{
		//Left Panel
		TRUE, 0, IDC_COMPORT_T, {6, 3, 80, 14},
		TRUE, 0, IDC_BAUDRATE_T, {98, 3, 80, 14},
		TRUE, 0, IDC_COMPORT, {6, 20, 80, 13},
		TRUE, 0, IDC_BAUDRATE_IDX, {98, 20, 80, 13},
		(!NMEA_INPUT), 0, IDC_OPEN_CLOSE_T, {235, 3, 51, 14},
		(!NMEA_INPUT), 0, IDC_CONNECT, {254, 20, 32, 24},
		(!NMEA_INPUT), 0, IDC_CLOSE, {254, 20, 32, 24},
		NMEA_INPUT, 0, IDC_PLAY, {182, 20, 32, 24},
		NMEA_INPUT, 0, IDC_RECORD, {218, 20, 32, 24},
		NMEA_INPUT, 0, IDC_STOP, {254, 20, 32, 24},
		NMEA_INPUT, 0, IDC_PLAY_T, {180, 3, 32, 14},
		NMEA_INPUT, 0, IDC_RECORD_T, {216, 3, 32, 14},
		NMEA_INPUT, 0, IDC_STOP_T, {252, 3, 32, 14},

		TRUE, 0, IDC_MESSAGE_T, {6, 48, 280, 29},
		TRUE, 0, IDC_FIXED_STATUS, {138, 50, 144, 29},
		//TRUE, 0, IDC_MESSAGE, {6, 89, 280, 158},
		//TRUE, 0, IDC_MESSAGE, {6, 82, 280, 162},
		TRUE, 0, IDC_MESSAGE, {6, 78, 280, 173},

		TRUE, 0, IDC_RESPONSE_T, {6, 253, 280, 29},
		TRUE, 0, IDC_RESPONSE, {6, 283, 280, 116},

		TRUE, 0, IDC_COORDINATE_T, {6, 402, 280, 29},
		TRUE, 0, IDC_COORDINATE_F, {6, 432, 280, 126},
		TRUE, 0, IDC_WGS84_X_T, {22, 438, 110, 18},
		TRUE, 0, IDC_WGS84_X, {22, 456, 110, 18},
		TRUE, 0, IDC_WGS84_Y_T, {22, 477, 110, 18},
		TRUE, 0, IDC_WGS84_Y, {22, 495, 110, 18},
		TRUE, 0, IDC_WGS84_Z_T, {22, 516, 110, 18},
		TRUE, 0, IDC_WGS84_Z, {22, 534, 110, 18},
		TRUE, 0, IDC_ENU_E_T, {160, 438, 110, 18},
		TRUE, 0, IDC_ENU_E, {160, 456, 110, 18},
		TRUE, 0, IDC_ENU_N_T, {160, 477, 110, 18},
		TRUE, 0, IDC_ENU_N, {160, 495, 110, 18},
		TRUE, 0, IDC_ENU_U_T, {160, 516, 110, 18},
		TRUE, 0, IDC_ENU_U, {160, 534, 110, 18},

		TRUE, 0, IDC_COMMAND_T, {6, 560, 280, 29},
		TRUE, 0, IDC_COMMAND_F, {6, 590, 280, 81},
#if defined(CUSTOMIZE_COSDSTART_BUTTON)
		FALSE, 0, IDC_HOTSTART, {15, 593, 82, 24},
		FALSE, 0, IDC_WARMSTART, {105, 593, 82, 24},
		TRUE, 0, IDC_COLDSTART, {15, 593, 262, 74},
		FALSE, 0, IDC_NO_OUTPUT, {15, 618, 82, 24},
		FALSE, 0, IDC_NMEA_OUTPUT, {105, 618, 82, 24},
		FALSE, 0, IDC_BIN_OUTPUT, {195, 618, 82, 24},
		FALSE, 0, IDC_SCANALL, {15, 643, 82, 24},
		FALSE, 0, IDC_SCANPORT, {105, 643, 82, 24},
		FALSE, 0, IDC_SCANBAUDRATE, {195, 643, 82, 24},
#else
		TRUE, 0, IDC_HOTSTART, {15, 593, 82, 24},
		TRUE, 0, IDC_WARMSTART, {105, 593, 82, 24},
		TRUE, 0, IDC_COLDSTART, {195, 593, 82, 24},
		TRUE, 0, IDC_NO_OUTPUT, {15, 618, 82, 24},
		TRUE, 0, IDC_NMEA_OUTPUT, {105, 618, 82, 24},
		TRUE, 0, IDC_BIN_OUTPUT, {195, 618, 82, 24},
		TRUE, 0, IDC_SCANALL, {15, 643, 82, 24},
		TRUE, 0, IDC_SCANPORT, {105, 643, 82, 24},
		TRUE, 0, IDC_SCANBAUDRATE, {195, 643, 82, 24},
#endif
		//Earth View
		TRUE, 0, IDC_EARTH_PANEL, {301, 346, 343, 253},
		TRUE, 0, IDC_EARTH, {332, 347, 312, 251},

		//Panel Background
		TRUE, 0, IDC_INFO_PANEL, {301, 24, 698, 104},
		TRUE, 0, IDC_SCATTER_PANEL, {656, 346, 343, 253},
		TRUE, 0, IDC_DOWNLOAD_PANEL, {301, 621, 698, 50},

		//Title
		TRUE, 0, IDC_INFORMATION_T, {301, 6, 100, 19},
		_TAB_LAYOUT_, 0, IDC_INFORMATION_B, {301, 6, 100, 19},
		_TAB_LAYOUT_, 0, IDC_RTK_INFO_T, {401, 6, 100, 19},
		_TAB_LAYOUT_, 0, IDC_RTK_INFO_B, {401, 6, 100, 19},
		TRUE, 0, IDC_EARTH_T, {301, 326, 106, 21},
		TRUE, 0, IDC_SCATTER_SNR_T, {656, 326, 106, 21},
		TRUE, 0, IDC_DOWNLOAD_T, {301, 603, 100, 19},

		//Panel Setting Button
		FALSE, 0, IDC_EARTHSETTING, {387, 334, 18, 18},
		FALSE, 0, IDC_SCATTERSETTING, {742, 334, 18, 18},

		//Information
#if(_TAB_LAYOUT_)
		TRUE, 0, IDC_TTFF_T,		{310, 35, 115, 37},
		TRUE, 0, IDC_TTFF,			{315, 55, 109, 16},
		TRUE, 0, IDC_LONGITUDE_T,	{310, 80, 115, 37},
		TRUE, 0, IDC_LONGITUDE,		{312, 100, 109, 16},
		TRUE, 0, IDC_COOR_SWITCH1,	{407, 80, 18, 18},

		TRUE, 0, IDC_DATE_T,		{435, 35, 115, 37},
		TRUE, 0, IDC_DATE,			{440, 55, 109, 16},
		TRUE, 0, IDC_LATITUDE_T,	{435, 80, 115, 37},
		TRUE, 0, IDC_LATITUDE,		{440, 100, 109, 16},
		TRUE, 0, IDC_COOR_SWITCH2,	{532, 80, 18, 18},

		TRUE, 0, IDC_TIME_T,		{560, 35, 100, 37},
		TRUE, 0, IDC_TIME,			{565, 55,  94, 16},
		TRUE, 0, IDC_ALTITUDE_T,	{560, 80, 100, 37},
		FALSE, 0, IDC_ALTITUDE2_T,	{560, 80, 100, 37},
		TRUE, 0, IDC_ALTITUDE,		{565, 100, 94, 16},
		TRUE, 0, IDC_ALT_SWITCH,	{640, 80, 18, 18},

		TRUE, 0, IDC_BOOT_STATUS_T,	{670, 35, 100, 37},
		TRUE, 0, IDC_BOOT_STATUS,	{675, 55,  94, 16},
		TRUE, 0, IDC_DIRECTION_T,	{670, 80, 100, 37},
		TRUE, 0, IDC_DIRECTION,		{675, 100, 94, 16},

		TRUE, 0, IDC_SW_VER_T,		{780, 35, 100, 37},
		TRUE, 0, IDC_SW_VER,		{785, 55,  94, 16},
		TRUE, 0, IDC_SPEED_T,		{780, 80, 100, 37},
		TRUE, 0, IDC_SPEED,			{785, 100, 94, 16},

		TRUE, 0, IDC_SW_REV_T,		{890, 35, 100, 37},
		TRUE, 0, IDC_SW_REV,		{895, 55,  94, 16},
		TRUE, 0, IDC_HDOP_T,		{890, 80, 100, 37},
		TRUE, 0, IDC_HDOP,			{895, 100, 94, 16},
//=========================================================
		//TRUE, 0, IDC_DATE2_T,		{310, 35, 115, 37},
		//TRUE, 0, IDC_DATE2,			{315, 55, 109, 16},

		//TRUE, 0, IDC_TIME2_T,		{435, 35, 115, 37},
		//TRUE, 0, IDC_TIME2,			{440, 55, 109, 16},

    //20170712 Remove IDC_DATE2_T, Add Cycle-Sliped
		TRUE, 0, IDC_TIME2_T,		{310, 35, 115, 37},
		TRUE, 0, IDC_TIME2,			{315, 55, 109, 16},

		TRUE, 0, IDC_CYCLE_SLIP_T, {435, 35, 115, 37},
		TRUE, 0, IDC_CYCLE_SLIP,	 {440, 55, 109, 16},


		TRUE, 0, IDC_RTK_AGE_T,		{560, 35, 100, 37},
		TRUE, 0, IDC_RTK_AGE,		{565, 55,  94, 16},

		TRUE, 0, IDC_RTK_RATIO_T,	{670, 35, 100, 37},
		TRUE, 0, IDC_RTK_RATIO,		{675, 55,  94, 16},
		TRUE, 0, IDC_EAST_PROJECTION_T,	{670, 80, 100, 37},
		TRUE, 0, IDC_EAST_PROJECTION,	{675, 100, 94, 16},

		TRUE, 0, IDC_BASELINE_LENGTH_T,	{780, 35, 100, 37},
		TRUE, 0, IDC_BASELINE_LENGTH,	{785, 55,  94, 16},
		TRUE, 0, IDC_NORTH_PROJECTION_T,{780, 80, 100, 37},
		TRUE, 0, IDC_NORTH_PROJECTION,	{785, 100, 94, 16},

		TRUE, 0, IDC_BASELINE_COURSE_T,	{890, 35, 100, 37},
		TRUE, 0, IDC_BASELINE_COURSE,	{895, 55,  94, 16},
		TRUE, 0, IDC_UP_PROJECTION_T,	{890, 80, 100, 37},
		TRUE, 0, IDC_UP_PROJECTION,		{895, 100, 94, 16},
#elif (_MORE_INFO_ == 1)
		TRUE, 0, IDC_TTFF_T,		{314, 35, 100, 37},
		TRUE, 0, IDC_TTFF,			{319, 55,  94, 16},
		TRUE, 0, IDC_LONGITUDE_T,	{314, 80, 100, 37},
		TRUE, 0, IDC_LONGITUDE,		{319, 100, 94, 16},
		TRUE, 0, IDC_COOR_SWITCH1,	{396, 80, 18, 18},
		TRUE, 0, IDC_DATE_T,		{426, 35, 100, 37},
		TRUE, 0, IDC_DATE,			{431, 55,  94, 16},
		TRUE, 0, IDC_LATITUDE_T,	{426, 80, 100, 37},
		TRUE, 0, IDC_LATITUDE,		{431, 100, 94, 16},
		TRUE, 0, IDC_COOR_SWITCH2,	{508, 80, 18, 18},

		TRUE, 0, IDC_TIME_T,		{538, 35, 80, 37},
		TRUE, 0, IDC_TIME,			{543, 55,  74, 16},
		TRUE, 0, IDC_ALTITUDE_T,	{538, 80, 80, 37},
		TRUE, 0, IDC_ALTITUDE2_T,	{538, 80, 80, 37},
		TRUE, 0, IDC_ALTITUDE,		{543, 100, 74, 16},
		TRUE, 0, IDC_ALT_SWITCH,	{620, 80, 18, 18},

		TRUE, 0, IDC_BOOT_STATUS_T,	{631, 35, 80, 37},
		TRUE, 0, IDC_BOOT_STATUS,	{636, 55,  74, 16},
		TRUE, 0, IDC_DIRECTION_T,	{631, 80, 80, 37},
		TRUE, 0, IDC_DIRECTION,		{636, 100, 74, 16},
		TRUE, 0, IDC_SW_VER_T,		{723, 35, 80, 37},
		TRUE, 0, IDC_SW_VER,		{728, 55,  74, 16},
		TRUE, 0, IDC_SPEED_T,		{723, 80, 80, 37},
		TRUE, 0, IDC_SPEED,			{728, 100, 74, 16},
		TRUE, 0, IDC_SW_REV_T,		{815, 35, 80, 37},
		TRUE, 0, IDC_SW_REV,		{820, 55,  74, 16},
		TRUE, 0, IDC_HDOP_T,		{815, 80, 80, 37},
		TRUE, 0, IDC_HDOP,			{820, 100, 74, 16},

		TRUE, 0, IDC_RTK_AGE_T,		{907, 35, 80, 37},
		TRUE, 0, IDC_RTK_AGE,		{912, 55,  74, 16},
		TRUE, 0, IDC_RTK_RATIO_T,	{907, 80, 80, 37},
		TRUE, 0, IDC_RTK_RATIO,		{912, 100, 74, 16},

		FALSE, 0, IDC_EAST_PROJECTION_T,	{670, 80, 100, 37},
		FALSE, 0, IDC_EAST_PROJECTION,		{675, 100, 94, 16},

		FALSE, 0, IDC_BASELINE_LENGTH_T,	{780, 35, 100, 37},
		FALSE, 0, IDC_BASELINE_LENGTH,		{785, 55,  94, 16},
		FALSE, 0, IDC_NORTH_PROJECTION_T,	{780, 80, 100, 37},
		FALSE, 0, IDC_NORTH_PROJECTION,		{785, 100, 94, 16},

		FALSE, 0, IDC_BASELINE_COURSE_T,	{890, 35, 100, 37},
		FALSE, 0, IDC_BASELINE_COURSE,		{895, 55,  94, 16},
		FALSE, 0, IDC_UP_PROJECTION_T,		{890, 80, 100, 37},
		FALSE, 0, IDC_UP_PROJECTION,		{895, 100, 94, 16},
#else
		TRUE, 0, IDC_TTFF_T,		{310, 35, 115, 37},
		TRUE, 0, IDC_TTFF,			{315, 55, 109, 16},
		TRUE, 0, IDC_LONGITUDE_T,	{310, 80, 115, 37},
		TRUE, 0, IDC_LONGITUDE,		{312, 100, 109, 16},

		TRUE, 0, IDC_DATE_T,		{435, 35, 115, 37},
		TRUE, 0, IDC_DATE,			{440, 55, 109, 16},
		TRUE, 0, IDC_LATITUDE_T,	{435, 80, 115, 37},
		TRUE, 0, IDC_LATITUDE,		{440, 100, 109, 16},

		TRUE, 0, IDC_TIME_T,		{560, 35, 100, 37},
		TRUE, 0, IDC_TIME,			{565, 55,  94, 16},
		TRUE, 0, IDC_ALTITUDE_T,	{560, 80, 100, 37},
		TRUE, 0, IDC_ALTITUDE2_T,	{560, 80, 100, 37},
		TRUE, 0, IDC_ALTITUDE,		{565, 100, 94, 16},

		TRUE, 0, IDC_BOOT_STATUS_T,	{670, 35, 100, 37},
		TRUE, 0, IDC_BOOT_STATUS,	{675, 55,  94, 16},
		TRUE, 0, IDC_DIRECTION_T,	{670, 80, 100, 37},
		TRUE, 0, IDC_DIRECTION,		{675, 100, 94, 16},

		TRUE, 0, IDC_SW_VER_T,		{780, 35, 100, 37},
		TRUE, 0, IDC_SW_VER,		{785, 55,  94, 16},
		TRUE, 0, IDC_SPEED_T,		{780, 80, 100, 37},
		TRUE, 0, IDC_SPEED,			{785, 100, 94, 16},

		TRUE, 0, IDC_SW_REV_T,		{890, 35, 100, 37},
		TRUE, 0, IDC_SW_REV,		{895, 55,  94, 16},
		TRUE, 0, IDC_HDOP_T,		{890, 80, 100, 37},
		TRUE, 0, IDC_HDOP,			{895, 100, 94, 16},

		FALSE,0, IDC_RTK_AGE_T,		{907, 35, 80, 37},
		FALSE,0, IDC_RTK_AGE,		{912, 55,  74, 16},
		FALSE,0, IDC_RTK_RATIO_T,	{907, 80, 80, 37},
		FALSE,0, IDC_RTK_RATIO,		{912, 100, 74, 16},
#endif
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
		TRUE, 0, IDC_GPS_SNR_T, {301, 133, 180, 18},
		TRUE, 0, IDC_BEIDOU_SNR_T, {301, 230, 180, 18},
		FALSE, 0, IDC_GA_SNR_T, {656, 230, 180, 18},
		TRUE, 0, IDC_GI_SNR_T, {656, 230, 180, 18},

		//SNR Chart
		TRUE, 0, IDC_GPS_BAR, {301, 151, 698, 75},
#if(SUPPORT_BDL2_GSV2)
		TRUE, 0, IDC_BD_BAR, {301, 248, 698, 75},
		FALSE, 0, IDC_GA_BAR, {656, 248, 343, 75},
		FALSE, 0, IDC_GI_BAR, {656, 248, 343, 75},
#else
		TRUE, 0, IDC_BD_BAR, {301, 248, 343, 75},
		FALSE, 0, IDC_GA_BAR, {656, 248, 343, 75},
		TRUE, 0, IDC_GI_BAR, {656, 248, 343, 75},
#endif

		//Scatter
		TRUE, 0, IDC_SCATTER, {657, 348, 258, 250},

#if (_MORE_INFO_ == 1)
		TRUE, 0, IDC_2DRMS_T, {916, 483, 78, 31},
		TRUE, 0, IDC_2DRMS_2, {917, 499, 76, 15},
		TRUE, 0, IDC_CEP50_T, {916, 520, 78, 31},
		TRUE, 0, IDC_CEP50_2, {917, 536, 76, 15},
		IS_DEBUG, 0, IDC_CENTER_ALT, {915, 450, 78, 30},
		TRUE, 0, IDC_SETORIGIN, {927, 553, 67, 24},
		TRUE, 0, IDC_CLEAR, {927, 578, 67, 24},
		FALSE,0, IDC_2DRMS, {915, 476, 78, 18},
		FALSE,0, IDC_CEP50, {915, 516, 78, 18},
		FALSE, 0, IDC_SCATTER_ALT, {915, 450, 78, 30},
#else
		TRUE, 0, IDC_2DRMS_T, {916, 457, 78, 18},
		TRUE, 0, IDC_2DRMS, {915, 476, 78, 18},
		TRUE, 0, IDC_CEP50_T, {916, 497, 78, 18},
		TRUE, 0, IDC_CEP50, {915, 516, 78, 18},
		TRUE, 0, IDC_SETORIGIN, {925, 540, 67, 24},
		TRUE, 0, IDC_CLEAR, {925, 568, 67, 24},
		FALSE,0, IDC_2DRMS_2, {915, 476, 78, 18},
		FALSE,0, IDC_CEP50_2, {915, 516, 78, 18},
		FALSE, 0, IDC_CENTER_ALT, {916, 453, 78, 32},
		FALSE, 0, IDC_SCATTER_ALT, {915, 450, 78, 30},
#endif

		TRUE, 0, IDC_ENUSCALE_T, {915, 358, 78, 18},
		TRUE, 0, IDC_ENUSCALE, {915, 377, 78, 24},
		TRUE, 0, IDC_MAPSCALE, {915, 377, 78, 24},
		TRUE, 0, IDC_COOR_T, {915, 406, 78, 18},
		TRUE, 0, IDC_COOR, {915, 425, 78, 18},

		//DR
		SOFTWARE_FUNCTION & SW_FUN_DR, 0, IDC_ODO_METER_T, {915, 405, 80, 18},
		SOFTWARE_FUNCTION & SW_FUN_DR, 0, IDC_ODO_METER, {915, 421, 66, 24},
		SOFTWARE_FUNCTION & SW_FUN_DR, 0, IDC_GYRO_DATA_T, {916, 450, 80, 18},
		SOFTWARE_FUNCTION & SW_FUN_DR, 0, IDC_GYRO_DATA, {916, 466, 66, 24},
		SOFTWARE_FUNCTION & SW_FUN_DR, 0, IDC_BK_INDICATOR_T, {916, 495, 80, 18},
		SOFTWARE_FUNCTION & SW_FUN_DR, 0, IDC_BK_INDICATOR, {916, 511, 66, 24},

		//E-Compass Calibration
		ECOMPASS_CALIBRATION, 0, IDC_ECOM_CALIB, {852, 571, 80, 36},
		ECOMPASS_CALIBRATION, 0, IDC_ECOM_COUNTER, {852, 608, 80, 30},

		//Download
		FIRMWARE_DOWNLOAD, 0, IDC_DOWNLOAD, {305, 632, 67, 27},
		FIRMWARE_DOWNLOAD, 0, IDC_DL_BAUDRATE, {375, 633, 80, 18},

		FIRMWARE_DOWNLOAD, 0, IDC_BROWSE, {456, 633, 24, 24},
		FIRMWARE_DOWNLOAD, 0, IDC_FIRMWARE_PATH, {485, 637, 513, 33},

		0, 0, IDC_IN_LOADER, {300, 618, 146, 15},

		0, 0, 0, {0, 0, 0, 0}
	};
#endif

	CRect rcWin, rcClient, rcNewSize;
	CSize szClient(CLIENT_WIDTH, CLIENT_HEIGHT);

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

		//if(p->showOption)
		//{
			pWnd->MoveWindow(&rcWnd);
		//}
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
	if(!NMEA_INPUT)
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
	if(duration > timeOut)
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

void CGPSDlg::OnHostLog()
{
	if(!CGPSDlg::gpsDlg->m_isConnectOn)
	{
		AfxMessageBox("Please connect to GNSS device");
		return;
	}

	if(m_saveNmeaDlg)
	{
		::AfxMessageBox("Save function has been activated!");
		return;
	}

	CTime t = CTime::GetCurrentTime();
	CString fileName;
	fileName.Format("Host%02d-%02d-%02d_%02d%02d%02d.dat", t.GetYear(), t.GetMonth(), t.GetDay(),
		t.GetHour(), t.GetMinute(), t.GetSecond());

	CFileDialog dlgFile(FALSE, _T("*.dat"), fileName, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		_T("ALL Files (*.*)|*.*||"), this);

	dlgFile.GetOFN().lpstrFile = fileName.GetBuffer(MyMaxPath);
	dlgFile.GetOFN().nMaxFile = MyMaxPath;
	INT_PTR nResult = dlgFile.DoModal();
	fileName.ReleaseBuffer();

	if(nResult != IDOK)
	{
		return;
	}

  CSaveNmea dlg;
  m_saveNmeaDlg = &dlg;
	UWM_SAVENMEA_EVENT = dlg.RegisterEventMessage();
	dlg.SetNotifyWindow(this->GetSafeHwnd());
  dlg.StartSave(CSaveNmea::HostLog_Mode, fileName, fileName);
	dlg.DoModal();
  m_saveNmeaDlg = NULL;
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

#define DumpFileName		"\\Dump0x50000000.bin"
#define DumpFileName2		"\\Dump0x60000000.bin"
bool CGPSDlg::WriteAddressToFile(const CString& filename, const U32 start, const U32 size)
{
	CString path = theApp.GetCurrrentDir() + DumpFileName;
	U32 addr = start;
	U32 data = 0;
	U08 dataW[4] = {0};
	bool suc = true;
	CFile f;
	f.Open(path, CFile::modeCreate | CFile::modeWrite);
	for(U32 i = 0; i < size; i += 4)
	{
		m_regAddress = addr + i;
    if(m_regAddress == 0x500010f4)
    {
      int a = 0;
    }
		CmdErrorCode ack = QueryRegister(Return, &data);
		if(ack != Ack) 
		{
			suc = false;
			break;
		}
		//Byte order change
		dataW[0] = ((U08*)&data)[3];
		dataW[1] = ((U08*)&data)[2];
		dataW[2] = ((U08*)&data)[1];
		dataW[3] = ((U08*)&data)[0];
		f.Write(dataW, sizeof(dataW));
		//Directly
		//f.Write(&data, sizeof(data));

		if(i % 0x200 == 0)
		{
			CString txt;
			txt.Format("Reading 0x%08X/0x%08X", m_regAddress, start + size);
			add_msgtolist(txt);
			Utility::DoEvents();
		}
	}
	f.Close();
  return suc;
}

bool CGPSDlg::PatchRomEphemeris()
{
  int i;
  U32 eph_wn = 0;
  U32 addr, val;
  int has_eph = 0;
  U32 regAddBackup = m_regAddress; 
  CmdErrorCode ack = Ack;


  for(i = 0; i < 32; ++i)
  {
    U32 eph_addr = (0x50000908UL) + i * (0x80UL);

    addr = eph_addr + (0x40UL);
    m_regAddress = addr;
    CmdErrorCode ack = QueryRegister(Return, &val);
		if(ack != Ack) 
		{
		  goto PatchRomEphemerisEnd;
		}
    val = val & (1UL << 24);

    if(val)
    { 
      addr = eph_addr + (0x7CUL);
      m_regAddress = addr;
      ack = QueryRegister(Return, &val);
		  if(ack != Ack) 
		  {
		    goto PatchRomEphemerisEnd;
		  }
      val = (val << 8UL) >> 24UL;

      if(!val) //0: satellite health
      { 
        addr = eph_addr + (0x3CUL);
        m_regAddress = addr;
        ack = QueryRegister(Return, &val);
		    if(ack != Ack) 
		    {
		      goto PatchRomEphemerisEnd;
		    }
        eph_wn = (val << 16UL) >> 16UL;
        has_eph = 1;
        break;
      }
    }
  }

  if(has_eph)
  {
    U32 wn = (eph_wn >= 940) ? (eph_wn + 1024) : (eph_wn + 2048);
    U32 raw_wn, gtc_wn_diff;
    m_regAddress = 0x600123B0UL;
    ack = QueryRegister(Return, &val);
	  if(ack != Ack) 
	  {
      has_eph = 0;
		  goto PatchRomEphemerisEnd;
	  }
    raw_wn = (val << 16UL) >> 16UL;
    gtc_wn_diff = wn - raw_wn;

    if(raw_wn == 0 && gtc_wn_diff < 0) 
      gtc_wn_diff = 0;

    //Gtc wn diff
    if(!WriteRegister(0x600123A4UL, gtc_wn_diff))
    {
      has_eph = 0;
		  goto PatchRomEphemerisEnd;
    }

    //Gtc wn adjust type
    m_regAddress = 0x6001239CUL;
    ack = QueryRegister(Return, &val);
	  if(ack != Ack) 
	  {
      has_eph = 0;
		  goto PatchRomEphemerisEnd;
	  }
    val = val |  (0x00050000UL);
    if(!WriteRegister(0x6001239CUL, val))
    {
      has_eph = 0;
		  goto PatchRomEphemerisEnd;
    }
  }
PatchRomEphemerisEnd:
  m_regAddress = regAddBackup;
  return (has_eph != 0);
}

void CGPSDlg::OnReadMemToFile()
{
	if(!CheckConnect())
	{
		return;
	}
  CString path = theApp.GetCurrrentDir() + DumpFileName;
  bool suc = WriteAddressToFile(path, 0x50000000, 20 * 1024);

	if(suc)
		add_msgtolist("Read memory to a file successfully");
	else
		add_msgtolist("Read memory to a file failed");	

	SetMode();
	CreateGPSThread();
}

void CGPSDlg::OnReadMemToFile2()
{
	if(!CheckConnect())
	{
		return;
	}
  CString path = theApp.GetCurrrentDir() + DumpFileName2;
  bool suc = WriteAddressToFile(path, 0x60000000, 1 * 1024);

	if(suc)
		add_msgtolist("Read memory to a file successfully");
	else
		add_msgtolist("Read memory to a file failed");	

	SetMode();
	CreateGPSThread();
}

void CGPSDlg::OnPatchRom20130221Ephemeris()
{
	if(!CheckConnect())
	{
		return;
	}
  bool suc = PatchRomEphemeris();

	if(suc)
		add_msgtolist("Patch ROM ephemeris successfully");
	else
		add_msgtolist("Patch ROM ephemeris failed");	

	SetMode();
	CreateGPSThread();
}

void CGPSDlg::OnIoTester()
{
	if(!CheckConnect())
	{
		return;
	}

	CIoTester dlg;
	INT_PTR ret = dlg.DoModal();

	SetMode();
	CreateGPSThread();
}

void CGPSDlg::OnWriteMemToFile()
{
	CString strPath = theApp.GetCurrrentDir() + DumpFileName;
	CFile f;

	if(!Utility::IsFileExist(strPath))
	{
		strPath.Empty();
	}
	CFileDialog fd(true, "*.bin", strPath, OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, "*.bin|*.bin||");
	if(fd.DoModal() == IDOK)
	{
		strPath = fd.GetPathName();
		if(0 == f.Open(strPath, CFile::modeRead))
		{
			AfxMessageBox("Can't open bin file!");
			return;
		}
		ULONGLONG usize = f.GetLength();
		if(f.GetLength() != 20480)
		{
			f.Close();
			AfxMessageBox("File size must be 20480 bytes!");
			return;
		}
	}
	else
	{
		return;
	}

	if(!CheckConnect())
	{
		f.Close();
		return;
	}

	U32 addr = 0x50000000;
	U32 data = 0;
	U32 dataW = 0;
	bool suc = true;

	for(int i = 0; i < 20 * 1024; i += 4)
	{
		f.Read(&data, sizeof(data));
		U32 regAddress = addr + i;
		//Byte order change
		((U08*)&dataW)[0] = ((U08*)&data)[3];
		((U08*)&dataW)[1] = ((U08*)&data)[2];
		((U08*)&dataW)[2] = ((U08*)&data)[1];
		((U08*)&dataW)[3] = ((U08*)&data)[0];

		//Directly
		//CmdErrorCode ack = QueryRegister(Return, &data);
		bool ack = WriteRegister(regAddress, dataW);
		if(!ack) 
		{
			suc = false;
			break;
		}

		if(i % 0x200 == 0)
		{
			CString txt;
			txt.Format("Writting 0x%08X/0x%08X", regAddress, 0x50000000 + 20 * 1024);
			add_msgtolist(txt);
			Utility::DoEvents();
		}

	}
	f.Close();
	if(suc)
		add_msgtolist("Wrote file to memory successfully");
	else
		add_msgtolist("Wrote file to memory failed");	

	SetMode();
	CreateGPSThread();
}

void CGPSDlg::SetNmeaUpdated(bool b)
{
	m_isNmeaUpdated = b;
}

bool CGPSDlg::SetFirstDataIn(bool b)
{
	if(!m_firstDataIn && b)
	{
		PostMessage(UWM_FIRST_NMEA, 800, 0);
		m_firstDataIn = true;
		return true;
	}
	return false;
}

LRESULT CGPSDlg::OnFirstNmea(WPARAM wParam, LPARAM lParam)
{
	if(DOWNLOAD_IMMEDIATELY)
	{
		Sleep(100);
		OnBnClickedDownload();
		return 0;
	}

	if(wParam)
	{
    if(!CUSTOMER_ZENLANE_160808)
    {
		  SetTimer(DELAY_QUERY_TIMER, (UINT)wParam, NULL);
    }
	}
	else
	{
		GetGPSStatus();
	}
	DeleteNmeaMemery();
	ClearInformation();
	m_ttffCount = 0;
	m_initTtff = false;
	SetTTFF(0);
	ClearGlonass();

	if(lParam != 0 && IS_DEBUG)
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

LRESULT CGPSDlg::OnShowRMCTime(WPARAM wParam, LPARAM lParam)
{
  ShowRMCTime();
	return 0;
}

LRESULT CGPSDlg::OnUpdateUI(WPARAM wParam, LPARAM lParam)
{
	gpsSnrBar->Invalidate(FALSE);
	bdSnrBar->Invalidate(FALSE);
#if(!SUPPORT_BDL2_GSV2)
	gaSnrBar->Invalidate(FALSE);
	giSnrBar->Invalidate(FALSE);
#endif
  pic_earth->Invalidate(FALSE);
	return 0;
}

//LRESULT CGPSDlg::OnGpsdoHiDownload(WPARAM wParam, LPARAM lParam)
//{
//	WaitForSingleObject(g_gpsThread, INFINITE);
//	m_gpsdoInProgress = true;
//	if(!DoDownload(7))
//	{
//		m_gpsdoInProgress = false;
//	}
//
//	return 0;
//}

LRESULT CGPSDlg::OnDoZenlaneCmd(WPARAM wParam, LPARAM lParam)
{
  if(!CUSTOMER_ZENLANE_160808)
  {
    return 0;
  }

  if(wParam == 0)
  {
		SetTimer(ZENLANE_INIT_TIMER, 1000, NULL);
  }
  else if(wParam == 1)
  {
		SetTimer(ZENLANE_QUERY_TIMER, 1000, NULL);
  }
	return 0;
}

void CGPSDlg::GetGPSStatus()
{
	if(!g_setting.autoQueryVersion && !AUTO_QUERY_VERSION)
	{	//Not need auto query version.
		return;
	}
	if(m_nmeaPlayThread)
	{	//Player mode doesn't need query version.
		return;
	}
	if(m_serial == NULL)
	{	//Close GPSThread very fast. The connection is cloesd.
		return;
	}
	QuerySoftwareVersionSystemCode(NoWait, NULL);
	Sleep(60);
	QueryGnssBootStatus(NoWait, NULL);
}

bool CGPSDlg::DoZenlandInit()
{
	if(!CUSTOMER_ZENLANE_160808)
	{	
		return false;
	}
	if(m_nmeaPlayThread)
	{	//Player mode doesn't need query version.
		return false;
	}
	if(m_serial == NULL)
	{	//Close GPSThread very fast. The connection already cloesd.
		return false;
	}
	return (Ack == SendZenlandInitCmd(NoWait, NULL));
}

bool CGPSDlg::DoZenlandQuery()
{
	if(!CUSTOMER_ZENLANE_160808)
	{	
		return false;
	}
	if(m_nmeaPlayThread)
	{	//Player mode doesn't need query version.
		return false;
	}
	if(m_serial == NULL)
	{	//Close GPSThread very fast. The connection already cloesd.
		return false;
	}
	return (Ack == SendZenlandQueryCmd(NoWait, NULL));
}

void CGPSDlg::SetConnectTitle(bool isInConnect)
{
	m_connectT.SetWindowText((isInConnect) ? "Connect" : "Close");
}

void CGPSDlg::NmeaOutput(LPCTSTR pt, int len)
{
	m_nmeaList.AddTextAsync(pt);
}

void CGPSDlg::OnBnClickedSetOrigin()
{
	g_scatterData.SetOrigin();
}

static HANDLE hIQNamedFile = INVALID_HANDLE_VALUE;
static CString strIQPipeName;
void CreateIQNamedPipe()
{
	if(hIQNamedFile != INVALID_HANDLE_VALUE)
		return;
	strIQPipeName.Format("//./pipe/%s", Utility::GetNameAttachPid("SkytraqIQPlotPipe"));
	hIQNamedFile = CreateFile(strIQPipeName, GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE , NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL, NULL);
}

void CloseIQNamedPipe()
{
	if(hIQNamedFile == INVALID_HANDLE_VALUE)
		return;

	CloseHandle(hIQNamedFile);
	hIQNamedFile = INVALID_HANDLE_VALUE;
}

void SendToNamedPipe(U08 *buffer, int length)
{
	if(hIQNamedFile == INVALID_HANDLE_VALUE)
	{
		CreateIQNamedPipe();
	}

	if(hIQNamedFile == INVALID_HANDLE_VALUE)
		return;

	DWORD dwWrite = 0;
	if(!WriteFile(hIQNamedFile, buffer, length, &dwWrite, NULL))
	{
		hIQNamedFile = INVALID_HANDLE_VALUE;
	}
}

bool CGPSDlg::ShowCommand(U08 *buffer, int length)
{
	if(length >= 9 && 0x83 == buffer[4])	//Query ACK
	{
		if(m_bShowBinaryCmdData)
		{
			add_msgtolist("Ack : " + theApp.GetHexString(buffer, length));
		}
    if(buffer[5] == 1)
    {
      CSaveNmea::isRestartAck = TRUE;
      add_msgtolist("System Restart successfully");
    }
		return true;
	}
	else if(length >= 9 && 0x84 == buffer[4])	//Query NACK
	{
		if(m_bShowBinaryCmdData)
		{
			add_msgtolist("NACK : " + theApp.GetHexString(buffer, length));
		}
		return true;
	}
	else if(length >= 21 && 0x80 == buffer[4])	//Query Version Return
	{
		if(m_bShowBinaryCmdData)
		{
			add_msgtolist("Return : " + theApp.GetHexString(buffer, length));
		}
		m_versionInfo.ReadFromMemory(buffer, length);
		return true;
	}
	else if(length >= 11 && 0x64 == buffer[4] && 0x80 == buffer[5])	//Query Version Return
	{
		if(m_bShowBinaryCmdData)
		{
			add_msgtolist("Return : " + theApp.GetHexString(buffer, length));
		}
		m_bootInfo.ReadFromMemory(buffer, length);
		return true;
	}
	else if(length >= 14 && 0x64 == buffer[4] && 0xFD == buffer[5])	//IQ Info
	{
		SendToNamedPipe(buffer, length);
		return true;
	}
	return false;
}

void CGPSDlg::MSG_PROC()
{
	static MsgMode msgMode = NoOutputMode;
	static U08 buffer[COM_BUFFER_SIZE] = { 0 };
	int length = 0;

	//m_lastNmeaToken = MSG_Unknown;
	ClearQue();
	CreateIQNamedPipe();

	while(m_isConnectOn)
	{
    if(m_nDoFlag)
    {
      switch(m_nDoFlag)
      {
      case DO_QUERY_VERSION:
        GetGPSStatus();
        m_nDoFlag = DO_NOTHING;
        break;
      case DO_ZENLANE_INIT:
        if(DoZenlandInit())
        {
          m_nDoFlag = DO_ZENLANE_QUERY;
        }
        else
        {
          m_nDoFlag = DO_NOTHING;
		      PostMessage(UWM_DO_ZENLANE_CMD, 0, 0);
        }
        break;
      case DO_ZENLANE_QUERY:
        if(DoZenlandQuery())
        {
          m_nDoFlag = DO_NOTHING;
        }
        else
        {
          m_nDoFlag = DO_NOTHING;
		      PostMessage(UWM_DO_ZENLANE_CMD, 1, 0);
        }
        break;
      default:
        break;
      }
    }
		//Get one line from stream.
    if(m_serial == NULL) break;

    if(CUSTOMER_ZENLANE_160808)
    {
#if CUSTOMER_ZENLANE_160808
		  length = m_serial->GetZenlaneMessage(buffer, sizeof(buffer) - 1);
#endif
    }
    else
    {
      //memset(buffer, 0, sizeof(buffer));
		  length = m_serial->GetBinary(buffer, sizeof(buffer) - 1);
    }

		if(ReadOK(length))
		{
		  buffer[length] = 0;
			bool ret = CSaveNmea::SaveBinary(buffer, length);
		}
		else
		{
			Sleep(10);
			continue;
		}
    if(CUSTOMER_ZENLANE_160808)
    {
      if(buffer[0] == 0xE9 && buffer[1] == 0x0E && buffer[2] == 0x01)
      {
        memcpy(&buffer[0], &buffer[19], length - 19);
      }
    }

    bool pasing = PreprocessInputLine(buffer, length);

		switch(m_inputMode)
		{
		case NmeaMessageMode:
			if(SetFirstDataIn(true))
			{	//First data in notification
				NmeaOutput((LPCSTR)buffer, length);
				continue;
			}
			//It's binary command, show it on response.
			if(buffer[0] == 0xA0 && buffer[1] == 0xA1)
			{
				if(ShowCommand(buffer, length))
				{
					continue;
				}

				if(BINMSG_ERROR != BinaryProc(buffer, length + 1))
        {
				  //Switch to Binary message mode
				  if (msgMode != BinaryMessageMode)
				  {
					  //m_nmeaList.ClearAllText();
            InitMessageBox(BinaryMessageMode);
				    msgMode = BinaryMessageMode;
          }
				  Copy_NMEA_Memery();
				  SetNmeaUpdated(true);
        }
			}
			else
			{
				if(!m_isConnectOn)
				{
					break;
				}

				NmeaType nmeaType = MSG_ERROR;
        if(pasing)
        {
          CSaveNmea::SaveText(buffer, length);

          length = strlen((LPCSTR)buffer);
				  if(NmeaProc((const char*)buffer, length, nmeaType))
				  {
				    if (msgMode != NmeaMessageMode)
				    {
					    //m_nmeaList.ClearAllText();
              InitMessageBox(NmeaMessageMode);
					    msgMode = NmeaMessageMode;
			      }
					  Copy_NMEA_Memery();
					  SetNmeaUpdated(true);
				  }
        }
        else
        {
          if(AllPrintable((LPCSTR)buffer, length))
          {
            CSaveNmea::SaveText(buffer, length);
            NmeaOutput((LPCSTR)buffer, length);
          }
        }

				if(MSG_ERROR==nmeaType && m_firstDataIn && g_setting.checkNmeaError)
				{
					add_msgtolist("Detect NMEA checksum error:");
					add_msgtolist((const char*)buffer);
				}
			}
			break;
		case BinaryMessageMode:
			if(BINMSG_ERROR != BinaryProc(buffer, length + 1))
      {
			  Copy_NMEA_Memery();
			  SetNmeaUpdated(true);
      }
		  break;
		default:
			break;
		}
	}
	CloseIQNamedPipe();
	m_isConnectOn = false;
}

void CGPSDlg::DoFlag()
{
  switch(m_nDoFlag)
  {
  case DO_QUERY_VERSION:
    GetGPSStatus();
    m_nDoFlag = DO_NOTHING;
    break;
  case DO_ZENLANE_INIT:
    if(DoZenlandInit())
    {
      m_nDoFlag = DO_ZENLANE_QUERY;
    }
    else
    {
      m_nDoFlag = DO_NOTHING;
      PostMessage(UWM_DO_ZENLANE_CMD, 0, 0);
    }
    break;
  case DO_ZENLANE_QUERY:
    if(DoZenlandQuery())
    {
      m_nDoFlag = DO_NOTHING;
    }
    else
    {
      m_nDoFlag = DO_NOTHING;
      PostMessage(UWM_DO_ZENLANE_CMD, 1, 0);
    }
    break;
  default:
    break;
  }
}

static DWORD ReadSerialOneChar(U08* c, int timeout)
{
#ifdef _DEBUG
  const int dbgMode = 0;
#else
  const int dbgMode = 0;
#endif
  DWORD dwBytesRead = 0;

  if(dbgMode)
  {
    static CFile* ff = NULL;
    if(ff == NULL)
    {
      ff = new CFile("G:\\gpsData.dat", CFile::modeRead);
    }
    if(ff)
    {
      dwBytesRead = ff->Read(c, 1);
      if(dwBytesRead == 0)
      {
        ff->Close();
        ff = NULL;
        return MessageParser::READERROR;
      }
      return dwBytesRead;
    }
  }

  if(CGPSDlg::gpsDlg->m_serial->GetOneChar(c, &dwBytesRead, timeout))
  {
    return dwBytesRead;
  }
  else
  {
    return MessageParser::READERROR;
  }
}

static DWORD ReadFileOneChar(U08* c, int timeout)
{
  DWORD dwBytesRead = 0;
  dwBytesRead = playFile.Read(c, 1);
  if(dwBytesRead == 0)
  {
    return MessageParser::READERROR;
  }
  return dwBytesRead;
}

void CGPSDlg::ParsingMessage(BOOL isPlayer)
{
	static MsgMode msgMode = NoOutputMode;
	static U08 buffer[COM_BUFFER_SIZE] = { 0 };
	DWORD length = 0;
  MessageType lastType = MtUnknown;
  MessageParser mp;
  //For Player
	CTime playTime(2000, 1, 1, 12, 0, 0);
	bool needSleep = false;
	DWORD currentTick = 0;
	DWORD startTick = ::GetTickCount();
	int lineCount = 0;
  DWORD total = (isPlayer) ?(DWORD)playFile.GetLength() : 1;

  if(!isPlayer)
  {
	  ClearQue();
	  CreateIQNamedPipe();
  }

  mp.SetCancelTransmission((isPlayer) ? &g_nmeaInputTerminate : m_serial->GetCancelTransmissionPtr());
  mp.SetReadOneCharCallback((isPlayer) ? ReadFileOneChar : ReadSerialOneChar);
	while(m_isConnectOn && (isPlayer || m_serial != NULL))
	{
    if(m_nDoFlag && !isPlayer)
    {
      DoFlag();
    }

    MessageType type = mp.GetParsingData(buffer, sizeof(buffer) - 1, &length, 1000);
    if(isPlayer && type == MtReadError)
    {
      break;
    }
		if(0 == length)
		{
			Sleep(10);
			continue;
		}
    else if (MessageParser::READERROR == length)
    { //Cancel
      break;
    }
		else if(!isPlayer)
		{
			CSaveNmea::SaveBinary(buffer, length);
		}

		NmeaType nmeaType = MSG_ERROR;
		switch(type)
		{
		case StqBinary:
			if(!ShowCommand(buffer, length))
			{
			  if(BINMSG_ERROR != BinaryProc(buffer, length + 1))
        {
          if(lastType != type)
          {
            InitMessageBox(BinaryMessageMode);
            lastType = type;
          }
			    Copy_NMEA_Memery();
			    SetNmeaUpdated(true);
        }
      }
		  break;
		case RtcmMessage:
			  if(BINMSG_ERROR != RtcmProc(buffer, length + 1))
        {
          if(lastType != type)
          {
            InitMessageBox(BinaryMessageMode);
            lastType = type;
          }
			    Copy_NMEA_Memery();
			    SetNmeaUpdated(true);
        }
		  break;
		case UbloxMessage:
			  if(BINMSG_ERROR != UbloxProc(buffer, length + 1))
        {
          if(lastType != type)
          {
            InitMessageBox(BinaryMessageMode);
            lastType = type;
          }
			    Copy_NMEA_Memery();
			    SetNmeaUpdated(true);
        }
		  break;
		case NmeaMessage:
			//It's binary command, show it on response.
      CSaveNmea::SaveText(buffer, length);

      length = strlen((LPCSTR)buffer);
		  if(NmeaProc((const char*)buffer, length, nmeaType))
		  {
        if(lastType != type)
        {
          InitMessageBox(NmeaMessageMode);
          lastType = type;
        }
			  Copy_NMEA_Memery();
			  SetNmeaUpdated(true);
			  SetFirstDataIn(true);
		  }

			if(MSG_ERROR==nmeaType && m_firstDataIn && g_setting.checkNmeaError)
			{
				add_msgtolist("Detect NMEA checksum error:");
				add_msgtolist((const char*)buffer);
			}
      break;
    default:
      if(AllPrintable((LPCSTR)buffer, length))
      {
        CSaveNmea::SaveText(buffer, length);
        NmeaOutput((LPCSTR)buffer, length);
      }
			if(m_firstDataIn && g_setting.checkNmeaError)
			{
				add_msgtolist("Unknown message:");
				add_msgtolist((const char*)buffer);
			}
			break;
		}
    if(isPlayer)
    {
		  CTime t = GetPlayTime();
		  if(t > playTime)
		  {
			  needSleep = true;
			  playTime = t;
		  }
      else if(t < playTime)
      {
        playTime = t;
      }
		  currentTick = ::GetTickCount();
		  if(needSleep)
		  {
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
				  }
				  startTick = ::GetTickCount();
				  if(g_nmeaInputTerminate)
				  {
					  break;
				  }
			  } while(nDuration);
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
      DWORD current = (DWORD)playFile.GetPosition();

		  CGPSDlg::gpsDlg->m_playNmea->SetLineCount(++lineCount, current, total);
    }

	}
  if(!isPlayer)
  {
	  CloseIQNamedPipe();
  }
	m_isConnectOn = false;
}

bool CGPSDlg::QueryDataLogBoundary(U16 *end, U16 *total)
{
	int nackTimes = 0;

	U08 cmd[1] = {0x17};	//query logstaus
	U08 message[8] = {0};
	int len = SetMessage2(message, cmd, sizeof(cmd));

	for(int i=0; i<30; ++i)
	{
		if(!SendToTarget(message, len, "", true))
		{
			continue;
		}

		while(1)
		{
			U08 buff[512] = {0};
			m_serial->GetBinaryAck(buff, sizeof(buff));
			if(Cal_Checksum(buff) != BINMSG_REPLY_LOG_STATUS)
			{
				Utility::LogFatal(__FUNCTION__, "[DataLog] Cal_Checksum error", __LINE__);
				continue;
			}

			U16 left;
			memcpy(&left, &buff[9], sizeof(U16));
			memcpy(total, &buff[11], sizeof(U16));
			if(left == 0)
			{
				*end = *total - left;
			}
			else
			{
				*end = *total - left + 1;
			}
			return true;
		}
		Utility::LogFatal(__FUNCTION__, "[DataLog] impossible fail i = ", i);
		break;
	} //for(int i=0; i<30; ++i)
	Utility::LogFatal(__FUNCTION__, "[DataLog] return false", __LINE__);
	return false;
}

BOOL CGPSDlg::PreTranslateMessage(MSG* pMsg)
{
	m_tip.RelayEvent(pMsg);

	if(pMsg->message==WM_KEYDOWN &&
		(pMsg->wParam==VK_ESCAPE || pMsg->wParam==VK_RETURN))
	{
		return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg);
}

LRESULT CGPSDlg::OnUpdateRtkInfo(WPARAM wParam, LPARAM lParam)
{
	F32 rtkAge = *((F32 *)&wParam);
	F32 rtkRatio =  *((F32 *)&lParam);

	CString temp;
	temp.Format("%.1f", rtkAge);
	m_rtkAge.SetWindowText(temp);

	temp.Format("%.1f", rtkRatio);
	m_rtkRatio.SetWindowText(temp);

	return 0;
}

LRESULT CGPSDlg::OnUpdatePsti030(WPARAM wParam, LPARAM lParam)
{
	PSTI030_Data* psti030 = (PSTI030_Data*)wParam;
	CString txt;

	txt.Format("%.1f", psti030->rtkAge);
	m_rtkAge.SetWindowText(txt);
	txt.Format("%.1f", psti030->rtkRatio);
	m_rtkRatio.SetWindowText(txt);

	return 0;
}

LRESULT CGPSDlg::OnUpdatePsti031(WPARAM wParam, LPARAM lParam)
{
#if(_TAB_LAYOUT_)

	PSTI031_Data* psti031 = (PSTI031_Data*)wParam;
	CString txt;

	txt.Format("%.1f", psti031->baseline);
	m_baselineLength.SetWindowText(txt);
#endif
	return 0;
}

LRESULT CGPSDlg::OnUpdatePsti032(WPARAM wParam, LPARAM lParam)
{	
#if(_TAB_LAYOUT_)
	if(wParam != 0)	
	{
		PSTI032_Data* psti032 = (PSTI032_Data*)wParam;
		CString txt;

		txt.Format("%.3f", psti032->eastProjection);
		m_eastProjection.SetWindowText(txt);
		txt.Format("%.3f", psti032->northProjection);
		m_northProjection.SetWindowText(txt);
		txt.Format("%.3f", psti032->upProjection);
		m_upProjection.SetWindowText(txt);
		txt.Format("%.3f", psti032->baselineLength);
		m_baselineLength.SetWindowText(txt);
		txt.Format("%.2f", psti032->baselineCourse);
		m_baselineCourse.SetWindowText(txt);
	}
	else
	{
		m_bClearPsti032 = FALSE;
		m_eastProjection.SetWindowText("");
		m_northProjection.SetWindowText("");
		m_upProjection.SetWindowText("");
		m_baselineLength.SetWindowText("");
		m_baselineCourse.SetWindowText("");
	}
#endif
	return 0;
}

LRESULT CGPSDlg::OnUpdatePsti033(WPARAM wParam, LPARAM lParam)
{	
#if(_TAB_LAYOUT_)
	if(wParam != 0)	
	{
    DisplayCycleSliped();
	}
	else
	{
		//m_bClearPsti032 = FALSE;
		//m_eastProjection.SetWindowText("");
		//m_northProjection.SetWindowText("");
		//m_upProjection.SetWindowText("");
		//m_baselineLength.SetWindowText("");
		//m_baselineCourse.SetWindowText("");
	}
#endif
	return 0;
}

void CGPSDlg::ShowFormatError(U08* cmd, U08* ack)
{
	CString txt;
	//if(cmd[4]>0x6A && cmd[5]==0x06)
  if(cmd[4] >= 0x60)
	{
		int cmdLen = ConvertLeonU16(ack + 7);
		txt.Format("Format Error! Viewer/FW Length: %d/%d", ConvertLeonU16(cmd + 2), cmdLen);
		add_msgtolist(txt);	
	}
  else
	{
		int cmdLen = ConvertLeonU16(ack + 6);
		txt.Format("Format Error! Viewer/FW Length: %d/%d", ConvertLeonU16(cmd + 2), cmdLen);
		add_msgtolist(txt);	
	}
}

void CGPSDlg::OnStnClickedInformationB()
{
	if(m_InfoTabStat != BasicInfo)
	{
		m_InfoTabStat = BasicInfo;
		SwitchInfoTab();
	}
}

void CGPSDlg::OnStnClickedRtkInfoB()
{
	if(m_InfoTabStat != RtkInfo)
	{
		m_InfoTabStat = RtkInfo;
		SwitchInfoTab();
	}
}

void CGPSDlg::SwitchInfoTab()
{
	switch(m_InfoTabStat)
	{
	case BasicInfo:
		GetDlgItem(IDC_INFORMATION_T)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_INFORMATION_B)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RTK_INFO_T)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RTK_INFO_B)->ShowWindow(SW_SHOW);
		//
		GetDlgItem(IDC_TTFF_T)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_TTFF)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_DATE_T)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_DATE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_TIME_T)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_TIME)->ShowWindow(SW_SHOW);
		//
		GetDlgItem(IDC_BOOT_STATUS_T)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BOOT_STATUS)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_DIRECTION_T)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_DIRECTION)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SW_VER_T)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SW_VER)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SPEED_T)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SPEED)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SW_REV_T)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_SW_REV)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_HDOP_T)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_HDOP)->ShowWindow(SW_SHOW);
#if(_MORE_INFO_)
		GetDlgItem(IDC_RTK_AGE_T)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_RTK_AGE)->ShowWindow(SW_SHOW);
#endif
#if(_TAB_LAYOUT_)
		GetDlgItem(IDC_RTK_AGE_T)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RTK_AGE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RTK_RATIO_T)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RTK_RATIO)->ShowWindow(SW_HIDE);
		//GetDlgItem(IDC_DATE2)->ShowWindow(SW_HIDE);
		//GetDlgItem(IDC_DATE2_T)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CYCLE_SLIP_T)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_CYCLE_SLIP)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_TIME2_T)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_TIME2)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EAST_PROJECTION_T)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_EAST_PROJECTION)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BASELINE_LENGTH_T)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BASELINE_LENGTH)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_NORTH_PROJECTION_T)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_NORTH_PROJECTION)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BASELINE_COURSE_T)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BASELINE_COURSE)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_UP_PROJECTION_T)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_UP_PROJECTION)->ShowWindow(SW_HIDE);
#endif
		break;
	case RtkInfo:
		GetDlgItem(IDC_INFORMATION_T)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_INFORMATION_B)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_RTK_INFO_T)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_RTK_INFO_B)->ShowWindow(SW_HIDE);
		//
		GetDlgItem(IDC_TTFF_T)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_TTFF)->ShowWindow(SW_HIDE);
		//GetDlgItem(IDC_LONGITUDE_T)->ShowWindow(SW_SHOW);
		//GetDlgItem(IDC_LONGITUDE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_DATE_T)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_DATE)->ShowWindow(SW_HIDE);
		//GetDlgItem(IDC_LATITUDE_T)->ShowWindow(SW_SHOW);
		//GetDlgItem(IDC_LATITUDE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_TIME_T)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_TIME)->ShowWindow(SW_HIDE);
		//GetDlgItem(IDC_ALTITUDE_T)->ShowWindow(SW_SHOW);
		//GetDlgItem(IDC_ALTITUDE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BOOT_STATUS_T)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_BOOT_STATUS)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_DIRECTION_T)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_DIRECTION)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SW_VER_T)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SW_VER)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SPEED_T)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SPEED)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SW_REV_T)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_SW_REV)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_HDOP_T)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_HDOP)->ShowWindow(SW_HIDE);
		GetDlgItem(IDC_RTK_AGE_T)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_RTK_AGE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_RTK_RATIO_T)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_RTK_RATIO)->ShowWindow(SW_SHOW);

		//GetDlgItem(IDC_DATE2_T)->ShowWindow(SW_SHOW);
		//GetDlgItem(IDC_DATE2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CYCLE_SLIP_T)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_CYCLE_SLIP)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_TIME2_T)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_TIME2)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EAST_PROJECTION_T)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_EAST_PROJECTION)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BASELINE_LENGTH_T)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BASELINE_LENGTH)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_NORTH_PROJECTION_T)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_NORTH_PROJECTION)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BASELINE_COURSE_T)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_BASELINE_COURSE)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_UP_PROJECTION_T)->ShowWindow(SW_SHOW);
		GetDlgItem(IDC_UP_PROJECTION)->ShowWindow(SW_SHOW);
		break;
	}
}

void CGPSDlg::OnBnClickedCoorSwitch()
{
	switch(m_coorFormat)
	{
	case DegreeMinuteSecond:
		m_coorFormat = DegreeMinute;
		break;
	case DegreeMinute:
		m_coorFormat = Degree;
		break;
	case Degree:
		m_coorFormat = DegreeMinuteSecond;
		break;
	}
	m_copyLatLon = TRUE;
	ShowLongitudeLatitude();
}

void CGPSDlg::OnBnClickedAltitudeSwitch()
{
	switch(m_altFormat)
	{
	case Altitude:
		m_altFormat = EllipsoidHeight;
    GetDlgItem(IDC_ALTITUDE_T)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_ALTITUDE2_T)->ShowWindow(SW_SHOW);
		break;
	case EllipsoidHeight:
		m_altFormat = Altitude;
    GetDlgItem(IDC_ALTITUDE_T)->ShowWindow(SW_SHOW);
    GetDlgItem(IDC_ALTITUDE2_T)->ShowWindow(SW_HIDE);
		break;
	}
	//m_copyLatLon = TRUE;
	ShowAltitude();
}

void CGPSDlg::OnMouseMove(UINT nFlags, CPoint point)
{
  m_mouseNoMoving = FALSE;
  m_mouseMouingTick = GetTickCount();
  CDialog::OnMouseMove(nFlags, point);
}

BOOL CGPSDlg::NeedUpdate()
{
  return TRUE;
  DWORD t = ::GetTickCount();
  if(m_mouseNoMoving == FALSE &&
    t - m_mouseMouingTick > 1000 * 1000)
  {
    m_mouseNoMoving = TRUE;
  }

  return !m_mouseNoMoving;
}

#if defined(XN120_TESTER)
LRESULT CGPSDlg::OnTestXn112Start(WPARAM wParam, LPARAM lParam)
{
  if(wParam == 1)
  {
    m_nXn120TestSatus = 2;
	  SetTimer(XN116_TESTER_TIMER, 1000, NULL);
    return 0;
  }

  CString strMsg;
  strMsg.Format("%s\r\n\r\nCold start failure!!!\r\nTEST FAILED!!!", m_strXn120Version);
  KillTimer(XN116_TESTER_TIMER);
  m_nXn120TestSatus = 0;
  MessageBox(strMsg, APP_CAPTION);
	return 0;
}

void CGPSDlg::Xn116TesterEvent()
{
  BOOL pass = FALSE;
  for(int i = 0; i < MAX_SATELLITE; ++i)
  {
    if(sate_gp[i].SNR >= 36 && sate_gp[i].SNR <= 60)
    {
      pass = TRUE;
      break;
    }
  }
#if defined(XN120_TESTER_BEIDOU)
  if(pass)
  {
    pass = FALSE;
    for(int i = 0; i < MAX_SATELLITE; ++i)
    {
      if(sate_bd[i].SNR >= 36 && sate_bd[i].SNR <= 60)
      {
        pass = TRUE;
        break;
      }
    }
  }
#endif

  ++m_nXn120TestSatus;
  if(pass || m_nXn120TestSatus >= 12)
  {
    CString strMsg;
    strMsg.Format("%s\r\n\r\nTest duration: %d\r\n%s", m_strXn120Version, m_nXn120TestSatus - 2, (pass) ? "PASS" : "TEST FAILED!!!");
    KillTimer(XN116_TESTER_TIMER);
    m_nXn120TestSatus = 0;
    MessageBox(strMsg, APP_CAPTION);
  }
}
#endif