// RawMeasmentOutputConvertDlg.cpp 
//

#include "stdafx.h"
#include "RawMeasmentOutputConvertDlg.h"
#include "GPS.h"
#include "MessageParser.h"
#include "GPSDlg.h"

#define UWM_RAW_PROGRESS		(WM_USER + 0x1039)
// CRawMeasmentOutputConvertDlg
const int MaxProgress = 100;

IMPLEMENT_DYNAMIC(CRawMeasmentOutputConvertDlg, CDialog)

CRawMeasmentOutputConvertDlg::CRawMeasmentOutputConvertDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_RAW_MEAS_OUT_CONV, pParent)
{
	m_convertRunning = false;
  mode = RawMeasment;
}

CRawMeasmentOutputConvertDlg::~CRawMeasmentOutputConvertDlg()
{
}

void CRawMeasmentOutputConvertDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CRawMeasmentOutputConvertDlg, CDialog)
	ON_BN_CLICKED(IDC_BROWSE, &CRawMeasmentOutputConvertDlg::OnBnClickedBrowse)
	ON_BN_CLICKED(IDC_GO, &CRawMeasmentOutputConvertDlg::OnBnClickedGo)
	ON_MESSAGE(UWM_RAW_PROGRESS, OnRawProgress)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CRawMeasmentOutputConvertDlg
void CRawMeasmentOutputConvertDlg::OnBnClickedBrowse()
{
	CFileDialog dlgFile(TRUE, _T("*.*"), NULL, 
		OFN_HIDEREADONLY, _T("ALL Files (*.*)|*.*||"), this);
	
  INT_PTR nResult = dlgFile.DoModal();
	m_filePath = dlgFile.GetPathName();
	if(nResult == IDOK)
	{
		GetDlgItem(IDC_PATH)->SetWindowText(m_filePath);
	}
	if(Utility::IsFileExist(m_filePath))
	{
		GetDlgItem(IDC_GO)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_GO)->EnableWindow(FALSE);
	}
}

DWORD GetBinary(void *buffer, DWORD bufferSize, CFile& f)
{	
	U08* bufferIter = (U08*)buffer;
	DWORD totalSize = 0;
	DWORD failCount = 10;
  bool hasHeader = false;
  bool hasSize = false;
  U16 headerSize = 0;
	while(totalSize < bufferSize - 1)
	{ 
		DWORD dwBytesDoRead = 0;
		DWORD dwErrorFlags = 0;

		DWORD bytesinbuff = (DWORD)(f.GetLength() - f.GetPosition());
		while(bytesinbuff)
		{
			BOOL bb = ReadFile(f.m_hFile, bufferIter, 1, &dwBytesDoRead, 0);
			if(dwBytesDoRead <= 0)
			{	//Read fail.
				DWORD dwErr = ::GetLastError();
				break;
			}

			if(totalSize > 0)
			{	//not first char.
				if(!hasHeader && *bufferIter == 0xa1 && *(bufferIter - 1) == 0xa0)
				{
					bufferIter -= totalSize;
					*bufferIter = 0xa0; 
					++bufferIter;
					*bufferIter = 0xa1; 
					++bufferIter;
					totalSize = 2;
          hasHeader = true;
					continue;
				}
				else if(hasHeader && hasSize & (totalSize > headerSize + 3U) && 
          *bufferIter == 0x0a && *(bufferIter - 1) == 0x0d)
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
        else if(totalSize == 2)
        {
          headerSize = *bufferIter << 8;
        }
        else if(totalSize == 3)
        {
          headerSize |= *bufferIter;
          hasSize = true;
        }
			} //if(totalSize > 0)
			++totalSize;
			if (totalSize >=  bufferSize - 1)
			{	//Check 
				*(bufferIter+1) = 0;
				break;
			}
				
			++bufferIter;
			--bytesinbuff;
		} //while(bytesinbuff)
		if(bytesinbuff==0)
		{
			return totalSize;
		}
	} //while(total < size - 1)
	return totalSize;
}

#include "BinaryMSG.h"
void BinaryProc(U08* buffer, int len, CFile& f)
{
	if(len <= 7)
	{
		return;
	}

	int packetLen = MAKEWORD(buffer[3], buffer[2]);
	if(packetLen + 7 > len) 
	{
		return;
	}

	U08 msgType = Cal_Checksum(buffer);	
	CString strOutput;
	switch(msgType)
	{
	case 0xDC:		// measurement time
		ShowMeasurementTime(buffer, true, &strOutput);
		break;
	case 0xDD:		// raw measurement
		ShowMeasurementChannel(buffer, true, &strOutput);
		//Show_Message(buffer,len);
		break;
	case 0xDE:		// SV_CH status
		ShowMeasurementSv(buffer, true, &strOutput);
		//Show_Message(buffer,len);
		break;
	case 0xDF:		// receiver state
		ShowReceiverNav(buffer, true, &strOutput);
		//Show_Message(buffer,len);
		break;
	case 0xE0:		// sub frame data
	case 0xE1:		// sub frame data
	case 0xE2:		// sub frame data
	case 0xE3:		// sub frame data
		//Show_Message(buffer,len);
		ShowSubframe(buffer, true, &strOutput);
		break;
	case 0xE4:		// sub frame data
    //20160913 Don't show SBAS subframe data, request from Ryan and Andrew
#if(IS_DEBUG)
		ShowSubframe(buffer, true, &strOutput);
#endif
		break;
	case 0xE5:		// EXT_RAW_MEAS Extended Raw Measurement Data v.1 (0xE5) (Periodic)
		ExtRawMeas(buffer, true, &strOutput);
		break;
	case BINMSG_ECEF_USER_PVT:
		ShowBinaryOutput(buffer, true, &strOutput);
		break;
	case 0xEF:		// Navigation data and status (0xEF)
		ExtRawMeasEf(buffer, true, &strOutput);
		break;
	default:
//		add_msgtolist("Unknown: " + theApp.GetHexString(buffer, len));	
		break;
	}
	if(strOutput.GetLength() > 0)
	{
		f.Write((LPCTSTR)strOutput, strOutput.GetLength());
	}
}

static bool cancelConvert = false;
UINT RawMeasmentOutputConvertThread(LPVOID pParam)
{
	CRawMeasmentOutputConvertDlg* pDlg = (CRawMeasmentOutputConvertDlg*)pParam;
	int length = 0;	

	static U08 buffer[COM_BUFFER_SIZE] = {0};
	CFile f(pDlg->GetFilePath(), CFile::modeRead);
	CString strOutput = pDlg->GetFilePath();
	strOutput += ".txt";
	CFile fo(strOutput, CFile::modeWrite | CFile::modeCreate);
	ULONGLONG total = f.GetLength();
	WPARAM progress = 0;
	pDlg->PostMessage(UWM_RAW_PROGRESS, progress);

	while(!cancelConvert)
	{	
		length = GetBinary(buffer, sizeof(buffer), f);
		if(progress != (WPARAM)(MaxProgress * (double)f.GetPosition() / total))
		{
			progress = (WPARAM)(MaxProgress * (double)f.GetPosition() / total);
			pDlg->PostMessage(UWM_RAW_PROGRESS, progress);
		}
		//p->SetPos((int)f.GetPosition());
		if(length == 0)
		{
			break;
		}
		buffer[length + 1] = 0;
		BinaryProc(buffer, length, fo);				
	}
	fo.Close();
	f.Close();
  if(!cancelConvert)
  {
	  pDlg->PostMessage(UWM_RAW_PROGRESS, MaxProgress, TRUE);
  }
  return 0;
}

static CFile convertFile;
static DWORD ReadFileOneChar(U08* c, int timeout)
{
  DWORD dwBytesRead = 0;
  dwBytesRead = convertFile.Read(c, 1);
  if(dwBytesRead == 0)
  {
    return MessageParser::READERROR;
  }
  return dwBytesRead;
}

UINT UbloxBinaryOutputConvertThread(LPVOID pParam)
{
	CRawMeasmentOutputConvertDlg* pDlg = (CRawMeasmentOutputConvertDlg*)pParam;
	DWORD length = 0;	

	static U08 buffer[COM_BUFFER_SIZE] = {0};
  convertFile.Open(pDlg->GetFilePath(), CFile::modeRead);
	CString strOutput = pDlg->GetFilePath();
	strOutput += ".txt";
	CFile fo(strOutput, CFile::modeWrite | CFile::modeCreate);
	ULONGLONG total = convertFile.GetLength();
	WPARAM progress = 0;
	pDlg->PostMessage(UWM_RAW_PROGRESS, progress);
  MessageType lastType = MtUnknown;
  MessageParser mp;

  mp.SetCancelTransmission(&cancelConvert);
  mp.SetReadOneCharCallback(ReadFileOneChar);

  strOutput = "$UBX-TIM-TP,towMS,towSubMS,qErr,week\r\n";
  strOutput += "$UBX-NAV-SOL,iTOW,fTOW,week,gpsFix,flags,ecefX,ecefY,ecefZ,pAcc,ecefVX,ecefVY,ecefVZ,sAcc,pDop,numSv\r\n";
  strOutput += "$UBX-NAV-SVINFO,iTOW,numCh,globalFlags\r\n";
  strOutput += "$UBX-NAV-STATUS,iTOW,gpsFix,flags,fixStat,flags2,ttff,msss\r\n";
  strOutput += "$UBX-NAV-POSLLH,iTOW,lon,lat,height,hMSL,hAcc,vAcc\r\n";
  strOutput += "$UBX-NAV-DOP,iTOW,gDOP,pDOP,tDOP,vDOP,hDOP,nDOP,eDOP\r\n";
  strOutput += "$UBX-NAV-VELEND,iTOW,velN,velE,velD,speed,gSpeed,heading,sAcc,cAcc\r\n\r\n";
  fo.Write((LPCTSTR)strOutput, strOutput.GetLength());

	while(!cancelConvert)
	{	
    MessageType type = mp.GetParsingData(buffer, sizeof(buffer) - 1, &length, 5000);
		if(progress != (WPARAM)(MaxProgress * (double)convertFile.GetPosition() / total))
		{
			progress = (WPARAM)(MaxProgress * (double)convertFile.GetPosition() / total);
			pDlg->PostMessage(UWM_RAW_PROGRESS, progress);
		}

		if(type == MtReadError)
		{
			break;
		}
 		if(type == UbloxMessage)
    {
			  CGPSDlg::gpsDlg->UbloxProc(buffer, length + 1, &fo);
    }  
	}
	fo.Close();
	convertFile.Close();
  if(!cancelConvert)
  {
	  pDlg->PostMessage(UWM_RAW_PROGRESS, MaxProgress, TRUE);
  }
  return 0;
}

bool ReadOneLineInFile(CFile& f, char* buffer, int size);
bool PreprocessInputLine(U08 *buf, int& bufLen);
bool AllPrintable(const char* buffer, int len);
UINT RawMeasmentOutputConvertHostLogThread(LPVOID pParam)
{
	CRawMeasmentOutputConvertDlg* pDlg = (CRawMeasmentOutputConvertDlg*)pParam;
	int length = 0;	

	static char buffer[COM_BUFFER_SIZE * 16] = {0};
	static char line[4096] = {0};
	CFile f(pDlg->GetFilePath(), CFile::modeRead);
	CString strOutput = pDlg->GetFilePath();
	strOutput += ".txt";
	CFile fo(strOutput, CFile::modeWrite | CFile::modeCreate);
	ULONGLONG total = f.GetLength();
	WPARAM progress = 0;
	pDlg->PostMessage(UWM_RAW_PROGRESS, progress);
  UINT size = 0;
  const char *iter = buffer;
  char *witer = line;
  enum LineState
  {
    None,
    Start,
    Got0D,
  };

  LineState getline = None;
  line[0] = '$';
  do
  {
    size = f.Read(buffer, sizeof(buffer));
    UINT count = size;
    iter = buffer;
	  while(!cancelConvert && count > 0)
	  {
      if(getline == None && *iter == '$')
      {
        getline = Start;
        witer = line + 1;
      }
      else if(getline == Start && *iter == '$')
      {
        witer = line + 1;
      }
      else if(getline == Start && *iter == 0x0D && (witer - line) < 3)
      {
        getline = None;
      }
      else if(getline == Start && *iter == 0x0D)
      {
        *witer = 0x0D;
        ++witer;
        getline = Got0D;
      }
      else if(getline == Got0D && *iter == 0x0A)
      { //
        *witer = 0x0A;
        *(witer + 1) = 0;
        fo.Write(line, strlen(line));
        getline = None;
      }
      else if(getline == Got0D && *iter != 0x0A)
      {
        getline = None;
      }
      else if(getline == Start && (*iter < 0x20 || *iter > 0x7E))
      { //line hase non-printable char, 
        getline = None;
      }
      else if(getline == Start)
      {
        *witer = *iter;
        ++witer;
      }
      ++iter;
      --count;
      if(cancelConvert)
      {
        int a = 0;
      }
	  } //while(!cancelConvert && count > 0)

    if(progress != (WPARAM)(MaxProgress * (double)f.GetPosition() / total))
	  {
		  progress = (WPARAM)(MaxProgress * (double)f.GetPosition() / total);
		  pDlg->PostMessage(UWM_RAW_PROGRESS, progress);
	  }
  } while(size == sizeof(buffer) && !cancelConvert);
	fo.Close();
	f.Close();
  if(!cancelConvert)
  {
	  pDlg->PostMessage(UWM_RAW_PROGRESS, MaxProgress, TRUE);
  }
	return 0;
}

CWinThread* pThread = NULL;
void CRawMeasmentOutputConvertDlg::OnBnClickedGo()
{
	if(!m_convertRunning)
	{
		cancelConvert = false;
    if(mode == RawMeasment)
    {
		  pThread = AfxBeginThread(RawMeasmentOutputConvertThread, this);
    }
    else if(mode == HostLog)
    {
		  pThread = AfxBeginThread(RawMeasmentOutputConvertHostLogThread, this);
    }
    else if(mode == UbloxBinary)
    {
		  pThread = AfxBeginThread(UbloxBinaryOutputConvertThread, this);
    }		
    CProgressCtrl *p = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS1);
		p->SetRange32(0, MaxProgress);
		p->SetPos(0);
		m_convertRunning = true;
	}
	else
	{
		cancelConvert = true;
		::WaitForSingleObject(pThread->m_hThread, INFINITE);
		m_convertRunning = false;
	}
	UpdateConvertUI();

}

BOOL CRawMeasmentOutputConvertDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	GetDlgItem(IDC_MESSAGE)->SetWindowText("");
	OnBnClickedBrowse();

	return TRUE;  // return TRUE unless you set the focus to a control
}

LRESULT CRawMeasmentOutputConvertDlg::OnRawProgress(WPARAM wParam, LPARAM lParam)
{
	CProgressCtrl *p = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS1);
	p->SetPos(wParam);

	CString strMsg;
	strMsg.Format("Converting %d / %d", (int)wParam, MaxProgress);
	GetDlgItem(IDC_MESSAGE)->SetWindowText(strMsg);

	if(lParam == TRUE)
	{
		cancelConvert = true;
		m_convertRunning = false;
		strMsg.Format("Converting finished.");
		GetDlgItem(IDC_MESSAGE)->SetWindowText(strMsg);
	}
	UpdateConvertUI();
	return 0;
}

void CRawMeasmentOutputConvertDlg::UpdateConvertUI()
{
	if(m_convertRunning)
	{
		GetDlgItem(IDC_GO)->SetWindowText("Cancel");
		GetDlgItem(IDC_BROWSE)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_GO)->SetWindowText("Go");
		GetDlgItem(IDC_BROWSE)->EnableWindow(TRUE);
	}
}

void CRawMeasmentOutputConvertDlg::OnClose()
{
	if(m_convertRunning)
	{
		::AfxMessageBox("Converter still running!");
		return;
	}
	CDialog::OnClose();
}
