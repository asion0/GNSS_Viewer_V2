// GetGNSSEphemeris.cpp : 實作檔
//

#include "stdafx.h"
#include "GPS.h"
#include "GetGNSSEphemeris.h"
#include "GPSDlg.h"

CGetGNSSEphemeris *pDlg_this;
U08 SV_GPS,SV_Glonass;
// CGetGNSSEphemeris 對話方塊

IMPLEMENT_DYNAMIC(CGetGNSSEphemeris, CDialog)

CGetGNSSEphemeris::CGetGNSSEphemeris(CWnd* pParent /*=NULL*/)
	: CDialog(CGetGNSSEphemeris::IDD, pParent)
{
	isAlmanac = 0;
}

CGetGNSSEphemeris::~CGetGNSSEphemeris()
{
}

void CGetGNSSEphemeris::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX,IDC_FILE_GPS,m_file_gps);
	DDX_Control(pDX,IDC_FILE_GLONASS,m_file_glonass);

	DDX_Control(pDX,IDC_SV_GPS,m_sv_gps);
	DDX_Control(pDX,IDC_SV_GLONASS,m_sv_glonass);
}


BEGIN_MESSAGE_MAP(CGetGNSSEphemeris, CDialog)
	ON_BN_CLICKED(IDOK, &CGetGNSSEphemeris::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BTN_BROWSE_GPS, &CGetGNSSEphemeris::OnBnClickedBtnBrowseGps)
	ON_BN_CLICKED(IDC_BTN_BROWSE_GLONASS, &CGetGNSSEphemeris::OnBnClickedBtnBrowseGlonass)
END_MESSAGE_MAP()


// CGetGNSSEphemeris 訊息處理常式

UINT GetEphmsAllThread(LPVOID pParam)
{	
	CString glonass_file = pDlg_this->Glonass_fileName;
	U08 isAlmanac = pDlg_this->isAlmanac;
	CFileException ef;
	
	//pDlg_this->GPS_fileName.ReleaseBuffer();	
	if (pDlg_this->isAlmanac)
	{
#if TMP_ALMANAC
		CGPSDlg::gpsDlg->GetAlmanac_tmp();
#else
#if NEW_ALMANAC
		CGPSDlg::gpsDlg->GetAlmanac_new(pDlg_this->GPS_fileName,SV_GPS,TRUE);
#else
		CGPSDlg::gpsDlg->GetAlmanac();
#endif
#endif
	}else
	{
		
		try
		{				
			if(!CGPSDlg::gpsDlg->m_ephmsFile.Open(pDlg_this->GPS_fileName,CFile::modeReadWrite|CFile::modeCreate,&ef))
			{
				//CGPSDlg::gpsDlg->m_ephmsFile.Close();
				ef.ReportError();
				return -1;
			}		
		}
		catch(CFileException *fe)
		{
			fe->ReportError();
			fe->Delete();
			return -1;
		}	
		CGPSDlg::gpsDlg->GetEphms(SV_GPS, TRUE);	
	}
	


	//pDlg_this->Glonass_fileName.ReleaseBuffer();	
	if (isAlmanac)
	{
		CGPSDlg::gpsDlg->GetGlonassAlmanac(glonass_file,SV_Glonass,TRUE);	
	}
	else
	{
		
		try
		{				
			if(!CGPSDlg::gpsDlg->m_ephmsFile.Open(glonass_file,CFile::modeReadWrite|CFile::modeCreate,&ef))
			{
				//CGPSDlg::gpsDlg->m_ephmsFile.Close();
				ef.ReportError();
				return -1;
			}		
		}
		catch(CFileException *fe)
		{
			fe->ReportError();
			fe->Delete();
			return -1;
		}		
		CGPSDlg::gpsDlg->GetGlonassEphms(SV_Glonass,TRUE);	
	}
//	AfxEndThread(0);
	return 0;
}


void CGetGNSSEphemeris::OnBnClickedOk()
{
	if(GPS_fileName=="" || Glonass_fileName==""){AfxMessageBox("Please create a new file to save Ephemeris data!");return;}


	SV_GPS = m_sv_gps.GetCurSel(); 
	SV_Glonass = m_sv_glonass.GetCurSel(); 

	AfxBeginThread(GetEphmsAllThread,0);

	OnOK();
}

void CGetGNSSEphemeris::OnBnClickedBtnBrowseGps()
{
	if (isAlmanac)
		GPS_fileName = "GPS_Almanac.log";
	else
		GPS_fileName = "GPS_Ephemeris.log";
		
	CFileDialog dlgFile(false, _T("log"), GPS_fileName, OFN_HIDEREADONLY, _T("ALL Files (*.*)|*.*||"), this);
	INT_PTR nResult = dlgFile.DoModal();
	GPS_fileName = dlgFile.GetPathName();
	if(nResult == IDOK)
	{
		m_file_gps.SetWindowText(GPS_fileName);
	}
}

void CGetGNSSEphemeris::OnBnClickedBtnBrowseGlonass()
{
	if (isAlmanac)
		Glonass_fileName = "Glonass_Almanac.log";
	else
		Glonass_fileName = "Glonass_Ephemeris.log";
	CFileDialog dlgFile(false, _T("log"), Glonass_fileName, OFN_HIDEREADONLY, _T("ALL Files (*.*)|*.*||"), this);
	INT_PTR nResult = dlgFile.DoModal();
	Glonass_fileName = dlgFile.GetPathName();
	if(nResult == IDOK)
	{
		m_file_glonass.SetWindowText(Glonass_fileName);
	}
}

BOOL CGetGNSSEphemeris::OnInitDialog()
{
	CDialog::OnInitDialog();

	pDlg_this = this;

	if (isAlmanac)
		this->SetWindowText("Get GPS+Glonass Almanac");
	else
		this->SetWindowText("Get GPS+Glonass Ephemeris");
	

	CString temp;
	m_sv_gps.ResetContent();
	m_sv_gps.AddString("All SVs");
	for(int i=0;i<24;i++)
	{
		temp.Format("SV #%d",i+1);
		m_sv_gps.AddString(temp);
	}
	m_sv_gps.SetCurSel(0);

	m_sv_glonass.ResetContent();
	m_sv_glonass.AddString("All SVs");
	for(int i=0;i<32;i++)
	{
		temp.Format("SV #%d",i+1);
		m_sv_glonass.AddString(temp);
	}
	m_sv_glonass.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX 屬性頁應傳回 FALSE
}
