// Ephems.cpp : 實作檔
//

#include "stdafx.h"
#include "GPS.h"
#include "Ephems.h"
#include "GPSDlg.h"


U08 g_SV = 0;
bool g_doesNotChangeBaudrate = false;
UINT GetEphmsThread(LPVOID pParam)
{
	CGetEphemerisDlg::EphemerisType ephType = (CGetEphemerisDlg::EphemerisType)(int)pParam;
	switch(ephType)
	{
	case CGetEphemerisDlg::GpsEphemeris:
		CGPSDlg::gpsDlg->GetEphms(g_SV, FALSE);	
		break;
	case CGetEphemerisDlg::GlonassEphemeris:
		CGPSDlg::gpsDlg->GetGlonassEphms(g_SV, FALSE);	
		break;
	case CGetEphemerisDlg::BeidouEphemeris:
		CGPSDlg::gpsDlg->GetBeidouEphms(g_SV, FALSE);	
		//ASSERT(FALSE);
		break;
	case CGetEphemerisDlg::GalileoEphemeris:
		ASSERT(FALSE);
		break;
	default:
		ASSERT(FALSE);
		break;
	}
	return 0;
}
// CGetEphemerisDlg 對話方塊

IMPLEMENT_DYNAMIC(CGetEphemerisDlg, CDialog)
CGetEphemerisDlg::CGetEphemerisDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_GETEPHEMERIS, pParent)
{
	//Default is GPS Ephems
	ephType = GpsEphemeris;
}

CGetEphemerisDlg::~CGetEphemerisDlg()
{

}

void CGetEphemerisDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SV, m_ephems);
	DDX_Control(pDX, IDC_EDIT1, m_file);
}


BEGIN_MESSAGE_MAP(CGetEphemerisDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, OnBnClickedBrowse)
END_MESSAGE_MAP()


// CGetEphemerisDlg 訊息處理常式

BOOL CGetEphemerisDlg::OnInitDialog()
{
	CDialog::OnInitDialog();	

	CString temp;
	m_ephems.ResetContent();
	m_ephems.AddString("All SVs");
	int MaxSV[] = {32, 24, 37, 24};
	int maxSv = MaxSV[(int)ephType];
	for(int i=0; i<maxSv; i++)
	{
		temp.Format("SV #%d", i+1);
		m_ephems.AddString(temp);
	}
	m_ephems.SetCurSel(0);

	CString desktopPath;
	::SHGetSpecialFolderPath(NULL, desktopPath.GetBuffer(MyMaxPath), CSIDL_DESKTOP, FALSE);
	desktopPath.ReleaseBuffer();
	desktopPath += "\\";
	m_fileName = desktopPath + GetEphemerisNmae();
	m_file.SetWindowText(m_fileName);
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CGetEphemerisDlg::OnBnClickedOk()
{
	m_file.GetWindowText(m_fileName);

	if(m_fileName.IsEmpty())
	{
		AfxMessageBox("Please create a new file to save Ephemeris data!");
		return;
	}

	if(Utility::IsFileExist(m_fileName))
	{
		AfxMessageBox("File already exist!");
		return;
	}

	CFileException ef;
	try
	{				
		if(!CGPSDlg::gpsDlg->m_ephmsFile.Open(m_fileName, CFile::modeReadWrite|CFile::modeCreate, &ef))
		{
			//CGPSDlg::gpsDlg->m_ephmsFile.Close();
			ef.ReportError();
			return;
		}		
	}
	catch(CFileException *fe)
	{
		fe->ReportError();
		fe->Delete();
		return;
	}		
	g_SV = m_ephems.GetCurSel(); 
	//g_doesNotChangeBaudrate = !g_setting.boostEphemeris;

	AfxBeginThread(GetEphmsThread, (void*)ephType);
	OnOK();
}

CString CGetEphemerisDlg::GetEphemerisNmae()
{
	CString f;
	CTime t = CTime::GetCurrentTime();
	f.Format("NMEA%02d-%02d-%02d_%02d%02d%02d.txt", t.GetYear(), t.GetMonth(), t.GetDay(),
		t.GetHour(), t.GetMinute(), t.GetSecond());

	switch(ephType)
	{
	case GpsEphemeris:
		f.Format("GPS_Ephemeris%02d-%02d-%02d_%02d%02d%02d.log", t.GetYear(), t.GetMonth(), t.GetDay(),
			t.GetHour(), t.GetMinute(), t.GetSecond());
		break;
	case GlonassEphemeris:
		f.Format("Glonass_Ephemeris%02d-%02d-%02d_%02d%02d%02d.log", t.GetYear(), t.GetMonth(), t.GetDay(),
			t.GetHour(), t.GetMinute(), t.GetSecond());
		break;
	case BeidouEphemeris:
		f.Format("Beidou_Ephemeris%02d-%02d-%02d_%02d%02d%02d.log", t.GetYear(), t.GetMonth(), t.GetDay(),
			t.GetHour(), t.GetMinute(), t.GetSecond());
		break;
	case GalileoEphemeris:
		f.Format("Galileo_Ephemeris%02d-%02d-%02d_%02d%02d%02d.log", t.GetYear(), t.GetMonth(), t.GetDay(),
			t.GetHour(), t.GetMinute(), t.GetSecond());
		break;
	default:
		ASSERT(FALSE);
		break;
	}
	return f;
}

void CGetEphemerisDlg::OnBnClickedBrowse()
{	
	CString f;
	m_file.GetWindowText(f);

	CFileDialog dlgFile(FALSE, _T("log"), f, 
		OFN_HIDEREADONLY, _T("ALL Files (*.*)|*.*||"), this);
	
  	INT_PTR nResult = dlgFile.DoModal();
	m_fileName = dlgFile.GetPathName();
	if(nResult == IDOK)
	{
		m_file.SetWindowText(m_fileName);
	}
}
