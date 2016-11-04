#pragma once
//#include "afxwin.h"
//#include "afxcmn.h"


// CSavenmea

class CSaveNmea : public CDialog
{
	DECLARE_DYNAMIC(CSaveNmea)

public:
	CSaveNmea(CWnd* pParent = NULL);   
	virtual ~CSaveNmea();

	virtual BOOL OnInitDialog();

	enum EventType
	{
		StopWriteNmea,
		ClickClose,
	};

	enum LogFunction
	{
		NMEA_Mode,
    Binary_Mode,
    HostLog_Mode
	};

	UINT RegisterEventMessage() 
	{	
		m_dialogEvent = ::RegisterWindowMessage("CSaveNmea-Event");
		return m_dialogEvent; 
	}
	void SetNotifyWindow(HWND h) { m_notifyWindow = h; };
	void Initialize(LPCSTR nmeaFile);
	void StartSave(LogFunction fun, LPCSTR title, LPCSTR filePath);
	void StopSave();

	static bool IsNmeaFileOpen() { return isNmeaFileOpen; }
	static bool IsPressNmeaCommend() { return isPressNmeaCommend; }
  static bool ClearBuffer();
	static bool SaveData(const void* p, int len);
	static bool SaveText(const void* p, int len);
	static bool SaveBinary(const void* p, int len);
	static void SetLogFunction(LogFunction f);
	static void WriteFile(void* p, int len);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 

	DECLARE_MESSAGE_MAP()
	virtual void OnCancel();
	virtual void PostNcDestroy();
public:
  static bool isRestartAck;	
	static CSaveNmea* sthis;

	afx_msg void OnBnClickedContinue();
	afx_msg void OnBnClickedStop();
	afx_msg void OnBnClickedClose();

protected:
	UINT m_dialogEvent;
	HWND m_notifyWindow;

	static CString writeFilePath;
	static bool isNmeaFileOpen;	
	static bool isPressNmeaCommend;
	static LogFunction logFunction;

};

class CPlayNmea : public CDialog
{
	DECLARE_DYNAMIC(CPlayNmea)

public:
	CPlayNmea(CWnd* pParent = NULL);  
	virtual ~CPlayNmea();

	enum EventType
	{
		IntervalChange,
		PauseStatus,
		ClickClose,
	};

	UINT RegisterEventMessage();
	void SetNotifyWindow(HWND h) { m_notifyWindow = h; };

	void Initialize(LPCSTR nmeaFile);
	int GetPlayInterval();
	void SetLineCount(int count, long size, long total);

	afx_msg void OnBnClickedClose();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedPlayControl();
	afx_msg void OnBnClickedTimeSync();

protected:
	UINT m_dialogEvent;
	HWND m_notifyWindow;
	CSliderCtrl m_playIntervalSlider;
	bool m_isPlayControlInPause;

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 
	virtual void OnCancel();
	virtual void PostNcDestroy();

	DECLARE_MESSAGE_MAP()
};
