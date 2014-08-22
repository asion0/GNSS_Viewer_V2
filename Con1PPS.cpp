// Con1PPS.cpp : 實作檔
//

#include "stdafx.h"
#include "GPS.h"
#include "Con1PPS.h"


// CCon1PPS 對話方塊

IMPLEMENT_DYNAMIC(CCon1PPS, CDialog)
CCon1PPS::CCon1PPS(CWnd* pParent /*=NULL*/)
	: CDialog(CCon1PPS::IDD, pParent)
{
	_1ppsmode = _1ppsattr = 0;
}

CCon1PPS::~CCon1PPS()
{
}

void CCon1PPS::DoDataExchange(CDataExchange* pDX)
{
	DDX_CBIndex(pDX,IDC_1PPS_MODE,_1ppsmode);
	DDX_CBIndex(pDX,IDC_1PPS_ATTR,_1ppsattr);
	DDX_Control(pDX,IDC_1PPS_MODE,m_1pps);
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CCon1PPS, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CCon1PPS 訊息處理常式

BOOL CCon1PPS::OnInitDialog()
{
	CDialog::OnInitDialog();
	//Off;On when 3D fix;On when 1 SV;
	m_1pps.Clear();


	m_1pps.AddString("Not sync to UTC second");
	m_1pps.AddString("Sync to UTC second when 3D fix");

	m_1pps.SetCurSel(0);
	// TODO:  在此加入額外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX 屬性頁應傳回 FALSE
}

void CCon1PPS::OnBnClickedOk()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	OnOK();
}
