// SetTimeCorrections.cpp 
//

#include "stdafx.h"
#include "GPS.h"
#include "SetTimeCorrections.h"


// CSetTimeCorrections 

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


// CSetTimeCorrections 

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
	return TRUE;  // return TRUE unless you set the focus to a control
}
