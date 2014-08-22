// PanelBackground.cpp : 實作檔
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



// CPanelBackground 訊息處理常式



BOOL CPanelBackground::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
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
	// TODO: 在此加入您的訊息處理常式程式碼
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
