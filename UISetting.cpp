#include "StdAfx.h"
#include "UISetting.h"
#include "resource.h"

UISetting::UISetting(COLORREF bk, COLORREF line, COLORREF inPen, COLORREF noPen, 
					 COLORREF inBru, COLORREF noBru, COLORREF inTxt, 
					 COLORREF noTxt, COLORREF inIcoTxt, COLORREF noIcoTxt, 
					 UINT inBmp, UINT noBmp, const LOGFONT* lpLogFont) :
	panelBkColor(bk),
	panelLineColor(line),
	inUseSnrBarPenColor(inPen),
	noUseSnrBarPenColor(noPen),
	inUseSnrBarBrushColor(inBru),
	noUseSnrBarBrushColor(noBru),
	inUseBarTextColor(inTxt),
	noUseBarTextColor(noTxt),
	inUseIcoTextColor(inIcoTxt),
	noUseIcoTextColor(noIcoTxt),
	panelBkBrush(bk),
	panelLinePen(PS_SOLID, 1, line),
	inUseSateBmpId(inBmp),
	noUseSateBmpId(noBmp),
	inUseSnrBarPen(PS_SOLID, 1, inPen),
	inUseSnrBarBrush(inBru),
	noUseSnrBarPen(PS_SOLID, 1, noPen),
	noUseSnrBarBrush(noBru)
{
	idFont.CreateFont(
		17, //   LONG      lfHeight;
		0, //   LONG      lfWidth;
		0, //   LONG      lfEscapement;
		0, //   LONG      lfOrientation;
		400,//  LONG      lfWeight;
		0, //   BYTE      lfItalic;
		0, //   BYTE      lfUnderline;
		0, //    BYTE      lfStrikeOut;
		DEFAULT_CHARSET, //    BYTE      lfCharSet;
		OUT_DEFAULT_PRECIS, //    BYTE      lfOutPrecision;
		CLIP_DEFAULT_PRECIS, //    BYTE      lfClipPrecision;
		DEFAULT_QUALITY, //    BYTE      lfQuality;
		DEFAULT_PITCH, //    BYTE      lfPitchAndFamily;
		"Impact" //    WCHAR     lfFaceName[LF_FACESIZE];
		);
	barFont.CreateFont(
		16, //   LONG      lfHeight;
		0, //   LONG      lfWidth;
		0, //   LONG      lfEscapement;
		0, //   LONG      lfOrientation;
		400,//  LONG      lfWeight;
		0, //   BYTE      lfItalic;
		0, //   BYTE      lfUnderline;
		0, //    BYTE      lfStrikeOut;
		DEFAULT_CHARSET, //    BYTE      lfCharSet;
		OUT_DEFAULT_PRECIS, //    BYTE      lfOutPrecision;
		CLIP_DEFAULT_PRECIS, //    BYTE      lfClipPrecision;
		DEFAULT_QUALITY, //    BYTE      lfQuality;
		DEFAULT_PITCH, //    BYTE      lfPitchAndFamily;
		"Arial" //    WCHAR     lfFaceName[LF_FACESIZE];
		);
}
	
UISetting::~UISetting(void)
{
}


UISetting gpUI(RGB(255, 255, 204), RGB(150, 150, 150),
			   RGB(200, 200, 200), RGB( 26, 144, 255), 
			   RGB( 26, 144, 255), RGB(255, 255, 255), 
			   RGB(255, 255, 255), RGB(128, 128, 128), //inTxt, noTxt
			   RGB(255, 255, 255), RGB( 61, 179, 255), //inIcoTxt, noIcoTxt, 
			   IDB_GP_ACT, IDB_GP_DIS, NULL);

UISetting glUI(RGB(255, 255, 204), RGB(150, 150, 150), 
			   RGB(200, 200, 200), RGB(156, 102, 204), 
			   RGB(156, 102, 204), RGB(255, 255, 255), 
			   RGB(255, 255, 255), RGB(128, 128, 128),
			   RGB(255, 255, 255), RGB(195, 128, 255), //inIcoTxt, noIcoTxt, 
			   IDB_GL_ACT, IDB_GL_DIS, NULL);

UISetting bdUI(RGB(204, 255, 255), RGB(150, 150, 150), 
			   RGB(200, 200, 200), RGB(255, 153,   0), 
			   RGB(255, 153,   0), RGB(255, 255, 255), 
			   RGB(255, 255, 255), RGB(128, 128, 128),
			   RGB(255, 255, 255), RGB(255, 153,   0), 
			   IDB_BD_ACT, IDB_BD_DIS, NULL);

UISetting gaUI(RGB(255, 217, 217), RGB(150, 150, 150), 
			   RGB(200, 200, 200), RGB( 90, 190, 45), 
			   RGB( 90, 190, 45), RGB(255, 255, 255), 
			   RGB(255, 255, 255), RGB(128, 128, 128),
			   RGB(255, 255, 255), RGB( 90, 190, 45), 
			   IDB_GA_ACT, IDB_GA_DIS, NULL);