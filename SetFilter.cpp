// SetFilter.cpp : 實作檔
//

#include "stdafx.h"
#include "GPS.h"
#include "SetFilter.h"


// CSetFilter 對話方塊

IMPLEMENT_DYNAMIC(CSetFilter, CDialog)
CSetFilter::CSetFilter(CWnd* pParent /*=NULL*/)
	: CDialog(CSetFilter::IDD, pParent)
{
}

CSetFilter::~CSetFilter()
{
}

void CSetFilter::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CSetFilter, CDialog)
END_MESSAGE_MAP()


// CSetFilter 訊息處理常式
