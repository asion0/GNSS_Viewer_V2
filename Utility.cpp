#include "stdafx.h"
#include "Utility.h"


void Utility::Log(const char* str1, const char* str2, int n)
{
	CString strLog; 
	strLog.Format("%s, %s(%d)", str1, str2, n);
	::OutputDebugString(strLog);
}

void Utility::LogFatal(const char* str1, const char* str2, int n)
{
	CString strLog; 
	strLog.Format("%s, %s(%d)!!!", str1, str2, n);
	::OutputDebugString(strLog);
}

DWORD Utility::GetRegValue(HKEY hKeyHandle, LPCTSTR lpSubkey, LPCTSTR lpRegName, CString &csRegvalue)
{
	const int nBufferSize = 256;
	HKEY hKey;
	LONG lRet;
	DWORD dwDataSize;
	DWORD dwType;
	TCHAR pchBuf[nBufferSize];
	DWORD uRetCode = ERROR_SUCCESS;

	lRet = RegOpenKeyEx(hKeyHandle, 
						lpSubkey,
						0, 
						KEY_QUERY_VALUE, 
						&hKey);

	if( lRet != ERROR_SUCCESS )
	{
		return lRet;			
	}

	dwDataSize = nBufferSize;
	dwType     = REG_SZ;
	lRet = RegQueryValueEx(hKey, lpRegName, NULL, &dwType, (LPBYTE)pchBuf, &dwDataSize);

	if( lRet != ERROR_SUCCESS )
	{
		RegCloseKey(hKey);
		return lRet;
	}
	lRet = RegCloseKey(hKey);
	if( lRet != ERROR_SUCCESS )
	{
		return lRet;
	}

	if(dwDataSize > 0)
	{
		csRegvalue = pchBuf;
	}
	else
	{
		csRegvalue.Empty();
	}
	return ERROR_SUCCESS;
}

DWORD Utility::GetRegValue(HKEY hKeyHandle, LPCTSTR lpSubkey, LPCTSTR lpRegName, int &nRegvalue)
{
	CString strRegValue;
	DWORD dwRet;
	dwRet = GetRegValue(hKeyHandle, lpSubkey, lpRegName, strRegValue);
	if( dwRet != ERROR_SUCCESS )
	{
		return dwRet;
	}
	nRegvalue = _ttoi(strRegValue);
	return dwRet;
}
DWORD Utility::SetRegValue(HKEY hKeyHandle, LPCTSTR lpSubkey, LPCTSTR lpRegName, LPCTSTR szRegvalue)
{
	HKEY    hKey;
	LONG	lRet;
	DWORD	dwReturn;
	DWORD	dwKeySize;
	DWORD	uRetCode = ERROR_SUCCESS;
	CString csRegvalue(szRegvalue);

	lRet = RegCreateKeyEx(hKeyHandle,
			              lpSubkey,
						  0,
						  NULL,
						  REG_OPTION_NON_VOLATILE,
						  KEY_ALL_ACCESS,
						  NULL,
						  &hKey,
						  &dwReturn);

	if( lRet != ERROR_SUCCESS)
	{
		return lRet;
	}

	dwKeySize = csRegvalue.GetLength() * sizeof(TCHAR);

	lRet = RegSetValueEx(hKey, lpRegName, 0, REG_SZ, (LPBYTE)(LPCTSTR)csRegvalue, dwKeySize);
	if( lRet != ERROR_SUCCESS)
	{
		return lRet;
	}

	lRet = RegCloseKey(hKey);
	if( lRet != ERROR_SUCCESS)
	{
		return lRet;
	}
	return ERROR_SUCCESS;
}
DWORD Utility::SetRegValue(HKEY hKeyHandle, LPCTSTR lpSubkey, LPCTSTR lpRegName, int &nRegvalue)
{
	CString strRegValue;
	DWORD dwRet;
	strRegValue.Format(_T("%d"), nRegvalue);
	dwRet = SetRegValue(hKeyHandle, lpSubkey, lpRegName, strRegValue);
	return dwRet;
}

//Read UTF-8 format file to an unicode text buffer.
BOOL Utility::ReadU8FileToUText(LPCTSTR pszFile, CStringW& strOutText)
{
	CStringA strUTF8;
	CFile fNib;
	//Read an UTF-8 text file to pU8Buffer
	fNib.Open(pszFile, CFile::modeRead);
	int nSize = (int)fNib.GetLength();
	char *pU8Buffer = strUTF8.GetBuffer(nSize + 1);	//UTF-8 buffer
	fNib.Read(pU8Buffer, nSize);
	strUTF8.ReleaseBuffer();
	fNib.Close();

	//Convert pU8Buffer from UTF-8 to Unicode
	int count = MultiByteToWideChar(CP_UTF8, 0, strUTF8, -1 ,NULL,0);
	LPWSTR pszUnicode = strOutText.GetBuffer(count);
	MultiByteToWideChar(CP_UTF8, 0, pU8Buffer, -1, pszUnicode, count);
	strOutText.ReleaseBuffer();
	return TRUE;
}

//Write Unicode format text to an utf-8 file.
BOOL Utility::WriteUTextToU8File(LPCTSTR pszFile, LPCWSTR pszInputText)
{
	int count = WideCharToMultiByte(CP_UTF8, 0, pszInputText, -1 ,NULL, 0, NULL, NULL);
	CStringA strUTF8;
	char *pU8Buffer = strUTF8.GetBuffer(count);	//UTF-8 buffer
	WideCharToMultiByte(CP_UTF8, 0, pszInputText, -1 ,pU8Buffer, count, NULL, NULL);
	strUTF8.ReleaseBuffer();

	CFile fNibNew;
	//Write an UTF-8 text file from pU8Buffer
	fNibNew.Open(pszFile, CFile::modeWrite | CFile::modeCreate);
	fNibNew.Write(strUTF8, count - 1);	//Text file doesn't need null ending.
	fNibNew.Close();
	return TRUE;
}

const int unicodeMarkLen = 2;
union UnicodeMark
{
	BYTE mark_b[unicodeMarkLen];
	WORD mark_w;
};

//Read unicode big endian format file to an unicode text buffer.
void Utility::ConvertBE2LE(void* pBuffer, int nSize)
{
	int nSizeInWord = nSize / sizeof(WORD);
	WORD* pWordBuf = (WORD*)pBuffer;
	WORD tmp;
	while(nSizeInWord--)
	{
		tmp = (*pWordBuf)>>8;
		tmp |= (*pWordBuf)<<8;
		*pWordBuf = tmp;
		++pWordBuf;
	}
}

BOOL Utility::ReadUFileToUText(LPCTSTR pszFile, CStringW& strOutText, BOOL& bBigEndian)
{
	UnicodeMark umark;
	CFile fNib;
	//Read an UTF-8 text file to pU8Buffer
	BOOL bRet = fNib.Open(pszFile, CFile::modeRead);
	int nSize = (int)fNib.GetLength();
	fNib.Read(umark.mark_b, unicodeMarkLen);
	if(0xfffe == umark.mark_w)
	{
		bBigEndian = TRUE;
	}
	else if(0xfeff == umark.mark_w)
	{
		bBigEndian = FALSE;
	}
	else
	{
		return FALSE;
	}

	wchar_t *pUBuffer = strOutText.GetBuffer(nSize - unicodeMarkLen + 2);	//Unicode buffer
	ZeroMemory(pUBuffer, nSize - unicodeMarkLen + 2);
	fNib.Read(pUBuffer, nSize - unicodeMarkLen);
	if(bBigEndian)
	{
		ConvertBE2LE(pUBuffer, nSize - unicodeMarkLen);
	}
	fNib.Close();
	strOutText.ReleaseBuffer();

	return TRUE;
}

//Write Unicode format text to an unicode big endian file.
BOOL Utility::WriteUTextToUBFile(LPCTSTR pszFile, LPCWSTR pszInputText, BOOL bBigEndian)
{
	UnicodeMark umark;
	if(bBigEndian)
	{
		umark.mark_w = 0xfffe;
	}
	else
	{
		umark.mark_w = 0xfeff;
	}

	CFile fNibNew;
	fNibNew.Open(pszFile, CFile::modeWrite | CFile::modeCreate);
	fNibNew.Write(umark.mark_b, unicodeMarkLen);

	int nSize = (int)wcslen(pszInputText) * sizeof(wchar_t);
	if(bBigEndian)
	{
		int nSizeInWord = nSize / sizeof(WORD);
		const WORD* pWordBuf = (const WORD*)pszInputText;
		WORD tmp;
		while(nSizeInWord--)
		{
			tmp = (*pWordBuf)>>8;
			tmp |= (*pWordBuf)<<8;
			fNibNew.Write(&tmp, sizeof(WORD));	//Text file doesn't need null ending.
			++pWordBuf;
		}
	}
	else
	{
		fNibNew.Write(pszInputText, nSize);	//Text file doesn't need null ending.
	}

	fNibNew.Close();
	return TRUE;
}

// strResult - command standard output
// return Exit code of command line 
DWORD Utility::ExecuteExternalFileW(LPCWSTR csCmdLine, CString& strResult)
{
	CStringW strCmdLine(csCmdLine);
	strResult.Empty();

	SECURITY_ATTRIBUTES secattr; 
	ZeroMemory(&secattr,sizeof(secattr));
	secattr.nLength = sizeof(secattr);
	secattr.bInheritHandle = TRUE;

	//Create pipes to write and read data
	HANDLE rPipe, wPipe;
	CreatePipe(&rPipe, &wPipe, &secattr, 0);

	STARTUPINFOW sInfo; 
	ZeroMemory(&sInfo, sizeof(sInfo));
	PROCESS_INFORMATION pInfo; 
	ZeroMemory(&pInfo, sizeof(pInfo));

	sInfo.cb = sizeof(sInfo);
	sInfo.dwFlags = STARTF_USESTDHANDLES;
	sInfo.hStdInput = NULL; 
	sInfo.hStdOutput = wPipe; 
	sInfo.hStdError = wPipe;

	//Create the process here.
	CreateProcessW(0, (LPWSTR)(LPCWSTR)strCmdLine, 
		0, 0, TRUE,
		NORMAL_PRIORITY_CLASS|CREATE_NO_WINDOW, 0, 0,
		&sInfo, &pInfo);
	CloseHandle(wPipe);

	//now read the output pipe here.
	const int nReadBlockSize = 1024;
	char buf[nReadBlockSize + 1];
	DWORD reDword; 
	CString csTemp;
	BOOL res;
	do
	{
		ZeroMemory(buf, nReadBlockSize + 1);
		res = ::ReadFile(rPipe, buf, nReadBlockSize, &reDword, 0);
		csTemp = buf;
		strResult += csTemp.Left(reDword);
	}while(res);

	//Get Exit Code
	DWORD dwResult;
	do
	{
		::GetExitCodeProcess(pInfo.hProcess, &dwResult);
		Sleep(100);
	}
	while(STILL_ACTIVE==dwResult);

	return dwResult;
}

// delete directory
BOOL Utility::DeleteDirectory(LPCTSTR pszSrcDir, BOOL bSilent, BOOL bConfirm)
{
	ASSERT(pszSrcDir);
	CWaitCursor WaitCursor;

	SHFILEOPSTRUCT fos;
	FILEOP_FLAGS flags = 0;
	if(bSilent)
	{
		flags |= FOF_SILENT;
	}

	if(!bConfirm)
	{
		flags |= FOF_NOCONFIRMATION;
	}
 
	memset(&fos, 0, sizeof(SHFILEOPSTRUCT));
	CWnd *pWnd = AfxGetMainWnd();
	fos.hwnd = pWnd->GetSafeHwnd();
	fos.wFunc = FO_DELETE;
	fos.fFlags = flags;

	CString strSrc(pszSrcDir);
	int nSrcLen = strSrc.GetLength();
	LPTSTR pszFileList = strSrc.GetBuffer(nSrcLen + 2);
	for(int i=0; i<nSrcLen; ++i)
	{
		if(pszFileList[i]==_T('|'))
		{
			pszFileList[i] = 0;
		}
	}
	pszFileList[nSrcLen + 1] = 0;

	fos.pFrom = pszFileList;
	int nRet = SHFileOperation(&fos);
	strSrc.ReleaseBuffer();
	return nRet==0;
}

// Copy files
// pszSrcFiles - file list, sparate by '|' ex: "file1.ext|file2.ext|file3.exe"
// if more than one file, pszNewName will be ignored.
BOOL Utility::CopyFiles(LPCTSTR pszSrcFiles, LPCTSTR pszDestFolder, LPCTSTR pszNewName, BOOL bSilent, BOOL bConfirm)
{
	ASSERT(pszSrcFiles);
	ASSERT(pszDestFolder);
	CWaitCursor WaitCursor;

	SHFILEOPSTRUCT fos;
	FILEOP_FLAGS flags = 0;
	if(bSilent)
	{
		flags |= FOF_SILENT;
	}

	if(!bConfirm)
	{
		flags |= FOF_NOCONFIRMATION;
	}
 
	memset(&fos, 0, sizeof(SHFILEOPSTRUCT));
	CWnd *pWnd = AfxGetMainWnd();
	fos.hwnd = pWnd->GetSafeHwnd();
	fos.wFunc = FO_COPY;
	fos.fFlags = flags;

	CString strSrc(pszSrcFiles);
	int nSrcLen = strSrc.GetLength();
	LPTSTR pszFileList = strSrc.GetBuffer(nSrcLen + 2);
	BOOL bMultiSrc = FALSE;
	for(int i=0; i<nSrcLen; ++i)
	{
		if(pszFileList[i]==_T('|'))
		{
			pszFileList[i] = 0;
			bMultiSrc = TRUE;
		}
	}
	pszFileList[nSrcLen + 1] = 0;
	fos.pFrom = pszFileList;

	CString strDst(pszDestFolder);
	if(!bMultiSrc)
	{
		strDst += _T("\\");
		strDst += pszNewName;
	}
	int nDstLen = strDst.GetLength();
	LPTSTR pszDst = strDst.GetBuffer(nDstLen + 2);	//Double null-terminator
	pszDst[nDstLen + 1] = 0;
	fos.pTo = pszDst;

	int nRet = SHFileOperation(&fos);
	strSrc.ReleaseBuffer();
	strDst.ReleaseBuffer();
	return nRet==0;
}

CString Utility::GetFilePath(LPCTSTR pszPathname)
{
	CString strRet(pszPathname);
	int nSlash1 = strRet.ReverseFind(_T('\\'));
	int nSlash2 = strRet.ReverseFind(_T('/'));
	int nSlash = max(nSlash1, nSlash2);
	BOOL bColon = (strRet.GetLength()>=2)? strRet.GetAt(1)==_T(':') :FALSE;
	if(-1 == nSlash && !bColon)	//No slash and no colon, it's mean no path
	{
		return CString(_T(""));
	}
	else if(nSlash>0)	//Has slash, before slash is the path.
	{
		strRet = strRet.Left(nSlash);
	}
	else	//No slash but has colon, the device letter is the path
	{	
		strRet = strRet.Left(2);
	}
	return strRet;
}

// Copy files
// pszSrcFiles - file pathname. ex: "c:\\file1.ext"
// pszNewName - new file name, does not include path. ex: "filenewname.ext"
BOOL Utility::RenameFile(LPCTSTR pszSrcFile, LPCTSTR pszNewName, BOOL bSilent, BOOL bConfirm)
{
	ASSERT(pszSrcFile);
	ASSERT(pszNewName);
	CWaitCursor WaitCursor;

	SHFILEOPSTRUCT fos;
	FILEOP_FLAGS flags = 0;
	if(bSilent)
	{
		flags |= FOF_SILENT;
	}

	if(!bConfirm)
	{
		flags |= FOF_NOCONFIRMATION;
	}
 
	memset(&fos, 0, sizeof(SHFILEOPSTRUCT));
	CWnd *pWnd = AfxGetMainWnd();
	fos.hwnd = pWnd->GetSafeHwnd();
	fos.wFunc = FO_RENAME;
	fos.fFlags = flags;

	CString strSrc(pszSrcFile);
	int nSrcLen = strSrc.GetLength();
	LPTSTR pszFileList = strSrc.GetBuffer(nSrcLen + 2);
	pszFileList[nSrcLen + 1] = 0;	//Double null-terminator
	fos.pFrom = pszFileList;

	CString strDst = GetFilePath(pszSrcFile);
	strDst += _T("\\");
	strDst += pszNewName;
	int nDstLen = strDst.GetLength();
	LPTSTR pszDst = strDst.GetBuffer(nDstLen + 2);	//Double null-terminator
	pszDst[nDstLen + 1] = 0;
	fos.pTo = pszDst;

	int nRet = SHFileOperation(&fos);
	strSrc.ReleaseBuffer();
	strDst.ReleaseBuffer();
	return nRet==0;
}
CString Utility::GetFileExt(LPCTSTR pszPathname)
{
	CString strRet(pszPathname);
	int nDot = strRet.ReverseFind(_T('.'));
	if(-1 == nDot)
	{
		return _T("");
	}
	int nSlash1 = strRet.ReverseFind(_T('\\'));
	int nSlash2 = strRet.ReverseFind(_T('/'));
	int nSlash = max(nSlash1, nSlash2);
	if(nSlash > nDot)
	{
		return CString(_T(""));
	}
	else
	{
		strRet = strRet.Right(strRet.GetLength() - nDot - 1);
	}
	return strRet;
}

BOOL Utility::CopyResToFile(LPCTSTR szDesFileName, DWORD dRes, LPCTSTR szType)
{
	HANDLE			hFile;	
	HRSRC			hRsrc;	
	DWORD			cntBytes, cbWrite;	
	HGLOBAL			hGlob;	
	LPVOID			lp;	
	CString			csTmp(szDesFileName);	 
	int				i;	
 
	//Copy App to distination directory, and we finish clone....
	HINSTANCE  hResource = AfxGetResourceHandle();

	hRsrc = FindResource(hResource, MAKEINTRESOURCE(dRes), szType);	
	if(hRsrc == NULL)	
		return 0;	

	cntBytes = SizeofResource(AfxGetResourceHandle(), hRsrc);	
	if(cntBytes <= 0)		
		return 0;	
	 
	hGlob = LoadResource(AfxGetResourceHandle(), hRsrc);	
	if(hGlob == NULL)		
		return 0;	
	lp = LockResource(hGlob);	
	if(lp == NULL)	
	{	
		FreeResource(hGlob);
		return 0;
	}	
	//Create folder
	if ((i = csTmp.ReverseFind('\\')) > 2)
	{
		csTmp.SetAt(i+1, '\0');
		LPVOID lpMsgBuf;


		if (!CreateDirectory(csTmp, NULL))
		{
			
			int nResult = GetLastError();
			FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM
						| FORMAT_MESSAGE_IGNORE_INSERTS, NULL, nResult, 
						MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), 
						(LPTSTR) &lpMsgBuf, 0, NULL);
			
			if (nResult != ERROR_ALREADY_EXISTS)
			{
			//	MessageBox(NULL, (LPTSTR)lpMsgBuf, "Install driver", MB_OK);
				LocalFree(lpMsgBuf);
				return 0;
			}
			LocalFree(lpMsgBuf);
		}
	}

	if((hFile = CreateFile(szDesFileName, GENERIC_READ|GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, 
							FILE_ATTRIBUTE_NORMAL, NULL))==INVALID_HANDLE_VALUE)
	{		
		DWORD derr = GetLastError();
		UnlockResource(hGlob);		
		FreeResource(hGlob);		
		//MessageBox(NULL, "Can't create file! Install fail!", "Install driver", MB_OK);
		return 0;	
	}	
	if(!WriteFile(hFile, lp, cntBytes, &cbWrite, NULL))	
	{		
		CloseHandle(hFile);	
		DeleteFile(szDesFileName);
		UnlockResource(hGlob);
		FreeResource(hRsrc);	
		//MessageBox(NULL, "Can't write directory! Install fail!", "Install driver", MB_OK);
		return 0;	
	}
	CloseHandle(hFile);
	UnlockResource(hGlob);
	FreeResource(hRsrc);
	return TRUE;
}

double Utility::GetPrivateProfileDouble(LPCSTR appName, LPCSTR keyName, LPCSTR defaultString, LPCSTR fileName)
{
	CString strBuffer;
	const int bufferSize = 1024;
	GetPrivateProfileString(appName, keyName, defaultString, strBuffer.GetBuffer(bufferSize), bufferSize, fileName);
	strBuffer.ReleaseBuffer();
	return atof(strBuffer);
}

bool Utility::IsFileExist(const char* filePath)
{
	return (INVALID_FILE_ATTRIBUTES != ::GetFileAttributes(filePath));
}

int Utility::LSB(char lsb)
{
	if(lsb>='0' && lsb<='9')
	{
		return (lsb - '0');
	}
	else if(lsb>='A' && lsb<='F')
	{
		return (lsb - 'A' + 0xA);
	}
	else
	{
//		ASSERT(FALSE);
	}
	return 0;
}

int Utility::MSB(char msb)
{
	return LSB(msb) * 0x10;
}	

int Utility::GetHexValue(char highByte, char lowByte)
{
	return LSB(highByte) * 16 + LSB(lowByte);
}

int Utility::GetOctValue(char highByte, char lowByte)
{
	return LSB(highByte) * 10 + LSB(lowByte);
}

CString Utility::ErrorString(DWORD err)
{
	CString strError;
	LPTSTR s;
	if(::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
		FORMAT_MESSAGE_FROM_SYSTEM,
		NULL,
		err,
		0,
		(LPTSTR)&s,
		0,
		NULL) == 0)
	{ /* failed */
		// Unknown error code %08x (%d)
	//	CString fmt;
	//	CString t;
	//	fmt.LoadString(IDS_UNKNOWN_ERROR);
	//	t.Format(fmt, err, LOWORD(err));
	//	Error = t;
	} /* failed */
	else
	{ /* success */
		LPTSTR p = _tcschr(s, _T('\r'));
		if(p != NULL)
		{ /* lose CRLF */
			*p = _T('\0');
		} /* lose CRLF */
		strError = s;
		::LocalFree(s);
	} /* success */
	return strError;
} // ErrorString

