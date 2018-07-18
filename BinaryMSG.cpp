#include "stdafx.h"
#include "GPS.h"
#include "BinaryMSG.h"
#include "GPSDlg.h"

struct CHANNEL_DATA
{
	U08 prn;
	U08 cn0;
	D64 pseudo_range;
	D64 carrier_cycle;
	F32 doppler_freq;
	U08 indicator;
};

struct ExtMeasChannelData
{
	U08 typeNsingel;
	U08 svid;
	U08 freqIdNlockTimeInd;
	U08 cn0;
  D64 pseduRange;
  D64 accCarrierCycle;
	F32 dopplerFreq;
	U08 prStdDeviation;
	U08 accStdDeviation;
	U08 dfDeviation;
	U16 chInd;
};

struct SV_CH_DATA
{
	U08 channel_id;
	U08 prn;
	U08 SV_status;
	U08 URA;
	S08 cn0;
	S16 elevation;
	S16 azimuth;
	U08 channel_status;
};

struct RECEIVER_NAV_DATA
{
	U08 nav_status;
	U16 wn;
	D64 tow;
	D64 ecef_x;
	D64 ecef_y;
	D64 ecef_z;
	F32 ecef_vx;
	F32 ecef_vy;
	F32 ecef_vz;
	D64 clock_bias;
	F32 clock_drift;
	F32 gdop;
	F32 pdop;
	F32 hdop;
	F32 vdop;
	F32 tdop;
};

extern void add2message(const char* buffer, int offset);
extern void SetListCtrlRedraw(BOOL b);

static CString strBuffer("", 512);

static void DisplayAndSave(bool convertOnly, CString* pStr, const char* buf, int len)
{
  const char eol[] = "\r\n";
	if(convertOnly)
	{
    if(pStr)
    {
		  *pStr += buf;
		  *pStr += eol;
    }
	}
  else
  {
    add2message(buf, len);
    CSaveNmea::SaveText(buf, len);
    CSaveNmea::SaveText(eol, 2);
  }
}

static U08* decode_4bytes(U08* src, U32* dst)
{
	*dst = src[0]<<24 | src[1]<<16 | src[2]<<8 | src[3];
	src += 4;
	return src;
}

static U08* decode_2bytes(U08* src, U16* dst)
{
	*dst = src[0]<<8 | src[1];
	src += 2;
	return src;
}

static U08* decode_1bytes(U08* src, U08* dst)
{
	*dst = src[0];
	src += 1;
	return src;
}

void ShowMeasurementChannel(U08* src, bool convertOnly, CString* pStr)
{
	U08* ptr = &src[5];
  U08 iod = 0;
  U08 nmeas = 0;

	ptr = decode_1bytes(ptr, &iod);
	ptr = decode_1bytes(ptr, &nmeas);

  //SetListCtrlRedraw(FALSE);
  strBuffer.Format("$RAW_MEAS(0xDD),IOD=%d,NMEAS=%d", iod, nmeas);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

  if(nmeas == 0)
  {
    //SetListCtrlRedraw(TRUE);
    return;
  }

  strBuffer.Format(" SVID|CN0|   PseudoRange|    CarrierCycle|DopFreq|Indicator|");
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

  strBuffer.Format(" ----+---+--------------+----------------+-------+---------+");
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	CHANNEL_DATA channel = { 0 };
	U32 tmp[2];
	for (int i=0; i<nmeas; ++i)
	{
		ptr = decode_1bytes(ptr, &channel.prn);
		ptr = decode_1bytes(ptr, &channel.cn0);
		ptr = decode_4bytes(ptr, &tmp[1]);
		ptr = decode_4bytes(ptr, &tmp[0]);
		memcpy(&channel.pseudo_range, tmp, sizeof(D64));
		ptr = decode_4bytes(ptr, &tmp[1]);
		ptr = decode_4bytes(ptr, &tmp[0]);
		memcpy(&channel.carrier_cycle,tmp, sizeof(D64));
		ptr = decode_4bytes(ptr, &tmp[0]);
		memcpy(&channel.doppler_freq, tmp, sizeof(F32));
		ptr = decode_1bytes(ptr, &channel.indicator);

    strBuffer.Format("  %3d|%3d| %13.3lf| % 15.3lf| %6.0f|     0x%02X|",
      channel.prn, channel.cn0, channel.pseudo_range, channel.carrier_cycle, 
			channel.doppler_freq, channel.indicator);
    DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());
	}
  //SetListCtrlRedraw(TRUE);
	return;
}

void ExtRawMeas(U08* src, bool convertOnly, CString* pStr)
{
	U08* ptr = &src[5];
  U08 ver = 0;
  U08 iod = 0;
  U16 weeks = 0;
  U32 tow = 0;
  U16 measPeriod = 0;
  U08 measIndFlag = 0;
  U08 reserve = 0;
  U08 nmeas = 0;

	ptr = decode_1bytes(ptr, &ver);
	ptr = decode_1bytes(ptr, &iod);
	ptr = decode_2bytes(ptr, &weeks);
	ptr = decode_4bytes(ptr, &tow);
	ptr = decode_2bytes(ptr, &measPeriod);
	ptr = decode_1bytes(ptr, &measIndFlag);
	ptr = decode_1bytes(ptr, &reserve);
	ptr = decode_1bytes(ptr, &nmeas);

  if(tow == 366333100)
  {
    int a = 0;
  }
  //SetListCtrlRedraw(FALSE);
  strBuffer.Format("$EXT_RAW_MEAS(0xE5),V%d,IOD=%d,WN=%d,TOW=%d,MeasPeriod=%d,MeasIndicator=0x%02X,NMEAS=%d",
    ver, iod, weeks, tow, measPeriod, measIndFlag, nmeas);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

  if(nmeas != 0)
  {
    strBuffer.Format(" GnTp|SgTp|SVID|FrqID|LTInd|CN0|   PseudoRange|       AccCrrCyc|DopFreq|PRSdDv|ACCSdDv|DFSdDv| ChInd|");
    DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

    strBuffer.Format(" ----+----+----+-----+-----+---+--------------+----------------+-------+------+-------+------+------+");
    DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	  ExtMeasChannelData channel = { 0 };
    U32 tmp[2] = { 0 };
	  for (int i=0; i<nmeas; ++i)
	  {
		  ptr = decode_1bytes(ptr, &channel.typeNsingel);
		  ptr = decode_1bytes(ptr, &channel.svid);
		  ptr = decode_1bytes(ptr, &channel.freqIdNlockTimeInd);
		  ptr = decode_1bytes(ptr, &channel.cn0);
		  ptr = decode_4bytes(ptr, &tmp[1]);
		  ptr = decode_4bytes(ptr, &tmp[0]);
		  memcpy(&channel.pseduRange, tmp, sizeof(D64));
		  ptr = decode_4bytes(ptr, &tmp[1]);
		  ptr = decode_4bytes(ptr, &tmp[0]);
		  memcpy(&channel.accCarrierCycle, tmp, sizeof(D64));
		  ptr = decode_4bytes(ptr, &tmp[0]);
		  memcpy(&channel.dopplerFreq, tmp, sizeof(F32));
		  ptr = decode_1bytes(ptr, &channel.prStdDeviation);
		  ptr = decode_1bytes(ptr, &channel.accStdDeviation);
		  ptr = decode_1bytes(ptr, &channel.dfDeviation);
	    ptr = decode_2bytes(ptr, &channel.chInd);
	    ptr = decode_1bytes(ptr, &reserve);
	    ptr = decode_1bytes(ptr, &reserve);

      //"$GnTp|SgTp|SVID|
      // FrqID|LTInd|CN0|  
      // PseudoRange|      AccCrrCyc|DopplerFreq|
      // PRSdDv|ACCSdDv|DFSdDv| ChInd|");
      strBuffer.Format("   %2d|  %2d| %3d|%5d|%5d|%3d| %13.3lf| % 15.3lf| % 6.0f|%6d|%7d|%6d|0x%04X|",
          channel.typeNsingel & 0xF, channel.typeNsingel >> 4, channel.svid, 
          channel.freqIdNlockTimeInd & 0x0f, channel.freqIdNlockTimeInd >> 4, channel.cn0,
          channel.pseduRange, channel.accCarrierCycle, channel.dopplerFreq, 
          channel.prStdDeviation, channel.accStdDeviation, channel.dfDeviation, channel.chInd);
      DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());
	  }
  }

	if(convertOnly)
	{
		return;
	}

	UtcTime utc;
	UtcConvertGpsToUtcTime(weeks, (tow / 1000.0), &utc);

	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Hour = utc.hour;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Min = utc.minute;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Sec = utc.sec;

	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Year = utc.year;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Month = utc.month;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Day = utc.day;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Hour = utc.hour;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Min = utc.minute;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Sec = utc.sec;
	return;
}

void ShowSubframe(U08 *src, bool convertOnly, CString* pStr)
{
	U08 msgType = src[4];
	CString subFrameType;
	if(msgType==0xE0)
	{
		subFrameType = "GPS_SUBFRAME(0xE0)";
	}
	else if(msgType==0xE1)
	{
		subFrameType = "GLONASS_STRING(0xE1)";
	}	
	else if(msgType==0xE2)
	{
		subFrameType = "BEIDOU2_D1_SUBFRAME(0xE2)";
	}	
	else if(msgType==0xE3)
	{
		subFrameType = "BEIDOU2_D2_SUBFRAME(0xE3)";
	}	
#if(IS_DEBUG)
	else if(msgType==0xE4)
	{
		subFrameType = "SBAS_SUBFRAME(0xE3)";
	}
#endif
	U08 prn = src[5];
	U08 subFrmId = src[6];
  strBuffer.Format("$%s,SVID=%d,SFID=%d,data=", subFrameType, prn, subFrmId);

	WORD packetLen = MAKEWORD(src[3], src[2]);
	for(int i = 3; i < packetLen; ++i)
	{
		CString tmpBuff;
    tmpBuff.Format("%02X ", src[4+i]);
		strBuffer += tmpBuff;
	}
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());
}

void ShowMeasurementTime(U08 *src, bool convertOnly, CString* pStr)
{
  static CString strBuffer("", 512);
	U08 *ptr = &src[6];

	U16 wn = 0;
	U32 tow = 0;
	U16 period = 0;

	ptr = decode_2bytes(ptr, &wn);
	ptr = decode_4bytes(ptr, &tow);
	ptr = decode_2bytes(ptr, &period);

  strBuffer.Format("$MEAS_TIME(0xDC),WN=%d,TOW=%d,Period=%d",
    wn, tow, period);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

  if(convertOnly)
	{
		return;
	}

	UtcTime utc;
	UtcConvertGpsToUtcTime(wn, (tow / 1000.0), &utc);

	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Hour = utc.hour;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Min = utc.minute;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Sec = utc.sec;

	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Year = utc.year;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Month = utc.month;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Day = utc.day;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Hour = utc.hour;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Min = utc.minute;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Sec = utc.sec;
	return;
}

void ShowMeasurementSv(U08 *src, bool convertOnly, CString* pStr)
{
  static CString strBuffer("", 512);
	U08* ptr = &src[5];

  U08 iod = 0;
  U08 nsvs = 0;

	ptr = decode_1bytes(ptr, &iod);
	ptr = decode_1bytes(ptr, &nsvs);

  //SetListCtrlRedraw(FALSE);
  strBuffer.Format("$SV_CH_STATUS(0xDE),IOD=%d,NSVS=%d", iod, nsvs);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

  if(nsvs == 0)
  {
    //SetListCtrlRedraw(TRUE);
    return;
  }

  strBuffer.Format(" ChId|SVID|SvInd| URA|CN0|ELE| AZI|ChInd|");
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

  strBuffer.Format(" ----+----+-----+----+---+---+----+-----+");
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	int fixed_gps_c = 0, fixed_glonass_c = 0, fixed_beidou_c = 0, fixed_galileo_c = 0, fixed_navic_c = 0;
	int gps_c = 0, glonass_c = 0, beidou_c = 0, galileo_c = 0, navic_c = 0;
	SV_CH_DATA sv;
	if(!convertOnly)
	{
		memset(CGPSDlg::gpsDlg->m_gagsaMsgCopy.SatelliteID, 0, 
      sizeof(CGPSDlg::gpsDlg->m_gagsaMsgCopy.SatelliteID));
		memset(CGPSDlg::gpsDlg->m_bdgsaMsgCopy.SatelliteID, 0, 
      sizeof(CGPSDlg::gpsDlg->m_bdgsaMsgCopy.SatelliteID));
		memset(CGPSDlg::gpsDlg->m_glgsaMsgCopy.SatelliteID, 0, 
      sizeof(CGPSDlg::gpsDlg->m_glgsaMsgCopy.SatelliteID));
		memset(CGPSDlg::gpsDlg->m_gpgsaMsgCopy.SatelliteID, 0, 
      sizeof(CGPSDlg::gpsDlg->m_gpgsaMsgCopy.SatelliteID));
	}

	for (int i=0; i<nsvs; ++i)
	{
		ptr = decode_1bytes(ptr, &sv.channel_id);
		ptr = decode_1bytes(ptr, &sv.prn);
		ptr = decode_1bytes(ptr, &sv.SV_status);
		ptr = decode_1bytes(ptr, &sv.URA);
		ptr = decode_1bytes(ptr, (U08*)&sv.cn0);
		ptr = decode_2bytes(ptr, (U16*)&sv.elevation);
		ptr = decode_2bytes(ptr, (U16*)&sv.azimuth);
		ptr = decode_1bytes(ptr, &sv.channel_status);

    strBuffer.Format("%5d|%4d| 0x%02X|%4d|%3d|%3d|%4d| 0x%02X|",
			sv.channel_id, sv.prn, sv.SV_status, sv.URA,sv.cn0, 
			sv.elevation, sv.azimuth, sv.channel_status);

    DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());
    //SetListCtrlRedraw(TRUE);

	  if(convertOnly)
	  {
		  continue;
	  }

		if(NMEA::Glonass == NMEA::GetGNSSSystem(sv.prn))
		{
			if(glonass_c==0)
			{
        CGPSDlg::gpsDlg->nmea.satellites_gl.Clear();
			}
			if (sv.channel_status & 0x30)
			{
				CGPSDlg::gpsDlg->m_glgsaMsgCopy.SatelliteID[fixed_glonass_c] = sv.prn;
				++fixed_glonass_c;
			}

      CGPSDlg::gpsDlg->nmea.satellites_gl.SetSate(sv.prn, sv.elevation, sv.azimuth, sv.cn0);
			glonass_c++;
		}
		else if(NMEA::Gps == NMEA::GetGNSSSystem(sv.prn))
		{
			if(gps_c==0)
			{
        CGPSDlg::gpsDlg->nmea.satellites_gp.Clear();
			}
			if (sv.channel_status & 0x30)
			{
				CGPSDlg::gpsDlg->m_gpgsaMsgCopy.SatelliteID[fixed_gps_c] = sv.prn;
				fixed_gps_c++;
			}

      CGPSDlg::gpsDlg->nmea.satellites_gp.SetSate(sv.prn, sv.elevation, sv.azimuth, sv.cn0);
			gps_c++;
		}
		else if(NMEA::Beidou == NMEA::GetGNSSSystem(sv.prn))
		{
			if(beidou_c==0)
			{
        CGPSDlg::gpsDlg->nmea.satellites_bd.Clear();
			}
			if (sv.channel_status & 0x30)
			{
				CGPSDlg::gpsDlg->m_bdgsaMsgCopy.SatelliteID[fixed_beidou_c] = sv.prn;
				fixed_beidou_c++;
			}

      CGPSDlg::gpsDlg->nmea.satellites_bd.SetSate(sv.prn, sv.elevation, sv.azimuth, sv.cn0);
			beidou_c++;
		}
		else if(NMEA::Galileo == NMEA::GetGNSSSystem(sv.prn))
		{
			if(galileo_c==0)
			{
        CGPSDlg::gpsDlg->nmea.satellites_ga.Clear();
			}
			if (sv.channel_status & 0x30)
			{
				CGPSDlg::gpsDlg->m_gagsaMsgCopy.SatelliteID[fixed_galileo_c] = sv.prn;
				fixed_galileo_c++;
			}

      CGPSDlg::gpsDlg->nmea.satellites_ga.SetSate(sv.prn, sv.elevation, sv.azimuth, sv.cn0);
			galileo_c++;
		}
		else if(NMEA::Navic == NMEA::GetGNSSSystem(sv.prn))
		{
			if(navic_c==0)
			{
        CGPSDlg::gpsDlg->nmea.satellites_gi.Clear();
			}
			if (sv.channel_status & 0x30)
			{
				CGPSDlg::gpsDlg->m_gigsaMsgCopy.SatelliteID[fixed_navic_c] = sv.prn - 240;
				fixed_navic_c++;
			}

      CGPSDlg::gpsDlg->nmea.satellites_gi.SetSate(sv.prn - 240, sv.elevation, sv.azimuth, sv.cn0);
			navic_c++;
		}
	}
	
	CGPSDlg::gpsDlg->m_gpgsvMsgCopy.NumOfSate = gps_c;
	CGPSDlg::gpsDlg->m_gpgsvMsgCopy.NumOfMessage = (gps_c + 3) / 4;
	CGPSDlg::gpsDlg->m_gpgsvMsgCopy.SequenceNum = (gps_c + 3) / 4;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.NumsOfSatellites = fixed_gps_c + fixed_glonass_c;

	CGPSDlg::gpsDlg->m_glgsvMsgCopy.NumOfSate = glonass_c;
	CGPSDlg::gpsDlg->m_glgsvMsgCopy.NumOfMessage = (glonass_c + 3) / 4;
	CGPSDlg::gpsDlg->m_glgsvMsgCopy.SequenceNum = (glonass_c + 3) / 4;

	CGPSDlg::gpsDlg->m_bdgsvMsgCopy.NumOfSate = beidou_c;
	CGPSDlg::gpsDlg->m_bdgsvMsgCopy.NumOfMessage = (beidou_c + 3) / 4;
	CGPSDlg::gpsDlg->m_bdgsvMsgCopy.SequenceNum = (beidou_c + 3) / 4;

	CGPSDlg::gpsDlg->m_gigsvMsgCopy.NumOfSate = navic_c;
	CGPSDlg::gpsDlg->m_gigsvMsgCopy.NumOfMessage = (navic_c + 3) / 4;
	CGPSDlg::gpsDlg->m_gigsvMsgCopy.SequenceNum = (navic_c + 3) / 4;
}

void ShowReceiverNav(U08 *src, bool convertOnly, CString* pStr)
{
  U08 *ptr = &src[6];
  U08 iod = src[5];

	U32 tmp[2];
	RECEIVER_NAV_DATA receiver;

	ptr = decode_1bytes(ptr, &receiver.nav_status);
	ptr = decode_2bytes(ptr, &receiver.wn);
	
	ptr = decode_4bytes(ptr, &tmp[1]);
	ptr = decode_4bytes(ptr, &tmp[0]);
	memcpy(&receiver.tow, tmp, sizeof(D64));

	ptr = decode_4bytes(ptr, &tmp[1]);
	ptr = decode_4bytes(ptr, &tmp[0]);
	memcpy(&receiver.ecef_x, tmp, sizeof(D64));

	ptr = decode_4bytes(ptr, &tmp[1]);
	ptr = decode_4bytes(ptr, &tmp[0]);
	memcpy(&receiver.ecef_y, tmp, sizeof(D64));

	ptr = decode_4bytes(ptr, &tmp[1]);
	ptr = decode_4bytes(ptr, &tmp[0]);
	memcpy(&receiver.ecef_z, tmp, sizeof(D64));

	ptr = decode_4bytes(ptr, &tmp[0]);
	memcpy(&receiver.ecef_vx, tmp, sizeof(F32));

	ptr = decode_4bytes(ptr, &tmp[0]);
	memcpy(&receiver.ecef_vy, tmp, sizeof(F32));

	ptr = decode_4bytes(ptr, &tmp[0]);
	memcpy(&receiver.ecef_vz, tmp, sizeof(F32));

	ptr = decode_4bytes(ptr, &tmp[1]);
	ptr = decode_4bytes(ptr, &tmp[0]);
	memcpy(&receiver.clock_bias, tmp, sizeof(D64));

	ptr = decode_4bytes(ptr, &tmp[0]);
	memcpy(&receiver.clock_drift, tmp, sizeof(F32));

	ptr = decode_4bytes(ptr, &tmp[0]);
	memcpy(&receiver.gdop, tmp, sizeof(F32));

	ptr = decode_4bytes(ptr, &tmp[0]);
	memcpy(&receiver.pdop, tmp, sizeof(F32));

	ptr = decode_4bytes(ptr, &tmp[0]);
	memcpy(&receiver.hdop, tmp, sizeof(F32));

	ptr = decode_4bytes(ptr, &tmp[0]);
	memcpy(&receiver.vdop, tmp, sizeof(F32));

	ptr = decode_4bytes(ptr, &tmp[0]);
	memcpy(&receiver.tdop, tmp, sizeof(F32));

  strBuffer.Format("$RCV_STATE(0xDF),IOD=%d,NavState=%d,WN=%d,TOW=%lf,ECEF_x=%lf,ECEF_y=%lf,ECEF_z=%lf," \
    "ECEF_vx=%f,ECEF_vy=%f,ECEF_vz=%f,ClockBias=%lf,ClockDrift=%f,GDOP=%f,PDOP=%f,HDOP=%f,VDOP=%f,TDOP=%f",
		iod, receiver.nav_status, receiver.wn, receiver.tow, 
		receiver.ecef_x, receiver.ecef_y, receiver.ecef_z,
		receiver.ecef_vx, receiver.ecef_vy, receiver.ecef_vz,
		receiver.clock_bias, receiver.clock_drift,
		receiver.gdop, receiver.pdop, receiver.hdop, 
		receiver.vdop, receiver.tdop);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());
  if(convertOnly)
  {
    return;
  }

  switch(receiver.nav_status)
	{
	case 0:
		CGPSDlg::gpsDlg->m_gpggaMsgCopy.GPSQualityIndicator = '0';
		CGPSDlg::gpsDlg->m_gpgsaMsgCopy.Mode = 1;
		CGPSDlg::gpsDlg->m_gprmcMsgCopy.ModeIndicator = 'V';
		break;
	case 1:
		CGPSDlg::gpsDlg->m_gpggaMsgCopy.GPSQualityIndicator = '1';
		CGPSDlg::gpsDlg->m_gpgsaMsgCopy.Mode = 2;
		CGPSDlg::gpsDlg->m_gprmcMsgCopy.ModeIndicator = 'A';
		break;
	case 2:
		CGPSDlg::gpsDlg->m_gpggaMsgCopy.GPSQualityIndicator = '1';
		CGPSDlg::gpsDlg->m_gpgsaMsgCopy.Mode = 2;
		CGPSDlg::gpsDlg->m_gprmcMsgCopy.ModeIndicator = 'A';
		break;
	case 3:
		CGPSDlg::gpsDlg->m_gpggaMsgCopy.GPSQualityIndicator = '1';
		CGPSDlg::gpsDlg->m_gpgsaMsgCopy.Mode = 3;
		CGPSDlg::gpsDlg->m_gprmcMsgCopy.ModeIndicator = 'A';
		break;
	case 4:
		CGPSDlg::gpsDlg->m_gpggaMsgCopy.GPSQualityIndicator = '2';
		CGPSDlg::gpsDlg->m_gpgsaMsgCopy.Mode = 3;
		CGPSDlg::gpsDlg->m_gprmcMsgCopy.ModeIndicator = 'D';
		break;
	}

	POS_T pos;
	pos.px = receiver.ecef_x;
	pos.py = receiver.ecef_y;
	pos.pz = receiver.ecef_z;

	LLA_T lla;
	CooCartesianToGeodetic(&pos, &lla);

	lla.lat *= R2D;
	lla.lon *= R2D;
	double lat_d = (S16)fabs(lla.lat );
	double lon_d = (S16)fabs(lla.lon );
	double lat_m = fmod( fabs(lla.lat), 1.0) * 60.0;
	double lon_m = fmod( fabs(lla.lon), 1.0) * 60.0;

	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Latitude = lat_d * 100.0 + lat_m;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Latitude_N_S = (lla.lat >= 0) ? 'N' : 'S';
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Longitude = lon_d * 100.0 + lon_m;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Longitude_E_W = (lla.lon >= 0) ? 'E' : 'W';
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Altitude = lla.alt - CGPSDlg::gpsDlg->m_gpggaMsgCopy.GeoidalSeparation;

	CGPSDlg::gpsDlg->m_gpgsaMsgCopy.PDOP = receiver.pdop;
	CGPSDlg::gpsDlg->m_gpgsaMsgCopy.HDOP = receiver.hdop;
	CGPSDlg::gpsDlg->m_gpgsaMsgCopy.VDOP = receiver.vdop;
}

void ShowBinaryOutput(U08* src, bool convertOnly, CString* pStr)
{
  U16 size = src[2] << 8 | src[3];
  BOOL v2Format = (size == 0x3B) ? FALSE : TRUE;

  int ix = 5;
  U08 fixMode = src[ix];
  ++ix;

  U08 nos = src[ix];
  ++ix;


  U16 wn = src[ix] << 8 | src[ix + 1];
  ix += 2;

  U32 tow = 0;
  D64 ftow = 0;

  if(v2Format)
  {
    ftow = ConvertLeonDouble(&src[ix]);
    ix += 8;
  }
  else
  {
	  tow = src[ix]<<24 | src[ix + 1]<<16 | src[ix + 2]<<8 | src[ix + 3];
    ix += 4;
    ftow = (double)tow / 100.0;
  }

	UtcTime utc;
	UtcConvertGpsToUtcTime(wn, ftow, &utc);

	double lat = src[ix]<<24 | src[ix + 1]<<16 | src[ix + 2]<<8 | src[ix + 3];
  ix += 4;

	double lon = src[ix]<<24 | src[ix + 1]<<16 | src[ix + 2]<<8 | src[ix + 3];
  ix += 4;

	D64 flat = lat;
	flat /= 10000000;
	flat =((int)flat)*100 +(double)(flat - (int)flat)*60;

	D64 flon = lon;
	flon /= 10000000;
	flon = ((int)flon)*100 +(double)(flon - (int)flon)*60;

  U32 d = src[ix]<<24 | src[ix + 1]<<16 | src[ix + 2]<<8 | src[ix + 3];
  ix += 4;
	S32 alt_t = *((S32*)(&d));

  d = src[ix]<<24 | src[ix + 1]<<16 | src[ix + 2]<<8 | src[ix + 3];
  ix += 4;
	S32 alt = *((S32*)(&d));

	U32 gdop = src[ix]<<8 | src[ix + 1];
  ix += 2;

	U32 pdop = src[ix]<<8 | src[ix + 1];
  ix += 2;

	U32 hdop = src[ix]<<8 | src[ix + 1];
  ix += 2;

	U32 vdop = src[ix]<<8 | src[ix + 1];
  ix += 2;

	U32 tdop = src[ix]<<8 | src[ix + 1];
  ix += 2;

	S32 ecefx = src[ix]<<24 | src[ix + 1]<<16 | src[ix + 2]<<8 | src[ix + 3];
  ix += 4;

	S32 ecefy = src[ix]<<24 | src[ix + 1]<<16 | src[ix + 2]<<8 | src[ix + 3];
  ix += 4;

	S32 ecefz = src[ix]<<24 | src[ix + 1]<<16 | src[ix + 2]<<8 | src[ix + 3];
  ix += 4;

	S32 vx = src[ix]<<24 | src[ix + 1]<<16 | src[ix + 2]<<8 | src[ix + 3];
  ix += 4;

	S32 vy = src[ix]<<24 | src[ix + 1]<<16 | src[ix + 2]<<8 | src[ix + 3];
  ix += 4;

	S32 vz = src[ix]<<24 | src[ix + 1]<<16 | src[ix + 2]<<8 | src[ix + 3];
  ix += 4;

	D64 fx = (D64)vx / 100;
	D64 fy = (D64)vy / 100;
	D64 fz = (D64)vz / 100;
	D64 fv = sqrt(fx * fx + fy * fy + fz * fz);

  //SetListCtrlRedraw(FALSE);
	strBuffer.Format("$fix mode=%d", fixMode);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$number if sv in fix=%d", nos);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$GPS Week=%d", wn);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$GPS TOW=%.2f",ftow);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$latitude=%.6f",flat);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$longitude=%.6f",flon);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$ellipsoid altitude=%.2f", (F32)alt_t / 100);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$sea level altitude=%.2f", (F32)alt / 100);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$gdop=%.2f",(F32)gdop/100);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$pdop=%.2f",(F32)pdop/100);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$hdop=%.2f",(F32)hdop/100);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$tdop=%.2f",(F32)tdop/100);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$ecef-x=%.2f",(double)ecefx/100);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$ecef-y=%.2f",(double)ecefy/100);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$ecef-z=%.2f",(double)ecefz/100);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$ecef-vx=%.2f",fx);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$ecef-vy=%.2f",fy);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$ecef-vz=%.2f",fz);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

  //SetListCtrlRedraw(TRUE);
	if(convertOnly)
	{
		return;
	}

	if(src[5]==0)
	{
		CGPSDlg::gpsDlg->m_gpggaMsgCopy.GPSQualityIndicator = '0';
		CGPSDlg::gpsDlg->m_gpgsaMsgCopy.Mode = 1;
		CGPSDlg::gpsDlg->m_gprmcMsgCopy.ModeIndicator = 'V';
	}
	else if(src[5] ==1)
	{
		CGPSDlg::gpsDlg->m_gpggaMsgCopy.GPSQualityIndicator = '1';
		CGPSDlg::gpsDlg->m_gpgsaMsgCopy.Mode = 2;
		CGPSDlg::gpsDlg->m_gprmcMsgCopy.ModeIndicator = 'A';
	}
	else if(src[5] == 2)
	{
		CGPSDlg::gpsDlg->m_gpggaMsgCopy.GPSQualityIndicator = '1';
		CGPSDlg::gpsDlg->m_gpgsaMsgCopy.Mode = 3;
		CGPSDlg::gpsDlg->m_gprmcMsgCopy.ModeIndicator = 'A';
	}
	else if(src[5] == 3)
	{
		CGPSDlg::gpsDlg->m_gpggaMsgCopy.GPSQualityIndicator = '2';
		CGPSDlg::gpsDlg->m_gpgsaMsgCopy.Mode = 3;
		CGPSDlg::gpsDlg->m_gprmcMsgCopy.ModeIndicator = 'A';
	}
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.NumsOfSatellites = src[6];

	//volatile U16 i_sec;
	//i_sec = (U16)(utc.sec * 1000);

	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Hour = utc.hour;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Min = utc.minute;
	//CGPSDlg::gpsDlg->m_gpggaMsgCopy.Sec = i_sec*0.001f;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Sec = utc.sec;

	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Year = utc.year;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Month = utc.month;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Day = utc.day;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Hour = utc.hour;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Min = utc.minute;
	//CGPSDlg::gpsDlg->m_gprmcMsgCopy.Sec = i_sec * 0.001f;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Sec = utc.sec;

	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Latitude = flat;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Latitude_N_S = flat >= 0 ? 'N' : 'S';
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Longitude = flon;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Longitude_E_W = flon >= 0 ? 'E' : 'W';

	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Latitude = flat;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Latitude_N_S = flat >= 0 ? 'N' : 'S';
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Longitude = flon;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Longitude_E_W = flon >= 0 ? 'E' : 'W';

	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Altitude = (F32)alt;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Altitude /= 100.0F;

	CGPSDlg::gpsDlg->m_gpgsaMsgCopy.PDOP = (F32)pdop/100;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.HDOP = (F32)hdop/100;
	CGPSDlg::gpsDlg->m_gpgsaMsgCopy.VDOP = (F32)vdop/100;

	CGPSDlg::gpsDlg->m_gprmcMsgCopy.SpeedKnots = (F32)(fv * MS2KMHR / KNOTS2KMHR);
}

void ShowDjiBinaryOutput(U08* src, bool convertOnly, CString* pStr)
{
	src += 3;	//pass id and sid
	U16 wn = ConvertLeonU16(src + 6);
	D64 tow = ConvertLeonDouble(src + 8);

	UtcTime utc;
	UtcConvertGpsToUtcTime(wn, tow, &utc);

	D64 lat = ConvertLeonDouble(src + 16);
	D64 lon = ConvertLeonDouble(src + 24);
	lat =((int)lat)*100 +(double)(lat - (int)lat)*60;
	lon = ((int)lon)*100 +(double)(lon - (int)lon)*60;

	F32 alt_t = ConvertLeonFloat(src + 32);
	F32 alt = ConvertLeonFloat(src + 36);

	U16 gdop = ConvertLeonU16(src + 40);
	U16 pdop = ConvertLeonU16(src + 42);
	U16 hdop = ConvertLeonU16(src + 44);
	U16 vdop = ConvertLeonU16(src + 46);
	U16 tdop = ConvertLeonU16(src + 48);

	D64 ecefx = ConvertLeonDouble(src + 50);
	D64 ecefy = ConvertLeonDouble(src + 58);
	D64 ecefz = ConvertLeonDouble(src + 66);

	D64 vx = ConvertLeonDouble(src + 74);
	D64 vy = ConvertLeonDouble(src + 78);
	D64 vz = ConvertLeonDouble(src + 82);

	F32 rtkAge = (F32)ConvertLeonU16(src + 86) / 10;
	F32 rtkFix = (F32)ConvertLeonU16(src + 88) / 10;
	
	CGPSDlg::gpsDlg->PostMessage(UWM_UPDATE_RTK_INFO, *(WPARAM*)&rtkAge, *(LPARAM*)&rtkFix);
	D64 fv = sqrt(vx * vx + vy * vy + vz * vz);

	strBuffer.Format("$fix mode=%d", src[4]);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$number if sv in fix=%d", src[5]);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$GPS Week=%d", wn);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$GPS TOW=%.2f", tow);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$latitude=%.7f", lat);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$longutide=%.7f",lon);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$ellipsoid altitude=%.2f", alt_t);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$sea level altitude=%.2f", alt);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$gdop=%.2f",(F32)gdop/100);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$pdop=%.2f",(F32)pdop/100);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$hdop=%.2f",(F32)hdop/100);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$tdop=%.2f",(F32)tdop/100);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$ecef-x=%.2f",(double)ecefx);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$ecef-y=%.2f",(double)ecefy);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$ecef-z=%.2f",(double)ecefz);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$ecef-vx=%.2f",vx);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$ecef-vy=%.2f",vy);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$ecef-vz=%.2f",vz);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$RTK age=%.1f", rtkAge);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$RTK ratio=%.1f", rtkFix);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	if(convertOnly)
	{
		return;
	}

	if(src[4]==0)
	{
		CGPSDlg::gpsDlg->m_gpggaMsgCopy.GPSQualityIndicator = '0';
		CGPSDlg::gpsDlg->m_gpgsaMsgCopy.Mode = 1;
		CGPSDlg::gpsDlg->m_gprmcMsgCopy.ModeIndicator = 'V';
	}
	else if(src[4] ==1)
	{
		CGPSDlg::gpsDlg->m_gpggaMsgCopy.GPSQualityIndicator = '1';
		CGPSDlg::gpsDlg->m_gpgsaMsgCopy.Mode = 2;
		CGPSDlg::gpsDlg->m_gprmcMsgCopy.ModeIndicator = 'A';
	}
	else if(src[4] == 2)
	{
		CGPSDlg::gpsDlg->m_gpggaMsgCopy.GPSQualityIndicator = '1';
		CGPSDlg::gpsDlg->m_gpgsaMsgCopy.Mode = 3;
		CGPSDlg::gpsDlg->m_gprmcMsgCopy.ModeIndicator = 'A';
	}
	else if(src[4] == 3)
	{
		CGPSDlg::gpsDlg->m_gpggaMsgCopy.GPSQualityIndicator = '2';
		CGPSDlg::gpsDlg->m_gpgsaMsgCopy.Mode = 3;
		CGPSDlg::gpsDlg->m_gprmcMsgCopy.ModeIndicator = 'A';
	}
	else if(src[4] == 4)
	{
		CGPSDlg::gpsDlg->m_gpggaMsgCopy.GPSQualityIndicator = '5';
		CGPSDlg::gpsDlg->m_gpgsaMsgCopy.Mode = 3;
		CGPSDlg::gpsDlg->m_gprmcMsgCopy.ModeIndicator = 'F';
	}
	else if(src[4] == 5)
	{
		CGPSDlg::gpsDlg->m_gpggaMsgCopy.GPSQualityIndicator = '4';
		CGPSDlg::gpsDlg->m_gpgsaMsgCopy.Mode = 3;
		CGPSDlg::gpsDlg->m_gprmcMsgCopy.ModeIndicator = 'R';
	}	CGPSDlg::gpsDlg->m_gpggaMsgCopy.NumsOfSatellites = src[5];

	volatile U16 i_sec;
	i_sec = (U16)(utc.sec * 1000);

	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Hour = utc.hour;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Min = utc.minute;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Sec = i_sec*0.001f;

	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Year = utc.year;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Month = utc.month;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Day = utc.day;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Hour = utc.hour;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Min = utc.minute;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Sec = i_sec * 0.001f;

	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Latitude = lat;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Latitude_N_S = lat >= 0 ? 'N' : 'S';
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Longitude = lon;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Longitude_E_W = lon >= 0 ? 'E' : 'W';

	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Latitude = lat;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Latitude_N_S = lat >= 0 ? 'N' : 'S';
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Longitude = lon;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Longitude_E_W = lon >= 0 ? 'E' : 'W';

	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Altitude = alt;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Altitude /= 100.0F;

	CGPSDlg::gpsDlg->m_gpgsaMsgCopy.PDOP = (F32)pdop/100;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.HDOP = (F32)hdop/100;
	CGPSDlg::gpsDlg->m_gpgsaMsgCopy.VDOP = (F32)vdop/100;

	CGPSDlg::gpsDlg->m_gprmcMsgCopy.SpeedKnots = (F32)(fv * MS2KMHR/ KNOTS2KMHR);
}

void ShowRtcm1005(U08* src, bool convertOnly, CString* pStr)
{
  U16 satId = 0;
  GetBitData(&src[3], 12, 12, (U08*)(&satId), sizeof(satId));
  U08 gpQtyInd = 0;
  GetBitData(&src[3], 30, 1, (U08*)(&gpQtyInd), sizeof(gpQtyInd));
  U08 glQtyInd = 0;
  GetBitData(&src[3], 31, 1, (U08*)(&glQtyInd), sizeof(glQtyInd));
  U08 gaQtyInd = 0;
  GetBitData(&src[3], 32, 1, (U08*)(&gaQtyInd), sizeof(gaQtyInd));

  S64 ecefx = 0;
  GetBitData(&src[3], 34, 38, (U08*)(&ecefx), sizeof(ecefx));
  ConvertInt38Sign(&ecefx);

  S64 ecefy = 0;
  GetBitData(&src[3], 74, 38, (U08*)(&ecefy), sizeof(ecefy));
  ConvertInt38Sign(&ecefy);
  S64 ecefz = 0;
  GetBitData(&src[3], 114, 38, (U08*)(&ecefz), sizeof(ecefz));
  ConvertInt38Sign(&ecefz);

  strBuffer.Format("$Rtcm1005,sta-Id=%d,gps-Ind=%d,glo-Ind=%d,gal-Ind=%d,ecef-x=%lld,ecef-y=%lld,ecef-z=%lld",
		satId, gpQtyInd, glQtyInd, gaQtyInd, ecefx, ecefy, ecefz);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	if(convertOnly)
	{
		return;
	}

	if(gpQtyInd || glQtyInd)
	{
		CGPSDlg::gpsDlg->m_gpggaMsgCopy.GPSQualityIndicator = '1';
		CGPSDlg::gpsDlg->m_gpgsaMsgCopy.Mode = 3;
		CGPSDlg::gpsDlg->m_gprmcMsgCopy.ModeIndicator = 'A';
	}

	POS_T pos;
	pos.px = (D64)ecefx / 10000;
	pos.py = (D64)ecefy / 10000;
	pos.pz = (D64)ecefz / 10000;

	LLA_T lla;
	CooCartesianToGeodetic(&pos, &lla);

	lla.lat *= R2D;
	lla.lon *= R2D;
	double lat_d = (S16)fabs(lla.lat );
	double lon_d = (S16)fabs(lla.lon );
	double lat_m = fmod( fabs(lla.lat), 1.0) * 60.0;
	double lon_m = fmod( fabs(lla.lon), 1.0) * 60.0;

	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Latitude = lat_d * 100.0 + lat_m;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Latitude_N_S = (lla.lat >= 0) ? 'N' : 'S';
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Longitude = lon_d * 100.0 + lon_m;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Longitude_E_W = (lla.lon >= 0) ? 'E' : 'W';
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Altitude = lla.alt - CGPSDlg::gpsDlg->m_gpggaMsgCopy.GeoidalSeparation;
}
typedef struct{
  U08 valid;
  U16 sta_id;
  U32 gnss_epoch_time;

  U08 multi_msg_bit;
  U08 iods;
  U08 div_free_smoothing_ind;
  U08 smoothing_interval;
  U32 sig_mask;

  U64 sat_mask;
  U64 cell_mask;
} RTCM_MSM_HEADER_T;

typedef struct  {
  U08 rough_range_in_ms;
  U08 ext_sat_info;   //knum + 7 in glonass, 0 in gps
  U16 rough_range_mudulo_1_ms;
  S16 rough_phase_range_rate;
} RTCM_MSM_5_7_SAT_DATA_T;

typedef struct  {
  S32 fine_pseudorange_ext_resol;
  S32 fine_phase_range_ext_resol;

  U16 lock_time_ind_ext_resol;
  U08 half_cycle_ambiguity_ind;

  U16 snr_ext_resol;  //cn0 * 16
  S16 fine_phase_range_rate;
} RTCM_MSM_7_SIG_DATA_T;
RTCM_MSM_HEADER_T msm_header = {0};
RTCM_MSM_5_7_SAT_DATA_T satData[64] = {0};
RTCM_MSM_7_SIG_DATA_T sigData[64] = {0};

int ParsingMSM7(U08* src)
{
  int bitIndex = 0;
  bitIndex += 12;  //pass Message number

  //U16 satId = 0;
  GetBitData(&src[3], bitIndex, 12, (U08*)(&msm_header.sta_id), sizeof(msm_header.sta_id));
  bitIndex += 12;

  GetBitData(&src[3], bitIndex, 30, (U08*)(&msm_header.gnss_epoch_time), sizeof(msm_header.gnss_epoch_time));
  bitIndex += 30;

  GetBitData(&src[3], bitIndex, 1, (U08*)(&msm_header.multi_msg_bit), sizeof(msm_header.multi_msg_bit));
  bitIndex += 1;

  GetBitData(&src[3], bitIndex, 3, (U08*)(&msm_header.iods), sizeof(msm_header.iods));
  bitIndex += 3;

  //Reserved 7 bits
  //Reserved 2 bits for clock steering ind
  //Reserved 2 bits for external clock ind
  bitIndex += 11;

  GetBitData(&src[3], bitIndex, 1, (U08*)(&msm_header.div_free_smoothing_ind), sizeof(msm_header.div_free_smoothing_ind));
  bitIndex += 1;

  GetBitData(&src[3], bitIndex, 3, (U08*)(&msm_header.smoothing_interval), sizeof(msm_header.smoothing_interval));
  bitIndex += 3;

  GetBitData(&src[3], bitIndex, 64, (U08*)(&msm_header.sat_mask), sizeof(msm_header.sat_mask));
  bitIndex += 64;

  int nSat = GetBitFlagCounts((U08*)&msm_header.sat_mask, sizeof(msm_header.sat_mask));
  if(nSat == 0)
  {
    return 0;
  }

  GetBitData(&src[3], bitIndex, 32, (U08*)(&msm_header.sig_mask), sizeof(msm_header.sig_mask));
  bitIndex += 32;

  GetBitData(&src[3], bitIndex, nSat, (U08*)(&msm_header.cell_mask), sizeof(msm_header.cell_mask));
  bitIndex += nSat;

  //Table 3.5-81 Content of Satellite Data for MSM5 and MSM7
  int i;
  for(i = 0; i < nSat; ++i)
  {
    GetBitData(&src[3], bitIndex, 8, (U08*)&(satData[i].rough_range_mudulo_1_ms), sizeof(satData[i].rough_range_mudulo_1_ms));
    bitIndex += 8;
  }
  for(i = 0; i < nSat; ++i)
  {
    GetBitData(&src[3], bitIndex, 4, (U08*)&(satData[i].ext_sat_info), sizeof(satData[i].ext_sat_info));
    bitIndex += 4;
  }
  for(i = 0; i < nSat; ++i)
  {
    GetBitData(&src[3], bitIndex, 10, (U08*)&(satData[i].rough_range_mudulo_1_ms), sizeof(satData[i].rough_range_mudulo_1_ms));
    bitIndex += 10;
  }
  for(i = 0; i < nSat; ++i)
  {
    GetBitData(&src[3], bitIndex, 14, (U08*)&(satData[i].rough_phase_range_rate), sizeof(satData[i].rough_phase_range_rate));
    bitIndex += 14;
  }

  //Table 3.5-88 Content of Signal Data for MSM7
  for(i = 0; i < nSat; ++i)
  {
    GetBitData(&src[3], bitIndex, 20, (U08*)&(sigData[i].fine_pseudorange_ext_resol), sizeof(sigData[i].fine_pseudorange_ext_resol));
    bitIndex += 20;
  }
  for(i = 0; i < nSat; ++i)
  {
    GetBitData(&src[3], bitIndex, 24, (U08*)&(sigData[i].fine_phase_range_ext_resol), sizeof(sigData[i].fine_phase_range_ext_resol));
    bitIndex += 24;
  }
  for(i = 0; i < nSat; ++i)
  {
    GetBitData(&src[3], bitIndex, 10, (U08*)&(sigData[i].lock_time_ind_ext_resol), sizeof(sigData[i].lock_time_ind_ext_resol));
    bitIndex += 10;
  }
  for(i = 0; i < nSat; ++i)
  {
    GetBitData(&src[3], bitIndex, 1, (U08*)&(sigData[i].half_cycle_ambiguity_ind), sizeof(sigData[i].half_cycle_ambiguity_ind));
    bitIndex += 1;
  }
  for(i = 0; i < nSat; ++i)
  {
    GetBitData(&src[3], bitIndex, 10, (U08*)&(sigData[i].snr_ext_resol), sizeof(sigData[i].snr_ext_resol));
    bitIndex += 10;
  }
  for(i = 0; i < nSat; ++i)
  {
    GetBitData(&src[3], bitIndex, 15, (U08*)&(sigData[i].fine_phase_range_rate), sizeof(sigData[i].fine_phase_range_rate));
    bitIndex += 15;
  }
  return nSat;
}

int GetNoneZeroBitPosition(U08* d, int size, int order)
{
  int count = 0;
  for(int i = 0; i < size * 8; ++i)
  {
    if((d[size - i / 8 - 1] >> (8 - (i % 8) - 1)) & 0x1)
    {
      if(count++ == order)
      {
        return i;
      }
    }
  }
  return -1;
}

void ShowRtcmMsm7Data(int satNum, const char* title, U08* src, bool convertOnly, CString* pStr)
{
  //SetListCtrlRedraw(FALSE);
  strBuffer.Format("$%s,sta_id=0x%04X,gnss_epoch_time=0x%08X,multi_msg_bit=0x%02X,iods=0x%02X,div_free_smoothing_ind=0x%02X,smoothing_interval=0x%02X,sig_mask=0x%08X,sat_mask=0x%016llX,cell_mask=0x%016llX",
    title,
    msm_header.sta_id,
    msm_header.gnss_epoch_time,
    msm_header.multi_msg_bit,
    msm_header.iods,
    msm_header.div_free_smoothing_ind,
    msm_header.smoothing_interval,
    msm_header.sig_mask,
    msm_header.sat_mask,
    msm_header.cell_mask);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());
  
  if(satNum == 0)
  {
    //SetListCtrlRedraw(TRUE);
    return;
  }

  strBuffer.Format("Content of Satellite Data for MSM5 and MSM7");
  strBuffer.Format("    |Rough Range|Ext. Sat.|Rough Range|Rough Phase|");
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());
  strBuffer.Format(" NO.|      In ms|     Info|Mudulo 1 ms| Range Rate|");
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());
  strBuffer.Format(" ---+-----------+---------+-----------+-----------+");
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());
  for (int i = 0; i < satNum; ++i)
	{
    strBuffer.Format("  %02d|       0x%02X|     0x%02X|     0x%04X|     0x%04X|",
      i,
      satData[i].rough_range_in_ms,
      satData[i].ext_sat_info,
      satData[i].rough_range_mudulo_1_ms,
      satData[i].rough_phase_range_rate);
    DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());
	}

  strBuffer.Format("Content of Signal Data for MSM7");
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());
  strBuffer.Format("    |    Fine Psedudo |Fine Phase Range|  Time Ind.|    Half Cycle|SNR Ext.|Fine Phase|");
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());
  strBuffer.Format(" NO.|Range Ext. Resol.|     Ext. Resol.|Ext. Resol.|Ambiguity Ind.|  Resol.|Range Rate|");
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());
  strBuffer.Format(" ---+-----------------+----------------+-----------+--------------+--------+----------+");
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());
  for (int i = 0; i < satNum; ++i)
	{
    strBuffer.Format("  %02d|       0x%08X|      0x%08X|     0x%04X|          0x%02X|  0x%04X|    0x%04X|",
      i,
      sigData[i].fine_pseudorange_ext_resol,
      sigData[i].fine_phase_range_ext_resol,
      sigData[i].lock_time_ind_ext_resol,
      sigData[i].half_cycle_ambiguity_ind,
      sigData[i].snr_ext_resol,
      sigData[i].fine_phase_range_rate);
    DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());
	}
  //SetListCtrlRedraw(TRUE);
}

void ShowRtcm1077(U08* src, bool convertOnly, CString* pStr)
{
  int satNum = ParsingMSM7(src);
  ShowRtcmMsm7Data(satNum, "Rtcm1077", src, convertOnly, pStr);

  if(convertOnly)
	{
		return;
	}

  int pos = CGPSDlg::gpsDlg->nmea.ClearSatellitesInRange(NMEA::Gps, 1, 32);
  for (int i = 0; i < satNum; ++i)
	{
    int bitOrder = GetNoneZeroBitPosition((U08*)(&msm_header.sat_mask), sizeof(msm_header.sat_mask), i);

    CGPSDlg::gpsDlg->nmea.satellites_gp.SetSate(1 + bitOrder, 0, 0, (F32)(sigData[i].snr_ext_resol / 16));
	  //CGPSDlg::gpsDlg->nmea.satellites_gp[pos + i].SatelliteID = 1 + bitOrder;
	  //CGPSDlg::gpsDlg->nmea.satellites_gp[pos + i].SNR = sigData[i].snr_ext_resol / 16;
	  //CGPSDlg::gpsDlg->nmea.satellites_gp[pos + i].Elevation = 0;
	  //CGPSDlg::gpsDlg->nmea.satellites_gp[pos + i].Azimuth = 0;
  }
	CGPSDlg::gpsDlg->m_gpgsvMsgCopy.NumOfSate = satNum;
	CGPSDlg::gpsDlg->m_gpgsvMsgCopy.NumOfMessage = 1;
	CGPSDlg::gpsDlg->m_gpgsvMsgCopy.SequenceNum = 1;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.NumsOfSatellites = 0;
}

void ShowRtcm1107(U08* src, bool convertOnly, CString* pStr)
{
  int satNum = ParsingMSM7(src);
  ShowRtcmMsm7Data(satNum, "Rtcm1107", src, convertOnly, pStr);

  if(convertOnly)
	{
		return;
	}

  int pos = CGPSDlg::gpsDlg->nmea.ClearSatellitesInRange(NMEA::Gps, 33, 64);
  for (int i = 0; i < satNum; ++i)
	{
    int bitOrder = GetNoneZeroBitPosition((U08*)(&msm_header.sat_mask), sizeof(msm_header.sat_mask), i);

    CGPSDlg::gpsDlg->nmea.satellites_gp.SetSate(33 + bitOrder, 0, 0, (F32)(sigData[i].snr_ext_resol / 16));
	  //CGPSDlg::gpsDlg->nmea.satellites_gp[pos + i].SatelliteID = 33 + bitOrder;
	  //CGPSDlg::gpsDlg->nmea.satellites_gp[pos + i].SNR = sigData[i].snr_ext_resol / 16;
	  //CGPSDlg::gpsDlg->nmea.satellites_gp[pos + i].Elevation = 0;
	  //CGPSDlg::gpsDlg->nmea.satellites_gp[pos + i].Azimuth = 0;
  }
	CGPSDlg::gpsDlg->m_gpgsvMsgCopy.NumOfSate = satNum;
	CGPSDlg::gpsDlg->m_gpgsvMsgCopy.NumOfMessage = 1;
	CGPSDlg::gpsDlg->m_gpgsvMsgCopy.SequenceNum = 1;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.NumsOfSatellites = 0;
}

void ShowRtcm1117(U08* src, bool convertOnly, CString* pStr)
{
  int satNum = ParsingMSM7(src);
  ShowRtcmMsm7Data(satNum, "Rtcm1117", src, convertOnly, pStr);

  if(convertOnly)
	{
		return;
	}

  int pos = CGPSDlg::gpsDlg->nmea.ClearSatellitesInRange(NMEA::Gps, 193, 195);
  for (int i = 0; i < satNum; ++i)
	{
    int bitOrder = GetNoneZeroBitPosition((U08*)(&msm_header.sat_mask), sizeof(msm_header.sat_mask), i);

    CGPSDlg::gpsDlg->nmea.satellites_gp.SetSate(193 + bitOrder, 0, 0, (F32)(sigData[i].snr_ext_resol / 16));
	  //CGPSDlg::gpsDlg->nmea.satellites_gp[pos + i].SatelliteID = 193 + bitOrder;
	  //CGPSDlg::gpsDlg->nmea.satellites_gp[pos + i].SNR = sigData[i].snr_ext_resol / 16;
	  //CGPSDlg::gpsDlg->nmea.satellites_gp[pos + i].Elevation = 0;
	  //CGPSDlg::gpsDlg->nmea.satellites_gp[pos + i].Azimuth = 0;
  }
	CGPSDlg::gpsDlg->m_gpgsvMsgCopy.NumOfSate = satNum;
	CGPSDlg::gpsDlg->m_gpgsvMsgCopy.NumOfMessage = 1;
	CGPSDlg::gpsDlg->m_gpgsvMsgCopy.SequenceNum = 1;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.NumsOfSatellites = 0;
}

void ShowRtcm1127(U08* src, bool convertOnly, CString* pStr)
{
  int satNum = ParsingMSM7(src);
  ShowRtcmMsm7Data(satNum, "Rtcm1127", src, convertOnly, pStr);

  if(convertOnly)
	{
		return;
	}

  int pos = CGPSDlg::gpsDlg->nmea.ClearSatellitesInRange(NMEA::Beidou, 201, 232);
  for (int i = 0; i < satNum; ++i)
	{
    int bitOrder = GetNoneZeroBitPosition((U08*)(&msm_header.sat_mask), sizeof(msm_header.sat_mask), i);

    CGPSDlg::gpsDlg->nmea.satellites_bd.SetSate(201 + bitOrder, 0, 0, (F32)(sigData[i].snr_ext_resol / 16));
	  //CGPSDlg::gpsDlg->nmea.satellites_bd[pos + i].SatelliteID = 201 + bitOrder;
	  //CGPSDlg::gpsDlg->nmea.satellites_bd[pos + i].SNR = sigData[i].snr_ext_resol / 16;
	  //CGPSDlg::gpsDlg->nmea.satellites_bd[pos + i].Elevation = 0;
	  //CGPSDlg::gpsDlg->nmea.satellites_bd[pos + i].Azimuth = 0;
  }                
	CGPSDlg::gpsDlg->m_bdgsvMsgCopy.NumOfSate = satNum;
	CGPSDlg::gpsDlg->m_bdgsvMsgCopy.NumOfMessage = 1;
	CGPSDlg::gpsDlg->m_bdgsvMsgCopy.SequenceNum = 1;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.NumsOfSatellites = 0;
}

void ShowRtcm1087(U08* src, bool convertOnly, CString* pStr)
{
  int satNum = ParsingMSM7(src);
  ShowRtcmMsm7Data(satNum, "Rtcm1087", src, convertOnly, pStr);

  if(convertOnly)
	{
		return;
	}

  int pos = CGPSDlg::gpsDlg->nmea.ClearSatellitesInRange(NMEA::Glonass, 65, 100);
  for (int i = 0; i < satNum; ++i)
	{
    int bitOrder = GetNoneZeroBitPosition((U08*)(&msm_header.sat_mask), sizeof(msm_header.sat_mask), i);

    CGPSDlg::gpsDlg->nmea.satellites_gl.SetSate(65 + bitOrder, 0, 0, (F32)(sigData[i].snr_ext_resol / 16));
	  //CGPSDlg::gpsDlg->nmea.satellites_gl[pos + i].SatelliteID = 65 + bitOrder;
	  //CGPSDlg::gpsDlg->nmea.satellites_gl[pos + i].SNR = sigData[i].snr_ext_resol / 16;
	  //CGPSDlg::gpsDlg->nmea.satellites_gl[pos + i].Elevation = 0;
	  //CGPSDlg::gpsDlg->nmea.satellites_gl[pos + i].Azimuth = 0;
  }                
	CGPSDlg::gpsDlg->m_glgsvMsgCopy.NumOfSate = satNum;
	CGPSDlg::gpsDlg->m_glgsvMsgCopy.NumOfMessage = 1;
	CGPSDlg::gpsDlg->m_glgsvMsgCopy.SequenceNum = 1;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.NumsOfSatellites = 0;
}

void ShowUbxTimTp(U08* src, bool convertOnly, CString* pStr)
{
  U32 towMs = 0;
  U32 towSubMs = 0;
  S32 qErr = 0;
  U16 week = 0;

  GetByteDataFromLE(&src[6], 0, 4, (U08*)(&towMs), sizeof(towMs));
  GetByteDataFromLE(&src[6], 4, 4, (U08*)(&towSubMs), sizeof(towSubMs));
  GetByteDataFromLE(&src[6], 8, 4, (U08*)(&qErr), sizeof(qErr));
  GetByteDataFromLE(&src[6], 12, 2, (U08*)(&week), sizeof(week));

  if(convertOnly)
  {
    strBuffer.Format("$UBX-TIM-TP,%d,%d,%d,%d",
      towMs, towSubMs, qErr, week);
  }
  else
  {
    strBuffer.Format("$UBX-TIM-TP(0x0D,0x01),towMS=%d,towSubMS=%d,qErr=%d,week=%d",
      towMs, towSubMs, qErr, week);
  }
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

  if(convertOnly)
	{
		return;
	}

	UtcTime utc;
	UtcConvertGpsToUtcTime(week, (towMs / 1000.0), &utc);

	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Hour = utc.hour;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Min = utc.minute;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Sec = utc.sec;

	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Year = utc.year;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Month = utc.month;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Day = utc.day;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Hour = utc.hour;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Min = utc.minute;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Sec = utc.sec;
}

void ShowUbxNavSol(U08* src, bool convertOnly, CString* pStr)
{
  U32 towMs = 0;
  U32 towNs = 0;
  U16 week = 0;
  U08 fixMode = 0, flags = 0;
  GetByteDataFromLE(&src[6], 0, 4, (U08*)(&towMs), sizeof(towMs));
  GetByteDataFromLE(&src[6], 4, 4, (U08*)(&towNs), sizeof(towNs));
  GetByteDataFromLE(&src[6], 8, 2, (U08*)(&week), sizeof(week));
  GetByteDataFromLE(&src[6], 10, 1, (U08*)(&fixMode), sizeof(fixMode));
  GetByteDataFromLE(&src[6], 11, 1, (U08*)(&flags), sizeof(flags));

  S32 ecefX = 0, ecefY = 0, ecefZ = 0;
  U32 pAcc = 0;
  GetByteDataFromLE(&src[6], 12, 4, (U08*)(&ecefX), sizeof(ecefX));
  GetByteDataFromLE(&src[6], 16, 4, (U08*)(&ecefY), sizeof(ecefY));
  GetByteDataFromLE(&src[6], 20, 4, (U08*)(&ecefZ), sizeof(ecefZ));
  GetByteDataFromLE(&src[6], 24, 4, (U08*)(&pAcc), sizeof(pAcc));

  S32 ecefVX = 0, ecefVY = 0, ecefVZ = 0;
  U32 sAcc = 0;
  GetByteDataFromLE(&src[6], 28, 4, (U08*)(&ecefVX), sizeof(ecefVX));
  GetByteDataFromLE(&src[6], 32, 4, (U08*)(&ecefVY), sizeof(ecefVY));
  GetByteDataFromLE(&src[6], 36, 4, (U08*)(&ecefVZ), sizeof(ecefVZ));
  GetByteDataFromLE(&src[6], 40, 4, (U08*)(&sAcc), sizeof(sAcc));

  U16 pDop = 0;
  U08 numSv = 0;
  U08 rev1 = 0, rev2 = 0;
  GetByteDataFromLE(&src[6], 44, 2, (U08*)(&pDop), sizeof(pDop));
  GetByteDataFromLE(&src[6], 46, 1, (U08*)(&rev1), sizeof(rev1));
  GetByteDataFromLE(&src[6], 47, 1, (U08*)(&numSv), sizeof(numSv));
  GetByteDataFromLE(&src[6], 48, 1, (U08*)(&rev2), sizeof(rev2));

  if(convertOnly)
  {
    strBuffer.Format("$UBX-NAV-SOL,%d,%d,%d,%d,%08Xh," \
      "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",
      towMs, towNs, week, fixMode, flags,
      ecefX, ecefY, ecefZ, pAcc, ecefVX, ecefVY, ecefVZ, sAcc, pDop, numSv);
  }
  else
  {
    /*
    strBuffer.Format("$UBX-NAV-SOL(0x01,0x06),iTOW=%d,fTOW=%d,week=%d,gpsFix=%d,flags=0x%08X," \
      "ecefX=%d,ecefY=%d,ecefZ=%d,pAcc=%d,ecefVX=%d,ecefVY=%d,ecefVZ=%d,sAcc=%d,pDop=%d,numSv=%d",
      towMs, towNs, week, fixMode, flags,
      ecefX, ecefY, ecefZ, pAcc, ecefVX, ecefVY, ecefVZ, sAcc, pDop, numSv);
    */
    strBuffer.Format("$UBX-NAV-SOL(0x01,0x06),rev1=%d,rev2=%d,iTOW=%d,fTOW=%d,week=%d,gpsFix=%d,flags=0x%08X," \
      "ecefX=%d,ecefY=%d,ecefZ=%d,pAcc=%d,ecefVX=%d,ecefVY=%d,ecefVZ=%d,sAcc=%d,pDop=%d,numSv=%d",
      rev1,rev2,towMs, towNs, week, fixMode, flags,
      ecefX, ecefY, ecefZ, pAcc, ecefVX, ecefVY, ecefVZ, sAcc, pDop, numSv);
  }

  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

  if(convertOnly)
	{
		return;
	}

  if(fixMode == 0)
  {
		CGPSDlg::gpsDlg->m_gpggaMsgCopy.GPSQualityIndicator = '0';
		CGPSDlg::gpsDlg->m_gpgsaMsgCopy.Mode = 1;
		CGPSDlg::gpsDlg->m_gprmcMsgCopy.ModeIndicator = 'V';
    //return;
  }
	else if(fixMode == 2 || fixMode == 3)
	{
		CGPSDlg::gpsDlg->m_gpggaMsgCopy.GPSQualityIndicator = '1';
		CGPSDlg::gpsDlg->m_gpgsaMsgCopy.Mode = fixMode;
		CGPSDlg::gpsDlg->m_gprmcMsgCopy.ModeIndicator = 'A';
	}
	else if(fixMode == 4 || fixMode == 5)
	{
		CGPSDlg::gpsDlg->m_gpggaMsgCopy.GPSQualityIndicator = '1';
		CGPSDlg::gpsDlg->m_gpgsaMsgCopy.Mode = 3;
		CGPSDlg::gpsDlg->m_gprmcMsgCopy.ModeIndicator = 'A';
	}
	else if(fixMode == 6)
	{
		CGPSDlg::gpsDlg->m_gpggaMsgCopy.GPSQualityIndicator = '2';
		CGPSDlg::gpsDlg->m_gpgsaMsgCopy.Mode = 3;
		CGPSDlg::gpsDlg->m_gprmcMsgCopy.ModeIndicator = 'D';
	}
	else if(fixMode == 7)
	{
		CGPSDlg::gpsDlg->m_gpggaMsgCopy.GPSQualityIndicator = '5';
		CGPSDlg::gpsDlg->m_gpgsaMsgCopy.Mode = 3;
		CGPSDlg::gpsDlg->m_gprmcMsgCopy.ModeIndicator = 'F';
	}
	else if(fixMode == 8)
	{
		CGPSDlg::gpsDlg->m_gpggaMsgCopy.GPSQualityIndicator = '4';
		CGPSDlg::gpsDlg->m_gpgsaMsgCopy.Mode = 3;
		CGPSDlg::gpsDlg->m_gprmcMsgCopy.ModeIndicator = 'R';
	}
  CGPSDlg::gpsDlg->m_gpggaMsgCopy.NumsOfSatellites = numSv;

	UtcTime utc;
	UtcConvertGpsToUtcTime(week, (towMs / 1000.0), &utc);
 
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Hour = utc.hour;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Min = utc.minute;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Sec = utc.sec;

	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Year = utc.year;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Month = utc.month;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Day = utc.day;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Hour = utc.hour;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Min = utc.minute;
	CGPSDlg::gpsDlg->m_gprmcMsgCopy.Sec = utc.sec;
  if(fixMode == 0)
  {
    return;
  }

  D64 latitude, longitude, altitude;
  ConvertEcefToLonLatAlt(ecefX / 100, ecefY / 100, ecefZ / 100, latitude, longitude, altitude);
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Latitude = latitude;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Latitude_N_S = (latitude >= 0) ? 'N' : 'S';
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Longitude = longitude;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Longitude_E_W = (longitude >= 0) ? 'E' : 'W';
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Altitude = (F32)altitude - CGPSDlg::gpsDlg->m_gpggaMsgCopy.GeoidalSeparation;
}

U08 AdjustUbloxPrn(U08 p)
{
    if(p >= 1 && p <= 32)
    { //GPS
      return p;
    }
    else if(p >= 130 && p <= 148)
    { //SBAS
      return p - 97;
    }
    else if(p >= 65 && p <= 96)
    { //GLONASS
      return p;
    }
    else if(p >= 33 && p <= 64)
    { //Beidou
      return p + 168;
    }
    return p;
}

void ShowUbxNavSvInfo(U08* src, bool convertOnly, CString* pStr)
{
  U32 towMs = 0;
  U08 numCh = 0;
  U08 globalFlags = 0;
  U16 rev = 0;
  GetByteDataFromLE(&src[6], 0, 4, (U08*)(&towMs), sizeof(towMs));
  GetByteDataFromLE(&src[6], 4, 1, (U08*)(&numCh), sizeof(numCh));
  GetByteDataFromLE(&src[6], 5, 1, (U08*)(&globalFlags), sizeof(globalFlags));
  GetByteDataFromLE(&src[6], 6, 2, (U08*)(&rev), sizeof(rev));

  if(convertOnly)
  {
    strBuffer.Format("$UBX-NAV-SVINFO,%d,%d,%02Xh", towMs, numCh, globalFlags);
  }
  else
  {
    //strBuffer.Format("$UBX-NAV-SVINFO(0x01,0x30),iTOW=%d,numCh=%d,globalFlags=0x%02X", towMs, numCh, globalFlags);
    strBuffer.Format("$UBX-NAV-SVINFO(0x01,0x30),rev=%d,iTOW=%d,numCh=%d,globalFlags=0x%02X", rev, towMs, numCh, globalFlags);
  }
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());
  
  if(numCh == 0 || convertOnly)
  {
    return;
  }

  strBuffer.Format("  chn|svid|flags| qty|cn0|ele| azm|prRes|");
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

  strBuffer.Format(" ----+----+-----+----+---+---+----+-----+");
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	int fixed_gps_c = 0, fixed_glonass_c = 0, fixed_beidou_c = 0, fixed_galileo_c = 0, fixed_navic_c = 0;
	int gps_c = 0, glonass_c = 0, beidou_c = 0, galileo_c = 0, navic_c = 0;
	SV_CH_DATA sv;
	if(!convertOnly)
	{
		memset(CGPSDlg::gpsDlg->m_gigsaMsgCopy.SatelliteID, 0, sizeof(CGPSDlg::gpsDlg->m_gigsaMsgCopy.SatelliteID));
		memset(CGPSDlg::gpsDlg->m_gagsaMsgCopy.SatelliteID, 0, sizeof(CGPSDlg::gpsDlg->m_gagsaMsgCopy.SatelliteID));
		memset(CGPSDlg::gpsDlg->m_bdgsaMsgCopy.SatelliteID, 0, sizeof(CGPSDlg::gpsDlg->m_bdgsaMsgCopy.SatelliteID));
		memset(CGPSDlg::gpsDlg->m_glgsaMsgCopy.SatelliteID, 0, sizeof(CGPSDlg::gpsDlg->m_glgsaMsgCopy.SatelliteID));
		memset(CGPSDlg::gpsDlg->m_gpgsaMsgCopy.SatelliteID, 0, sizeof(CGPSDlg::gpsDlg->m_gpgsaMsgCopy.SatelliteID));
	}

	for (int i=0; i<numCh; ++i)
	{
    S32 prRes = 0;
    S08 elev = 0;
    GetByteDataFromLE(&src[6], 8 + 12 * i, 1, (U08*)(&sv.channel_id), sizeof(sv.channel_id));
    GetByteDataFromLE(&src[6], 9 + 12 * i, 1, (U08*)(&sv.prn), sizeof(sv.prn));
    GetByteDataFromLE(&src[6], 10 + 12 * i, 1, (U08*)(&sv.SV_status), sizeof(sv.SV_status));
    GetByteDataFromLE(&src[6], 11 + 12 * i, 1, (U08*)(&sv.URA), sizeof(sv.URA));
    GetByteDataFromLE(&src[6], 12 + 12 * i, 1, (U08*)(&sv.cn0), sizeof(sv.cn0));
    GetByteDataFromLE(&src[6], 13 + 12 * i, 1, (U08*)(&elev), sizeof(elev));
    GetByteDataFromLE(&src[6], 14 + 12 * i, 2, (U08*)(&sv.azimuth), sizeof(sv.azimuth));
    GetByteDataFromLE(&src[6], 16 + 12 * i, 4, (U08*)(&prRes), sizeof(prRes));

    strBuffer.Format("%5d|%4d| 0x%02X|%4d|%3d|%3d|%4d|%5d|",
			sv.channel_id, sv.prn, sv.SV_status, sv.URA,sv.cn0, 
			elev, sv.azimuth, prRes);
    DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());
	  if(convertOnly)
	  {
		  continue;
	  }

    sv.prn = AdjustUbloxPrn(sv.prn);
    NMEA::GNSS_System gs = NMEA::GetGNSSSystem(sv.prn);

		if(NMEA::Gps == gs)
		{
			if(gps_c == 0)
			{
        CGPSDlg::gpsDlg->nmea.satellites_gp.Clear();
			}
			if(sv.SV_status & 0x01)
			{
				CGPSDlg::gpsDlg->m_gpgsaMsgCopy.SatelliteID[fixed_gps_c] = sv.prn;
				fixed_gps_c++;
			}

      CGPSDlg::gpsDlg->nmea.satellites_gp.SetSate(sv.prn, elev, sv.azimuth, sv.cn0);
			gps_c++;
		}
		else if(NMEA::Glonass == gs)
		{
			if(glonass_c==0)
			{
        CGPSDlg::gpsDlg->nmea.satellites_gl.Clear();
			}
			if(sv.SV_status & 0x01)
			{
				CGPSDlg::gpsDlg->m_glgsaMsgCopy.SatelliteID[fixed_glonass_c] = sv.prn;
				fixed_glonass_c++;
			}

      CGPSDlg::gpsDlg->nmea.satellites_gl.SetSate(sv.prn, elev, sv.azimuth, sv.cn0);
			glonass_c++;
    }
		else if(NMEA::Beidou == gs)
		{
			if(beidou_c==0)
			{
        CGPSDlg::gpsDlg->nmea.satellites_bd.Clear();
			}
			if(sv.SV_status & 0x01)
			{
				CGPSDlg::gpsDlg->m_bdgsaMsgCopy.SatelliteID[fixed_beidou_c] = sv.prn;
				fixed_beidou_c++;
			}

      CGPSDlg::gpsDlg->nmea.satellites_bd.SetSate(sv.prn, elev, sv.azimuth, sv.cn0);
			beidou_c++;
    }
		else if(NMEA::Galileo == gs)
		{
			if(galileo_c==0)
			{
        CGPSDlg::gpsDlg->nmea.satellites_ga.Clear();
			}
			if(sv.SV_status & 0x01)
			{
				CGPSDlg::gpsDlg->m_gagsaMsgCopy.SatelliteID[fixed_galileo_c] = sv.prn;
				fixed_galileo_c++;
			}

      CGPSDlg::gpsDlg->nmea.satellites_ga.SetSate(sv.prn, elev, sv.azimuth, sv.cn0);
			galileo_c++;
    }
		else if(NMEA::Navic == gs)
		{
			if(navic_c==0)
			{
        CGPSDlg::gpsDlg->nmea.satellites_gi.Clear();
			}
			if(sv.SV_status & 0x01)
			{
				CGPSDlg::gpsDlg->m_gigsaMsgCopy.SatelliteID[fixed_navic_c] = sv.prn;
				fixed_navic_c++;
			}

      CGPSDlg::gpsDlg->nmea.satellites_gi.SetSate(sv.prn, elev, sv.azimuth, sv.cn0);
			navic_c++;
    }


	  CGPSDlg::gpsDlg->m_gpgsvMsgCopy.NumOfSate = gps_c;
	  CGPSDlg::gpsDlg->m_gpgsvMsgCopy.NumOfMessage = (gps_c + 3) / 4;
	  CGPSDlg::gpsDlg->m_gpgsvMsgCopy.SequenceNum = (gps_c + 3) / 4;
	  CGPSDlg::gpsDlg->m_gpggaMsgCopy.NumsOfSatellites = fixed_gps_c + fixed_glonass_c;

	  CGPSDlg::gpsDlg->m_glgsvMsgCopy.NumOfSate = glonass_c;
	  CGPSDlg::gpsDlg->m_glgsvMsgCopy.NumOfMessage = (glonass_c + 3) / 4;
	  CGPSDlg::gpsDlg->m_glgsvMsgCopy.SequenceNum = (glonass_c + 3) / 4;

	  CGPSDlg::gpsDlg->m_bdgsvMsgCopy.NumOfSate = beidou_c;
	  CGPSDlg::gpsDlg->m_bdgsvMsgCopy.NumOfMessage = (beidou_c + 3) / 4;
	  CGPSDlg::gpsDlg->m_bdgsvMsgCopy.SequenceNum = (beidou_c + 3) / 4;
  }

}

void ShowUbxNavSvStatus(U08* src, bool convertOnly, CString* pStr)
{
  U32 towMs = 0;
  U08 gpsFix = 0;
  U08 flags = 0;
  U08 fixStat = 0;
  U08 flags2 = 0;
  U32 ttff = 0;
  U32 msss = 0;

  GetByteDataFromLE(&src[6], 0, 4, (U08*)(&towMs), sizeof(towMs));
  GetByteDataFromLE(&src[6], 4, 1, (U08*)(&gpsFix), sizeof(gpsFix));
  GetByteDataFromLE(&src[6], 5, 1, (U08*)(&flags), sizeof(flags));
  GetByteDataFromLE(&src[6], 6, 1, (U08*)(&fixStat), sizeof(fixStat));
  GetByteDataFromLE(&src[6], 7, 1, (U08*)(&flags2), sizeof(flags2));
  GetByteDataFromLE(&src[6], 8, 4, (U08*)(&ttff), sizeof(ttff));
  GetByteDataFromLE(&src[6], 12, 4, (U08*)(&msss), sizeof(msss));

  if(convertOnly)
  {
    strBuffer.Format("$UBX-NAV-STATUS,%d,%d,%02Xh,%02Xh," \
      "%d,%d,%d", towMs, gpsFix, flags, fixStat, flags2, ttff, msss);
  }
  else
  {
    strBuffer.Format("$UBX-NAV-STATUS(0x01,0x03),iTOW=%d,gpsFix=%d,flags=0x%02X,fixStat=0x%02X," \
      "flags2=%d,ttff=%d,msss=%d", towMs, gpsFix, flags, fixStat, flags2, ttff, msss);
  }
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

  //if(convertOnly)
	//{
	//	return;
	//}
}

void ShowUbxNavPosllh(U08* src, bool convertOnly, CString* pStr)
{
  U32 iTOW = 0;
  S32 lon = 0;
  S32 lat = 0;
  S32 height = 0;
  S32 hMSL = 0;
  U32 hAcc = 0;
  U32 vAcc = 0;

  GetByteDataFromLE(&src[6], 0, 4, (U08*)(&iTOW), sizeof(iTOW));
  GetByteDataFromLE(&src[6], 4, 4, (U08*)(&lon), sizeof(lon));
  GetByteDataFromLE(&src[6], 8, 4, (U08*)(&lat), sizeof(lat));
  GetByteDataFromLE(&src[6], 12, 4, (U08*)(&height), sizeof(height));
  GetByteDataFromLE(&src[6], 16, 4, (U08*)(&hMSL), sizeof(hMSL));
  GetByteDataFromLE(&src[6], 20, 4, (U08*)(&hAcc), sizeof(hAcc));
  GetByteDataFromLE(&src[6], 24, 4, (U08*)(&vAcc), sizeof(vAcc));

  if(convertOnly)
  {
    strBuffer.Format("$UBX-NAV-POSLLH,%d,%d,%d,%d," \
      "%d,%d,%d", iTOW, lon, lat, height, hMSL, hAcc, vAcc);
  }
  else
  {
    strBuffer.Format("$UBX-NAV-POSLLH(0x01,0x02),iTOW=%d,lon=%d,lat=%d,height=%d," \
      "hMSL=%d,hAcc=%d,vAcc=%d", iTOW, lon, lat, height, hMSL, hAcc, vAcc);
  }
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

  if(convertOnly)
	{
		return;
	}

  CGPSDlg::gpsDlg->m_gpggaMsgCopy.GeoidalSeparation = (F32)(height - hMSL) / 1000;
}

void ShowUbxNavDop(U08* src, bool convertOnly, CString* pStr)
{
  U32 iTOW = 0;
  U16 gDOP = 0;
  U16 pDOP = 0;
  U16 tDOP = 0;
  U16 vDOP = 0;
  U16 hDOP = 0;
  U16 nDOP = 0;
  U16 eDOP = 0;

  GetByteDataFromLE(&src[6], 0, 4, (U08*)(&iTOW), sizeof(iTOW));
  GetByteDataFromLE(&src[6], 4, 2, (U08*)(&gDOP), sizeof(gDOP));
  GetByteDataFromLE(&src[6], 6, 2, (U08*)(&pDOP), sizeof(pDOP));
  GetByteDataFromLE(&src[6], 8, 2, (U08*)(&tDOP), sizeof(tDOP));
  GetByteDataFromLE(&src[6], 10, 2, (U08*)(&vDOP), sizeof(vDOP));
  GetByteDataFromLE(&src[6], 12, 2, (U08*)(&hDOP), sizeof(hDOP));
  GetByteDataFromLE(&src[6], 14, 2, (U08*)(&nDOP), sizeof(nDOP));
  GetByteDataFromLE(&src[6], 16, 2, (U08*)(&eDOP), sizeof(eDOP));

  if(convertOnly)
  {
    strBuffer.Format("$UBX-NAV-DOP,%d,%d,%d,%d," \
      "%d,%d,%d,%d", iTOW, gDOP, pDOP, tDOP, vDOP, hDOP, nDOP, eDOP);
  }
  else
  {
    strBuffer.Format("$UBX-NAV-DOP(0x01,0x04),iTOW=%d,gDOP=%d,pDOP=%d,tDOP=%d," \
      "vDOP=%d,hDOP=%d,nDOP=%d,eDOP=%d", iTOW, gDOP, pDOP, tDOP, vDOP, hDOP, nDOP, eDOP);
  }

  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

  if(convertOnly)
	{
		return;
	}

	CGPSDlg::gpsDlg->m_gpgsaMsgCopy.PDOP = (F32)pDOP / 1000;
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.HDOP = (F32)hDOP / 1000;
	CGPSDlg::gpsDlg->m_gpgsaMsgCopy.VDOP = (F32)vDOP / 1000;
}

void ShowUbxNavVelned(U08* src, bool convertOnly, CString* pStr)
{
  U32 iTOW = 0;
  U16 velN = 0;
  U16 velE = 0;
  U16 velD = 0;
  U32 speed = 0;
  U32 gSpeed = 0;
  S32 heading = 0;
  U32 sAcc = 0;
  U32 cAcc = 0;

  GetByteDataFromLE(&src[6], 0, 4, (U08*)(&iTOW), sizeof(iTOW));
  GetByteDataFromLE(&src[6], 4, 4, (U08*)(&velN), sizeof(velN));
  GetByteDataFromLE(&src[6], 8, 4, (U08*)(&velE), sizeof(velE));
  GetByteDataFromLE(&src[6], 12, 4, (U08*)(&velD), sizeof(velD));
  GetByteDataFromLE(&src[6], 16, 4, (U08*)(&speed), sizeof(speed));
  GetByteDataFromLE(&src[6], 20, 4, (U08*)(&gSpeed), sizeof(gSpeed));
  GetByteDataFromLE(&src[6], 24, 4, (U08*)(&heading), sizeof(heading));
  GetByteDataFromLE(&src[6], 28, 4, (U08*)(&sAcc), sizeof(sAcc));
  GetByteDataFromLE(&src[6], 32, 4, (U08*)(&cAcc), sizeof(cAcc));

  if(convertOnly)
  {
    strBuffer.Format("$UBX-NAV-VELEND,%d,%d,%d,%d,%d,%d,%d,%d,%d", 
      iTOW, velN, velE, velD, speed, gSpeed, heading, sAcc, cAcc);
  }
  else
  {
    strBuffer.Format("$UBX-NAV-VELEND(0x01,0x12),iTOW=%d,velN=%d,velE=%d,velD=%d," \
      "speed=%d,gSpeed=%d,heading=%d,sAcc=%d,cAcc=%d", 
      iTOW, velN, velE, velD, speed, gSpeed, heading, sAcc, cAcc);
  }

  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

  if(convertOnly)
	{
		return;
	}

  CGPSDlg::gpsDlg->m_gprmcMsgCopy.SpeedKnots = (F32)gSpeed / 100 * MS2KMHR / KNOTS2KMHR;
  CGPSDlg::gpsDlg->m_gprmcMsgCopy.TrueCourse = (F32)heading / 100000;
}




