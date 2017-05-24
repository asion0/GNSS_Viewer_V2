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

#ifndef WINVER                       
#define WINVER 0x0600           
#endif

#ifndef _WIN32_WINNT           
#define _WIN32_WINNT 0x0600     
#endif						

#ifndef _WIN32_WINDOWS          
#define _WIN32_WINDOWS 0x0410 
#endif

#ifndef _WIN32_IE                      
#define _WIN32_IE 0x0700       
#endif

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS	// some CString constructors will be explicit

// turns off MFC's hiding of some common and often safely ignored warning messages
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions





#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>          
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     

#include <conio.h>
#include <afxinet.h>

#include "StBaseType.h"
#include "Vender_newGNSS.h"
#include "Global.h"

