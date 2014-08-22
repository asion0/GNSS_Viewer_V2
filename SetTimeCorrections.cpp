// SetTimeCorrections.cpp : 實作檔
//

#include "stdafx.h"
#include "GPS.h"
#include "SetTimeCorrections.h"


// CSetTimeCorrections 對話方塊

IMPLEMENT_DYNAMIC(CSetTimeCorrections, CDialog)

CSetTimeCorrections::CSetTimeCorrections(CWnd* pParent /*=NULL*/)
	: CDialog(CSetTimeCorrections::IDD, pParent)
{
	_attr = 0;
}

CSetTimeCorrections::~CSetTimeCorrections()
{
}

void CSetTimeCorrections::DoDataExchange(CDataExchange* pDX)
{
	DDX_Text(pDX,IDC_FILE,_filename);
	DDX_CBIndex(pDX,IDC_ATTR,_attr);
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSetTimeCorrections, CDialog)
	ON_BN_CLICKED(IDC_BROWSE, &CSetTimeCorrections::OnBnClickedBrowse)
END_MESSAGE_MAP()


// CSetTimeCorrections 訊息處理常式

void CSetTimeCorrections::OnBnClickedBrowse()
{
	CFileDialog dlgFile(false, _T("log"), "GlonassTimeCorrections.log", OFN_HIDEREADONLY, _T("ALL Files (*.*)|*.*||"), this);
	if(dlgFile.DoModal() == IDOK)
	{
		_filename = dlgFile.GetPathName();

		UpdateData(FALSE);
	}
}

BOOL CSetTimeCorrections::OnInitDialog()
{
	CDialog::OnInitDialog();

	
	//UpdateData(TRUE);
	// TODO:  在此加入額外的初始化

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX 屬性頁應傳回 FALSE
}
