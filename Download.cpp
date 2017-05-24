#include "stdafx.h"
#include "GPS.h"
#include "Serial.h"
#include "GPSDlg.h"

#include "SoftImDwDlg.h"
#include "InputPasswordDlg.h"
#include "CryptHash.inl"

#define tagSize 8L
const unsigned char GG12A_TAG[tagSize] = {0xf4,0x0e,0xe0,0xb8,0x16,0xa0,0x01,0xa2};
const unsigned char TAG_ADDR[tagSize] = {0xf4,0x0e,0xe0,0xb8,0x16,0xa0,0x01,0xa1};
const int BinSizeTimeout = 20*1000;

typedef struct 
{
	char inputFile[256];
	char outputFile[256];
	int gg12a;
	unsigned long long tagAddress;
} Param;

int ParserBinFile(const char *filename, Param *p)
{
	unsigned char buf[tagSize];
	int totalTagSize = 0;

#if defined(_MSC_VER)
	FILE* f = NULL;
	fopen_s(&f, filename, "rb");
#else
	FILE* f = fopen(filename, "rb");
#endif

	p->inputFile[0] = 0;
	p->outputFile[0] = 0;
	p->gg12a = 0;
	p->tagAddress = 0;

	if(f==NULL)
	{
		return totalTagSize;
	}

#if defined(_MSC_VER)
	strcpy_s(p->inputFile, sizeof(p->inputFile), filename);
#else
	strcpy(p->inputFile, filename);
#endif

	fseek(f, 0 - tagSize, SEEK_END);		
	fread(buf, 1, tagSize, f);
	if(0==memcmp(buf, GG12A_TAG, tagSize))
	{
		p->gg12a = 1;
		fseek(f, 0 - tagSize, SEEK_END);
		totalTagSize += tagSize;
	}
	fseek(f, 0 - tagSize, SEEK_CUR);		
	fread(buf, 1, tagSize, f);

	if(0==memcmp(buf, TAG_ADDR, tagSize))
	{	
		fseek(f, 0 - tagSize - tagSize, SEEK_CUR);		
		fread(buf, 1, tagSize, f);

		p->tagAddress = *((unsigned long long*)buf);
		totalTagSize += tagSize * 2;
	}
	
	fclose(f);
	return totalTagSize;
}

U16 customerCrc = 0;
UINT CGPSDlg::GetBinFromResource(int baud)
{
	UINT* promTable = NULL;

	UINT glonassPromTable[] = { 
				(IDR_GNSS_PRELOADER_4800),
				(IDR_GNSS_PRELOADER_9600),
				(IDR_GNSS_PRELOADER_19200),
				(IDR_GNSS_PRELOADER_38400),
				(IDR_GNSS_PRELOADER_57600),
				(IDR_GNSS_PRELOADER_115200),
				(IDR_GNSS_PRELOADER_230400),
			};
	UINT gg12aPromTable[] = {
				(IDR_GG12A_PRELOADER_4800),
				(IDR_GG12A_PRELOADER_9600),
				(IDR_GG12A_PRELOADER_19200),
				(IDR_GG12A_PRELOADER_38400),
				(IDR_GG12A_PRELOADER_57600),
				(IDR_GG12A_PRELOADER_115200),
				(IDR_GG12A_PRELOADER_230400),
			};
	UINT gpsPromTable[] = {
			(IDR_GPS_PRELOADER_4800),
			(IDR_GPS_PRELOADER_9600),
			(IDR_GPS_PRELOADER_19200),
			(IDR_GPS_PRELOADER_38400),
			(IDR_GPS_PRELOADER_57600),
			(IDR_GPS_PRELOADER_115200),
			(IDR_GPS_PRELOADER_230400),
		};

	switch(m_DownloadMode)
	{
		case InternalLoaderV6Gps:
		case InternalLoaderV6GpsAddTag:
		case InternalLoaderV6GpsDelTag:
			promTable = gpsPromTable;
			break;
		case InternalLoaderV6Gnss:
		case InternalLoaderV6GnssAddTag:
		case InternalLoaderV6GnssDelTag:
			promTable = glonassPromTable;
			break;
		case InternalLoaderV6Gg12a:
			promTable = gg12aPromTable;
			break;
		default:
			ASSERT(FALSE);
	}
	return promTable[baud];
}

UINT CGPSDlg::GetSrecFromResource(int buad)
{
	UINT gpsAddTagSrecTable[] = {
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			IDR_GPS_CREATETAG_SREC_115200,
			NULL,
			NULL,
			NULL };

	UINT gpsDelTagSrecTable[] = {
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			IDR_GPS_REMOVETAG_SREC_115200,
			NULL,
			NULL,
			NULL };

	UINT gnssAddTagSrecTable[] = {
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			IDR_GNSS_CREATETAG_SREC_115200,
			NULL,
			NULL,
			NULL };

	UINT gnssDelTagSrecTable[] = {
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			IDR_GNSS_REMOVETAG_SREC_115200,
			NULL,
			NULL,
			NULL };

	UINT gpsSrecTable[] = {
			IDR_GPS_SREC_4800,
			IDR_GPS_SREC_9600,
			IDR_GPS_SREC_19200,
			IDR_GPS_SREC_38400,
			IDR_GPS_SREC_57600,
			IDR_GPS_SREC_115200,
			IDR_GPS_SREC_230400,
			NULL,
			NULL };

	UINT gnssSrecTable[] = {
			IDR_GNSS_SREC_4800,
			IDR_GNSS_SREC_9600,
			IDR_GNSS_SREC_19200,
			IDR_GNSS_SREC_38400,
			IDR_GNSS_SREC_57600,
			IDR_GNSS_SREC_115200,
			IDR_GNSS_SREC_230400,
			NULL,
			NULL };

	UINT parallelSrecTable0[] = {
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			IDR_PARALLEL_TYPE0_SREC_115200,
			NULL,
			NULL,
			NULL };

	UINT parallelSrecTable1[] = {
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			IDR_PARALLEL_TYPE1_SREC_115200,
			NULL,
			NULL,
			NULL };

	UINT v8SrecTable[] = {
			NULL,
			NULL,
			NULL,
			NULL,
			NULL,
			IDR_V8_DL_SREC_115200,
			IDR_V8_DL_SREC_230400,
			IDR_V8_DL_SREC_460800,
			NULL };

	UINT v8AddTagSrecTable[] = {
			IDR_V8_AT_SREC,
			IDR_V8_AT_SREC,
			IDR_V8_AT_SREC,
			IDR_V8_AT_SREC,
			IDR_V8_AT_SREC,
			IDR_V8_AT_SREC,
			IDR_V8_AT_SREC,
			IDR_V8_AT_SREC,
			IDR_V8_AT_SREC };
			
	UINT* srecTable = NULL;
	switch(m_DownloadMode)
	{
		case EnternalLoader:
			ASSERT(FALSE);
			break;
		case FileLoader:
			ASSERT(FALSE);
			break;
		case EnternalLoaderInBinCmd:
			ASSERT(FALSE);
			break;
		case FileLoaderInBinCmd:
			ASSERT(FALSE);
			break;
		case ParallelDownloadType0:
			srecTable = parallelSrecTable0;
			break;
		case ParallelDownloadType1:
			srecTable = parallelSrecTable1;
			break;
		case InternalLoaderV6Gps:
			srecTable = gpsSrecTable;
			break;
		case InternalLoaderV6Gnss:
			srecTable = gnssSrecTable;
			break;
		case InternalLoaderV6Gg12a:
			ASSERT(FALSE);
			break;
		case InternalLoaderV6GpsAddTag:
			srecTable = gpsAddTagSrecTable;
			break;
		case InternalLoaderV6GpsDelTag:
			srecTable = gpsDelTagSrecTable;
			break;
		case InternalLoaderV6GnssAddTag:
			srecTable = gnssDelTagSrecTable;
			break;
		case InternalLoaderV6GnssDelTag:
			srecTable = gnssDelTagSrecTable;
			break;
		case InternalLoaderV8AddTag:
			srecTable = v8AddTagSrecTable;
			break;
		case HostBasedDownload:
			ASSERT(FALSE);
			break;
		case HostBasedCmdOnly:
			ASSERT(FALSE);
			break;
		case InternalLoaderV8:
			srecTable = v8SrecTable;
			break;
		case InternalLoaderSpecial:
			return _RESOURCE_LOADER_ID_;
		case RomExternalDownload:
			srecTable = v8AddTagSrecTable;
			break;
		default:
			ASSERT(FALSE);
	}
	if(srecTable)
	{
		return srecTable[buad];
	}
	return NULL;
}

DownloadErrocCode ProcessWlfResult(WlfResult w)
{
	DownloadErrocCode err = RETURN_ERROR;
	switch(w)
	{
	case wlf_timeout:
		AfxMessageBox("Timeout: GPS device no response.");						
		break;
	case wlf_error1:
		AfxMessageBox("Error1 : Flash Wrong");	
		break;
	case wlf_error3:
		AfxMessageBox("Error3 : Format Error!");					
		break;
	case wlf_error4:
		AfxMessageBox("Error4 : Flash Test Failed!");				
		break;
	case wlf_error5:
		AfxMessageBox("Error5 : srec timeout!");	
		break;
	default:
		err = RETURN_NO_ERROR;
		break;
	}
	return err;
}

bool CGPSDlg::FirmwareUpdate(const CString& strFwPath)
{
	bool isSuccessful = false;
	for(int i=0; i<3; i++)
	{
		U08 res = 0;
		switch(m_DownloadMode)
		{
			case InternalLoaderV6Gps:
			case InternalLoaderV6Gnss:
			case InternalLoaderV6Gg12a:
			case InternalLoaderV6GpsAddTag:
			case InternalLoaderV6GpsDelTag:
			case InternalLoaderV6GnssAddTag:
			case InternalLoaderV6GnssDelTag:
			case HostBasedDownload:
			case HostBasedCmdOnly:
				res = PlRomNoAlloc2(strFwPath);
				break;
			case EnternalLoader:
			case FileLoader:
			case GpsdoMasterSlave:
			case EnternalLoaderInBinCmd:
			case FileLoaderInBinCmd:
			case InternalLoaderV8:
			case ParallelDownloadType0:
			case ParallelDownloadType1:
			case InternalLoaderV8AddTag:
			case InternalLoaderSpecial:
			case RomExternalDownload:
				res = PlRomNoAllocV8(strFwPath);
				break;
			case CustomerUpgrade:
				res = PlRomCustomerUpgrade(m_nDownloadResource);
				break;
			default:
				ASSERT(FALSE);
		}

		if(res == RETURN_RETRY)
		{
			add_msgtolist("Software Image Download Retry...");
		}
		else 
		{
			if(res == RETURN_NO_ERROR)
			{
				add_msgtolist("Software Image Download successfully");	
				isSuccessful = true;
			}
			else
			{
				add_msgtolist("Software Image Download fails!");
			}
			break;
		}
	}
	return isSuccessful;
}

U08 CalcCheckSum16(const U08* dataPtr, int sentbytes)
{
	U16 checkSum = 0;
	const U08* ptr = dataPtr;
	const U16 loopCount = sentbytes / sizeof(U16);
	U16 i;

	for(i=0; i<loopCount; ++i)
	{
		U16 word =  MAKEWORD(*(ptr+1), *ptr);
		checkSum += word;
		ptr += 2;
	}
	return HIBYTE(checkSum) + LOBYTE(checkSum);
	//*/
}
int CGPSDlg::SendRomBufferCustomerUpgrade(const U08* sData, int sDataSize, BinaryData &data, 
							int fbinSize, bool needSleep, CWnd* notifyWnd)
{
	int nFlashBytes = FlashBytes[m_nDownloadBufferIdx];
	/* sending data */	
	const int headerSize = 3;
	const int buffSize = nFlashBytes + headerSize;
	int realTotalSize = BOOTLOADER_SIZE + fbinSize;
	//U08 buff[buffSize];
	U08* buff = new U08[buffSize];
	char messages[100] = {0};
	bool bResend = false;
	int lSize = 0;
	int sentbytes = 0;
	int TotalByte = 0;
	int nBytesSent = 0;
	U16 sequence = 0;

	if(sData)
	{
		char* prom_ptr = (char*)sData;
		lSize = BOOTLOADER_SIZE;

		while(lSize>0)
		{ 
			if(!bResend)
			{
				sentbytes = nFlashBytes;
				TotalByte += sentbytes;	

				memset(buff, 0, buffSize);
				if(sDataSize > nFlashBytes)
				{
					memcpy(&buff[headerSize], prom_ptr, nFlashBytes);
					sDataSize -= nFlashBytes;
					prom_ptr += nFlashBytes;
				}
				else if(sDataSize > 0)
				{
					memcpy(&buff[headerSize], prom_ptr, sDataSize);
					sDataSize = 0;
				}
				int checkSum = CalcCheckSum16(&(buff[headerSize]), sentbytes);
				//Fill packet
				buff[0] = HIBYTE(sequence);
				buff[1] = LOBYTE(sequence);
				buff[2] = checkSum;
			}

			if( EnternalLoaderInBinCmd==m_DownloadMode || 
				FileLoaderInBinCmd==m_DownloadMode || 
				EnternalLoader==m_DownloadMode || 
				FileLoader==m_DownloadMode || 
				InternalLoaderV8==m_DownloadMode ||
				InternalLoaderSpecial==m_DownloadMode )
			{
				nBytesSent = SendToTargetNoAck((U08*)buff + headerSize, sentbytes);	 
			}
			else
			{
				nBytesSent = SendToTargetNoAck((U08*)buff, sentbytes + headerSize);	 
			}

			switch(WaitingLoaderFeedback(CGPSDlg::gpsDlg->m_serial, TIME_OUT_MS, notifyWnd))
			{
			case wlf_ok:
				sequence++;
				lSize -= sentbytes;
				bResend = false;
				break;
			case wlf_timeout:
				Utility::LogFatal(__FUNCTION__, "wlf_timeout", __LINE__);
				return RETURN_ERROR;
				break;
			case wlf_resend:
				Utility::Log(__FUNCTION__, "wlf_resend", __LINE__);
				bResend = true;
				break;
			case wlf_reset:
				Utility::Log(__FUNCTION__, "wlf_reset", __LINE__);
				return RETURN_RETRY;
				break;
			default:
				Utility::LogFatal(__FUNCTION__, messages, __LINE__);
				return RETURN_ERROR;
				break;
			}	
//* Alex wait
			if(!bResend)
			{
				downloadProgress += sentbytes;
			}
			m_psoftImgDlDlg->PostMessage(UWM_SETPROGRESS, downloadProgress, downloadTotalSize);

			if(needSleep)
			{
				Sleep(1);
			}
		}	//End of while(lSize>0)
	} //if(sData)
//---------------------------
	bResend = false;
	lSize = (sData) ? fbinSize - BOOTLOADER_SIZE : fbinSize;

	while(lSize>0)
	{ 
		if(!bResend)
		{
			sentbytes = (lSize >= nFlashBytes)?nFlashBytes :lSize;
			TotalByte += sentbytes;	

			memset(buff, 0, buffSize);
			//int realRead = fread(&buff[headerSize], 1, sentbytes, f);
			int realRead = data.Read(&buff[headerSize], sentbytes);

			if(realRead < sentbytes)
			{
				data.Seek(0);
				//realRead = fread(&buff[realRead + headerSize], 1, sentbytes - realRead, f);
				realRead = data.Read(&buff[realRead + headerSize], sentbytes - realRead);
			}
			int checkSum = CalcCheckSum16(&(buff[headerSize]), sentbytes);
			//Fill packet
			buff[0] = HIBYTE(sequence);
			buff[1] = LOBYTE(sequence);
			buff[2] = checkSum;
		}

		if( InternalLoaderV6Gps==m_DownloadMode ||
			InternalLoaderV6Gnss==m_DownloadMode ||
			InternalLoaderV6Gg12a==m_DownloadMode ||
			InternalLoaderV6GpsAddTag==m_DownloadMode ||
			InternalLoaderV6GpsDelTag==m_DownloadMode ||
			InternalLoaderV6GnssAddTag==m_DownloadMode ||
			InternalLoaderV6GnssDelTag==m_DownloadMode )
		{
			nBytesSent = SendToTargetNoAck((U08*)buff, sentbytes + headerSize);	 
		}
		else
		{
			nBytesSent = SendToTargetNoAck((U08*)buff + headerSize, sentbytes);	 
		}

		switch(WaitingLoaderFeedback(CGPSDlg::gpsDlg->m_serial, TIME_OUT_MS, &m_responseList))
		{
		case wlf_ok:
			sequence++;
			lSize -= sentbytes;
			bResend = false;
			break;
		case wlf_timeout:
			Utility::LogFatal(__FUNCTION__, "wlf_timeout", __LINE__);
			return RETURN_ERROR;
			break;
		case wlf_resend:
			Utility::Log(__FUNCTION__, "wlf_resend", __LINE__);
			bResend = true;
			break;
		case wlf_reset:
			Utility::Log(__FUNCTION__, "wlf_reset", __LINE__);
			return RETURN_RETRY;
			break;
		default:
			Utility::LogFatal(__FUNCTION__, messages, __LINE__);
			return RETURN_ERROR;
			break;
		}	
//* Alex wait
		if(!bResend)
		{
			downloadProgress += sentbytes;
		}
		m_psoftImgDlDlg->PostMessage(UWM_SETPROGRESS, downloadProgress, downloadTotalSize);

		if(needSleep)
		{
			Sleep(1);
		}
	}	//End of while(lSize>0)
	delete [] buff;

	WlfResult wlf = WaitingLoaderFeedback(CGPSDlg::gpsDlg->m_serial, 20000, notifyWnd);
	switch(wlf)
	{
	case wlf_end:
		sprintf_s(messages, sizeof(messages), "The total bytes transferred = %d", TotalByte);
		add_msgtolist(messages);
		//msgWnd->SetWindowText(messages);	
		break;
	case wlf_reset:
		AfxMessageBox("Reset!");						
		return RETURN_RETRY;
		break;
	default:
		ProcessWlfResult(wlf);
		Utility::LogFatal(__FUNCTION__, messages, __LINE__);
		return RETURN_ERROR;
		break;
	}	
	return RETURN_NO_ERROR;		
}


//int CGPSDlg::SendRomBuffer3(const U08* sData, int sDataSize, FILE *f, 
//							int fbinSize, bool needSleep, CWnd* notifyWnd)
int CGPSDlg::SendRomBuffer3(const U08* sData, int sDataSize, BinaryData &binData, 
							int fbinSize, bool needSleep, CWnd* notifyWnd)
{
	int nFlashBytes = FlashBytes[m_nDownloadBufferIdx];
	/* sending data */	
	const int headerSize = 3;
	const int buffSize = nFlashBytes + headerSize;
	int realTotalSize = BOOTLOADER_SIZE + fbinSize;
	//U08 buff[buffSize];
	U08* buff = new U08[buffSize];
	char messages[100] = {0};
	bool bResend = false;
	int lSize = 0;
	int sentbytes = 0;
	int TotalByte = 0;
	int nBytesSent = 0;
	U16 sequence = 0;

	if(sData)
	{
		char* prom_ptr = (char*)sData;
		lSize = BOOTLOADER_SIZE;

		while(lSize>0)
		{ 
			if(!bResend)
			{
				sentbytes = nFlashBytes;
				TotalByte += sentbytes;	

				memset(buff, 0, buffSize);
				if(sDataSize > nFlashBytes)
				{
					memcpy(&buff[headerSize], prom_ptr, nFlashBytes);
					sDataSize -= nFlashBytes;
					prom_ptr += nFlashBytes;
				}
				else if(sDataSize > 0)
				{
					memcpy(&buff[headerSize], prom_ptr, sDataSize);
					sDataSize = 0;
				}
				int checkSum = CalcCheckSum16(&(buff[headerSize]), sentbytes);
				//Fill packet
				buff[0] = HIBYTE(sequence);
				buff[1] = LOBYTE(sequence);
				buff[2] = checkSum;
			}

			if( EnternalLoaderInBinCmd==m_DownloadMode || 
				FileLoaderInBinCmd==m_DownloadMode || 
				EnternalLoader==m_DownloadMode || 
				FileLoader==m_DownloadMode || 
				InternalLoaderV8==m_DownloadMode ||
				InternalLoaderSpecial==m_DownloadMode )
			{
				nBytesSent = SendToTargetNoAck((U08*)buff + headerSize, sentbytes);	 
			}
			else
			{
				nBytesSent = SendToTargetNoAck((U08*)buff, sentbytes + headerSize);	 
			}

			switch(WaitingLoaderFeedback(CGPSDlg::gpsDlg->m_serial, TIME_OUT_MS, notifyWnd))
			{
			case wlf_ok:
				sequence++;
				lSize -= sentbytes;
				bResend = false;
				break;
			case wlf_timeout:
				Utility::LogFatal(__FUNCTION__, "wlf_timeout", __LINE__);
				return RETURN_ERROR;
				break;
			case wlf_resend:
				Utility::Log(__FUNCTION__, "wlf_resend", __LINE__);
				bResend = true;
				break;
			case wlf_reset:
				Utility::Log(__FUNCTION__, "wlf_reset", __LINE__);
				return RETURN_RETRY;
				break;
			default:
				Utility::LogFatal(__FUNCTION__, messages, __LINE__);
				return RETURN_ERROR;
				break;
			}	
//* Alex wait
			if(!bResend)
			{
				downloadProgress += sentbytes;
			}
			m_psoftImgDlDlg->PostMessage(UWM_SETPROGRESS, downloadProgress, downloadTotalSize);

			if(needSleep)
			{
				Sleep(1);
			}
		}	//End of while(lSize>0)
	} //if(sData)
//---------------------------
	bResend = false;
	lSize = (sData) ? fbinSize - BOOTLOADER_SIZE : fbinSize;

	while(lSize>0)
	{ 
		if(!bResend)
		{
			sentbytes = (lSize >= nFlashBytes)?nFlashBytes :lSize;
			TotalByte += sentbytes;	

			memset(buff, 0, buffSize);
			int realRead = binData.Read(&buff[headerSize], sentbytes);
			if(realRead<sentbytes)
			{
				binData.Seek(0);
				realRead = (int)binData.Read(&buff[realRead + headerSize], sentbytes - realRead);
			}
			int checkSum = CalcCheckSum16(&(buff[headerSize]), sentbytes);
			//Fill packet
			buff[0] = HIBYTE(sequence);
			buff[1] = LOBYTE(sequence);
			buff[2] = checkSum;
		}

		if( InternalLoaderV6Gps==m_DownloadMode ||
			InternalLoaderV6Gnss==m_DownloadMode ||
			InternalLoaderV6Gg12a==m_DownloadMode ||
			InternalLoaderV6GpsAddTag==m_DownloadMode ||
			InternalLoaderV6GpsDelTag==m_DownloadMode ||
			InternalLoaderV6GnssAddTag==m_DownloadMode ||
			InternalLoaderV6GnssDelTag==m_DownloadMode )
		{
			nBytesSent = SendToTargetNoAck((U08*)buff, sentbytes + headerSize);	 
		}
		else
		{
			nBytesSent = SendToTargetNoAck((U08*)buff + headerSize, sentbytes);	 
		}

		switch(WaitingLoaderFeedback(CGPSDlg::gpsDlg->m_serial, TIME_OUT_MS, &m_responseList))
		{
		case wlf_ok:
			sequence++;
			lSize -= sentbytes;
			bResend = false;
			break;
		case wlf_timeout:
			Utility::LogFatal(__FUNCTION__, "wlf_timeout", __LINE__);
			return RETURN_ERROR;
			break;
		case wlf_resend:
			Utility::Log(__FUNCTION__, "wlf_resend", __LINE__);
			bResend = true;
			break;
		case wlf_reset:
			Utility::Log(__FUNCTION__, "wlf_reset", __LINE__);
			return RETURN_RETRY;
			break;
		default:
			Utility::LogFatal(__FUNCTION__, messages, __LINE__);
			return RETURN_ERROR;
			break;
		}	
//* Alex wait
		if(!bResend)
		{
			downloadProgress += sentbytes;
		}
		m_psoftImgDlDlg->PostMessage(UWM_SETPROGRESS, downloadProgress, downloadTotalSize);

		if(needSleep)
		{
			Sleep(1);
		}
	}	//End of while(lSize>0)
	delete [] buff;

	WlfResult wlf = WaitingLoaderFeedback(CGPSDlg::gpsDlg->m_serial, 20000, notifyWnd);
	switch(wlf)
	{
	case wlf_end:
		sprintf_s(messages, sizeof(messages), "The total bytes transferred = %d", TotalByte);
		add_msgtolist(messages);
		//msgWnd->SetWindowText(messages);	
		break;
	case wlf_reset:
		AfxMessageBox("Reset!");						
		return RETURN_RETRY;
		break;
	default:
		ProcessWlfResult(wlf);
		Utility::LogFatal(__FUNCTION__, messages, __LINE__);
		return RETURN_ERROR;
		break;
	}	
	return RETURN_NO_ERROR;		
}

U08 GetPromBinCheckSum(CFile& f, int size)
{
	U08 c, cs = 0;
	for(int i=0; i<size; ++i)
	{
		f.Read(&c, 1);
		cs += c;
	}
	return cs;
}

U08 GetPromBinCheckSum(BinaryData& b, int size)
{
	U08 c, cs = 0;
	for(int i=0; i<size; ++i)
	{
		b.Read(&c, 1);
		cs += c;
	}
	return cs;
}

U08 GetPromBinCheckSum(const U08* b, int size)
{
	U08 cs = 0;
	for(int i=0; i<size; ++i)
	{
		cs += *b;
		b++;
	}
	return cs;
}

U08 CGPSDlg::PlRomNoAlloc2(const CString& prom_path)
{
	
	Param promTag;
	int extraSize = ParserBinFile(prom_path, &promTag);
	char messages[100] = {0};

	BinaryData binFile(prom_path);
	//CFile f;
	//FILE *f = NULL;

	if(binFile.Size() == 0)
	//if(0 == f.Open(prom_path, CFile::modeRead))
	//if(0 != fopen_s(&f, prom_path, "rb"))
	{
		return RETURN_ERROR;
	}

	binFile.Seek(0);
	//fseek(f, 0, SEEK_END);

	DWORD promLen = (DWORD)binFile.Size() - extraSize;
	//long promLen = ftell(f) - extraSize;
	if(promTag.tagAddress==0)
	{
		promTag.tagAddress = (promLen>=0x70000) ? 0x000f7ffc : 0x00077ffc;
	}
	//Get preloader bin and it's size.
	CString externalPreloaderPath;
	BinaryData preLoader;
	int bootLoaderSize = 0;
	if(theApp.CheckExternalPreloader(externalPreloaderPath))
	{
		bootLoaderSize = preLoader.ReadFromFile(externalPreloaderPath);
	}
	else
	{
		bootLoaderSize = preLoader.ReadFromResource(GetBinFromResource(g_setting.GetBaudrateIndex()), "BIN");
	}

	const U08* sData = preLoader.Ptr();
	U08 mycheck = GetPromBinCheckSum(sData, preLoader.Size());
	//U08 mycheck = 0;
	//for(int i=0; i<preLoader.Size(); ++i)
	//{
	//	mycheck += sData[i];
	//}

	binFile.Seek(BOOTLOADER_SIZE);
	mycheck += GetPromBinCheckSum(binFile, promLen-BOOTLOADER_SIZE);
	//fseek(f, BOOTLOADER_SIZE, SEEK_SET);
	//char c;
	//for(int i=0; i<promLen-BOOTLOADER_SIZE; ++i)
	//{
	//	fread(&c, 1, 1, f);
	//	mycheck = mycheck + c;
	//}

	bool bResendbin = true;
	while(bResendbin)
	{
		unsigned long long check = promLen + mycheck + promTag.tagAddress;
		sprintf_s(messages, sizeof(messages), "BINSIZE = %d Checksum = %d %lld %lld ", promLen, mycheck, promTag.tagAddress, check);
		m_serial->SendData((U08*)messages, (U16)strlen(messages) + 1);	
		Utility::Log(__FUNCTION__, messages, __LINE__);

		WlfResult wlf = WaitingLoaderFeedback(CGPSDlg::gpsDlg->m_serial, BinSizeTimeout, &m_responseList);
		switch(wlf)
		{
		case wlf_ok:
			bResendbin = false;
			break;
		case wlf_resendbin:
			Utility::Log(__FUNCTION__, messages, __LINE__);
			bResendbin = true;
			break;
		default:
			ProcessWlfResult(wlf);
			Utility::LogFatal(__FUNCTION__, messages, __LINE__);
			//f.Close();
			//fclose(f);
			return RETURN_ERROR;		        
			break;
		}	//switch(WaitingLoaderFeedback(CGPSDlg::gpsDlg->m_serial, TIME_OUT_MS, &m_responseList))	
	}	//while(bResendbin)

	downloadTotalSize = BOOTLOADER_SIZE + promLen;
	downloadProgress = 0;
	m_psoftImgDlDlg->PostMessage(UWM_SETPROMPT_MSG, IDS_WAITROM_MSG);
	
	int result = RETURN_NO_ERROR;
	int retryCount = 3;
	do
	{
		binFile.Seek(BOOTLOADER_SIZE);
		result = SendRomBuffer3(sData, bootLoaderSize, binFile, promLen, false, m_psoftImgDlDlg);
		//fseek(f, BOOTLOADER_SIZE, SEEK_SET);
		//result = SendRomBuffer3(sData, bootLoaderSize, f, promLen, false, m_psoftImgDlDlg);
	} while(result == RETURN_RETRY && (retryCount--) > 0);

//---------------------------------------------------------------
	promLen = BOOTLOADER_SIZE;

	binFile.Seek(0);
	mycheck = GetPromBinCheckSum(binFile, BOOTLOADER_SIZE);
	//fseek(f, 0, SEEK_SET);
	//for(int i=0; i<BOOTLOADER_SIZE; ++i)
	//{
	//	fread(&c, 1, 1, f);
	//	mycheck = mycheck + c;
	//}

	bResendbin = true;
	while(bResendbin)
	{
		unsigned long long check = promLen + mycheck + promTag.tagAddress;
		sprintf_s(messages, sizeof(messages), "BINSIZE = %d Checksum = %d %lld %lld ", promLen, mycheck, promTag.tagAddress, check);
		m_serial->SendData((U08*)messages, (U16)strlen(messages)+1);	
		Utility::Log(__FUNCTION__, messages, __LINE__);

		WlfResult wlf = WaitingLoaderFeedback(CGPSDlg::gpsDlg->m_serial, BinSizeTimeout, &m_responseList);
		switch(wlf)
		{
		case wlf_ok:
			bResendbin = false;
			break;
		case wlf_resendbin:
			Utility::Log(__FUNCTION__, messages, __LINE__);
			bResendbin = true;
			break;
		default:
			ProcessWlfResult(wlf);
			Utility::LogFatal(__FUNCTION__, messages, __LINE__);
			//f.Close();
			//fclose(f);
			return RETURN_ERROR;		        
			break;
		}	//switch(WaitingLoaderFeedback(CGPSDlg::gpsDlg->m_serial, TIME_OUT_MS, &m_responseList))	
	}	//while(bResendbin)
// Alex wait
	m_psoftImgDlDlg->PostMessage(UWM_SETPROMPT_MSG, IDS_WAITROM_MSG);
//	sprintf_s(messages, sizeof(messages), "Waiting for Rom Writing...");
//	m_psoftImgDlDlg->m_msg.SetWindowText(messages);	
	
	result = RETURN_NO_ERROR;
	retryCount = 3;
	do
	{
		binFile.Seek(0);
		//fseek(f, 0, SEEK_SET);
		result = SendRomBuffer3(NULL, bootLoaderSize, binFile, promLen, false, m_psoftImgDlDlg);
	} while(result == RETURN_RETRY && (retryCount--) > 0);
	//f.Close();
	//fclose(f);
	BoostBaudrate(TRUE);
	m_psoftImgDlDlg->PostMessage(UWM_SETPROMPT_MSG, IDS_ROMWRITINGOK_MSG);

	return RETURN_NO_ERROR;		
}

U08 CGPSDlg::PlRomNoAllocV8(const CString& prom_path)
{
	Param promTag;
	int extraSize = ParserBinFile(prom_path, &promTag);
	char messages[100] = {0};

	//CFile f;
	BinaryData binFile(prom_path);
	//if(0 == f.Open(prom_path, CFile::modeRead))
	if(binFile.Size() == 0)
	{
		::AfxMessageBox("PROM bin file not found!");
		return RETURN_ERROR;
	}

	//DWORD promLen = (DWORD)f.GetLength();
	DWORD promLen = (DWORD)binFile.Size();

	//f.SeekToBegin();
	binFile.Seek(0);
	//U08 mycheck = GetPromBinCheckSum(f, promLen);
	U08 mycheck = GetPromBinCheckSum(binFile, promLen);

#ifdef TEST_SREC
	::AfxMessageBox("Press OK to start download.");
#endif
	//Old FW will return "END" itself after loader upload, but new FW will return "END" via loader. 
	//Therefore, using the old FW with the new loader, it will receive "END" twice.
	bool bResendbin = true;
	while(bResendbin)
	{
		U32 checkCode = 0;
		CString strBinsizeCmd;
		if(m_DownloadMode == InternalLoaderV8AddTag || downloadAddTag)
		{
      if(m_DownloadMode == InternalLoaderV8AddTag)
      {
			  tagAddress = 0xAAA56556;	//V8 tag address is 0xFCFFC ^ 0xAAAAAAAA
			  tagValue = 0x5555AAAA;	//default tag is 0xFFFF ^ 0x55555555 (No Tag)
      }
			//U32 tagValue = 0x55555A54;	//Skytraq DR tag is 0x0F01 ^ 0x55555555
			if(m_customerId==OlinkStar)
			{
				tagValue = 0x55555E54;	//OLinkStar tag is 0x0B01 ^ 0x55555555
			}
			else if(m_DownloadMode == InternalLoaderV8AddTag)
			{
				tagValue = 0x55555F54;	//Skytraq tag is 0x0A01 ^ 0x55555555
			}
			checkCode = promLen + mycheck + m_nDownloadBaudIdx + tagAddress + tagValue;
			strBinsizeCmd.Format("BINSIZ2 = %d %d %u %u %u %u ", promLen, mycheck, m_nDownloadBaudIdx,
									tagAddress, tagValue, checkCode);
      downloadAddTag = FALSE;
      tagAddress = 0;
      tagValue = 0;
		}
		else
		{
			checkCode = promLen + mycheck;
			strBinsizeCmd.Format("BINSIZE = %d Checksum = %d %u ", promLen, mycheck, checkCode);
		}

		m_serial->SendData((LPCSTR)strBinsizeCmd, strBinsizeCmd.GetLength() + 1);	
		Utility::Log(__FUNCTION__, messages, __LINE__);
		bool isEnd = false;
		do 
		{
			//For Andrew Test EON Flash, wait for 1 minute.
			//switch(WaitingLoaderFeedback(CGPSDlg::gpsDlg->m_serial, 60 * 1000, &m_responseList))
			WlfResult wlf = WaitingLoaderFeedback(CGPSDlg::gpsDlg->m_serial, BinSizeTimeout, &m_responseList);
			switch(wlf)
			{
			case wlf_end:
				isEnd = true;
				break;
			case wlf_ok:
				bResendbin = false;
				isEnd = false;
				break;
			case wlf_resendbin:
				Utility::Log(__FUNCTION__, messages, __LINE__);
				bResendbin = true;
				isEnd = false;
				break;
			default:
				ProcessWlfResult(wlf);
				Utility::LogFatal(__FUNCTION__, messages, __LINE__);
				//f.Close();
				//fclose(f);
				return RETURN_ERROR;		        
				break;
			}	//switch(WaitingLoaderFeedback(CGPSDlg::gpsDlg->m_serial, TIME_OUT_MS, &m_responseList))	
		} while(isEnd);
	}	//while(bResendbin)
	downloadTotalSize = promLen;
	downloadProgress = 0;
	m_psoftImgDlDlg->PostMessage(UWM_SETPROMPT_MSG, IDS_WAITROM_MSG);
	
	int result = RETURN_NO_ERROR;
	int retryCount = 1;
	do
	{
		//f.SeekToBegin();
		binFile.Seek(0);
		Sleep(500);	//for GPSDO
		result = SendRomBuffer3(NULL, 0, binFile, promLen, false, m_psoftImgDlDlg);
	} while(result == RETURN_RETRY && (retryCount--) > 0);

	//f.Close();
	m_psoftImgDlDlg->PostMessage(UWM_SETPROMPT_MSG, IDS_ROMWRITINGOK_MSG);
	return result;		
}

U08 CGPSDlg::PlRomCustomerUpgrade(UINT rid)
{
	BinaryData prom(rid, "BIN");
	long promLen = prom.Size();

	U08 c;
	U08 mycheck = 0;
	for(int i=0; i<promLen; ++i)
	{
		c = *prom.GetBuffer(i);
		mycheck = mycheck + (c & 0xff);
	}

	//Old FW will return "END" itself after loader upload, but new FW will return "END" via loader. 
	//Therefore, using the old FW with the new loader, it will receive "END" twice.
	bool bResendbin = true;
	while(bResendbin)
	{
		U32 checkCode = 0;
		CString strBinsizeCmd;
		if(UPGRADE_ADD_TAG)
		{
			U32 tagAddress = 0xAAA56556;	//V8 tag address is 0xFCFFC ^ 0xAAAAAAAA
			U32 tagValue = UPGRADE_TAG_VALUE ^ 0x55555555;	//Skytraq tag is 0x0A01 ^ 0x55555555
			checkCode = promLen + mycheck + m_nDownloadBaudIdx + tagAddress + tagValue;
			strBinsizeCmd.Format("BINSIZ2 = %d %d %u %u %u %u ", promLen, mycheck, m_nDownloadBaudIdx,
									tagAddress, tagValue, checkCode);
		}
		else
		{
			checkCode = promLen + mycheck;
			strBinsizeCmd.Format("BINSIZE = %d Checksum = %d %u ", promLen, mycheck, checkCode);
		}

		m_serial->SendData((LPCSTR)strBinsizeCmd, strBinsizeCmd.GetLength() + 1);	
		bool isEnd = false;
		do 
		{
			//For Andrew Test EON Flash, wait for 1 minute.
			//switch(WaitingLoaderFeedback(CGPSDlg::gpsDlg->m_serial, 60 * 1000, &m_responseList))
			WlfResult wlf = WaitingLoaderFeedback(CGPSDlg::gpsDlg->m_serial, BinSizeTimeout, &m_responseList);
			switch(wlf)
			{
			case wlf_end:
				isEnd = true;
				break;
			case wlf_ok:
				bResendbin = false;
				isEnd = false;
				break;
			case wlf_resendbin:
				//Utility::Log(__FUNCTION__, messages, __LINE__);
				bResendbin = true;
				isEnd = false;
				break;
			default:
				ProcessWlfResult(wlf);
				//Utility::LogFatal(__FUNCTION__, messages, __LINE__);
				//fclose(f);
				return RETURN_ERROR;		        
				break;
			}	//switch(WaitingLoaderFeedback(CGPSDlg::gpsDlg->m_serial, TIME_OUT_MS, &m_responseList))	
		} while(isEnd);
	}	//while(bResendbin)

	downloadTotalSize = promLen;
	downloadProgress = 0;
	m_psoftImgDlDlg->PostMessage(UWM_SETPROMPT_MSG, IDS_WAITROM_MSG);
	
	int result = RETURN_NO_ERROR;
	int retryCount = 1;
	do
	{
		//fseek(f, 0, SEEK_SET);
		Sleep(500);	//for GPSDO
		result = SendRomBufferCustomerUpgrade(NULL, 0, prom, promLen, false, m_psoftImgDlDlg);
	} while(result == RETURN_RETRY && (retryCount--) > 0);

	//fclose(f);
	m_psoftImgDlDlg->PostMessage(UWM_SETPROMPT_MSG, IDS_ROMWRITINGOK_MSG);
	return result;		
}

void CGPSDlg::GetLoaderDownloadCmd(char* msg, int size)
{
	if(customerCrc==0x00A0  || customerCrc==0x0724 || customerCrc==0x6469)
	{
		sprintf_s(msg, size, "$OLINKSTAR IMAGE");		
	}
	else
	{
		sprintf_s(msg, size, "$LOADER DOWNLOAD");		
	}
}

bool CGPSDlg::DownloadLoader()
{
	ScopeTimer t("DownloadLoader()");
	const int bufferSize = 256;
	char messages[100] = {0};
	//U08 dummy[9] = {0xa0, 0xa1, 0x00, 0x02, 0x00, 0x00, 0x01, 0x0d, 0x0a};

	if(m_DownloadMode != EnternalLoaderInBinCmd && m_DownloadMode != FileLoaderInBinCmd)
	{
		GetLoaderDownloadCmd(messages, sizeof(messages));
		
		const int retryCount = 3;
		for (int i=1; i<retryCount; i++)
		{
			//SendToTargetNoAck((U08*)dummy, 9);
			SendToTargetNoAck((U08*)messages, (U16)strlen(messages) + 1);
			Sleep(300);

			switch(WaitingLoaderFeedback(CGPSDlg::gpsDlg->m_serial, TIME_OUT_MS, &m_responseList))
			{
			case wlf_timeout:
				AfxMessageBox("Target doesn't reply, please power cycle the target!");				
				return false;
				break;
			case wlf_ok:
				i = retryCount;
				break;
			default:
				Utility::LogFatal(__FUNCTION__, messages, __LINE__);
				break;
			}	
		}
	}
	else
	{
		U08 msg[7] = {0};
		msg[0] = 0x64;
		msg[1] = 0x1B;
		msg[2] = (U08)m_nDownloadBaudIdx;
		msg[3] = (U08)0;
		msg[4] = (U08)0;
		msg[5] = (U08)0;
		msg[6] = (U08)m_nDownloadBufferIdx;
		int len = SetMessage(msg, sizeof(msg));

		bool b = SendToTarget(m_inputMsg, len, "Send upload loader successfully", 6000);
		//bool b = false;
		if(!b)
		{
			m_DownloadMode = EnternalLoader;
			BoostBaudrate(FALSE);
			//add_msgtolist("Upload Loader command NACK.");	
			return DownloadLoader();
		}
		else
		{
			CloseOpenUart();
			m_serial->ResetPort(m_nDownloadBaudIdx);
			m_BaudRateCombo.SetCurSel(m_nDownloadBaudIdx);
			Sleep(200);
		}
	}

// Alex wait
	m_psoftImgDlDlg->PostMessage(UWM_SETPROMPT_MSG, IDS_FLASHWRITINGOK_MSG);
//	sprintf_s(messages, sizeof(messages), "Waiting for Flash Loading...\r\n");
//	m_psoftImgDlDlg->m_msg.SetWindowText(messages);	
	
	BinaryData srec;
	CString externalSrecFile;
	theApp.CheckExternalSrec(externalSrecFile);

	if(EnternalLoaderInBinCmd == m_DownloadMode || 
     FileLoader == m_DownloadMode || FileLoaderInBinCmd == m_DownloadMode)
	{
    if(externalSrecFile.IsEmpty())
    {
      srec.ReadFromResource(IDR_V8_AT_SREC, "SREC");
    }
    else
    {
		  srec.ReadFromFile(externalSrecFile);
      //if(FileLoader == m_DownloadMode)
      //{
      //  m_DownloadMode = EnternalLoader;
      //}
      //else if(FileLoaderInBinCmd == m_DownloadMode)
      //{
      //  m_DownloadMode = FileLoader;
      //}
    }
	}
	else
	{
		UINT loaderID = GetSrecFromResource(m_nDownloadBaudIdx);
		if(!loaderID)
		{
			AfxMessageBox("Can not find the specified Loader!");
			return false;
		}
		srec.ReadFromResource(loaderID, "SREC");
	}

	const U08* sData = srec.Ptr();
	long leftSize = srec.Size();
	int needdelay = 0;
	int totalByte = 0;
	char buff[bufferSize] = {0};

	while(leftSize>0)
	{
		int ProgressPos = 0;
		int packetSize = 0;
		const U08 *tmp = sData;

		memset(buff, 0, sizeof(buff));
		while(1)
		{
			++packetSize;
			if(*tmp == '\n')
			{
				break;
			}
			++tmp;
		}
		memcpy(buff, sData, packetSize - 1);
		buff[packetSize - 2] = 0x0a;
		SendToTargetNoAck((U08*)buff, packetSize);
	
		leftSize -= packetSize;
		sData += packetSize;
		totalByte += packetSize;//deduct by end of string character in sending
//* Alex wait
		m_psoftImgDlDlg->PostMessage(UWM_SETPROGRESS, totalByte, srec.Size());
	}			

	memset(buff, 0, sizeof(buff));
	switch(WaitingLoaderFeedback(CGPSDlg::gpsDlg->m_serial, TIME_OUT_MS, &m_responseList))
	{
	case wlf_end:
		sprintf_s(messages, sizeof(messages), "The total bytes transferred = %d", totalByte);
		add_msgtolist(messages);	
		break;
	case wlf_timeout:
		Utility::LogFatal(__FUNCTION__, "wlf_timeout", __LINE__);
		return false;
		break;
	default:
		Utility::LogFatal(__FUNCTION__, messages, __LINE__);
		break;
	}	
	m_psoftImgDlDlg->PostMessage(UWM_SETPROGRESS, 100, 100);


// Alex wait
//	sprintf_s(messages, sizeof(messages), "The total bytes transferred = %d", totalByte);
//	m_psoftImgDlDlg->m_msg.SetWindowText(messages);	
//	m_psoftImgDlDlg->m_msg.SetWindowText("Flash loading is OK!");
	m_psoftImgDlDlg->PostMessage(UWM_SETPROMPT_MSG, IDS_FLASHWRITINGOK_MSG);

	switch(m_DownloadMode)
	{
		case ParallelDownloadType0:
		case ParallelDownloadType1:
		case InternalLoaderV6Gps:
		case InternalLoaderV6Gnss:
		case InternalLoaderV6Gg12a:
		case InternalLoaderV6GpsAddTag:
		case InternalLoaderV6GpsDelTag:
		case InternalLoaderV6GnssAddTag:
		case InternalLoaderV6GnssDelTag:
		case HostBasedDownload:
		case HostBasedCmdOnly:
			Sleep(1000);
			break;
		case EnternalLoader:
		case FileLoader:
		case EnternalLoaderInBinCmd:
		case FileLoaderInBinCmd:
		case InternalLoaderV8:
		case InternalLoaderV8AddTag:
		case RomExternalDownload:
		case InternalLoaderSpecial:
			break;
		default:
			ASSERT(FALSE);
	}

	return true;
}

BOOL VarifyPassword(const CString& pwd)
{
	if(!_CREATE_LICENSE_TAG_)
	{
		return true;
	}

	const char ConvertTabe[] = "0123456789abcdfghijkmnpqrstuvwyz";
	CString key, binPart, answer;
	CTime currentTime = CTime::GetCurrentTime();
	key.Format("SkytraqTagGPSViewer%d%d", currentTime.GetYear() - 2000, (currentTime.GetMonth() - 1) / 3);
	CString shaData = CryptHash::HashStringSHA1(key);

	for(int i = shaData.GetLength()-1; i>0; i-=3)
	{
		binPart = shaData[i - 1];
		binPart += shaData[i];
		answer += ConvertTabe[strtol(binPart, NULL, 16) % strlen(ConvertTabe)];
	}
	return 0 == answer.CompareNoCase(pwd);
}

bool CGPSDlg::QueryPassword()
{
	if(!_CREATE_LICENSE_TAG_)
	{
		return true;
	}
#ifdef _DEBUG
		return true;
#endif
	CInputPassword dlg;
	CString pwInput;
	INT_PTR nResponse = dlg.DoModal();
	if(nResponse==IDCANCEL)
	{
		return false;
	}

	if(!VarifyPassword(dlg.GetPassword()))
	{
		return false;
	}
	return true;
}

UINT ShowDownloadProgressThread(LPVOID pParam)
{
	CGPSDlg::gpsDlg->m_psoftImgDlDlg->DoModal();
	delete CGPSDlg::gpsDlg->m_psoftImgDlDlg;
	CGPSDlg::gpsDlg->m_psoftImgDlDlg = NULL;
	return 0;	
}

const int SpiBaudIndex = 9;
bool SecondRun = false;
bool CGPSDlg::Download()
{
	if(m_DownloadMode == CustomerUpgrade)
	{
		return Download2();
	}

	if(m_DownloadMode == GpsdoMasterSlave)
	{
		return Download3();
	}

	//Create Tag version need authenticate.
	if(!QueryPassword())
	{	//Password authentication failed!
		SetMode();
		CreateGPSThread();
		m_gpsdoInProgress = false;
		return false;
	}

	bool isSuccessful = true;
	do 
	{	//Download test loop
		customerCrc = 0;
		if((_CREATE_LICENSE_TAG_ && m_DownloadMode==InternalLoaderV8) || (m_DownloadMode == EnternalLoader))
		{
			m_DownloadMode = InternalLoaderV8AddTag;
		}

		U16 crcCode = 0;
		BOOL hasAckVersion = FALSE;
		m_nDefaultTimeout = 5000;
		if(m_DownloadMode != FileLoader && 
       !DOWNLOAD_IMMEDIATELY && 
       Ack == QuerySoftwareCrcSystemCode(Return, &crcCode))
		{
			hasAckVersion = TRUE;
		}
		m_nDefaultTimeout = g_setting.defaultTimeout;

		if(m_DownloadMode != EnternalLoader && CUSTOMER_DOWNLOAD && m_customerId==OlinkStar)
		{
			if(hasAckVersion && (crcCode==0xD3CF || crcCode==0x00A0  || crcCode==0x0724 || crcCode==0x6469))
			{	//The firmware which released in 2013/10/24, 2013/11/11, 2013/11/08, 2013/11/07 
				//for OLinkStar should be added tag.
				m_DownloadMode = InternalLoaderV8AddTag;
				customerCrc = crcCode;
			}
		}
		else if(m_DownloadMode == InternalLoaderV8 && (crcCode == 0xe463 /*|| crcCode == 0x74cf*/))
		{	
      if(!g_setting.downloadRomInternal)
      { //V8 ROM Code A must use external loader.
			  m_DownloadMode = RomExternalDownload;
      }
		}

		if(!DOWNLOAD_IMMEDIATELY)
		{
			Sleep(100);
		}

		if(m_DownloadMode==GpsdoMasterSlave && SecondRun)
		{
			if(crcCode==0xe463)
			{
				::AfxMessageBox("Please change BOOT_SEL to flash, and push reset.");
			}
			
			CmdErrorCode err = GpsdoEnterDownload(Return, NULL);
			m_nDownloadBaudIdx = 5;
			SetBaudrate(m_nDownloadBaudIdx);		//115200 bps
			Sleep(1000);

			if(Ack != err)
			{
				::AfxMessageBox("Can't download GPSDO Slave!");
				break;
			}
		}

		ScopeTimer t("Download()");
	
		switch(m_DownloadMode)
		{
			case EnternalLoader:
			case FileLoader:
			case InternalLoaderV6Gps:
			case InternalLoaderV6Gnss:
			case InternalLoaderV6Gg12a:
			case InternalLoaderV6GpsAddTag:
			case InternalLoaderV6GpsDelTag:
			case InternalLoaderV6GnssAddTag:
			case InternalLoaderV6GnssDelTag:
			case ParallelDownloadType0:
			case ParallelDownloadType1:
			case InternalLoaderV8AddTag:
			case RomExternalDownload:
			case InternalLoaderSpecial:
				BoostBaudrate(FALSE);
			case FileLoaderInBinCmd:
			case EnternalLoaderInBinCmd:
			{
				//1. Boost baud rate 2. Download Loader 
				m_psoftImgDlDlg = new CSoftImDwDlg;	
				AfxBeginThread(ShowDownloadProgressThread, 0); 
				if(!DownloadLoader())
				{
					add_msgtolist("Software image download failed...");	 
					::AfxMessageBox("Load loader into RAM failed!");
					m_psoftImgDlDlg->SetFinish(true);	
					BoostBaudrate(TRUE);
					SetMode();
					CreateGPSThread();
					m_gpsdoInProgress = false;
					return false;
				}
				break;
			}
			case HostBasedDownload:
			case HostBasedCmdOnly:
			case InternalLoaderV8:
			case CustomerDownload:
			case GpsdoMasterSlave:
			{	//Using command to initial loader.
				U08 msg[9] = {0};
				int cmdSize = 0;	
				if(m_DownloadMode==InternalLoaderV8 || m_DownloadMode==GpsdoMasterSlave)
				{
					msg[0] = 0x0B;
					msg[1] = (U08)m_nDownloadBaudIdx;
					msg[2] = (U08)0;
					msg[3] = (U08)0;
					msg[4] = (U08)0;
					msg[5] = (U08)m_nDownloadBufferIdx;
					cmdSize = 6;
				}
				else if(m_DownloadMode==CustomerDownload)
				{
					msg[0] = 0x64;
					msg[1] = 0x7F;
					msg[2] = HIBYTE(m_customerId);
					msg[3] = LOBYTE(m_customerId);
					msg[4] = 0;
					msg[5] = (U08)m_nDownloadBaudIdx;
					msg[6] = 0;
					msg[7] = 0;
					msg[8] = 0;
					cmdSize = 9;

					m_DownloadMode = InternalLoaderV8;
				}
				else
				{
					msg[0] = 0x64;
					msg[1] = 0x10;
					msg[2] = (U08)m_nDownloadBaudIdx;
					msg[3] = (U08)m_nDownloadBufferIdx;
					cmdSize = 4;
				}

				int len = SetMessage(msg, cmdSize);
				if(!SendToTarget(CGPSDlg::m_inputMsg, len, "Loader start...", true))
				{
					isSuccessful = false;
				}
				else if(m_nDownloadBaudIdx < SpiBaudIndex)
				{
					CloseOpenUart();
					m_serial->ResetPort(m_nDownloadBaudIdx);
					m_BaudRateCombo.SetCurSel(m_nDownloadBaudIdx);
					Sleep(200);
				}
				break;
			}
			default:
				ASSERT(FALSE);
		}

		if(m_DownloadMode==HostBasedCmdOnly)
		{
			m_gpsdoInProgress = false;
			return false;
		} 

		if(NULL == m_serial) return false;
		if(isSuccessful)
		{
			if(NULL==m_psoftImgDlDlg)
			{
				m_psoftImgDlDlg = new CSoftImDwDlg;	
				AfxBeginThread(ShowDownloadProgressThread, 0); 
			}
			if(g_setting.delayBeforeBinsize)
			{
				Sleep(g_setting.delayBeforeBinsize);
			}

			isSuccessful = FirmwareUpdate((m_DownloadMode==GpsdoMasterSlave && SecondRun) ? m_strDownloadImage2 : m_strDownloadImage);
			if(isSuccessful)
			{
				CString strMsg;
				strMsg.Format("Download total time %d seconds.", t.GetDuration() / 1000);
				add_msgtolist(strMsg);
				DeleteNmeaMemery();
				m_firstDataIn = false;
				m_inputMode = NmeaMessageMode;	
				SetMsgType(NmeaMessageMode);
			}
			else
			{
				add_msgtolist("Software Image Download Failed...");
				::AfxMessageBox("Download failed!");
			}
		}
		else
		{
			add_msgtolist("Software Image Download Failed...");
			::AfxMessageBox("Download failed!");
		}

		if(m_psoftImgDlDlg)
		{
			m_psoftImgDlDlg->SetFinish(true);	
		}

		if(NULL == m_serial) return false;
		BoostBaudrate(TRUE);
		if(g_setting.downloadTesting)
		{
			Sleep(3000);
		}

		if(GpsdoMasterSlave == m_DownloadMode)
		{
			SecondRun = (SecondRun) ? false : true;
			if(SecondRun)
			{
				Sleep(1000);
				continue;
			}
			else
			{
				GpsdoLeaveDownload(Return, NULL);
				if(g_setting.downloadTesting)
				{
					Sleep(1000);
				}
			}
		}
		if(!g_setting.downloadTesting)
		{
			break;
		}
	} while(1);

	if(m_gpsdoInProgress)
	{
		return isSuccessful;
	}

	SetMode();
	m_firstDataIn = false;
	ClearInformation();
	CreateGPSThread();
	return isSuccessful;
}

CString patchLog;
U32 patchData = 0;
U32 patchStatus = 0;
bool CGPSDlg::CheckTagType()
{
	U32 t = m_regAddress;
	U32 data = 0;
	m_regAddress = 0xFCFFC;
	bool suc = false;

	CmdErrorCode ack = QueryRegister(Return, &data);
	m_regAddress = t;

	if( (data >> 16) == 0x901)
	{
		suc = true;
	}
	else
	{
		patchStatus += 0x04;
	}
	patchData = (data >> 16) ^ 0x55555555;

	CString str;
	if(ack != Ack) 
	{
		suc = false;
		patchStatus += 0x02;
	}
	return suc;
}

bool CGPSDlg::Download2()
{
	patchLog = "";
	patchData = 0;
	patchStatus = 0;
	if(UPGRADE_DUEDATE_Y > 0 && UPGRADE_DUEDATE_M > 0 && UPGRADE_DUEDATE_D)
	{
		CTime current = CTime::GetCurrentTime();
		CTime dueDate(UPGRADE_DUEDATE_Y, UPGRADE_DUEDATE_M, UPGRADE_DUEDATE_D, 23, 59, 59);
		if(current > dueDate)
		{
			patchStatus += 0x01;
			::AfxMessageBox("This tool has expired!");
			return false;
		}
	}
	customerCrc = 0;

  if(UPGRADE_CRC != 0xFFFFFFFF)
  {
	  U16 crcCode = 0;
	  BOOL hasAckVersion = FALSE;
	  m_nDefaultTimeout = 5000;
	  if(Ack == QuerySoftwareCrcSystemCode(Return, &crcCode))
	  {
		  hasAckVersion = TRUE;
	  }
	  m_nDefaultTimeout = g_setting.defaultTimeout;

	  if(UPGRADE_CRC!=0xFFFFFFFF && (!hasAckVersion || (crcCode!=UPGRADE_CRC && crcCode!=0xb02c) ) )
	  {
		  ::AfxMessageBox("Can't support upgrade!");
		  return false;
	  }
  }

	bool checkOK = false;
	if(SHOW_PATCH_MENU)
	{
		checkOK = CheckTagType();
	}

	if(!checkOK)
	{
		::AfxMessageBox("This device doesn't support patch!");
		patchLog.Format("PatchResult: %08X%08X", patchData, patchStatus);
		add_msgtolist(patchLog);
		AfxMessageBox(patchLog);
		return false;
	}
	Sleep(100);

	if(NULL==m_psoftImgDlDlg)
	{
		m_psoftImgDlDlg = new CSoftImDwDlg;	
		AfxBeginThread(ShowDownloadProgressThread, 0); 
	}

	bool isSuccessful = true;
	ScopeTimer t("Download()");
	if(UPGRADE_ADD_TAG)
	{
		BoostBaudrate(FALSE);
		m_DownloadMode = InternalLoaderV8AddTag;
		if(!DownloadLoader())
		{
			add_msgtolist("Software image download failed...");	 
			::AfxMessageBox("Load loader into RAM failed!");
			m_psoftImgDlDlg->SetFinish(true);	
			BoostBaudrate(TRUE);
			SetMode();
			CreateGPSThread();
			m_gpsdoInProgress = false;
			if(SHOW_PATCH_MENU)
			{
				patchLog.Format("PatchResult: %08X%08X", patchData, patchStatus);
				add_msgtolist(patchLog);
				AfxMessageBox(patchLog);
			}
			return false;
		}
		m_DownloadMode = CustomerUpgrade;
	}
	else
	{
		U08 msg[9] = {0};
		int cmdSize = 6;
		msg[0] = 0x0B;
		msg[1] = (U08)m_nDownloadBaudIdx;
		msg[2] = (U08)0;
		msg[3] = (U08)0;
		msg[4] = (U08)0;
		msg[5] = (U08)m_nDownloadBufferIdx;

		int len = SetMessage(msg, cmdSize);
		if(!SendToTarget(CGPSDlg::m_inputMsg, len, "Loader start...", true))
		{
			isSuccessful = false;
		}
		else if(m_nDownloadBaudIdx < SpiBaudIndex)
		{
			CloseOpenUart();
			m_serial->ResetPort(m_nDownloadBaudIdx);
			m_BaudRateCombo.SetCurSel(m_nDownloadBaudIdx);
			Sleep(200);
		}

		if(!isSuccessful)
		{
			add_msgtolist("Software Image Download Failed...");
			::AfxMessageBox("Download failed!");
			if(SHOW_PATCH_MENU)
			{
				patchLog.Format("PatchResult: %08X%08X", patchData, patchStatus);
				add_msgtolist(patchLog);
				AfxMessageBox(patchLog);
			}
			return false;
		}
	}

	if(g_setting.delayBeforeBinsize)
	{
		Sleep(g_setting.delayBeforeBinsize);
	}

	isSuccessful = FirmwareUpdate(m_strDownloadImage);
	if(!isSuccessful)
	{
		add_msgtolist("Software Image Download Failed...");
		::AfxMessageBox("Download failed!");
		if(SHOW_PATCH_MENU)
		{
			patchLog.Format("PatchResult: %08X%08X", patchData, patchStatus);
			add_msgtolist(patchLog);
			AfxMessageBox(patchLog);
		}
		return false;
	}

	CString strMsg;
	strMsg.Format("Download total time %d seconds.", t.GetDuration() / 1000);
	add_msgtolist(strMsg);
	DeleteNmeaMemery();
	m_firstDataIn = false;
	m_inputMode = NmeaMessageMode;	
	SetMsgType(NmeaMessageMode);

	if(m_psoftImgDlDlg)
	{
		m_psoftImgDlDlg->SetFinish(true);	
	}

	BoostBaudrate(TRUE);
	if(g_setting.downloadTesting)
	{
		Sleep(3000);
	}

	if(m_gpsdoInProgress)
	{
		GpsdoLeaveDownload(Display, NULL);
		SetBaudrate(5);
		m_gpsdoInProgress = false;
	}
	if(SHOW_PATCH_MENU)
	{
		patchLog.Format("PatchResult: %08X%08X", patchData, patchStatus);
		add_msgtolist(patchLog);
		AfxMessageBox(patchLog);
	}
	SetMode();
	m_firstDataIn = false;
	ClearInformation();
	CreateGPSThread();
	return true;
}

bool CGPSDlg::Download3()
{
	m_DownloadMode = InternalLoaderV8;
	m_nDownloadBaudIdx = ((CComboBox*)GetDlgItem(IDC_DL_BAUDRATE))->GetCurSel();
	//m_nDownloadBaudIdx += 5;	//Download Baudrate start in 115200
	m_gpsdoInProgress = true;
	bool isSuccessful = Download();

	Sleep(1000);	//Wait for master firmware boot.
	BinaryCommand configCmd;
	BinaryData cmd(4);

	//In : a0 a1 00 04 05 01 01 02 05 0d 0a ,Cfg COM1-9600 bps
	*cmd.GetBuffer(0) = 0x05;
	*cmd.GetBuffer(1) = 0x01;
	*cmd.GetBuffer(2) = m_nSlaveSourceBaud;
	*cmd.GetBuffer(3) = 0x02;
	configCmd.SetData(cmd);
	isSuccessful = CGPSDlg::gpsDlg->ExecuteConfigureCommand(configCmd.GetBuffer(), configCmd.Size(), "Configure master COM1 successfully", false);
	Sleep(500);	//Wait for master firmware boot.

	//In : a0 a1 00 04 7a 08 01 01 72 0d 0a 
	*cmd.GetBuffer(0) = 0x7a;
	*cmd.GetBuffer(1) = 0x08;
	*cmd.GetBuffer(2) = 0x01;
	*cmd.GetBuffer(3) = 0x01;
	configCmd.SetData(cmd);
	isSuccessful = CGPSDlg::gpsDlg->ExecuteConfigureCommand(configCmd.GetBuffer(), configCmd.Size(), "Enter UART passthrough successfully", false);
	Sleep(500);	//Wait for master firmware boot.

	//In : a0 a1 00 04 05 00 06 00 03 0d 0a 
	*cmd.GetBuffer(0) = 0x05;
	*cmd.GetBuffer(1) = 0x00;
	*cmd.GetBuffer(2) = 0x06;		//Slave download baud rate is 230400 bps
	*cmd.GetBuffer(3) = 0x02;
	configCmd.SetData(cmd);
	isSuccessful = CGPSDlg::gpsDlg->ExecuteConfigureCommand(configCmd.GetBuffer(), configCmd.Size(), "Configure slave COM0 successfully", false);
	Sleep(500);	//Wait for master firmware boot.

	//In : a0 a1 00 04 7a 08 01 00 73 0d 0a 
	*cmd.GetBuffer(0) = 0x7a;
	*cmd.GetBuffer(1) = 0x08;
	*cmd.GetBuffer(2) = 0x01;
	*cmd.GetBuffer(3) = 0x00;
	configCmd.SetData(cmd);
	isSuccessful = CGPSDlg::gpsDlg->ExecuteConfigureCommand(configCmd.GetBuffer(), configCmd.Size(), "Leave UART passthrough successfully", false);
	Sleep(500);	//Wait for master firmware boot.

	//In : a0 a1 00 04 05 01 06 00 02 0d 0a 
	*cmd.GetBuffer(0) = 0x05;
	*cmd.GetBuffer(1) = 0x01;
	*cmd.GetBuffer(2) = 0x06;		//Slave download baud rate is 230400 bps
	*cmd.GetBuffer(3) = 0x02;
	configCmd.SetData(cmd);
	isSuccessful = CGPSDlg::gpsDlg->ExecuteConfigureCommand(configCmd.GetBuffer(), configCmd.Size(), "Configure master COM1 successfully", false);
	Sleep(500);	//Wait for master firmware boot.

	//In : a0 a1 00 04 05 00 06 00 02 0d 0a 
	*cmd.GetBuffer(0) = 0x05;
	*cmd.GetBuffer(1) = 0x00;
	*cmd.GetBuffer(2) = 0x06;		//Slave download baud rate is 230400 bps
	*cmd.GetBuffer(3) = 0x02;
	configCmd.SetData(cmd);
	isSuccessful = CGPSDlg::gpsDlg->ExecuteConfigureCommand(configCmd.GetBuffer(), configCmd.Size(), "Configure master COM0 successfully", false);
	Sleep(500);	//Wait for master firmware boot.

	//In : a0 a1 00 04 7a 08 01 03 70 0d 0a 
	*cmd.GetBuffer(0) = 0x7a;
	*cmd.GetBuffer(1) = 0x08;
	*cmd.GetBuffer(2) = 0x01;
	*cmd.GetBuffer(3) = 0x03;
	configCmd.SetData(cmd);
	isSuccessful = CGPSDlg::gpsDlg->ExecuteConfigureCommand(configCmd.GetBuffer(), configCmd.Size(), "Enter slave download successfully", false);
	Sleep(500);	//Wait for master firmware boot.

	//Download slave
	m_DownloadMode = InternalLoaderV8;
	m_nDownloadBaudIdx = 6;		//Slave download baud rate is 230400 bps
	m_gpsdoInProgress = true;
	isSuccessful = Download();

	//In : a0 a1 00 04 7a 08 01 02 71 0d 0a 
	*cmd.GetBuffer(0) = 0x7a;
	*cmd.GetBuffer(1) = 0x08;
	*cmd.GetBuffer(2) = 0x01;
	*cmd.GetBuffer(3) = 0x02;
	configCmd.SetData(cmd);
	isSuccessful = CGPSDlg::gpsDlg->ExecuteConfigureCommand(configCmd.GetBuffer(), configCmd.Size(), "Leave slave download successfully", false);
	Sleep(500);	//Wait for master firmware boot.

	//In : a0 a1 00 04 05 01 05 00 02 0d 0a 
	*cmd.GetBuffer(0) = 0x05;
	*cmd.GetBuffer(1) = 0x01;
	*cmd.GetBuffer(2) = m_nSlaveTargetBaud;		//Slave target baud rate
	*cmd.GetBuffer(3) = 0x00;
	configCmd.SetData(cmd);
	isSuccessful = CGPSDlg::gpsDlg->ExecuteConfigureCommand(configCmd.GetBuffer(), configCmd.Size(), "Configure master COM1 successfully", false);
	Sleep(500);	//Wait for master firmware boot.


	BoostBaudrate(TRUE);

	SetMode();
	m_firstDataIn = false;
	ClearInformation();
	CreateGPSThread();
	return true;
}

