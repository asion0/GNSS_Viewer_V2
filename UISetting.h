#pragma once

class UISetting
{
public:
#if FLOAT_SNR
  UISetting(COLORREF, COLORREF, COLORREF, COLORREF, COLORREF, COLORREF, 
		COLORREF, COLORREF, COLORREF, COLORREF, UINT, UINT, 
    int idFontSize = 17, int barFontSize = 12);
#else
  UISetting(COLORREF, COLORREF, COLORREF, COLORREF, COLORREF, COLORREF, 
		COLORREF, COLORREF, COLORREF, COLORREF, UINT, UINT, 
    int idFontSize = 17, int barFontSize = 16);
#endif
	//UISetting(void);
	~UISetting(void);

public:
	const COLORREF panelBkColor;			//pink green
	const COLORREF panelLineColor;			//pink green
	const COLORREF inUseSnrBarPenColor;		//Gray
	const COLORREF noUseSnrBarPenColor;		//Green
	const COLORREF inUseSnrBarBrushColor;	//Purple
	const COLORREF noUseSnrBarBrushColor;	//White
	const COLORREF inUseBarTextColor;		//White
	const COLORREF noUseBarTextColor;		//Green
	const COLORREF inUseIcoTextColor;		//White
	const COLORREF noUseIcoTextColor;		//Green
	const UINT inUseSateBmpId;
	const UINT noUseSateBmpId;
	CFont idFont;
	CFont barFont;

	CBrush panelBkBrush;
	CPen panelLinePen;
	CPen inUseSnrBarPen;
	CBrush inUseSnrBarBrush;
	CPen noUseSnrBarPen;
	CBrush noUseSnrBarBrush;
};

extern UISetting gpUI;
extern UISetting glUI;
extern UISetting bdUI;
extern UISetting gaUI;
extern UISetting bdl1UI;
extern UISetting bdl2UI;
extern UISetting gpl1UI;
extern UISetting gpl2UI;
extern UISetting gll1UI;
extern UISetting gll2UI;

