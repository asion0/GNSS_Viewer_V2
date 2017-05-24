// Config_Proprietary.cpp 
//

#include "stdafx.h"
#include "GPS.h"
#include "Config_Proprietary.h"


// CConfig_Proprietary 

IMPLEMENT_DYNAMIC(CConfig_Proprietary, CDialog)

CConfig_Proprietary::CConfig_Proprietary(CWnd* pParent /*=NULL*/)
	: CDialog(CConfig_Proprietary::IDD, pParent)
{
	enable = attr = 0;
}

CConfig_Proprietary::~CConfig_Proprietary()
{
}

void CConfig_Proprietary::DoDataExchange(CDataExchange* pDX)
{
	DDX_CBIndex(pDX,IDC_PROPRI_MSG,enable);
	DDX_CBIndex(pDX,IDC_PROPRI_ATTR,attr);
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CConfig_Proprietary, CDialog)
END_MESSAGE_MAP()


// CConfig_Proprietary 
