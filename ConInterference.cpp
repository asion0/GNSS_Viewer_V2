// ConInterference.cpp : 實作檔
//

#include "stdafx.h"
#include "GPS.h"
#include "ConInterference.h"


// CConInterference 對話方塊

IMPLEMENT_DYNAMIC(CConInterference, CDialog)

CConInterference::CConInterference(CWnd* pParent /*=NULL*/)
	: CDialog(CConInterference::IDD, pParent)
{
	mode = 0;
}

CConInterference::~CConInterference()
{
}

void CConInterference::DoDataExchange(CDataExchange* pDX)
{

	DDX_CBIndex(pDX,IDC_INTERFERENCE_MODE,mode);
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CConInterference, CDialog)
END_MESSAGE_MAP()


// CConInterference 訊息處理常式

BOOL CConInterference::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此加入額外的初始化

	//UpdateData();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX 屬性頁應傳回 FALSE
}
