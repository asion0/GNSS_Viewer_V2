#pragma once
#include "stdafx.h"

#define BINMSG_ECEF_USER_PVT               0xA8
#define BINMSG_GEODETIC_USER_PVT           0xA9
#define BINMSG_USER_SATELLITE_INFOMATION   0xAA
#define BINMSG_SATELLITE_MEASUREMENT_DATA  0xAC
#define BINMSG_LOG_READ_DATA               0x95
#define BINMSG_REPLY_LOG_STATUS            0x94
#define BINMSG_GET_EPHEMERIS               0xB1

void ShowMeasurementChannel(U08* src, bool convertOnly = false, CString* pStr = NULL);
void ShowSubframe(U08*src, bool convertOnly = false, CString* pStr = NULL);
void ShowMeasurementTime(U08* src, bool convertOnly = false, CString* pStr = NULL);
void ShowMeasurementSv(U08* src, bool convertOnly = false, CString* pStr = NULL);
void ShowReceiverNav(U08* src, bool convertOnly = false, CString* pStr = NULL);
void ShowBinaryOutput(U08* src, bool convertOnly = false, CString* pStr = NULL);
void ShowDjiBinaryOutput(U08* src, bool convertOnly = false, CString* pStr = NULL);
void ExtRawMeas(U08* src, bool convertOnly = false, CString* pStr = NULL);
void ExtRawMeasEf(U08* src, bool convertOnly = false, CString* pStr = NULL);
void ShowRtcm1005(U08* src, bool convertOnly = false, CString* pStr = NULL);
void ShowRtcm1077(U08* src, int len, bool convertOnly = false, CString* pStr = NULL);
void ShowRtcm1107(U08* src, int len, bool convertOnly = false, CString* pStr = NULL);
void ShowRtcm1117(U08* src, int len, bool convertOnly = false, CString* pStr = NULL);
void ShowRtcm1127(U08* src, int len, bool convertOnly = false, CString* pStr = NULL);
void ShowRtcm1087(U08* src, int len, bool convertOnly = false, CString* pStr = NULL);
void ShowRtcm1097(U08* src, int len, bool convertOnly = false, CString* pStr = NULL);
void ShowUbxTimTp(U08* src, bool convertOnly = false, CString* pStr = NULL);
void ShowUbxNavSol(U08* src, bool convertOnly = false, CString* pStr = NULL);
void ShowUbxNavPvt(U08* src, bool convertOnly = false, CString* pStr = NULL);
void ShowUbxNavClock(U08* src, bool convertOnly = false, CString* pStr = NULL);
void ShowUbxNavSvInfo(U08* src, bool convertOnly = false, CString* pStr = NULL);
void ShowUbxNavSvStatus(U08* src, bool convertOnly = false, CString* pStr = NULL);
void ShowUbxNavPosllh(U08* src, bool convertOnly = false, CString* pStr = NULL);
void ShowUbxNavDop(U08* src, bool convertOnly = false, CString* pStr = NULL);
void ShowUbxNavVelned(U08* src, bool convertOnly = false, CString* pStr = NULL);
void ShowStqSpecialDrInfo(U08* src, bool convertOnly = false, CString* pStr = NULL);

