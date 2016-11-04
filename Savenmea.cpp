// Savenmea.cpp
//

#include "stdafx.h"
#include "GPS.h"
#include "Savenmea.h"
#include "GPSDlg.h"


// CSaveNmea
CString CSaveNmea::writeFilePath;	
bool CSaveNmea::isNmeaFileOpen = false;	
bool CSaveNmea::isPressNmeaCommend = false;
bool CSaveNmea::isRestartAck = false;
CSaveNmea* CSaveNmea::sthis = NULL;
CSaveNmea::LogFunction CSaveNmea::logFunction = CSaveNmea::NMEA_Mode;

static const int bufferSize = 64 * 1024;
static const int bufferBlocks = 4;
static BinaryData buffer[bufferBlocks];
static U08* bufferPtr[bufferBlocks];
static int writeIter = 0, flashIter = 0;
static bool initialized = false;
static DWORD writeTime = 0;
static bool stopFlash = false;
static CString strDbg("", 128);

UINT AFX_CDECL FlashBufferThread(LPVOID param)
{
	DWORD t = 0;
  while(!stopFlash)
  {
    if(writeIter == flashIter)
    {
      Sleep(100);
      continue;
    }

    int flashLen = bufferPtr[flashIter] - buffer[flashIter].GetBuffer(0);
    strDbg.Format("WriteFileB,%d,%d,%d\r\n", flashIter, writeIter, flashLen); ::OutputDebugString(strDbg);
    t = ::GetTickCount();
    if(CSaveNmea::sthis)
    {
      CSaveNmea::sthis->WriteFile(buffer[flashIter].GetBuffer(0), flashLen);
      bufferPtr[flashIter] = buffer[flashIter].GetBuffer(0);
      if(++flashIter == bufferBlocks)
      {
        flashIter = 0;
      }
      strDbg.Format("WriteFileA,%d\r\n", ::GetTickCount() - t); ::OutputDebugString(strDbg);
    }
  }
  strDbg.Format("FlashBufferThread End\r\n"); ::OutputDebugString(strDbg);
  return 0;
}

bool NextWritePtr()
{
  if(++writeIter == bufferBlocks)
  {
    writeIter = 0;
  }

  strDbg.Format("NextWritePtr,%d,%d,%d\r\n", flashIter, writeIter, ((writeIter - flashIter) + bufferBlocks) % bufferBlocks); ::OutputDebugString(strDbg);
  if(flashIter == writeIter)
  { //fatel error!
    strDbg.Format("NextWritePtr fatel error!\r\n"); ::OutputDebugString(strDbg);
    return false;
  }
  return true;
}

IMPLEMENT_DYNAMIC(CSaveNmea, CDialog)
CSaveNmea::CSaveNmea(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_SAVENMEA, pParent)
{
	m_dialogEvent = 0;
	m_notifyWindow = NULL;
	sthis = this;
}

CSaveNmea::~CSaveNmea()
{
	isNmeaFileOpen = false;	
	isPressNmeaCommend = false;
	sthis = NULL;
}

void CSaveNmea::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSaveNmea, CDialog)
	ON_BN_CLICKED(IDC_CONTINUE, OnBnClickedContinue)
	ON_BN_CLICKED(IDC_STOP, OnBnClickedStop)
	ON_BN_CLICKED(IDC_CLOSE, OnBnClickedClose)
END_MESSAGE_MAP()


// CSaveNmea
BOOL CSaveNmea::OnInitDialog()
{
	CDialog::OnInitDialog();
  if(logFunction == HostLog_Mode)
  {
    GetDlgItem(IDC_CONTINUE)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_STOP)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_PROMPT)->SetWindowText("Restart does not ACK.");
    GetDlgItem(IDC_PROMPT)->ShowWindow(SW_SHOW);

    SetWindowText("Save HostLog");
    GetDlgItem(IDC_FILENAME)->SetWindowText(writeFilePath);
    GetDlgItem(IDC_FILESIZE)->SetWindowText("0 Bytes");
    GetDlgItem(IDC_CONTINUE)->EnableWindow(FALSE);
    GetDlgItem(IDC_STOP)->EnableWindow(TRUE);

	  CTime t = CTime::GetCurrentTime();
	  CString fileName;
	  fileName.Format("Binary%02d-%02d-%02d_%02d%02d%02d.out", t.GetYear(), t.GetMonth(), t.GetDay(),
		  t.GetHour(), t.GetMinute(), t.GetSecond());
    
    isRestartAck = FALSE;
	  BinaryData binData(15);
	  *(binData.GetBuffer(0)) = 1;
	  *(binData.GetBuffer(1)) = 1;
	  BinaryCommand binCmd(binData);
	  CGPSDlg::gpsDlg->Restart(binCmd.GetBuffer(), FALSE);
  }
  else if(logFunction == Binary_Mode)
  {

  }
  else if(logFunction == NMEA_Mode)
  {

  }
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CSaveNmea::OnCancel()
{
	ShowWindow(SW_HIDE);
}

void CSaveNmea::PostNcDestroy()
{	
	CDialog::PostNcDestroy();
}

void CSaveNmea::OnBnClickedContinue()
{
	//CGPSDlg::gpsDlg->continue_write_nmea();
	//::PostMessage(m_notifyWindow, m_dialogEvent, StopWriteNmea, 0);
	if(isNmeaFileOpen == false)
	{
		isNmeaFileOpen = true;	
	}
	GetDlgItem(IDC_CONTINUE)->EnableWindow(FALSE);
	GetDlgItem(IDC_STOP)->EnableWindow(TRUE);
}

void CSaveNmea::OnBnClickedStop()
{
	if(isNmeaFileOpen == true)
	{
		isNmeaFileOpen = false;	
	}
	GetDlgItem(IDC_CONTINUE)->EnableWindow(TRUE);
	GetDlgItem(IDC_STOP)->EnableWindow(FALSE);
}

void CSaveNmea::StopSave() 
{ 
  isNmeaFileOpen = false;
  NextWritePtr();
  Sleep(500);
  stopFlash = true;
}

void CSaveNmea::OnBnClickedClose()
{
  StopSave();

  if(logFunction == HostLog_Mode)
  {
    OnOK();
  }
  else
  {
	  DestroyWindow();
	  ::PostMessage(m_notifyWindow, m_dialogEvent, ClickClose, 0);
  }
}

void CSaveNmea::Initialize(LPCSTR nmeaFile)
{
  switch(logFunction)
  {
  case NMEA_Mode:
    SetWindowText("Save Message");
    GetDlgItem(IDC_PROMPT)->ShowWindow(SW_HIDE);
    break;
  case Binary_Mode:
    SetWindowText("Save Device Output");
    GetDlgItem(IDC_PROMPT)->ShowWindow(SW_HIDE);
    break;
  case HostLog_Mode:
    return;
  }

	GetDlgItem(IDC_FILENAME)->SetWindowText(nmeaFile);
	GetDlgItem(IDC_FILESIZE)->SetWindowText("0 Bytes");
	GetDlgItem(IDC_CONTINUE)->EnableWindow(FALSE);
	GetDlgItem(IDC_STOP)->EnableWindow(TRUE);
}


void CSaveNmea::StartSave(LogFunction fun, LPCSTR title, LPCSTR filePath)
{
  SetLogFunction(fun);
	writeFilePath = filePath;
	Initialize(title);

	CFile f;
	f.Open(writeFilePath, CFile::modeReadWrite | CFile::modeCreate | CFile::shareDenyWrite);
	CString nmeaFileTitle("\r\n");
	f.Write(nmeaFileTitle, nmeaFileTitle.GetLength());
	f.Close();

	isNmeaFileOpen = true;	
  CSerial::readCount = 0;

	isPressNmeaCommend = true;
  stopFlash = false;
  ClearBuffer();
  AfxBeginThread(FlashBufferThread, 0);
}

bool CSaveNmea::ClearBuffer()
{
  for(int i = 0; i < bufferBlocks; ++i)
  {
    if(!initialized)
    {
      buffer[i].Alloc(bufferSize);
    }
    bufferPtr[i] = buffer[i].GetBuffer(0);
    writeIter = 0;
    flashIter = 0;
    writeTime = 0;
  }
  initialized = true;

  return true;
}

bool CSaveNmea::SaveData(const void* p, int len)
{
	DWORD t = ::GetTickCount();
  strDbg.Format("SaveData,%d,%d,%02X %02X %02X %02X %02X %02X %02X\r\n", len, CSerial::readCount, ((U08*)p)[7], ((U08*)p)[8], ((U08*)p)[9], ((U08*)p)[10], ((U08*)p)[11], ((U08*)p)[12], ((U08*)p)[14]); ::OutputDebugString(strDbg);
  CSerial::readCount = 0;

	if((bufferPtr[writeIter] - buffer[writeIter].GetBuffer(0) + len) > bufferSize)
	{
    strDbg.Format("SaveData1,%d,%d,%d\r\n", bufferPtr[writeIter] - buffer[writeIter].GetBuffer(0), len, bufferPtr[writeIter] - buffer[writeIter].GetBuffer(0) + len); ::OutputDebugString(strDbg);
    NextWritePtr();
	}
	memcpy(bufferPtr[writeIter], p, len);
	bufferPtr[writeIter] += len;
	if((t < writeTime) || (t - writeTime > 500))
  {
    strDbg.Format("SaveData2,%d,%d,%d\r\n", t, writeTime, t - writeTime); ::OutputDebugString(strDbg);
    NextWritePtr();
		writeTime = t;
	}
	return true;
}

bool CSaveNmea::SaveText(const void* p, int len)
{
	if(sthis==NULL || !IsNmeaFileOpen())
	{
		return false;
	}
	U08* buff = (U08*)p;
  if(logFunction != CSaveNmea::NMEA_Mode)
	{
		return false;
	}
	return SaveData(p, len);
}

void CSaveNmea::SetLogFunction(LogFunction f) 
{ 
  logFunction = f; 
}

bool CSaveNmea::SaveBinary(const void* p, int len)
{
	if(sthis==NULL || !IsNmeaFileOpen())
	{
		return false;
	}
	if(logFunction == NMEA_Mode)
	{
		return false;
	}
	return SaveData(p, len);
}

void CSaveNmea::WriteFile(void* p, int len)
{
  if(isRestartAck && CSaveNmea::sthis && CSaveNmea::sthis->GetSafeHwnd())
  {
    CSaveNmea::sthis->GetDlgItem(IDC_PROMPT)->SetWindowText("HostLog starts.");
  }

  CFileException fe;
  try
  {
	  CString msg;
	  CFile f;
	  f.Open(writeFilePath, CFile::modeReadWrite | CFile::shareDenyWrite, &fe);
	  f.SeekToEnd();
	  f.Write(p, len);
	  msg.Format("%d Bytes", f.GetLength());
	  f.Close();
    if(CSaveNmea::sthis && CSaveNmea::sthis->GetSafeHwnd())
    {
	    CSaveNmea::sthis->GetDlgItem(IDC_FILESIZE)->SetWindowText(msg);
    }
  }
  catch(CFileException e)
  {
    CString s;
    e.GetErrorMessage(s.GetBuffer(255), 255);
  }
}

// CPlayNmea

IMPLEMENT_DYNAMIC(CPlayNmea, CDialog)
CPlayNmea::CPlayNmea(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_PLAY_NMEA, pParent)
{
	m_dialogEvent = 0;
	m_notifyWindow = NULL;
}

CPlayNmea::~CPlayNmea()
{
}

void CPlayNmea::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_PLAY_INV_SL, m_playIntervalSlider);
}


BEGIN_MESSAGE_MAP(CPlayNmea, CDialog)
	ON_WM_HSCROLL()
	ON_BN_CLICKED(IDC_CLOSE, OnBnClickedClose)
	ON_BN_CLICKED(IDC_PLAY_CONTROL, &CPlayNmea::OnBnClickedPlayControl)
	ON_BN_CLICKED(IDC_TIME_SYNC, &CPlayNmea::OnBnClickedTimeSync)
END_MESSAGE_MAP()


// CPlayNmea 
void CPlayNmea::OnCancel()
{
	ShowWindow(SW_HIDE);
}

void CPlayNmea::PostNcDestroy()
{	
	CDialog::PostNcDestroy();
}

UINT CPlayNmea::RegisterEventMessage()
{
	m_dialogEvent = ::RegisterWindowMessage("CPlayNmea-Event");
	return m_dialogEvent;
}

const int IntervalRate[] = { 
	10000, 9000, 8000, 
	7000, 6000, 5000, 
	4000, 3000, 2000, 
	1000, 500, 250, 
	200, 125, 100, 
	50, 40, 25, 
	20 };

const char *IntervalStringTable[] = {
	"10 seconds", "9 seconds", "8 seconds",
	"7 seconds", "6 seconds", "5 seconds",
	"4 seconds", "3 seconds", "2 seconds",
	"1 seconds", "2 Hz", "4 Hz",
    "5 Hz", "8 Hz", "10 Hz",
	"20 Hz", "25 Hz", "40 Hz",
	"50 Hz" };
		
void CPlayNmea::Initialize(LPCSTR nmeaFile)
{
	int defaultRate = 9;
	GetDlgItem(IDC_NMEA_FILE)->SetWindowTextA(nmeaFile);
	GetDlgItem(IDC_PLAY_INV)->SetWindowTextA("1 seconds");
	m_playIntervalSlider.SetRange(0, sizeof(IntervalRate) / sizeof(IntervalRate[0]) - 1);
	m_playIntervalSlider.SetPageSize(1);
	m_playIntervalSlider.SetPos(defaultRate);
	m_isPlayControlInPause = false;
	GetDlgItem(IDC_PLAY_CONTROL)->SetWindowTextA("Pause");
}

int CPlayNmea::GetPlayInterval()
{
	int n = m_playIntervalSlider.GetPos();
	if(n < 0 || n > 19)
	{
		ASSERT(FALSE);
	}
	return IntervalRate[n];
}

void CPlayNmea::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if(pScrollBar->GetDlgCtrlID()==IDC_PLAY_INV_SL)
	{

		CString str = IntervalStringTable[m_playIntervalSlider.GetPos()];
		GetDlgItem(IDC_PLAY_INV)->SetWindowTextA(str);
		switch(nSBCode)
		{
		case TB_LINEUP:
		case TB_LINEDOWN:
		case TB_PAGEUP:
		case TB_PAGEDOWN:
		case TB_THUMBTRACK:
		case TB_TOP:
		case TB_BOTTOM:
			::PostMessage(m_notifyWindow, m_dialogEvent, IntervalChange, IntervalRate[m_playIntervalSlider.GetPos()]);
			break;
		default:
			break;
		}
	}
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CPlayNmea::SetLineCount(int count, long size, long total)
{
	CString str;
	str.Format("%d (%d / %d)", count, size, total);
	GetDlgItem(IDC_LINE_COUNT)->SetWindowTextA(str);

	CProgressCtrl* p = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS1);
	p->SetRange32(0, (int)total);
	p->SetPos((int)size);
	
}

void CPlayNmea::OnBnClickedPlayControl()
{
	if(m_isPlayControlInPause)
	{
		m_isPlayControlInPause = false;
		GetDlgItem(IDC_PLAY_CONTROL)->SetWindowTextA("Pause");
		::PostMessage(m_notifyWindow, m_dialogEvent, PauseStatus, FALSE);
	}
	else
	{
		m_isPlayControlInPause = true;
		GetDlgItem(IDC_PLAY_CONTROL)->SetWindowTextA("Resume");
		::PostMessage(m_notifyWindow, m_dialogEvent, PauseStatus, TRUE);
	}
}

void CPlayNmea::OnBnClickedClose()
{
  ::PostMessage(m_notifyWindow, m_dialogEvent, ClickClose, 0);
  ShowWindow(SW_HIDE);
}

void CPlayNmea::OnBnClickedTimeSync()
{
	BOOL b = ((CButton*)GetDlgItem(IDC_TIME_SYNC))->GetCheck();
	GetDlgItem(IDC_PLAY_INV_SL)->EnableWindow(!b);
}
