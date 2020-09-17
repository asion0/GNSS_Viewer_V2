// GPS.h : main header file for the PROJECT_NAME application
//

#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols


// CGPSApp:
// See GPS.cpp for the implementation of this class
//

class CGPSApp : public CWinApp
{
public:
	CGPSApp();

// Overrides
public:
	virtual BOOL InitInstance();

	bool CheckExternalSrec(CString& externalSrecFile);
	bool CheckExternalRomSrec(CString& externalSrecFile);
	bool CheckExternalPreloader(CString& externalPreloaderFile);
	CString GetTitle();
	CString GetHexString(const U08* buf, int size);
	CString GetCurrrentDir() { return m_currentDir; };
	CString GetPathName() { return m_modulePathName; };
	int GetIntSetting(LPCSTR key, int defaultValue);
//	CString GetStringSetting(LPCSTR key, LPCSTR defaultValue);
	bool SetIntSetting(LPCSTR key, int data);
//	bool SetStringSetting(LPCSTR key, LPCSTR data);
// Implementation

	DECLARE_MESSAGE_MAP()
protected:
	void GetEnvPath();

	CString m_currentDir;
	CString m_modulePathName;
};

extern CGPSApp theApp;