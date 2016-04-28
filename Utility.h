#pragma once

#include "StBaseType.h"
#define		UTILITY_MAX_PATH		(MAX_PATH * 4)
///////////////// Define Value and Data Struct/////////////
//Delete pointer that creating by operator new 
#define SafelyDelPtr(x)  { delete (x); x = NULL; }

//Delete pointer that creating by operator new []
#define SafelyDelArray(x)  { delete [] (x); x = NULL; }

 //Delete pointer that create by CreateWindow()
#define SafelyDelWnd(x)  if(x) { (x)->DestroyWindow(); delete x; x = NULL; }

//Calculate the number of elements in the array, x must be an array not pointer
#define Dim(x) ( sizeof(x) / sizeof(x[0]) )


class BinaryData;
namespace Utility
{
	void Log(const char* str1, const char* str2, int n);
	void LogFatal(const char* str1, const char* str2, int n);

	DWORD GetRegValue(HKEY hKeyHandle, LPCTSTR lpSubkey, LPCTSTR lpRegName, CString &csRegvalue);
	DWORD GetRegValue(HKEY hKeyHandle, LPCTSTR lpSubkey, LPCTSTR lpRegName, int &nRegvalue);
	DWORD SetRegValue(HKEY hKeyHandle, LPCTSTR lpSubkey, LPCTSTR lpRegName, LPCTSTR szRegvalue);
	DWORD SetRegValue(HKEY hKeyHandle, LPCTSTR lpSubkey, LPCTSTR lpRegName, int &nRegvalue);
	BOOL ReadU8FileToUText(LPCTSTR pszFile, CStringW& strOutText);
	BOOL WriteUTextToU8File(LPCTSTR pszFile, LPCWSTR pszInputText);
	void ConvertBE2LE(void* pBuffer, int nSize);
	BOOL ReadUFileToUText(LPCTSTR pszFile, CStringW& strOutText, BOOL& bBigEndian);
	BOOL WriteUTextToUBFile(LPCTSTR pszFile, LPCWSTR pszInputText, BOOL bBigEndian);
	DWORD ExecuteExternalFileW(LPCWSTR csCmdLine, CString& strResult);
	BOOL ExecuteExeNoWait(LPCSTR csCmdLine);
	bool IsProcessRunning(LPWSTR processName);
	bool IsNamedPipeUsing(LPCTSTR name);
	BOOL DeleteDirectory(LPCTSTR pszSrcDir, BOOL bSilent, BOOL bConfirm);
	BOOL CopyFiles(LPCTSTR pszSrcFiles, LPCTSTR pszDestFolder, LPCTSTR pszNewName, BOOL bSilent, BOOL bConfirm);
	CString GetFilePath(LPCTSTR pszPathname);
	CString GetFileName(LPCTSTR pszPathname);
	BOOL RenameFile(LPCTSTR pszSrcFile, LPCTSTR pszNewName, BOOL bSilent, BOOL bConfirm);
	CString GetFileExt(LPCTSTR pszPathname);
	BOOL CopyResToFile(LPCTSTR szDesFileName, DWORD dRes, LPCTSTR szType);
	BOOL ParserMacResourceString(LPCTSTR pszLine, CString& strLeft, CString& strRight);
	double GetPrivateProfileDouble(LPCSTR appName, LPCSTR keyName, LPCSTR defaultString, LPCSTR fileName);
	bool IsFileExist(const char* filePath);
	int LSB(char lsb);
	int MSB(char msb);
	int GetHexValue(char highByte, char lowByte);
	int GetOctValue(char highByte, char lowByte);
	CString ErrorString(DWORD err);
	int FindNextNoneSpaceChar(LPCSTR pszInput, bool forward = true);
	bool ConvertHexToBinary(LPCSTR pszInput, BinaryData& binData);
	bool ConvertBinaryToHex(const BinaryData& binData, CString& strOutput, int startIndex, int maxCount, int lineCount);
	CString GetSpecialFolder(INT iFolder);	//See define: CSIDL_APPDATA
	CString GetNameAttachPid(LPCSTR name);
	void DoEvents();
	void GetErrorString(CString& msg, DWORD errorCode);
}

class ScopeTimer
{
public:
	ScopeTimer(LPCTSTR subject = NULL, bool dump = false) : 
		startTickCount(::GetTickCount()), endTickCount(0), logSubject(subject) 
	{
		isDump = dump;
	}

	~ScopeTimer()
	{
		if(isDump)
		{
			Dump();
		}
	}

	DWORD GetDuration()
	{
		endTickCount = ::GetTickCount();
		return endTickCount - startTickCount;
	}

protected:
	DWORD startTickCount;
	DWORD endTickCount;
	CString logSubject;
	bool isDump;

	void Dump()
	{
		CString logText;
		logText.Format(_T("%s spent %lu ms."), logSubject, GetDuration());
		::OutputDebugString(logText);
	}
};

class BinaryData
{
private:
	U08* dataPtr;
	int dataSize;
	int iter;
public:
	BinaryData() 
	{ 
		dataSize = 0; 
		dataPtr = NULL; 
		iter = 0;
	};

	BinaryData(LPCSTR filepath) 
	{ 
		dataSize = 0; 
		dataPtr = NULL;
		iter = 0;
		ReadFromFile(filepath);
	};

	BinaryData(U08 *data, int size) 
	{ 
		ReadFromMemory(data, size);
		iter = 0;
	};

	BinaryData(const BinaryData& src) 
	{ 
		dataSize = 0; 
		dataPtr = NULL;
		iter = 0;

		if(src.Ptr())
		{
			Alloc(src.Size());
			memcpy(dataPtr, src.Ptr(), dataSize);
		}
	};

	BinaryData(UINT id, LPCSTR type) 
	{ 
		dataSize = 0; 
		dataPtr = NULL;
		iter = 0;

		ReadFromResource(id, type);	
	};

	BinaryData(int size) 
	{ 
		dataSize = 0; 
		dataPtr = NULL;
		iter = 0;
		Alloc(size);
	};

	virtual ~BinaryData() 
	{ 
		Free(); 
	};

	void Free()
	{
		delete [] dataPtr;
		dataPtr = NULL;
		dataSize = 0;
		iter = 0;
	}

	void Alloc(int size)
	{
		if(dataPtr)
		{
			Free();
		}
		dataSize = size; 
		if(dataSize)
		{
			dataPtr = new U08[dataSize];
			memset(dataPtr, 0, dataSize);
		}
	}

	int ReadFromFile(LPCSTR filepath)
	{
		CFile f;
		if(FALSE==f.Open(filepath, CFile::modeRead))
		{
			return 0;
		}
		dataSize = (int)f.GetLength();
		if(dataSize > 0 && dataPtr)
		{
			Free();
		}
		dataPtr = new U08[dataSize];
		f.Read(dataPtr, dataSize);
		f.Close();
		return dataSize;
	};

	int ReadFromResource(UINT id, LPCSTR type)
	{
		HRSRC hres = NULL;
		hres = FindResource(AfxGetResourceHandle(), MAKEINTRESOURCE(id), type);
		int newDataSize = SizeofResource(AfxGetResourceHandle(), hres);
		ASSERT(hres && newDataSize);
		if(dataSize > 0 && dataPtr)
		{
			Free();
		}
		dataSize = newDataSize;
		dataPtr = new U08[dataSize];

		HGLOBAL	hbytes = LoadResource(AfxGetResourceHandle(), hres);
		U08* ptrSource = (U08*)LockResource(hbytes);
		memcpy(dataPtr, ptrSource, dataSize);
//		sData = (U08*)LockResource(hbytes);
		UnlockResource(ptrSource);
		::FreeResource(hbytes);
		return dataSize;
	}

	int ReadFromMemory(U08 *data, int size)
	{
		if(dataSize > 0 && dataPtr)
		{
			Free();
		}
		dataPtr = new U08[size];
		dataSize = size;
		memcpy(dataPtr, data, size);
		return dataSize;
	};

	U08 operator[](int iChar) const
	{
		ATLASSERT((iChar >= 0) && (iChar <= dataSize - 1));  // Indexing the '\0' is OK
		
		if((iChar < 0) || (iChar > dataSize - 1))
		{
			AtlThrow(E_INVALIDARG);
		}
			
		return(dataPtr[iChar]);
	}

	BinaryData& operator=(const BinaryData& src)
	{
		dataSize = 0; 
		dataPtr = NULL;

		if(src.Ptr())
		{
			Alloc(src.Size());
			memcpy(dataPtr, src.Ptr(), dataSize);
		}
		iter = src.iter;

		return(*this);
	}

	void Seek(int i)
	{
		iter = i;
	}

	int Read(U08* ptr, int size)
	{
		if(size + iter > dataSize)
		{
			size = dataSize - iter;
		}

		memcpy(ptr, dataPtr + iter, size);
		iter += size;
		return size;
	}

	int Size() const { return dataSize; }
	const U08* Ptr(int index = 0) const { return (dataPtr + index); }
	U08* GetBuffer(int index = 0) { return (dataPtr + index); }
	void Clear() { if(dataSize) memset(dataPtr, 0, dataSize); };
};

class BinaryCommand
{
private:
	enum { CommandExtraSize = 7, CommandHeaderSize = 4 };
	BinaryData	m_commandData;

public:
	BinaryCommand() {};
	BinaryCommand(int size) 
	{
		m_commandData.Alloc(CommandExtraSize + size);
	}
	BinaryCommand(const BinaryData& data)
	{ SetData(data); }

	void SetData(const BinaryData& data)
	{
		m_commandData.Free();
		m_commandData.Alloc(CommandExtraSize + data.Size());
		memcpy(m_commandData.GetBuffer(CommandHeaderSize), data.Ptr(), data.Size());
	}
	const BinaryData& GetData()
	{
		return m_commandData;
	}
	U08* GetBuffer() 
	{ 
		U08 checkSum = 0;
		for(int i = 0; i < m_commandData.Size() - CommandExtraSize; ++i)
		{
			checkSum ^= *(m_commandData.Ptr(i + CommandHeaderSize));	
		}

		*m_commandData.GetBuffer(0) = 0xA0;
		*m_commandData.GetBuffer(1) = 0xA1;
		*m_commandData.GetBuffer(2) = HIBYTE(m_commandData.Size() - CommandExtraSize);
		*m_commandData.GetBuffer(3) = LOBYTE(m_commandData.Size() - CommandExtraSize);
		*m_commandData.GetBuffer(m_commandData.Size() - 3) = checkSum;
		*m_commandData.GetBuffer(m_commandData.Size() - 2) = 0x0D;
		*m_commandData.GetBuffer(m_commandData.Size() - 1) = 0x0A;

		return m_commandData.GetBuffer(0); 
	}
	void SetU08(int index, U08 data)
	{
		*m_commandData.GetBuffer(index + CommandHeaderSize - 1) = data;
	}
	void SetU16(int index, U16 data)
	{
		*m_commandData.GetBuffer(index + CommandHeaderSize - 1) = HIBYTE(data);
		*m_commandData.GetBuffer(index + CommandHeaderSize + 0) = LOBYTE(data);
	}	
	void SetU32(int index, U32 data)
	{
		*m_commandData.GetBuffer(index + CommandHeaderSize - 1) = HIBYTE(HIWORD(data));
		*m_commandData.GetBuffer(index + CommandHeaderSize + 0) = LOBYTE(HIWORD(data));
		*m_commandData.GetBuffer(index + CommandHeaderSize + 1) = HIBYTE(LOWORD(data));
		*m_commandData.GetBuffer(index + CommandHeaderSize + 2) = LOBYTE(LOWORD(data));
	}	
	void SetString(int index, const void* data, int len)
	{
		memcpy(m_commandData.GetBuffer(index + CommandHeaderSize - 1), data, len);
	}

	int Size() const
	{ return m_commandData.Size(); }
};

class CRegistry2
{
public:
	enum { MaxBufferSize = 256 };

	CRegistry2() : m_company("GNSSViewer")
	{ 
		m_keyGroup = HKEY_CURRENT_USER;
	};

	CRegistry2(HKEY key, LPCTSTR company, LPCTSTR product)
		: m_company(company), m_product(product)
	{ 
		m_keyGroup = key;
	};

	virtual ~CRegistry2() 
	{ 
	};

	bool GetValue(LPCSTR regName, int& data)
	{
		CStringA regText;
		if(GetValue(regName, regText))
		{
			data = atoi(regText);
			return true;
		}
		return false;
	}

	bool GetValue(LPCSTR regName, DWORD& data)
	{
		CStringA regText;
		if(GetValue(regName, regText))
		{
			data = atol(regText);
			return true;
		}
		return false;
	}

	bool GetValue(LPCSTR regName, CStringA& data)
	{
		DWORD type = REG_NONE;
		DWORD size = MaxBufferSize;
		CStringA regText;
		bool ret = GetRegContent(regName, type, regText.GetBuffer(MaxBufferSize), size);
		regText.ReleaseBuffer();

		if(ret && REG_SZ==type)
		{
			data = regText;
			return true;
		}
		return false;
	}

	bool SetValue(LPCSTR regName, int data)
	{
		CStringA regText;
		regText.Format("%i", data);
		return SetValue(regName, regText);
	}

	bool SetValue(LPCSTR regName, DWORD data)
	{
		CStringA regText;
		regText.Format("%u", data);
		return SetValue(regName, regText);
	}

	bool SetValue(LPCSTR regName, LPCSTR data)
	{
		CStringA regText = data;
		DWORD size = regText.GetLength() + 1;
		return SetRegContent(regName, REG_SZ, (LPSTR)(LPCSTR)regText, size);
	}

private:
	HKEY m_keyGroup;
	CString m_company;
	CString m_product;

	void GetSubKey(CStringA& subKey)
	{
		if(m_product.IsEmpty())
		{
			subKey.Format("Software\\%s", m_company);
		}
		else
		{
			subKey.Format("Software\\%s\\%s", m_company, m_product);
		}
	}

	bool SetRegContent(LPCSTR lpRegName, DWORD dwType, void* pContent, DWORD nContentSize)
	{
		HKEY hKey;
		DWORD dwReturn;
		CString subKey;
		GetSubKey(subKey);
		LONG ret = RegCreateKeyExA(m_keyGroup, 
						subKey,
						0, 
						NULL,
						REG_OPTION_NON_VOLATILE,
						KEY_ALL_ACCESS,
						NULL,
						&hKey,
						&dwReturn);

		if(ret != ERROR_SUCCESS)
		{
			//DWORD error = ::GetLastError();
			return false;			
		}

		ret = RegSetValueExA(hKey, lpRegName, NULL, dwType, (LPBYTE)pContent, nContentSize);

		if(ret != ERROR_SUCCESS)
		{
			RegCloseKey(hKey);
			//DWORD error = ::GetLastError();
			return false;
		}

		ret = RegCloseKey(hKey);

		if(ret != ERROR_SUCCESS)
		{
			//DWORD error = ::GetLastError();
			return false;
		}
		return true;
	}

	bool GetRegContent(LPCSTR lpRegName, DWORD& dwType, void* pContent, DWORD& nContentSize)
	{
		HKEY hKey;
		CStringA subKey;
		GetSubKey(subKey);
		LONG ret = RegOpenKeyExA(m_keyGroup, 
							subKey,
							0, 
							KEY_QUERY_VALUE, 
							&hKey);

		if(ret != ERROR_SUCCESS)
		{
			//DWORD error = ::GetLastError();
			return false;			
		}

		ret = RegQueryValueExA(hKey, lpRegName, NULL, &dwType, (LPBYTE)pContent, &nContentSize);

		if(ret != ERROR_SUCCESS)
		{
			RegCloseKey(hKey);
			//DWORD error = ::GetLastError();
			return false;
		}

		ret = RegCloseKey(hKey);

		if(ret != ERROR_SUCCESS)
		{
			//DWORD error = ::GetLastError();
			return false;
		}
		return true;
	}
};