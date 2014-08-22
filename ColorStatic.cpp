// ColorStatic.cpp : implementation file
//

#include "stdafx.h"
#include "ColorStatic.h"


// CColorStatic

IMPLEMENT_DYNAMIC(CColorStatic, CStatic)

CColorStatic::CColorStatic()
{
	m_pBgBrush = NULL;
	m_pTextBrush = NULL;
	m_crTextColor = 0;
	m_crBgColor = 1;
}

CColorStatic::~CColorStatic()
{
	if(m_pTextBrush)
		delete m_pTextBrush;
	if(m_pBgBrush)
		delete m_pBgBrush;
}


BEGIN_MESSAGE_MAP(CColorStatic, CStatic)
	//ON_WM_PAINT()
	//ON_MESSAGE(WM_SETTEXT,OnSetText)
	//ON_WM_ERASEBKGND()
	ON_WM_CTLCOLOR_REFLECT()
END_MESSAGE_MAP()

// CColorStatic message handlers
void CColorStatic::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	CRect rcText, rcClient;
	CString strText;
	CFont* pOldFont;
	CFont* m_pTextFont = this->GetFont();
	pOldFont = dc.SelectObject(m_pTextFont);

	GetClientRect(&rcClient);
	GetWindowText(strText);
	rcText = rcClient;

//	LONG uStyle = GetWindowLong(this->GetSafeHwnd(), GWL_STYLE);
	DWORD uStyle = this->GetStyle();
	UINT uDrawTextFormat = DT_CALCRECT;

	if(SS_NOPREFIX == (uStyle & SS_NOPREFIX))
	{
		uDrawTextFormat |= DT_NOPREFIX;
	}

	int height = dc.DrawText(strText, &rcText, uDrawTextFormat);
	int width = rcText.right - rcText.left;

	//BS_LEFT + BS_RIGHT is BS_CENTER
	if(SS_CENTER == (uStyle & SS_CENTER))
	{
		rcText.left = (rcClient.Width() - width) / 2;
		rcText.right = rcText.left + width;
	}
	else if(SS_LEFT == (uStyle & SS_LEFT))
	{
		rcText.left = 0;
		rcText.right = rcText.left + width;
	}
	else if(SS_RIGHT == (uStyle & SS_RIGHT))
	{
		rcText.right = rcClient.Width();
		rcText.left = rcText.right - width;
	}
	else	//Default is Center
	{
		rcText.left = (rcClient.Width() - width) / 2;
		rcText.right = rcText.left + width;
	}

	//VCENTER
	rcText.top = (rcClient.Height() - height) / 2;
	rcText.bottom = rcText.top + height;
	uDrawTextFormat = uDrawTextFormat & ~DT_CALCRECT;

	//Center the rcText
	if(m_pBgBrush)
	{
		dc.FillRect(rcClient, m_pBgBrush);
	}
	dc.SetBkMode(TRANSPARENT);
	dc.SetTextColor(m_crTextColor);

	dc.DrawText(strText, &rcText, uDrawTextFormat);
}

LRESULT CColorStatic::OnSetText(WPARAM wParam,LPARAM lParam)
{
   LRESULT Result = Default();
   CRect Rect;
   GetWindowRect(&Rect);
   GetParent()->ScreenToClient(&Rect);
   GetParent()->InvalidateRect(&Rect);
   GetParent()->UpdateWindow();
   return Result;
}

BOOL CColorStatic::OnEraseBkgnd(CDC* pDC)
{
	CRect rcClient;
	GetUpdateRect(&rcClient);			  
	pDC->FillSolidRect(rcClient, RGB(255,255,255));	
	return FALSE;	
}

HBRUSH CColorStatic::CtlColor(CDC* pDC, UINT nCtlColor)
{
	// TODO:  在此變更 DC 的任何屬性
	pDC->SetBkMode(TRANSPARENT);
	//pDC->SetTextColor(RGB(0, 0, 0));
	pDC->SetTextColor(m_crTextColor);
	//return (HBRUSH)GetStockObject(NULL_BRUSH);
	return (m_pBgBrush) ? (HBRUSH)m_pBgBrush->GetSafeHandle() : (HBRUSH)GetStockObject(WHITE_BRUSH);
	// TODO:  如果不應該呼叫父處理常式，則傳回非 NULL 筆刷
	return NULL;
}
