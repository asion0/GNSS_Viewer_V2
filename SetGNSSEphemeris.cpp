// SetGNSSEphemeris.cpp : 實作檔
//

#include "stdafx.h"
#include "GPS.h"
#include "SetGNSSEphemeris.h"
#include "GPSDlg.h"

// CSetGNSSEphemeris 對話方塊
IMPLEMENT_DYNAMIC(CSetGNSSEphemeris, CDialog)

CSetGNSSEphemeris *pDlg_this;

UINT SetEphmsAllThread(LPVOID pParam)
{	
	CString glonass_file = pDlg_this->Glonass_fileName;
	U08 isAlmanac = pDlg_this->isAlmanac;

	CFileException ef;
	try
	{				
		if(!CGPSDlg::gpsDlg->m_ephmsFile.Open(pDlg_this->GPS_fileName,CFile::modeRead,&ef))
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
	//pDlg_this->GPS_fileName.ReleaseBuffer();	
	if (pDlg_this->isAlmanac)
	{
		CGPSDlg::gpsDlg->SetAlmanac(TRUE);	
	}else
		CGPSDlg::gpsDlg->SetEphms(TRUE);	

	try
	{				
		if(!CGPSDlg::gpsDlg->m_ephmsFile.Open(glonass_file,CFile::modeRead,&ef))
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
	//pDlg_this->Glonass_fileName.ReleaseBuffer();	
	if (isAlmanac)
		CGPSDlg::gpsDlg->SetGlonassAlmanac(TRUE);	
	else
		CGPSDlg::gpsDlg->SetGlonassEphms(TRUE);
	//AfxEndThread(0);
	return 0;
}

CSetGNSSEphemeris::CSetGNSSEphemeris(CWnd* pParent /*=NULL*/)
	: CDialog(CSetGNSSEphemeris::IDD, pParent)
{
	isAlmanac = 0;
}

CSetGNSSEphemeris::~CSetGNSSEphemeris()
{
}

void CSetGNSSEphemeris::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX,IDC_FILE_GPS,m_file_gps);
	DDX_Control(pDX,IDC_FILE_GLONASS,m_file_glonass);
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSetGNSSEphemeris, CDialog)
	ON_BN_CLICKED(IDC_BTN_BROWSE_GPS, &CSetGNSSEphemeris::OnBnClickedBtnBrowseGps)
	ON_BN_CLICKED(IDC_BTN_BROWSE_GLONASS, &CSetGNSSEphemeris::OnBnClickedBtnBrowseGlonass)
	ON_BN_CLICKED(IDOK, &CSetGNSSEphemeris::OnBnClickedOk)
END_MESSAGE_MAP()


// CSetGNSSEphemeris 訊息處理常式

void CSetGNSSEphemeris::OnBnClickedBtnBrowseGps()
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

void CSetGNSSEphemeris::OnBnClickedBtnBrowseGlonass()
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

void CSetGNSSEphemeris::OnBnClickedOk()
{
	if(GPS_fileName=="" || Glonass_fileName==""){AfxMessageBox("Please select a file to load data!");return;}

	AfxBeginThread(SetEphmsAllThread,0);

	OnOK();
}

BOOL CSetGNSSEphemeris::OnInitDialog()
{
	CDialog::OnInitDialog();

	pDlg_this = this;
	// TODO:  在此加入額外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX 屬性頁應傳回 FALSE
}
