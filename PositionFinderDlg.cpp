// PositionFinderDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "GPS.h"
#include "PositionFinderDlg.h"

// CPositionFinderDlg 對話方塊

IMPLEMENT_DYNCREATE(CPositionFinderDlg, CDialog)

CPositionFinderDlg::CPositionFinderDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPositionFinderDlg::IDD, pParent)
{
	unit_sel = 0;
	attr = 0;
}

CPositionFinderDlg::~CPositionFinderDlg()
{
}

void CPositionFinderDlg::DoDataExchange(CDataExchange* pDX)
{
	//DDX_Text(pDX,IDC_POS_LAT,lat);
	//DDX_Text(pDX,IDC_POS_LON,lon);

	DDX_Control(pDX,IDC_POS_LAT,m_lat);
	DDX_Control(pDX,IDC_POS_LON,m_lon);

	DDX_CBIndex(pDX,IDC_DIS_UNIT,unit_sel);
	DDX_CBIndex(pDX,IDC_POS_ATTRI,attr);
	CDialog::DoDataExchange(pDX);
}

BOOL CPositionFinderDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	CString tmp;

	tmp.Format("%.6f",lat);
	m_lat.SetWindowText(tmp);
	tmp.Format("%.6f",lon);
	m_lon.SetWindowText(tmp);
	return TRUE;  // 傳回 TRUE，除非您對控制項設定焦點
}

BEGIN_MESSAGE_MAP(CPositionFinderDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


void CPositionFinderDlg::OnBnClickedOk()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	CString tmp;
	m_lat.GetWindowText(tmp);
	lat = atof(tmp);

	m_lon.GetWindowText(tmp);
	lon = atof(tmp);

	OnOK();
}
