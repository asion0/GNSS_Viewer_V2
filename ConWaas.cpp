// ConWaas.cpp 
//

#include "stdafx.h"
#include "GPS.h"
#include "ConWaas.h"

// CConWaas 

IMPLEMENT_DYNAMIC(CConWaas, CDialog)
CConWaas::CConWaas(CWnd* pParent /*=NULL*/)
	: CDialog(CConWaas::IDD, pParent)
{
	waas = 0;
	attribute = 0;
}

CConWaas::~CConWaas()
{
}

void CConWaas::DoDataExchange(CDataExchange* pDX)
{
	DDX_CBIndex(pDX,IDC_WAAS,waas);
	DDX_CBIndex(pDX,IDC_WAAS_ATTR,attribute);
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CConWaas, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
END_MESSAGE_MAP()


// CConWaas 

void CConWaas::OnBnClickedOk()
{
	OnOK();
}
