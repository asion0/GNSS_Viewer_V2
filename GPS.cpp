// GPS.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "GPS.h"
#include "GPSDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CGPSApp
BEGIN_MESSAGE_MAP(CGPSApp, CWinApp)
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()


// CGPSApp construction

CGPSApp::CGPSApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CGPSApp object

CGPSApp theApp;


// CGPSApp initialization

BOOL CGPSApp::InitInstance()
{
	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	CWinApp::InitInstance();

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));
	::OutputDebugString("GNSS Viewer Start...");

	GetEnvPath();

	CGPSDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

void CGPSApp::GetEnvPath()
{
	GetCurrentDirectory(MyMaxPath, m_currentDir.GetBuffer(MyMaxPath));
	m_currentDir.ReleaseBuffer();

	::GetModuleFileName(NULL, m_modulePathName.GetBuffer(MyMaxPath), MyMaxPath);
	m_modulePathName.ReleaseBuffer();

}

bool CGPSApp::CheckExternalSrec(CString& externalSrecFile)
{
	CString filePath = m_modulePathName + "SkytraqTestBin.srec";
	if(INVALID_FILE_ATTRIBUTES != ::GetFileAttributes(filePath))
	{
		externalSrecFile = filePath;
		return true;
	}
	return false;
}

bool CGPSApp::CheckExternalRomSrec(CString& externalSrecFile)
{
	CString filePath = m_modulePathName + "SkytraqTestBinRom.srec";
	if(INVALID_FILE_ATTRIBUTES != ::GetFileAttributes(filePath))
	{
		externalSrecFile = filePath;
		return true;
	}
	return false;
}

bool CGPSApp::CheckExternalPreloader(CString& externalPreloaderFile)
{
	return false;
}

CString CGPSApp::GetTitle()
{
	CString title;
	title.Format("%s %s V%s for %s", APP_CAPTION, APP_TITLE, APP_VERSION, APP_MODULE);
	return title;
}

CString CGPSApp::GetHexString(const U08* buf, int size)
{
	CString strResult;
	const int DisplayLimite = 999;
	if(size>=DisplayLimite)
	{
		size = DisplayLimite;
	}
	for(int i=0; i<size; ++i)
	{
		CString strByte;
		strByte.Format("%02x ", buf[i]);
		strResult += strByte;
	}
	return strResult;
}

int CGPSApp::GetIntSetting(LPCSTR key, int defaultValue)
{
	CRegistry reg;
	reg.SetRootKey(HKEY_CURRENT_USER);
	if(reg.SetKey("Software\\GNSSViewer\\GPS", true))
	{
		return reg.ReadInt(key, defaultValue);
	}
	return defaultValue;
}

CString CGPSApp::GetStringSetting(LPCSTR key, LPCSTR defaultValue)
{
	CRegistry reg;
	reg.SetRootKey(HKEY_CURRENT_USER);
	if(reg.SetKey("Software\\GNSSViewer\\GPS", true))
	{
		return reg.ReadString(key, defaultValue);
	}
	return defaultValue;
}

bool CGPSApp::SetIntSetting(LPCSTR key, int data)
{
	CRegistry reg;
	reg.SetRootKey(HKEY_CURRENT_USER);
	if(reg.SetKey("Software\\GNSSViewer\\GPS", true))
	{
		return reg.WriteInt(key, data)==TRUE;
	}
	return true;
}
//
//bool CGPSApp::SetStringSetting(LPCSTR key, LPCSTR data)
//{
//	CRegistry reg;
//	reg.SetRootKey(HKEY_CURRENT_USER);
//	if(reg.SetKey("Software\\GNSSViewer\\GPS", true))
//	{
//		return reg.WriteString(key, data)==TRUE;
//	}
//	return true;
//}