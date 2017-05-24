// PanelBackground.cpp 
//

#include "stdafx.h"
#include "PanelBackground.h"


// CPanelBackground

IMPLEMENT_DYNAMIC(CPanelBackground, CStatic)

CPanelBackground::CPanelBackground()
{

}

CPanelBackground::~CPanelBackground()
{
}

BEGIN_MESSAGE_MAP(CPanelBackground, CStatic)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
END_MESSAGE_MAP()



// CPanelBackground 
BOOL CPanelBackground::OnEraseBkgnd(CDC* pDC)
{
	CRect rcClient;
	GetClientRect(rcClient);
	CBrush bgBrush(g_panelBkColor);
	pDC->FillRect(rcClient, &bgBrush);
	bgBrush.DeleteObject();
	return CStatic::OnEraseBkgnd(pDC);
}

void CPanelBackground::OnPaint()
{
	CPaintDC dc(this); // device context for painting

  CRect rcClient;
	GetClientRect(rcClient);
	CPen penGray(PS_SOLID, 1, RGB(150,150,150));
	CPen *oldPen = dc.SelectObject(&penGray);

	dc.MoveTo(0, 0);
	dc.LineTo(0, rcClient.bottom - 1);
	dc.LineTo(rcClient.right - 1, rcClient.bottom - 1);
	dc.LineTo(rcClient.right - 1, 0);
	dc.LineTo(0, 0);

	dc.SelectObject(oldPen);	
	penGray.DeleteObject();
}
