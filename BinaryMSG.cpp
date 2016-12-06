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

static void CooCartesianToGeodetic(const POS_T* xyz_p, LLA_T* lla_p)
{
	D64 p;
	D64 theta;
	D64 temp;
	D64 s_theta, c_theta, s_phi, c_phi;
	D64 phi; // latitude

	p = sqrt( (xyz_p->px)*(xyz_p->px) + (xyz_p->py)*(xyz_p->py) );

	if( p <= 0.01f )
	{
		lla_p->lat = (xyz_p->pz >= 0)?(PI/2.0):(-PI/2.0);
		lla_p->lon = 0.0;
		lla_p->alt = (F32)(fabs(xyz_p->pz) - WGS84_RB);
		return;
	}

	theta = atan2( ( xyz_p->pz*WGS84_RA ), ( p*WGS84_RB ) );
	s_theta = sin(theta);
	c_theta = cos(theta);    

	temp = ( xyz_p->pz + WGS84_E2P*WGS84_RB*s_theta*s_theta*s_theta ); 
	phi = atan2( temp, ( p - WGS84_E2*WGS84_RA*c_theta*c_theta*c_theta ) );

	s_phi = sin(phi);
	c_phi = cos(phi);

	lla_p->lat = phi;    
	lla_p->lon = atan2( xyz_p->py, xyz_p->px );
	lla_p->alt = (F32)( (p / c_phi) - ( WGS84_RA / sqrt(1.0 - WGS84_E2*s_phi*s_phi ) ) );
}

void ShowMeasurementChannel(U08* src, bool convertOnly, CString* pStr)
{
	U08* ptr = &src[5];
  U08 iod = 0;
  U08 nmeas = 0;

	ptr = decode_1bytes(ptr, &iod);
	ptr = decode_1bytes(ptr, &nmeas);

  strBuffer.Format("$RAW_MEAS(0xDD),IOD=%d,NMEAS=%d", iod, nmeas);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

  if(nmeas == 0)
  {
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

  strBuffer.Format("$EXT_RAW_MEAS(0xE5),V%d,IOD=%d,WN=%d,TOW=%d,MeasPeriod=%d,MeasIndicator=0x%02X,NMEAS=%d",
    ver, iod, weeks, tow, measPeriod, measIndFlag, nmeas);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

  if(nmeas == 0)
  {
    return;
  }

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

  strBuffer.Format("$SV_CH_STATUS(0xDE),IOD=%d,NSVS=%d", iod, nsvs);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

  if(nsvs == 0)
  {
    return;
  }

  strBuffer.Format(" ChId|SVID|SvInd| URA|CN0|ELE| AZI|ChInd|");
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

  strBuffer.Format(" ----+----+-----+----+---+---+----+-----+");
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	int fixed_gps_c = 0, fixed_glonass_c = 0, fixed_beidou_c = 0, fixed_galileo_c = 0;
	int gps_c = 0, glonass_c = 0, beidou_c = 0, galileo_c = 0;
	SV_CH_DATA sv;
	if(!convertOnly)
	{
		memset(CGPSDlg::gpsDlg->m_gagsaMsgCopy.SatelliteID, 0, sizeof(CGPSDlg::gpsDlg->m_gagsaMsgCopy.SatelliteID));
		memset(CGPSDlg::gpsDlg->m_bdgsaMsgCopy.SatelliteID, 0, sizeof(CGPSDlg::gpsDlg->m_bdgsaMsgCopy.SatelliteID));
		memset(CGPSDlg::gpsDlg->m_glgsaMsgCopy.SatelliteID, 0, sizeof(CGPSDlg::gpsDlg->m_glgsaMsgCopy.SatelliteID));
		memset(CGPSDlg::gpsDlg->m_gpgsaMsgCopy.SatelliteID, 0, sizeof(CGPSDlg::gpsDlg->m_gpgsaMsgCopy.SatelliteID));
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
	  if(convertOnly)
	  {
		  continue;
	  }

		if(NMEA::Glonass == NMEA::GetGNSSSystem(sv.prn))
		{
			if(glonass_c==0)
			{
				memset(CGPSDlg::gpsDlg->nmea.satellites_gnss, 0, sizeof(CGPSDlg::gpsDlg->nmea.satellites_gnss));
			}
			if (sv.channel_status & 0x30)
			{
				CGPSDlg::gpsDlg->m_glgsaMsgCopy.SatelliteID[fixed_glonass_c] = sv.prn;
				fixed_glonass_c++;
			}
			CGPSDlg::gpsDlg->nmea.satellites_gnss[glonass_c].SatelliteID = sv.prn;
			CGPSDlg::gpsDlg->nmea.satellites_gnss[glonass_c].SNR = sv.cn0;
			CGPSDlg::gpsDlg->nmea.satellites_gnss[glonass_c].Elevation = sv.elevation;
			CGPSDlg::gpsDlg->nmea.satellites_gnss[glonass_c].Azimuth = sv.azimuth;
			glonass_c++;
		}
		else if(NMEA::Gps == NMEA::GetGNSSSystem(sv.prn))
		{
			if(gps_c==0)
			{
				memset(CGPSDlg::gpsDlg->nmea.satellites_gps, 0, sizeof(CGPSDlg::gpsDlg->nmea.satellites_gnss));
			}
			if (sv.channel_status & 0x30)
			{
				CGPSDlg::gpsDlg->m_gpgsaMsgCopy.SatelliteID[fixed_gps_c] = sv.prn;
				fixed_gps_c++;
			}
			CGPSDlg::gpsDlg->nmea.satellites_gps[gps_c].SatelliteID = sv.prn;
			CGPSDlg::gpsDlg->nmea.satellites_gps[gps_c].SNR = sv.cn0;
			CGPSDlg::gpsDlg->nmea.satellites_gps[gps_c].Elevation = sv.elevation;
			CGPSDlg::gpsDlg->nmea.satellites_gps[gps_c].Azimuth = sv.azimuth;
			gps_c++;
		}
		else if(NMEA::Beidou == NMEA::GetGNSSSystem(sv.prn))
		{
			if(beidou_c==0)
			{
				memset(CGPSDlg::gpsDlg->nmea.satellites_bd, 0, sizeof(CGPSDlg::gpsDlg->nmea.satellites_bd));
			}
			if (sv.channel_status & 0x30)
			{
				CGPSDlg::gpsDlg->m_bdgsaMsgCopy.SatelliteID[fixed_beidou_c] = sv.prn;
				fixed_beidou_c++;
			}
			CGPSDlg::gpsDlg->nmea.satellites_bd[beidou_c].SatelliteID = sv.prn;
			CGPSDlg::gpsDlg->nmea.satellites_bd[beidou_c].SNR = sv.cn0;
			CGPSDlg::gpsDlg->nmea.satellites_bd[beidou_c].Elevation = sv.elevation;
			CGPSDlg::gpsDlg->nmea.satellites_bd[beidou_c].Azimuth = sv.azimuth;
			beidou_c++;
		}
		else if(NMEA::Galileo == NMEA::GetGNSSSystem(sv.prn))
		{
			if(galileo_c==0)
			{
				memset(CGPSDlg::gpsDlg->nmea.satellites_ga, 0, sizeof(CGPSDlg::gpsDlg->nmea.satellites_ga));
			}
			if (sv.channel_status & 0x30)
			{
				CGPSDlg::gpsDlg->m_gagsaMsgCopy.SatelliteID[fixed_galileo_c] = sv.prn;
				fixed_galileo_c++;
			}
			CGPSDlg::gpsDlg->nmea.satellites_ga[galileo_c].SatelliteID = sv.prn;
			CGPSDlg::gpsDlg->nmea.satellites_ga[galileo_c].SNR = sv.cn0;
			CGPSDlg::gpsDlg->nmea.satellites_ga[galileo_c].Elevation = sv.elevation;
			CGPSDlg::gpsDlg->nmea.satellites_ga[galileo_c].Azimuth = sv.azimuth;
			galileo_c++;
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
	CGPSDlg::gpsDlg->m_gpggaMsgCopy.Altitude = lla.alt;

	CGPSDlg::gpsDlg->m_gpgsaMsgCopy.PDOP = receiver.pdop;
	CGPSDlg::gpsDlg->m_gpgsaMsgCopy.HDOP = receiver.hdop;
	CGPSDlg::gpsDlg->m_gpgsaMsgCopy.VDOP = receiver.vdop;
}

void ShowBinaryOutput(U08* src, bool convertOnly, CString* pStr)
{
	U16 wn = src[7] << 8 | src[8];
	U32 tow = src[9]<<24 | src[10]<<16 | src[11]<<8 | src[12];
	D64 ftow = (double)tow / 100.0;

	UtcTime utc;
	UtcConvertGpsToUtcTime(wn, ftow, &utc);

	double lat = src[13]<<24 | src[14]<<16 | src[15]<<8 | src[16];
	double lon = src[17]<<24 | src[18]<<16 | src[19]<<8 | src[20];
	D64 flat = lat;
	flat /= 10000000;
	flat =((int)flat)*100 +(double)(flat - (int)flat)*60;

	D64 flon = lon;
	flon /= 10000000;
	flon = ((int)flon)*100 +(double)(flon - (int)flon)*60;

  U32 d = src[21]<<24 | src[22]<<16 | src[23]<<8 | src[24];
	S32 alt_t = *((S32*)(&d));
  d = src[25]<<24 | src[26]<<16 | src[27]<<8 | src[28];
	S32 alt = *((S32*)(&d));

	U32 gdop = src[29]<<8 | src[30];
	U32 pdop = src[31]<<8 | src[32];
	U32 hdop = src[33]<<8 | src[34];
	U32 vdop = src[35]<<8 | src[36];
	U32 tdop = src[37]<<8 | src[38];

	S32 ecefx = src[39]<<24 | src[40]<<16 | src[41]<<8 | src[42];
	S32 ecefy = src[43]<<24 | src[44]<<16 | src[45]<<8 | src[46];
	S32 ecefz = src[47]<<24 | src[48]<<16 | src[49]<<8 | src[50];

	S32 vx = src[51]<<24 | src[52]<<16 | src[53]<<8 | src[54];
	S32 vy = src[55]<<24 | src[56]<<16 | src[57]<<8 | src[58];
	S32 vz = src[59]<<24 | src[60]<<16 | src[61]<<8 | src[62];

	D64 fx = (D64)vx / 100;
	D64 fy = (D64)vy / 100;
	D64 fz = (D64)vz / 100;
	D64 fv = sqrt(fx * fx + fy * fy + fz * fz);

	strBuffer.Format("$fix mode=%d", src[5]);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$number if sv in fix=%d", src[6]);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$GPS Week=%d", wn);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$GPS TOW=%.2f",ftow);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$latitude=%.6f",flat);
  DisplayAndSave(convertOnly, pStr, strBuffer, strBuffer.GetLength());

	strBuffer.Format("$longutide=%.6f",flon);
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
