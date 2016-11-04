#pragma once
#include "stdafx.h"

#define BINMSG_ECEF_USER_PVT               0xA8
#define BINMSG_GEODETIC_USER_PVT           0xA9
#define BINMSG_USER_SATELLITE_INFOMATION   0xAA
#define BINMSG_SATELLITE_MEASUREMENT_DATA  0xAC
#define BINMSG_LOG_READ_DATA               0x95
#define BINMSG_REPLY_LOG_STATUS            0x94
#define BINMSG_GET_EPHEMERIS               0xb1

void ShowMeasurementChannel(U08* src, bool convertOnly = false, CString* pStr = NULL);
void ShowSubframe(U08*src, bool convertOnly = false, CString* pStr = NULL);
void ShowMeasurementTime(U08* src, bool convertOnly = false, CString* pStr = NULL);
void ShowMeasurementSv(U08* src, bool convertOnly = false, CString* pStr = NULL);
void ShowReceiverNav(U08* src, bool convertOnly = false, CString* pStr = NULL);
void ShowBinaryOutput(U08* src, bool convertOnly = false, CString* pStr = NULL);
void ShowDjiBinaryOutput(U08* src, bool convertOnly = false, CString* pStr = NULL);
void ExtRawMeas(U08* src, bool convertOnly = false, CString* pStr = NULL);
