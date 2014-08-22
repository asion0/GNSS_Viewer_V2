// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently,
// but are changed infrequently

#pragma once

#ifndef _SECURE_ATL
#define _SECURE_ATL 1
#endif

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#endif

#ifndef WINVER                          // 指定最低平台需求為 Windows Vista。
#define WINVER 0x0600           // 將它變更為針對 Windows 其他版本的適當值。
#endif

#ifndef _WIN32_WINNT            // 指定最低平台需求為 Windows Vista。
#define _WIN32_WINNT 0x0600     // 將它變更為針對 Windows 其他版本的適當值。
#endif						

#ifndef _WIN32_WINDOWS          // 指定最低平台需求為 Windows 98。
#define _WIN32_WINDOWS 0x0410 // 將它變更為針對 Windows Me (含) 以後版本的適當值。
#endif

#ifndef _WIN32_IE                       // 指定最低平台需求為 Internet Explorer 7.0。
#define _WIN32_IE 0x0700        // 將它變更為針對 IE 其他版本的適當值。
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions





#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 支援的 Internet Explorer 4 通用控制項
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC 支援的 Windows 通用控制項
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // 功能區和控制列的 MFC 支援

#include <conio.h>
#include <afxinet.h>

#include "StBaseType.h"

#define MAX_WAIT_TIME		      INFINITE
#define TIME_OUT_MS               10000
#define TIME_OUT_QUICK_MS         1000
#define SCAN_TIME_OUT_MS          2000
#define DOWNLOAD_TIME_OUT_LOOP    10
#define BUF_SIZE 4096

#define PI          3.141592653589793
#define WGS84a      6378137.0
#define WGS84b      6356752.314
#define e           0.0818191913

#define BOOTLOADER_SIZE		0x10000

#include "Vender_newGNSS.h"
#include "Global.h"


#if WITH_CONFIG_USB_BAUDRATE
#pragma comment(lib, "CP210xManufacturing.lib")
#include "CP210xManufacturingDLL.h"
#endif

