// SetupDialog.cpp : 實作檔
//

#include "stdafx.h"
#include "SetupDialog.h"
#include "Pic_Scatter.h"
#include "resource.h"

// CSetupDialog 對話方塊
IMPLEMENT_DYNAMIC(CSetupDialog, CDialog)

CSetupDialog::CSetupDialog(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_SETUP, pParent)
{
	setting = NULL;
}

CSetupDialog::~CSetupDialog()
{
}

void CSetupDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CSetupDialog, CDialog)
	ON_BN_CLICKED(IDOK, &CSetupDialog::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BROWSE, &CSetupDialog::OnBnClickedBrowse)
END_MESSAGE_MAP()

#define BOOST_BAUDIDX_BASE		5
BOOL ParsingLatLon(const CStringW& s, D64& lon, D64& lat)
{
	int pos = s.Find(',');
	if(pos >= s.GetLength() || pos < 0)
	{
		return FALSE;
	}
	CStringW p1 = s.Left(pos);
	CStringW p2 = s.Right(s.GetLength() - pos - 1);

	BOOL newStyle = FALSE;
	int signLon = 1, signLat = 1;
	//121.0087235°E, 24.7848383°N

	if(p1.GetLength() >= 2 && p2.GetLength() >= 2)
	{
		WCHAR c1 = p1[p1.GetLength() - 1];
		WCHAR c2 = p2[p2.GetLength() - 1];
		if(p1[p1.GetLength() - 2] == L'°' && p2[p2.GetLength() - 2] == L'°')
		{
			newStyle = TRUE;
			if(c1 == 'E' || 'e')
			{

			}
			else if(c1 == 'W' || 'w')
			{
				signLon = -1;
			}
			else
			{
				return FALSE;
			}
			if(c2 == 'N' || 'n')
			{

			}
			else if(c1 == 'S' || 's')
			{
				signLat = -1;
			}
			else
			{
				return FALSE;
			}
			p1 = p1.Left(p1.GetLength() - 2);
			p2 = p2.Left(p2.GetLength() - 2);
		}
	}
	
	lon = _wtof(p1);
	lat = _wtof(p2);
	if(newStyle)
	{
		lon *= signLon;
		lat *= signLat;
	}

	return TRUE;
}

void CSetupDialog::OnBnClickedOk()
{
	if(((CButton*)GetDlgItem(IDC_SPY_CENTER))->GetCheck())
	{
		CStringW w;
		::GetWindowTextW(GetDlgItem(IDC_LONLAT)->GetSafeHwnd(), w.GetBuffer(128), 128);
		w.ReleaseBuffer();
		D64 lat, lon;
		if(!ParsingLatLon(w, lon, lat))
		{
			::AfxMessageBox("The scatter center point format error!", MB_OK);
			return;
		}
		setting->scatterCenterLon = lon;
		setting->scatterCenterLat = lat;
		setting->AddRecentScatterCenter(CString(w));

	}

	CString s;
	GetDlgItem(IDC_BINSIZE_DELAY)->GetWindowText(s);
	setting->delayBeforeBinsize = (s.IsEmpty()) ? 0 : atoi(s);

	int boostBaudIdx = ((CComboBox*)GetDlgItem(IDC_BOOST_BAUDRATE))->GetCurSel() + BOOST_BAUDIDX_BASE;
	setting->boostBaudIndex = boostBaudIdx;

	setting->autoQueryVersion = ((CButton*)GetDlgItem(IDC_AUTO_QUERY_VERSION))->GetCheck();
	setting->boostEphemeris = ((CButton*)GetDlgItem(IDC_BOOST_EPHEMERIS))->GetCheck();
	setting->checkNmeaError = ((CButton*)GetDlgItem(IDC_CHECK_NMEA_ERR))->GetCheck();
	setting->downloadTesting = ((CButton*)GetDlgItem(IDC_DOWNLOAD_TEST))->GetCheck();
	setting->downloadRomInternal = ((CButton*)GetDlgItem(IDC_ROM_INTERNAL))->GetCheck();

	setting->responseLog = ((CButton*)GetDlgItem(IDC_ENABLE_LOG))->GetCheck();
	((CEdit*)GetDlgItem(IDC_LOG_PATH))->GetWindowText(setting->responseLogPath);
	setting->specifyCenter = ((CButton*)GetDlgItem(IDC_SPY_CENTER))->GetCheck();
	setting->specifyCenterAlt = ((CButton*)GetDlgItem(IDC_SPY_ALT))->GetCheck();


	GetDlgItem(IDC_ALT)->GetWindowText(s);
	setting->scatterCenterAlt = atof(s);
	GetDlgItem(IDC_TIMEOUT)->GetWindowText(s);
	setting->defaultTimeout = (s.IsEmpty()) ? DefaultTimeout : atoi(s);

	GetDlgItem(IDC_SCATTER_COUNT)->GetWindowText(s);
	int tmp = atoi(s);
	if(!s.IsEmpty() && tmp > 0 && setting->scatterCount != tmp)
	{
		int r = ::AfxMessageBox("Modify Scatter Count will clear all data in Scatter View, are you sure?", MB_YESNO);
		if(r == IDYES)
		{
			setting->scatterCount = tmp;
			g_scatterData.Clear();
		}
	}	
	
	OnOK();
}

void CSetupDialog::OnBnClickedBrowse()
{
	CString fileName(setting->responseLogPath);	
	CFileDialog dlgFile(FALSE, _T("log"), fileName, OFN_HIDEREADONLY, _T("ALL Files (*.*)|*.*||"), this);
	INT_PTR nResult = dlgFile.DoModal();
	if(nResult == IDOK)
	{  				
		fileName = dlgFile.GetPathName();
		setting->responseLogPath = fileName;
		((CEdit*)GetDlgItem(IDC_LOG_PATH))->SetWindowText(fileName);
	}		
}

BOOL CSetupDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	if(setting==NULL)
	{
		return TRUE;
	}

	CString s;
	s.Format("%d", setting->delayBeforeBinsize);
	GetDlgItem(IDC_BINSIZE_DELAY)->SetWindowText(s);
	
	if(setting->boostBaudIndex >= BOOST_BAUDIDX_BASE)
	{
		((CComboBox*)GetDlgItem(IDC_BOOST_BAUDRATE))->SetCurSel(setting->boostBaudIndex - BOOST_BAUDIDX_BASE);
	}
	((CButton*)GetDlgItem(IDC_AUTO_QUERY_VERSION))->SetCheck(setting->autoQueryVersion);
	((CButton*)GetDlgItem(IDC_BOOST_EPHEMERIS))->SetCheck(setting->boostEphemeris);
	((CButton*)GetDlgItem(IDC_CHECK_NMEA_ERR))->SetCheck(setting->checkNmeaError);
	((CButton*)GetDlgItem(IDC_DOWNLOAD_TEST))->SetCheck(setting->downloadTesting);
	((CButton*)GetDlgItem(IDC_ROM_INTERNAL))->SetCheck(setting->downloadRomInternal);
	((CButton*)GetDlgItem(IDC_ENABLE_LOG))->SetCheck(setting->responseLog);
	((CEdit*)GetDlgItem(IDC_LOG_PATH))->SetWindowText(setting->responseLogPath);
	((CButton*)GetDlgItem(IDC_SPY_CENTER))->SetCheck(setting->specifyCenter);
	((CButton*)GetDlgItem(IDC_SPY_ALT))->SetCheck(setting->specifyCenterAlt);

	s.Format("%12.9lf", setting->scatterCenterLon);
	GetDlgItem(IDC_LON)->SetWindowText(s);
	s.Format("%12.9lf", setting->scatterCenterLat);
	GetDlgItem(IDC_LAT)->SetWindowText(s);
	s.Format("%7.2lf", setting->scatterCenterAlt);
	GetDlgItem(IDC_ALT)->SetWindowText(s);
	s.Format("%d", setting->defaultTimeout);
	GetDlgItem(IDC_TIMEOUT)->SetWindowText(s);
	s.Format("%d", setting->scatterCount);
	GetDlgItem(IDC_SCATTER_COUNT)->SetWindowText(s);

	if(setting->recentScatterCenter.GetCount())
	{
		CComboBox *lonlat = (CComboBox *)GetDlgItem(IDC_LONLAT);
		for(int i = setting->recentScatterCenter.GetCount() - 1; i >= 0 ; --i)
		{
			lonlat->AddString(setting->recentScatterCenter[i]);
		}
		lonlat->SetWindowText(setting->recentScatterCenter[setting->recentScatterCenter.GetCount() - 1]);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
}
