#include "stdafx.h"
#include "GPS.h"
#include "BinaryMSG.h"
#include "Serial.h"
#include "WaitReadLog.h"
#include "Redir.h"
#include "MaskedBitmap.h" 
#include "Monitor_1PPS.h"
#include "SysRestartDlg.h"
#include "SnrBarChart.h" 
#include "Pic_Earth.h"
#include "Pic_Scatter.h"
#include "Config1ppsPulseWidthDlg.h"
#include "Timing_start.h"
#include "Config_Proprietary.h"
#include "ConAntennaDetection.h"
#include "ConfigNmeaIntervalDlg.h"
#include "ConSrePorDlg.h"
#include "Config1ppsFrequenceOutput.h"
#include "ConfigEricssonIntervalDlg.h"
#include "GetAlmanac.h"
#include "CommonConfigDlg.h"
#include "CommonQueryDlg.h"
#include "GPSDlg.h"
#include "DrTestDlg.h"
#include "CRC24Q/crc24q_parity.h"

struct CommandEntry
{
	U08 cmdId;
	U08 cmdSubId;
	U16 cmdSize;
	U08 cmdAck;
	U08 cmdAckSub;
};

static CommandEntry cmdTable[] =
{
	//QuerySwVerSysCmd
	{ 0x02, 0x01, 2, 0x80, 0x00 },
	//QuerySwCrcSysCmd
	{ 0x03, 0x01, 2, 0x81, 0x00 },
	//QueryCustomerIDCmd,
	{ 0x0D, 0xFF, 1, 0x92, 0x00 },
	//QueryPositionRateCmd
	{ 0x10, 0xFF, 1, 0x86, 0x00 },
	//QueryPowerModeCmd
	{ 0x15, 0xFF, 1, 0xB9, 0x00 },
	//QueryDataLogStatusCmd,
	{ 0x17, 0xFF, 1, 0x94, 0x00 },
	//DatalogClearCmd,
	{ 0x19, 0xFF, 1, 0x00, 0x00 },
	//DatalogReadCmd,
	{ 0x1D, 0xFF, 5, 0x00, 0x00 },
	//QueryBinaryMeasurementDataOutCmd,
	{ 0x1F, 0xFF, 1, 0x89, 0x00 },
	//QueryRtcmMeasurementDataOutCmd,
	{ 0x21, 0xFF, 1, 0x8A, 0x00 },
	//QueryBasePositionCmd,
	{ 0x23, 0xFF, 1, 0x8B, 0x00 },
	//QueryDatumCmd
	{ 0x2D, 0xFF, 1, 0xAE, 0x00 },
	//QueryDopMaskCmd
	{ 0x2E, 0xFF, 1, 0xAF, 0x00 },
	//QueryElevationAndCnrMaskCmd,
	{ 0x2F, 0xFF, 1, 0xB0, 0x00 },
	//QueryAgpsStatusCmd,
	//{ 0x34, 0xFF, 8, 0xB2, 0x00 },
	//QueryPositionPinningCmd
	{ 0x3A, 0xFF, 1, 0xB4, 0x00 },
	//Query1ppsModeCmd
	{ 0x3F, 0xFF, 1, 0xB6, 0x00 },
	//QueryNoisePowerCmd,
	{ 0x40, 0xFF, 1, 0xB7, 0x00 },
	//QueryTimingCmd
	{ 0x44, 0xFF, 1, 0xC2, 0x00 },
	//QueryCableDelayCmd,
	{ 0x46, 0xFF, 1, 0xBB, 0x00 },
	//QueryAntennaDetectionCmd,
	{ 0x48, 0xFF, 1, 0xBC, 0x00 },
	//QueryProprietaryMessageCmd
	{ 0x4A, 0xFF, 1, 0xBF, 0x00 },
	//QueryGnssNmeaTalkIdCmd,
	{ 0x4F, 0xFF, 1, 0x93, 0x00 },
	//GetGpAlmanacCmd,
	{ 0x50, 0xFF, 2, 0xBE, 0x00 },
	//GetPpsOutputModeCmd,
	{ 0x56, 0xFF, 1, 0xC3, 0x00 },
	//QuerySbasCmd,
	{ 0x62, 0x02, 2, 0x62, 0x80 },
	//QueryQzssCmd,
	{ 0x62, 0x04, 2, 0x62, 0x81 },
	//QuerySbas2Cmd,
	{ 0x62, 0x06, 2, 0x62, 0x82 },
	//QuerySbasDefaultCmd,
	{ 0x62, 0x07, 2, 0x62, 0x83 },
	//QuerySagpsCmd,
	{ 0x63, 0x02, 2, 0x63, 0x80 },
	//QueryGnssBootStatusCmd,
	{ 0x64, 0x01, 2, 0x64, 0x80 },
	//QueryNmeaIntervalV8Cmd,
	{ 0x64, 0x03, 2, 0x64, 0x81 },
	//QueryNmeaBinaryOutputDestinationCmd,
	{ 0x64, 0x05, 2, 0x64, 0x82 },
	//QueryInterferenceDetectControlCmd,
	{ 0x64, 0x07, 2, 0x64, 0x83 },
	//QueryNoisePowerControlCmd,
	{ 0x64, 0x09, 2, 0x64, 0x84 },
	//QueryParameterSearchEngineNumberCmd,
	{ 0x64, 0x0B, 2, 0x64, 0x85 },
	//QueryV8PowerSaveParameters,
	{ 0x64, 0x0D, 2, 0x64, 0x86 },
	//QueryPositionFixNavigationMaskCmd,
	{ 0x64, 0x12, 2, 0x64, 0x88 },
	//QueryRefTimeSyncToGpsTimeCmd,
	{ 0x64, 0x16, 2, 0x64, 0x8A },
	//QueryNavigationModeV8Cmd,
	{ 0x64, 0x18, 2, 0x64, 0x8B },
	//QueryGnssConstellationTypeCmd,
	{ 0x64, 0x1A, 2, 0x64, 0x8C }, 
	//QueryTimeStampingCmd,
	{ 0x64, 0x1E, 2, 0x64, 0x8D },
	//QueryGpsTimeCmd,
	{ 0x64, 0x20, 2, 0x64, 0x8E },
  //QueryPstiCmd
	{ 0x64, 0x22, 3, 0x64, 0x8F },
	//QuerySearchEngineSleepCriteriaCmd,
	{ 0x64, 0x26, 2, 0x64, 0x91 },
	//QueryDatumIndexCmd,
	{ 0x64, 0x28, 2, 0x64, 0x92 },
	//QueryConstellationCapabilityCmd
	{ 0x64, 0x29, 2, 0x64, 0x93 },
	//QuerySignalDisturbanceStatusCmd,
	{ 0x64, 0x2B, 2, 0x64, 0x94 },
	//QuerySignalDisturbanceDataCmd,
	{ 0x64, 0x2C, 2, 0x64, 0x95 },
	//QueryGeofenceCmd,
	{ 0x64, 0x35, 3, 0x64, 0x99 },
	//QueryGeofenceResultCmd,
	{ 0x64, 0x31, 2, 0x64, 0x97 },
	//QueryGeofenceCmdEx,
	{ 0x64, 0x35, 3, 0x64, 0x99 },
	//QueryGeofenceResultCmdEx,
	{ 0x64, 0x36, 2, 0x64, 0x9A },
	//QueryVeryLowSpeedCmd,
	{ 0x64, 0x38, 2, 0x64, 0x9B },
  //QueryNmeaStringCmd
	{ 0x64, 0x3C, 5, 0x64, 0x9D },
	//QueryDataLogStatus2Cmd,
	{ 0x64, 0x47, 2, 0x64, 0xA3 },
	//DatalogClear2Cmd,
	{ 0x64, 0x49, 2, 0x00, 0x00 },
	//DatalogRead2Cmd,
	{ 0x64, 0x4A, 6, 0x00, 0x00 },
  //QueryIfreeModeCmd
	{ 0x64, 0x6B, 2, 0x64, 0xF2 },
  //QuerySecurityTagCmd
	{ 0x64, 0x6C, 31, 0x64, 0xF2 },
  //QueryPhoenixSoftwareFeatureCmd
	{ 0x64, 0x6D, 2, 0x64, 0xF3 },
  //QueryExtendedIdCmd
	{ 0x64, 0x6E, 2, 0x64, 0xF4 },
  //QueryOneRfRegisterCmd
	{ 0x64, 0x70, 3, 0x64, 0xF5 },
  //QueryV9TagAddressCmd
	{ 0x64, 0x71, 2, 0x64, 0xF6 },
  //QueryV9TagCmd
	{ 0x64, 0x72, 2, 0x64, 0xF7 },
  //QueryRfIcCmd
	{ 0x64, 0x74, 3, 0x64, 0xF8 },
  //QueryAlphaUniqueIdCmd
	{ 0x64, 0x76, 2, 0x64, 0xF9 },
	//QueryCpuBoostModeCmd,
	{ 0x64, 0x78, 2, 0x64, 0xFA },
	//HostDataDumpOnCmd,
	{ 0x64, 0x7A, 4, 0x00, 0x00 },
  	//QuerySha1StringCmd
	{ 0x64, 0x7E, 2, 0x64, 0xFF },
	//QueryVersionExtensionCmd
	{ 0x64, 0x7D, 2, 0x64, 0xFE },
	//Query1ppsPulseWidthCmd,
	{ 0x65, 0x02, 2, 0x65, 0x80 },
	//Query1ppsFreqencyOutputCmd,
	{ 0x65, 0x04, 2, 0x65, 0x81 },
	//QueryGnssKnumberSlotCnr2Cmd,
	{ 0x66, 0x7F, 2, 0x66, 0xFF },
	//GetBdAlmanacCmd,
	{ 0x67, 0x04, 3, 0x67, 0x81 },
	//QueryDgpsCmd,
	{ 0x69, 0x02, 2, 0x69, 0x80 },
	//QuerySmoothModeCmd,
	{ 0x69, 0x04, 2, 0x69, 0x81 },
	//QueryRtkModeCmd,
	{ 0x6A, 0x02, 2, 0x6A, 0x80 },
	//QueryRtkParametersCmd,
	{ 0x6A, 0x04, 2, 0x6A, 0x81 },
	//QueryRtkModeCmd2,
	{ 0x6A, 0x07, 2, 0x6A, 0x83 },
	//QueryRtkSlaveBaudCmd,
	{ 0x6A, 0x0D, 2, 0x6A, 0x85 },
	//QueryRtkCpifBiasCmd,
	{ 0x6A, 0x10, 2, 0x6A, 0x86 },
	//OnQueryRtkElevationAndCnrMaskCmd,
	{ 0x6A, 0x12, 2, 0x6A, 0x87 },
	//ClearRtkSlaveDataCmd,
	{ 0x6A, 0x0E, 2, 0x6A, 0x85 },
	//QueryRtkKinematicBaudCmd,
	{ 0x6A, 0x14, 2, 0x6A, 0x88 },
	//QueryDrRateCmd,
	{ 0x6C, 0x01, 2, 0x6C, 0x80 },
	//QueryDrRawRateCmd,
	{ 0x6C, 0x05, 2, 0x6C, 0x81 },
	//QueryAdrOdometerScalingFactorCmd
	{ 0x6C, 0x7C, 2, 0x6C, 0xFE },
	//QueryDrMemsNoiseLevelCmd
	{ 0x6C, 0x7E, 2, 0x6C, 0xFF },
	//QueryNavicMessageIntervalCmd,
	{ 0x6F, 0x02, 2, 0x6F, 0x80 },
  //QueryDrMultiHzCmd,
	//{ 0x6F, 0x02, 2, 0x6F, 0x80 },
	//QueryRegisterCmd,
	{ 0x71, 0xFF, 5, 0xC0, 0x00 },
  //ConfigureRegisterCmd
  { 0x72, 0xFF, 9, 0x00, 0x00 },
	//QueryRegisterCmd16,
	{ 0x73, 0xFF, 5, 0xC1, 0x00 },
	//InsdrAccelerometerSelfTestCmd,
	{ 0x79, 0x01, 2, 0x79, 0x80 },
	//InsdrGyroscopeSelfTestCmd,
	{ 0x79, 0x02, 2, 0x79, 0x81 },
	//InsdrAccumulateAngleStartCmd,
	{ 0x79, 0x03, 2, 0x79, 0x82 },
	//InsdrAccumulateAngleStopCmd,
	{ 0x79, 0x04, 2, 0x79, 0x86 },
	//QueryNmeaInterval2V8Cmd,
	{ 0x7A, 0x01, 3, 0x7A, 0x01 },
	//QueryDofunUniqueIdCmd,
	{ 0x7A, 0x02, 3, 0x7A, 0x02 },
	//QueryEricssonIntervalCmd,
	{ 0x7A, 0x04, 3, 0x7A, 0x04 },
	//QuerySerialNumberCmd,
	{ 0x7A, 0x05, 3, 0x7A, 0x05 },
	//QueryUartPassCmd,
	{ 0x7A, 0x08, 3, 0x7A, 0x08 },
  //QueryAlphaKeyCmd
	{ 0x7A, 0x08, 3, 0x7A, 0x08 },
	//ReadSup800UserDataCmd,
	{ 0x7A, 0x09, 8, 0x7A, 0x09 },
	//QueryPstmDeviceAddressCmd
	{ 0x7A, 0x0A, 3, 0x7A, 0x0A },
	//QueryPstnLatLonDigitsCmd
	{ 0x7A, 0x0A, 3, 0x7A, 0x0A },
  //EnterRtkDebugModeCmd
  { 0x7A, 0x0B, 5, 0x7A, 0x0B },
  //BackRtkDebugModeCmd
  { 0x7A, 0x0B, 5, 0x7A, 0x0B },
	//QueryDataLogStatusWatchCmd,
	{ 0x7A, 0X0C, 3, 0x7A, 0x0C },
	//QueryGpioDataCmd,
	{ 0x7A, 0X7F, 4, 0x7A, 0x7F },

	//QueryChannelDopplerCmd,
	{ 0x7B, 0xFF, 2, 0xFE, 0x00 },
	//QueryChannelClockOffsetCmd,
	{ 0x7C, 0xFF, 9, 0xFF, 0x00 },
	//QueryDrHwParameterCmd,
	//{ 0x7E, 0xFF, 1, 0xF1, 0x00 },
	//QueryDrInfoCmd,
	{ 0x7F, 0xFF, 1, 0xF0, 0x00 },
};

enum SqBinaryCmd
{
	QuerySwVerSysCmd = 0,
	QuerySwCrcSysCmd,
	QueryCustomerIDCmd,
	QueryPositionRateCmd,
	QueryPowerModeCmd,
	QueryDataLogStatusCmd,
  DatalogClearCmd,
  DatalogReadCmd,
	QueryBinaryMeasurementDataOutCmd,
	QueryRtcmMeasurementDataOutCmd,
  QueryBasePositionCmd,
	QueryDatumCmd,
	QueryDopMaskCmd,
	QueryElevationAndCnrMaskCmd,
	//QueryAgpsStatusCmd,
	QueryPositionPinningCmd,
	Query1ppsModeCmd,
	QueryNoisePowerCmd,
	QueryTimingCmd,
	QueryCableDelayCmd,
	QueryAntennaDetectionCmd,
	QueryProprietaryMessageCmd,
	QueryGnssNmeaTalkIdCmd,
  GetGpAlmanacCmd,
  GetPpsOutputModeCmd,
	QuerySbasCmd,
	QueryQzssCmd,
  QuerySbas2Cmd,
  QuerySbasDefaultCmd,
	QuerySagpsCmd,
	QueryGnssBootStatusCmd,
	QueryNmeaIntervalV8Cmd,
	QueryNmeaBinaryOutputDestinationCmd,
	QueryInterferenceDetectControlCmd,
	QueryNoisePowerControlCmd,
	QueryParameterSearchEngineNumberCmd,
	QueryV8PowerSaveParameters,
	QueryPositionFixNavigationMaskCmd,
	QueryRefTimeSyncToGpsTimeCmd,
	QueryNavigationModeV8Cmd,
	QueryGnssConstellationTypeCmd,
	QueryTimeStampingCmd,
	QueryGpsTimeCmd,
  QueryPstiCmd,
	QuerySearchEngineSleepCriteriaCmd,
	QueryDatumIndexCmd,
	QueryConstellationCapabilityCmd,
	QuerySignalDisturbanceStatusCmd,
	QuerySignalDisturbanceDataCmd,
	QueryGeofenceCmd,
	QueryGeofenceResultCmd,
	QueryGeofenceCmdEx,
	QueryGeofenceResultCmdEx,
	QueryVeryLowSpeedCmd,
  QueryNmeaStringCmd,
  QueryDataLogStatus2Cmd,
  DatalogClear2Cmd,
  DatalogRead2Cmd,
  QueryIfreeModeCmd,
  QuerySecurityTagCmd,
  QueryPhoenixSoftwareFeatureCmd,
  QueryExtendedIdCmd,
  QueryOneRfRegisterCmd,
  QueryV9TagAddressCmd,
  QueryV9TagCmd,
  QueryRfIcCmd,
  QueryAlphaUniqueIdCmd,
  QueryCpuBoostModeCmd,
  HostDataDumpOnCmd,
	QuerySha1StringCmd,
	QueryVersionExtensionCmd,
  Query1ppsPulseWidthCmd,
	Query1ppsFreqencyOutputCmd,
	QueryGnssKnumberSlotCnr2Cmd,
  GetBdAlmanacCmd,
	QueryDgpsCmd,
	QuerySmoothModeCmd,
	QueryRtkModeCmd,
	QueryRtkParametersCmd,
	QueryRtkModeCmd2,
  QueryRtkSlaveBaudCmd,
  QueryRtkCpifBiasCmd,
  OnQueryRtkElevationAndCnrMaskCmd,
  ClearRtkSlaveDataCmd,
  QueryRtkKinematicBaudCmd,
  QueryDrRateCmd,
  QueryDrRawRateCmd,
  QueryAdrOdometerScalingFactorCmd,
  QueryDrMemsNoiseLevelCmd,
  QueryNavicMessageIntervalCmd,
	//QueryDrMultiHzCmd,
	QueryRegisterCmd,
  ConfigureRegisterCmd,
	QueryRegisterCmd16,
	InsdrAccelerometerSelfTestCmd,
	InsdrGyroscopeSelfTestCmd,
	InsdrAccumulateAngleStartCmd,
	InsdrAccumulateAngleStopCmd,
	QueryNmeaInterval2V8Cmd,
	QueryDofunUniqueIdCmd,
	QueryEricssonIntervalCmd,
	QuerySerialNumberCmd,
	QueryUartPassCmd,
  QueryAlphaKeyCmd,
	ReadSup800UserDataCmd,
	QueryPstmDeviceAddressCmd,
	QueryPstnLatLonDigitsCmd,
  EnterRtkDebugModeCmd,
  BackRtkDebugModeCmd,
  QueryDataLogStatusWatchCmd,
  QueryGpioDataCmd,
	QueryChannelDopplerCmd,
	QueryChannelClockOffsetCmd,
	//QueryDrHwParameterCmd,
	QueryDrInfoCmd,
};

bool CGPSDlg::SaveEphemeris(U08* buff, U08 id)
{	
	if(CalcStqMsgChecksum(buff) == id)
	{
		int len = buff[3] - 1;
		m_ephmsFile.Write(&buff[5], len);     
		return true;
	}
	return false;
}

bool CGPSDlg::SaveEphemeris2(U08* buff, WORD id)
{	
	if(CalCheckSum2(buff) == id)
	{	
		int len = buff[3] - 2;	//id and sub id are two bytes.
		m_ephmsFile.Write(&buff[6], len);     
		return true;
	}
	return false;
}

CGPSDlg::CmdErrorCode CGPSDlg::GetCommandReturnType(U08* buff, int tail, bool showMsg)
{	
	if(buff[0] == 0xA0 && buff[1] == 0xA1 &&
		(buff[tail-1] == 0x0D) && (buff[tail] == 0x0A) && (buff[4] == 0x83) && (buff[5] == 0x00))
	{ //ACK0 (V6 Firmware will return this before ACK) - 0xa0, 0xa1, l1, l2, 0x83, 0x00, ... ,0x0d, 0x0a
		return Ack0;
	}
	if((buff[0] == 0xA0) && (buff[1] == 0xA1) &&
		(buff[tail-1] == 0x0D) && (buff[tail] == 0x0A) && (buff[4] == 0x83))
	{ //ACK - 0xa0, 0xa1, l1, l2, 0x83, 0xcommandId, ... ,0x0d, 0x0a
		return Ack;
	}
	if((buff[0] == 0xA0) && (buff[1] == 0xA1) &&
		(buff[tail-1] == 0x0D) && (buff[tail] == 0x0A) && (buff[4] == 0x84))
	{ //NACK - 0xa0, 0xa1, l1, l2, 0x84, 0xcommandId, ... ,0x0d, 0x0a
		if(showMsg)
		{
			add_msgtolist("Received NACK...");
		}
		return NACK;
	}
	if((buff[0] == 0xA0) && (buff[1] == 0xA1) &&
		(buff[tail-1] == 0x0D) && (buff[tail] == 0x0A) && (buff[4] == 0x85))
	{ //FormatError (V8 FW after 2016 will return this when command format error
    //- 0xa0, 0xa1, l1, l2, 0x85, 0xcommandId, ... ,0x0d, 0x0a
		if(showMsg)
		{
			add_msgtolist("Received FormatError...");
		}
		return FormatError;
	}
	return Timeout;
}

/*
CGPSDlg::CmdErrorCode CGPSDlg::GetCommandReturnTypeWithoutMsg(U08* buff, int tail, bool showMsg)
{	
	if(buff[0]==0xa0 && buff[1]==0xa1 &&
		(buff[tail-1]==0x0d) && (buff[tail]==0x0a) && (buff[4]==0x83) && (buff[5]==0x0))
	{ //ACK0 (V6 Firmware will return this before ACK) - 0xa0, 0xa1, l1, l2, 0x83, 0x00, ... ,0x0d, 0x0a
		return Ack0;
	}
	if((buff[0]==0xa0) && (buff[1]==0xa1) &&
		(buff[tail-1]==0x0d) && (buff[tail]==0x0a) && (buff[4]==0x83))
	{ //ACK - 0xa0, 0xa1, l1, l2, 0x83, 0xcommandId, ... ,0x0d, 0x0a
		return Ack;
	}
	if((buff[0]==0xa0) && (buff[1]==0xa1) &&
		(buff[tail-1]==0x0d) && (buff[tail]==0x0a) && (buff[4]==0x84))
	{ //NACK - 0xa0, 0xa1, l1, l2, 0x84, 0xcommandId, ... ,0x0d, 0x0a
		return NACK;
	}
	if((buff[0]==0xa0) && (buff[1]==0xa1) &&
		(buff[tail-1]==0x0d) && (buff[tail]==0x0a) && (buff[4]==0x85))
	{ //FormatError (V8 FW after 2016 will return this when command format error
    //- 0xa0, 0xa1, l1, l2, 0x85, 0xcommandId, ... ,0x0d, 0x0a
		return FormatError;
	}
	return Timeout;
}
//*/

bool CGPSDlg::SendToTarget(U08* message, U16 length, LPCSTR promptMessage, int timeout)
{	
  //For old style, timeout is boolean type.
  if(timeout == 1)
  {
    timeout = 1000;
  }
  else if(timeout == 0)
  {
    timeout = 2000;
  }

	if(m_bShowBinaryCmdData && !m_autoAgpsSilentMode)
	{
		add_msgtolist("In: " + theApp.GetHexString(message, length));	
	}

	ClearQue();
  if(NULL == m_serial) 
  {
    return false;
  }
  else
  {
	  m_serial->SendData(message, length);
  }

	static U08 buffer[COM_BUFFER_SIZE] = { 0 };
	ScopeTimer t;
	while(NULL != m_serial)
	{	
    if((int)t.GetDuration() > timeout)
    {
      DWORD time = t.GetDuration();
      break;
    }
	  int retLen = m_serial->GetBinary(buffer, sizeof(buffer) - 1, timeout);
		if(!ReadOK(retLen))
		{
			Sleep(10);
			continue;
		}
		buffer[retLen] = 0;
    if(!PreprocessInputLine(buffer, retLen))
    {
      continue;
    }
	
		CmdErrorCode ack = GetCommandReturnType(buffer, retLen - 1, (promptMessage != NULL));	
		if(ack == Ack)
		{
			if(m_bShowBinaryCmdData && !m_autoAgpsSilentMode)
			{
				add_msgtolist("Ack: " + theApp.GetHexString(buffer, retLen));	
			}

			if(strlen(promptMessage) && !m_autoAgpsSilentMode)
			{
				add_msgtolist(promptMessage);	
			}
			return true;
		}
		else if(ack == NACK)
		{
			return false;
		}
		else if(ack == FormatError)
		{
			if(m_bShowBinaryCmdData && !m_autoAgpsSilentMode)
			{
				add_msgtolist("FormatError: " + theApp.GetHexString(buffer, retLen));	
			}
			ShowFormatError(message, buffer);
			return false;
		}
	}	//while(1)	
  return false;
}

bool CGPSDlg::SendToTargetAndCheckAck(U08* message, U16 length, LPCSTR promptMessage, int timeout, int idLen)
{	
  //For old style, timeout is boolean type.
  if(timeout == 1)
  {
    timeout = 1000;
  }
  else if(timeout == 0)
  {
    timeout = 2000;
  }

	if(m_bShowBinaryCmdData)
	{
		add_msgtolist("In: " + theApp.GetHexString(message, length));	
	}

	ClearQue();
  if(NULL == m_serial) 
  {
    return false;
  }
  else
  {
	  m_serial->SendData(message, length);
    //For test
    //m_serial->SendData(message, length - 3);
    //Sleep(100);
	  //m_serial->SendData(message + 8, 3);
  }

	static U08 buffer[COM_BUFFER_SIZE] = { 0 };
	ScopeTimer t;
	while(NULL != m_serial)
	{	
#if !defined(_DEBUG) || !defined(NO_TIMEOUT)
    if((int)t.GetDuration() > timeout)
    {
      DWORD time = t.GetDuration();
      break;
    }
#endif
	  int retLen = m_serial->GetBinary(buffer, sizeof(buffer) - 1, timeout);
		if(!ReadOK(retLen))
		{
			Sleep(10);
			continue;
		}
		buffer[retLen] = 0;
    if(!PreprocessInputLine(buffer, retLen))
    {
      continue;
    }
	
		CmdErrorCode ack = GetCommandReturnType(buffer, retLen - 1, false);	
		if(ack == Ack)
		{
			if(m_bShowBinaryCmdData)
			{
				add_msgtolist("Ack: " + theApp.GetHexString(buffer, retLen));	
			}

			if(strlen(promptMessage))
			{
				add_msgtolist(promptMessage);	
			}
			return true;
		}
		else if(ack == NACK)
		{
      if(retLen != 8 + idLen)
      { //Not a correct NACK
        continue;
      }
		  if((promptMessage != NULL))
		  {
			  add_msgtolist("Received NACK...");
		  }
			return false;
		}
		else if(ack == FormatError)
		{
		  if((promptMessage != NULL))
		  {
			  add_msgtolist("Received FormatError...");
		  }

			if(m_bShowBinaryCmdData)
			{
				add_msgtolist("FormatError: " + theApp.GetHexString(buffer, retLen));	
			}
			ShowFormatError(message, buffer);
			return false;
		}
	}	//while(1)	
  return false;
}

bool CGPSDlg::SendToTargetNoAck(U08* message, U16 length)
{		
	if(NULL == m_serial) return false;
	ClearQue();
	m_serial->SendData(message, length, true);	
	return true;
}

bool CGPSDlg::SendToTargetNoWait(U08* message, U16 length, LPCSTR Msg)
{		
	time_t start,end;
	start = clock();		    
	if(m_bShowBinaryCmdData)
	{
		add_msgtolist("In: " + theApp.GetHexString(message, length));	
	}

	if(NULL == m_serial) return false;
	ClearQue();
	m_serial->SendData(message, length);	
	while(1)
	{	
		U08 buff[1024] = {0};

		if(NULL == m_serial) return false;
		m_serial->GetBinary(buff, sizeof(buff));

		U08 len = buff[2] << 8 | buff[3];
		int k1 = len + 5;
		int k2 = len + 6;		
		CmdErrorCode ack = GetCommandReturnType(buff, k2);		
		if(ack == Ack)
		{
			if(m_bShowBinaryCmdData)
			{
				add_msgtolist("Ack: " + theApp.GetHexString(buff, k2+1));	
			}

			if(strlen(Msg))
			{
				add_msgtolist(Msg);	
			}
			return true;
		}
		else if(ack == NACK)
		{
			return false;
		}
		
		end = clock();
		if(TIMEOUT_METHOD(start, end))
		{			
			return false;
		}	
	}
}

bool CGPSDlg::CheckGPS(U08* message, U16 length, char* Msg)
{	
	if(NULL == m_serial) return false;
	m_serial->SendData(message, length, true);	  
	time_t start,end;
	start = clock();		    
	while(1)
	{		
		U08 buff[1024] = {0};
		if(NULL == m_serial) return false;
		m_serial->GetBinary(buff, sizeof(buff), SCAN_TIME_OUT_MS);			
		U08 len = buff[2] << 8 | buff[3];
		int k1 = len + 5;
		int k2 = len + 6;			
		if(GetCommandReturnType(buff, k2) != Timeout)
		{
			if(Msg[0] != 0)
			{
				add_msgtolist(Msg);	
			}		
			return true;
		}
		end = clock();
		if((end-start) > SCAN_TIME_OUT_MS)
		{				
			return false;
		}
	}
}

UINT LogReadBatchControlThread(LPVOID pParam)
{
  CGPSDlg::DataLogCommandSet cmdSet = (CGPSDlg::DataLogCommandSet)(int)pParam;
	CGPSDlg::gpsDlg->LogReadBatchControl(cmdSet);
	return 0;
}

void CGPSDlg::DoDatalogLogReadBatch(DataLogCommandSet cmdSet)
{
	if(!CheckConnect())
	{
		return;
	}
	SetInputMode(NoOutputMode);
	CTime t = CTime::GetCurrentTime();
	CString fileName;	
	fileName.Format("DataLog%02d-%02d-%02d_%02d%02d%02d.log", t.GetYear(), t.GetMonth(), t.GetDay(),
		t.GetHour(), t.GetMinute(), t.GetSecond());

	CFileDialog dlgFile(FALSE, _T("log"), fileName, OFN_HIDEREADONLY, _T("ALL Files (*.*)|*.*||"), this);
	INT_PTR nResult = dlgFile.DoModal();
	fileName = dlgFile.GetPathName();			

	//CFileException ef;
	try
	{
		if(nResult == IDOK)
		{  				
			datalogFilename = fileName;
			this->m_nDownloadBaudIdx = g_setting.boostBaudIndex;
			::AfxBeginThread(LogReadBatchControlThread, (LPVOID)(int)cmdSet);
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
}

void CGPSDlg::OnDatalogLogReadBatch()
{
  DoDatalogLogReadBatch(DataLogCommandSet1);
}

void CGPSDlg::OnDatalogLogReadBatch2()
{
  DoDatalogLogReadBatch(DataLogCommandSet2);
}

bool CGPSDlg::DataLogReadOffsetCtrl(DataLogCommandSet cmdSet, int startId, int totalSector, int offset, U08 *buffer, long bufferSize, long *receiveBytes)
{
	g_waitReadDialog = new CWaitReadLog;
	AfxBeginThread(WaitLogRead, 0);	
	WaitForSingleObject(waitlog, INFINITE);

	memset(buffer, 0, bufferSize);
	for(int i = startId; i < totalSector; i += offset)
	{
		int step = ((i + offset) > totalSector) ? (totalSector - i) : offset;
		int tryCount = 0;
		while(1)
		{
			++tryCount;
			if(tryCount > 60)
			{
				return false;
			}			
			
			long tmp_count = 0;
			if(DatalogReadAll(cmdSet, i, step, &buffer[*receiveBytes], step * 4096, &tmp_count))
			{
				*receiveBytes += tmp_count;
				CString displayMessage;
				displayMessage.Format("Retrieve Log data #%d sector #%d bytes\n", i, *receiveBytes);
				g_waitReadDialog->msg.SetWindowText(displayMessage);
				Sleep(150);
				break;
			}
			else
			{
				Utility::LogFatal(__FUNCTION__, "[DataLog] DatalogReadAll fail", __LINE__);
				Sleep(150);
			}
		}
	}
	VerifyDataLogFormat(buffer, receiveBytes);
	return true;
}

bool CGPSDlg::DatalogReadAll(DataLogCommandSet cmdSet, int startId, int offset, U08 *datalog, long size, long *receiveBytes)
{
  U16 cmdSize = cmdTable[((cmdSet == DataLogCommandSet1) ? DatalogReadCmd : DatalogRead2Cmd)].cmdSize;
  BinaryCommand cmd(cmdSize);
  int cmdIdx = 0;
  if(cmdSet == DataLogCommandSet1)
  {
    cmd.SetU08(++cmdIdx, cmdTable[DatalogReadCmd].cmdId);
  }
  else
  {
    cmd.SetU08(++cmdIdx, cmdTable[DatalogRead2Cmd].cmdId);
    cmd.SetU08(++cmdIdx, cmdTable[DatalogRead2Cmd].cmdSubId);
  }

  cmd.SetU08(++cmdIdx, startId >> 8);
  cmd.SetU08(++cmdIdx, startId & 0xFF);
  cmd.SetU08(++cmdIdx, offset >> 8);
  cmd.SetU08(++cmdIdx, offset & 0xFF);

  bool ret = ExecuteConfigureCommand(cmd.GetBuffer(), cmd.Size(), "", false);
  if(!ret)
  {
    return false;
  }
	//U08 cmd[5] = { 0x1D, startId >> 8, startId & 0xFF, offset >> 8, offset & 0xFF };
	//U08 message[12] = { 0 };
	//int len = SetMessage2(message, cmd, sizeof(cmd));
	//SendToTarget(message, len, "", true);

	U08 tmpPtr = 0;
	bool finished = false;
	DWORD revCount = 0;
	*receiveBytes = 0;
	U08 revBuff[0x1000] = { 0 };
	while(m_serial != NULL)
	{		
		U08 buff[0x2000] = { 0 };
		DWORD readBytes = m_serial->ReadData(buff, sizeof(buff), true);
		if(readBytes == 0)
		{
			continue;
		}

		if(VerifyDataLogBuffer(buff, datalog, &tmpPtr, readBytes, receiveBytes))
		{
			if(tmpPtr > 0)
			{
				memcpy(revBuff, buff, tmpPtr);
			}
			finished = true;
			break;
		}

		if(*receiveBytes > size + 10)
		{
		  return false;
		}
		memcpy(&datalog[*receiveBytes], buff, readBytes);
		*receiveBytes += readBytes;
	}
	if(NULL == m_serial)
  {
    return false;
  }

	revCount = tmpPtr;
	int count = 0;
	//bool myChkSum = false;
	while(1)
	{
		if(revCount > 0)
		{
			if(!strncmp((char*)revBuff, "CHECKSUM=", 9) && revCount >= 12)
			{
				U08 chk = (U08)revBuff[9];
				U16 id;
				if(revBuff[10] == 0x0A && revBuff[11] == 0x0D)
					id = startId;	
				else
					id = (U08)revBuff[11] << 8 | (U08)revBuff[10];

				U08 chkSum = 0;
				for(int i=0; i < *receiveBytes; ++i)
        {
					chkSum = datalog[i] ^ chkSum;
        }

				//TRACE("chk=%d,chk_sum=%d,*receiveBytes=%d\n", chk, chk_sum, *receiveBytes);
				if(chk == chkSum && id == startId)
				{
					return true;
				}
			  return false;
			}
			Utility::LogFatal(__FUNCTION__, "[DataLog] CHECKSUM fail", __LINE__);
		}

		U08 buff[1024] = { 0 };
		if(NULL == m_serial) 
    {
      return false;
    }

		DWORD readBytes = m_serial->ReadData(&revBuff[revCount], sizeof(buff), true);
		revCount += readBytes;
		count++;
		if(count > 10000)
		{
			return false;
		}
	} //while(1)
	return false;
}
/*
BOOL CGPSDlg::OpenDataLogFile(UINT nOpenFlags)
{
	return dataLogFile.Open(datalogFilename, nOpenFlags);
}
*/
void CGPSDlg::LogReadBatchControl(DataLogCommandSet cmdSet)
{
	if(!dataLogFile.Open(datalogFilename, CFile::modeReadWrite | CFile::modeCreate))
	{
		return;
	}

  BinaryData ackCmd;
  CmdErrorCode err = (cmdSet == DataLogCommandSet1) 
    ? QueryDataLogStatus(CGPSDlg::Return, &ackCmd)
    : QueryDataLogStatus2(CGPSDlg::Return, &ackCmd);
  if(CGPSDlg::Ack != err)
  {
    dataLogFile.Close();
    ::AfxMessageBox("No response from device!");
		return;
  }

  U16 left = ConvertLeU16(ackCmd.Ptr((cmdSet == DataLogCommandSet1) ? 9 : 10));
  U16 totalBytes = ConvertLeU16(ackCmd.Ptr((cmdSet == DataLogCommandSet1) ? 11 : 12));
  U16 endPos = totalBytes - left + ((left == 0) ? 0 : 1);

	BoostBaudrate(FALSE);
	long bufferSize = (endPos + 1) * 0x1000;
	U08* buffer = (U08*)malloc(bufferSize);
	long receiveBytes = 0;

	if(DataLogReadOffsetCtrl(cmdSet, 0, endPos, 2, buffer, bufferSize, &receiveBytes))
	{
		dataLogFile.Write(buffer, receiveBytes);
		g_waitReadDialog->msg.SetWindowText("Data Log reading complete.");
		CGPSDlg::gpsDlg->add_msgtolist("Data Log reading successfully");
	}
	else
	{
		g_waitReadDialog->msg.SetWindowText("Data Log reading fails!");
		CGPSDlg::gpsDlg->add_msgtolist("Data Log reading fails!");
	}
	g_waitReadDialog->IsFinish = true;
	free(buffer);
	
	CGPSDlg::gpsDlg->target_only_restart(0);
	if(NULL == m_serial) return;
	CGPSDlg::gpsDlg->m_serial->ResetPort(g_setting.GetBaudrateIndex());
	CGPSDlg::gpsDlg->m_BaudRateCombo.SetCurSel(g_setting.GetBaudrateIndex());
	dataLogFile.Close();
}

//DWORD ReadDataLogSector(CSerial* serial, void* buffer, DWORD bufferSize)
//{
//	DWORD totalSize = 0;
//	char* bufferIter = (char*)buffer;
//	DWORD nBytesRead = 0;
//	do
//	{ 
//		nBytesRead = serial->ReadData(bufferIter, 1);
//		if(nBytesRead <= 0)
//		{
//			return totalSize;
//		}
//
//		if((*bufferIter == 0x0a) && (*(bufferIter - 1) == 0x0d))
//		{	//When read 0d, 0a, check pack size to make sure until tail.
//			DWORD len = *(bufferIter - totalSize + 3);
//			if(totalSize == len + 6)
//			{
//				break;
//			}
//		}
//		bufferIter += nBytesRead;
//		totalSize += nBytesRead;
//	}
//	while(totalSize < bufferSize);
//	return totalSize;	
//}

void CGPSDlg::VerifyDataLogFormat(U08 *datalog, long *size)
{
	long count = 0;
	U08* buff = new U08[*size];
	for(long i = 0; i < *size; i += 0x1000)
	{
		U08 *bufferIter = &datalog[i];
		long tmp_count = 0;
		while(1)
		{
			U08 type = bufferIter[0] & 0xF0;
			if(type == 0x40 || type == 0x60)
			{
				bufferIter += 18;
				tmp_count += 18;
			}
			else if(type == 0x80)
			{
				bufferIter += 8;
				tmp_count += 8;
			}
			else if(type == 0x20 || type == 0xC0)
			{
				bufferIter += 20;
				tmp_count += 20;
			}
			else if(type == 0x50 || type == 0x70)
			{
				bufferIter += 20;
				tmp_count += 20;
			}
			else if(type == 0x90)
			{
				bufferIter += 10;
				tmp_count += 10;
			}
			else if(type == 0x30 || type == 0xD0)
			{
				bufferIter += 22;
				tmp_count += 22;
			}
			//else if(type == 0xF0)
			//{
			//	bufferIter += 2;
			//	tmp_count += 2;
			//}
			else
			{
				break;
			}
		} //while(1)
		memcpy(&buff[count], &datalog[i], tmp_count);
		count += tmp_count;
	}
	memcpy(datalog, buff, count);
	*size = count;
	delete [] buff;
}


bool CGPSDlg::VerifyDataLogBuffer(U08 *buff, U08 *datalog, U08 *ptr_last, int size, long *receive_count)
{
	bool isEnd = false;
	for(int i=0; i<size; ++i)
	{
		if(i < size - 3 && 
			buff[i] == 'E' && buff[i+1] == 'N' && buff[i+2] == 'D' && buff[i+3] == '\0')
		{
			isEnd = true;
			memcpy(&datalog[*receive_count], buff, i);
			*receive_count += i;
			if(size > i + 3)
			{
				memcpy(buff, &buff[i+4], size - (i+4));
				*ptr_last = size - (i + 4);
			}
			Utility::Log(__FUNCTION__, "[DataLog] VerifyDataLogBuffer type 1", __LINE__);
			break;
		}
		else if(i < size - 2 && *receive_count >= 1 && 
			datalog[*receive_count-1] == 'E' && buff[i] == 'N' && buff[i+1] == 'D' && buff[i+2] == '\0')
		{
			isEnd = true;
			*receive_count -= 1;
			if(size > i + 3)
			{
				memcpy(buff, &buff[i+3], size - (i + 3));
				*ptr_last = size - (i + 3);
			}
			Utility::Log(__FUNCTION__, "[DataLog] VerifyDataLogBuffer type 2", __LINE__);
			break;					
		}
		else if(i < size - 1 && *receive_count >= 2 && 
			datalog[*receive_count-2] == 'E' && datalog[*receive_count-1] == 'N' && buff[i] == 'D' && buff[i+1] == '\0')
		{
			isEnd = true;
			*receive_count -= 2;
			if(size > i + 2)
			{
				memcpy(buff, &buff[i+2], size - (i + 2));
				*ptr_last = size - (i + 2);
			}
			Utility::Log(__FUNCTION__, "[DataLog] VerifyDataLogBuffer type 3", __LINE__);
			break;					
		}
		else if(i < size && *receive_count >= 3 && 
			datalog[*receive_count-3] == 'E' && datalog[*receive_count-2] == 'N' && datalog[*receive_count-1] == 'D' && buff[i] == '\0')
		{
			isEnd = true;
			*receive_count -= 3;
			if(size > i + 1)
			{
				memcpy(buff, &buff[i+1], size - (i + 1));
				*ptr_last = size - (i + 1);
			}
			Utility::Log(__FUNCTION__, "[DataLog] VerifyDataLogBuffer type 4", __LINE__);
			break;					
		}
	}
	return isEnd;
}

inline const char *go_next_dot(const char *buff)
{
	while(1)
	{
		if(*buff == 0 || *buff=='*') return NULL;
		if(*buff == ',') return buff+1;
		buff++;
	}
}

void CGPSDlg::parse_sti_20_message(const char *buff,int len) // for timing module
{
	const char *ptr = buff;
	CString temp;

	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;
  //Field 1
	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;
  //Field 2
	temp.Format("%d",atoi(ptr));
	GetDlgItem(IDC_GYRO_DATA)->SetWindowText(temp);
	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;
  //Field 3
	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;
  //Field 4
	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;
  //Field 5
	temp.Format("%d",atoi(ptr));
	m_odo_meter.SetWindowText(temp);
	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;
  //Field 6
	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;
  //Field 7
	temp.Format("%d",atoi(ptr));
	m_backward_indicator.SetWindowText(temp);
	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;
  //Field 8
	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;
  //Field 9
  ptr = go_next_dot(ptr);
	if(ptr == NULL) return;
  //Field 10
	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;
  //Field 11
	//temp.Format("%.2f", atof(ptr));
	//GetDlgItem(IDC_GYRO_DATA)->SetWindowText(temp);
}

void CGPSDlg::parse_sti_03_message(const char *buff,int len) // for timing module
{
	const char *ptr = buff;

	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;

	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;

	CString temp;
	temp.Format("%d",atoi(ptr));
	m_odo_meter.SetWindowText(temp);
}

#if (_SHOW_RTK_BASELINE_ || _TAB_LAYOUT_)
void CGPSDlg::parse_sti_31_message(const char *buff,int len) // for timing module
{
	const char *ptr = buff;

	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;
	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;

	CString temp;
	temp.Format("%.3f",atof(ptr));
#if (_SHOW_RTK_BASELINE_)
	m_bootStatus.SetWindowText(temp);
#else
	m_baselineLength.SetWindowText(temp);
#endif
}
#endif

#if(_MORE_INFO_ || _TAB_LAYOUT_)
void CGPSDlg::parse_sti_30_message(const char *buff,int len) // for timing module
{
	const char *ptr = buff;
	memset(&m_psti030, 0, sizeof(&m_psti030));

	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;
	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;
	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;
	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;
	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;
	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;
	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;
	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;
	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;
	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;
	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;
	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;
	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;
	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;
	m_psti030.rtkAge = (F32)atof(ptr);

	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;
	m_psti030.rtkRatio = (F32)atof(ptr);

	PostMessage(UWM_UPDATE_PSTI030, (WPARAM)&m_psti030, 0);
}
#endif
#if(_TAB_LAYOUT_)
void CGPSDlg::parse_sti_32_message(LPCSTR buff, int len) // for timing module
{
	const char *ptr = buff;
	memset(&m_psti032, 0, sizeof(&m_psti032));

	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;
	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;
	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;
	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;
	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;
	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;
	m_psti032.eastProjection = (F32)atof(ptr);

	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;
	m_psti032.northProjection = (F32)atof(ptr);

	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;
	m_psti032.upProjection = (F32)atof(ptr);

	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;
	m_psti032.baselineLength = (F32)atof(ptr);

	ptr = go_next_dot(ptr);
	m_psti032.baselineCourse = (F32)atof(ptr);

	PostMessage(UWM_UPDATE_PSTI032, (WPARAM)&m_psti032, 0);
}

void CGPSDlg::parse_sti_33_message(LPCSTR buff, int len) // for timing module
{
	const char *ptr = buff;
	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;
	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;
	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;
	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;
	int version = atoi(ptr);

	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;
	U08 r = ptr[0];

	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;
	int cycleSilped = atoi(ptr);

  if(r == 'R')
  {
	  memset(&m_psti033R, 0, sizeof(&m_psti033R));
	  m_psti033R.version = version;
	  m_psti033R.receiver = r;
	  m_psti033R.numCycleSlippedTotal = cycleSilped;  
	  PostMessage(UWM_UPDATE_PSTI033, (WPARAM)&m_psti033R, 0);
  }
  else
  {
	  memset(&m_psti033B, 0, sizeof(&m_psti033B));
	  m_psti033B.version = version;
	  m_psti033B.receiver = r;
	  m_psti033B.numCycleSlippedTotal = cycleSilped;  
	  PostMessage(UWM_UPDATE_PSTI033, (WPARAM)&m_psti033B, 1);
  }
}
#endif

#if(MODULE_SUP_800)
void CGPSDlg::parse_sti_04_001_message(const char *buff,int len) // for timing module
{
	const char *ptr = buff;

	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;

	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;

	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;

	this->m_psti004001Copy.Valide = atoi(ptr);

	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;

	this->m_psti004001Copy.Pitch = atof(ptr);

	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;

	this->m_psti004001Copy.Roll = atof(ptr);

	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;

	this->m_psti004001Copy.Yaw = atof(ptr);

	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;

	this->m_psti004001Copy.Pressure = atoi(ptr);

	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;

	this->m_psti004001Copy.Temperature = atof(ptr);
}
#endif

void CGPSDlg::parse_sti_message(const char *buff,int len)
{
	const char *ptr = buff;
	int psti_id;

	ptr = go_next_dot(ptr);
	if(ptr == NULL) 
		return;

	psti_id = atoi(ptr);
	if(psti_id == 50)  // glonass k-number
	{
		parse_psti_50(ptr);
	} 
	else if(psti_id == 0)
	{
		parse_sti_0_message(buff, len);
	}
	else if(psti_id == 20)		// for dr
	{
		parse_sti_20_message(buff, len);
	}
	else if(psti_id == 3)		// for jamming interference
	{
		parse_sti_03_message(buff, len);
	}
#if (_SHOW_RTK_BASELINE_ || _TAB_LAYOUT_)
	else if(psti_id == 31)		// for jamming interference
	{
		parse_sti_31_message(buff, len);
	}
#endif
#if (_MORE_INFO_ || _TAB_LAYOUT_)
	else if(psti_id == 30)		// for jamming interference
	{
		parse_sti_30_message(buff, len);
	}
#endif
#if (_TAB_LAYOUT_)
	else if(psti_id == 32)	
	{
		parse_sti_32_message(buff, len);
	}
	else if(psti_id == 33)	
	{
		parse_sti_33_message(buff, len);
	}
#endif
#if(MODULE_SUP_800)
	else if(psti_id == 4)		// for SUP800
	{
		ptr = go_next_dot(ptr);
		int psti_sub_id = atoi(ptr);

		if(psti_sub_id==1)
		{
			parse_sti_04_001_message(buff,len);
		}
	}
#endif
}

void CGPSDlg::parse_pirnsf_message(const char *buff, int len)
{
  //For pirnsf debug only, Alex 20191226
#if (0)
	const char *ptr = buff;
	U32 svId, subId, msgId, serID, crc24;
  U08 subFrame[36] = {0};

  char tmp[3] = {0};
  int error = 1;
  while(1)
  {
	  ptr = go_next_dot(ptr);
	  if(ptr == NULL) break;
	  svId = atoi(ptr);

	  ptr = go_next_dot(ptr);
	  if(ptr == NULL) break;
    subId = atoi(ptr);

    int i = 0;
    for(i = 0; i < 36; ++i)
    {
	    ptr = go_next_dot(ptr);
	    if(ptr == NULL) break;
      tmp[0] = ptr[0];
      tmp[1] = ptr[1];
	    subFrame[i] = (U08)ConvertCharToU32(tmp);
    }

    if(i < 36)
    {
      error = 2;     
      break;
    }
    msgId = (subFrame[3] & 0x03) << 4 | (subFrame[4] >> 4);
    serID = subFrame[4] & 0x0F;

    U32 dws_ary[9] = {0};
    for(i = 0; i < 9; ++i)
    {
	    dws_ary[i] = 
        subFrame[i * 4] << 24 | subFrame[i * 4 + 1] << 16 |
        subFrame[i * 4 + 2] << 8 | subFrame[i * 4 + 3];
    }

    U32 crc = CRC24Q_calc(dws_ary, 286) & 0x00FFFFFFUL;
    crc24 = crc;
    if(crc != 0)
    {
      error = 3;
      break;
    }
    error = 0;
    break;
  }

  CString strMsg, txt(buff);
  txt = txt.Left(len);
  strMsg.Format("%d,%d,%d,\"%02X\",\"%06X\",%d,%d,\"%s\"", svId, subId, msgId, serID, crc24,(error==3) ? 1 : 0, ((subId == 3 || subId == 4) && (msgId == 20 || msgId == 21))? 1 : 0,txt);
  add_msgtolist(strMsg);
  /*
  txt = txt.Left(23);
  txt += "..*";
  txt += buff[len - 2];
  txt += buff[len - 1];
  if(error)
  {
    switch(error)
    {
    case 1:
      strMsg.Format("PIRNSF format error! %s", txt);
    case 2:
      strMsg.Format("PIRNSF length error! %s", txt);
    case 3:
      strMsg.Format("PIRNSF CRC24 error! %s,id=%d", txt, msgId);
    }
    add_msgtolist(strMsg);
    //...
  }
  if((subId == 3 || subId == 4) && (msgId == 20 || msgId == 21))
  {
    if(error == 0)
    {
    strMsg.Format(".INCOIS:%s,id=%d", txt, msgId);
    add_msgtolist(strMsg);
    }
    else
    {
    strMsg.Format("!INCOIS:%s,id=%d", txt, msgId);
    add_msgtolist(strMsg);
    }
  }
  */
#endif
}

#if(TIMING_MODE)
void CGPSDlg::parse_sti_0_message(const char *buff,int len) // for timing module
{
	int mode,survery_len;
	float error;
	int set_std,now_std;
	const char *ptr = buff;

	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;
	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;
	mode = atoi(ptr);
	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;
	survery_len = atoi(ptr);
	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;
	error = (float)atof(ptr);

	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;
	set_std = atoi(ptr);
	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;
	now_std = atoi(ptr);

	if(dia_monitor_1pps != NULL)
		dia_monitor_1pps->Show1PPSTiming(mode,survery_len,error,set_std,now_std);
}
#else
void CGPSDlg::parse_sti_0_message(const char *buff,int len) // for timing module
{
	int mode,survery_len;
	float error;
	const char *ptr = buff;


	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;
	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;
	mode = atoi(ptr);
	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;
	survery_len = atoi(ptr);
	ptr = go_next_dot(ptr);
	if(ptr == NULL) return;
	error = (float)atof(ptr);

	if(dia_monitor_1pps != NULL)
		dia_monitor_1pps->Show1PPSTiming(mode,survery_len,error);
}
#endif

void CGPSDlg::OnMonitoring1Pps()
{
	if(dia_monitor_1pps == NULL)
	{
		dia_monitor_1pps = new CMonitor_1PPS();
		dia_monitor_1pps->Create(IDD_1PPS_MONITOR);
		dia_monitor_1pps->ShowWindow(SW_SHOW);
	}
	else
	{
		dia_monitor_1pps->SetFocus();
	}
}

void CGPSDlg::close_minitor_1pps_window()
{
	dia_monitor_1pps->DestroyWindow();
	dia_monitor_1pps = NULL;
}

void CGPSDlg::parse_psti_50(const char *buff)		// gnss
{
	const char *ptr = buff;
	U08 psti_num, seq_num;
	U08 total_gnss;

	ptr = go_next_dot(ptr);

	psti_num = atoi(ptr);
	ptr = go_next_dot(ptr);
	seq_num = atoi(ptr);
	ptr = go_next_dot(ptr);
	total_gnss = atoi(ptr);
	ptr = go_next_dot(ptr);

	if(seq_num == 1)
	{
		memset(&m_gnssTemp,0,sizeof(GNSS_T));
	}

	while(ptr && *ptr != ',')
	{ 
		m_gnssTemp.sate[m_gnssTemp.gnss_in_view].k_num = atoi(ptr);
		ptr=go_next_dot(ptr);
		m_gnssTemp.sate[m_gnssTemp.gnss_in_view].slot_num = atoi(ptr);
		ptr=go_next_dot(ptr);
		m_gnssTemp.sate[m_gnssTemp.gnss_in_view].snr = atoi(ptr);
		ptr=go_next_dot(ptr);
		m_gnssTemp.gnss_in_view++;
	}

	if(seq_num == psti_num && m_gnssTemp.gnss_in_view == total_gnss)
	{
		memcpy(&m_gnss, &m_gnssTemp,sizeof(GNSS_T));
		memset(&m_gnssTemp,0,sizeof(GNSS_T));	
	}

	ptr = NULL;
}

void CGPSDlg::OnConfigurePositionUpdateRate()
{
	CPositionUpdateRateDlg dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnBnClickedECompassCalibration()
{
	if(!CheckConnect())
	{
		return;
	}

	ClearQue();

	U08 message[8];
	U08 msg[1];

	msg[0] = 0x54; //msgid
	int msg_len = SetMessage2(message, msg, sizeof(msg));

	int res = SendToTarget(message, msg_len, "E-Compass Calibration successfully", true);
	if(res)
	{
		GetDlgItem(IDC_ECOM_COUNTER)->SetWindowText("20");
		SetTimer(ECOM_CALIB_TIMER, 1000, NULL);
	}
	else
	{
		::AfxMessageBox("No E-Compass for calibration.");
	}
	CreateGPSThread();
}

void CGPSDlg::OnBinarySystemRestart()
{	
	if(!CheckConnect())
	{
		return;
	}

	SetInputMode(NoOutputMode);
	CSysRestartDlg dlg;
	dlg.ReStartType = 0;
	INT_PTR nResult = dlg.DoModal();	
	if(nResult == IDOK)
	{		
		m_ttffCount = 0;
		m_initTtff = false;
		SetTTFF(0);
		DeleteNmeaMemery();
	}
	else
	{
		SetMode();  
		CreateGPSThread();
	}
}

void CGPSDlg::OnBnClickedHotstart()
{
	if(!CheckConnect())
	{
		return;
	}

	SetInputMode(NoOutputMode);	
	m_ttffCount      = 0;	
	m_initTtff = false;
	if(IS_DEBUG == FALSE)
	{
		target_only_restart(1);
		//CreateGPSThread();
	}
	else
	{
		CSysRestartDlg* dlg = new CSysRestartDlg;
		dlg->ReStartType = 1;
		INT_PTR nResult = dlg->DoModal();	
		if(nResult == IDOK)
		{		
			SetTTFF(0);
		}
		else
		{
			SetMode();  
			Sleep(500);
			CreateGPSThread();
		}
		delete dlg;	dlg= NULL;
	}
	DeleteNmeaMemery();
	m_ttffCount      = 0;	
	m_initTtff = false;
	//m_ttff.SetWindowText("0");
	SetTTFF(0);

	ClearGlonass();
}

//warm start
void CGPSDlg::OnBnClickedWarmstart()
{
	if(!CheckConnect())
		return;
	SetInputMode(NoOutputMode);	

	m_ttffCount      = 0;	
	m_initTtff = false;
	if(IS_DEBUG == FALSE)
	{
		target_only_restart(2);
	}
	else
	{
		CSysRestartDlg* dlg = new CSysRestartDlg;
		dlg->ReStartType = 2;
		INT_PTR nResult = dlg->DoModal();	
		if(nResult == IDOK)
		{		
			SetTTFF(0);
		}
		else
		{
			SetMode();  
			Sleep(500);
			CreateGPSThread();
		}
		delete dlg;	dlg= NULL;	
	}
	DeleteNmeaMemery();
	ClearGlonass();
}

void CGPSDlg::target_only_restart(int mode)
{
	SendRestartCommand(mode);
	SetMode(); 
	CreateGPSThread();
}

void CGPSDlg::SendRestartCommand(int mode)
{
	SYSTEMTIME	now; 
	GetSystemTime(&now);

	U08 msg[15] = {0};
	msg[0] = 0x1; //msgid
	msg[1] = mode; //mode

	if(mode!=1 && mode!=5)	
	{
		msg[2] = HIBYTE(now.wYear);
		msg[3] = LOBYTE(now.wYear);	
		msg[4] = (U08)now.wMonth;
		msg[5] = (U08)now.wDay;
		msg[6] = (U08)now.wHour;
		msg[7] = (U08)now.wMinute;
		msg[8] = (U08)now.wSecond;
		
		if(mode!=4)
		{
			if(m_gpggaMsgBk.Latitude == 0.0)
			{
				m_gpggaMsgBk.Latitude = warmstart_latitude;
			}
			if(m_gpggaMsgBk.Longitude == 0.0)
			{
				m_gpggaMsgBk.Longitude = warmstart_longitude;
			}
			if(m_gpggaMsgBk.Altitude == 0.0)
			{
				m_gpggaMsgBk.Altitude = (F32)warmstart_altitude;
			}

			S16 lat = (S16)(m_gpggaMsgBk.Latitude / 100) * 100;
			lat += (S16)((m_gpggaMsgBk.Latitude - lat)* 100 / 60 + 0.5);
			if(m_gpggaMsgBk.Latitude_N_S == 'S')
			{
				lat *= -1;
			}
			msg[9] = HIBYTE(lat);
			msg[10]= LOBYTE(lat);	

			S16 lon = (S16)((m_gpggaMsgBk.Longitude / 100) * 100);
			lon += (S16)((m_gpggaMsgBk.Longitude - lon)* 100 / 60 + 0.5);
			if(m_gpggaMsgBk.Longitude_E_W == 'W')
			{
				lon *= -1;
			}
			msg[11]= HIBYTE(lon);
			msg[12]= LOBYTE(lon);

			msg[13]= HIBYTE((int)m_gpggaMsgBk.Altitude);
			msg[14]= LOBYTE((int)m_gpggaMsgBk.Altitude);
		}
	}

	int len = CGPSDlg::gpsDlg->SetMessage(msg,sizeof(msg));
	SendToTarget(CGPSDlg::m_inputMsg, len, "System Restart successfully", true);
}

void CGPSDlg::Restart(U08* messages, BOOL restoreConnection /* = TRUE */)
{
	m_CloseBtn.ShowWindow(0);
	//WaitEvent();
	ClearQue();
  bool bAck = false;
 //= (restoreConnection || m_autoAgpsSilentMode) 
 //   ? SendToTarget(messages, 22, "System Restart successfully", true)
 //   : SendToTargetNoAck(messages, 22);
  if(restoreConnection || m_autoAgpsSilentMode)
  {
    if(m_autoAgpsSilentMode)
    {
      m_autoAgpsSilentMode = FALSE;
      bAck = SendToTarget(messages, 22, "System Restart successfully", true);
      m_autoAgpsSilentMode = TRUE;
    }
    else
    {
      bAck = SendToTarget(messages, 22, "System Restart successfully", true);
    }
  }
  else
  {
    bAck = SendToTargetNoAck(messages, 22);
  }


  if(bAck)
  {
    DeleteNmeaMemery();
    ClearInformation();
    m_initTtff = false;
    m_ttffCount = 0;
    SetTTFF(0);
    ClearGlonass();
  }

  if(restoreConnection)
  {
	  SetMode(); 
	  CreateGPSThread();	
  }
	gpsSnrBar->Invalidate(FALSE);
	bdSnrBar->Invalidate(FALSE);
//#if(!SUPPORT_BDL2_GSV2)
//	gaSnrBar->Invalidate(FALSE);
//	giSnrBar->Invalidate(FALSE);
//#endif

	pic_earth->Invalidate(FALSE);
	pic_scatter->Invalidate(FALSE);
	
	m_CloseBtn.ShowWindow(1);
  PostMessage(UWM_TEST_XN112_START, (bAck) ? 1 : 0, 0);
}

UINT RestartThread(LPVOID pParam)
{
  U08 mode = CGPSDlg::gpsDlg->GetRestartMode();
  if(mode == 10)
  {
    BinaryData binData(15);
    *(binData.GetBuffer(0)) = 1;
    *(binData.GetBuffer(1)) = 3;
    BinaryCommand binCmd(binData);
    //Need set silent mode to get ACK
    CGPSDlg::gpsDlg->AutoAgpsSilentModeOn();
    CGPSDlg::gpsDlg->Restart(binCmd.GetBuffer(), false);
    if(!ENABLE_AUTO_AGPS) //Only force enable mode need to hide messages
    {
      CGPSDlg::gpsDlg->AutoAgpsSilentModeOff();
    }

    //Hide this warm start
    Sleep(500);
    CGPSDlg::gpsDlg->SendRestartCommand(2);
    //Hide all set ephemeris messages
    Sleep(200);
    CGPSDlg::gpsDlg->SetAgpsEphms(NULL, true);

    //CGPSDlg::gpsDlg->SendRestartCommand(1);
    CGPSDlg::gpsDlg->AutoAgpsSilentModeOff();
    return 0;
  }

  BinaryData binData(15);
  *(binData.GetBuffer(0)) = 1;
  *(binData.GetBuffer(1)) = mode;
  BinaryCommand binCmd(binData);
  CGPSDlg::gpsDlg->Restart(binCmd.GetBuffer());
	return 0;
}

void CGPSDlg::OnBnClickedColdstart()
{
#if defined(PRODUCTION_OLIVER20161128)
  OnBinaryConfigureClockOffset();
  return;
#elif defined(XN120_TESTER)
  DoXn120Tester();
  return;
#endif
	if(!CheckConnect())
	{
		return;
	}
	SetInputMode(NoOutputMode);
  if(g_setting.autoAgpsAfterColdStart || ENABLE_AUTO_AGPS)
  {
    m_restartMode = 10;
    CTimeSpan span = CTime::GetCurrentTime() - m_autoGi3EphTime;
    if(m_autoGi3Eph.Size() == 0 ||  span.GetMinutes() > 0)
    {
      AutoAgpsBackgroundDownload();
    }
  }
  else
  {
	  m_restartMode = 3;	
  }
	AfxBeginThread(RestartThread, 0);
}

UINT AFX_CDECL Configure1ppsPulseWidthThread(LPVOID param)
{
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(CGPSDlg::gpsDlg->m_inputMsg, 14, "Configure 1PPS Pulse Width successfully");
	return 0;
}

//void CGPSDlg::OnConfigure1PpsPulseWidth()
//{

	//if(!CheckConnect())
	//{
	//	return;
	//}

	//SetInputMode(NoOutputMode);
	//CConfig1ppsPulseWidthDlg dlg;
	//INT_PTR nResult = dlg.DoModal();
	//if(nResult == IDOK) 
	//{
 //   U08 msg[7] = {0};
	//	msg[0] = 0x65;
	//	msg[1] = 0x01;
	//	msg[2] = dlg.m_nPulseWidth >> 24 & 0xFF;
	//	msg[3] = dlg.m_nPulseWidth >> 16 & 0xFF;
	//	msg[4] = dlg.m_nPulseWidth >> 8 & 0xFF;
	//	msg[5] = dlg.m_nPulseWidth & 0xFF;
	//	msg[6] = (U08)dlg.m_nAttribute;

	//	int len = SetMessage(msg, sizeof(msg));
 //   AfxBeginThread(Configure1ppsPulseWidthThread, 0);
	//}
	//else
	//{
	//	SetMode();  
	//	CreateGPSThread();
	//}
//}

long int g_1ppsFrequencyOutput = 0;
U08 g_1ppsFrequencyOutputAttr = 0;
UINT Config1ppsFrequencyOutputThread(LPVOID param)
{
	U08 msg[7];

	msg[0] = 0x65;
	msg[1] = 0x03;
	msg[2] = (U08)HIBYTE(HIWORD(g_1ppsFrequencyOutput));
	msg[3] = (U08)LOBYTE(HIWORD(g_1ppsFrequencyOutput));
	msg[4] = (U08)HIBYTE(LOWORD(g_1ppsFrequencyOutput));
	msg[5] = (U08)LOBYTE(LOWORD(g_1ppsFrequencyOutput));
	msg[6] = g_1ppsFrequencyOutputAttr;

	int len = CGPSDlg::gpsDlg->SetMessage(msg, sizeof(msg));
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(CGPSDlg::m_inputMsg, len, "Configure 1PPS Frequency Output successfully");
	return 0;	
}

void CGPSDlg::OnConfig1ppsFrequencyOutput()
{
	CConfig1ppsFrequenceOutput dlg;

	if(!CheckConnect())	
	{
		return;
	}

	if(dlg.DoModal()!=IDOK)
	{
		SetMode();  
		CreateGPSThread();
		return;
	}

	g_1ppsFrequencyOutput = dlg.freqOutput;
	g_1ppsFrequencyOutputAttr = dlg.freqOutputAttr;
	AfxBeginThread(Config1ppsFrequencyOutputThread, 0);
}

void CGPSDlg::OnLineAssistance()
{
	if(!CheckConnect())	
	{
    return;
  }

	CTiming_start dlg;
	dlg.DoModal();
	SetMode();  
	CreateGPSThread();
}

UINT AFX_CDECL ConfigureNoisePowerControlThread(LPVOID param)
{
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(CGPSDlg::gpsDlg->m_inputMsg, 11, "Configure Noise Power Control successfully");
	return 0;
}

U32 m_propri_enable;
int m_propri_attr;
UINT ConfigProprietaryMessageThread(LPVOID param)
{
	U08 msg[6] = { 0 };

	msg[0] = 0x49;
	msg[1] = m_propri_enable>>24 & 0xFF;
	msg[2] = m_propri_enable>>16 & 0xFF;
	msg[3] = m_propri_enable>>8 & 0xFF;
	msg[4] = m_propri_enable & 0xFF;
	msg[5] = m_propri_attr;

	int len = CGPSDlg::gpsDlg->SetMessage(msg,sizeof(msg));
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(CGPSDlg::m_inputMsg, len, "Configure Proprietary NMEA successfully");

	return 0;	
}

void CGPSDlg::OnConfigProprietaryMessage()
{
	if(!CheckConnect())	
	{
		return;
	}

	CConfig_Proprietary dlg;
	if(dlg.DoModal()==IDOK)
	{
		m_propri_enable = dlg.enable;
		m_propri_attr = dlg.attr;
		AfxBeginThread(ConfigProprietaryMessageThread, 0);
	}
	else
	{
		SetMode();  
		CreateGPSThread();
	}
}

int m_antenna_control,m_antenna_attr;
UINT configy_antenna_detection_thread(LPVOID param)
{
	U08 msg[3];

	memset(msg,0,sizeof(msg));

	msg[0] = 0x47;
	msg[1] = m_antenna_control;
	msg[2] = m_antenna_attr;

	int len = CGPSDlg::gpsDlg->SetMessage(msg,sizeof(msg));
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(CGPSDlg::m_inputMsg, len, "Configure Antenna Detection successfully");

	return 0;	
}

void CGPSDlg::OnBinaryConfigureantennadetection()
{
	if(!CheckConnect())	
	{
    return;
  }
   
  CConAntennaDetection dlg;
	if(dlg.DoModal()==IDOK)
	{
		m_antenna_control = dlg.antenna_control;
		m_antenna_attr = dlg.attr;
		AfxBeginThread(configy_antenna_detection_thread, 0);
	}
	else
	{
		SetMode();  
		CreateGPSThread();
	}
}

U08 CGPSDlg::QueryChanelFreq(int chanel,U16 *prn,double *freq)
{
	CmdErrorCode ack;
	U32 v;
	long int nco_freq;
	int clk_base_reg = 0x60005A24;
	int clk_struct_size = 0x5b0;
	m_regAddress = clk_base_reg+4 + chanel*clk_struct_size;
	ack = QueryRegister(Return, &v);

	if(ack != Ack) 
	{
	return 0;
	}
	//return 0;

	*prn = v>>16;

	m_regAddress = 0x2000401C + chanel*0x100;
	ack = QueryRegister(Return, &v);
  if(ack != Ack) 
  {
    return 0;
  }
  v = v<<9;
  memcpy(&nco_freq,&v,4);
  nco_freq = nco_freq / 512;
  *freq = nco_freq * 0.030487 - 32051.25;

  return 1;
}

U08 CGPSDlg::PredictClockOffset(double *clk_offset)
{
	U08 ack;
	U16 prn;
	double nco_freq;
	int i;

	for(i=0;i<12;i++)
	{ 
		ack = QueryChanelFreq(i,&prn,&nco_freq);
		//return ack; // Test
		if(ack == 1)
		{       
			if(prn == 1)
			{
				*clk_offset = nco_freq - 173.3276;
				return 1;
			}     
		}
	}
	return 0;
}

void CGPSDlg::OnClockOffsetPredict()
{
	if(!CheckConnect())
	{
		return;
	}
	SetInputMode(NoOutputMode);
	int channelTable[GSA_MAX_SATELLITE] = {0};
	QueryChannelDoppler(Return, channelTable);
	QueryClockOffset(Display, channelTable);
	SetMode();
	CreateGPSThread();
}

void CGPSDlg::OnClockOffsetPredictOld()
{
	if(!CheckConnect())
	{
		return;
	}
	SetInputMode(NoOutputMode);
	QueryChannelDoppler(Display, NULL);
	SetMode();
	CreateGPSThread();
}

CGPSDlg::CmdErrorCode CGPSDlg::GetBinaryResponse(BinaryData* ackCmd, U08 cAck, U08 cAckSub, DWORD timeOut, bool silent, bool noWaitAck, int cmdSize, int cmdLen)
{
	ScopeTimer t;
	bool alreadyAck = noWaitAck;
	while(NULL != m_serial)
	{
		if(CGPSDlg::gpsDlg->CheckTimeOut(t.GetDuration(), timeOut, silent))
		{	//Time out
			return Timeout;
		}

		ackCmd->Clear();
    DWORD len = m_serial->GetBinaryAck(ackCmd->GetBuffer(), ackCmd->Size(), timeOut - t.GetDuration());
		if(!ReadOK(len))
		{	
			continue;
		}

    if((*ackCmd)[0] != 0xA0 || (*ackCmd)[1] != 0xA1)
    { //Not binary response
      continue;
    }
		int cmdSize = MAKEWORD((*ackCmd)[3], (*ackCmd)[2]);
		if(cmdSize != len - 7)
		{	//Packet Size Error
			continue;
		}
		if( (*ackCmd)[0] != 0xa0 || (*ackCmd)[1] != 0xa1 ||
			(*ackCmd)[len-2] != 0x0d || (*ackCmd)[len-1] != 0x0a)
		{	//Format Error
			continue;
		}
		if((*ackCmd)[4] == 0x83 && (*ackCmd)[5] == 0x0)
		{	//ACK0
			continue;
		}
		if((*ackCmd)[4] == 0x84)
		{	//NACK
			if(m_bShowBinaryCmdData)
			{
				add_msgtolist("NACK: " + theApp.GetHexString(ackCmd->Ptr(), len));	
			}
			add_msgtolist("Received NACK...");
			return NACK;
		}
		if((*ackCmd)[4] == 0x85)
		{	//FormatError
			if(m_bShowBinaryCmdData)
			{
				add_msgtolist("FormatError: " + theApp.GetHexString(ackCmd->Ptr(), len));	
			}

			if(cmdSize > 0 && cmdLen > 0)
			{
				CString txt;
				txt.Format("Format Error! Viewer / FW Length: %d/%d", ConvertLeonU16(ackCmd->Ptr(5 + cmdSize)), cmdLen);
				add_msgtolist(txt);
			}
			add_msgtolist("Received FormatError...");
			return FormatError;
		}

		if( (*ackCmd)[4] == 0x83)
		{	//Get ACK
			if(m_bShowBinaryCmdData)
			{
				add_msgtolist("Ack: " + theApp.GetHexString(ackCmd->Ptr(), len));	
			}
			alreadyAck = true;
			continue;
		}

		if(alreadyAck && CalcStqMsgChecksum((*ackCmd).GetBuffer()) && (*ackCmd)[4]==cAck && cAckSub==0x00)
		{
			if(m_bShowBinaryCmdData)
			{
				add_msgtolist("Out: "  +theApp.GetHexString(ackCmd->Ptr(), len));	
			}
			return Ack;
		}
		if(alreadyAck && CalcStqMsgChecksum((*ackCmd).GetBuffer()) && (*ackCmd)[4]==cAck && (*ackCmd)[5]==cAckSub)
		{
			if(m_bShowBinaryCmdData)
			{
				add_msgtolist("Out: "  +theApp.GetHexString(ackCmd->Ptr(), len));	
			}
			return Ack;
		}
	}
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::ExcuteBinaryCommand(U08 ackId, U08 ackSubId, BinaryCommand* cmd, BinaryData* ackCmd, DWORD timeOut, bool silent)
{
	U08* pCmd = cmd->GetBuffer();
	int inSize = cmd->Size();
	if(m_bShowBinaryCmdData)
	{
		add_msgtolist("In: " + theApp.GetHexString(pCmd, inSize));	
	}
	ackCmd->Alloc(1024);
	if(NULL == CGPSDlg::gpsDlg->m_serial) 
		return Timeout;
	m_serial->ClearQueue();
	m_serial->SendData(pCmd, inSize);

	return GetBinaryResponse(ackCmd, ackId, ackSubId, timeOut, silent);
}

CGPSDlg::CmdErrorCode CGPSDlg::ExcuteBinaryCommand(int cmdIdx, BinaryCommand* cmd, BinaryData* ackCmd, DWORD timeOut, bool silent)
{
	CommandEntry binMsg = cmdTable[cmdIdx];
  return CGPSDlg::ExcuteBinaryCommand(binMsg.cmdAck, binMsg.cmdAckSub, cmd, ackCmd, timeOut, silent);
}

CGPSDlg::CmdErrorCode CGPSDlg::ExcuteBinaryCommandNoWait(int cmdIdx, BinaryCommand* cmd)
{
	CommandEntry binMsg = cmdTable[cmdIdx];
	U08* pCmd = cmd->GetBuffer();
	int inSize = cmd->Size();
	if(m_bShowBinaryCmdData)
	{
		add_msgtolist("In: " + theApp.GetHexString(pCmd, inSize));	
	}

	m_serial->SendData(pCmd, inSize);
	return Timeout;
}

CGPSDlg::CmdErrorCode (CGPSDlg::*queryFunction)(CGPSDlg::CmdExeMode, void*) = NULL;
UINT GenericQueryThread(LPVOID param)
{
	(*CGPSDlg::gpsDlg.*queryFunction)(CGPSDlg::Display, NULL);
	if(NULL == CGPSDlg::gpsDlg->m_serial) return FALSE;
	CGPSDlg::gpsDlg->SetMode();  
	CGPSDlg::gpsDlg->CreateGPSThread();
	return TRUE;	
}

void CGPSDlg::GenericQuery(QueryFunction pfn)
{
	if(!CheckConnect())
	{
		return;
	}

	SetInputMode(NoOutputMode);
	queryFunction = pfn;
	::AfxBeginThread(GenericQueryThread, 0);
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryPositionRate(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryPositionRateCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryPositionRateCmd].cmdId);

	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(QueryPositionRateCmd, &cmd, &ackCmd))
	{
		if(nMode == Return)
		{
			*((U08*)outputData) = ackCmd[5];
			return Ack;
		}

		CString strMsg = "Query Position Rate successfully";
		add_msgtolist(strMsg);
		strMsg.Format("Position Rate = %d Hz", ackCmd[5]);
		add_msgtolist(strMsg);
	}
	return Timeout;
}

typedef struct ellipsoidlist
{
	D64 a;
	D64 I_F;
} TEL;

static const TEL el[] = 
{
	{0,0},
	{6377563.396,	299.3249646},
	{6377340.189,	299.3249646},
	{6378160,	    298.25},
	{6377483.865,	299.1528128},
	{6377397.155,	299.1528128},
	{6378206.4,  	294.9786982},
	{6378249.145,	293.465},
	{6377276.345,	300.8017},
	{6377298.556,	300.8017},
	{6377301.243,	300.8017},
	{6377295.664,	300.8017},
	{6377304.063,	300.8017},
	{6377309.613,	300.8017},
	{6378155,	    298.3},
	{6378200,	    298.3},
	{6378270,	    297},
	{6378160,	    298.247},
	{6378388,	    297},
	{6378245,	    298.3},
	{6378137,	    298.257222101},
	{6378160,	    298.25},
	{6378135,	    298.26},
	{6378137,	    298.257223563}
};

typedef struct datumreferencelist
{
	S16 DeltaX;
	S16 DeltaY;
	S16 DeltaZ;
	D64 Semi_Major_Axis;
	D64 Inversd_Flattening;
	U08 EllipsoidIndex;
} TDRL;

static const TDRL datum[] =
{
	{   0,    0,    0, el[23].a, el[23].I_F,23},
	{-118,  -14,  218, el[7].a,  el[7].I_F ,23},   
	{-134,   -2,  210, el[7].a,  el[7].I_F,7 },
	{-165,  -11,  206, el[7].a,  el[7].I_F ,7},	
	{-123,  -20,  220, el[7].a,  el[7].I_F ,7},
	{-166,  -15,  204, el[7].a,  el[7].I_F ,7},	
	{-128,  -18,  224, el[7].a,  el[7].I_F ,7},
	{-161,  -14,  205, el[7].a,  el[7].I_F ,7},
	{ -43, -163,   45, el[19].a, el[19].I_F,19},
	{-150, -250,   -1, el[18].a, el[18].I_F,18},	
	{-143, -236,    7, el[18].a, el[18].I_F,18},
	{-115,	118,  426, el[6].a,  el[6].I_F ,6},
	{-491,  -22,  435, el[3].a,  el[3].I_F ,3},  
	{-270,	 13,   62, el[7].a,  el[7].I_F ,7},	
	{-138, -105, -289, el[7].a, el[7].I_F,7},
	{-153,	 -5, -292, el[7].a, el[7].I_F ,7},	
	{-125, -108, -295, el[7].a, el[7].I_F,7}, 
	{-161,  -73, -317, el[7].a, el[7].I_F ,7},	
	{-143,	-90, -294, el[7].a, el[7].I_F,7},
	{-134, -105, -295, el[7].a, el[7].I_F ,7},	
	{-169,	-19, -278, el[7].a, el[7].I_F,7},
	{-147,  -74, -283, el[7].a, el[7].I_F ,7},	
	{-142,	-96, -293, el[7].a, el[7].I_F,7},
	{-160,   -6, -302, el[7].a, el[7].I_F ,7},	
	{-157,	 -2, -299, el[7].a, el[7].I_F,7},
	{-175,  -23, -303, el[7].a, el[7].I_F ,7},
	{-205,	107,   53, el[18].a, el[18].I_F,18},   
	{ 145,	 75, -272, el[18].a, el[18].I_F,18},	
	{-320,	550, -494, el[18].a, el[18].I_F,18},	
	{ 114, -116, -333,  el[18].a, el[18].I_F,18},	
	{ 124, -234,  -25, el[18].a, el[18].I_F,18},   
	{-133,  -48,  148, el[3].a,  el[3].I_F ,3},	
	{-134,	-48,  149, el[3].a, el[3].I_F,3},
	{ -79, -129,  145, el[7].a,  el[7].I_F,7},
	{-127, -769,  472, el[18].a, el[18].I_F,18},   
	{ -73,  213,  296, el[6].a, el[6].I_F,6},
	{-173,	253,   27, el[18].a, el[18].I_F,18},   
	{ 307,  304, -318, el[18].a, el[18].I_F,18},
	{-384,	664,  -48, el[5].a,  el[5].I_F,5},
	{-104, -129,  239, el[18].a, el[18].I_F,18},	
	{-148,	136,   90, el[18].a, el[18].I_F,18},
	{ 298, -304, -375, el[18].a, el[18].I_F,18},
	{-136, -108, -292, el[7].a, el[7].I_F,7},
	{  -2,  151,  181, el[6].a, el[6].I_F,6},
	{-263,	  6,  431, el[7].a, el[7].I_F,7},
	{ 175,  -38,  113, el[18].a, el[18].I_F,18},	
	{-134,	229,  -29, el[18].a, el[18].I_F,18},
	{-206,  172,   -6, el[18].a, el[18].I_F,18},
	{ -83,	 37,  124, el[7].a, el[7].I_F,7},     
	{ 260,   12, -147, el[7].a, el[7].I_F,7},
	{-377,	681,  -50, el[5].a, el[5].I_F,5},
	{ 230, -199, -752, el[18].a, el[18].I_F,18},	
	{ 211,	147,  111, el[18].a, el[18].I_F,18},
	{ 374,  150,  588, el[5].a, el[5].I_F,5},
	{-104, -101, -140, el[18].a, el[18].I_F,18},   
	{-130, -117, -151, el[18].a, el[18].I_F,18},
	{ -86,  -96, -120, el[18].a, el[18].I_F,18},   
	{ -86,  -96, -120, el[18].a, el[18].I_F,18},
	{ -87,	-95, -120, el[18].a, el[18].I_F,18},   
	{ -84,  -95, -130, el[18].a, el[18].I_F,18},
	{-117, -132, -164, el[18].a, el[18].I_F,18},   
	{ -97, -103, -120, el[18].a, el[18].I_F,18},
	{ -97,	-88, -135, el[18].a, el[18].I_F,18},   
	{-107,  -88, -149, el[18].a, el[18].I_F,18},
	{ -87,	-98, -121, el[18].a, el[18].I_F,18},   
	{ -87,  -96, -120, el[18].a, el[18].I_F,18},
	{-103, -106, -141, el[18].a, el[18].I_F,18},   
	{ -84, -107, -120, el[18].a, el[18].I_F,18},
	{-112,	-77, -145, el[18].a, el[18].I_F,18},   
	{ -86,  -98, -119, el[18].a, el[18].I_F,18},
	{  -7,	215,  225, el[7].a,  el[7].I_F,7},
	{-133, -321,   50, el[18].a,  el[18].I_F,18},	
	{  84,	-22,  209, el[18].a,  el[18].I_F,18},
	{-104,  167,  -38, el[18].a,  el[18].I_F ,18},
	{-100, -248,  259, el[6].a,  el[6].I_F,6},
	{-403,  684,   41, el[5].a,  el[5].I_F,5},
	{ 252, -209, -751, el[18].a,  el[18].I_F,18},  
	{-333, -222,  114, el[18].a,  el[18].I_F,18},
	{ 653, -212,  449, el[4].a,  el[4].I_F,4},
	{ -73,   46,  -86, el[18].a,  el[18].I_F,18},	
	{-156, -271, -189, el[18].a,  el[18].I_F,18},
	{-637, -549, -203, el[18].a,  el[18].I_F,18},
	{ 282,  726,  254, el[8].a,  el[8].I_F,8},
	{ 295,  736,  257, el[10].a,  el[10].I_F,10},
	{ 283,	682,  231, el[13].a,  el[13].I_F,13},
	{ 217,  823,  299, el[8].a,  el[8].I_F,8},	
	{ 182,	915,  344, el[8].a,  el[8].I_F,8},
	{ 198,  881,  317, el[8].a,  el[8].I_F,8},	
	{ 210,	814,  289, el[8].a,  el[8].I_F,8},
	{ -24,  -15,    5, el[17].a,  el[17].I_F,17},
	{ 506, -122,  611, el[2].a,  el[2].I_F,2},
	{-794,	119, -298, el[18].a,  el[18].I_F,18},	
	{ 208, -435, -229, el[18].a,  el[18].I_F,18},
	{ 189,	-79, -202, el[18].a,  el[18].I_F,18},
	{ -97,	787,   86,  el[8].a,  el[8].I_F,8},
	{ 145, -187,  103, el[18].a,  el[18].I_F,18},
	{ -11,	851,	5, el[12].a,  el[12].I_F,12},
	{ 647, 1777,-1124, el[18].a,  el[18].I_F,18},
	{   0,	  0,	0,  el[20].a,  el[20].I_F,20},
	{  42,  124,  147, el[6].a,  el[6].I_F,6},
	{-130,	 29,  364, el[7].a,  el[7].I_F,7},    
	{ -90,   40,   88, el[7].a,  el[7].I_F,7},
	{-133,	-77,  -51, el[6].a,  el[6].I_F,6},    
	{-133,  -79,  -72, el[6].a,  el[6].I_F,6},
	{ -74, -130,   42, el[7].a,  el[7].I_F,7},    
	{  41, -220, -134, el[7].a,  el[7].I_F,7},
	{ 639,	405,   60, el[5].a,  el[5].I_F,5},    
	{  31,  146,   47, el[7].a,  el[7].I_F,7},
	{ 912,	-58, 1227, el[18].a,  el[18].I_F,18},
	{ -81,  -84,  115, el[7].a,  el[7].I_F,7},	
	{ -92,  -93,  122, el[7].a,  el[7].I_F,7},
	{ 174,  359,  365, el[7].a,  el[7].I_F},	
	{-247, -148,  369, el[7].a,  el[7].I_F,7},
	{-243, -192,  477, el[7].a,  el[7].I_F},	
	{-249, -156,  381, el[7].a,  el[7].I_F,7},
	{ -10,  375,  165, el[18].a,  el[18].I_F,18},
	{  -5,	135,  172, el[6].a,  el[6].I_F,6},    
	{  -2,  152,  149, el[6].a,  el[6].I_F,6},
	{   2,	204,  105, el[6].a,  el[6].I_F,6},    
	{  -4,  154,  178, el[6].a,  el[6].I_F,6},
	{   1,	140,  165, el[6].a,  el[6].I_F,6},    
	{  -7,  162,  188, el[6].a,  el[6].I_F,6},
	{  -9,	157,  184, el[6].a,  el[6].I_F,6},    
	{ -22,  160,  190, el[6].a,  el[6].I_F,6},
	{   4,	159,  188, el[6].a,  el[6].I_F,6},    
	{  -7,	139,  181, el[6].a,  el[6].I_F,6},
	{   0,	125,  201, el[6].a,  el[6].I_F,6},    
	{  -9,	152,  178, el[6].a,  el[6].I_F,6},
	{  11,	114,  195, el[6].a,  el[6].I_F,6},    
	{  -3,	142,  183, el[6].a,  el[6].I_F,6},
	{   0,	125,  194, el[6].a,  el[6].I_F,6},    
	{ -10,	158,  187, el[6].a,  el[6].I_F,6},
	{  -8,	160,  176, el[6].a,  el[6].I_F,6},    
	{  -9,	161,  179, el[6].a,  el[6].I_F,6},
	{  -8,	159,  175, el[6].a,  el[6].I_F,6},    
	{ -12,	130,  190, el[6].a,  el[6].I_F,6},
	{   0,	  0,    0, el[20].a, el[20].I_F,20},   
	{  -2,	  0,    4, el[20].a,  el[20].I_F,20},
	{   0,	  0,	0, el[20].a, el[20].I_F,20},   
	{   0,	  0,    0, el[20].a,  el[20].I_F,20},
	{   1,	  1,   -1, el[20].a, el[20].I_F,20},   
	{   0,	  0,    0, el[20].a,  el[20].I_F,20},
	{-186,	-93,  310, el[7].a,  el[7].I_F,7},  
	{-425, -169,   81,  el[18].a,  el[18].I_F,18},
	{-130,	110,  -13,  el[15].a,  el[15].I_F,15},
	{  89, -279, -183,  el[6].a,  el[6].I_F,6},  
	{  45, -290, -172,  el[6].a,  el[6].I_F,6},
	{  65, -290, -190,  el[6].a,  el[6].I_F,6},	
	{  61, -285, -181,  el[6].a,  el[6].I_F,6}, 
	{  58, -283, -182,  el[6].a,  el[6].I_F,6},
	{-346,	 -1,  224,  el[7].a,  el[7].I_F,7},
	{ 371, -112,  434,  el[1].a,  el[1].I_F,1}, 	
	{ 371, -111,  434,  el[1].a,  el[1].I_F,1},
	{ 375, -111,  431,  el[1].a,  el[1].I_F,1},	
	{ 384, -111,  425,  el[1].a,  el[1].I_F,1}, 
	{ 370, -108,  434,  el[1].a,  el[1].I_F,1}, 	
	{-307,  -92,  127,  el[18].a,  el[18].I_F,18}, 
	{ 185,  165,   42,  el[18].a,  el[18].I_F,18}, 
	{-106, -129,  165,  el[7].a,  el[7].I_F,7},
	{-148,	 51, -291,  el[18].a,  el[18].I_F,18}, 
	{-499, -249,  314,  el[18].a,  el[18].I_F,18}, 
	{-270,  188, -388,  el[18].a,  el[18].I_F,18}, 
	{-270,	183, -390,  el[18].a,  el[18].I_F,18}, 
	{-305,	243, -442,  el[18].a,  el[18].I_F,18}, 
	{-282,	169, -371,  el[18].a,  el[18].I_F,18}, 
	{-278,	171, -367,  el[18].a,  el[18].I_F,18}, 
	{-298,	159, -369,  el[18].a,  el[18].I_F,18}, 
	{-288,	175, -376,  el[18].a,  el[18].I_F,18}, 
	{-279,	175, -379,  el[18].a,  el[18].I_F,18}, 
	{-295,	173, -371,  el[18].a,  el[18].I_F,18}, 
	{  16,	196,   93,  el[18].a,  el[18].I_F,18}, 
	{  11,	 72, -101,  el[6].a,  el[6].I_F,6},
	{  28, -130,  -95,  el[19].a,  el[19].I_F,19}, 
	{-128, -283,   22,  el[18].a,  el[18].I_F,18}, 
	{ 164,	138, -189,  el[18].a,  el[18].I_F,18}, 
	{  94, -948,-1262,  el[18].a,  el[18].I_F,18}, 
	{-225,	-65,	9,  el[18].a,  el[18].I_F,18},
	{  28, -121,  -77,  el[19].a,  el[19].I_F,19},
	{  23, -124,  -82,  el[19].a,  el[19].I_F,19},
	{  26, -121,  -78,   el[19].a,  el[19].I_F,19}, 
	{  24, -124,  -82,    el[19].a,  el[19].I_F,19}, 
	{  15, -130,  -84,    el[19].a,  el[19].I_F,19}, 
	{  24, -130,  -92,    el[19].a,  el[19].I_F,19}, 
	{  28, -121,  -77,   el[19].a,  el[19].I_F,19}, 
	{ 589,	 76,  480,   el[5].a,  el[5].I_F,5}, 
	{ 170,   42,   84,   el[18].a,  el[18].I_F,18}, 
	{-203,	141,   53,   el[18].a,  el[18].I_F,5}, 
	{-355,   21,   72,   el[18].a,  el[18].I_F,18}, 
	{ 616,	 97, -251,   el[4].a,  el[4].I_F,4}, 
	{-289, -124,   60,   el[18].a,  el[18].I_F,18}, 
	{ -88,	  4,  101,   el[7].a,  el[7].I_F,7}, 
	{ -62,   -1,  -37,   el[21].a,  el[21].I_F,21}, 
	{ -61,    2,  -48,  el[21].a,  el[21].I_F,21}, 
	{ -60,   -2,  -41,   el[21].a,  el[21].I_F,21}, 
	{ -75,	 -1,  -44,   el[21].a,  el[21].I_F,21}, 
	{ -44,    6,  -36,   el[21].a,  el[21].I_F,21}, 
	{ -48,    3,  -44,  el[21].a,  el[21].I_F,21}, 
	{ -47,   26,  -42,   el[21].a,  el[21].I_F,21}, 
	{ -53,	  3,  -47,   el[21].a,  el[21].I_F,21}, 
	{ -57,    1,  -41,   el[21].a,  el[21].I_F,21}, 
	{ -61,	  2,  -33,   el[21].a,  el[21].I_F,21}, 
	{ -58,    0,  -44,   el[21].a,  el[21].I_F,21}, 
	{ -45,	 12,  -33,   el[21].a,  el[21].I_F,21}, 
	{ -45,    8,  -33,   el[21].a,  el[21].I_F,21}, 
	{   7,	-10,  -26,  el[14].a,  el[14].I_F,14}, 
	{-189, -242,  -91,  el[18].a,  el[18].I_F,18}, 
	{-679,	669,  -48,   el[9].a,  el[7].I_F,7},
	{-148,  507,  685, el[5].a,  el[5].I_F,5},	
	{-148,	507,  685,  el[5].a,  el[5].I_F,5},
	{-158,  507,  676,  el[5].a,  el[5].I_F,5},	
	{-147,	506,  687,  el[5].a,  el[5].I_F,5},
	{-632,  438, -609,  el[18].a,  el[18].I_F,18},
	{  51,	391,  -36,  el[7].a,  el[7].I_F,7},
	{-123, -206,  219, el[7].a,  el[7].I_F,7},
	{ 276,	-57,  149,  el[18].a,  el[18].I_F,18},
	{ 102,   52,  -38, el[16].a,  el[16].I_F,16},
	{   0,	  0,	0,  el[22].a,  el[22].I_F,22},
	{-155,  171,   37,  el[18].a,  el[18].I_F,18},
	{-265,	120, -358,  el[18].a,  el[18].I_F,22},     	
};

CGPSDlg::CmdErrorCode CGPSDlg::QueryDatum(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryDatumCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryDatumCmd].cmdId);

	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(QueryDatumCmd, &cmd, &ackCmd))
	{
		U16 datumIdx = MAKEWORD(ackCmd[6], ackCmd[5]);
		CString strMsg = "Query Datum successfully";
		add_msgtolist(strMsg);
		strMsg.Format("DeltaX: %d, DeltaY: %d, DeltaZ: %d", 
			datum[datumIdx].DeltaX, datum[datumIdx].DeltaY, datum[datumIdx].DeltaZ);
		add_msgtolist(strMsg);
		strMsg.Format("Semi_Major_Axis: %.3f", datum[datumIdx].Semi_Major_Axis);
		add_msgtolist(strMsg);
		strMsg.Format("Inversed_Flattening: %.9f", datum[datumIdx].Inversd_Flattening);
		add_msgtolist(strMsg);
	}
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::QuerySha1String(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QuerySha1StringCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QuerySha1StringCmd].cmdId);
	cmd.SetU08(2, cmdTable[QuerySha1StringCmd].cmdSubId);

	if(NoWait==nMode)
	{
		ExcuteBinaryCommandNoWait(QuerySha1StringCmd, &cmd);
		return Timeout;
	}
	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(QuerySha1StringCmd, &cmd, &ackCmd))
	{
		CString strMsg, strSha;
		const int Sha1Length = 32;
		char* p = strSha.GetBuffer(Sha1Length + 1);
		memcpy(p, ackCmd.GetBuffer(6), Sha1Length);
		p[Sha1Length] = 0;
		strSha.ReleaseBuffer();
		strMsg.Format("SHA1: %s", strSha);
		add_msgtolist(strMsg);
	}
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryConstellationCapability(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryConstellationCapabilityCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryConstellationCapabilityCmd].cmdId);
	cmd.SetU08(2, cmdTable[QueryConstellationCapabilityCmd].cmdSubId);

	if(NoWait==nMode)
	{
		ExcuteBinaryCommandNoWait(QueryConstellationCapabilityCmd, &cmd);
		return Timeout;
	}
	BinaryData ackCmd;
  CmdErrorCode err = ExcuteBinaryCommand(QueryConstellationCapabilityCmd, &cmd, &ackCmd);
	if(err != Ack)
	{
    return err;
  }

  if(Return == nMode)
  {	
    *((BinaryData*)outputData) = ackCmd;
	  return err;
  }

	CString strMsg;
	U16 mode = MAKEWORD(ackCmd[7], ackCmd[6]);
	add_msgtolist("Constellation Capability: ");

	if(mode & 0x0001)
	{
		strMsg += "GPS, ";
	}
	if(mode & 0x0002)
	{
		strMsg += "GLONASS, ";
	}
	if(mode & 0x0004)
	{
		strMsg += "Galileo, ";
	}
	if(mode & 0x0008)
	{
		strMsg += "Beidou, ";
	}
	add_msgtolist(strMsg);

	UINT32 freq = ackCmd[8] << 24 & 0xff000000 | 
				ackCmd[9] << 16 & 0xff0000 | 
				ackCmd[10] << 8 &0xff00 | 
				ackCmd[11] & 0xff;
	strMsg.Format("GPS Ref. Freq.: %d Hz", freq);
	add_msgtolist(strMsg);

	freq = ackCmd[12] << 24 & 0xff000000 | 
				ackCmd[13] << 16 & 0xff0000 | 
				ackCmd[14] << 8 &0xff00 | 
				ackCmd[15] & 0xff;
	strMsg.Format("GLONASS Ref. Freq.: %d Hz", freq);
	add_msgtolist(strMsg);

	freq = ackCmd[16] << 24 & 0xff000000 | 
				ackCmd[17] << 16 & 0xff0000 | 
				ackCmd[18] << 8 &0xff00 | 
				ackCmd[19] & 0xff;
	strMsg.Format("Galileo Ref. Freq.: %d Hz", freq);
	add_msgtolist(strMsg);

	freq = ackCmd[20] << 24 & 0xff000000 | 
				ackCmd[21] << 16 & 0xff0000 | 
				ackCmd[22] << 8 &0xff00 | 
				ackCmd[23] & 0xff;
	strMsg.Format("Beidou Ref. Freq.: %d Hz", freq);
	add_msgtolist(strMsg);

	return Ack;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryVersionExtension(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryVersionExtensionCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryVersionExtensionCmd].cmdId);
	cmd.SetU08(2, cmdTable[QueryVersionExtensionCmd].cmdSubId);

	if(NoWait==nMode)
	{
		ExcuteBinaryCommandNoWait(QueryVersionExtensionCmd, &cmd);
		return Ack;
	}
	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(QueryVersionExtensionCmd, &cmd, &ackCmd))
	{
		CString strMsg, strSha;
		const int Sha1Length = 32;
		char* p = strSha.GetBuffer(Sha1Length + 1);
		memcpy(p, ackCmd.GetBuffer(6), Sha1Length);
		p[Sha1Length] = 0;
		strSha.ReleaseBuffer();

		if(nMode==Return)
		{
			*((CString*)outputData) = strSha;
			return Ack;
		}

		strMsg.Format("Version Extension: %s", strSha);
		add_msgtolist(strMsg);
	}
	return Timeout;
}

#if CUSTOMER_ZENLANE_160808
CGPSDlg::CmdErrorCode CGPSDlg::SendZenlandInitCmd(CmdExeMode nMode, void* outputData)
{
  //0xF2 0x0E 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x66 0x77 0x01 0x86 0x72 0x06 0x00 0x00
  U08 cmd[] = { 0xF2, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x66, 0x77, 0x01, 0x86, 0x72, 0x06, 0x00, 0x00 };
	BinaryData ackCmd;

  m_serial->ClearQueue();
  m_serial->SendData(cmd, sizeof(cmd), true);
  ackCmd.Alloc(32);
	DWORD len = m_serial->GetZenlaneResponse1(ackCmd.GetBuffer(), ackCmd.Size(), 2000);
  CString strMsg = "Initialization successfully";
  if(ackCmd[0] == 0xF2 && ackCmd[6] == 0x12 && ackCmd[10]==0x4b && ackCmd[11]==0x43 && ackCmd[12]==0x41)
  {
    add_msgtolist(strMsg);
    return Ack;
  }
  else
  {
    strMsg = "Initialization no response";
    add_msgtolist(strMsg);
  }

	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::SendZenlandQueryCmd(CmdExeMode nMode, void* outputData)
{
  //0xE7 0x0E 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x66 0x77 0x01 0x86 0x4F 0X06 0x00 0x00
  U08 cmd[] = { 0xE7, 0x0E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                0x00, 0x66, 0x77, 0x01, 0x86, 0x4F, 0x06, 0x00, 0x00 };
	BinaryData ackCmd;

    m_serial->ClearQueue();
	  m_serial->SendData(cmd, sizeof(cmd), true);
    ackCmd.Alloc(32);
		DWORD len = m_serial->GetZenlaneResponse1(ackCmd.GetBuffer(), ackCmd.Size(), 2000);
    CString strMsg = "Send query command...";
    add_msgtolist(strMsg);
    //if(ackCmd[6] == 0x12 && ackCmd[10]==0x4b && ackCmd[11]==0x43 && ackCmd[12]==0x41)
    //{
    //  //add_msgtolist(strMsg);
    //  return Ack;
    //}
    //else
    //{
    //  //strMsg = "Query no response";
    //  //add_msgtolist(strMsg);
    //}
		return Ack;
}
#endif 

CGPSDlg::CmdErrorCode CGPSDlg::QuerySoftwareVersionSystemCode(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QuerySwVerSysCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QuerySwVerSysCmd].cmdId);
	cmd.SetU08(2, cmdTable[QuerySwVerSysCmd].cmdSubId);

	if(NoWait == nMode)
	{
		ExcuteBinaryCommandNoWait(QuerySwVerSysCmd, &cmd);
		return Ack;
	}

	BinaryData ackCmd;
  CmdErrorCode err = ExcuteBinaryCommand(QuerySwVerSysCmd, &cmd, &ackCmd, (nMode == Display) ? g_setting.GetLongTimeout() : g_setting.GetShortTimeout());
	if(err != Ack)
	{
		return err;
	}

	if(Return == nMode)
	{	//Return command data
    *((BinaryData*)outputData) = ackCmd;
		return err;
	}

	CString strMsg, strExt;
	if(Ack == QueryVersionExtension(Return, &strExt))
	{
	}

	strMsg = "Query Version successfully";
	add_msgtolist(strMsg);
	strMsg.Format("%s%d.%d.%d%s", "Kernel Version ", ackCmd[7], ackCmd[8], ackCmd[9], strExt);
	add_msgtolist(strMsg);
	strMsg.Format("%s%d.%d.%d", "Software Version ", ackCmd[11], ackCmd[12], ackCmd[13]);
	add_msgtolist(strMsg);
	strMsg.Format("%s%d.%d.%d", "Revision ", ackCmd[15] + 2000, ackCmd[16], ackCmd[17]);
	add_msgtolist(strMsg);
	m_versionInfo = ackCmd;
	return Ack;
}

/*
CGPSDlg::CmdErrorCode CGPSDlg::QuerySoftwareCrcRomCode(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QuerySwCrcRomCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QuerySwCrcRomCmd].cmdId);
	cmd.SetU08(2, cmdTable[QuerySwCrcRomCmd].cmdSubId);

	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(QuerySwCrcRomCmd, &cmd, &ackCmd, 5000))
	{
		CString strMsg = "Query CRC successfully";
		add_msgtolist(strMsg);
		strMsg.Format("%s%02x%02x", "Rom CRC: ", ackCmd[6], ackCmd[7]);
		add_msgtolist(strMsg);
	}
	return Timeout;
}
*/
CGPSDlg::CmdErrorCode CGPSDlg::QuerySoftwareCrcSystemCode(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QuerySwCrcSysCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QuerySwCrcSysCmd].cmdId);
	cmd.SetU08(2, cmdTable[QuerySwCrcSysCmd].cmdSubId);

	BinaryData ackCmd;
  //QuerySoftwareCrcSystemCode need long timeout
	if(Ack == ExcuteBinaryCommand(QuerySwCrcSysCmd, &cmd, &ackCmd, 5000))
	{
		if(nMode == Return)
		{
			*((U16*)outputData) = (ackCmd[6] << 8) | ackCmd[7];
			return Ack;
		}

		CString strMsg = "Query CRC successfully";
    WORD ackSize = MAKEWORD(ackCmd[3], ackCmd[2]);
		add_msgtolist(strMsg);
    if(ackSize == 4)
    {
		  strMsg.Format("%s%02x%02x", "System CRC: ", ackCmd[6], ackCmd[7]);
    }
    else
    {
      strMsg.Format("%s%02x%02x%02x%02x", "System CRC32: ", ackCmd[6], ackCmd[7], ackCmd[8], ackCmd[9]);
    }
		add_msgtolist(strMsg);
	}
	return Ack;
}

CGPSDlg::CmdErrorCode CGPSDlg::QuerySoftwareCrc32SystemCode(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QuerySwCrcSysCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QuerySwCrcSysCmd].cmdId);
	cmd.SetU08(2, 0x03);

	BinaryData ackCmd;
  CmdErrorCode err = ExcuteBinaryCommand(QuerySwCrcSysCmd, &cmd, &ackCmd, 5000);
	if(err != Ack)
	{
		return err;
	}

	if(Return == nMode)
	{	//Return command data
    U32 data = MAKELONG(MAKEWORD(ackCmd[9], ackCmd[8]), MAKEWORD(ackCmd[7], ackCmd[6]));
		if(nMode == Return)
		{
			*((U32*)outputData) = data;
			return err;
		}
  }

	CString strMsg = "Query CRC successfully";
  WORD ackSize = MAKEWORD(ackCmd[3], ackCmd[2]);
	add_msgtolist(strMsg);
  if(ackSize == 4)
  {
	  strMsg.Format("%s%02x%02x", "System CRC: ", ackCmd[6], ackCmd[7]);
  }
  else
  {
    strMsg.Format("%s%02x%02x%02x%02x", "System CRC32: ", ackCmd[6], ackCmd[7], ackCmd[8], ackCmd[9]);
  }
	add_msgtolist(strMsg);

	return err;
}
/*
CGPSDlg::CmdErrorCode CGPSDlg::QueryWaasStatus(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryWaasStatusCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryWaasStatusCmd].cmdId);
	cmd.SetU08(2, cmdTable[QueryWaasStatusCmd].cmdSubId);

	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(QueryWaasStatusCmd, &cmd, &ackCmd))
	{
		CString strMsg = "Query Waas Status successfully";
		add_msgtolist(strMsg);
		strMsg.Format("WAAS status: %s", (ackCmd[5]) ? "Enable" : "Disable");
		add_msgtolist(strMsg);
	}
	return Timeout;
}
*/
CGPSDlg::CmdErrorCode CGPSDlg::QueryPositionPinning(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryPositionPinningCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryPositionPinningCmd].cmdId);

	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(QueryPositionPinningCmd, &cmd, &ackCmd))
	{
		CString strMsg = "Query Position Pinning successfully";
		add_msgtolist(strMsg);
		strMsg = "Position Pinning: ";
		if(0==ackCmd[5])
		{
			strMsg += "Default";
		}
		else if(1==ackCmd[5])
		{
			strMsg += "Enable";
		}
		else if(2==ackCmd[5])
		{
			strMsg += "Disable";
		}
		add_msgtolist(strMsg);
		strMsg.Format("Pinning Speed: %d", MAKEWORD(ackCmd[7], ackCmd[6]));
		add_msgtolist(strMsg);
		strMsg.Format("Pinning Cnt: %d", MAKEWORD(ackCmd[9], ackCmd[8]));
		add_msgtolist(strMsg);
		strMsg.Format("Unpinning Speed: %d", MAKEWORD(ackCmd[11], ackCmd[10]));
		add_msgtolist(strMsg);
		strMsg.Format("Unpinning Cnt: %d", MAKEWORD(ackCmd[13], ackCmd[12]));
		add_msgtolist(strMsg);
		strMsg.Format("Unpinning Distance: %d", MAKEWORD(ackCmd[15], ackCmd[14]));
		add_msgtolist(strMsg);
	}
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::Query1ppsMode(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[Query1ppsModeCmd].cmdSize);
	cmd.SetU08(1, cmdTable[Query1ppsModeCmd].cmdId);

	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(Query1ppsModeCmd, &cmd, &ackCmd))
	{
		CString strMsg = "Query GPS Measurement Mode successfully";
		add_msgtolist(strMsg);
		strMsg = "GPS Measurement Mode: ";
		if(0==ackCmd[5])
		{
			strMsg += "Not sync to UTC second";
		}
		else if(1==ackCmd[5])
		{
			strMsg += "Sync to UTC second when 3D fix";
		}
		else if(2==ackCmd[5])
		{
			strMsg += "On when 1 SV";
		}
		add_msgtolist(strMsg);
	}
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryPowerMode(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryPowerModeCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryPowerModeCmd].cmdId);

	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(QueryPowerModeCmd, &cmd, &ackCmd))
	{
		CString strMsg = "Query Power Mode successfully";
		add_msgtolist(strMsg);
		strMsg = "Power Mode: ";
		if(1==ackCmd[5])
		{
			strMsg += "Power Save";
		}
		else
		{
			strMsg += "Normal";
		}
		add_msgtolist(strMsg);
	}
	return Timeout;
}
/*
CGPSDlg::CmdErrorCode CGPSDlg::QueryPowerSavingParameters(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryPowerSavingParametersCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryPowerSavingParametersCmd].cmdId);

	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(QueryPowerSavingParametersCmd, &cmd, &ackCmd))
	{
		CString strMsg = "Query Power Saving Parameters successfully";
		add_msgtolist(strMsg);
		strMsg.Format("2 PSE no fix cycle time:%d", MAKEWORD(ackCmd[7], ackCmd[6]));
		add_msgtolist(strMsg);
		strMsg.Format("2 PSE fix cycle time:%d", MAKEWORD(ackCmd[9], ackCmd[8]));
		add_msgtolist(strMsg);
		strMsg.Format("4 PSE no fix cycle time:%d", MAKEWORD(ackCmd[11], ackCmd[10]));
		add_msgtolist(strMsg);
		strMsg.Format("4 PSE fix cycle time:%d", MAKEWORD(ackCmd[13], ackCmd[12]));
		add_msgtolist(strMsg);
		strMsg.Format("First boot up full power time:%d", MAKEWORD(ackCmd[15], ackCmd[14]));
		add_msgtolist(strMsg);
		strMsg.Format("Power save entering wait:%d", MAKEWORD(ackCmd[17], ackCmd[16]));
		add_msgtolist(strMsg);
		strMsg.Format("Cold start full power time:%d", MAKEWORD(ackCmd[19], ackCmd[18]));
		add_msgtolist(strMsg);
		strMsg.Format("Tunnel full power time:%d", MAKEWORD(ackCmd[21], ackCmd[20]));
		add_msgtolist(strMsg);
		strMsg.Format("SV no diff time:%d", MAKEWORD(ackCmd[23], ackCmd[22]));
		add_msgtolist(strMsg);
		strMsg = "Power Mode: ";
		if(1==ackCmd[24])
		{
			strMsg += "Power Save";
		}
		else
		{
			strMsg += "Normal";
		}
		add_msgtolist(strMsg);
	}
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryV8PowerSavingParametersRom(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryV8RomPowerSaveParameters].cmdSize);
	cmd.SetU08(1, cmdTable[QueryV8RomPowerSaveParameters].cmdId);
	cmd.SetU08(2, cmdTable[QueryV8RomPowerSaveParameters].cmdSubId);

	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(QueryV8RomPowerSaveParameters, &cmd, &ackCmd))
	{
		CString strMsg = "Query Power Saving Parameters successfully";
		add_msgtolist(strMsg);
		strMsg.Format("2 PSE no fix cycle time:%d", MAKEWORD(ackCmd[7], ackCmd[6]));
		add_msgtolist(strMsg);
		strMsg.Format("2 PSE fix cycle time:%d", MAKEWORD(ackCmd[9], ackCmd[8]));
		add_msgtolist(strMsg);
		strMsg.Format("4 PSE no fix cycle time:%d", MAKEWORD(ackCmd[11], ackCmd[10]));
		add_msgtolist(strMsg);
		strMsg.Format("4 PSE fix cycle time:%d", MAKEWORD(ackCmd[13], ackCmd[12]));
		add_msgtolist(strMsg);
		strMsg.Format("First boot up full power time:%d", MAKEWORD(ackCmd[15], ackCmd[14]));
		add_msgtolist(strMsg);
		strMsg.Format("Power save entering wait:%d", MAKEWORD(ackCmd[17], ackCmd[16]));
		add_msgtolist(strMsg);
		strMsg.Format("Cold start full power time:%d", MAKEWORD(ackCmd[19], ackCmd[18]));
		add_msgtolist(strMsg);
	}
	return Timeout;
}
*/
CGPSDlg::CmdErrorCode CGPSDlg::QueryV8PowerSavingParameters(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryV8PowerSaveParameters].cmdSize);
	cmd.SetU08(1, cmdTable[QueryV8PowerSaveParameters].cmdId);
	cmd.SetU08(2, cmdTable[QueryV8PowerSaveParameters].cmdSubId);

	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(QueryV8PowerSaveParameters, &cmd, &ackCmd))
	{
		CString strMsg = "Query Power Saving Parameters successfully";
		add_msgtolist(strMsg);
		strMsg.Format("2 PSE no fix cycle time:%d", MAKEWORD(ackCmd[7], ackCmd[6]));
		add_msgtolist(strMsg);
		strMsg.Format("2 PSE fix cycle time:%d", MAKEWORD(ackCmd[9], ackCmd[8]));
		add_msgtolist(strMsg);
		strMsg.Format("4 PSE no fix cycle time:%d", MAKEWORD(ackCmd[11], ackCmd[10]));
		add_msgtolist(strMsg);
		strMsg.Format("4 PSE fix cycle time:%d", MAKEWORD(ackCmd[13], ackCmd[12]));
		add_msgtolist(strMsg);
		strMsg.Format("First boot up full power time:%d", MAKEWORD(ackCmd[15], ackCmd[14]));
		add_msgtolist(strMsg);
		strMsg.Format("Power save entering wait:%d", MAKEWORD(ackCmd[17], ackCmd[16]));
		add_msgtolist(strMsg);
		strMsg.Format("Cold start full power time:%d", MAKEWORD(ackCmd[19], ackCmd[18]));
		add_msgtolist(strMsg);
		strMsg.Format("Reserved:%d", MAKEWORD(ackCmd[21], ackCmd[20]));
		add_msgtolist(strMsg);
		strMsg.Format("SV no diff time:%d", MAKEWORD(ackCmd[23], ackCmd[22]));
		add_msgtolist(strMsg);
		strMsg = "Power Mode: ";
		if(1==ackCmd[24])
		{
			strMsg += "Power Save";
		}
		else
		{
			strMsg += "Normal";
		}
		add_msgtolist(strMsg);
	}
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryProprietaryMessage(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryProprietaryMessageCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryProprietaryMessageCmd].cmdId);

	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(QueryProprietaryMessageCmd, &cmd, &ackCmd))
	{
		add_msgtolist("Query Proprietary Message successfully");
		DWORD enable = MAKELONG(MAKEWORD(ackCmd[8], ackCmd[7]), MAKEWORD(ackCmd[6], ackCmd[5]));
		if(enable & 0x00000001)
		{
			add_msgtolist("epe enable");
		}
		else
		{
			add_msgtolist("epe disable");
		}
	}
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryTiming(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryTimingCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryTimingCmd].cmdId);

	BinaryData ackCmd;
	CmdErrorCode err = ExcuteBinaryCommand(QueryTimingCmd, &cmd, &ackCmd, (nMode == Display) ? g_setting.GetLongTimeout() : g_setting.GetShortTimeout());
	if(err != Ack)
	{
		return err;
	}
 
	if(Return == nMode)
	{	//Return command data
    *((BinaryData*)outputData) = ackCmd;
		return err;
	}

	CString strMsg = "Query Timing successfully";
	add_msgtolist(strMsg);

	//_1PPS_Timing_T t;
	U08 timingMode = ackCmd[5];
	U32 surveyLength = MAKELONG(MAKEWORD(ackCmd[9], ackCmd[8]), MAKEWORD(ackCmd[7], ackCmd[6]));
	U32 standardDeviation = MAKELONG(MAKEWORD(ackCmd[13], ackCmd[12]), MAKEWORD(ackCmd[11], ackCmd[10]));
	D64 latitude;
	D64 longitude;
	F32 altitude;

	for(int i = 0; i < sizeof(latitude); ++i)
	{
		((BYTE*)(&latitude))[7 - i] = ackCmd[14 + i];
		((BYTE*)(&longitude))[7 - i] = ackCmd[22 + i];
		if(i < sizeof(altitude))
		{
			((BYTE*)(&altitude))[3 - i] = ackCmd[30 + i];
		}
	}
	U08 rtTimingMode = ackCmd[34];
	U32 rtSurveyLength = MAKELONG(MAKEWORD(ackCmd[38], ackCmd[37]), MAKEWORD(ackCmd[36], ackCmd[35]));

	if(timingMode == 0)
	{
		add_msgtolist("Timing Mode: PVT Mode");
	}
	else if(timingMode == 1)
	{
		add_msgtolist("Timing Mode: Survey Mode");
		strMsg.Format("Survey Length: %d", surveyLength);
		add_msgtolist(strMsg);
		strMsg.Format("Standard deviation: %d", standardDeviation);
		add_msgtolist(strMsg);
	}
	else if(timingMode == 2)
	{
		add_msgtolist("Timing Mode: Static Mode");
		strMsg.Format("Saved Latitude: %12.9lf", latitude);
		add_msgtolist(strMsg);
		strMsg.Format("Saved Longitude: %12.9lf", longitude);
		add_msgtolist(strMsg);
		strMsg.Format("Saved Altitude: %3.2f", altitude);
		add_msgtolist(strMsg);
	}

	if(rtTimingMode == 0)
	{
		add_msgtolist("Run-time Timing Mode: PVT Mode");
	}
	else if(rtTimingMode == 1)
	{
		add_msgtolist("Run-time Timing Mode: Survey Mode");
		strMsg.Format("Run-time Survey Length: %d", rtSurveyLength);
		add_msgtolist(strMsg);
	}
	else if(rtTimingMode == 2)
	{
		add_msgtolist("Run-time Timing Mode: Static Mode");
	}
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryDopMask(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryDopMaskCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryDopMaskCmd].cmdId);

	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(QueryDopMaskCmd, &cmd, &ackCmd))
	{
		CString strMsg = "Query DOP Mask successfully";
		add_msgtolist(strMsg);

		strMsg.Format("DOP Mode: %d", ackCmd[5]);
		add_msgtolist(strMsg);
		if(ackCmd[5]==1)
		{
			strMsg.Format("PDOP: %.1f", MAKEWORD(ackCmd[7], ackCmd[6]) / 10.0);
			add_msgtolist(strMsg);
			strMsg.Format("HDOP: %.1f", MAKEWORD(ackCmd[9], ackCmd[8]) / 10.0);
			add_msgtolist(strMsg);
		}
		else if(ackCmd[5] == 2)
		{
			strMsg.Format("PDOP: %.1f", MAKEWORD(ackCmd[7], ackCmd[6]) / 10.0);
			add_msgtolist(strMsg);
		}
		else if(ackCmd[5] == 3)
		{
			strMsg.Format("HDOP: %.1f", MAKEWORD(ackCmd[9], ackCmd[8]) / 10.0);
			add_msgtolist(strMsg);
		}
		else if(ackCmd[5] == 4)
		{
			strMsg.Format("GDOP: %.1f", MAKEWORD(ackCmd[11], ackCmd[10]) / 10.0);
			add_msgtolist(strMsg);
		}
	}
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryElevationAndCnrMask(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryElevationAndCnrMaskCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryElevationAndCnrMaskCmd].cmdId);

	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(QueryElevationAndCnrMaskCmd, &cmd, &ackCmd))
	{
		CString strMsg = "Query Elevation and CNR Mask successfully";
		add_msgtolist(strMsg);
		strMsg.Format("Elevation and CNR Mask Mode: %d", ackCmd[5]);
		add_msgtolist(strMsg);
		if(ackCmd[5]==1)
		{
			strMsg.Format("Elevation Mask: %d", ackCmd[6]);
			add_msgtolist(strMsg);
			strMsg.Format("CNR Mask: %d", ackCmd[7]);
			add_msgtolist(strMsg);
		}
		else if(ackCmd[5] == 2)
		{
			strMsg.Format("Elevation Mask: %d", ackCmd[6]);
			add_msgtolist(strMsg);
		}
		else if(ackCmd[5] == 3)
		{
			strMsg.Format("CNR Mask: %d", ackCmd[7]);
			add_msgtolist(strMsg);
		}
	}
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryAntennaDetection(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryAntennaDetectionCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryAntennaDetectionCmd].cmdId);

	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(QueryAntennaDetectionCmd, &cmd, &ackCmd))
	{
		CString strMsg = "Query Antenna Detection successfully";
		add_msgtolist(strMsg);
		//20150709 Modify for fw spec, just only ON/OFF.
		//if(ackCmd[5] & 0x01)
		//{
		//	add_msgtolist("Short Circuit Detection On");
		//}
		//else
		//{
		//	add_msgtolist("Short Circuit Detection Off");
		//}
		//if(ackCmd[5] & 0x02)
		//{
		//	add_msgtolist("Antenna Detection On");
		//}
		//else
		//{
		//	add_msgtolist("Antenna Detection Off");
		//}
		if(ackCmd[5])
		{
			add_msgtolist("Antenna Detection On");
		}
		else
		{
			add_msgtolist("Antenna Detection Off");
		}

		strMsg.Format("Antenna Mode Status:0x%02X", ackCmd[6]);
		add_msgtolist(strMsg);
	}
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryNoisePower(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryNoisePowerCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryNoisePowerCmd].cmdId);

	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(QueryNoisePowerCmd, &cmd, &ackCmd))
	{
		CString strMsg = "Query Noise Power successfully";
		add_msgtolist(strMsg);
		strMsg.Format("Noise Power: %d", MAKELONG(MAKEWORD(ackCmd[8], ackCmd[7]), MAKEWORD(ackCmd[6], ackCmd[5])));
		add_msgtolist(strMsg);
	}
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryDrInfo(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryDrInfoCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryDrInfoCmd].cmdId);

	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(QueryDrInfoCmd, &cmd, &ackCmd))
	{
		CString strMsg = "Query DR Info successfully";
		add_msgtolist(strMsg);
		//data = buff[7]<<24 | buff[8]<<16 | buff[9]<<8 | buff[10];
		F32 gyroData;
		for(int i=0; i<sizeof(gyroData); ++i)
		{
			((BYTE*)(&gyroData))[sizeof(gyroData) - i - 1] = ackCmd[7 + i];

		}
		strMsg.Format("Temperature = %f",(F32)MAKEWORD(ackCmd[6], ackCmd[5]) / 10.0);
		add_msgtolist(strMsg);
		strMsg.Format("Gyro Data = %f",gyroData);
		add_msgtolist(strMsg);
		strMsg.Format("Odo Data = %d", MAKEWORD(ackCmd[12], ackCmd[11]));
		add_msgtolist(strMsg);
		strMsg.Format("Odo backward indicator =  %s", (ackCmd[13]) ? "TRUE" : "FALSE");
		add_msgtolist(strMsg);

	}
	return Timeout;
}
/*
CGPSDlg::CmdErrorCode CGPSDlg::QueryDrHwParameter(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryDrHwParameterCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryDrHwParameterCmd].cmdId);

	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(QueryDrHwParameterCmd, &cmd, &ackCmd))
	{
		CString strMsg = "Query DR HW Parameter successfully";
		add_msgtolist(strMsg);

		F32 gyroOutputUnit, gyroReference, gyroAdcReference, gyroAdcGain, defaultOdoScale;
		for(int i=0; i<sizeof(gyroOutputUnit); ++i)
		{
			((BYTE*)(&gyroOutputUnit))[sizeof(F32) - i - 1] = ackCmd[5 + i];
			((BYTE*)(&gyroReference))[sizeof(F32) - i - 1] = ackCmd[9 + i];
			((BYTE*)(&gyroAdcReference))[sizeof(F32) - i - 1] = ackCmd[14 + i];
			((BYTE*)(&gyroAdcGain))[sizeof(F32) - i - 1] = ackCmd[21 + i];
			((BYTE*)(&defaultOdoScale))[sizeof(F32) - i - 1] = ackCmd[25 + i];

		}
		strMsg.Format("Gyro output unit = %f mV/dps", gyroOutputUnit);
		add_msgtolist(strMsg);
		strMsg.Format("Gyro reference voltage = %f", gyroReference);
		add_msgtolist(strMsg);
		strMsg.Format("Gyro ADC type = %s", (ackCmd[13]) ? "unipolar" : "differential");
		add_msgtolist(strMsg);
		strMsg.Format("Gyro ADC reference voltage = %f", gyroAdcReference);
		add_msgtolist(strMsg);
		strMsg.Format("Gyro ADC sampling freq = %d", MAKEWORD(ackCmd[19], ackCmd[18]));
		add_msgtolist(strMsg);
		strMsg.Format("Gyro ADC NBITS = %d", ackCmd[20]);
		add_msgtolist(strMsg);
		strMsg.Format("Gyro ADC gain = %f", gyroAdcGain);
		add_msgtolist(strMsg);
		strMsg.Format("Default ODO scale = %f", defaultOdoScale);
		add_msgtolist(strMsg);
		strMsg.Format("%s", (ackCmd[29]) ? "ODO has backward" : "ODO no backward");
		add_msgtolist(strMsg);
		strMsg.Format("%s", (ackCmd[30]) ? "ODO invert FW/BW" : "ODO not invert FW/BW");
		add_msgtolist(strMsg);
		strMsg.Format("%s", (ackCmd[31]) ? "ODO in period mode" : "ODO in hz mode");
		add_msgtolist(strMsg);
		strMsg.Format("ODO period in = %d us", MAKEWORD(ackCmd[33], ackCmd[32]));
		add_msgtolist(strMsg);
		strMsg.Format("DR firmware version = %d.%d.%d", ackCmd[35], ackCmd[36], ackCmd[37]);
		add_msgtolist(strMsg);
		strMsg.Format("%s", (ackCmd[38]) ? "DR enable temperature" : "DR not enable temperature");
		add_msgtolist(strMsg);
		strMsg.Format("%s", (ackCmd[39]) ? "DR enable tunnel table" : "DR not enable tunnel table");
		add_msgtolist(strMsg);
		strMsg = "DR table region = ";
		if(ackCmd[43] & 0x01)
		{
			strMsg.Append("Taiwan,");
		}
		if(ackCmd[43] & 0x02)
		{
			strMsg.Append("Korea,");
		}
		if(ackCmd[43] & 0x04)
		{
			strMsg.Append("Japan,");
		}
		if(ackCmd[43] & 0x08)
		{
			strMsg.Append("China,");
		}
		add_msgtolist(strMsg);

	}
	return Timeout;
}
*/
CGPSDlg::CmdErrorCode CGPSDlg::QuerySbas(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QuerySbasCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QuerySbasCmd].cmdId);
	cmd.SetU08(2, cmdTable[QuerySbasCmd].cmdSubId);

	BinaryData ackCmd;
  CmdErrorCode err = ExcuteBinaryCommand(QuerySbasCmd, &cmd, &ackCmd, (nMode == Display) ? g_setting.GetLongTimeout() : g_setting.GetShortTimeout());
	if(err != Ack)
	{
    return err;
  }

  if(Return == nMode)
	{	
    *((BinaryData*)outputData) = ackCmd;
		return err;
  }

	CString strMsg = "Query SBAS successfully";
	add_msgtolist(strMsg);
	strMsg.Format("SBAS system: %s", ((ackCmd[6]) ? "Enable" : "Disable"));
	add_msgtolist(strMsg);
	if(ackCmd[7]==0)
	{
		strMsg.Format("Ranging: %s", "Disable");
	}
	else if(ackCmd[7]==1)
	{
		strMsg.Format("Ranging: %s", "Enable");
	}
	else
	{
		strMsg.Format("Ranging: %s", "Auto");
	}
	add_msgtolist(strMsg);
	strMsg.Format("Ranking URA Mask: %d", ackCmd[8]);
	add_msgtolist(strMsg);
	strMsg.Format("Correction: %s", ((ackCmd[9]) ? "Enable" : "Disable"));
	add_msgtolist(strMsg);
	strMsg.Format("Number of tracking channels: %d", ackCmd[10]);
	add_msgtolist(strMsg);
	strMsg.Format("All: %s", ((ackCmd[11] & 0x80) ? "Enable" : "Disable"));
	add_msgtolist(strMsg);	
	if(!(ackCmd[11] & 0x80))
	{
		strMsg.Format("WAAS: %s", ((ackCmd[11] & 0x01) ? "Enable" : "Disable"));
		add_msgtolist(strMsg);
		strMsg.Format("EGNOS: %s", ((ackCmd[11] & 0x02) ? "Enable" : "Disable"));
		add_msgtolist(strMsg);
		strMsg.Format("MSAS: %s", ((ackCmd[11] & 0x04) ? "Enable" : "Disable"));
		add_msgtolist(strMsg);
		strMsg.Format("GAGAN: %s", ((ackCmd[11] & 0x08) ? "Enable" : "Disable"));
		add_msgtolist(strMsg);
	}
	return Ack;
}

CGPSDlg::CmdErrorCode CGPSDlg::QuerySbas2(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QuerySbas2Cmd].cmdSize);
	cmd.SetU08(1, cmdTable[QuerySbas2Cmd].cmdId);
	cmd.SetU08(2, cmdTable[QuerySbas2Cmd].cmdSubId);

	BinaryData ackCmd;
  CmdErrorCode err = ExcuteBinaryCommand(QuerySbas2Cmd, &cmd, &ackCmd, (nMode == Display) ? g_setting.GetLongTimeout() : g_setting.GetShortTimeout());
	if(err != Ack)
	{
    return err;
  }

  if(Return == nMode)
	{	
    *((BinaryData*)outputData) = ackCmd;
		return err;
  }

  U16 cmdLen = ConvertLeonU16(ackCmd.Ptr(2));
	CString strMsg = "Query SBAS Advanced successfully";
	add_msgtolist(strMsg);
  if(25 == cmdLen)  //Old style
  {
	  strMsg.Format("SBAS system: %s", ((ackCmd[6]) ? "Enable" : "Disable"));
	  add_msgtolist(strMsg);
	  if(ackCmd[7] == 0)
	  {
		  strMsg.Format("Ranging: %s", "Disable");
	  }
	  else if(ackCmd[7] == 1)
	  {
		  strMsg.Format("Ranging: %s", "Enable");
	  }
	  else
	  {
		  strMsg.Format("Ranging: %s", "Auto");
	  }
	  add_msgtolist(strMsg);
	  strMsg.Format("Ranking URA Mask: %d", ackCmd[8]);
	  add_msgtolist(strMsg);
	  strMsg.Format("Correction: %s", ((ackCmd[9]) ? "Enable" : "Disable"));
	  add_msgtolist(strMsg);
	  strMsg.Format("Number of tracking channels: %d", ackCmd[10]);
	  add_msgtolist(strMsg);
	  strMsg.Format("All: %s", ((ackCmd[11] & 0x80) ? "Enable" : "Disable"));
	  add_msgtolist(strMsg);	
	  if(!(ackCmd[11] & 0x80))
	  {
		  strMsg.Format("WAAS: %s (%s)", (ackCmd[11] & 0x01) ? "Enable" : "Disable",
        (ackCmd[12] & 0x01) ? "Modify default PRN" : "Default search PRN");
		  add_msgtolist(strMsg);
		  if(ackCmd[12] & 0x01)
      {
        CString txt;
        const int MaxTableSize = 3;
        strMsg.Format("WAAS search PRN (%d): ", ackCmd[13]);
        for(int i = 0; i < MaxTableSize && i < ackCmd[13]; ++i)
        {
          if(ackCmd[14 + i] != 0)
          {
            txt.Format("%d", ackCmd[14 + i]);
            if(i != 0)
            {
              strMsg += ", ";
            }
            strMsg += txt;
          }
        }
		    add_msgtolist(strMsg);
      }

		  strMsg.Format("EGNOS: %s(%s)", ((ackCmd[11] & 0x02) ? "Enable" : "Disable"),
        (ackCmd[12] & 0x02) ? "Modify default PRN" : "Default search PRN");
		  add_msgtolist(strMsg);
		  if(ackCmd[12] & 0x02)
      {
        CString txt;
        strMsg.Format("EGNOS search PRN (%d): ", ackCmd[17]);
        for(int i = 0; i < 3 && i < ackCmd[17]; ++i)
        {
          if(ackCmd[14 + i] != 0)
          {
            txt.Format("%d", ackCmd[18 + i]);
            if(i != 0)
            {
              strMsg += ", ";
            }
            strMsg += txt;
          }
        }
		    add_msgtolist(strMsg);
      }

		  strMsg.Format("MSAS: %s (%s)", ((ackCmd[11] & 0x04) ? "Enable" : "Disable"),
        (ackCmd[12] & 0x04) ? "Modify default PRN" : "Default search PRN");
		  add_msgtolist(strMsg);
		  if(ackCmd[12] & 0x04)
      {
        CString txt;
        strMsg.Format("MSAS search PRN (%d): ", ackCmd[21]);
        for(int i = 0; i < 3 && i < ackCmd[21]; ++i)
        {
          if(ackCmd[14 + i] != 0)
          {
            txt.Format("%d", ackCmd[22 + i]);
            if(i != 0)
            {
              strMsg += ", ";
            }
            strMsg += txt;
          }
        }
		    add_msgtolist(strMsg);
      }
      
      strMsg.Format("GAGAN: %s (%s)", ((ackCmd[11] & 0x08) ? "Enable" : "Disable"),
        (ackCmd[12] & 0x08) ? "Modify default PRN" : "Default search PRN");
		  add_msgtolist(strMsg);
		  if(ackCmd[12] & 0x08)
      {
        CString txt;
        strMsg.Format("GAGAN search PRN (%d): ", ackCmd[25]);
        for(int i = 0; i < 3 && i < ackCmd[25]; ++i)
        {
          if(ackCmd[14 + i] != 0)
          {
            txt.Format("%d", ackCmd[26 + i]);
            if(i != 0)
            {
              strMsg += ", ";
            }
            strMsg += txt;
          }
        }
		    add_msgtolist(strMsg);
      }
	  }
  }
  else
  { //New Style
    int idx = 5;

	  strMsg.Format("SBAS system: %s", ((ackCmd[++idx]) ? "Enable" : "Disable"));
	  add_msgtolist(strMsg);
    U08 ranging = ackCmd[++idx];
	  if(ranging == 0)
	  {
		  strMsg.Format("Ranging: %s", "Disable");
	  }
	  else if(ranging == 1)
	  {
		  strMsg.Format("Ranging: %s", "Enable");
	  }
	  else
	  {
		  strMsg.Format("Ranging: %s", "Auto");
	  }
	  add_msgtolist(strMsg);
	  strMsg.Format("Ranking URA Mask: %d", ackCmd[++idx]);
	  add_msgtolist(strMsg);
	  strMsg.Format("Correction: %s", ((ackCmd[++idx]) ? "Enable" : "Disable"));
	  add_msgtolist(strMsg);
	  strMsg.Format("Number of tracking channels: %d", ackCmd[++idx]);
	  add_msgtolist(strMsg);

    U08 enableMask = ackCmd[++idx]; //11
  #if(!NEW_SBAS2)
    U08 userMask = ackCmd[++idx]; //12
  #endif
	  strMsg.Format("All: %s", ((enableMask & 0x80) ? "Enable" : "Disable"));
	  add_msgtolist(strMsg);	
    
    const int MaxTableSize = 3;
    CString txt;
	  if(!(enableMask & 0x80))
	  {
		  strMsg.Format("WAAS: %s", (enableMask & 0x01) ? "Enable" : "Disable");
		  add_msgtolist(strMsg);
      if(enableMask & 0x01)
      {
        strMsg.Format("WAAS search PRN(%d): ", ackCmd[idx + 1]);
        for(int i = 0; i < MaxTableSize && i < ackCmd[idx + 1]; ++i)
        {
          if(ackCmd[idx + 2 + i] != 0)
          {
            txt.Format("%d", ackCmd[idx + 2 + i]);
            if(i != 0)
            {
              strMsg += ", ";
            }
            strMsg += txt;
          }
        }
	      add_msgtolist(strMsg);
      }

		  strMsg.Format("EGNOS: %s", (enableMask & 0x02) ? "Enable" : "Disable");
      add_msgtolist(strMsg);
      if(enableMask & 0x02)
      {
        strMsg.Format("EGNOS search PRN(%d): ", ackCmd[idx + 5]);
        for(int i = 0; i < 3 && i < ackCmd[idx + 5]; ++i)
        {
          if(ackCmd[idx + 6 + i] != 0)
          {
            txt.Format("%d", ackCmd[idx + 6 + i]);
            if(i != 0)
            {
              strMsg += ", ";
            }
            strMsg += txt;
          }
        }
	      add_msgtolist(strMsg);
      }
    
		  strMsg.Format("MSAS: %s", (enableMask & 0x04) ? "Enable" : "Disable");
      add_msgtolist(strMsg);
      if(enableMask & 0x04)
      {
        strMsg.Format("MSAS search PRN(%d): ", ackCmd[idx + 9]);
        for(int i = 0; i < 3 && i < ackCmd[idx + 9]; ++i)
        {
          if(ackCmd[idx + 10 + i] != 0)
          {
            txt.Format("%d", ackCmd[idx + 10 + i]);
            if(i != 0)
            {
              strMsg += ", ";
            }
            strMsg += txt;
          }
        }
	      add_msgtolist(strMsg);
      }
      
 		  strMsg.Format("GAGAN: %s", (enableMask & 0x08) ? "Enable" : "Disable");
		  add_msgtolist(strMsg);
      if(enableMask & 0x08)
      {
        strMsg.Format("GAGAN search PRN(%d): ", ackCmd[idx + 13]);
        for(int i = 0; i < 3 && i < ackCmd[idx + 13]; ++i)
        {
          if(ackCmd[idx + 14 + i] != 0)
          {
            txt.Format("%d", ackCmd[idx + 14 + i]);
            if(i != 0)
            {
              strMsg += ", ";
            }
            strMsg += txt;
          }
        }
	      add_msgtolist(strMsg);
      }
	  }
  }
	return Ack;
}

CGPSDlg::CmdErrorCode CGPSDlg::QuerySbasDefault(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QuerySbasDefaultCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QuerySbasDefaultCmd].cmdId);
	cmd.SetU08(2, cmdTable[QuerySbasDefaultCmd].cmdSubId);

	BinaryData ackCmd;
  CmdErrorCode err = ExcuteBinaryCommand(QuerySbasDefaultCmd, &cmd, &ackCmd, (nMode == Display) ? g_setting.GetLongTimeout() : g_setting.GetShortTimeout());
	if(err != Ack)
	{
    return err;
  }

  if(Return == nMode)
	{	
    *((BinaryData*)outputData) = ackCmd;
		return err;
  }

	CString strMsg = "Query SBAS Default successfully";
	add_msgtolist(strMsg);

  CString txt;
  const int MaxTableSize = 3;
  strMsg.Format("WAAS default PRN (%d): ", ackCmd[6]);
  for(int i = 0; i < MaxTableSize; ++i)
  {
    txt.Format("%d", ackCmd[7 + i]);
    if(i != 0)
    {
      strMsg += ", ";
    }
    strMsg += txt;
  }
  add_msgtolist(strMsg);

  strMsg.Format("EGNOS default PRN (%d): ", ackCmd[10]);
  for(int i = 0; i < MaxTableSize; ++i)
  {
    txt.Format("%d", ackCmd[11 + i]);
    if(i != 0)
    {
      strMsg += ", ";
    }
    strMsg += txt;
  }
  add_msgtolist(strMsg);

  strMsg.Format("MSAS default PRN (%d): ", ackCmd[14]);
  for(int i = 0; i < MaxTableSize; ++i)
  {
    txt.Format("%d", ackCmd[15 + i]);
    if(i != 0)
    {
      strMsg += ", ";
    }
    strMsg += txt;
  }
  add_msgtolist(strMsg);

  strMsg.Format("GAGAN default PRN (%d): ", ackCmd[18]);
  for(int i = 0; i < MaxTableSize; ++i)
  {
    txt.Format("%d", ackCmd[19 + i]);
    if(i != 0)
    {
      strMsg += ", ";
    }
    strMsg += txt;
  }
  add_msgtolist(strMsg);
	return Ack;
}

CGPSDlg::CmdErrorCode CGPSDlg::QuerySagps(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QuerySagpsCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QuerySagpsCmd].cmdId);
	cmd.SetU08(2, cmdTable[QuerySagpsCmd].cmdSubId);

	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(QuerySagpsCmd, &cmd, &ackCmd))
	{
		if(nMode==Return)
		{
			*((U08*)outputData) = ackCmd[6];
			return Ack;
		}

		CString strMsg = "Query SAEE successfully";
		add_msgtolist(strMsg);
		strMsg = "SAEE System: ";
		if(0==(ackCmd[6]))
		{
			strMsg += "Default";
		}
		else if(1==(ackCmd[6]))
		{
			strMsg += "Enable";
		}
		else if(2==(ackCmd[6]))
		{
			strMsg += "Disable";
		}
		add_msgtolist(strMsg);
	}
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryQzss(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryQzssCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryQzssCmd].cmdId);
	cmd.SetU08(2, cmdTable[QueryQzssCmd].cmdSubId);

	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(QueryQzssCmd, &cmd, &ackCmd))
	{
		CString strMsg = "Query QZSS successfully";
		add_msgtolist(strMsg);
		strMsg.Format("QZSS System: %s", ((ackCmd[6]) ? "Enable" : "Disable"));
		add_msgtolist(strMsg);
		strMsg.Format("Number of Tracking Channels: %d", ackCmd[7]);
		add_msgtolist(strMsg);
	}
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryDgps(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryDgpsCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryDgpsCmd].cmdId);
	cmd.SetU08(2, cmdTable[QueryDgpsCmd].cmdSubId);

	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(QueryDgpsCmd, &cmd, &ackCmd))
	{
		CString strMsg = "Query DGPS successfully";
		add_msgtolist(strMsg);
		if(ackCmd[6] <= 1)
		{
			strMsg.Format("DGPS Status: %s", ((ackCmd[6]) ? "Enable" : "Disable"));
		}
		else
		{
			strMsg.Format("DGPS Status: %d", ackCmd[6]);
		}
		add_msgtolist(strMsg);
		strMsg.Format("Overdue seconds: %d", MAKEWORD(ackCmd[8], ackCmd[7]));
		add_msgtolist(strMsg);
	}
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::QuerySmoothMode(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QuerySmoothModeCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QuerySmoothModeCmd].cmdId);
	cmd.SetU08(2, cmdTable[QuerySmoothModeCmd].cmdSubId);

	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(QuerySmoothModeCmd, &cmd, &ackCmd))
	{
		CString strMsg = "Query Carrier Smooth Mode successfully";
		add_msgtolist(strMsg);
		if(ackCmd[6] <= 1)
		{
			strMsg.Format("Mode: %s", ((ackCmd[6]) ? "Enable" : "Disable"));
		}
		else
		{
			strMsg.Format("Mode: %d", ackCmd[6]);
		}
		add_msgtolist(strMsg);
	}
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryTimeStamping(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryTimeStampingCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryTimeStampingCmd].cmdId);
	cmd.SetU08(2, cmdTable[QueryTimeStampingCmd].cmdSubId);

	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(QueryTimeStampingCmd, &cmd, &ackCmd))
	{
		CString strMsg = "Query Time Stamping successfully";
		add_msgtolist(strMsg);
		if(ackCmd[6] <= 1)
		{
			strMsg.Format("Time Stamping %s", ((ackCmd[6]) ? "Enable" : "Disable"));
		}
		else
		{
			strMsg.Format("Mode: %d", ackCmd[6]);
		}
		add_msgtolist(strMsg);

		if(ackCmd[7] <= 1)
		{
			strMsg.Format("Tigger Mode: %s", ((ackCmd[7]) ? "Falling Edge" : "Rising Edge"));
		}
		else
		{
			strMsg.Format("Tigger Mode: %d", ackCmd[7]);
		}
		add_msgtolist(strMsg);

		if(ackCmd[8] <= 1)
		{
			strMsg.Format("Time Valid: %s", ((ackCmd[8]) ? "Valid" : "Invalid"));
		}
		else
		{
			strMsg.Format("Time Valid: %d", ackCmd[8]);
		}
		add_msgtolist(strMsg);
		
		strMsg.Format("Tigger Counter: %d", MAKEWORD(ackCmd[10], ackCmd[9]));
		add_msgtolist(strMsg);
		strMsg.Format("Week Number: %d", MAKEWORD(ackCmd[12], ackCmd[11]));
		add_msgtolist(strMsg);
		strMsg.Format("Time of Week: %d", MAKELONG(MAKEWORD(ackCmd[16], ackCmd[15]), MAKEWORD(ackCmd[14], ackCmd[13])));
		add_msgtolist(strMsg);
		strMsg.Format("Sub Time of Week: %d", MAKELONG(MAKEWORD(ackCmd[20], ackCmd[19]), MAKEWORD(ackCmd[18], ackCmd[17])));
		add_msgtolist(strMsg);
	}
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryGpsTime(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryGpsTimeCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryGpsTimeCmd].cmdId);
	cmd.SetU08(2, cmdTable[QueryGpsTimeCmd].cmdSubId);

	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(QueryGpsTimeCmd, &cmd, &ackCmd))
	{
		CString strMsg = "Query GPS time successfully";
		add_msgtolist(strMsg);

		strMsg.Format("Time of week %d", MAKELONG(MAKEWORD(ackCmd[9], ackCmd[8]), MAKEWORD(ackCmd[7], ackCmd[6])));
		add_msgtolist(strMsg);

		strMsg.Format("Sub time of week %d", MAKELONG(MAKEWORD(ackCmd[13], ackCmd[12]), MAKEWORD(ackCmd[11], ackCmd[10])));
		add_msgtolist(strMsg);

		strMsg.Format("Week number %d", MAKEWORD(ackCmd[15], ackCmd[14]));
		add_msgtolist(strMsg);

		strMsg.Format("Default leap seconds %d", ackCmd[16]);
		add_msgtolist(strMsg);

		strMsg.Format("Current leap seconds %d", ackCmd[17]);
		add_msgtolist(strMsg);

		strMsg.Format("Valid %d", ackCmd[18]);
		add_msgtolist(strMsg);
	}
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryNoisePowerControl(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryNoisePowerControlCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryNoisePowerControlCmd].cmdId);
	cmd.SetU08(2, cmdTable[QueryNoisePowerControlCmd].cmdSubId);

	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(QueryNoisePowerControlCmd, &cmd, &ackCmd))
	{
		CString strMsg;
		strMsg = "Query Noise Power Control successfully";
		add_msgtolist(strMsg);
		strMsg.Format("Noise Power Control: %s", ((ackCmd[6]) ? "Dynamic" : "Static"));
		add_msgtolist(strMsg);

		WORD ackSize = MAKEWORD(ackCmd[3], ackCmd[2]);
		if(ackSize == 8)
		{
			strMsg.Format("Default Noise Power Control: %d(%s)", ackCmd[7], (ackCmd[7]) ? "external" : "defalut");
			add_msgtolist(strMsg);
			U32 data = MAKELONG(MAKEWORD(ackCmd[11], ackCmd[10]), MAKEWORD(ackCmd[9], ackCmd[8]));
			strMsg.Format("External Noise Power Value: %u", data);
			add_msgtolist(strMsg);
		};
	}
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryInterferenceDetectControl(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryInterferenceDetectControlCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryInterferenceDetectControlCmd].cmdId);
	cmd.SetU08(2, cmdTable[QueryInterferenceDetectControlCmd].cmdSubId);

	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(QueryInterferenceDetectControlCmd, &cmd, &ackCmd))
	{
		CString strMsg = "Query Interference Detect Control successfully";
		add_msgtolist(strMsg);
		strMsg.Format("Interference Control Detect: %s", ((ackCmd[6]) ? "Enable" : "Disable"));
		add_msgtolist(strMsg);
		strMsg.Format("Interference Status: ");
		if(0==ackCmd[7])
		{
			strMsg.Append("Unknown");
		}
		else if(1==ackCmd[7])
		{
			strMsg.Append("No Interference");
		}
		else if(2==ackCmd[7])
		{
			strMsg.Append("Lite");
		}
		else 
		{
			strMsg.Append("Critical");
		}
		add_msgtolist(strMsg);
	}
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryNmeaBinaryOutputDestination(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryNmeaBinaryOutputDestinationCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryNmeaBinaryOutputDestinationCmd].cmdId);
	cmd.SetU08(2, cmdTable[QueryNmeaBinaryOutputDestinationCmd].cmdSubId);

	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(QueryNmeaBinaryOutputDestinationCmd, &cmd, &ackCmd))
	{
		CString strMsg = "Query NMEA Binary Output Dest. successfully";
		add_msgtolist(strMsg);
		if(ackCmd[7] & 0x0001)
		{
			add_msgtolist("UART Port");
		}
		if(ackCmd[7] & 0x0002)
		{
			add_msgtolist("SPI Slave Port");
		}
		if(ackCmd[7] & 0x0004)
		{
			add_msgtolist("I2C Slave Port");
		}
	}
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryParameterSearchEngineNumber(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryParameterSearchEngineNumberCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryParameterSearchEngineNumberCmd].cmdId);
	cmd.SetU08(2, cmdTable[QueryParameterSearchEngineNumberCmd].cmdSubId);

	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(QueryParameterSearchEngineNumberCmd, &cmd, &ackCmd))
	{
		CString strMsg = "Query Param. Search Engine Num. successfully";
		add_msgtolist(strMsg);
		strMsg.Format("Search Engine Mode: ");
		if(0==ackCmd[6])
		{
			strMsg.Append("Default");
		}
		else if(1==ackCmd[6])
		{
			strMsg.Append("Low");
		}
		else if(2==ackCmd[6])
		{
			strMsg.Append("Middle");
		}
		else if(3==ackCmd[6])
		{
			strMsg.Append("High");
		}
		else if(4==ackCmd[6])
		{
			strMsg.Append("Full");
		}
		add_msgtolist(strMsg);
	}
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryGnssKnumberSlotCnr2(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryGnssKnumberSlotCnr2Cmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryGnssKnumberSlotCnr2Cmd].cmdId);
	cmd.SetU08(2, cmdTable[QueryGnssKnumberSlotCnr2Cmd].cmdSubId);

	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(QueryGnssKnumberSlotCnr2Cmd, &cmd, &ackCmd))
	{
		CString strMsg = "Query GLONASS K-Num, Slot, CNR successfully";
		add_msgtolist(strMsg);
		for(int i=0; i<ackCmd[6]; ++i)
		{
			strMsg.Format("K-Num=%d, Slot=%d, CNR=%d", (int)(char)ackCmd[7+i*3], ackCmd[8+i*3], ackCmd[9+i*3]);
			add_msgtolist(strMsg);
		}
	}
	return Timeout;
}
/*
CGPSDlg::CmdErrorCode CGPSDlg::QueryAgpsStatus(CmdExeMode nMode, void* outputData)
{
	SYSTEMTIME	now; 
	GetSystemTime(&now);

	BinaryCommand cmd(cmdTable[QueryAgpsStatusCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryAgpsStatusCmd].cmdId);
	cmd.SetU08(2, HIBYTE(now.wYear));
	cmd.SetU08(3, LOBYTE(now.wYear));
	cmd.SetU08(4, (U08)now.wMonth);
	cmd.SetU08(5, (U08)now.wDay);
	cmd.SetU08(6, (U08)now.wHour);
	cmd.SetU08(7, (U08)now.wMinute);
	cmd.SetU08(8, (U08)now.wSecond);

	BinaryData ackCmd;
	CmdErrorCode ret = ExcuteBinaryCommand(QueryAgpsStatusCmd, &cmd, &ackCmd);
	if(Return==nMode)
	{
		return ret;
	}

	if(!ret)
	{
		CString strMsg = "---------  AGPS Status  ---------";
		add_msgtolist(strMsg);
		strMsg.Format("Aiding days: %d days %d hours", MAKEWORD(ackCmd[5], ackCmd[6]) / 24, MAKEWORD(ackCmd[5], ackCmd[6]) % 24);
		add_msgtolist(strMsg);
		strMsg.Format("AGPS: %s", (ackCmd[7]) ? "Enable" : "Disable");
		add_msgtolist(strMsg);
	}
	return ret;
}
*/
CGPSDlg::CmdErrorCode CGPSDlg::QueryGnssNmeaTalkId(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryGnssNmeaTalkIdCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryGnssNmeaTalkIdCmd].cmdId);

	BinaryData ackCmd;
  CmdErrorCode err = ExcuteBinaryCommand(QueryGnssNmeaTalkIdCmd, &cmd, &ackCmd);
	if(err != Ack)
	{
		return err;
	}

	if(Return == nMode)
	{
    *((int*)outputData) = ackCmd[5];
		return err;
	}

	CString strMsg = "Query Gnss NMEA Talk ID successfully";
	add_msgtolist(strMsg);
	strMsg = "NMEA Talk ID: ";

	if(ackCmd[5] == 0)
	{
		strMsg.Append("GP Mode");
	}
	else if(ackCmd[5] == 1)
	{
		strMsg.Append("GN Mode");
	}
	else if(ackCmd[5] == 2)
	{
		strMsg.Append("Auto Mode");
	}
	add_msgtolist(strMsg);

	return err;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryDataLogStatus(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryDataLogStatusCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryDataLogStatusCmd].cmdId);

	BinaryData ackCmd;
	CmdErrorCode err = ExcuteBinaryCommand(QueryDataLogStatusCmd, &cmd, &ackCmd);
	if(err != Ack)
	{
		return err;
	}

	if(Return == nMode)
	{
    *((BinaryData*)outputData) = ackCmd;
		return err;
	}

	LogFlashInfo1 logInfo = {0};
	logInfo.log_flash_current_prt = MAKELONG(MAKEWORD(ackCmd[5], ackCmd[6]), MAKEWORD(ackCmd[7], ackCmd[8]));
	logInfo.sector_left = MAKEWORD(ackCmd[9], ackCmd[10]);
	logInfo.total_sector = MAKEWORD(ackCmd[11], ackCmd[12]);
	logInfo.max_time = MAKELONG(MAKEWORD(ackCmd[13], ackCmd[14]), MAKEWORD(ackCmd[15], ackCmd[16]));
	logInfo.min_time = MAKELONG(MAKEWORD(ackCmd[17], ackCmd[18]), MAKEWORD(ackCmd[19], ackCmd[20]));
	logInfo.max_distance = MAKELONG(MAKEWORD(ackCmd[21], ackCmd[22]), MAKEWORD(ackCmd[23], ackCmd[24]));
	logInfo.min_distance = MAKELONG(MAKEWORD(ackCmd[25], ackCmd[26]), MAKEWORD(ackCmd[27], ackCmd[28]));
	logInfo.max_speed = MAKELONG(MAKEWORD(ackCmd[29], ackCmd[30]), MAKEWORD(ackCmd[31], ackCmd[32]));
	logInfo.min_speed = MAKELONG(MAKEWORD(ackCmd[33], ackCmd[34]), MAKEWORD(ackCmd[35], ackCmd[36]));
	logInfo.datalog_enable = ackCmd[37];
	logInfo.fifo_mode = ackCmd[38];
#if DATA_POI
	logInfo.poi_entry = MAKELONG(MAKEWORD(ackCmd[39], ackCmd[40]), MAKEWORD(ackCmd[41], ackCmd[42]));
	logInfo.autolog_full = ackCmd[43];
	logInfo.poi_full = ackCmd[44];
#endif

	add_msgtolist("Get Log Status successfully");
	add_msgtolist("---------  Log Status  ---------");
	CString strMsg;
	if(logInfo.total_sector)
	{
		if(logInfo.sector_left == 0x0)
		{
			strMsg = "Sector Full!";
		}
		else if((logInfo.sector_left &0x80000000) != 0)
		{
			logInfo.sector_left = logInfo.sector_left << 1 >> 1;
			strMsg.Format("Circular Sector left: %d / %d", logInfo.sector_left, logInfo.total_sector);
		}
		else if((logInfo.sector_left &0x80000000) == 0)
		{
			logInfo.sector_left = logInfo.sector_left << 1 >> 1;
			strMsg.Format("Sector left: %d / %d", logInfo.sector_left, logInfo.total_sector);
		}
	}
	else
	{
		if(logInfo.sector_left == 0x0)
		{
			strMsg = "Sector Full!";
		}
		else if((logInfo.sector_left &0x80000000) != 0)
		{
			logInfo.sector_left = logInfo.sector_left << 1 >> 1;
			strMsg.Format("Circular Sector left: %d", logInfo.sector_left);
		}
		else if((logInfo.sector_left &0x80000000) == 0)
		{
			logInfo.sector_left = logInfo.sector_left << 1 >> 1;
			strMsg.Format("Sector left: %d", logInfo.sector_left);
		}
	}
	add_msgtolist(strMsg);
	strMsg.Format("max T: %d, min T: %d", logInfo.max_time, logInfo.min_time);
	add_msgtolist(strMsg);
	strMsg.Format("max D: %d, min D: %d", logInfo.max_distance, logInfo.min_distance);
	add_msgtolist(strMsg);
	strMsg.Format("max V: %d, min V: %d", logInfo.max_speed, logInfo.min_speed);
	add_msgtolist(strMsg);
	strMsg.Format("Datalog: %s", (logInfo.datalog_enable) ? "Enable" : "Disable");
	add_msgtolist(strMsg);
	strMsg.Format("FIFO mode: %s", (logInfo.fifo_mode) ? "Circular" : "Oneway");
#if DATA_POI		
	strMsg.Format("POI entry Datalogger1: %d", logInfo.poi_entry);
	add_msgtolist(strMsg);
	strMsg.Format("Autolog full: %d", logInfo.autolog_full);
	add_msgtolist(strMsg);
	strMsg.Format("POIlog full: %d", logInfo.poi_full);
	add_msgtolist(strMsg);
#endif

  return err;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryDataLogStatus2(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryDataLogStatus2Cmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryDataLogStatus2Cmd].cmdId);
  cmd.SetU08(2, cmdTable[QueryDataLogStatus2Cmd].cmdSubId);

	BinaryData ackCmd;
	CmdErrorCode err = ExcuteBinaryCommand(QueryDataLogStatus2Cmd, &cmd, &ackCmd);
	if(err != Ack)
	{
		return err;
	}

	if(Return == nMode)
	{
    *((BinaryData*)outputData) = ackCmd;
		return err;
	}

	U32 log_wr_ptr = ConvertLeU32(ackCmd.Ptr(6));
	//U32 sttus = ConvertLeonU32(ackCmd.Ptr(10));
  U16 total_sector = ConvertLeU16(ackCmd.Ptr(12));
  U16 sector_left = ConvertLeU16(ackCmd.Ptr(10));
	U32 max_time = ConvertLeU32(ackCmd.Ptr(14));
	U32 min_time = ConvertLeU32(ackCmd.Ptr(18));
	U32 max_distance = ConvertLeU32(ackCmd.Ptr(22));
	U32 min_distance = ConvertLeU32(ackCmd.Ptr(26));
	U32 max_speed = ConvertLeU32(ackCmd.Ptr(30));
	U32 min_speed = ConvertLeU32(ackCmd.Ptr(34));
	U08 datalog_enable = ackCmd[38];
	U08 log_fifo_mode = ackCmd[39];

	add_msgtolist("Get New Log Status successfully");

  CString strMsg;
  strMsg.Format("Write ptr: 0x%08X", log_wr_ptr);
  add_msgtolist(strMsg);

	if(sector_left == 0x00)
	{
		strMsg = "Sector Full!";
	}
	else
	{
	  if(total_sector)
	  {
      strMsg.Format("Sector left: %d / %d", sector_left, total_sector);
    }
    else
		{
			strMsg.Format("Sector left: %d / ??", sector_left);
		}
	}
	add_msgtolist(strMsg);

	strMsg.Format("max T: %d, min T: %d", max_time, min_time);
	add_msgtolist(strMsg);
	strMsg.Format("max D: %d, min D: %d", max_distance, min_distance);
	add_msgtolist(strMsg);
	strMsg.Format("max V: %d, min V: %d", max_speed, min_speed);
	add_msgtolist(strMsg);
	strMsg.Format("Datalog: %s", (datalog_enable) ? "Enable" : "Disable");
	add_msgtolist(strMsg);
	strMsg.Format("FIFO mode: %s", (log_fifo_mode) ? "Circular" : "Oneway");
	add_msgtolist(strMsg);

  return err;
}

CGPSDlg::CmdErrorCode CGPSDlg::DatalogClear2(CmdExeMode nMode, void* outputData)
{
  if(nMode == Display)
  {
	  if(AfxMessageBox("Clear all DataLog?", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) != IDYES)
	  {
		  return Timeout;
	  }
  }

	BinaryCommand cmd(cmdTable[DatalogClear2Cmd].cmdSize);
	cmd.SetU08(1, cmdTable[DatalogClear2Cmd].cmdId);
  cmd.SetU08(2, cmdTable[DatalogClear2Cmd].cmdSubId);
 	bool ret = CGPSDlg::gpsDlg->ExecuteConfigureCommand(cmd.GetBuffer(), cmd.Size(), "DataLog Clear successfully", false);
  if(!ret)
  {
    AfxMessageBox("Timeout1: GPS device no response.");
  }
  return (ret) ? Ack : Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryDataLogStatusWatch(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryDataLogStatusWatchCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryDataLogStatusWatchCmd].cmdId);
  cmd.SetU08(2, cmdTable[QueryDataLogStatusWatchCmd].cmdSubId);
	cmd.SetU08(3, 0x0C);

	BinaryData ackCmd;
	CmdErrorCode err = ExcuteBinaryCommand(QueryDataLogStatusWatchCmd, &cmd, &ackCmd, (nMode == Display) ? g_setting.GetLongTimeout() : g_setting.GetShortTimeout());
	if(err != Ack)
	{
    return err;
  }

	if(Return == nMode)
	{
    *((BinaryData*)outputData) = ackCmd;
		return err;
	}

	LogFlashInfo1 logInfo = {0};
	logInfo.log_flash_current_prt = MAKELONG(MAKEWORD(ackCmd[7], ackCmd[8]), MAKEWORD(ackCmd[9], ackCmd[10]));
	logInfo.sector_left = MAKEWORD(ackCmd[11], ackCmd[12]);
	logInfo.total_sector = MAKEWORD(ackCmd[13], ackCmd[14]);
	logInfo.max_time = MAKELONG(MAKEWORD(ackCmd[15], ackCmd[16]), MAKEWORD(ackCmd[17], ackCmd[18]));
	logInfo.min_time = MAKELONG(MAKEWORD(ackCmd[19], ackCmd[20]), MAKEWORD(ackCmd[21], ackCmd[22]));
	logInfo.max_distance = MAKELONG(MAKEWORD(ackCmd[23], ackCmd[24]), MAKEWORD(ackCmd[25], ackCmd[26]));
	logInfo.min_distance = MAKELONG(MAKEWORD(ackCmd[27], ackCmd[28]), MAKEWORD(ackCmd[29], ackCmd[30]));
	logInfo.max_speed = MAKELONG(MAKEWORD(ackCmd[31], ackCmd[32]), MAKEWORD(ackCmd[33], ackCmd[34]));
	logInfo.min_speed = MAKELONG(MAKEWORD(ackCmd[35], ackCmd[36]), MAKEWORD(ackCmd[37], ackCmd[38]));
	logInfo.datalog_enable = ackCmd[39];
	logInfo.fifo_mode = ackCmd[40];

	add_msgtolist("Get Log Status successfully");
	add_msgtolist("------ Log Status ------");
	CString strMsg;
	if(logInfo.total_sector)
	{
		if(logInfo.sector_left == 0x0)
		{
			strMsg = "Sector Full!";
		}
		else if((logInfo.sector_left &0x80000000) != 0)
		{
			logInfo.sector_left = logInfo.sector_left << 1 >> 1;
			strMsg.Format("Circular Sector left: %d / %d", logInfo.sector_left, logInfo.total_sector);
		}
		else if((logInfo.sector_left &0x80000000) == 0)
		{
			logInfo.sector_left = logInfo.sector_left << 1 >> 1;
			strMsg.Format("Sector left: %d / %d", logInfo.sector_left, logInfo.total_sector);
		}
	}
	else
	{
		if(logInfo.sector_left == 0x0)
		{
			strMsg = "Sector Full!";
		}
		else if((logInfo.sector_left &0x80000000) != 0)
		{
			logInfo.sector_left = logInfo.sector_left << 1 >> 1;
			strMsg.Format("Circular Sector left: %d", logInfo.sector_left);
		}
		else if((logInfo.sector_left &0x80000000) == 0)
		{
			logInfo.sector_left = logInfo.sector_left << 1 >> 1;
			strMsg.Format("Sector left: %d", logInfo.sector_left);
		}
	}
	add_msgtolist(strMsg);
	strMsg.Format("max T: %d, min T: %d", logInfo.max_time, logInfo.min_time);
	add_msgtolist(strMsg);
	strMsg.Format("max D: %d, min D: %d", logInfo.max_distance, logInfo.min_distance);
	add_msgtolist(strMsg);
	strMsg.Format("max V: %d, min V: %d", logInfo.max_speed, logInfo.min_speed);
	add_msgtolist(strMsg);
	strMsg.Format("Datalog: %s", (logInfo.datalog_enable) ? "Enable" : "Disable");
	add_msgtolist(strMsg);
	strMsg.Format("FIFO mode: %s", (logInfo.fifo_mode) ? "Circular" : "Oneway");

	return err;
}

CGPSDlg::CmdErrorCode CGPSDlg::DatalogClear(CmdExeMode nMode, void* outputData)
{
  if(nMode == Display)
  {
	  if(AfxMessageBox("Clear all DataLog?", MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) != IDYES)
	  {
		  return Timeout;
	  }
  }

	BinaryCommand cmd(cmdTable[DatalogClearCmd].cmdSize);
	cmd.SetU08(1, cmdTable[DatalogClearCmd].cmdId);
 	bool ret = CGPSDlg::gpsDlg->ExecuteConfigureCommand(cmd.GetBuffer(), cmd.Size(), "DataLog Clear successfully", false);
  if(!ret)
  {
    AfxMessageBox("Timeout2: GPS device no response.");
  }
  return (ret) ? Ack : Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::DatalogWatchClear(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryDataLogStatusWatchCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryDataLogStatusWatchCmd].cmdId);

	BinaryData ackCmd;
	CmdErrorCode err = ExcuteBinaryCommand(QueryDataLogStatusWatchCmd, &cmd, &ackCmd, (nMode == Display) ? g_setting.GetLongTimeout() : g_setting.GetShortTimeout());
	if(err != Ack)
	{
    return err;
  }

	if(Return == nMode)
	{
    //*((BinaryData*)outputData) = ackCmd;
		return err;
	}

	add_msgtolist("DataLog Clear successfully");
	return err;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryRegisterx(CmdExeMode nMode, U32 addr, void* outputData)
{	    
	BinaryCommand cmd(cmdTable[QueryRegisterCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryRegisterCmd].cmdId);
	cmd.SetU08(2, HIBYTE(HIWORD(addr)));
	cmd.SetU08(3, LOBYTE(HIWORD(addr)));
	cmd.SetU08(4, HIBYTE(LOWORD(addr)));
	cmd.SetU08(5, LOBYTE(LOWORD(addr)));

	if(NoWait == nMode)
	{
		ExcuteBinaryCommandNoWait(QueryRegisterCmd, &cmd);
		return Ack;
	}

	BinaryData ackCmd;
  CmdErrorCode err = ExcuteBinaryCommand(QueryRegisterCmd, &cmd, &ackCmd);
	if(err != Ack)
	{
		return err;
	}

	U32 data = MAKELONG(MAKEWORD(ackCmd[8], ackCmd[7]), MAKEWORD(ackCmd[6], ackCmd[5]));
	if(Return == nMode)
	{ //Return command data
		*((U32*)outputData) = data;
		return Ack;
	}

	CString strMsg;
	strMsg.Format("Get Register in 0x%08X", m_regAddress);
	add_msgtolist(strMsg);
	strMsg.Format("0x%08X (%d)", data, data);
	add_msgtolist(strMsg);

	return Ack;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryNmeaStringX(CmdExeMode nMode, LPCSTR nmeaStr, void* outputData)
{	    
	BinaryCommand cmd(cmdTable[QueryNmeaStringCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryNmeaStringCmd].cmdId);
  cmd.SetU08(2, cmdTable[QueryNmeaStringCmd].cmdSubId);
	cmd.SetU08(3, nmeaStr[0]);
	cmd.SetU08(4, nmeaStr[1]);
	cmd.SetU08(5, nmeaStr[2]);

	//if(NoWait == nMode)
	//{
	//	ExcuteBinaryCommandNoWait(QueryNmeaStringCmd, &cmd);
	//	return Ack;
	//}

	BinaryData ackCmd;
  CmdErrorCode err = ExcuteBinaryCommand(QueryNmeaStringCmd, &cmd, &ackCmd);
	if(err != Ack)
	{
		return err;
	}

	if(Return == nMode)
	{ //Return command data
		*((BinaryData*)outputData) = ackCmd;
		return Ack;
	}

	CString strMsg;
	strMsg.Format("Query NMEA String Interval successfully");
	add_msgtolist(strMsg);
	strMsg.Format("%c%c%c NMEA Interval: %d second(s)", nmeaStr[0], nmeaStr[1], nmeaStr[2], ackCmd[6]);
	add_msgtolist(strMsg);

	return Ack;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryRegister(CmdExeMode nMode, void* outputData)
{
  return QueryRegisterx(nMode, m_regAddress, outputData);
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryRegister16x(CmdExeMode nMode, U32 addr, void* outputData)
{	    
	BinaryCommand cmd(cmdTable[QueryRegisterCmd16].cmdSize);
	cmd.SetU08(1, cmdTable[QueryRegisterCmd16].cmdId);
	cmd.SetU08(2, HIBYTE(HIWORD(addr)));
	cmd.SetU08(3, LOBYTE(HIWORD(addr)));
	cmd.SetU08(4, HIBYTE(LOWORD(addr)));
	cmd.SetU08(5, LOBYTE(LOWORD(addr)));

	BinaryData ackCmd;
  CmdErrorCode err = ExcuteBinaryCommand(QueryRegisterCmd16, &cmd, &ackCmd);
	if(err != Ack)
	{
		return err;
	}
  
  U32 data = MAKELONG(MAKEWORD(ackCmd[8], ackCmd[7]), MAKEWORD(ackCmd[6], ackCmd[5]));
	if(Return == nMode)
	{	//Return command data
		*((U32*)outputData) = data;
  	return Ack;
	} 
	
  CString strMsg;
	strMsg.Format("Get 16-IO Register in 0x%08X", m_regAddress);
	add_msgtolist(strMsg);
	strMsg.Format("0x%08X (%d)", data, data);
	add_msgtolist(strMsg);

  return Ack;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryRegister16(CmdExeMode nMode, void* outputData)
{
  return QueryRegister16x(nMode, m_regAddress, outputData);
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryGetGpsAlmanac(CmdExeMode nMode, void* outputData)
{	    
	BinaryCommand cmd(cmdTable[GetGpAlmanacCmd].cmdSize);
	cmd.SetU08(1, cmdTable[GetGpAlmanacCmd].cmdId);
	cmd.SetU08(2, *((U08*)(outputData)));

	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(GetGpAlmanacCmd, &cmd, &ackCmd))
	{
		CString strMsg;
    strMsg.Format("Query GPS almanac #%d successfully", *((U08*)(outputData)));
		add_msgtolist(strMsg);

    strMsg.Format("A:%f", ConvertLeonFloat(ackCmd.Ptr(8)));
		add_msgtolist(strMsg);
    strMsg.Format("Omega_0:%f", ConvertLeonFloat(ackCmd.Ptr(12)));
		add_msgtolist(strMsg);
    strMsg.Format("omega:%f", ConvertLeonFloat(ackCmd.Ptr(16)));
		add_msgtolist(strMsg);
    strMsg.Format("M0,:%f", ConvertLeonFloat(ackCmd.Ptr(20)));
		add_msgtolist(strMsg);
    strMsg.Format("e:%f", ConvertLeonFloat(ackCmd.Ptr(24)));
		add_msgtolist(strMsg);
    strMsg.Format("i0:%f", ConvertLeonFloat(ackCmd.Ptr(28)));
		add_msgtolist(strMsg);
    strMsg.Format("Omega_dot:%f", ConvertLeonFloat(ackCmd.Ptr(32)));
		add_msgtolist(strMsg);
    strMsg.Format("af0:%f", ConvertLeonFloat(ackCmd.Ptr(36)));
		add_msgtolist(strMsg);
    strMsg.Format("af1:%f", ConvertLeonFloat(ackCmd.Ptr(40)));
		add_msgtolist(strMsg);
    strMsg.Format("toa:%d", (S32)MAKELONG(MAKEWORD(ackCmd[47], ackCmd[46]), MAKEWORD(ackCmd[45], ackCmd[44])));
		add_msgtolist(strMsg);
    strMsg.Format("prn:%d", (S16)MAKEWORD(ackCmd[49], ackCmd[48]));
		add_msgtolist(strMsg);
    strMsg.Format("wna:%d", (S16)MAKEWORD(ackCmd[51], ackCmd[50]));
		add_msgtolist(strMsg);
    strMsg.Format("valid:%d", (S16)MAKEWORD(ackCmd[53], ackCmd[52]));
		add_msgtolist(strMsg);
    strMsg.Format("health:%d", ackCmd[54]);
		add_msgtolist(strMsg);
  }
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryGetBeidouAlmanac(CmdExeMode nMode, void* outputData)
{	    
	BinaryCommand cmd(cmdTable[GetBdAlmanacCmd].cmdSize);
	cmd.SetU08(1, cmdTable[GetBdAlmanacCmd].cmdId);
	cmd.SetU08(2, cmdTable[GetBdAlmanacCmd].cmdSubId);
	cmd.SetU08(3, *((U08*)(outputData)));

	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(GetBdAlmanacCmd, &cmd, &ackCmd))
	{
		CString strMsg;
    strMsg.Format("Query Beidou almanac #%d successfully", *((U08*)(outputData)));
		add_msgtolist(strMsg);

    strMsg.Format("A:%f", ConvertLeonFloat(ackCmd.Ptr(9)));
		add_msgtolist(strMsg);
    strMsg.Format("Omega_0:%f", ConvertLeonFloat(ackCmd.Ptr(13)));
		add_msgtolist(strMsg);
    strMsg.Format("omega:%f", ConvertLeonFloat(ackCmd.Ptr(17)));
		add_msgtolist(strMsg);
    strMsg.Format("M0,:%f", ConvertLeonFloat(ackCmd.Ptr(21)));
		add_msgtolist(strMsg);
    strMsg.Format("e:%f", ConvertLeonFloat(ackCmd.Ptr(25)));
		add_msgtolist(strMsg);
    strMsg.Format("i0:%f", ConvertLeonFloat(ackCmd.Ptr(29)));
		add_msgtolist(strMsg);
    strMsg.Format("Omega_dot:%f", ConvertLeonFloat(ackCmd.Ptr(33)));
		add_msgtolist(strMsg);
    strMsg.Format("af0:%f", ConvertLeonFloat(ackCmd.Ptr(37)));
		add_msgtolist(strMsg);
    strMsg.Format("af1:%f", ConvertLeonFloat(ackCmd.Ptr(41)));
		add_msgtolist(strMsg);
    strMsg.Format("toa:%d", (S32)MAKELONG(MAKEWORD(ackCmd[48], ackCmd[47]), MAKEWORD(ackCmd[46], ackCmd[45])));
		add_msgtolist(strMsg);
    strMsg.Format("prn:%d", (S16)MAKEWORD(ackCmd[50], ackCmd[49]));
		add_msgtolist(strMsg);
    strMsg.Format("wna:%d", (S16)MAKEWORD(ackCmd[52], ackCmd[51]));
		add_msgtolist(strMsg);
    strMsg.Format("valid:%d", (S16)MAKEWORD(ackCmd[54], ackCmd[53]));
		add_msgtolist(strMsg);
    strMsg.Format("health:%d", ackCmd[55]);
		add_msgtolist(strMsg);
  }
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryPositionFixNavigationMask(CmdExeMode nMode, void* outputData)
{	    
	BinaryCommand cmd(cmdTable[QueryPositionFixNavigationMaskCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryPositionFixNavigationMaskCmd].cmdId);
	cmd.SetU08(2, cmdTable[QueryPositionFixNavigationMaskCmd].cmdSubId);

	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(QueryPositionFixNavigationMaskCmd, &cmd, &ackCmd))
	{
		CString strMsg = "Query Position Fix Navigation Mask successfully";
		add_msgtolist(strMsg);
		strMsg.Format("First fix navigation mask: %s", (ackCmd[6]) ? "2D" : "3D");
		add_msgtolist(strMsg);
		strMsg.Format("Subsequent fix navigation mask: %s", (ackCmd[7]) ? "2D" : "3D");
		add_msgtolist(strMsg);
	}
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryNavigationModeV8(CmdExeMode nMode, void* outputData)
{	    
	BinaryCommand cmd(cmdTable[QueryNavigationModeV8Cmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryNavigationModeV8Cmd].cmdId);
	cmd.SetU08(2, cmdTable[QueryNavigationModeV8Cmd].cmdSubId);

	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(QueryNavigationModeV8Cmd, &cmd, &ackCmd))
	{
		CString strMsg = "Query Navigation Mode successfully";
		add_msgtolist(strMsg);
		switch(ackCmd[6])
		{
		case 0:
			strMsg.SetString("Navigation Mode: Auto");
			break;
		case 1:
			strMsg.SetString("Navigation Mode: Pedestrian");
			break;
		case 2:
			strMsg.SetString("Navigation Mode: Car");
			break;
		case 3:
			strMsg.SetString("Navigation Mode: Marine");
			break;
		case 4:
			strMsg.SetString("Navigation Mode: Balloon");
			break;
		case 5:
			strMsg.SetString("Navigation Mode: Airborne");
			break;
		case 6:
			strMsg.SetString("Navigation Mode: Surveying and mapping");
			break;
		case 7:
			strMsg.SetString("Navigation Mode: Quadcopter");
			break;
		case 8:
			strMsg.SetString("Navigation Mode: Mower");
			break;
		default:
			strMsg.Format("Navigation Mode: %d", ackCmd[6]);
			break;
		}
		add_msgtolist(strMsg);
	}
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryGnssBootStatus(CmdExeMode nMode, void* outputData)
{	    
	BinaryCommand cmd(cmdTable[QueryGnssBootStatusCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryGnssBootStatusCmd].cmdId);
	cmd.SetU08(2, cmdTable[QueryGnssBootStatusCmd].cmdSubId);
	
	if(NoWait == nMode)
	{
		ExcuteBinaryCommandNoWait(QueryGnssBootStatusCmd, &cmd);
		return Timeout;
	}

	BinaryData ackCmd;
  CmdErrorCode err = ExcuteBinaryCommand(QueryGnssBootStatusCmd, &cmd, &ackCmd);
	if(Ack != err)
	{
    return err;
  }

	m_bootInfo = ackCmd;
	if(nMode == Return)
	{
		*((U32*)outputData) = MAKEWORD(ackCmd[7], ackCmd[6]);
		return Ack;
	}

	CString strMsg = "Get GNSS ROM Boot Status successfully";
	add_msgtolist(strMsg);
	switch(ackCmd[6])
	{
	case 0:
		strMsg.SetString("Status: Boot flash OK");
		break;
	case 1:
		strMsg.SetString("Status: Fail over to boot from ROM");
		break;
	default:
		strMsg.Format("Status: %d", ackCmd[6]);
		break;
	}
	add_msgtolist(strMsg);
	switch(ackCmd[7])
	{
	case 0:
		strMsg.SetString("Flash Type: ROM");
		break;
	case 1:
		strMsg.SetString("Flash Type: QSPI Flash Type 1");		//Winbond-type 
		break;
	case 2:
		strMsg.SetString("Flash Type: QSPI Flash Type 2");		// EON-type 
		break;
	case 4:
		strMsg.SetString("Flash Type: Parallel Flash");
		break;
	default:
		strMsg.Format("Flash Type: %d", ackCmd[7]);
		break;
	}
	add_msgtolist(strMsg);	
	return Ack;
}

/*
CGPSDlg::CmdErrorCode CGPSDlg::QueryDrMultiHz(CmdExeMode nMode, void* outputData)
{	    
	BinaryCommand cmd(cmdTable[QueryDrMultiHzCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryDrMultiHzCmd].cmdId);
	cmd.SetU08(2, cmdTable[QueryDrMultiHzCmd].cmdSubId);

	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(QueryDrMultiHzCmd, &cmd, &ackCmd))
	{
		CString strMsg = "Get DR Multi-Hz successfully";
		strMsg.Format("DR Update Rate = %d Hz", ackCmd[6]);
		add_msgtolist(strMsg);
	}
	return Timeout;
}
//*/

CGPSDlg::CmdErrorCode CGPSDlg::QueryNmeaIntervalV8(CmdExeMode nMode, void* outputData)
{	    
	BinaryCommand cmd(cmdTable[QueryNmeaIntervalV8Cmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryNmeaIntervalV8Cmd].cmdId);
	cmd.SetU08(2, cmdTable[QueryNmeaIntervalV8Cmd].cmdSubId);

	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(QueryNmeaIntervalV8Cmd, &cmd, &ackCmd))
	{
		CString strMsg = "Query NMEA Message Interval successfully";
		add_msgtolist(strMsg);
		if(ackCmd[6])
		{
			strMsg.Format("GGA Interval: %d second(s)", ackCmd[6]);
			add_msgtolist(strMsg);
		}
		if(ackCmd[7])
		{
			strMsg.Format("GSA Interval: %d second(s)", ackCmd[7]);
			add_msgtolist(strMsg);
		}
		if(ackCmd[8])
		{
			strMsg.Format("GSV Interval: %d second(s)", ackCmd[8]);
			add_msgtolist(strMsg);
		}
		if(ackCmd[9])
		{
			strMsg.Format("GLL Interval: %d second(s)", ackCmd[9]);
			add_msgtolist(strMsg);
		}
		if(ackCmd[10])
		{
			strMsg.Format("RMC Interval: %d second(s)", ackCmd[10]);
			add_msgtolist(strMsg);
		}
		if(ackCmd[11])
		{
			strMsg.Format("VTG Interval: %d second(s)", ackCmd[11]);
			add_msgtolist(strMsg);
		}
		if(ackCmd[12])
		{
			strMsg.Format("ZDA Interval: %d second(s)", ackCmd[12]);
			add_msgtolist(strMsg);
		}
		if(ackCmd[13])
		{
			strMsg.Format("GNS Interval: %d second(s)", ackCmd[13]);
			add_msgtolist(strMsg);
		}
		if(ackCmd[14])
		{
			strMsg.Format("GBS Interval: %d second(s)", ackCmd[14]);
			add_msgtolist(strMsg);
		}
		if(ackCmd[15])
		{
			strMsg.Format("GRS Interval: %d second(s)", ackCmd[15]);
			add_msgtolist(strMsg);
		}
		if(ackCmd[16])
		{
			strMsg.Format("DTM Interval: %d second(s)", ackCmd[16]);
			add_msgtolist(strMsg);
		}
		if(ackCmd[17])
		{
			strMsg.Format("GST Interval: %d second(s)", ackCmd[17]);
			add_msgtolist(strMsg);
		}
		if(ackCmd[6]==0 && ackCmd[7]==0 && ackCmd[8]==0 && ackCmd[9]==0 && ackCmd[10]==0 && 
			ackCmd[11]==0 && ackCmd[12]==0 && ackCmd[13]==0 && ackCmd[14]==0 && ackCmd[15]==0 && 
			ackCmd[16]==0 && ackCmd[17]==0)
		{
			strMsg.Format("All message intervals are zero");
			add_msgtolist(strMsg);
		}
	}
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryNmeaInterval2V8(CmdExeMode nMode, void* outputData)
{	    
	BinaryCommand cmd(cmdTable[QueryNmeaInterval2V8Cmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryNmeaInterval2V8Cmd].cmdId);
	cmd.SetU08(2, cmdTable[QueryNmeaInterval2V8Cmd].cmdSubId);
	cmd.SetU08(3, 0x02);

	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(QueryNmeaInterval2V8Cmd, &cmd, &ackCmd))
	{
		CString strMsg = "Query NMEA Message Interval successfully";
		add_msgtolist(strMsg);
		if(ackCmd[7])
		{
			strMsg.Format("GGA Interval: %d second(s)", ackCmd[7]);
			add_msgtolist(strMsg);
		}
		if(ackCmd[8])
		{
			strMsg.Format("GSA Interval: %d second(s)", ackCmd[8]);
			add_msgtolist(strMsg);
		}
		if(ackCmd[9])
		{
			strMsg.Format("GSV Interval: %d second(s)", ackCmd[9]);
			add_msgtolist(strMsg);
		}
		if(ackCmd[10])
		{
			strMsg.Format("GPtps Interval: %d second(s)", ackCmd[10]);
			add_msgtolist(strMsg);
		}
		if(ackCmd[11])
		{
			strMsg.Format("GPanc Interval: %d second(s)", ackCmd[11]);
			add_msgtolist(strMsg);
		}
	}
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryEricssonInterval(CmdExeMode nMode, void* outputData)
{	    
	BinaryCommand cmd(cmdTable[QueryEricssonIntervalCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryEricssonIntervalCmd].cmdId);
	cmd.SetU08(2, cmdTable[QueryEricssonIntervalCmd].cmdSubId);
	cmd.SetU08(3, 0x02);

	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(QueryEricssonIntervalCmd, &cmd, &ackCmd))
	{
		CString strMsg = "Query Ericsson Sentence Interval successfully";
		add_msgtolist(strMsg);
		if(ackCmd[7])
		{
			strMsg.Format("GPppr Interval: %d second(s)", ackCmd[7]);
			add_msgtolist(strMsg);
		}
		if(ackCmd[8])
		{
			strMsg.Format("GPsts Interval: %d second(s)", ackCmd[8]);
			add_msgtolist(strMsg);
		}
		if(ackCmd[9])
		{
			strMsg.Format("GPver Interval: %d second(s)", ackCmd[9]);
			add_msgtolist(strMsg);
		}
		if(ackCmd[10])
		{
			strMsg.Format("GPavp Interval: %d second(s)", ackCmd[10]);
			add_msgtolist(strMsg);
		}
	}
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::QuerySerialNumber(CmdExeMode nMode, void* outputData)
{	    
	BinaryCommand cmd(cmdTable[QuerySerialNumberCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QuerySerialNumberCmd].cmdId);
	cmd.SetU08(2, cmdTable[QuerySerialNumberCmd].cmdSubId);
	cmd.SetU08(3, 0x02);

	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(QuerySerialNumberCmd, &cmd, &ackCmd))
	{
		CString strMsg = "Query Serial Number successfully";
		add_msgtolist(strMsg);
		if(ackCmd[7] > 0)	//Has Serial Number
		{
			strMsg.Format("Serial Number:");
			add_msgtolist(strMsg);

			char* p = strMsg.GetBuffer(ackCmd[7] + 1);
			memcpy(p, ackCmd.GetBuffer(8), ackCmd[7]);
			p[ackCmd[7]] = 0;
			strMsg.ReleaseBuffer();
			add_msgtolist(strMsg);
		}
		else	//No Serial Number
		{
			strMsg.Format("No valid serial number.");
			add_msgtolist(strMsg);
		}
	}
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryGeofenceResult(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryGeofenceResultCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryGeofenceResultCmd].cmdId);
	cmd.SetU08(2, cmdTable[QueryGeofenceResultCmd].cmdSubId);

	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(QueryGeofenceResultCmd, &cmd, &ackCmd))
	{
		CString strMsg;
		strMsg = "Query geofencing result successfully";
		add_msgtolist(strMsg);

		const U08* ptr = ackCmd.Ptr(7);
		double d1 = ConvertLeonDouble(ptr);
		ptr = ackCmd.Ptr(15);
		double d2 = ConvertLeonDouble(ptr);
		if(INVERT_LON_LAT)
		{
			strMsg.Format("Geofencing in %12.9lf, %12.9lf", d1, d2);
		}
		else
		{
			strMsg.Format("Geofencing in %12.9lf, %12.9lf", d2, d1);
		}
		add_msgtolist(strMsg);

		strMsg.Format("Geofencing result: %d", ackCmd[6]);
		add_msgtolist(strMsg);
	}
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryGeofenceResultEx(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryGeofenceResultCmdEx].cmdSize);
	cmd.SetU08(1, cmdTable[QueryGeofenceResultCmdEx].cmdId);
	cmd.SetU08(2, cmdTable[QueryGeofenceResultCmdEx].cmdSubId);

	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(QueryGeofenceResultCmdEx, &cmd, &ackCmd))
	{
		CString strMsg;
		strMsg = "Query geofencing result successfully";
		add_msgtolist(strMsg);

		const U08* ptr = ackCmd.Ptr(10);
		double d1 = ConvertLeonDouble(ptr);
		ptr = ackCmd.Ptr(18);
		double d2 = ConvertLeonDouble(ptr);
		if(INVERT_LON_LAT)
		{
			strMsg.Format("Geofencing in %12.9lf, %12.9lf", d1, d2);
		}
		else
		{
			strMsg.Format("Geofencing in %12.9lf, %12.9lf", d2, d1);
		}
		add_msgtolist(strMsg);

		strMsg.Format("Geofencing result NO.1: %d", ackCmd[6]);
		add_msgtolist(strMsg);
		strMsg.Format("Geofencing result NO.2: %d", ackCmd[7]);
		add_msgtolist(strMsg);
		strMsg.Format("Geofencing result NO.3: %d", ackCmd[8]);
		add_msgtolist(strMsg);
		strMsg.Format("Geofencing result NO.4: %d", ackCmd[9]);
		add_msgtolist(strMsg);
	}
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryGeofenceEx(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryGeofenceCmdEx].cmdSize);
	cmd.SetU08(1, cmdTable[QueryGeofenceCmdEx].cmdId);
	cmd.SetU08(2, cmdTable[QueryGeofenceCmdEx].cmdSubId);
	cmd.SetU08(3, m_nGeofecingNo);

	BinaryData ackCmd;
  CmdErrorCode err = ExcuteBinaryCommand(QueryGeofenceCmdEx, &cmd, &ackCmd, 2000);
	if(err != Ack)
	{
		return err;
	}

	U08 size = ackCmd[7];
  CStringArray* points = (CStringArray*)outputData;

	CString strMsg;
	strMsg.Format("Query geofencing data %d successfully", m_nGeofecingNo);
	add_msgtolist(strMsg);
	
	if(size == 0)
	{
		strMsg.Format("No geofencing data");
		add_msgtolist(strMsg);
	}

	const U08* ptr = ackCmd.Ptr(8);
	for(int i = 0; i < size; ++i)
	{
		U08 temp1[8] = {0};
		U08 temp2[8] = {0};
		for(int j = 0; j < 8; ++j)
		{
			temp1[7 - j] = *ptr++;
		}
		for(int j = 0; j < 8; ++j)
		{
			temp2[7 - j] = *ptr++;
		}
		if(INVERT_LON_LAT)
		{
			strMsg.Format("%12.9lf, %12.9lf", *((double*)temp1), *((double*)temp2));
		}
		else
		{
			strMsg.Format("%12.9lf, %12.9lf", *((double*)temp2), *((double*)temp1));
		}
    if(Return == nMode)
    {
      points->Add(strMsg);
    }
    else
    {
		  add_msgtolist(strMsg);
    }
	}
  return err;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryRtkMode(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryRtkModeCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryRtkModeCmd].cmdId);
	cmd.SetU08(2, cmdTable[QueryRtkModeCmd].cmdSubId);

	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(QueryRtkModeCmd, &cmd, &ackCmd))
	{
		CString strMsg;
		strMsg = "Query RTK mode successfully";
		add_msgtolist(strMsg);

		if(ackCmd[6] == 0)
		{
			strMsg.Format("RTK rover mode");
		}
		else if(ackCmd[6] == 1)
		{
			strMsg.Format("RTK base mode");
		}
    else if(ackCmd[6] == 2)
		{
			strMsg.Format("RTK precisely kinematic base mode");
		}
		add_msgtolist(strMsg);
	}
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryPstmDeviceAddress(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryPstmDeviceAddressCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryPstmDeviceAddressCmd].cmdId);
	cmd.SetU08(2, cmdTable[QueryPstmDeviceAddressCmd].cmdSubId);
	cmd.SetU08(3, 0x02);

	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(QueryPstmDeviceAddressCmd, &cmd, &ackCmd))
	{
		CString strMsg;
		strMsg = "Query PSTM device address successfully";
		add_msgtolist(strMsg);
		strMsg.Format("PSCM device address: %d", ackCmd[7]);
		add_msgtolist(strMsg);
	}
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryPstnLatLonDigits(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryPstnLatLonDigitsCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryPstnLatLonDigitsCmd].cmdId);
	cmd.SetU08(2, cmdTable[QueryPstnLatLonDigitsCmd].cmdSubId);
	cmd.SetU08(3, 0x04);

	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(QueryPstnLatLonDigitsCmd, &cmd, &ackCmd))
	{
		CString strMsg;
		strMsg = "Query PSTM LAT/LON fractional digits successfully";
		add_msgtolist(strMsg);
		strMsg.Format("LAT/LON Fractional Digits: %d", ackCmd[7]);
		add_msgtolist(strMsg);
	}
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryPsti(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryPstiCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryPstiCmd].cmdId);
	cmd.SetU08(2, cmdTable[QueryPstiCmd].cmdSubId);
	cmd.SetU08(3, (U08)m_nPstiNo);

	BinaryData ackCmd;
  CmdErrorCode err = ExcuteBinaryCommand(QueryPstiCmd, &cmd, &ackCmd, 2000);
	if(err != Ack)
	{
		return err;
	}

	if(Return == nMode)
	{	//Return command length
		*((U08*)outputData) = ackCmd[6];
		return err;
	}

	CString strMsg;
  strMsg.Format("Query PSTI%03d Interval successfully", m_nPstiNo);
	add_msgtolist(strMsg);
  strMsg.Format("PSTI%03d Interval: %d", m_nPstiNo, ackCmd[6]);
	add_msgtolist(strMsg);

	return err;
}
/*
CGPSDlg::CmdErrorCode CGPSDlg::QueryPsti032(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryPstiCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryPstiCmd].cmdId);
	cmd.SetU08(2, cmdTable[QueryPstiCmd].cmdSubId);
	cmd.SetU08(3, 32);

	BinaryData ackCmd;
  CmdErrorCode err = ExcuteBinaryCommand(QueryPstiCmd, &cmd, &ackCmd, 2000);
	if(err != Ack)
	{
		return err;
	}

	if(Return == nMode)
	{	//Return command length
		*((U08*)outputData) = ackCmd[6];
		return err;
	}

	CString strMsg;
	strMsg = "Query PSTI032 Interval successfully";
	add_msgtolist(strMsg);
	strMsg.Format("PSTI032 Interval: %d", ackCmd[6]);
	add_msgtolist(strMsg);

	return err;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryPsti033(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryPstiCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryPstiCmd].cmdId);
	cmd.SetU08(2, cmdTable[QueryPstiCmd].cmdSubId);
	cmd.SetU08(3, 33);

	BinaryData ackCmd;
  CmdErrorCode err = ExcuteBinaryCommand(QueryPstiCmd, &cmd, &ackCmd, 2000);
	if(err != Ack)
	{
		return err;
	}

	if(Return == nMode)
	{	//Return command length
		*((U08*)outputData) = ackCmd[6];
		return err;
	}

	CString strMsg;
	strMsg = "Query PSTI033 Interval successfully";
	add_msgtolist(strMsg);
	strMsg.Format("PSTI033 Interval: %d", ackCmd[6]);
	add_msgtolist(strMsg);

	return err;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryPsti(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryPstiCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryPstiCmd].cmdId);
	cmd.SetU08(2, cmdTable[QueryPstiCmd].cmdSubId);
	cmd.SetU08(3, (U08)m_nPstiNo);

	BinaryData ackCmd;
  CmdErrorCode err = ExcuteBinaryCommand(QueryPstiCmd, &cmd, &ackCmd, 2000);
	if(err != Ack)
	{
		return err;
	}

	if(Return == nMode)
	{	//Return command length
		*((U08*)outputData) = ackCmd[6];
		return err;
	}

	CString strMsg;
	strMsg = "Query PSTI067 Interval successfully";
	add_msgtolist(strMsg);
	strMsg.Format("PSTI067 Interval: %d", ackCmd[6]);
	add_msgtolist(strMsg);

	return err;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryPsti004(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryPstiCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryPstiCmd].cmdId);
	cmd.SetU08(2, cmdTable[QueryPstiCmd].cmdSubId);
	cmd.SetU08(3, 4);

	BinaryData ackCmd;
  CmdErrorCode err = ExcuteBinaryCommand(QueryPstiCmd, &cmd, &ackCmd, 2000);
	if(err != Ack)
	{
		return err;
	}

	if(Return == nMode)
	{	//Return command length
		*((U08*)outputData) = ackCmd[6];
		return err;
	}

	CString strMsg;
	strMsg = "Query PSTI004 Interval successfully";
	add_msgtolist(strMsg);
	strMsg.Format("PSTI004 Interval: %d", ackCmd[6]);
	add_msgtolist(strMsg);

	return err;
}
*/
CGPSDlg::CmdErrorCode CGPSDlg::QueryRtkMode2(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryRtkModeCmd2].cmdSize);
	cmd.SetU08(1, cmdTable[QueryRtkModeCmd2].cmdId);
	cmd.SetU08(2, cmdTable[QueryRtkModeCmd2].cmdSubId);

	BinaryData ackCmd;
	CmdErrorCode err = ExcuteBinaryCommand(QueryRtkModeCmd2, &cmd, &ackCmd, (nMode == Display) ? g_setting.GetLongTimeout() : g_setting.GetShortTimeout());
	if(err != Ack)
	{
		return err;
	}

	if(Return == nMode)
	{	//Return command data
    *((BinaryData*)outputData) = ackCmd;
		return err;
	}

	CString strMsg;
	strMsg = "Query RTK mode successfully";
	add_msgtolist(strMsg);

	U16 cmdLen = ConvertLeonU16(ackCmd.Ptr(2));

	UINT8 rtkMode = ackCmd[6];
	if(ackCmd[6] == 0)
	{
		strMsg.Format("RTK rover mode");
	}
	else if(ackCmd[6] == 1)
	{
		strMsg.Format("RTK base mode");
	}
  else if(ackCmd[6] == 2)
	{
		strMsg.Format("RTK precisely kinematic base mode");
	}
	add_msgtolist(strMsg);

	UINT8 rtkOpr = ackCmd[7];
	if(rtkMode == 1)	//base mode
	{
	  strMsg.Format("Saved Operational Function: ");
		switch(rtkOpr)
		{
		case 0:
			strMsg += "Kinematic Mode";
			break;
		case 1:
			strMsg += "Survey Mode";
			break;
		case 2:
			strMsg += "Static Mode";
			break;
    default:
      strMsg.Format("Operational Function: %d", rtkOpr);
      break;
		}
	}
	else if(rtkMode == 0)	//rover mode
	{
	  strMsg.Format("Operational Function: ");
		switch(rtkOpr)
		{
		case 0:
			strMsg += "Normal";
			break;
		case 1:
			strMsg += "Float";
			break;
		case 2:
			strMsg += "Moving base";
			if(cmdLen == 41)
			{	//The length in old style is 37, new is 41. 20160422 changed by Ryan
				add_msgtolist(strMsg);
				strMsg.Format("Baseline length:%f", ConvertLeonFloat(ackCmd.Ptr(36)));
			}
			break;
    default:
      strMsg.Format("Operational Function: %d", rtkOpr);
      break;
		}		
	}
  else if(rtkMode == 2)	//RTK precisely kinematic base mode
  {
    switch(rtkOpr)
    {
    case 0:
      strMsg.Format("Operational Function: Normal");
      break;
    case 1:
      strMsg.Format("Operational Function: Float");
      break;
    default:
      strMsg.Format("Operational Function: %d", rtkOpr);
      break;
    }
  }
	add_msgtolist(strMsg);
	
	if(rtkMode == 0 || rtkMode == 2 )	//rover mode or precisely kinematic base mode
	{
		return Ack;
	}

	strMsg.Format("Run-time Timing Mode: ");
	switch(rtkOpr)
	{
	case 0:
		break;
	case 1:
		strMsg.Format("Saved Survey Length:%u", MAKELONG(MAKEWORD(ackCmd[11], ackCmd[10]), MAKEWORD(ackCmd[9], ackCmd[8])));
		add_msgtolist(strMsg);
		strMsg.Format("Standard Deviation:%u", MAKELONG(MAKEWORD(ackCmd[15], ackCmd[14]), MAKEWORD(ackCmd[13], ackCmd[12])));
		add_msgtolist(strMsg);
		break;
	case 2:
		strMsg.Format("Saved Latitude:%f", ConvertLeonDouble(ackCmd.Ptr(16)));
		add_msgtolist(strMsg);
		strMsg.Format("Saved Longitude:%f", ConvertLeonDouble(ackCmd.Ptr(24)));
		add_msgtolist(strMsg);
		strMsg.Format("Saved Ellipsoidal Height:%f", ConvertLeonFloat(ackCmd.Ptr(32)));
		add_msgtolist(strMsg);
		break;
	}	

	UINT8 timingMode = ackCmd[36];
  strMsg.Format("Run-time Operational Function: ");
	switch(timingMode)
	{
	case 0:
		strMsg += "Kinematic Mode";
		add_msgtolist(strMsg);
		break;
	case 1:
		strMsg += "Survey Mode";
		add_msgtolist(strMsg);
		strMsg.Format("Run-time Survey Length:%u", MAKELONG(MAKEWORD(ackCmd[40], ackCmd[39]), MAKEWORD(ackCmd[38], ackCmd[37])));
		add_msgtolist(strMsg);
		break;
	case 2:
		strMsg += "Static Mode";
		add_msgtolist(strMsg);
		break;
	}	
	return Ack;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryRtkSlaveBaud(CmdExeMode nMode, void* outputData)
{ 
	BinaryCommand cmd(cmdTable[QueryRtkSlaveBaudCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryRtkSlaveBaudCmd].cmdId);
	cmd.SetU08(2, cmdTable[QueryRtkSlaveBaudCmd].cmdSubId);

	BinaryData ackCmd;
	CmdErrorCode err = ExcuteBinaryCommand(QueryRtkSlaveBaudCmd, &cmd, &ackCmd, (nMode == Display) ? g_setting.GetLongTimeout() : g_setting.GetShortTimeout());
	if(err != Ack)
	{
		return err;
	}

	if(Return == nMode)
	{	//Return command data
    *((BinaryData*)outputData) = ackCmd;
		return err;
	}

	CString strMsg;
	strMsg = "Query RTK slave serial port baud rate successfully";
	add_msgtolist(strMsg);
  strMsg.Format("Slave baud rate: %d", g_setting.BaudrateValue(ackCmd[6]));
	add_msgtolist(strMsg);

	return Ack;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryRtkKinematicBaud(CmdExeMode nMode, void* outputData)
{ 
	BinaryCommand cmd(cmdTable[QueryRtkKinematicBaudCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryRtkKinematicBaudCmd].cmdId);
	cmd.SetU08(2, cmdTable[QueryRtkKinematicBaudCmd].cmdSubId);

	BinaryData ackCmd;
	CmdErrorCode err = ExcuteBinaryCommand(QueryRtkKinematicBaudCmd, &cmd, &ackCmd, (nMode == Display) ? g_setting.GetLongTimeout() : g_setting.GetShortTimeout());
	if(err != Ack)
	{
		return err;
	}

	if(Return == nMode)
	{	//Return command data
    *((BinaryData*)outputData) = ackCmd;
		return err;
	}

	CString strMsg;
	strMsg = "Query RTK precisely kinematic base  serial port baud rate successfully";
	add_msgtolist(strMsg);
  strMsg.Format("baud rate: %d", g_setting.BaudrateValue(ackCmd[6]));
	add_msgtolist(strMsg);

	return Ack;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryRtkCpifBias(CmdExeMode nMode, void* outputData)
{ 
	BinaryCommand cmd(cmdTable[QueryRtkCpifBiasCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryRtkCpifBiasCmd].cmdId);
	cmd.SetU08(2, cmdTable[QueryRtkCpifBiasCmd].cmdSubId);

	BinaryData ackCmd;
	CmdErrorCode err = ExcuteBinaryCommand(QueryRtkCpifBiasCmd, &cmd, &ackCmd, (nMode == Display) ? g_setting.GetLongTimeout() : g_setting.GetShortTimeout());
	if(err != Ack)
	{
		return err;
	}

	if(Return == nMode)
	{	//Return command data
    *((BinaryData*)outputData) = ackCmd;
		return err;
	}

	CString strMsg;
	strMsg = "Query RTK GLONASS CPIF bias successfully";
	add_msgtolist(strMsg);
  strMsg = "Mode: ";
  if(ackCmd[6] == 0)
  {
    strMsg += "none(0)";
  }
  else if(ackCmd[6] == 1)
  {
    strMsg += "auto switch(1)";
  }
  else if(ackCmd[6] == 2)
  {
    strMsg += "set manually(2)";
  }
	add_msgtolist(strMsg);
  strMsg = "Frequency Mask: ";
  if((ackCmd[7] & 0x01) != 0)
  {
    strMsg += "L1 ";
  }
  if((ackCmd[7] & 0x02) != 0)
  {
    strMsg += "L2 ";
  }
  add_msgtolist(strMsg);
  strMsg.Format("L1 CPIF Bias: %f", ConvertLeonDouble(ackCmd.Ptr(8)));
  add_msgtolist(strMsg);
  strMsg.Format("L2 CPIF Bias: %f", ConvertLeonDouble(ackCmd.Ptr(16)));
  add_msgtolist(strMsg);
  strMsg.Format("reserved(U08): %d", ackCmd[24]);
  add_msgtolist(strMsg);
  strMsg.Format("reserved(U32): %d", ConvertLeonU32(ackCmd.Ptr(25)));
  add_msgtolist(strMsg);
	return Ack;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryRtkElevationAndCnrMask(CmdExeMode nMode, void* outputData)
{ 
	BinaryCommand cmd(cmdTable[OnQueryRtkElevationAndCnrMaskCmd].cmdSize);
	cmd.SetU08(1, cmdTable[OnQueryRtkElevationAndCnrMaskCmd].cmdId);
	cmd.SetU08(2, cmdTable[OnQueryRtkElevationAndCnrMaskCmd].cmdSubId);

	BinaryData ackCmd;
	CmdErrorCode err = ExcuteBinaryCommand(OnQueryRtkElevationAndCnrMaskCmd, &cmd, &ackCmd, (nMode == Display) ? g_setting.GetLongTimeout() : g_setting.GetShortTimeout());
	if(err != Ack)
	{
		return err;
	}

	if(Return == nMode)
	{	//Return command data
    *((BinaryData*)outputData) = ackCmd;
		return err;
	}

	CString strMsg;
	strMsg = "Query RTK elevation and CNR mask successfully";
	add_msgtolist(strMsg);

  
  strMsg.Format("ELE: %d", (S08)ackCmd[6]);
  add_msgtolist(strMsg);
  strMsg.Format("CNR: %d", (U08)ackCmd[7]);
  add_msgtolist(strMsg);
  strMsg.Format("Geo-CNR: %d", (U08)ackCmd[8]);
  add_msgtolist(strMsg);
  
  strMsg.Format("Ambiguity ELE: %d", (S08)ackCmd[9]);
  add_msgtolist(strMsg);
  strMsg.Format("Ambiguity CNR: %d", (U08)ackCmd[10]);
  add_msgtolist(strMsg);
  strMsg.Format("Ambiguity Geo-CNR: %d", (U08)ackCmd[11]);
  add_msgtolist(strMsg);
	return Ack;
}

CGPSDlg::CmdErrorCode CGPSDlg::ClearRtkSlaveData(CmdExeMode nMode, void* outputData)
{ 
	BinaryCommand cmd(cmdTable[ClearRtkSlaveDataCmd].cmdSize);
	cmd.SetU08(1, cmdTable[ClearRtkSlaveDataCmd].cmdId);
	cmd.SetU08(2, cmdTable[ClearRtkSlaveDataCmd].cmdSubId);
  
  bool ret = ExecuteConfigureCommand(cmd.GetBuffer(), cmd.Size(), "Clear RTK slave backup data successfully", false);

  return (ret) ? Ack : Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryBasePosition(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryBasePositionCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryBasePositionCmd].cmdId);

	BinaryData ackCmd;
	CmdErrorCode err = ExcuteBinaryCommand(QueryBasePositionCmd, &cmd, &ackCmd, (nMode == Display) ? g_setting.GetLongTimeout() : g_setting.GetShortTimeout());
	if(err != Ack)
	{
		return err;
	}

	if(Return == nMode)
	{	//Return command data
    *((BinaryData*)outputData) = ackCmd;
		return err;
	}

	CString strMsg;
	strMsg = "Query Base Position successfully";
	add_msgtolist(strMsg);

	U16 cmdLen = ConvertLeonU16(ackCmd.Ptr(2));
	UINT8 svBpMode = ackCmd[5];
	strMsg.Format("Saved Base Position Mode: ");
	switch(svBpMode)
	{
	case 0:
		strMsg += "Kinematic Mode";
		add_msgtolist(strMsg);
		break;
	case 1:
		strMsg += "Survey Mode";
		add_msgtolist(strMsg);
		strMsg.Format("Saved Survey Length:%u", MAKELONG(MAKEWORD(ackCmd[9], ackCmd[8]), MAKEWORD(ackCmd[7], ackCmd[6])));
		add_msgtolist(strMsg);
		strMsg.Format("Standard Deviation:%u", MAKELONG(MAKEWORD(ackCmd[13], ackCmd[12]), MAKEWORD(ackCmd[11], ackCmd[10])));
		add_msgtolist(strMsg);
		break;
	case 2:
		strMsg += "Static Mode";
		add_msgtolist(strMsg);
		strMsg.Format("Saved Latitude:%f", ConvertLeonDouble(ackCmd.Ptr(14)));
		add_msgtolist(strMsg);
		strMsg.Format("Saved Longitude:%f", ConvertLeonDouble(ackCmd.Ptr(22)));
		add_msgtolist(strMsg);
		strMsg.Format("Saved Ellipsoidal Height:%f", ConvertLeonFloat(ackCmd.Ptr(30)));
		add_msgtolist(strMsg);
		break;
	}	

	UINT8 rtBpMode = ackCmd[34];
  strMsg.Format("Run-time Base Position Mode: ");
	switch(rtBpMode)
	{
	case 0:
		strMsg += "Kinematic Mode";
		add_msgtolist(strMsg);
		break;
	case 1:
		strMsg += "Survey Mode";
		add_msgtolist(strMsg);
		strMsg.Format("Run-time Survey Length:%u", MAKELONG(MAKEWORD(ackCmd[38], ackCmd[37]), MAKEWORD(ackCmd[36], ackCmd[35])));
		add_msgtolist(strMsg);
		break;
	case 2:
		strMsg += "Static Mode";
		add_msgtolist(strMsg);
		break;
	}	
	return Ack;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryRtkParameters(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryRtkParametersCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryRtkParametersCmd].cmdId);
	cmd.SetU08(2, cmdTable[QueryRtkParametersCmd].cmdSubId);

	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(QueryRtkParametersCmd, &cmd, &ackCmd))
	{
		CString strMsg;
		strMsg = "Query RTK parameters successfully";
		add_msgtolist(strMsg);
		strMsg.Format("Fix Ratio Scale: %d", MAKEWORD(ackCmd[7], ackCmd[6]));
		add_msgtolist(strMsg);
		strMsg.Format("Elevation Mask: 0x%04X", MAKEWORD(ackCmd[9], ackCmd[8]));
		add_msgtolist(strMsg);
		strMsg.Format("CNR mask: 0x%04X", MAKEWORD(ackCmd[11], ackCmd[10]));
		add_msgtolist(strMsg);
		strMsg.Format("Geo CNR Mask: 0x%04X", MAKEWORD(ackCmd[13], ackCmd[12]));
		add_msgtolist(strMsg);
		strMsg.Format("GPS PRN Mask: 0x%08X", MAKELONG(MAKEWORD(ackCmd[17], ackCmd[16]), MAKEWORD(ackCmd[15], ackCmd[14])));
		add_msgtolist(strMsg);
		strMsg.Format("Galileo PRN Mask: 0x%08X", MAKELONG(MAKEWORD(ackCmd[21], ackCmd[20]), MAKEWORD(ackCmd[19], ackCmd[18])));
		add_msgtolist(strMsg);
		strMsg.Format("Glonass PRN Mask: 0x%08X", MAKELONG(MAKEWORD(ackCmd[25], ackCmd[24]), MAKEWORD(ackCmd[23], ackCmd[22])));
		add_msgtolist(strMsg);
		strMsg.Format("Beidou2 PRN Mask 1~32: 0x%08X", MAKELONG(MAKEWORD(ackCmd[29], ackCmd[28]), MAKEWORD(ackCmd[27], ackCmd[26])));
		add_msgtolist(strMsg);
		strMsg.Format("Beidou2 PRN Mask 33~40: 0x%02X", ackCmd[30]);
		add_msgtolist(strMsg);
		strMsg.Format("QZSS PRN mask: 0x%02X", ackCmd[31]);
		add_msgtolist(strMsg);
	}
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryChannelDoppler(CmdExeMode nMode, void* outputData)
{	    
	const int ChannelCount = GSA_MAX_SATELLITE;
	CString strMsg("PRN, DOPPLER");
	if(nMode == Display)
	{
		add_msgtolist(strMsg);
	}

	for(int i = 0; i < ChannelCount; ++i)
	{
		BinaryCommand cmd(cmdTable[QueryChannelDopplerCmd].cmdSize);
		cmd.SetU08(1, cmdTable[QueryChannelDopplerCmd].cmdId);
		cmd.SetU08(2, (U08)i);

		BinaryData ackCmd;
		if(Ack == ExcuteBinaryCommand(QueryChannelDopplerCmd, &cmd, &ackCmd))
		{
			//U32 data = MAKELONG(MAKEWORD(ackCmd[8], ackCmd[7]), MAKEWORD(ackCmd[6], ackCmd[5]));
			S16 prn = MAKEWORD(ackCmd[6], ackCmd[5]);
			S16 doppler = MAKEWORD(ackCmd[8], ackCmd[7]);
			if(nMode==Return)
			{
				int* ChannelTable = (int*)outputData;
				ChannelTable[i] = prn;
			}
			else if(nMode==Display)
			{
				strMsg.Format(" %02d, %d", prn, doppler);
				add_msgtolist(strMsg);
			}
		}
	}
	if(nMode==Return)
	{
		return Ack;
	}
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryClockOffset(CmdExeMode nMode, void* outputData)
{	    
	const int ChannelCount = GSA_MAX_SATELLITE;
	CString strMsg("PRN, Clock Offset, ppm");
	if(nMode==Display)
	{
		add_msgtolist(strMsg);
	}
	int* ChannelTable = (int*)outputData;

	int totalCount = 0;
	int totalOffset = 0;
	for(int i=0; i<ChannelCount; ++i)
	{
		if(-1==ChannelTable[i])
		{
			continue;
		}
		BinaryCommand cmd(cmdTable[QueryChannelClockOffsetCmd].cmdSize);
		cmd.SetU08(1, cmdTable[QueryChannelClockOffsetCmd].cmdId);
		cmd.SetU08(6, HIBYTE(ChannelTable[i]));
		cmd.SetU08(7, LOBYTE(ChannelTable[i]));

		BinaryData ackCmd;
		if(Ack == ExcuteBinaryCommand(QueryChannelClockOffsetCmd, &cmd, &ackCmd))
		{
			S32 data = MAKELONG(MAKEWORD(ackCmd[8], ackCmd[7]), MAKEWORD(ackCmd[6], ackCmd[5]));
			strMsg.Format(" %3d, %5d, %2.6fppm", ChannelTable[i], data, data / (96.25 * 16.367667));
			add_msgtolist(strMsg);
			++totalCount;
			totalOffset += data;
		}
	}
	if(nMode==Display)
	{
		strMsg.Format("Avg: %5d, %2.6fppm", totalOffset / totalCount, (totalOffset / totalCount) / (96.25 * 16.367667));
		add_msgtolist(strMsg);
	}

	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryRefTimeSyncToGpsTime(CmdExeMode nMode, void* outputData)
{	    
	BinaryCommand cmd(cmdTable[QueryRefTimeSyncToGpsTimeCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryRefTimeSyncToGpsTimeCmd].cmdId);
	cmd.SetU08(2, cmdTable[QueryRefTimeSyncToGpsTimeCmd].cmdSubId);

	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(QueryRefTimeSyncToGpsTimeCmd, &cmd, &ackCmd))
	{
		CString strMsg = "Query Ref Time Sync To Gps Time successfully";
		add_msgtolist(strMsg);
		strMsg.Format("Enable: %d", ackCmd[6]);
		add_msgtolist(strMsg);
		strMsg.Format("Date: %d/%d/%d", MAKEWORD(ackCmd[8], ackCmd[7]), ackCmd[9], ackCmd[10]);
		add_msgtolist(strMsg);
	}
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::QuerySearchEngineSleepCriteria(CmdExeMode nMode, void* outputData)
{	    
	BinaryCommand cmd(cmdTable[QuerySearchEngineSleepCriteriaCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QuerySearchEngineSleepCriteriaCmd].cmdId);
	cmd.SetU08(2, cmdTable[QuerySearchEngineSleepCriteriaCmd].cmdSubId);

	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(QuerySearchEngineSleepCriteriaCmd, &cmd, &ackCmd))
	{
		CString strMsg = "QuerySearchEngineSleepCriteriaCmd successfully";
		add_msgtolist(strMsg);
		strMsg.Format("GPS Satellite Tracked Number: %d", ackCmd[6]);
		add_msgtolist(strMsg);
	}
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryDatumIndex(CmdExeMode nMode, void* outputData)
{	    
	BinaryCommand cmd(cmdTable[QueryDatumIndexCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryDatumIndexCmd].cmdId);
	cmd.SetU08(2, cmdTable[QueryDatumIndexCmd].cmdSubId);

	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(QueryDatumIndexCmd, &cmd, &ackCmd))
	{
		CString strMsg = "QueryDatumIndex successfully";
		add_msgtolist(strMsg);
		int datumIndex = MAKEWORD(ackCmd[7], ackCmd[6]);
		strMsg.Format("Datum Index: %d", datumIndex);
		add_msgtolist(strMsg);
		if(datumIndex < DatumListSize)
		{
			strMsg = DatumList[datumIndex];
			add_msgtolist(strMsg);
		}
	}
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::QuerySignalDisturbanceStatus(CmdExeMode nMode, void* outputData)
{	    
	BinaryCommand cmd(cmdTable[QuerySignalDisturbanceStatusCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QuerySignalDisturbanceStatusCmd].cmdId);
	cmd.SetU08(2, cmdTable[QuerySignalDisturbanceStatusCmd].cmdSubId);

	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(QuerySignalDisturbanceStatusCmd, &cmd, &ackCmd))
	{
		CString strMsg = "QuerySignalDisturbanceStatus successfully";
		add_msgtolist(strMsg);
		strMsg.Format("Operation Type: %d (%s)", ackCmd[6], (ackCmd[6]==0) ? "Disable" : "Enable");
		add_msgtolist(strMsg);
	}
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::QuerySignalDisturbanceData(CmdExeMode nMode, void* outputData)
{	    
	BinaryCommand cmd(cmdTable[QuerySignalDisturbanceDataCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QuerySignalDisturbanceDataCmd].cmdId);
	cmd.SetU08(2, cmdTable[QuerySignalDisturbanceDataCmd].cmdSubId);

	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(QuerySignalDisturbanceDataCmd, &cmd, &ackCmd))
	{
		const int FieldLen = 7;
		int chCnt = (MAKEWORD(ackCmd[3], ackCmd[2]) - 2) / FieldLen;
		CString strMsg = "QuerySignalDisturbanceData successfully";
		add_msgtolist(strMsg);

		strMsg = "NO. GNSS_TYPE PRN CN0  Total Unreliable_Count";
		add_msgtolist(strMsg);
		for(int i = 0; i < chCnt; ++i)
		{
			int a = 6 + i * FieldLen;
			const char* type = NULL;
			switch(ackCmd[a])
			{
			case 0:
				type = "NONE";
				break;
			case 1:
				type = "GPS";
				break;
			case 2:
				type = "SBAS";
				break;
			case 3:
				type = "GLONASS";
				break;
			case 4:
				type = "GALILEO";
				break;
			case 5:
				type = "QZSS";
				break;
			case 6:
				type = "BD2";
				break;
			default:
				type = "Unknown";
				break;
			}
	
			strMsg.Format("%3d  %8s %3d %3d %6d %6d", i, type, ackCmd[a + 1], ackCmd[a + 2], 
				MAKEWORD(ackCmd[a + 4], ackCmd[a + 3]), MAKEWORD(ackCmd[a + 6], ackCmd[a + 5]));
			add_msgtolist(strMsg);
		}
	}
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::ResetOdometer(CmdExeMode nMode, void* outputData)
{	    
	BinaryCommand cmd(1);
	cmd.SetU08(1, 0x7F);

	ClearQue();
	SendToTarget(cmd.GetBuffer(), cmd.Size(), "Reset Odometer successfully", true);	
	return Timeout;
}

//GPSDO command
//bit 0 : Enter pass through mode
//bit 1 : Disable GPS ISR(must reset master when leave)
//bit 2 : Reset slave, master only
//bit 3 : Drive slave CPU clock(must reset slave), master only
//bit 4 : Boost baud rate to 460800
#define GPSDO_PASS_THROUGH			(1 << 0)
#define GPSDO_DISABLE_GPS_ISR		(1 << 1)
#define GPSDO_RESET_SLAVE			(1 << 2)
#define GPSDO_DRIVE_SLAVE			(1 << 3)
#define GPSDO_BOOST_BAUDRATE		(1 << 4)
CGPSDlg::CmdErrorCode CGPSDlg::GpsdoResetSlave(CmdExeMode nMode, void* outputData)
{	    
	BinaryCommand cmd(4);
	cmd.SetU08(1, 0x7A);
	cmd.SetU08(2, 0x08);
	cmd.SetU08(3, 0x01);
	cmd.SetU08(4, GPSDO_RESET_SLAVE);

	ClearQue();
	SendToTarget(cmd.GetBuffer(), cmd.Size(), "Reset Slave MCU successfully", true);	
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::GpsdoEnterRom(CmdExeMode nMode, void* outputData)
{	    
	BinaryCommand cmd(4);
	cmd.SetU08(1, 0x7A);
	cmd.SetU08(2, 0x08);
	cmd.SetU08(3, 0x01);
	cmd.SetU08(4, (GPSDO_PASS_THROUGH | GPSDO_DISABLE_GPS_ISR | GPSDO_DRIVE_SLAVE));

	ClearQue();
	SendToTarget(cmd.GetBuffer(), cmd.Size(), "Enter Slave ROM Download successfully", true);	
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::GpsdoLeaveRom(CmdExeMode nMode, void* outputData)
{	    
	BinaryCommand cmd(4);
	cmd.SetU08(1, 0x7A);
	cmd.SetU08(2, 0x08);
	cmd.SetU08(3, 0x01);
	cmd.SetU08(4, GPSDO_DISABLE_GPS_ISR | GPSDO_DRIVE_SLAVE);

	ClearQue();
	SendToTarget(cmd.GetBuffer(), cmd.Size(), "Back To Normal Mode from ROM Download successfully", true);	
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::GpsdoEnterDownload(CmdExeMode nMode, void* outputData)
{	    
	BinaryCommand cmd(4);
	cmd.SetU08(1, 0x7A);
	cmd.SetU08(2, 0x08);
	cmd.SetU08(3, 0x01);
	cmd.SetU08(4, GPSDO_PASS_THROUGH | GPSDO_DISABLE_GPS_ISR);

	ClearQue();
	bool r = SendToTarget(cmd.GetBuffer(), cmd.Size(), "Enter Slave Download successfully", true);	
	return (r) ? Ack : Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::GpsdoLeaveDownload(CmdExeMode nMode, void* outputData)
{	    
	BinaryCommand cmd(4);
	cmd.SetU08(1, 0x7A);
	cmd.SetU08(2, 0x08);
	cmd.SetU08(3, 0x01);
	cmd.SetU08(4, GPSDO_DISABLE_GPS_ISR);

	ClearQue();
	SendToTarget(cmd.GetBuffer(), cmd.Size(), "Back To Normal Mode from Slave Download successfully", true);	
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::InsdrEnterDownload(CmdExeMode nMode, void* outputData)
{	    
	BinaryCommand cmd(4);
	cmd.SetU08(1, 0x7A);
	cmd.SetU08(2, 0x08);
	cmd.SetU08(3, 0x01);
	cmd.SetU08(4, GPSDO_DISABLE_GPS_ISR);

	ClearQue();
	SendToTarget(cmd.GetBuffer(), cmd.Size(), "Enter UART pass through for slave flash code", true);	
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::GpsdoEnterDownloadHigh(CmdExeMode nMode, void* outputData)
{	    
	BinaryCommand cmd(4);
	cmd.SetU08(1, 0x7A);
	cmd.SetU08(2, 0x08);
	cmd.SetU08(3, 0x01);
	cmd.SetU08(4, GPSDO_PASS_THROUGH | GPSDO_DISABLE_GPS_ISR | GPSDO_DRIVE_SLAVE | GPSDO_BOOST_BAUDRATE);

	ClearQue();
	bool r = SendToTarget(cmd.GetBuffer(), cmd.Size(), "Enter High-Speed Slave Download successfully", false);	
	CGPSDlg::gpsDlg->SetBaudrate(7);		//460800 bps
	PostMessage(UWM_GPSDO_HI_DOWNLOAD, 0, 0);
	
	return (r) ? Ack : Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::GpsdoEnterUart(CmdExeMode nMode, void* outputData)
{	    
	BinaryCommand cmd(4);
	cmd.SetU08(1, 0x7A);
	cmd.SetU08(2, 0x08);
	cmd.SetU08(3, 0x01);
	cmd.SetU08(4, GPSDO_PASS_THROUGH);

	ClearQue();
	bool r = SendToTarget(cmd.GetBuffer(), cmd.Size(), "Enter Slave UART Pass Through successfully", true);	
	return (r) ? Ack : Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::InsdrEnterUart(CmdExeMode nMode, void* outputData)
{	    
	BinaryCommand cmd(4);
	cmd.SetU08(1, 0x7A);
	cmd.SetU08(2, 0x08);
	cmd.SetU08(3, 0x01);
	cmd.SetU08(4, GPSDO_PASS_THROUGH);

	ClearQue();
	bool r = SendToTarget(cmd.GetBuffer(), cmd.Size(), "Enter UART pass through for slave ROM code", true);	
	return (r) ? Ack : Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::GpsdoLeaveUart(CmdExeMode nMode, void* outputData)
{	    
	BinaryCommand cmd(4);
	cmd.SetU08(1, 0x7A);
	cmd.SetU08(2, 0x08);
	cmd.SetU08(3, 0x01);
	cmd.SetU08(4, 0x00);

	ClearQue();
	SendToTarget(cmd.GetBuffer(), cmd.Size(), "Back To Normal Mode from UART Passthrough successfully", true);	
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::InsdrLeaveUart(CmdExeMode nMode, void* outputData)
{	    
	BinaryCommand cmd(4);
	cmd.SetU08(1, 0x7A);
	cmd.SetU08(2, 0x08);
	cmd.SetU08(3, 0x01);
	cmd.SetU08(4, 0x00);

	ClearQue();
	bool r = SendToTargetAndCheckAck(cmd.GetBuffer(), cmd.Size(), "Back To Master successfully", 1000, 3);	
	return (r) ? Ack : Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryUartPass(CmdExeMode nMode, void* outputData)
{	    
	BinaryCommand cmd(cmdTable[QueryUartPassCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryUartPassCmd].cmdId);
	cmd.SetU08(2, cmdTable[QueryUartPassCmd].cmdSubId);
	cmd.SetU08(3, 0x02);

	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(QueryUartPassCmd, &cmd, &ackCmd))
	{
		if(nMode==Return && outputData)
		{
			*((BYTE*)outputData) = ackCmd[7];
			return Ack;
		}
		CString strMsg = "Query UART pass-through successfully";
		add_msgtolist(strMsg);
		if(ackCmd[7] == 1)
		{
			strMsg.Format("UART pass-through: MASTER");
		}
		else if(ackCmd[7] == 0)
		{
			strMsg.Format("UART pass-through: SLAVE");
		}
		else
		{
			strMsg.Format("UART pass-through: %d", ackCmd[7]);
		}
		add_msgtolist(strMsg);
	}
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryGnssConstellationType(CmdExeMode nMode, void* outputData)
{	    
	BinaryCommand cmd(cmdTable[QueryGnssConstellationTypeCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryGnssConstellationTypeCmd].cmdId);
	cmd.SetU08(2, cmdTable[QueryGnssConstellationTypeCmd].cmdSubId);

	BinaryData ackCmd;
  CmdErrorCode err = ExcuteBinaryCommand(QueryGnssConstellationTypeCmd, &cmd, &ackCmd, (nMode == Display) ? g_setting.GetLongTimeout() : g_setting.GetShortTimeout());
	if(err != Ack)
	{
    return err;
  }

  if(Return == nMode)
	{	
    *((BinaryData*)outputData) = ackCmd;
		return err;
  }

	CString strMsg = "Query GNSS constellation type successfully";
	add_msgtolist(strMsg);
	U16 mode = MAKEWORD(ackCmd[7], ackCmd[6]);
	add_msgtolist("Navigation Solution: ");

	CString strOutput;
	if(mode & 0x0001)
	{
		strOutput += "GPS + ";
	}
	if(mode & 0x0002)
	{
		strOutput += "GLONASS + ";
	}
	if(mode & 0x0004)
	{
		strOutput += "Galileo + ";
	}
	if(mode & 0x0008)
	{
		strOutput += "Beidou + ";
	}
	if(mode & 0x0010)
	{
		strOutput += "NavIC + ";
	}

	if(mode == 0)
	{
		add_msgtolist("None");
	}
	else
	{
		strOutput = strOutput.Left(strOutput.GetLength() - 2);
		add_msgtolist(strOutput);
	}

	return err;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryCustomerID(CmdExeMode nMode, void* outputData)
{	    
	BinaryCommand cmd(cmdTable[QueryCustomerIDCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryCustomerIDCmd].cmdId);

	BinaryData ackCmd;
	if(Return==nMode && outputData)
	{
		if(Ack == ExcuteBinaryCommand(QueryCustomerIDCmd, &cmd, &ackCmd, 300, true))
		{
			*((U32*)outputData) = MAKELONG(MAKEWORD(ackCmd[8], ackCmd[7]),MAKEWORD(ackCmd[6], ackCmd[5]));
		}
		return Timeout;
	}
	if(Ack == ExcuteBinaryCommand(QueryCustomerIDCmd, &cmd, &ackCmd))
	{
		CString strMsg = "Query Customer ID successfully";
		add_msgtolist(strMsg);
		strMsg.Format("Customer ID: 0X%08X",
			MAKELONG(MAKEWORD(ackCmd[8], ackCmd[7]),MAKEWORD(ackCmd[6], ackCmd[5])));
		add_msgtolist(strMsg);
	}
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::Query1ppsPulseWidth(CmdExeMode nMode, void* outputData)
{	    
	BinaryCommand cmd(cmdTable[Query1ppsPulseWidthCmd].cmdSize);
	cmd.SetU08(1, cmdTable[Query1ppsPulseWidthCmd].cmdId);
	cmd.SetU08(2, cmdTable[Query1ppsPulseWidthCmd].cmdSubId);

	BinaryData ackCmd;
  CmdErrorCode err = ExcuteBinaryCommand(Query1ppsPulseWidthCmd, &cmd, &ackCmd);
	if(err != Ack)
	{
    return err;
  }

  if(Return == nMode)
	{	//Return command length
    *((U32*)outputData) = MAKELONG(MAKEWORD(ackCmd[9], ackCmd[8]),MAKEWORD(ackCmd[7], ackCmd[6]));
		return err;
  }

	CString strMsg = "Query 1PPS pulse width successfully";
	add_msgtolist(strMsg);
	strMsg.Format("1PPS Pulse Width: %dus",
		MAKELONG(MAKEWORD(ackCmd[9], ackCmd[8]),MAKEWORD(ackCmd[7], ackCmd[6])));
	add_msgtolist(strMsg);

	return Ack;
}

CGPSDlg::CmdErrorCode CGPSDlg::Query1ppsFreqencyOutput(CmdExeMode nMode, void* outputData)
{	    
	BinaryCommand cmd(cmdTable[Query1ppsFreqencyOutputCmd].cmdSize);
	cmd.SetU08(1, cmdTable[Query1ppsFreqencyOutputCmd].cmdId);
	cmd.SetU08(2, cmdTable[Query1ppsFreqencyOutputCmd].cmdSubId);

	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(Query1ppsFreqencyOutputCmd, &cmd, &ackCmd))
	{
		CString strMsg = "Query 1PPS Freqency Output successfully";
		add_msgtolist(strMsg);
		strMsg.Format("Freqency: %d",
			MAKELONG(MAKEWORD(ackCmd[9], ackCmd[8]),MAKEWORD(ackCmd[7], ackCmd[6])));
		add_msgtolist(strMsg);
	}
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryBinaryMeasurementDataOut(CmdExeMode nMode, void* outputData)
{	    
	BinaryCommand cmd(cmdTable[QueryBinaryMeasurementDataOutCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryBinaryMeasurementDataOutCmd].cmdId);

	BinaryData ackCmd;
  CmdErrorCode err = ExcuteBinaryCommand(QueryBinaryMeasurementDataOutCmd, &cmd, &ackCmd, (nMode == Display) ? g_setting.GetLongTimeout() : g_setting.GetShortTimeout());
	if(err != Ack)
	{
    return err;
  }

  if(Return == nMode)
	{	//Return command length
    *((BinaryData*)outputData) = ackCmd;
		return err;
  }

	U16 cmdLen = ConvertLeonU16(ackCmd.Ptr(2));
	CString strMsg = "Query Binary Measurement Data Out successfully";
	add_msgtolist(strMsg);
	if(ackCmd[5]==0)
	{
		strMsg.Format("Output Rate: 1Hz");	
	}
	else if(ackCmd[5]==1)
	{
		strMsg.Format("Output Rate: 2Hz");
	}
	else if(ackCmd[5]==2)
	{
		strMsg.Format("Output Rate: 4Hz");
	}
	else if(ackCmd[5]==3)
	{
		strMsg.Format("Output Rate: 5Hz");
	}
	else if(ackCmd[5]==4)
	{
		strMsg.Format("Output Rate: 10Hz");
	}
	else if(ackCmd[5]==5)
	{
		strMsg.Format("Output Rate: 20Hz");
	}
	else if(ackCmd[5]==6)	//Add in 20160512, request from Andrew
	{
		strMsg.Format("Output Rate: 8Hz");
	}
	add_msgtolist(strMsg);

	strMsg.Format("Meas Time: %s", (ackCmd[6]) ? "Enable" : "Disable");
	add_msgtolist(strMsg);

	strMsg.Format("Raw Meas: %s", (ackCmd[7]) ? "Enable" : "Disable");
	add_msgtolist(strMsg);

	strMsg.Format("SV CH Status: %s", (ackCmd[8]) ? "Enable" : "Disable");
	add_msgtolist(strMsg);

	strMsg.Format("RCV State: %s", (ackCmd[9]) ? "Enable" : "Disable");
	add_msgtolist(strMsg);

	if(cmdLen == 8)
	{	//The length in old style is 7, new is 8. 20160914 changed by Ryan
	  strMsg.Format("Extended Raw Meas: %s", (ackCmd[11]) ? "Enable" : "Disable");
	  add_msgtolist(strMsg);
	}

	//20160419 Modify BinaryMeasurementDataOut command format, 
  //remove constellation field, Report from Andrew, Oliver.
  /*
	CString strOutput;
	if(ackCmd[10] & 0x01)
	{
		strOutput += "GPS + ";
	} 
	if(ackCmd[10] & 0x02)
	{
		strOutput += "GLONASS + ";
	}
	if(ackCmd[10] & 0x04)
	{
		strOutput += "Galileo + ";
	}
	if(ackCmd[10] & 0x08)
	{
		strOutput += "Beidou + ";
	}

	if(ackCmd[10]==0)
	{
		add_msgtolist("None");
	}
	else
	{
		add_msgtolist("Subframe for different constellation :");
		strOutput = strOutput.Left(strOutput.GetLength() - 2);
		add_msgtolist(strOutput);
	}
  //*/
	return Ack;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryRtcmMeasurementDataOut(CmdExeMode nMode, void* outputData)
{	    
	BinaryCommand cmd(cmdTable[QueryRtcmMeasurementDataOutCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryRtcmMeasurementDataOutCmd].cmdId);

	BinaryData ackCmd;
  CmdErrorCode err = ExcuteBinaryCommand(QueryRtcmMeasurementDataOutCmd, &cmd, &ackCmd, (nMode == Display) ? 3000 : 1000);
	if(err != Ack)
	{
    return err;
  }

  if(Return == nMode)
	{	
    *((BinaryData*)outputData) = ackCmd;
		return err;
  }

	U16 cmdLen = ConvertLeonU16(ackCmd.Ptr(2));
	CString strMsg = "Query RTCM Measurement Data Out successfully";
	add_msgtolist(strMsg);
  strMsg.Format("RTCM Output: %s", (ackCmd[5]) ? "Enable" : "Disable");	
	add_msgtolist(strMsg);
	if(ackCmd[6]==0)
	{
		strMsg.Format("Output Rate: 1Hz");	
	}
	else if(ackCmd[6]==1)
	{
		strMsg.Format("Output Rate for MSM: 2Hz");
	}
	else if(ackCmd[6]==2)
	{
		strMsg.Format("Output Rate for MSM: 4Hz");
	}
	else if(ackCmd[6]==3)
	{
		strMsg.Format("Output Rate for MSM: 5Hz");
	}
	else if(ackCmd[6]==4)
	{
		strMsg.Format("Output Rate for MSM: 10Hz");
	}
	else if(ackCmd[6]==5)
	{
		strMsg.Format("Output Rate for MSM: 20Hz");
	}
	else if(ackCmd[6]==6)	//Add in 20160512, request from Andrew
	{
		strMsg.Format("Output Rate for MSM: 8Hz");
	}
	add_msgtolist(strMsg);

	strMsg.Format("Message 1005: %s", (ackCmd[7]) ? "Enable" : "Disable");
	add_msgtolist(strMsg);

  if(ackCmd[18] == 0)
  {
	  strMsg.Format("Message 1077: %s", (ackCmd[8]) ? "Enable" : "Disable");
	  add_msgtolist(strMsg);

	  strMsg.Format("Message 1087: %s", (ackCmd[9]) ? "Enable" : "Disable");
	  add_msgtolist(strMsg);
    //Add in 20200205, request from Neil
	  strMsg.Format("Message 1097: %s", (ackCmd[10]) ? "Enable" : "Disable");
	  add_msgtolist(strMsg);

	  strMsg.Format("Message 1107: %s", (ackCmd[11]) ? "Enable" : "Disable");
	  add_msgtolist(strMsg);

	  strMsg.Format("Message 1117: %s", (ackCmd[12]) ? "Enable" : "Disable");
	  add_msgtolist(strMsg);

	  strMsg.Format("Message 1127: %s", (ackCmd[13]) ? "Enable" : "Disable");
	  add_msgtolist(strMsg);
  }
  else if(ackCmd[18] == 1)
  {
	  strMsg.Format("Message 1074: %s", (ackCmd[8]) ? "Enable" : "Disable");
	  add_msgtolist(strMsg);

	  strMsg.Format("Message 1084: %s", (ackCmd[9]) ? "Enable" : "Disable");
	  add_msgtolist(strMsg);

	  strMsg.Format("Message 1094: %s", (ackCmd[10]) ? "Enable" : "Disable");
	  add_msgtolist(strMsg);

	  strMsg.Format("Message 1104: %s", (ackCmd[11]) ? "Enable" : "Disable");
	  add_msgtolist(strMsg);

	  strMsg.Format("Message 1114: %s", (ackCmd[12]) ? "Enable" : "Disable");
	  add_msgtolist(strMsg);

	  strMsg.Format("Message 1124: %s", (ackCmd[13]) ? "Enable" : "Disable");
	  add_msgtolist(strMsg);
  }

  if(ackCmd[19] != 0)
  {
    if(ackCmd[14] == 0)
    {
      strMsg = "Message 1019: Disable";
    }
    else
    {
	    strMsg.Format("Message 1019 interval: %d seconds", ackCmd[14]);
    }
	  add_msgtolist(strMsg);
	  
    if(ackCmd[15] == 0)
    {
      strMsg = "Message 1020: Disable";
    }
    else
    {
      strMsg.Format("Message 1020 interval: %d seconds", ackCmd[15]);
    }
	  add_msgtolist(strMsg);

    if(ackCmd[16] == 0)
    {
      strMsg = "Message 1042: Disable";
    }
    else
    {
	    strMsg.Format("Message 1042 interval: %d seconds", ackCmd[16]);
    }
	  add_msgtolist(strMsg);

    if(ackCmd[17] == 0)
    {
      strMsg = "Message 1046: Disable";
    }
    else
    {
	    strMsg.Format("Message 1046 interval: %d seconds", ackCmd[17]);
    }
	  add_msgtolist(strMsg);
  }
  if(ackCmd[19] > 1)
  {
    strMsg.Format("MSM Type: %s", (ackCmd[18]) ? "MSM4" : "MSM7");
	  add_msgtolist(strMsg);
  }
	return Ack;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryCableDelay(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryCableDelayCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryCableDelayCmd].cmdId);

	BinaryData ackCmd;
  CmdErrorCode err = ExcuteBinaryCommand(QueryCableDelayCmd, &cmd, &ackCmd);
	if(err != Ack)
	{
    return err;
  }
  if(Return == nMode)
	{	//Return command length
    *((S32*)outputData) = (S32)MAKELONG(MAKEWORD(ackCmd[8], ackCmd[7]),MAKEWORD(ackCmd[6], ackCmd[5]));
		return err;
  }

	CString strMsg = "Query 1PPS Cable Delay successfully";
	add_msgtolist(strMsg);

	S32 data = (S32)MAKELONG(MAKEWORD(ackCmd[8], ackCmd[7]),MAKEWORD(ackCmd[6], ackCmd[5]));
	strMsg.Format("Cable delay: %0.2fns", data / 100.0);
	add_msgtolist(strMsg);

	return Ack;
}

UINT SetFacMsgThread(LPVOID pParam)
{	
	int len = *((int*)(pParam));
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(CGPSDlg::m_inputMsg, len, "Factroy Reset successfully");
#if defined(SAINTMAX_UI)
	Sleep(500);
	CGPSDlg::gpsDlg->m_nmea0183msg.EnableWindow(TRUE);
#endif
	return 0;
}

void CGPSDlg::SetFactoryDefault(bool isReboot)
{
	if(!CheckConnect())
	{
		return;
	}
	int cmdLen = 9;
	SetInputMode(NoOutputMode);
	memset(m_inputMsg, 0, cmdLen);   		    
	m_inputMsg[0]=(U08)0xA0;
	m_inputMsg[1]=(U08)0xA1;
	m_inputMsg[2]=0;
	m_inputMsg[3]=2;
	m_inputMsg[4]=4; //msgid
	if(isReboot)
	{
		m_inputMsg[5]=1;
		m_inputMsg[6]=5; //checksum right	 
	}
	else
	{
		m_inputMsg[5]=0;
		m_inputMsg[6]=4; //checksum right	 
	}
	m_inputMsg[7]=(U08)0x0D;
	m_inputMsg[8]=(U08)0x0A;
	//slgsv = 6;
	AfxBeginThread(SetFacMsgThread, &cmdLen);
}
/*
void CGPSDlg::OnSetFactoryDefaultNoReboot()
{		
	SetFactoryDefault(false);
}
*/
void CGPSDlg::OnSetFactoryDefaultReboot()
{	
	SetFactoryDefault(true);
}

void CGPSDlg::OnConfigureNmeaIntervalV8()
{
	if(!CheckConnect())
	{
		return;
	}
	SetInputMode(NoOutputMode);

	CConfigNmeaIntervalDlg dlg;
	if(dlg.DoModal() != IDOK)		
	{
		SetMode();  
		CreateGPSThread();
	}
}

void CGPSDlg::OnConfigureEricssonSentecneInterval()
{
	if(!CheckConnect())
	{
		return;
	}
	SetInputMode(NoOutputMode);
	ConfigEricssonIntervalDlg dlg;
	if(dlg.DoModal() != IDOK)		
	{
		SetMode();  
		CreateGPSThread();
	}
}

void CGPSDlg::OnConfigureSerialNumber()
{
	if(!CheckConnect())
	{
		return;
	}
	SetInputMode(NoOutputMode);
	ConfigSerialNumberDlg dlg;
	if(dlg.DoModal() != IDOK)		
	{
		SetMode();  
		CreateGPSThread();
	}
}
/*
void CGPSDlg::OnBinaryConfiguredatum()
{
	if(!CheckConnect())
	{
		return;
	}

	SetInputMode(NoOutputMode);
	CConDauDlg dlg;
	if(dlg.DoModal() != IDOK)
	{
		SetMode();  
		CreateGPSThread();
	}
}
*/
void CGPSDlg::OnConfigureSerialPort()
{
  if(g_setting.GetComPortIndex() == -1 && m_isConnectOn)
  {
    ::AfxMessageBox("TCP/IP connection doesn't support this function!");
    return;
  }
	if(!CheckConnect())
	{
		return;
	}

	SetInputMode(NoOutputMode);
	CConSrePorDlg dlg;
	if(dlg.DoModal() != IDOK) 	
	{
		SetMode();  
		CreateGPSThread();
	}
}

void CGPSDlg::GetGpsAlmanac(CString m_almanac_filename,U08 sv,U08 continues)
{
	int wait = 0;
	U08 msg[2];
	char BINMSG[1024] = {0};

	msg[0] = 0x50;
	msg[1] = sv;
	int res_len;
	int len = SetMessage(msg,sizeof(msg));
	FILE *f;

	//WaitEvent();
	ClearQue();
	if(SendToTarget(m_inputMsg, len, "Get Almance start...", true) == 1)
	{
		if(g_waitReadDialog == NULL)
		{
			g_waitReadDialog = new CWaitReadLog;
		}
		AfxBeginThread(WaitLogRead,0);
		WaitForSingleObject(waitlog,INFINITE);
		g_waitReadDialog->SetWindowText("Wait for get almanac");
		g_waitReadDialog->msg.SetWindowText("Please wait for get almanac!");

		U08 NumsOfEphemeris = 0;
		fopen_s(&f, m_almanac_filename, "wb+");
		fclose(f);

		while(1)
		{	
			wait++;
			if(wait == 50)
			{ 
				Sleep(500);
				g_waitReadDialog->msg.SetWindowText("Retrieve almanac data is Failed!");
				Sleep(500);
				g_waitReadDialog->IsFinish = true;
				add_msgtolist("Retrieve almanac Failed...");	
				//goto TheLast;
			}
			U08 buff[1024] = {0};
			res_len = m_serial->GetBinaryAck(buff, sizeof(buff));

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
			g_waitReadDialog->msg.SetWindowText(BINMSG);

			if(NumsOfEphemeris==32)break;
			//}
		}	
		Sleep(500);
		g_waitReadDialog->msg.SetWindowText("Retrieve almanac data is completed!");
		Sleep(500);
		g_waitReadDialog->IsFinish = true;
		add_msgtolist("Retrieve almanac successfully");	
	}

	if(!continues)
	{
		SetMode();  
		CreateGPSThread();
	}
}

UINT GetGpsAlmanacThread(LPVOID param)
{
	CGPSDlg::gpsDlg->GetGpsAlmanac(m_almanac_filename,m_almanac_no,FALSE);
	return 0;
}

void CGPSDlg::OnGetGpsAlmanac()
{
	if(!CheckConnect())	
	{
    return;
  }
    
  CGetAlmanac frm;
	if(frm.DoModal()==IDOK)
	{
		m_almanac_filename = frm.fileName;
		m_almanac_no = frm.sv;
		::AfxBeginThread(GetGpsAlmanacThread, 0);
	}
  else
	{
		SetMode();  
		CreateGPSThread();
	}
}

void CGPSDlg::OnShowGpsAlmanac()
{
	if(!CheckConnect())
	{
		return;
	}

	CGetAlmanac frm;
  frm.isDecode = 1;
	if(frm.DoModal()==IDOK)
	{
	  U08 no = frm.sv;
		QueryGetGpsAlmanac(Display, &no);

		SetMode();
		CreateGPSThread();
	}
}

UINT SetGpsAlmanacThread(LPVOID pParam)
{	
	CGPSDlg::gpsDlg->SetGpsAlmanac(FALSE);	
	return 0;
}

void CGPSDlg::SetGpsAlmanac(U08 continues)	
{	
	U16 SVID;	

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
			}
			else
			{
				if(SVID == 32)
					msg[one_entry_size] = 1;
				else
					msg[one_entry_size] = 0;
			}

			int len = SetMessage2(messages, msg, one_entry_size + 1);
			sprintf_s(m_nmeaBuffer, "Set SV#%d almanac successfully",SVID);
			if(!SendToTargetNoWait(messages, len,m_nmeaBuffer))
			{
				sprintf_s(m_nmeaBuffer, "Set SV#%d almanac failed...",SVID);
				add_msgtolist(m_nmeaBuffer);
			}
		}
	}
	else
	{
		AfxMessageBox("The Almanac data Format of the file is wrong");
	}	
	m_ephmsFile.Close();	
	if(!continues)
	{
		SetMode(); 
		CreateGPSThread();	
	}
}

void CGPSDlg::OnSetGpsAlmanac()
{
	if(!CheckConnect())
	{
		return;
	}

	SetInputMode(NoOutputMode);
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
			AfxBeginThread(SetGpsAlmanacThread, 0);			
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

	ClearQue();
	if(SendToTarget(m_inputMsg,len, "Get Glonass Almance start...", true) == 1)
	{
		if(g_waitReadDialog == NULL)
		{
			g_waitReadDialog = new CWaitReadLog;
		}
		AfxBeginThread(WaitLogRead, 0);
		WaitForSingleObject(waitlog, INFINITE);
		g_waitReadDialog->SetWindowText("Wait for get Glonass almanac");
		g_waitReadDialog->msg.SetWindowText("Please wait for get Glonass almanac!");

		U08 NumsOfEphemeris = 0;

		fopen_s(&f, m_almanac_filename, "wb+");
		fclose(f);

		while(1)
		{	
			wait++;
			if(wait == 50){ 
				Sleep(500);
				g_waitReadDialog->msg.SetWindowText("Retrieve Glonass almanac data is Failed!");
				Sleep(500);
				g_waitReadDialog->IsFinish = true;
				add_msgtolist("Retrieve Glonass almanac Failed...");	
				//goto TheLast;
			}
			U08 buff[1024] = {0};
			res_len = m_serial->GetBinaryAck(buff, sizeof(buff));

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
			sprintf_s(BINMSG, sizeof(BINMSG), "Retrieve Glonass Satellite ID # %d almanac", NumsOfEphemeris);
			g_waitReadDialog->msg.SetWindowText(BINMSG);

			if(NumsOfEphemeris==24)break;
		}	
		Sleep(500);
		g_waitReadDialog->msg.SetWindowText("Retrieve Glonass almanac data is completed!");
		Sleep(500);
		g_waitReadDialog->IsFinish = true;
		add_msgtolist("Retrieve Glonass almanac successfully");	

	}

	if(!continues)
	{
		SetMode();  
		CreateGPSThread();
	}
}

UINT GetGlonassAlmanacThread(LPVOID param)
{
	CGPSDlg::gpsDlg->GetGlonassAlmanac(m_almanac_filename,m_almanac_no,FALSE);
	return 0;
}

void CGPSDlg::OnGetGlonassAlmanac()
{
	if(!CheckConnect())	
	{
    return;
  }
    
  CGetAlmanac frm;
	frm.isGlonass = 1;
	if(frm.DoModal()==IDOK)
	{
		m_almanac_filename = frm.fileName;
		m_almanac_no = frm.sv;
		::AfxBeginThread(GetGlonassAlmanacThread, 0);
	}
	else
	{
		SetMode();  
		CreateGPSThread();
	}
}

UINT SetGlonassAlmanacThread(LPVOID pParam)
{	
	CGPSDlg::gpsDlg->SetGlonassAlmanac(FALSE);	
	return 0;
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
			sprintf_s(m_nmeaBuffer, sizeof(m_nmeaBuffer), "Set SV#%d Glonass almanac successfully",SVID);
			if(!SendToTargetNoWait(messages, len,m_nmeaBuffer))
			{
				sprintf_s(m_nmeaBuffer, sizeof(m_nmeaBuffer), "Set SV#%d Glonass almanac failed...",SVID);
				add_msgtolist(m_nmeaBuffer);
			}

			//Sleep(10);
		}
	}
	else
	{
		AfxMessageBox("The Glonass Almanac data Format of the file is wrong");
	}	
	m_ephmsFile.Close();	
	if(!continues)
	{
	SetMode(); 
	CreateGPSThread();
	}	
}

void CGPSDlg::OnSetGlonassAlmanac()
{
	if(!CheckConnect())
	{
		return;
	}

	SetInputMode(NoOutputMode);
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

void CGPSDlg::GetBeidouAlmanac(CString m_almanac_filename, U08 sv, U08 continues)
{
	int wait = 0;
	U08 msg[3];
	char BINMSG[1024] = {0};

	msg[0] = 0x67;
	msg[1] = 0x04;
	msg[2] = sv;
	int res_len;
	int len = SetMessage(msg, sizeof(msg));
	FILE *f = NULL;

	ClearQue();
	if(SendToTarget(m_inputMsg, len, "Get Beidou2 almance start...", true) == 1)
	{
		if(g_waitReadDialog == NULL)
		{
			g_waitReadDialog = new CWaitReadLog;
		}
		AfxBeginThread(WaitLogRead,0);
		WaitForSingleObject(waitlog,INFINITE);
		g_waitReadDialog->SetWindowText("Waiting for get Beidou2 almanac...");
		g_waitReadDialog->msg.SetWindowText("Please wait for the Beidou2 almanac.");

    U08 NumsOfEphemeris = 0;
		fopen_s(&f, m_almanac_filename, "wb+");
		fclose(f);

		while(1)
		{	
			wait++;
			if(wait == 50)
			{ 
				Sleep(500);
				g_waitReadDialog->msg.SetWindowText("Retrieving the Beidou2 almanac data fails!");
				Sleep(500);
				g_waitReadDialog->IsFinish = true;
				add_msgtolist("Retrieving the Beidou2 almanac fails!");	
			}
			U08 buff[1024] = {0};
			res_len = m_serial->GetBinaryAck(buff, sizeof(buff));

			if(res_len==60)
			{
				fopen_s(&f, m_almanac_filename, "ab");
				fwrite(&buff[6],1,res_len-9,f);
				fclose(f);
			}
			else
			{
				break;
			}

			NumsOfEphemeris = buff[8];
			sprintf_s(BINMSG, sizeof(BINMSG), "Retrieving the Beidou2 satellite ID #%d almanac.", NumsOfEphemeris);
			g_waitReadDialog->msg.SetWindowText(BINMSG);

			if(NumsOfEphemeris==37)
				break;
		}	
		Sleep(500);
		g_waitReadDialog->msg.SetWindowText("Retrieving the Beidou2 almanac data complete.");
		Sleep(500);
		g_waitReadDialog->IsFinish = true;
		add_msgtolist("Retrieving the Beidou2 almanac successfully.");	
	}

	if(!continues)
	{
		SetMode();  
		CreateGPSThread();
	}
}

UINT GetBeidouAlmanacThread(LPVOID param)
{
	CGPSDlg::gpsDlg->GetBeidouAlmanac(m_almanac_filename, m_almanac_no, FALSE);
	return 0;
}

void CGPSDlg::OnGetBeidouAlmanac()
{
	if(!CheckConnect())	
	{
    return;
  }

	CGetAlmanac frm;
	frm.isGlonass = 2;
	if(frm.DoModal()==IDOK)
	{
		m_almanac_filename = frm.fileName;
		m_almanac_no = frm.sv;
		::AfxBeginThread(GetBeidouAlmanacThread, 0);
	}
	else
	{
		SetMode();  
		CreateGPSThread();
	}
}

void CGPSDlg::OnShowBeidouAlmanac()
{
	if(!CheckConnect())
	{
		return;
	}

	CGetAlmanac frm;
	frm.isGlonass = 2;
  frm.isDecode = 1;
	if(frm.DoModal()==IDOK)
	{
	  U08 no = frm.sv;
		QueryGetBeidouAlmanac(Display, &no);

		SetMode();
		CreateGPSThread();
	}
}

UINT SetBeidouAlmanacThread(LPVOID pParam)
{	
	CGPSDlg::gpsDlg->SetBeidouAlmanac(FALSE);	
	return 0;
}

void CGPSDlg::SetBeidouAlmanac(U08 continues)	
{	
	BYTE buffer[1000];
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

	m_ephmsFile.Read(buffer, 1);
	int one_entry_size = 51;

	if(dwBytesRemaining == 37 * one_entry_size)
	{
		m_ephmsFile.SeekToBegin();
		while(1)
		{
			UINT nBytesRead = m_ephmsFile.Read(buffer, one_entry_size);	
			if(nBytesRead <= 0)
			{
				break;
			}
			U08 msg[53] = { 0 };	//id, sub-id, svid-h, svid-l, 48 bytes almanac, attributes
			U08 messages[100] = { 0 };
			msg[0] = 0x67;
			msg[1] = 0x03;

			memcpy(&msg[2], buffer + 1, one_entry_size - 1);
			U16 SVID = buffer[2];	
			if(SVID == 0) 
			{
				continue;
			}

			if(continues)
			{
				msg[one_entry_size] = 0;
			}
			else
			{
				msg[one_entry_size] = (SVID == 37) ? 1 : 0;
      }

			int len = SetMessage2(messages, msg, sizeof(msg));
			sprintf_s(m_nmeaBuffer, sizeof(m_nmeaBuffer), "Set SV#%d Beidou2 almanac successfuly.", SVID);
			if(!SendToTargetNoWait(messages, len, m_nmeaBuffer))
			{
				sprintf_s(m_nmeaBuffer, sizeof(m_nmeaBuffer), "Set SV#%d Beidou2 almanac fails!", SVID);
				add_msgtolist(m_nmeaBuffer);
			}
		}
	}
	else
	{
		AfxMessageBox("File of the Beidou2 almanac data format wrong!");
		//return;
	}	
	m_ephmsFile.Close();	
	if(!continues)
	{
		SetMode(); 
		CreateGPSThread();
	}	
}

void CGPSDlg::OnSetBeidouAlmanac()
{
	if(!CheckConnect())
	{
		return;
	}

	SetInputMode(NoOutputMode);
	CString fileName("Beidou_Almanac.log");	
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
			AfxBeginThread(SetBeidouAlmanacThread,0);			
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

UINT AFX_CDECL ConfigGnssDozeModeThread(LPVOID param)
{
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(CGPSDlg::gpsDlg->m_inputMsg, 9, "ConfigGnssDozeMode successfully");
	return 0;
}

void CGPSDlg::OnConfigGnssDozeMode()
{
	if(!CheckConnect())
	{
		return;
	}

	SetInputMode(NoOutputMode);
	{
		U08 msg[2] = {0};
		msg[0] = 0x64;
		msg[1] = 0x1C;

		int len = SetMessage(msg, sizeof(msg));
        AfxBeginThread(ConfigGnssDozeModeThread, 0);
	}
}

#include "GpsdoDownload.h"
UINT DownloadThread(LPVOID pParam);
void CGPSDlg::OnGpsdoFirmwareDownload()
{
	if(!CheckConnect())
	{
		return;
	}

	SetInputMode(NoOutputMode);
	CGpsdoDownload dlg;
	if(IDOK != dlg.DoModal())
	{
		SetMode();  
		CreateGPSThread();
		return;
	}

	m_nDownloadBaudIdx = dlg.m_downloadBaudIdx;
	m_nDownloadBufferIdx = 0;
	m_DownloadMode = GpsdoMasterSlave;
	m_strDownloadImage = dlg.m_strMasterPath;
	m_strDownloadImage2 = dlg.m_strSlavePath;
	//m_nSlaveSourceBaud = dlg.m_downloadBaudIdx;
	m_masterFwBaudIdx = dlg.m_masterFwBaudIdx;

	::AfxBeginThread(DownloadThread, 0);
}

bool CGPSDlg::DoCommonConfig(CCommonConfigDlg* dlg)
{
	if(!CheckConnect())
	{
		return false;
	}

	SetInputMode(NoOutputMode);
	INT_PTR nResult = dlg->DoModal();
	if(nResult == IDOK) 
	{
		dlg->DoCommand();
    return true;
	}
	else
	{
		SetMode();  
		CreateGPSThread();
	}
  return false;
}

bool CGPSDlg::DoCommonQuery(CCommonQueryDlg* dlg)
{
  if(!CheckConnect())
	{
		return false;
	}

	SetInputMode(NoOutputMode);
	INT_PTR nResult = dlg->DoModal();
	if(nResult == IDOK) 
	{
		dlg->DoCommand();
    return true;
	}
	else
	{
		SetMode();  
		CreateGPSThread();
	}
  return false;
}

void CGPSDlg::DoCommonConfigNoDisconnect(CCommonConfigDlg* dlg)
{
	INT_PTR nResult = dlg->DoModal();
	if(nResult == IDOK) 
	{
		dlg->DoCommand();
	}
}

void CGPSDlg::DoCommonConfigDirect(CCommonConfigDlg* dlg, int type)
{
	if(!CheckConnect())
	{
		return;
	}

	SetInputMode(NoOutputMode);
	INT_PTR nResult = dlg->DoDirect(type);
	if(nResult == IDOK) 
	{
		dlg->DoCommand();
	}
	else
	{
		SetMode();  
		CreateGPSThread();
	}
}

bool CGPSDlg::DoCConfigRegisterDirect(U32 addr, U32 value)
{
	CWaitCursor wait;
	BinaryData cmd(9);
	*cmd.GetBuffer(0) = 0x72;
	*cmd.GetBuffer(1) = addr >> 24 & 0xFF;
	*cmd.GetBuffer(2) = addr >> 16 & 0xFF;
	*cmd.GetBuffer(3) = addr >>  8 & 0xFF;
	*cmd.GetBuffer(4) = addr >>  0 & 0xFF;

	*cmd.GetBuffer(5) = value >> 24 & 0xFF;
	*cmd.GetBuffer(6) = value >> 16 & 0xFF;
	*cmd.GetBuffer(7) = value >>  8 & 0xFF;
	*cmd.GetBuffer(8) = value >>  0 & 0xFF;
  
  BinaryCommand configCmd;
	configCmd.SetData(cmd);
	return ExecuteConfigureCommand(configCmd.GetBuffer(), configCmd.Size(), "", 0);
}

bool CGPSDlg::DoCConfigRfIcDirect(U08 type, U32* reg, int size)
{
	CWaitCursor wait;
  int idx = 0;
	BinaryData cmd(87);
	*cmd.GetBuffer(idx++) = 0x64;
	*cmd.GetBuffer(idx++) = 0x75;
  //U08
	*cmd.GetBuffer(idx++) = type;
  for(int i = 0; i < size; ++i)
  {
	  //U32 
	  *cmd.GetBuffer(idx++) = HIBYTE(HIWORD(reg[i]));
	  *cmd.GetBuffer(idx++) = LOBYTE(HIWORD(reg[i]));
	  *cmd.GetBuffer(idx++) = HIBYTE(LOWORD(reg[i]));
	  *cmd.GetBuffer(idx++) = LOBYTE(LOWORD(reg[i]));
  }

  BinaryCommand configCmd;
	configCmd.SetData(cmd);
	return ExecuteConfigureCommand(configCmd.GetBuffer(), configCmd.Size(), "Configure RF IC successfully", 0);
}

void CGPSDlg::OnBinaryConfigureQZSS()
{
	CConfigQZSS dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnBinaryConfigureDGPS()
{
	CConfigDGPS dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnBinaryConfigureSmoothMode()
{
	CConfigSmoothMode dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnBinaryConfigTimeStamping()
{
	CConfigTimeStamping dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigLeapSeconds()
{
	CConfigLeapSeconds dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigParamSearchEngineSleepCriteria()
{
	CConfigParamSearchEngineSleepCriteria dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigDatumIndex()
{
	CConfigDatumIndex dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigVeryLowSpeed()
{
	CConfigVeryLowSpeed dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigCpuBoostMode()
{
	CConfigCpuBoostMode dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigDofunUniqueId()
{
	CConfigDofunUniqueId dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnEraseDofunUniqueId()
{
	CConfigDofunUniqueId dlg;
	DoCommonConfigDirect(&dlg, 0);
}

void CGPSDlg::OnBinaryConfigureSBAS()
{
	CConfigSBAS dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnBinaryConfigureSBAS2()
{
	CConfigSBAS2 dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnBinaryConfigureSAGPS()
{
	CConfigSAEE dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigureInterferenceDetectControl()
{
	CConfigInterferenceDetectControl dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigNMEABinaryOutputDestination()
{
	CConfigNMEABinaryOutputDestination dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigParameterSearchEngineNumber()
{
	CConfigParameterSearchEngineNumber dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigPositionFixNavigationMask()
{
	CConfigPositionFixNavigationMask dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigRefTimeSyncToGpsTime()
{
	ConfigRefTimeToGpsTimeDlg dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigGnssConstellationType()
{
	ConfigGnssConstellationTypeDlg dlg;
  if(DoCommonConfig(&dlg))
  {
 	  DeleteNmeaMemery();
	  ClearInformation();
  }
}

void CGPSDlg::OnConfigBinaryMeasurementDataOut()
{
	ConfigBinaryMeasurementDataOutDlg dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigRtcmMeasurementDataOut()
{
	ConfigRtcmMeasurementDataOutDlg dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnBinaryConfigurepowermode()
{
	CConfigPowerMode dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnSup800EraseData()
{
	CSUP800EraseUserDataDlg dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnSup800WriteData()
{
	CSUP800WriteUserDataDlg dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnSup800ReadData()
{
	CSUP800ReadUserDataDlg dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigureSignalDisturbanceStatus()
{
	CConfigureSignalDisturbanceStatusDlg dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigureGpsUtcLeapSecondsInUtc()
{
	CConfigureGpsUtcLeapSecondsInUtcDlg dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigureNoisePowerControl()
{
	CConfigNoisePowerControlDlg dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigPowerSavingParameters()
{
	ConfigPowerSavingParametersRomDlg dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnIqPlot()
{
	CIqPlotDlg dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnNtripClient()
{
	if(!m_isConnectOn)
	{
		AfxMessageBox("Please connect to GNSS device");
		return;
	}

  if(m_ntripClientDlg == NULL)
  {
    m_ntripClientDlg = new CNtripClientDlg(this);
    m_ntripClientDlg->Create(IDD_NTRIP_CLIENT);
  }
  m_ntripClientDlg->ShowWindow(SW_SHOW);
}

void CGPSDlg::OnTcpipServer()
{
  CString txt, file;
	txt = Utility::GetSpecialFolder(CSIDL_APPDATA);
	txt += "\\GNSS_Viewer_V2";
	::CreateDirectory(txt, NULL);

	file = txt + "\\CommunicationBase.dll";
	Utility::CopyResToFile(file, IDR_COMM_DLL, "EXEC");

	file = txt + "\\ViewerTcpipServer.exe";
	Utility::CopyResToFile(file, IDR_TCPIP_SERVER, "EXEC");
	Utility::ExecuteExeNoWait(file);
}

void CGPSDlg::OnConfigGeofence()
{
	CConfigGeofencing dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigIoRegister()
{
  CConfigRegisterDlg dlg;
  dlg.SetCommandType(CConfigRegisterDlg::Register);
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigMultiRfIc()
{
  CConfigRegisterDlg dlg;
  dlg.SetCommandType(CConfigRegisterDlg::RfIcRegister);
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigGeofence1()
{
	CConfigGeofencing dlg;
	dlg.SetDataNo(1);
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigGeofence2()
{
	CConfigGeofencing dlg;
	dlg.SetDataNo(2);
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigGeofence3()
{
	CConfigGeofencing dlg;
	dlg.SetDataNo(3);
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigGeofence4()
{
	CConfigGeofencing dlg;
	dlg.SetDataNo(4);
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnClearGeofenceAll()
{
	CConfigGeofencing dlg;
  dlg.SetDataNo(1);
	DoCommonConfigDirect(&dlg, 0);

  dlg.SetDataNo(2);
	DoCommonConfigDirect(&dlg, 0);

  dlg.SetDataNo(3);
	DoCommonConfigDirect(&dlg, 0);

  dlg.SetDataNo(4);
	DoCommonConfigDirect(&dlg, 0);
	return;
}

void CGPSDlg::OnClearGeofence1()
{
	CConfigGeofencing dlg;
  dlg.SetDataNo(1);
	DoCommonConfigDirect(&dlg, 0);
	return;
}

void CGPSDlg::OnClearGeofence2()
{
	CConfigGeofencing dlg;
  dlg.SetDataNo(2);
	DoCommonConfigDirect(&dlg, 0);
	return;
}

void CGPSDlg::OnClearGeofence3()
{
	CConfigGeofencing dlg;
  dlg.SetDataNo(3);
	DoCommonConfigDirect(&dlg, 0);
	return;
}

void CGPSDlg::OnClearGeofence4()
{
	CConfigGeofencing dlg;
  dlg.SetDataNo(4);
	DoCommonConfigDirect(&dlg, 0);
	return;
}

void CGPSDlg::OnConfigRtkMode()
{
	CConfigRtkMode dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigWatchTrackback()
{
	CConfigWatchTrackback dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigRtkParameters()
{
	CConfigRtkParameters dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigRtkCpifBias()
{
	CConfigRtkGlCpifBias dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigRtkElevationAndCnrMask()
{
	CConfigRtkElevationAndCnrMask dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnRtkReset()
{
	CConfigRtkReset dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigRtkMode2()
{
	CConfigRtkMode2 dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigRtkFunctions()
{
	CConfigRtkFunctions dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigRtkSlaveBaud()
{
	CConfigRtkSlaveBaud dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigRtkKinematicBaud()
{
	CConfigRtkKinematicBaud dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigBasePosition()
{
	CConfigRtkMode2 dlg;
  dlg.SetCommandMode(CConfigRtkMode2::CfgBasePosition);
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigMessageOut()
{
	CConfigMessageOut dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigSubSecRegister()
{
	CConfigSubSecRegister dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigTiming()
{
	//CConfigTiming dlg;
	CConfigRtkMode2 dlg;
  dlg.SetCommandMode(CConfigRtkMode2::CfgTiming);
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigTimingCableDelay()
{
	CConfigTimingCableDelayDlg dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigGpsMeasurementMode()
{
	CConfigGpsMeasurementMode dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigPstmDeviceAddress()
{
	CConfigPscmDeviceAddress dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigPstmLatLonDigits()
{
	CConfigPscmLatLonFractionalDigits dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigPsti020()
{
	CConfigPstiInterval dlg;
  dlg.SetPsti(20);
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigPsti030()
{
	CConfigPstiInterval dlg;
  dlg.SetPsti(30);
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigPsti032()
{
	CConfigPstiInterval dlg;
  dlg.SetPsti(32);
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigPsti033()
{
	CConfigPstiInterval dlg;
  dlg.SetPsti(33);
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigPsti060()
{
	CConfigPstiInterval dlg;
  dlg.SetPsti(60);
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigPsti063()
{
	CConfigPstiInterval dlg;
  dlg.SetPsti(63);
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigPsti065()
{
	CConfigPstiInterval dlg;
  dlg.SetPsti(65);
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigPsti067()
{
	CConfigPstiInterval dlg;
  dlg.SetPsti(67);
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigPsti068()
{
	CConfigPstiInterval dlg;
  dlg.SetPsti(68);
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigPsti070()
{
	CConfigPstiInterval dlg;
  dlg.SetPsti(70);
	DoCommonConfig(&dlg);
}

//void CGPSDlg::OnConfigPsti004()
//{
//	CConfigPstiInterval dlg;
//  dlg.SetPsti(04);
//	DoCommonConfig(&dlg);
//}

void CGPSDlg::OnConfigPsti007()
{
	CConfigPstiInterval dlg;
  dlg.SetPsti(07);
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigPsti005()
{
	CConfigPstiInterval dlg;
  dlg.SetPsti(05);
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigNavicMessageInterval()
{
	CConfigPstiInterval dlg;
  dlg.SetPsti(00, CConfigPstiInterval::Navic);
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnRtkOnOffGpsSv()
{
	CRtkOnOffSv dlg;
  dlg.SetSvMode(CRtkOnOffSv::RtkSvGps);
	DoCommonConfigNoDisconnect(&dlg);
}

void CGPSDlg::OnRtkOnOffSbasQzssSv()
{
	CRtkOnOffSv dlg;
  dlg.SetSvMode(CRtkOnOffSv::RtkSvSbasQzss);
	DoCommonConfigNoDisconnect(&dlg);
}

void CGPSDlg::OnRtkOnOffGlonassSv()
{
	CRtkOnOffSv dlg;
  dlg.SetSvMode(CRtkOnOffSv::RtkSvGlonass);
	DoCommonConfigNoDisconnect(&dlg);
}

void CGPSDlg::OnRtkOnOffBeidouSv()
{
	CRtkOnOffSv dlg;
  dlg.SetSvMode(CRtkOnOffSv::RtkSvBeidou2);
	DoCommonConfigNoDisconnect(&dlg);
}

void CGPSDlg::OnRtkOnOffGalileoSv()
{
	CRtkOnOffSv dlg;
  dlg.SetSvMode(CRtkOnOffSv::RtkSvGalileo);
	DoCommonConfigNoDisconnect(&dlg);
}

void CGPSDlg::OnQueryRtkReferencePosition()
{
	CConfigRtkReferencePosition dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigDrRate()
{
	CDrUpdateRateDlg dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigDrRawRate()
{
	CDrRawRateDlg dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigDrMemsNoiseLevel()
{
	CDrMemsNoiseLevelDlg dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigAdrOdometerScalingFactor()
{ 
	CAdrOdometerScalingFactorDlg dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigAlphaKey()
{
	CConfigureAlphaKeyDlg dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigV9AesTag()
{
  CConfigureAlphaKeyDlg dlg;
  dlg.SetCommandType(CConfigureAlphaKeyDlg::V9AesTag);
  DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigTrackingModuleParameterSetting()
{
  CTrackingModuleParameterSettingDlg dlg;
  //DoCommonConfig(&dlg);
  INT_PTR ret = dlg.DoModal();
	//BinaryChecksumCalDlg dlg;
 // dlg.SetMode(BinaryChecksumCalDlg::RawData);
	//INT_PTR ret = dlg.DoModal();
}

void CGPSDlg::OnConfigIfreeMode()
{
  CConfigIfreeModeDlg dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnConfigCustomStringInterval()
{
  CConfigCustomStringIntervalDlg dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnQueryCustomStringInterval()
{
  CQueryCustomStringIntervalDlg dlg;
  DoCommonQuery(&dlg);
}

void CGPSDlg::OnConfigure1PpsPulseWidth()
{
  CConfig1PpsPulseWidthDlg dlg;
  DoCommonConfig(&dlg);
}

void CGPSDlg::OnQueryStringInterval(UINT id)
{
  const char* NmeaMiscCode[] = { 
    "GGA", 
    "GNS", 
    "GSA", 
    "GSV", 
    "GLL", 
    "RMC", 
    "VTG", 
    "ZDA", 
    "DTM", 
    "GBS", 
    "GRS", 
    "GST", 
    "THS", 
    "HDT"};

	if(!CheckConnect())
	{
		return;
	}

	QueryNmeaStringX(CGPSDlg::Display, NmeaMiscCode[id - ID_QRY_GGA_STR_INTVAL], NULL);

	SetMode();
	CreateGPSThread();
}

//void CGPSDlg::OnResetV9AesTag()
//{
//  U32 tagAddr = 0xFE999996;
//  U32 regData = 0;
//  CString msg;
//
//  CmdErrorCode err = CGPSDlg::gpsDlg->QueryRegisterx(CGPSDlg::Return, tagAddr, &regData);
//	if(Ack != err)
//  {
//    ::AfxMessageBox("Reset Phoenix Tag failed!");  
//    return;
//  }
//
//  Sleep(1200);
//  tagAddr = 0xFE999998;
//  err = CGPSDlg::gpsDlg->QueryRegisterx(CGPSDlg::Return, tagAddr, &regData);
//	if(Ack != err)
//  {
//    ::AfxMessageBox("Reset Phoenix Tag failed!");  
//    return;
//  }
//
//  ::AfxMessageBox("Phoenix tag has been reset, please download the firmware again.");  
//}

void CGPSDlg::OnConfigQueryPstiInterval()
{
	CConfigQueryPstiIntervalDlg dlg;
	DoCommonConfig(&dlg);
}


void CGPSDlg::OnBnClickedNoOutput()
{
	CConfigMessageOut dlg;
	DoCommonConfigDirect(&dlg, 0);
	return;
}

void CGPSDlg::OnBnClickedNmeaOutput()
{
#if(PROUCTION_TEST_200611)
	CConfigRtkMode2 dlg;
	DoCommonConfigDirect(&dlg, 0);
#else
	CConfigMessageOut dlg;
	DoCommonConfigDirect(&dlg, 1);
#endif
	return;
}

void CGPSDlg::OnBnClickedBinaryOutput()
{
#if(PROUCTION_TEST_200611)
	CConfigRtkMode2 dlg;
	DoCommonConfigDirect(&dlg, 1);
#else
	CConfigMessageOut dlg;
	DoCommonConfigDirect(&dlg, 2);
#endif
	return;
}

void CGPSDlg::OnConfigV9PowerSaveByRtc30()
{
	CConfigV9PowerSave dlg;
  DoCommonConfigDirect(&dlg, CConfigV9PowerSave::DD_RTC_Timer_30);
	return;
}

void CGPSDlg::OnConfigV9PowerSaveByRtc60()
{
	CConfigV9PowerSave dlg;
  DoCommonConfigDirect(&dlg, CConfigV9PowerSave::DD_RTC_Timer_60);
	return;
}

void CGPSDlg::OnConfigV9PowerSaveByRtc90()
{
	CConfigV9PowerSave dlg;
  DoCommonConfigDirect(&dlg, CConfigV9PowerSave::DD_RTC_Timer_90);
	return;
}

void CGPSDlg::OnConfigV9PowerSaveByRtc120()
{
	CConfigV9PowerSave dlg;
  DoCommonConfigDirect(&dlg, CConfigV9PowerSave::DD_RTC_Timer_120);
	return;
}

void CGPSDlg::ConfigV9ClockToGpio0(bool on)
{  
  U32 miscRegAddr;
  U32 regData = 0;
  CString msg;
  CmdErrorCode err;

  if(on)
  { //GPIO0 must be output for Clock out in GPIO0
    miscRegAddr = 0x2000100C;
    err = CGPSDlg::gpsDlg->QueryRegisterx(CGPSDlg::Return, miscRegAddr, &regData);
	  if(Ack != err)
    {
	    msg.Format("Configure Phoenix RF clock to GPIO0 %s failed", (on) ? "on" : "off");  
      add_msgtolist(msg);
      return;
    }
    regData = regData & (~0x00000001);
	  if(!DoCConfigRegisterDirect(miscRegAddr, regData))
    {
	    msg.Format("Configure Phoenix RF clock to GPIO0 %s failed", (on) ? "on" : "off");  
      add_msgtolist(msg);
      return;
    }
  }
  
  miscRegAddr = 0x2000F014;
  err = CGPSDlg::gpsDlg->QueryRegisterx(CGPSDlg::Return, miscRegAddr, &regData);
	if(Ack != err)
  {
	  msg.Format("Configure Phoenix RF clock to GPIO0 %s failed", (on) ? "on" : "off");  
    add_msgtolist(msg);
    return;
  }

  if(on)
  {
    regData = regData | 0x00000100;
  }
  else
  {
    regData = regData & (~0x00000100);
  }

	if(DoCConfigRegisterDirect(miscRegAddr, regData))
	{
    msg.Format("Configure Phoenix RF clock to GPIO0 %s successfully", (on) ? "on" : "off");  
  }
  else
  {
	  msg.Format("Configure Phoenix RF clock to GPIO0 %s failed", (on) ? "on" : "off");  
  }
  add_msgtolist(msg);
}

void CGPSDlg::OnConfigV9ClockToGpio0On()
{
	if(!CheckConnect())
	{
		return;
	}

	SetInputMode(NoOutputMode);
	ConfigV9ClockToGpio0(true);

	SetMode();  
	CreateGPSThread();
	return;
}

void CGPSDlg::OnConfigV9ClockToGpio0Off()
{
	if(!CheckConnect())
	{
		return;
	}

	SetInputMode(NoOutputMode);
	ConfigV9ClockToGpio0(false);

	SetMode();  
	CreateGPSThread();
	return;
}

void CGPSDlg::ConfigV9ClockOut(bool on)
{
  const U32 miscRegAddr = 0x2000F080;
  U32 regData = 0;
  CString msg;

  CmdErrorCode err = CGPSDlg::gpsDlg->QueryRegisterx(CGPSDlg::Return, miscRegAddr, &regData);
	if(Ack != err)
  {
	  msg.Format("Configure Phoenix RF clock out %s failed", (on) ? "enable" : "disable");  
    add_msgtolist(msg);
    return;
  }

  if(on)
  {
    regData = regData | 0x00020000;
  }
  else
  {
    regData = regData & (~0x00020000);
  }

	if(DoCConfigRegisterDirect(miscRegAddr, regData))
	{
    msg.Format("Configure Phoenix RF clock out %s successfully", (on) ? "enable" : "disable");  
  }
  else
  {
	  msg.Format("Configure Phoenix RF clock out %s failed", (on) ? "enable" : "disable");  
  }
  add_msgtolist(msg);
}

void CGPSDlg::OnConfigV9ClockOutOn()
{
	if(!CheckConnect())
	{
		return;
	}

	SetInputMode(NoOutputMode);
	ConfigV9ClockOut(true);

	SetMode();  
	CreateGPSThread();
	return;
}

void CGPSDlg::OnConfigV9ClockOutOff()
{
	if(!CheckConnect())
	{
		return;
	}

	SetInputMode(NoOutputMode);
	ConfigV9ClockOut(false);

	SetMode();  
	CreateGPSThread();
	return;
}

void CGPSDlg::OnInsdrTest()
{
	if(!CheckConnect())
	{
		return;
	}

	DrTestDlg dlg;
  dlg.DoModal();

	SetMode();
	CreateGPSThread();
}

void CGPSDlg::OnPx100ExternalBurning()
{
	if(!CheckConnect())
	{
		return;
	}

  CmdErrorCode err = Timeout;
	CGPSDlg::DoCConfigRegisterDirect(0x2000F090, 0x03020100);
	CGPSDlg::DoCConfigRegisterDirect(0x2000F0A0, 0x03020100);
	CGPSDlg::DoCConfigRegisterDirect(0x2000F094, 0x070C0504);
	CGPSDlg::DoCConfigRegisterDirect(0x2000F0A4, 0x070C0504);
	CGPSDlg::DoCConfigRegisterDirect(0x2000F098, 0x0B0A0908);
	CGPSDlg::DoCConfigRegisterDirect(0x2000F0A8, 0x0B0A0908);
	CGPSDlg::DoCConfigRegisterDirect(0x2000F09C, 0x0F0E0D06);
	CGPSDlg::DoCConfigRegisterDirect(0x2000F0AC, 0x0F0E0D06);
	CGPSDlg::DoCConfigRegisterDirect(0x2000F124, 0x00000001);

	SetMode();
	CreateGPSThread();
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryVeryLowSpeed(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryVeryLowSpeedCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryVeryLowSpeedCmd].cmdId);
	cmd.SetU08(2, cmdTable[QueryVeryLowSpeedCmd].cmdSubId);

	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(QueryVeryLowSpeedCmd, &cmd, &ackCmd))
	{
		if(nMode == Return)
		{
			*((U08*)outputData) = ackCmd[6];
			return Ack;
		}

		CString strMsg = "Query kernel very low speed successfully";
		add_msgtolist(strMsg);
		strMsg = "Mode: ";
		//add_msgtolist(strMsg);
		if(0 == (ackCmd[6]))
		{
			strMsg += "Disable";
		}
		else if(1 == (ackCmd[6]))
		{
			strMsg += "Enable";
		}
		add_msgtolist(strMsg);
	}
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryRfIc(CmdExeMode nMode, void* outputData, U08 type)
{
	BinaryCommand cmd(cmdTable[QueryRfIcCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryRfIcCmd].cmdId);
	cmd.SetU08(2, cmdTable[QueryRfIcCmd].cmdSubId);
	cmd.SetU08(3, type);

	BinaryData ackCmd;
  CmdErrorCode err = ExcuteBinaryCommand(QueryRfIcCmd, &cmd, &ackCmd);
	if(err != Ack)
	{
    return err;
  }

	if(Return == nMode)
	{	//Return command data
    *((BinaryData*)outputData) = ackCmd;
		return err;
	}
/*
	CString strMsg = "Query Rf IC successfully";
	add_msgtolist(strMsg);
	strMsg.Format("R02 : 0x%08X", ConvertLeonU32(ackCmd.Ptr(6)));
	add_msgtolist(strMsg);
	strMsg.Format("R04 : 0x%08X", ConvertLeonU32(ackCmd.Ptr(10)));
	add_msgtolist(strMsg);
	strMsg.Format("R06 : 0x%08X", ConvertLeonU32(ackCmd.Ptr(14)));
	add_msgtolist(strMsg);
	strMsg.Format("R10 : 0x%08X", ConvertLeonU32(ackCmd.Ptr(18)));
	add_msgtolist(strMsg);
	add_msgtolist(strMsg);
	strMsg.Format("R12 : 0x%08X", ConvertLeonU32(ackCmd.Ptr(22)));
	add_msgtolist(strMsg);
	strMsg.Format("R13 : 0x%08X", ConvertLeonU32(ackCmd.Ptr(26)));
	add_msgtolist(strMsg);
	strMsg.Format("R14 : 0x%08X", ConvertLeonU32(ackCmd.Ptr(30)));
	add_msgtolist(strMsg);
*/
	return err;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryOneRfRegister(CmdExeMode nMode, void* outputData, U08 id)
{
	BinaryCommand cmd(cmdTable[QueryOneRfRegisterCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryOneRfRegisterCmd].cmdId);
	cmd.SetU08(2, cmdTable[QueryOneRfRegisterCmd].cmdSubId);
	cmd.SetU08(3, id);

	BinaryData ackCmd;
  CmdErrorCode err = ExcuteBinaryCommand(QueryOneRfRegisterCmd, &cmd, &ackCmd);
	if(err != Ack)
	{
    return err;
  }

	if(Return == nMode)
	{	//Return command data
    *((BinaryData*)outputData) = ackCmd;
		return err;
	}
/*
	CString strMsg = "Query Rf IC successfully";
	add_msgtolist(strMsg);
	strMsg.Format("R02 : 0x%08X", ConvertLeonU32(ackCmd.Ptr(6)));
	add_msgtolist(strMsg);
	strMsg.Format("R04 : 0x%08X", ConvertLeonU32(ackCmd.Ptr(10)));
	add_msgtolist(strMsg);
	strMsg.Format("R06 : 0x%08X", ConvertLeonU32(ackCmd.Ptr(14)));
	add_msgtolist(strMsg);
	strMsg.Format("R10 : 0x%08X", ConvertLeonU32(ackCmd.Ptr(18)));
	add_msgtolist(strMsg);
	add_msgtolist(strMsg);
	strMsg.Format("R12 : 0x%08X", ConvertLeonU32(ackCmd.Ptr(22)));
	add_msgtolist(strMsg);
	strMsg.Format("R13 : 0x%08X", ConvertLeonU32(ackCmd.Ptr(26)));
	add_msgtolist(strMsg);
	strMsg.Format("R14 : 0x%08X", ConvertLeonU32(ackCmd.Ptr(30)));
	add_msgtolist(strMsg);
*/
	return err;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryAlphaUniqueId(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryAlphaUniqueIdCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryAlphaUniqueIdCmd].cmdId);
	cmd.SetU08(2, cmdTable[QueryAlphaUniqueIdCmd].cmdSubId);

	BinaryData ackCmd;
  CmdErrorCode err = ExcuteBinaryCommand(QueryAlphaUniqueIdCmd, &cmd, &ackCmd);
	if(err != Ack)
	{
    return err;
  }

	if(Return == nMode)
	{	//Return command data
    *((BinaryData*)outputData) = ackCmd;
		return err;
	}

	CString strMsg = "Query Alpha Unique ID successfully";
	add_msgtolist(strMsg);
	strMsg.Format("Unique ID:");
	add_msgtolist(strMsg);
  strMsg.Format("%02X%02X%02X%02X%02X%02X%02X%02X", 
    ackCmd[6], ackCmd[7], ackCmd[8], ackCmd[9], 
    ackCmd[10], ackCmd[11], ackCmd[12], ackCmd[13] );
	add_msgtolist(strMsg);

	return err;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryV9TagAddress(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryV9TagAddressCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryV9TagAddressCmd].cmdId);
	cmd.SetU08(2, cmdTable[QueryV9TagAddressCmd].cmdSubId);

	BinaryData ackCmd;
  CmdErrorCode err = ExcuteBinaryCommand(QueryV9TagAddressCmd, &cmd, &ackCmd);
	if(err != Ack)
	{
    return err;
  }

	if(Return == nMode)
	{	//Return command data
    *((BinaryData*)outputData) = ackCmd;
		return err;
	}

  U32 tagAddr = ConvertLeonU32(ackCmd.Ptr(6));
	CString strMsg = "Query Phoenix Tag Address successfully";
	add_msgtolist(strMsg);
	strMsg.Format("Tag Address:");
	add_msgtolist(strMsg);
	strMsg.Format("%8X", tagAddr);
	add_msgtolist(strMsg);

	return err;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryV9UniqueId(CmdExeMode nMode, void* outputData)
{
  return QueryAlphaUniqueId(nMode, outputData);
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryPhoenixSoftwareFeature(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryPhoenixSoftwareFeatureCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryPhoenixSoftwareFeatureCmd].cmdId);
	cmd.SetU08(2, cmdTable[QueryPhoenixSoftwareFeatureCmd].cmdSubId);

	BinaryData ackCmd;
  CmdErrorCode err = ExcuteBinaryCommand(QueryPhoenixSoftwareFeatureCmd, &cmd, &ackCmd);
	if(err != Ack)
	{
    return err;
  }

	if(Return == nMode)
	{	//Return command data
    *((BinaryData*)outputData) = ackCmd;
		return err;
	}

	CString strMsg = "Query Software Feature successfully";
	add_msgtolist(strMsg);

  U32 swFeature = ConvertLeonU32(ackCmd.Ptr(6));
	strMsg.Format("Software Feature: %08X", swFeature);
	add_msgtolist(strMsg);
  const char *featureTable[] = {
    "GPS L1 C/A",
    "GPS L1C",
    "GPS L2C",
    "GPS L5",
    "BD B1I",
    "BD B1C",
    "BD B2A",
    "BD B2I",
    "BD B3I",
    "Glonass L1",
    "Glonass L2",
    "Glonass L3",
    "Galileo E1",
    "Galileo E5a",
    "Galileo E5b",
    "Galileo E6",
    "QZSS L1 C/A",
    "QZSS L1C",
    "QZSS L2C",
    "QZSS L5",
    "QZSS LEX",
    "SBAS L1",
    "IRNSS L5" };
  for(int i = 0; i <= 22; ++i)
  {
    U32 f = 1 << i;
    if((f & swFeature) != 0)
    {
      strMsg = featureTable[i];
	    add_msgtolist(strMsg);
    }
  }
	return err;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryV9ExtendedId(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryExtendedIdCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryExtendedIdCmd].cmdId);
	cmd.SetU08(2, cmdTable[QueryExtendedIdCmd].cmdSubId);

	BinaryData ackCmd;
  CmdErrorCode err = ExcuteBinaryCommand(QueryExtendedIdCmd, &cmd, &ackCmd);
	if(err != Ack)
	{
    return err;
  }

	if(Return == nMode)
	{	//Return command data
    *((BinaryData*)outputData) = ackCmd;
		return err;
	}

	CString strMsg = "Query Extended ID successfully";
	add_msgtolist(strMsg);
	strMsg.Format("Extended ID: ");
	add_msgtolist(strMsg);
  int len = ackCmd[6];
  strMsg.Empty();
  for(int i = 0; i < len; ++i)
  {
    CString tmp;
    tmp.Format("%02X", ackCmd[7 + i]);
    strMsg += tmp;
  }
	add_msgtolist(strMsg);

	return err;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryV9Tag(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryV9TagCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryV9TagCmd].cmdId);
	cmd.SetU08(2, cmdTable[QueryV9TagCmd].cmdSubId);

	BinaryData ackCmd;
  CmdErrorCode err = ExcuteBinaryCommand(QueryV9TagCmd, &cmd, &ackCmd);
	if(err != Ack)
	{
    return err;
  }

	if(Return == nMode)
	{	//Return command data
    *((BinaryData*)outputData) = ackCmd;
		return err;
	}

	CString strMsg = "Query Phoenix Tag successfully";
	add_msgtolist(strMsg);
	strMsg.Format("Phoenix Tag:");
	add_msgtolist(strMsg);
  strMsg.Format("%02X%02X%02X%02X%02X%02X%02X%02X", 
    ackCmd[6], ackCmd[7], ackCmd[8], ackCmd[9], 
    ackCmd[10], ackCmd[11], ackCmd[12], ackCmd[13] );
	add_msgtolist(strMsg);

	return err;
}

CGPSDlg::CmdErrorCode CGPSDlg::QuerySecurityTag(CmdExeMode nMode, void* outputData)
{
  int ndx = 1;
	BinaryCommand cmd(cmdTable[QuerySecurityTagCmd].cmdSize);
	cmd.SetU08(ndx++, cmdTable[QuerySecurityTagCmd].cmdId);
	cmd.SetU08(ndx++, cmdTable[QuerySecurityTagCmd].cmdSubId);
  //Class
  cmd.SetU08(ndx++, 0x00);
  cmd.SetU08(ndx++, 0x00);
  cmd.SetU08(ndx++, 0x00);
  cmd.SetU08(ndx++, 0x01);
  //UID length
  cmd.SetU08(ndx++, 12);
  //UID
  cmd.SetU08(ndx++, 0x54);
  cmd.SetU08(ndx++, 0x5A);
  cmd.SetU08(ndx++, 0x5D);
  cmd.SetU08(ndx++, 0x44);
  cmd.SetU08(ndx++, 0x41);
  cmd.SetU08(ndx++, 0x35);
  cmd.SetU08(ndx++, 0xE2);
  cmd.SetU08(ndx++, 0x55);
  cmd.SetU08(ndx++, 0x54);
  cmd.SetU08(ndx++, 0x74);
  cmd.SetU08(ndx++, 0x22);
  cmd.SetU08(ndx++, 0xD5);
  cmd.SetU08(ndx++, 0x00);
  cmd.SetU08(ndx++, 0x00);
  cmd.SetU08(ndx++, 0x00);
  cmd.SetU08(ndx++, 0x00);

  //TAG
  cmd.SetU08(ndx++, 0x01);
  cmd.SetU08(ndx++, 0x00);
  cmd.SetU08(ndx++, 0x00);
  cmd.SetU08(ndx++, 0x00);
  cmd.SetU08(ndx++, 0x00);
  cmd.SetU08(ndx++, 0x0A);
  cmd.SetU08(ndx++, 0x00);
  cmd.SetU08(ndx++, 0x01);

	BinaryData ackCmd;
  CmdErrorCode err = ExcuteBinaryCommand(QuerySecurityTagCmd, &cmd, &ackCmd);
	if(err != Ack)
	{
    return err;
  }

	if(Return == nMode)
	{	//Return command data
    *((BinaryData*)outputData) = ackCmd;
		return err;
	}

	CString strMsg = "Query Security Tag successfully";
	add_msgtolist(strMsg);

  U32 pdPts = ConvertLeonU32(ackCmd.Ptr(6));
  U32 pdClass = ConvertLeonU32(ackCmd.Ptr(10));
  strMsg.Format("Class:%d, Points left: %d", pdClass, pdPts);
	add_msgtolist(strMsg);

	strMsg.Format("Security Tag:");
	add_msgtolist(strMsg);
  ndx = 14;
  strMsg.Format("%02X %02X %02X %02X %02X %02X %02X %02X", 
    ackCmd[ndx + 0], ackCmd[ndx + 1], ackCmd[ndx + 2], ackCmd[ndx + 3], 
    ackCmd[ndx + 4], ackCmd[ndx + 5], ackCmd[ndx + 6], ackCmd[ndx + 7]);
	add_msgtolist(strMsg);
  ndx = 22;
  strMsg.Format("%02X %02X %02X %02X %02X %02X %02X %02X", 
    ackCmd[ndx + 0], ackCmd[ndx + 1], ackCmd[ndx + 2], ackCmd[ndx + 3], 
    ackCmd[ndx + 4], ackCmd[ndx + 5], ackCmd[ndx + 6], ackCmd[ndx + 7]);
  add_msgtolist(strMsg);
	return err;
}

CGPSDlg::CmdErrorCode CGPSDlg::QuerySecurityTagOnly(CmdExeMode nMode, void* outputData)
{
  int ndx = 1;
	BinaryCommand cmd(cmdTable[QuerySecurityTagCmd].cmdSize);
	cmd.SetU08(ndx++, cmdTable[QuerySecurityTagCmd].cmdId);
	cmd.SetU08(ndx++, cmdTable[QuerySecurityTagCmd].cmdSubId);
  //Class
  cmd.SetU08(ndx++, 0x00);
  cmd.SetU08(ndx++, 0x00);
  cmd.SetU08(ndx++, 0x00);
  cmd.SetU08(ndx++, 0x00);
  //UID length
  cmd.SetU08(ndx++, 00);
  //UID
  cmd.SetU08(ndx++, 0x00);
  cmd.SetU08(ndx++, 0x00);
  cmd.SetU08(ndx++, 0x00);
  cmd.SetU08(ndx++, 0x00);
  cmd.SetU08(ndx++, 0x00);
  cmd.SetU08(ndx++, 0x00);
  cmd.SetU08(ndx++, 0x00);
  cmd.SetU08(ndx++, 0x00);
  cmd.SetU08(ndx++, 0x00);
  cmd.SetU08(ndx++, 0x00);
  cmd.SetU08(ndx++, 0x00);
  cmd.SetU08(ndx++, 0xD5);
  cmd.SetU08(ndx++, 0x00);
  cmd.SetU08(ndx++, 0x00);
  cmd.SetU08(ndx++, 0x00);
  cmd.SetU08(ndx++, 0x00);

  //TAG
  cmd.SetU08(ndx++, 0x00);
  cmd.SetU08(ndx++, 0x00);
  cmd.SetU08(ndx++, 0x00);
  cmd.SetU08(ndx++, 0x00);
  cmd.SetU08(ndx++, 0x00);
  cmd.SetU08(ndx++, 0x00);
  cmd.SetU08(ndx++, 0x00);
  cmd.SetU08(ndx++, 0x00);

	BinaryData ackCmd;
  CmdErrorCode err = ExcuteBinaryCommand(QuerySecurityTagCmd, &cmd, &ackCmd);
	if(err != Ack)
	{
    return err;
  }

	if(Return == nMode)
	{	//Return command data
    *((BinaryData*)outputData) = ackCmd;
		return err;
	}

	CString strMsg = "Query Security Tag successfully";
	add_msgtolist(strMsg);

  U32 pdPts = ConvertLeonU32(ackCmd.Ptr(6));
  U32 pdClass = ConvertLeonU32(ackCmd.Ptr(10));
  strMsg.Format("Class:%d, Pts left: %d", pdClass, pdPts);
	add_msgtolist(strMsg);

	return err;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryV9PromAesTag(CmdExeMode nMode, void* outputData)
{
  BinaryData ackCmd;
  CmdErrorCode err = QueryV9TagAddress(Return, &ackCmd);
  if(Ack != err)
  {
    return err;
  }

  U32 tagAddr = ConvertLeonU32(ackCmd.Ptr(6));
  U32 addBackup = m_regAddress;
  U08 aesTag[16] = { 0 };
  m_regAddress = tagAddr;
  err = QueryRegister(Return, &aesTag[0]);
  m_regAddress = addBackup;
  if(err != Ack) return err;

  m_regAddress = tagAddr + 4;
  err = QueryRegister(Return, &aesTag[4]);
  m_regAddress = addBackup;
  if(err != Ack) return err;

  m_regAddress = tagAddr + 8;
  err = QueryRegister(Return, &aesTag[8]);
  m_regAddress = addBackup;
  if(err != Ack) return err;

  m_regAddress = tagAddr + 12;
  err = QueryRegister(Return, &aesTag[12]);
  m_regAddress = addBackup;
  if(err != Ack) return err;

	if(Return == nMode)
	{	//Return command data
    memcpy(outputData, aesTag, sizeof(aesTag));
		return err;
	}

	if(Return == nMode)
	{	//Return command data
    memcpy(outputData, aesTag, sizeof(aesTag[16]));
		return err;
	}

	CString strMsg = "Query Phoenix PROM Tag successfully";
	add_msgtolist(strMsg);
	strMsg.Format("Phoenix PROM Tag:");
	add_msgtolist(strMsg);
  CString tmp;
  strMsg.Empty();
  for(int i = 0; i < sizeof(aesTag) / 4; ++i)
  {
    tmp.Format("%02X %02X %02X %02X ", 
      aesTag[i * 4 + 3],
      aesTag[i * 4 + 2],
      aesTag[i * 4 + 1],
      aesTag[i * 4 + 0]);
    strMsg += tmp;
  }
	add_msgtolist(strMsg);

	return err;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryV9ExternalAesTag(CmdExeMode nMode, void* outputData)
{
  CString strMsg;
  BinaryData ackCmd;
  U32 tagAddr = 0xFC000;
  U08 aesTag[16] = { 0 };
  CmdErrorCode ack = Timeout;;

  while(1)
  {
    ack = QueryRegisterx(Return, 0x00000000, &tagAddr);
    if(ack != Ack) 
    {
      break;
    }

    if((tagAddr & 0xFF) == 0x15)
    {
      tagAddr = 0x1FC000;
    }
    else if((tagAddr & 0xFF) == 0x14)
    {
      tagAddr = 0xFC000;
    }
    else
    {
      break;
    }

    ack = QueryRegisterx(Return, tagAddr, &aesTag[0]);
    if(ack != Ack) 
    {
      break;
    }

    tagAddr += 4;
    ack = QueryRegisterx(Return, tagAddr, &aesTag[4]);
    if(ack != Ack) 
    {
      break;
    }

    tagAddr += 4;
    ack = QueryRegisterx(Return, tagAddr, &aesTag[8]);
    if(ack != Ack) 
    {
      break;
    }

    tagAddr += 4;
    ack = QueryRegisterx(Return, tagAddr, &aesTag[12]);
    if(ack != Ack) 
    {
      break;
    }

	  if(Return == nMode)
	  {	//Return command data
      memcpy(outputData, aesTag, sizeof(aesTag));
      return ack;
	  }

	  strMsg = "Query Phoenix External Tag successfully";
	  add_msgtolist(strMsg);
	  strMsg.Format("Phoenix External Tag:");
	  add_msgtolist(strMsg);
    CString tmp;
    strMsg.Empty();
    for(int i = 0; i < sizeof(aesTag) / 4; ++i)
    {
      tmp.Format("%02X %02X %02X %02X ", 
        aesTag[i * 4 + 3],
        aesTag[i * 4 + 2],
        aesTag[i * 4 + 1],
        aesTag[i * 4 + 0]);
      strMsg += tmp;
    }
	  add_msgtolist(strMsg);
    return ack;
  }

	strMsg = "Query Phoenix External Tag failed!";
	add_msgtolist(strMsg);
	return ack;
}

CGPSDlg::CmdErrorCode CGPSDlg::AutoActiveAlphaKey(CmdExeMode nMode, void* outputData)
{
  BinaryData ackCmd;
  CmdErrorCode err = QueryV9ExtendedId(Return, &ackCmd);
	if(err != Ack)
	{
    add_msgtolist("Query Extended ID failed!");
    return err;
  }

  int len = ackCmd[6];
  CString strExtId;
  for(int i = 0; i < len; ++i)
  {
    CString tmp;
    tmp.Format("%02X", ackCmd[7 + i]);
    strExtId += tmp;
  }
  add_msgtolist("Extended ID:");
  add_msgtolist(strExtId);

  err = QueryAlphaKey(Return, &ackCmd);
	if(err == NACK)
	{
    add_msgtolist("Query Alpha Key NACK");
    ::AfxMessageBox("The firmware doesn't need activation Alpha License!");
    return err;
  }
  else if(err != Ack)
  {
    add_msgtolist("Failed to activate Alpha License");
    return err;
  }
  
  CString txt, file;
	txt = Utility::GetSpecialFolder(CSIDL_APPDATA);
	txt += "\\GNSS_Viewer_V2";
	::CreateDirectory(txt, NULL);

	file = txt + "\\BouncyCastle.Crypto.dll";
	Utility::CopyResToFile(file, IDR_TAG_SERVER_DLL, "EXEC");

	file = txt + "\\TagServer.exe";
	Utility::CopyResToFile(file, IDR_TAG_SERVER, "EXEC");

  file.Format("%s %s %s %s", txt + "\\TagServer.exe", strExtId, "000000", "000000");
	CString activeKey;
  CStringW fileW(file);
  Utility::ExecuteExternalFileW(fileW, activeKey);
  //add_msgtolist(activeKey);
  if(activeKey.Left(5).CompareNoCase("error") == 0)
  {
    int n = activeKey.ReverseFind('.');
    activeKey = activeKey.Left(n);
    n = activeKey.ReverseFind('.');
    activeKey = activeKey.Left(n);
    n = activeKey.ReverseFind('.');
    activeKey = activeKey.Left(n - 3);

    //activeKey.Replace("210.65.10.238:8000", "")
    ::AfxMessageBox(activeKey);
    add_msgtolist("Failed to activate TAG");
    return Timeout;
  }

  int pos = 0;
  const int tagSize = 20;
  U08 tag[tagSize] = { 0 };
  int index = 0;
  while(1)
  {
    CString token = activeKey.Tokenize(_T(","), pos);
    if(token.IsEmpty() || index >= tagSize)
    { 
      break;
    }
    tag[index++] = atoi(token);
  }

  pos = tag[0] % 10;
  if(pos >= 5)
  {
    pos -= 5;
  }

  if(pos == 0)
  {
    ::AfxMessageBox("The server doesn't support, please contact the engineer of Viewer.");
    add_msgtolist("Failed to activate Alpha license");
    return Timeout;
  }
  
  int pattern = 0x55;
  for(int i = pos; i < pos + 16; ++i)
  {
    tag[i] ^= pattern;
    pattern = (pattern == 0x55) ? 0xAA : 0x55;
  }

  activeKey = "";
  for(int i = pos; i < pos + 16; ++i)
  {
    CString txt;
    txt.Format("%02X ", tag[i]);
    activeKey += txt;
  }
  //add_msgtolist(activeKey);

  int ndx = 1;
	BinaryCommand cmd(19);
	cmd.SetU08(ndx++, 0x7A);
	cmd.SetU08(ndx++, 0x08);
	cmd.SetU08(ndx++, 0x7D);
  for(int i = pos; i < pos + 16; ++i)
  {
	  cmd.SetU08(ndx++, tag[i]);
  }
  CGPSDlg::gpsDlg->ExecuteConfigureCommand(cmd.GetBuffer(), cmd.Size(), "Auto activate Alpha License successfully", FALSE);
  return err;
}


CGPSDlg::CmdErrorCode CGPSDlg::AutoActiveV9AesTag(CmdExeMode nMode, void* outputData)
{
  BinaryData ackCmd;
  CmdErrorCode err = QueryV9ExtendedId(Return, &ackCmd);
	if(err != Ack)
	{
    add_msgtolist("Query Extended ID failed!");
    return err;
  }

  int len = ackCmd[6];
  CString strExtId;
  for(int i = 0; i < len; ++i)
  {
    CString tmp;
    tmp.Format("%02X", ackCmd[7 + i]);
    strExtId += tmp;
  }
  add_msgtolist("Extended ID:");
  add_msgtolist(strExtId);

  err = QueryV9Tag(Return, &ackCmd);
	if(err == NACK)
	{
    add_msgtolist("Query Phoenix tag NACK");
    ::AfxMessageBox("The firmware doesn't need activation tag!");
    return err;
  }
  else if(err != Ack)
  {
    add_msgtolist("Failed to activate tag");
    return err;
  }
  
  U08 aesTag[16] = { 0 };
  err = QueryV9PromAesTag(Return, aesTag);
  if(err != Ack)
  {
    add_msgtolist("Failed to query PROM tag");
    return err;
  }

  int countFF = 0, count00 = 0;
  for(int i = 0; i < 16; ++i)
  {
    if(0xFF == aesTag[i])
    {
      ++countFF;
    }
    if(0x00 == aesTag[i])
    {
      ++count00;
    }
  }

	if(count00 == 16)
	{
    add_msgtolist("Failed to activate tag");
    ::AfxMessageBox("PROM tag is not empty, please download firmware again!");
    return err;
  }

	if(countFF != 16)
	{
    add_msgtolist("Failed to activate tag");
    ::AfxMessageBox("Another tag exists, please [Reset Pheonix Tag] and download firmware again!");
    return err;
  }

  CString strTag;
  strTag.Format("%02X%02X%02X%02X%02X%02X%02X%02X", 
    ackCmd[6], ackCmd[7], ackCmd[8], ackCmd[9], 
    ackCmd[10], ackCmd[11], ackCmd[12], ackCmd[13] );

  add_msgtolist("Phoenix Tag:");
  add_msgtolist(strTag);


  CString txt, file;
	txt = Utility::GetSpecialFolder(CSIDL_APPDATA);
	txt += "\\GNSS_Viewer_V2";
	::CreateDirectory(txt, NULL);

	file = txt + "\\BouncyCastle.Crypto.dll";
	Utility::CopyResToFile(file, IDR_TAG_SERVER_DLL, "EXEC");

	file = txt + "\\TagServer.exe";
	Utility::CopyResToFile(file, IDR_TAG_SERVER, "EXEC");

  file.Format("%s %s %s", txt + "\\TagServer.exe", strExtId, strTag);
	CString activeKey;
  CStringW fileW(file);
  Utility::ExecuteExternalFileW(fileW, activeKey);
  if(activeKey.Left(5).CompareNoCase("error") == 0)
  {
    int n = activeKey.ReverseFind('.');
    activeKey = activeKey.Left(n);
    n = activeKey.ReverseFind('.');
    activeKey = activeKey.Left(n);
    n = activeKey.ReverseFind('.');
    activeKey = activeKey.Left(n - 3);
    ::AfxMessageBox(activeKey);
    add_msgtolist("Failed to activate TAG");
    return Timeout;
  }

  int pos = 0;
  const int tagSize = 20;
  U08 tag[tagSize] = { 0 };
  int index = 0;
  while(1)
  {
    CString token = activeKey.Tokenize(_T(","), pos);
    if(token.IsEmpty() || index >= tagSize)
    { 
      break;
    }
    tag[index++] = atoi(token);
  }

  pos = tag[0] % 10;
  if(pos >= 5)
  {
    pos -= 5;
  }

  if(pos == 0)
  {
    ::AfxMessageBox("The server doesn't support, please contact the engineer of Viewer.");
    add_msgtolist("Failed to activate TAG");
    return Timeout;
  }
  
  int pattern = 0x55;
  for(int i = pos; i < pos + 16; ++i)
  {
    tag[i] ^= pattern;
    pattern = (pattern == 0x55) ? 0xAA : 0x55;
  }

  activeKey = "";
  for(int i = pos; i < pos + 16; ++i)
  {
    CString txt;
    txt.Format("%02X ", tag[i]);
    activeKey += txt;
  }
  //add_msgtolist(activeKey);

  int ndx = 1;
	BinaryCommand cmd(18);
	cmd.SetU08(ndx++, 0x64);
	cmd.SetU08(ndx++, 0x73);
  for(int i = pos; i < pos + 16; ++i)
  {
	  cmd.SetU08(ndx++, tag[i]);
  }
  CGPSDlg::gpsDlg->ExecuteConfigureCommand(cmd.GetBuffer(), cmd.Size(), "Auto activate tag successfully", FALSE);
  return err;
}

CGPSDlg::CmdErrorCode CGPSDlg::ResetV9AesTag(CmdExeMode nMode, void* outputData)
{
  U32 data = 0;
  CmdErrorCode ack = QueryRegisterx(Return, 0xFE999996, &data);
  if(ack != Ack) 
  {
    ::AfxMessageBox("Reset Phoenix tag failed!");  
    return ack;
  }

  Sleep(1200);
  ack = QueryRegisterx(Return, 0xFE999998, &data);
  if(ack != Ack) 
  {
    ::AfxMessageBox("Reset Phoenix tag failed!");  
    return ack;
  }
  ::AfxMessageBox("Phoenix tag has been reset, please download the firmware again.");  

	CString strMsg = "Reset Phoenix Tag successfully";
	add_msgtolist(strMsg);

  return ack;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryAlphaKey(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryAlphaKeyCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryAlphaKeyCmd].cmdId);
	cmd.SetU08(2, cmdTable[QueryAlphaKeyCmd].cmdSubId);
	cmd.SetU08(3, 0x7E);

	BinaryData ackCmd;
  CmdErrorCode err = ExcuteBinaryCommand(QueryAlphaKeyCmd, &cmd, &ackCmd);
	if(err != Ack)
	{
    return err;
  }

	if(Return == nMode)
	{	//Return command data
    *((BinaryData*)outputData) = ackCmd;
		return err;
	}

	CString strMsg = "Query Alpha Key successfully";
	add_msgtolist(strMsg);
	strMsg.Format("Alpha Key: %02X %02X %02X %02X %02X %02X %02X %02X %02X %02X", ackCmd[7], ackCmd[8], ackCmd[9], ackCmd[10], ackCmd[11], ackCmd[12], ackCmd[13], ackCmd[14], ackCmd[15], ackCmd[16] );
	add_msgtolist(strMsg);

	return err;
}

CGPSDlg::CmdErrorCode CGPSDlg::TmpActiveLicense(CmdExeMode nMode, void* outputData)
{
  CmdErrorCode err = Timeout;
	if(!CGPSDlg::DoCConfigRegisterDirect(0xFE00007C, 0x00000003))
	{
    return err;
  }
	CString strMsg = "Temporarily Activate License successfully";
	add_msgtolist(strMsg);

  //Enable PSTI030
  BinaryCommand cmd(5);
  cmd.SetU08(1, 0x64);
  cmd.SetU08(2, 0x21);
  cmd.SetU08(3, 30);  //PSTI030
  cmd.SetU08(4, 0x01);
  cmd.SetU08(5, 0x00);
	if(!ExecuteConfigureCommand(cmd.GetBuffer(), cmd.Size(), "", 0))
  {
    return err;
  }
  strMsg = "Set PSTI030 interval successfully";
	add_msgtolist(strMsg);

  //Enable PSTI032
  cmd.SetU08(1, 0x64);
  cmd.SetU08(2, 0x21);
  cmd.SetU08(3, 32);  //PSTI032
  cmd.SetU08(4, 0x01);
  cmd.SetU08(5, 0x00);
	if(!ExecuteConfigureCommand(cmd.GetBuffer(), cmd.Size(), "", 0))
  {
    return err;
  }
  strMsg = "Set PSTI032 interval successfully";
	add_msgtolist(strMsg);

	return err;
}

CGPSDlg::CmdErrorCode CGPSDlg::AlphaAgCalibrationUp(CmdExeMode nMode, void* outputData)
{
  BinaryCommand cmd(3);
  cmd.SetU08(1, 0x6D);
  cmd.SetU08(2, 0x07);
  cmd.SetU08(3, 0x00);  //Up
	if(!ExecuteConfigureCommand(cmd.GetBuffer(), cmd.Size(), "", 0))
  {
    return Timeout;
  }
  CString strMsg = "Sent Alpha AG Calibration Up successfully";
	add_msgtolist(strMsg);

	return Ack;
}

CGPSDlg::CmdErrorCode CGPSDlg::AlphaAgCalibrationDown(CmdExeMode nMode, void* outputData)
{
  BinaryCommand cmd(3);
  cmd.SetU08(1, 0x6D);
  cmd.SetU08(2, 0x07);
  cmd.SetU08(3, 0x01);  //Down
	if(!ExecuteConfigureCommand(cmd.GetBuffer(), cmd.Size(), "", 0))
  {
    return Timeout;
  }
  CString strMsg = "Sent Alpha AG Calibration Down successfully";
	add_msgtolist(strMsg);

	return Ack;
}

CGPSDlg::CmdErrorCode CGPSDlg::AlphaEcompassCalibration(CmdExeMode nMode, void* outputData)
{
  BinaryCommand cmd(2);
  cmd.SetU08(1, 0x6D);
  cmd.SetU08(2, 0x0A);
	if(!ExecuteConfigureCommand(cmd.GetBuffer(), cmd.Size(), "", 0))
  {
    return Timeout;
  }
  CString strMsg = "Sent Alpha E-Compass Calibration successfully";
	add_msgtolist(strMsg);

	return Ack;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryDofunUniqueId(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryDofunUniqueIdCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryDofunUniqueIdCmd].cmdId);
	cmd.SetU08(2, cmdTable[QueryDofunUniqueIdCmd].cmdSubId);
	cmd.SetU08(3, 0x04);

	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(QueryDofunUniqueIdCmd, &cmd, &ackCmd))
	{
		if(nMode==Return)
		{
			*((U08*)outputData) = ackCmd[6];
			return Ack;
		}

		CString strMsg = "Query device uinque ID successfully";
		add_msgtolist(strMsg);
		strMsg = "ID: ";
		for(int i = 0; i < 16; ++i)
		{
			strMsg += ackCmd[i + 7];
		}
		add_msgtolist(strMsg);

		strMsg = "HEX: ";
		for(int i = 0; i < 8; ++i)
		{
			CString s;
			s.Format("%02X", ackCmd[i + 7]);
			strMsg += s;
			strMsg += (i != 7) ? " " : "-";
		}
		add_msgtolist(strMsg);
		strMsg = "     ";
		for(int i = 8; i < 16; ++i)
		{
			CString s;
			s.Format("%02X", ackCmd[i + 7]);
			strMsg += s;
			strMsg += (i != 15) ? " " : "";
		}
		add_msgtolist(strMsg);
	}
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryCpuBoostMode(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryCpuBoostModeCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryCpuBoostModeCmd].cmdId);
	cmd.SetU08(2, cmdTable[QueryCpuBoostModeCmd].cmdSubId);

	BinaryData ackCmd;
	if(Ack == ExcuteBinaryCommand(QueryCpuBoostModeCmd, &cmd, &ackCmd))
	{
		if(nMode==Return)
		{
			*((U08*)outputData) = ackCmd[6];
			return Ack;
		}

		CString strMsg = "Query ISR CPU clock boost mode successfully";
		add_msgtolist(strMsg);
		strMsg = "Mode: ";
		//add_msgtolist(strMsg);
		if(0==(ackCmd[6]))
		{
			strMsg += "Disable";
		}
		else if(1==(ackCmd[6]))
		{
			strMsg += "Enable";
		}
		add_msgtolist(strMsg);
	}
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryNavicMessageInterval(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryNavicMessageIntervalCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryNavicMessageIntervalCmd].cmdId);
	cmd.SetU08(2, cmdTable[QueryNavicMessageIntervalCmd].cmdSubId);

	BinaryData ackCmd;
  CmdErrorCode err = ExcuteBinaryCommand(QueryNavicMessageIntervalCmd, &cmd, &ackCmd, (nMode == Display) ? 3000 : 1000);
  if(err != Ack)
	{
    return err;
  }

	if(Return == nMode)
	{
    *((U08*)outputData) = ackCmd[6];
		return err;
	}

	CString strMsg = "Query NavIC message interval successfully";
	add_msgtolist(strMsg);
  strMsg.Format("PIRNSF Interval: %d", ackCmd[6]);
	add_msgtolist(strMsg);
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::ReCalcuteGlonassIfb(CmdExeMode nMode, void* outputData)
{	    
	BinaryCommand cmd(2);
	cmd.SetU08(1, 0x6A);
	cmd.SetU08(2, 0x0A);

	ClearQue();
	SendToTarget(cmd.GetBuffer(), cmd.Size(), "Re-calculate GLONASS IFB successfully", true);	
	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::BinaryQueryClockOffset(CmdExeMode nMode, void* outputData)
{
  U32 temp = m_regAddress;
  U32 runTimeClkOft = 0, sramClkOft = 0, flashClkOst = 0;

  m_regAddress = 0x01;  //Runtime clock offset address
	CmdErrorCode ack = CGPSDlg::gpsDlg->QueryRegister(CGPSDlg::Return, &runTimeClkOft);
  if(Ack != ack)
  {
	  return Timeout;
  }
  m_regAddress = 0x02;  //SRAM clock offset address
	ack = CGPSDlg::gpsDlg->QueryRegister(CGPSDlg::Return, &sramClkOft);
  if(Ack != ack)
  {
	  return Timeout;
  }

  m_regAddress = 0xcdf;  //SRAM clock offset address
	ack = CGPSDlg::gpsDlg->QueryRegister(CGPSDlg::Return, &flashClkOst);
  if(Ack != ack)
  {
	  return Timeout;
  }

	CString strMsg = "Query clock offset succeeded";
	add_msgtolist(strMsg);
	strMsg.Format("Runtime Clock Offset: %d", (S32)runTimeClkOft);
	add_msgtolist(strMsg);
	strMsg.Format("SRAM Clock Offset: %d", (S32)sramClkOft);
	add_msgtolist(strMsg);
	strMsg.Format("Flash Clock Offset: %d", (S32)flashClkOst);
	add_msgtolist(strMsg);

	return Ack;
}

#if(NAVSPARK_MINI_GPIO_QUERY)
CGPSDlg::CmdErrorCode CGPSDlg::QueryGpioData(CmdExeMode nMode, void* outputData)
{
  U08 gpio = m_gpio;
	BinaryCommand cmd(cmdTable[QueryGpioDataCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryGpioDataCmd].cmdId);
	cmd.SetU08(2, cmdTable[QueryGpioDataCmd].cmdSubId);
	cmd.SetU08(3, 0x01);
	cmd.SetU08(4, gpio);

	BinaryData ackCmd;
	CmdErrorCode ack = ExcuteBinaryCommand(QueryGpioDataCmd, &cmd, &ackCmd);
  if(Ack != ack)
  {
	  return Timeout;
  }

	CString strMsg = "Query GPIO Data successfully";
	add_msgtolist(strMsg);

  strMsg.Format("GPIO %d:%s", gpio, (ackCmd[7]) ? "High" : "Low");
	add_msgtolist(strMsg);

	return Ack;
}
#endif

CGPSDlg::CmdErrorCode CGPSDlg::InsdrAccelerometerSelfTest(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[InsdrAccelerometerSelfTestCmd].cmdSize);
	cmd.SetU08(1, cmdTable[InsdrAccelerometerSelfTestCmd].cmdId);
	cmd.SetU08(2, cmdTable[InsdrAccelerometerSelfTestCmd].cmdSubId);

	BinaryData ackCmd;
	CmdErrorCode ack = ExcuteBinaryCommand(InsdrAccelerometerSelfTestCmd, &cmd, &ackCmd);
  if(Ack != ack)
  {
	  return Timeout;
  }

	if(nMode == Return)
	{
		*((U08*)outputData) = ackCmd[6];
		return ack;
	}

	CString strMsg = "INS DR accelerometer self-test successfully";
	add_msgtolist(strMsg);

  strMsg.Format("Test Status:%s",(ackCmd[6]) ? "PASS" : "FAILED");
	add_msgtolist(strMsg);

  strMsg.Format("POS X:%f", ConvertLeonFloat(ackCmd.Ptr(7)));
	add_msgtolist(strMsg);

  strMsg.Format("POS Y:%f", ConvertLeonFloat(ackCmd.Ptr(11)));
	add_msgtolist(strMsg);

  strMsg.Format("POS Z:%f", ConvertLeonFloat(ackCmd.Ptr(15)));
	add_msgtolist(strMsg);

  strMsg.Format("NEG X:%f", ConvertLeonFloat(ackCmd.Ptr(19)));
	add_msgtolist(strMsg);

  strMsg.Format("NEG Y:%f", ConvertLeonFloat(ackCmd.Ptr(23)));
	add_msgtolist(strMsg);

  strMsg.Format("NEG Z:%f", ConvertLeonFloat(ackCmd.Ptr(27)));
	add_msgtolist(strMsg);

	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::InsdrGyroscopeSelfTest(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[InsdrGyroscopeSelfTestCmd].cmdSize);
	cmd.SetU08(1, cmdTable[InsdrGyroscopeSelfTestCmd].cmdId);
	cmd.SetU08(2, cmdTable[InsdrGyroscopeSelfTestCmd].cmdSubId);

	BinaryData ackCmd;
	CmdErrorCode ack = ExcuteBinaryCommand(InsdrGyroscopeSelfTestCmd, &cmd, &ackCmd);
  if(Ack != ack)
  {
	  return Timeout;
  }

	if(nMode == Return)
	{
		*((U08*)outputData) = ackCmd[6];
		return ack;
	}

	CString strMsg = "INS DR gyroscope self-test successfully";
	add_msgtolist(strMsg);

  strMsg.Format("Test Status:%s",(ackCmd[6]) ? "PASS" : "FAILED");
	add_msgtolist(strMsg);

	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::InsdrAccumulateAngleStart(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[InsdrAccumulateAngleStartCmd].cmdSize);
	cmd.SetU08(1, cmdTable[InsdrAccumulateAngleStartCmd].cmdId);
	cmd.SetU08(2, cmdTable[InsdrAccumulateAngleStartCmd].cmdSubId);

	BinaryData ackCmd;
	CmdErrorCode ack = ExcuteBinaryCommand(InsdrAccumulateAngleStartCmd, &cmd, &ackCmd);
  if(Ack != ack)
  {
	  return Timeout;
  }

	if(nMode == Return)
	{
		*((U08*)outputData) = ackCmd[6];
		return ack;
	}

	CString strMsg = "INS DR accumulate angle start successfully";
	add_msgtolist(strMsg);

  strMsg.Format("Status:%s",(ackCmd[6]) ? "OK" : "FAILED");
	add_msgtolist(strMsg);

	return Timeout;
}

CGPSDlg::CmdErrorCode CGPSDlg::InsdrAccumulateAngleStop(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[InsdrAccumulateAngleStopCmd].cmdSize);
	cmd.SetU08(1, cmdTable[InsdrAccumulateAngleStopCmd].cmdId);
	cmd.SetU08(2, cmdTable[InsdrAccumulateAngleStopCmd].cmdSubId);

	BinaryData ackCmd;
	CmdErrorCode ack = ExcuteBinaryCommand(InsdrAccumulateAngleStopCmd, &cmd, &ackCmd);
  if(Ack != ack)
  {
	  return Timeout;
  }

	if(nMode == Return)
	{
		*((U08*)outputData) = ackCmd[6];
		return ack;
	}

	CString strMsg = "INS DR accumulate angle stop successfully";
	add_msgtolist(strMsg);

  strMsg.Format("Status:%s",(ackCmd[6]) ? "OK" : "FAILED");
	add_msgtolist(strMsg);

  strMsg.Format("ANGLE X:%f", ConvertLeonFloat(ackCmd.Ptr(7)));
	add_msgtolist(strMsg);

  strMsg.Format("ANGLE Y:%f", ConvertLeonFloat(ackCmd.Ptr(11)));
	add_msgtolist(strMsg);

  strMsg.Format("ANGLE Z:%f", ConvertLeonFloat(ackCmd.Ptr(15)));
	add_msgtolist(strMsg);

	return Timeout;
}

#if defined(XN120_TESTER)
void CGPSDlg::DoXn120Tester()
{
  if(m_nXn120TestSatus != 0)
  {
    ::AfxMessageBox("The test is still in progress!");
		return;
  }

	if(!CheckConnect())
	{
		return;
	}

  m_strXn120Version = "";
  m_nXn120TestSatus = 1;

  //QuerySoftwareVersionSystemCode(Return, &m_strXn120Version);
  BinaryData ackCmd;
  QuerySoftwareVersionSystemCode(Return, &ackCmd);
  CString strMsg;
  m_strXn120Version = "";

  strMsg.Format("%s%d.%d.%d\r\n", "Kernel Version ", ackCmd[7], ackCmd[8], ackCmd[9]);
  m_strXn120Version = strMsg;
  strMsg.Format("%s%d.%d.%d\r\n", "Software Version ", ackCmd[11], ackCmd[12], ackCmd[13]);
  m_strXn120Version += strMsg;
  strMsg.Format("%s%d.%d.%d\r\n", "Revision ", ackCmd[15] + 2000, ackCmd[16], ackCmd[17]);
  m_strXn120Version += strMsg;

  //::AfxMessageBox(strVersion);

	SetInputMode(NoOutputMode);
	m_restartMode = 3;	
	AfxBeginThread(RestartThread, 0);
}
#endif

CGPSDlg::CmdErrorCode CGPSDlg::EnterRtkDebugMode(CmdExeMode nMode, void* outputData)
{	    
	BinaryCommand cmd(cmdTable[EnterRtkDebugModeCmd].cmdSize);
	cmd.SetU08(1, cmdTable[EnterRtkDebugModeCmd].cmdId);
	cmd.SetU08(2, cmdTable[EnterRtkDebugModeCmd].cmdSubId);
	cmd.SetU08(3, 0x01);
	cmd.SetU08(4, 0x01);
	cmd.SetU08(5, 0x00);

	//BinaryData ackCmd;
 	CGPSDlg::gpsDlg->ExecuteConfigureCommand(cmd.GetBuffer(), cmd.Size(), "Enter RTK debug mode successfully", false);
	return Ack;
}

CGPSDlg::CmdErrorCode CGPSDlg::BackRtkDebugMode(CmdExeMode nMode, void* outputData)
{	    
	BinaryCommand cmd(cmdTable[BackRtkDebugModeCmd].cmdSize);
	cmd.SetU08(1, cmdTable[BackRtkDebugModeCmd].cmdId);
	cmd.SetU08(2, cmdTable[BackRtkDebugModeCmd].cmdSubId);
	cmd.SetU08(3, 0x01);
	cmd.SetU08(4, 0x00);
	cmd.SetU08(5, 0x00);

	//BinaryData ackCmd;
 	CGPSDlg::gpsDlg->ExecuteConfigureCommand(cmd.GetBuffer(), cmd.Size(), "Back from RTK debug mode successfully", false);
	return Ack;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryDrRate(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryDrRateCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryDrRateCmd].cmdId);
  cmd.SetU08(2, cmdTable[QueryDrRateCmd].cmdSubId);

	BinaryData ackCmd;
	CmdErrorCode err = ExcuteBinaryCommand(QueryDrRateCmd, &cmd, &ackCmd, (nMode == Display) ? 3000 : 1000);
	if(err != Ack)
	{
    return err;
  }

	if(Return == nMode)
	{
    *((BinaryData*)outputData) = ackCmd;
		return err;
	}

	add_msgtolist("Query DR predict update rate successfully");
  CString strMsg;
  strMsg.Format("DR predict update rate:%d", ackCmd[6]);
  add_msgtolist(strMsg);

	return err;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryDrRawRate(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryDrRawRateCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryDrRawRateCmd].cmdId);
  cmd.SetU08(2, cmdTable[QueryDrRawRateCmd].cmdSubId);

	BinaryData ackCmd;
	CmdErrorCode err = ExcuteBinaryCommand(QueryDrRawRateCmd, &cmd, &ackCmd, (nMode == Display) ? 3000 : 1000);
	if(err != Ack)
	{
    return err;
  }

	if(Return == nMode)
	{
    *((BinaryData*)outputData) = ackCmd;
		return err;
	}

	add_msgtolist("Query DR RAW sensor data output rate successfully");
  CString strMsg;
  strMsg.Format("RAW sensor data output rate:%d", ackCmd[6]);
  add_msgtolist(strMsg);

	return err;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryDrMemsNoiseLevel(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryDrMemsNoiseLevelCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryDrMemsNoiseLevelCmd].cmdId);
  cmd.SetU08(2, cmdTable[QueryDrMemsNoiseLevelCmd].cmdSubId);

	BinaryData ackCmd;
	CmdErrorCode err = ExcuteBinaryCommand(QueryDrMemsNoiseLevelCmd, &cmd, &ackCmd, (nMode == Display) ? 3000 : 1000);
	if(err != Ack)
	{
    return err;
  }

	if(Return == nMode)
	{
    *((BinaryData*)outputData) = ackCmd;
		return err;
	}

	add_msgtolist("Query DR MEMS noise level successfully");
  double d = ConvertLeonDouble(ackCmd.Ptr(6));
  
  CString strMsg;
  strMsg.Format("Accelemeter noise:%E", d);
  add_msgtolist(strMsg);

  d = ConvertLeonDouble(ackCmd.Ptr(14));
  strMsg.Format("Gyro noise:%E", d);
  add_msgtolist(strMsg);

	return err;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryAdrOdometerScalingFactor(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[QueryAdrOdometerScalingFactorCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryAdrOdometerScalingFactorCmd].cmdId);
  cmd.SetU08(2, cmdTable[QueryAdrOdometerScalingFactorCmd].cmdSubId);

	BinaryData ackCmd;
	CmdErrorCode err = ExcuteBinaryCommand(QueryAdrOdometerScalingFactorCmd, &cmd, &ackCmd, (nMode == Display) ? 3000 : 1000);
	if(err != Ack)
	{
    return err;
  }

	if(Return == nMode)
	{
    *((BinaryData*)outputData) = ackCmd;
		return err;
	}

	add_msgtolist("Query ADR ODO scaling factor successfully");
  F32 f = ConvertLeonFloat(ackCmd.Ptr(6));
  
  CString strMsg;
  strMsg.Format("ODO scaling factor:%f", f);
  add_msgtolist(strMsg);

  return err;
}

CGPSDlg::CmdErrorCode CGPSDlg::SendComCmdWithAck(HANDLE com, U08* cmd, int size, DWORD timeout)
{
	ScopeTimer t;
	DWORD error = 0;
	COMSTAT comStatus = {0};
	ClearCommError(com, &error, &comStatus);

  PurgeComm(com, PURGE_RXCLEAR);
	FlushFileBuffers(com);
  DWORD dwBytesRead = 0;
  ::WriteFile(com, cmd, size, &dwBytesRead, 0);  

  U08 cmdAck[128];
	while(NULL != com)
	{
		if(CGPSDlg::gpsDlg->CheckTimeOut(t.GetDuration(), timeout, true))
		{	//Time Out
			return Timeout;
		}

    DWORD len = CSerial::GetComBinaryAck(com, cmdAck, 128, timeout - t.GetDuration());
		if(!ReadOK(len))
		{	
			continue;
		}

		int cmdSize = MAKEWORD(cmdAck[3], cmdAck[2]);
		if(cmdSize != len - 7)
		{	//Packet Size Error
			continue;
		}
		if(cmdAck[0] != 0xA0 || cmdAck[1] != 0xA1 ||
			 cmdAck[len - 2] != 0x0D || cmdAck[len - 1] != 0x0A)
		{	//Format Error
			continue;
		}
		if(cmdAck[4] == 0x83 && cmdAck[5] == 0x0)
		{	//ACK0
			continue;
		}
		if(cmdAck[4] == 0x84)
		{	//NACK
			return NACK;
		}
		if(cmdAck[4] == 0x85)
		{	//FormatError
			return FormatError;
		}
		if(cmdAck[4] == 0x83)
		{	//Get ACK
			return Ack;
		}
	}
	return Timeout;
}

void CGPSDlg::OnConfigure1PpsOutputMode()
{
	C1ppsOutputModeDlg dlg;
	DoCommonConfig(&dlg);
}

void CGPSDlg::OnBinaryConfigurenmeatalkerid()
{
  ConfigureNmeaTalkerIdDlg dlg;
  DoCommonConfig(&dlg);
  /*
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
  */
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryPpsOutputMode(CmdExeMode nMode, void* outputData)
{
	BinaryCommand cmd(cmdTable[GetPpsOutputModeCmd].cmdSize);
	cmd.SetU08(1, cmdTable[GetPpsOutputModeCmd].cmdId);

	BinaryData ackCmd;
  CmdErrorCode err = ExcuteBinaryCommand(GetPpsOutputModeCmd, &cmd, &ackCmd, (nMode == Display) ? 3000 : 1000);
	if(err != Ack)
	{
    return err;
  }

  if(Return == nMode)
	{	
    *((BinaryData*)outputData) = ackCmd;
		return err;
  }

	CString strMsg = "Query 1PPS output mode successfully";
	add_msgtolist(strMsg);


	if(ackCmd[5] == 0)
  {
		CGPSDlg::gpsDlg->add_msgtolist("PPS No Output");
  }
	else if(ackCmd[5] == 1)
  {
		CGPSDlg::gpsDlg->add_msgtolist("PPS Output if GNSS time is available");
  }
	else if(ackCmd[5] == 2)
	{
		CGPSDlg::gpsDlg->add_msgtolist("PPS Output always and align to GNSS time");
	}

  if(ackCmd[6] == 0)
  {
		CGPSDlg::gpsDlg->add_msgtolist("Align to GNSS time");
  }
	else if(ackCmd[6] == 1)
  {
		CGPSDlg::gpsDlg->add_msgtolist("Align to UTC time");
  }
	else if(ackCmd[6] == 2)
  {
		CGPSDlg::gpsDlg->add_msgtolist("Align to NavIC");
  }
  return err;
}

CGPSDlg::CmdErrorCode CGPSDlg::HostDataDumpOn(CmdExeMode nMode, void* outputData)
{	    
	BinaryCommand cmd(cmdTable[HostDataDumpOnCmd].cmdSize);
	cmd.SetU08(1, cmdTable[HostDataDumpOnCmd].cmdId);
	cmd.SetU08(2, cmdTable[HostDataDumpOnCmd].cmdSubId);
	cmd.SetU08(3, 0x01);
	cmd.SetU08(4, 0x01);

	//BinaryData ackCmd;
 	CGPSDlg::gpsDlg->ExecuteConfigureCommand(cmd.GetBuffer(), cmd.Size(), "Host Data Dump On successfully", false);
	return Ack;
}

CGPSDlg::CmdErrorCode CGPSDlg::HostDataDumpOff(CmdExeMode nMode, void* outputData)
{	    
	BinaryCommand cmd(cmdTable[HostDataDumpOnCmd].cmdSize);
	cmd.SetU08(1, cmdTable[HostDataDumpOnCmd].cmdId);
	cmd.SetU08(2, cmdTable[HostDataDumpOnCmd].cmdSubId);
	cmd.SetU08(3, 0x00);
	cmd.SetU08(4, 0x01);

	//BinaryData ackCmd;
 	CGPSDlg::gpsDlg->ExecuteConfigureCommand(cmd.GetBuffer(), cmd.Size(), "Host Data Dump Off successfully", false);
	return Ack;
}

CGPSDlg::CmdErrorCode CGPSDlg::QueryIfreeMode(CmdExeMode nMode, void* outputData)
{	    
	BinaryCommand cmd(cmdTable[QueryIfreeModeCmd].cmdSize);
	cmd.SetU08(1, cmdTable[QueryIfreeModeCmd].cmdId);
	cmd.SetU08(2, cmdTable[QueryIfreeModeCmd].cmdSubId);

	BinaryData ackCmd;
  CmdErrorCode err = ExcuteBinaryCommand(QueryIfreeModeCmd, &cmd, &ackCmd, (nMode == Display) ? 3000 : 1000);
	if(err != Ack)
	{
    return err;
  }

  if(Return == nMode)
	{	
    *((BinaryData*)outputData) = ackCmd;
		return err;
  }

  CString txt;
  CString strMsg = "Query Ifree mode successfully";
	add_msgtolist(strMsg);
  if(ackCmd[6] == 0)
  {
    txt = "None";
  }
  else if(ackCmd[6] == 1)
  {
    txt = "Strict";
  }
  else if(ackCmd[6] == 2)
  {
    txt = "Broad";
  }
  strMsg.Format("Ifree Mode: %s", txt);
  add_msgtolist(strMsg);

  strMsg.Format("Minimum SV: %d", ackCmd[7]);
  add_msgtolist(strMsg);

  strMsg.Format("Weighting Div: %d", ackCmd[8]);
  add_msgtolist(strMsg);

	return Ack;
}

void CGPSDlg::OnQueryRtc()
{
	if(!CheckConnect())
	{
		return;
	}

  const U32 rtcRegAddr = 0x20014C34;
  U32 regData = 0;
  CmdErrorCode ack = CGPSDlg::gpsDlg->QueryRegisterx(CGPSDlg::Return, rtcRegAddr, &regData);
  if(ack != Ack) 
  {
		SetMode();
		CreateGPSThread();
    return;
  }

	CString strMsg;
  strMsg.Format("RTC:0x%08X (%d)", regData, regData);
	add_msgtolist(strMsg);

	SetMode();
	CreateGPSThread();
}