// Serial.cpp

#include "stdafx.h"
#include "Serial.h"
#include "Utility.h"

const DWORD defaultSendUnit = 512;

char CSerial::debugBuffer[debugSize] = { 0 };
char *CSerial::debugPtr = debugBuffer;
CString CSerial::debugWorking;
bool CSerial::debugModeOn = false;
CString CSerial::debugName("CSerial.log");
inline void CSerial::AddDebugString(const char* dbg)
{
	if(!debugModeOn)
	{
		return;
	}
	int l = strlen(dbg);
	strcpy_s(debugPtr, l + 1, dbg);
	debugPtr += l;
	strcpy_s(debugPtr, l + 1, "\n");
	debugPtr += 1;
}

void CSerial::SaveDebugString(bool backup)
{
	::GetModuleFileName(0, debugWorking.GetBuffer(1024), 1024);
	debugWorking.ReleaseBuffer();

	CString timeStamp;
	if(backup)
	{
		CTime t = CTime::GetCurrentTime();
		timeStamp.Format("%02d%02d%02d%02d%02d%02d-", t.GetYear() - 2000, t.GetMonth(), t.GetDay(),
			t.GetHour(), t.GetMinute(), t.GetSecond());
	}

	debugWorking = debugWorking.Left(debugWorking.ReverseFind('\\') + 1) + timeStamp + debugName;
	if(!backup)
	{
		::DeleteFile(debugWorking);
	}

	CFile f;
	f.Open(debugWorking, CFile::modeCreate | CFile::modeWrite);
	f.Write(debugBuffer, strlen(debugBuffer));
	f.Close();
}

CSerial::CSerial()
{
	memset(&m_OverlappedRead, 0, sizeof(OVERLAPPED));
	memset(&m_OverlappedWrite, 0, sizeof(OVERLAPPED));

	m_comDeviceHandle = NULL;
	m_comPort = 0;
	m_baudRate = 0;
	m_cancelTransmission = false;
	m_isOpened = false;
	m_sendUnit = defaultSendUnit;
	m_logStatus = 0;
} 

CSerial::~CSerial()
{
	Close();
}

bool CSerial::Open(int port, int baudIndex)
{
	CString comPort;
	comPort.Format("COM%d", port);
	return OpenByBaudrate(comPort, g_setting.BaudrateValue(baudIndex));
}

bool CSerial::OpenByBaudrate(LPCSTR comPort, int baudrate)
{
	CString portName;
	portName.Format("\\\\.\\%s", comPort);

  //160726 Test
  //if(SPECIAL_BAUD_RATE)
  //{
  //  baudrate = (int)(1.5 * baudrate);
  //}

	int tryCnt = 10;
	Utility::Log(__FUNCTION__, CTime::GetCurrentTime().Format("%Y%m%d%H%M%S"), __LINE__);
	while(TRUE)
	{
		Utility::Log(__FUNCTION__, CTime::GetCurrentTime().Format("%Y%m%d%H%M%S"), __LINE__);
		m_comDeviceHandle = CreateFile(portName, GENERIC_READ | GENERIC_WRITE,
				0, NULL, OPEN_EXISTING,  0, 0);
		Utility::Log(__FUNCTION__, CTime::GetCurrentTime().Format("%Y%m%d%H%M%S"), __LINE__);

		if(m_comDeviceHandle==INVALID_HANDLE_VALUE)
		{
			if(--tryCnt == 0)
			{
				errorCode = ::GetLastError();
				return false;
			}
			else
			{
				Utility::Log(__FUNCTION__, CTime::GetCurrentTime().Format("%Y%m%d%H%M%S"), __LINE__);
				Sleep(50);
				continue;
			}
		}
		break;
	}
	Utility::Log(__FUNCTION__, CTime::GetCurrentTime().Format("%Y%m%d%H%M%S"), __LINE__);

	ComInitial();
	Utility::Log(__FUNCTION__, CTime::GetCurrentTime().Format("%Y%m%d%H%M%S"), __LINE__);

	if(!SetupComm(m_comDeviceHandle, InQueueSize, OutQueueSize))
	{
		return false;
	}

	if(!PurgeComm(m_comDeviceHandle, 
			PURGE_TXABORT | PURGE_RXABORT |
			PURGE_TXCLEAR |	PURGE_RXCLEAR))
	{
		return false;
	}
	Utility::Log(__FUNCTION__, CTime::GetCurrentTime().Format("%Y%m%d%H%M%S"), __LINE__);

	if(!ResetPortNoDelay(baudrate))
	{
		return false;
	}

	Utility::Log(__FUNCTION__, CTime::GetCurrentTime().Format("%Y%m%d%H%M%S"), __LINE__);
	m_OverlappedRead.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	Utility::Log(__FUNCTION__, CTime::GetCurrentTime().Format("%Y%m%d%H%M%S"), __LINE__);
	m_OverlappedWrite.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	Utility::Log(__FUNCTION__, CTime::GetCurrentTime().Format("%Y%m%d%H%M%S"), __LINE__);

	m_isOpened = true;
	CString t(comPort);
	t = t.Right(t.GetLength() - 3);
	m_comPort = atoi(t);
	return true;
}

int CSerial::ComInitial()
{
	DWORD error = 0;
	COMSTAT comStatus = {0};
	ClearCommError(m_comDeviceHandle, &error, &comStatus);
	return comStatus.cbInQue;
}

void CSerial::Close()
{
	if(!m_isOpened || m_comDeviceHandle == NULL) 
	{
		return;
	}

	if(m_OverlappedRead.hEvent != NULL) 
	{
		CloseHandle(m_OverlappedRead.hEvent);
		m_OverlappedRead.hEvent = NULL;
	}

	if(m_OverlappedWrite.hEvent != NULL) 
	{
		CloseHandle(m_OverlappedWrite.hEvent);
		m_OverlappedWrite.hEvent = NULL;
	}

	CloseHandle(m_comDeviceHandle);
	m_comDeviceHandle = NULL;
	m_isOpened = false;
	return;
}

int CSerial::WaitingDataIn()
{
	int loopcount = 0;
	DWORD error = 0;
	COMSTAT comStatus = {0};
	ClearCommError(m_comDeviceHandle, &error, &comStatus);

	while(comStatus.cbInQue==0) 
	{
		++loopcount;
		if(loopcount == MaxQueueLoop)
		{
			return 0;
		}

		if(m_cancelTransmission)
		{
			m_cancelTransmission = false;
			return -1;
		}
		Sleep(2);
		ClearCommError(m_comDeviceHandle, &error, &comStatus);
	};
	return comStatus.cbInQue;
}

//Read data until buffer full, or com port is empty.
// if once is false, it'll check com port empty again after read data.
DWORD CSerial::ReadData(void* buffer, DWORD bufferSize, bool once)
{
	if(!m_isOpened || m_comDeviceHandle == NULL) 
	{
		return READ_ERROR;
	}

	DWORD totalSize = 0;
	char* bufferIter = (char*)buffer;
	do
	{
		DWORD dwBytesRead = 0;
		dwBytesRead = (DWORD)WaitingDataIn();
		if(0 == dwBytesRead)
		{
			return totalSize;
		}

		if(READ_ERROR == dwBytesRead)
		{	//User cancel
			return READ_ERROR;
		}

		if(bufferSize < (totalSize + dwBytesRead)) 
		{
			dwBytesRead = bufferSize - totalSize;
		}

		DWORD dwBytesDoRead = 0;
		BOOL bReadStatus = ReadFile(m_comDeviceHandle, bufferIter, 
							dwBytesRead, &dwBytesDoRead, 0);

		if(!bReadStatus)
		{ 
			DWORD error = ::GetLastError();
			if(error == ERROR_IO_PENDING)
			{
				WaitForSingleObject(m_OverlappedRead.hEvent, IoPendingTimeOut);
				return totalSize + dwBytesDoRead;
			}
			return 0;
		}
		bufferIter += dwBytesRead;
		totalSize += dwBytesRead;
	} while(!once && totalSize < bufferSize);

	return totalSize;
}

DWORD CSerial::SendData(const void* buffer, DWORD bufferSize, bool blockTransfer, int delayDuration)
{
	if(!m_isOpened || m_comDeviceHandle == NULL) 
	{
		return 0;
	}

	DWORD sentBytes = 0;
	DWORD leftSize = bufferSize;
	char* bufferIter = (char*)buffer;

	while(leftSize > 0)
	{
		sentBytes = (leftSize >= m_sendUnit) ? m_sendUnit : leftSize;

		if(blockTransfer)
		{
			WriteCommBytes(bufferIter, sentBytes);
		}
		else
		{
			for(DWORD i=0; i<sentBytes; ++i)
			{
				WriteCommBytes(bufferIter + i, 1);
			}
		}

		if(delayDuration)
		{
			Sleep(delayDuration);
		}

		bufferIter += sentBytes;
		leftSize -= sentBytes;
	}
	return bufferSize;	
}

bool CSerial::WriteCommBytes(char* buffer, int bufferSize)
{
	DWORD dwBytesDoWrite = 0;
	BOOL bWriteStatus = WriteFile(m_comDeviceHandle, buffer, 
							bufferSize, &dwBytesDoWrite, &m_OverlappedWrite);

	if(!bWriteStatus)
	{ 
		DWORD error = ::GetLastError();
		if(error == ERROR_IO_PENDING)
		{
			WaitForSingleObject(m_OverlappedWrite.hEvent, IoPendingTimeOut);
			GetOverlappedResult(m_comDeviceHandle, &m_OverlappedWrite, &dwBytesDoWrite, FALSE);
			m_OverlappedWrite.Offset += dwBytesDoWrite;
		}
		else
		{
			return false;
		}
	}
	return true;
}

bool CSerial::ResetPortNoDelay(int b)
{
	DCB dcb = {0};
	if(!GetCommState(m_comDeviceHandle, &dcb))
	{
		DWORD error = ::GetLastError();
		return false;
	}

  //if(SPECIAL_BAUD_RATE)
  //  b = (int)(b * 1.5);

	dcb.BaudRate = b;
	dcb.Parity   = NOPARITY;
	dcb.ByteSize = 8;
	dcb.StopBits = ONESTOPBIT;

	if(!SetCommState(m_comDeviceHandle, &dcb))
	{
		DWORD error = ::GetLastError();
		return false;
	}

	m_baudRate = b;
	return true;
}

bool CSerial::ResetPort(int baudIndex)
{
  //160726 Test Alex
	//if(ResetPortNoDelay((int)(Setting::BaudrateTable[baudIndex])))
	if(ResetPortNoDelay(g_setting.BaudrateValue(baudIndex)))
	{
		Sleep(800);
		return true;
	}
	return false;
}

void CSerial::ClearQueue()
{
	if(!m_isOpened || m_comDeviceHandle == NULL) 
	{
		return;
	}
	ComInitial();
	PurgeComm(m_comDeviceHandle, PURGE_RXCLEAR);
	FlushFileBuffers(m_comDeviceHandle);
}

//Read until eos or empty.
DWORD CSerial::GetString(void* buffer, DWORD bufferSize, DWORD timeOut)
{
	char* bufferIter = (char*)buffer;
	DWORD totalSize = 0;
	ScopeTimer t;
	do
	{ 
		DWORD nBytesRead = ReadData(bufferIter, 1);
		if(nBytesRead == 0)
		{
			if(t.GetDuration() > timeOut)
			{
				break;
			}
			else
			{
				Sleep(10);
			}
		}

		if(*bufferIter == 0)
		{
			break;
		}

		bufferIter += nBytesRead;
		totalSize += nBytesRead;
	} while(totalSize < bufferSize);
	return totalSize;
}

enum BufferContentType
{
	Unknown = 0,
	BinaryMessageStart,
	BinaryMessageEnd,
	MessageEnd,
};

BufferContentType CheckBufferContent(U08 current, U08 previous, U08 bufferStart, U08 sizeHigh, U08 sizeLow, DWORD total)
{
	if(BINARY_HD2==current && BINARY_HD1==previous)
	{
		return BinaryMessageStart;
	}

	if(ASCII_LF==current && ASCII_CR==previous)
	{
		if(BINARY_HD1==bufferStart) 
		{
			int packetLength = (sizeHigh << 8) + sizeLow;
			if(total == (packetLength + 6))
			{
				return BinaryMessageEnd;
			}
		}
		return MessageEnd;
	}
	return Unknown;
}

DWORD CSerial::GetBinary(void *buffer, DWORD bufferSize, DWORD timeout)
{	
	U08* bufferIter = (U08*)buffer;
	DWORD totalSize = 0;
	ScopeTimer t;
	bool cmdHeaderCome = false;
	while(totalSize < bufferSize - 1)
	{ 
		if(t.GetDuration() > timeout)
		{
			return READ_ERROR;
		}

		if(m_cancelTransmission)
		{
			m_cancelTransmission = false;
			return READ_ERROR;
		}

		DWORD dwBytesDoRead = 0;
		DWORD dwErrorFlags = 0;
		COMSTAT comStat;
		BOOL b2 = ClearCommError(m_comDeviceHandle, &dwErrorFlags, &comStat);
		if(!b2)
		{
			DWORD err = ::GetLastError();
			return -1;
		}
		if(comStat.cbInQue == 0) 
		{
			Sleep(2);
			continue;
		}

		DWORD bytesinbuff = comStat.cbInQue;
		while(bytesinbuff)
		{
			if(t.GetDuration() > timeout)
			{
				return READ_ERROR;
			}
			if(m_cancelTransmission)
			{
				m_cancelTransmission = false;
				return READ_ERROR;
			}
			BOOL bb = ReadFile(m_comDeviceHandle, bufferIter, 1, &dwBytesDoRead, 0);
			if(dwBytesDoRead == 0)
			{	//Read fail.
				DWORD dwErr = ::GetLastError();
				continue;
			}

			if(totalSize > 0)
			{	//not first char.
				if(!cmdHeaderCome && *bufferIter==0xa1 && *(bufferIter-1)==0xa0)
				{
					bufferIter -= totalSize;
					*bufferIter = 0xa0; 
					++bufferIter;
					*bufferIter = 0xa1; 
					++bufferIter;
					totalSize = 2;
					cmdHeaderCome = true;
					continue;
				}
				else if(*bufferIter==0x0a && *(bufferIter-1)==0x0d)
				{
					unsigned char *chk_ptr = bufferIter - totalSize;
					
					if (*chk_ptr == 0xa0)
					{
						int tmp_len = *(chk_ptr + 2);
						tmp_len = tmp_len << 8 | *(chk_ptr+3);
						if (totalSize == tmp_len + 6) 
						{
							*(bufferIter+1) = 0;
							return totalSize + 1;
						}
						cmdHeaderCome = false;
					}
					else
					{
						return totalSize;
					}
				}
			}
			++totalSize;
			if (totalSize >=  bufferSize - 1)
			{	//Check 
				*(bufferIter+1) = 0;
				break;
			}
				
			++bufferIter;
			--bytesinbuff;
		} //while(bytesinbuff)
	} //while(total < size - 1)
	return totalSize;
}

#if CUSTOMER_ZENLANE_160808
DWORD CSerial::GetZenlaneMessage(void *buffer, DWORD bufferSize, DWORD timeout)
{	
	U08* bufferIter = (U08*)buffer;
	DWORD totalSize = 0;
	ScopeTimer t;
	bool cmdHeaderCome = false;
	while(totalSize < bufferSize - 1)
	{ 
		if(t.GetDuration() > timeout)
		{
			return READ_ERROR;
		}

		if(m_cancelTransmission)
		{
			m_cancelTransmission = false;
			return READ_ERROR;
		}

		DWORD dwBytesDoRead = 0;
		DWORD dwErrorFlags = 0;
		COMSTAT comStat;
		BOOL b2 = ClearCommError(m_comDeviceHandle, &dwErrorFlags, &comStat);
		if(!b2)
		{
			DWORD err = ::GetLastError();
			return -1;
		}
		if(comStat.cbInQue == 0) 
		{
			Sleep(2);
			continue;
		}

		DWORD bytesinbuff = comStat.cbInQue;
		while(bytesinbuff)
		{
			if(t.GetDuration() > timeout)
			{
				return READ_ERROR;
			}
			if(m_cancelTransmission)
			{
				m_cancelTransmission = false;
				return READ_ERROR;
			}
			BOOL bb = ReadFile(m_comDeviceHandle, bufferIter, 1, &dwBytesDoRead, 0);
			if(dwBytesDoRead == 0)
			{	//Read fail.
				DWORD dwErr = ::GetLastError();
				continue;
			}
			if(totalSize > 0 && *bufferIter == 0)
			{
				return READ_ERROR;
			}
			if(totalSize > 0)
			{	//not first char.
				if(!cmdHeaderCome && *bufferIter==0xa1 && *(bufferIter-1)==0xa0)
				{
					bufferIter -= totalSize;
					*bufferIter = 0xa0; 
					++bufferIter;
					*bufferIter = 0xa1; 
					++bufferIter;
					totalSize = 2;
					cmdHeaderCome = true;
					continue;
				}
				else if(*bufferIter==0x0a && *(bufferIter-1)==0x0d)
				{
					unsigned char *chk_ptr = bufferIter - totalSize;
					
					if (*chk_ptr == 0xa0)
					{
						int tmp_len = *(chk_ptr + 2);
						tmp_len = tmp_len << 8 | *(chk_ptr+3);
						if (totalSize == tmp_len + 6) 
						{
							*(bufferIter+1) = 0;
							return totalSize + 1;
						}
						cmdHeaderCome = false;
					}
					else
					{
						return totalSize;
					}
				}
			  ++totalSize;
			  if (totalSize >=  bufferSize - 1)
			  {	//Check 
				  *(bufferIter+1) = 0;
				  break;
        }	
      
			  ++bufferIter;
			  --bytesinbuff;
			}
      else if(*bufferIter == '$')
      {
			  ++totalSize;
			  if (totalSize >=  bufferSize - 1)
			  {	//Check 
				  *(bufferIter+1) = 0;
				  break;
        }	
			  ++bufferIter;
			  --bytesinbuff;
      }
		} //while(bytesinbuff)
	} //while(total < size - 1)
	return totalSize;
}
#endif

DWORD CSerial::GetZenlaneResponse1(void *buffer, DWORD bufferSize, DWORD timeout)
{	
	//timeout = 0xFFFFFFFF;
	U08* bufferIter = (U08*)buffer;
	DWORD totalSize = 0;
	ScopeTimer t;
	bool cmdHeaderCome = false;
	while(totalSize < bufferSize - 1)
	{ 
		if(t.GetDuration() > timeout)
		{
			return READ_ERROR;
		}

		if(m_cancelTransmission)
		{
			m_cancelTransmission = false;
			return READ_ERROR;
		}

		DWORD dwBytesDoRead = 0;
		DWORD dwErrorFlags = 0;
		COMSTAT comStat;
		BOOL b2 = ClearCommError(m_comDeviceHandle, &dwErrorFlags, &comStat);
		if(!b2)
		{
			DWORD err = ::GetLastError();
			return -1;
		}
		if(comStat.cbInQue == 0) 
		{
			Sleep(2);
			continue;
		}

		DWORD bytesinbuff = comStat.cbInQue;
		while(bytesinbuff)
		{
			if(t.GetDuration() > timeout)
			{
				return READ_ERROR;
			}
			if(m_cancelTransmission)
			{
				m_cancelTransmission = false;
				return READ_ERROR;
			}
			BOOL bb = ReadFile(m_comDeviceHandle, bufferIter, 1, &dwBytesDoRead, 0);
			if(dwBytesDoRead == 0)
			{	//Read fail.
				DWORD dwErr = ::GetLastError();
				continue;
			}

			if(totalSize > 0)
			{	//not first char.
				if(!cmdHeaderCome && *bufferIter==0xa1 && *(bufferIter-1)==0xa0)
				{
					bufferIter -= totalSize;
					*bufferIter = 0xa0; 
					++bufferIter;
					*bufferIter = 0xa1; 
					++bufferIter;
					totalSize = 2;
					cmdHeaderCome = true;
					continue;
				}
				else if(*bufferIter==0x0a && *(bufferIter-1)==0x0d)
				{
					unsigned char *chk_ptr = bufferIter - totalSize;
					
					if (*chk_ptr == 0xa0)
					{
						int tmp_len = *(chk_ptr + 2);
						tmp_len = tmp_len << 8 | *(chk_ptr+3);
						if (totalSize == tmp_len + 6) 
						{
							*(bufferIter+1) = 0;
							return totalSize + 1;
						}
						cmdHeaderCome = false;
					}
					else
					{
						return totalSize;
					}
				}
			}
			++totalSize;
			if (totalSize >=  bufferSize - 1)
			{	//Check 
				*(bufferIter+1) = 0;
				break;
			}
				
			++bufferIter;
			--bytesinbuff;
		} //while(bytesinbuff)
	} //while(total < size - 1)
	return totalSize;
}

DWORD CSerial::GetBinaryAck(void *buffer, DWORD bufferSize, DWORD timeout)
{	
	U08* bufferIter = (U08*)buffer;
	DWORD totalSize = 0;
	DWORD failCount = 10;
	ScopeTimer clock;
	while(totalSize < bufferSize - 1)
	{ 
		if(clock.GetDuration() > timeout)
		{
			return READ_ERROR;
		}

		if(m_cancelTransmission)
		{
			m_cancelTransmission = false;
			return READ_ERROR;
		}

		DWORD dwBytesDoRead = 0;
		DWORD dwErrorFlags = 0;
		COMSTAT comStat;
		BOOL b2 = ClearCommError(m_comDeviceHandle, &dwErrorFlags, &comStat);
		if(!b2)
		{
			DWORD err = ::GetLastError();
			return -1;
		}
		if(comStat.cbInQue <= 0) 
		{
			Sleep(10);
			continue;
		}

		DWORD bytesinbuff = comStat.cbInQue;
		while(bytesinbuff)
		{
			if(m_cancelTransmission)
			{
				m_cancelTransmission = false;
				return -1;
			}
			BOOL bb = ReadFile(m_comDeviceHandle, bufferIter, 1, &dwBytesDoRead, 0);
			if(dwBytesDoRead <= 0)
			{	//Read fail.
				DWORD dwErr = ::GetLastError();
				continue;
			}

			if(totalSize > 0)
			{	//not first char.
				if(*bufferIter==0xa1 && *(bufferIter-1)==0xa0)
				{
					bufferIter -= totalSize;
					*bufferIter = 0xa0; 
					++bufferIter;
					*bufferIter = 0xa1; 
					++bufferIter;
					totalSize = 2;
					continue;
				}
				else if(*bufferIter==0x0a && *(bufferIter-1)==0x0d)
				{
					unsigned char *chk_ptr = bufferIter - totalSize;
					
					if (*chk_ptr == 0xa0)
					{
						int tmp_len = *(chk_ptr + 2);
						tmp_len = tmp_len << 8 | *(chk_ptr+3);
						if (totalSize == tmp_len + 6) 
						{
							*(bufferIter+1) = 0;
							return totalSize + 1;
						}
					}
					else
					{
						return totalSize;
					}
				}
			}
			++totalSize;
			if (totalSize >=  bufferSize - 1)
			{	//Check 
				*(bufferIter+1) = 0;

				break;
			}
				
			++bufferIter;
			--bytesinbuff;
		} //while(bytesinbuff)
	} //while(total < size - 1)
	return totalSize;
}


DWORD CSerial::GetBinaryBlock(void* buffer, DWORD bufferSize, DWORD blockSize)
{	
	U08* bufferIter = (U08*)buffer;
	DWORD totalSize = 0;
	do
	{ 
		DWORD nBytesRead = ReadData(bufferIter, 1);
		if(nBytesRead <= 0)
		{
			continue;
		}

		if(totalSize == blockSize && (*bufferIter==ASCII_LF) && (*(bufferIter-1)==ASCII_CR))
		{
			break;
		}
		++totalSize;
		if(totalSize >=  bufferSize - 1)
		{	//add end of string (eos)
			break;
		}
		bufferIter++;
	} while(totalSize < bufferSize);
	return totalSize;
}

DWORD CSerial::GetBinaryBlockInSize(void* buffer, DWORD bufferSize, DWORD blockSize)
{	
	U08* bufferIter = (U08*)buffer;
	DWORD totalSize = 0;
	do
	{ 
		DWORD nBytesRead = ReadData(bufferIter, 1);
		if(nBytesRead <= 0)
		{
			continue;
		}

		if(++totalSize == blockSize)
		{
			break;
		}
		if(totalSize >=  bufferSize - 1)
		{	//add end of string (eos)
			break;
		}
		bufferIter++;
	} while(totalSize < bufferSize);
	return totalSize;
}

//DWORD CSerial::GetBinaryBlockInTime(void* buffer, DWORD bufferSize, DWORD timeout)
//{	
//	U08* bufferIter = (U08*)buffer;
//	DWORD totalSize = 0;
//	const int tempBufferSize = 1024;
//	U08* tmpBuffer[tempBufferSize];
//	ScopeTimer t;
//	do
//	{
//		if(t.GetDuration() >= timeout)
//		{
//			return READ_ERROR;
//		}
//
//		memset(tmpBuffer, 0, tempBufferSize);
//		int readSize = ((totalSize + tempBufferSize) <= bufferSize) ? tempBufferSize : bufferSize - totalSize;
//		DWORD nBytesRead = ReadData(tmpBuffer, readSize);
//		if(nBytesRead == 0)
//		{
//			continue;
//		}
//
//		if(nBytesRead == READ_ERROR)
//		{
//			return READ_ERROR;
//		}
//
//		if((totalSize + nBytesRead) <= bufferSize)
//		{
//			memcpy(bufferIter, tmpBuffer, nBytesRead);
//			bufferIter += nBytesRead;
//			totalSize += nBytesRead;
//		}
//	} while(totalSize < bufferSize);
//	return totalSize;
//}