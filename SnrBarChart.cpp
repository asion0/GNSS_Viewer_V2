#include "StdAfx.h"
#include "SnrBarChart.h"
#include "MaskedBitmap.h"
#include "GPSDlg.h"
#include "UISetting.h"
#include <algorithm>


static const int GpsIdStart = 1;
static const int GpsIdEnd = 51;
#if (_V8_SUPPORT)
 static const int GpsQZSSIdStart = 193;
 static const int GpsQZSSIdEnd = 197;
#else
 static const int GpsQZSSIdStart = 52;
 static const int GpsQZSSIdEnd = 56;
#endif

static const int GnssIdStart = 65;
static const int GnssIdEnd = 88;

static const int MaxCountInRaw = 31;
static const int MaxLevelLineCount = 6;
static const int LevelLineStartY = 50;	//70
static const int LevelLineGapY = 10;
//static const int SecondLineStartY = 74;
//static const char* IdFontName = "Impact";
static const int BarChartIdGap = 2;
static const CSize SnrBar(18, 50);
static const CPoint StartGnssBarChart(7, 52);
static const COLORREF TransparentColor = RGB(255, 255, 255);

CSnrBarChart::CSnrBarChart(void)
{
	gsvData = NULL;
	gsaData = NULL;
	ggaData = NULL;
	sateStatus = NULL;
	SetUISetting(&gpUI);
	customerID = 0;
}

CSnrBarChart::~CSnrBarChart(void)
{

}

BEGIN_MESSAGE_MAP(CSnrBarChart, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CSnrBarChart::OnPaint()
{
	RefreshBarChart();
}

void CSnrBarChart::RefreshBarChart()
{
	CPaintDC dc(this); // device context for painting

	CRect clientRect;
	GetClientRect(clientRect);
	CMemDC memDc(dc, clientRect);

	CreateBarChart(&memDc.GetDC());
	if(gsvData)
	{
		ShowBoxChart(&memDc.GetDC());
	}
}

void CSnrBarChart::DrawBarChartId(CDC *dc, UISetting* s, bool isInUse, 
		int x, int y, int id, bool inCenter)
{
	CxImage img;
	UINT imgId = isInUse ?s->inUseSateBmpId : s->noUseSateBmpId;
	img.LoadResource(FindResource(NULL, MAKEINTRESOURCE(imgId), "PNG"), 
		CXIMAGE_FORMAT_PNG);
	if(!img.IsValid())
	{
		return;
	}

	if(inCenter)
	{
		x -= img.GetWidth() / 2;
		y -= img.GetHeight() / 2;
	}
	img.Draw(dc->GetSafeHdc(), x, y);

	//Draw Satellite ID text.
	CFont* oldFont = dc->SelectObject(&(s->idFont));
	COLORREF cr = (isInUse) ? s->inUseIcoTextColor : s->noUseIcoTextColor;
	dc->SetTextColor(cr);
	CString idText;
	idText.Format("%d", id);
	CRect idTextRect(x, y + 1, x + img.GetWidth(), y + img.GetHeight() + 1);	//Pic image size 20 * 20.
	dc->DrawText(idText, idTextRect, DT_VCENTER | DT_CENTER);
	dc->SelectObject(oldFont);
}

void CSnrBarChart::CreateBarChart(CDC *dc)
{
	//CBrush brushBk(uiSetting->panelBkColor);
	CRect rcClient;
	GetClientRect(rcClient);
	dc->FillRect(&rcClient, &(uiSetting->panelBkBrush));
	//brushBk.DeleteObject();

	//Draw horizontal line
	//CPen penGray(PS_SOLID, 1, uiSetting->panelLineColor);
	CPen *oldPen = dc->SelectObject(&(uiSetting->panelLinePen));
	for (int i=0; i<MaxLevelLineCount; ++i)
	{
		dc->MoveTo(0, LevelLineStartY - i * LevelLineGapY);
		dc->LineTo(rcClient.right, LevelLineStartY - i * LevelLineGapY);
	}
	//Draw frame
	dc->MoveTo(0, 0);
	dc->LineTo(0, rcClient.bottom - 1);
	dc->LineTo(rcClient.right - 1, rcClient.bottom - 1);
	dc->LineTo(rcClient.right - 1, 0);

	//Draw title text
	dc->SetBkMode(TRANSPARENT);
	dc->SelectObject(oldPen);	
//	penGray.DeleteObject();
}

void CSnrBarChart::ShowBoxChart(CDC *dc)
{
	int n = 0;
	DrawSnr(dc, n, uiSetting, gsvData, gsaData, ggaData, sateStatus);
}

int GetNoneZeroCount(Satellite* s)
{
	int count = 0;
	while(s[count].SatelliteID != 0)
	{
		count++;
	}
	return count;
}

void CSnrBarChart::DrawSnr(CDC *dc, int& startId, UISetting* s, GPGSV* gsv, GPGSA* gsa, GPGGA* gga, Satellites* sate)
{
	BITMAP bm = {0};
	bm.bmWidth = bm.bmHeight = 20;
	int cellOffset = BarChartIdGap / 2;

	//Sort the satellite prn. 
	//int sateCount = GetNoneZeroCount(sate);
	int sateCount = sate->GetSateCount();
	if(sateCount==0)
	{
		return;
	}

  sate->Sort();
	//vector< pair<int, int> > sateArray(sateCount);
	//for(int i=0; i<sateCount; ++i)
	//{
	//	sateArray[i] = make_pair(i, sate[i].SatelliteID);
	//}
	//struct sort_pred {
	//	bool operator()(const std::pair<int,int> &left, const std::pair<int,int> &right) 
	//	{ return left.second < right.second; }
	//};
	//std::sort(sateArray.begin(), sateArray.end(), sort_pred());

	CFont* oldFont = dc->SelectObject(&(s->idFont));	
	CBrush* oldBrush = dc->SelectObject(&(s->noUseSnrBarBrush));	
	CPen* oldPen = dc->SelectObject(&(s->noUseSnrBarPen));	

	for(int i=0; i<sateCount; ++i)
	{
		//int arrayIdx = sateArray[i].first;
    const Satellite* pSate = sate->GetSateIndex(i);
		int id = pSate->SatelliteID;

		int dx = 0, dy = 0;
		int index = startId + i;

		if(index < MaxCountInRaw)
		{	
			dx = m_startBarChart.x + index * (BarChartIdGap + bm.bmWidth);
			dy = m_startBarChart.y;
		}
		else
		{
			ASSERT(FALSE);
		}
		dx += cellOffset;

		bool isInUse = !CheckInUse(id, gsa);
#ifdef _NAVIC_CONVERT_
		DrawBarChartId(dc, s, isInUse, dx, dy, 
      (NMEA::GetGNSSSystem(id) == NMEA::Glonass) ? id - 64 : id); 
#else
		DrawBarChartId(dc, s, isInUse, dx, dy, id); 
#endif
#if (FLOAT_SNR)
		//F32 snrValue = sate[arrayIdx].SNR;
		F32 snrValue = pSate->snr[0];
#else
		//U16 snrValue = sate[arrayIdx].SNR;
		U16 snrValue = pSate->snr[0];
#endif
		if(snrValue == INVALIDATE_SNR)
		{
			continue;
		}

		//Draw gray rectangle out line.
		dc->SelectObject(&(s->inUseSnrBarPen));
		dc->SelectObject(&(s->noUseSnrBarBrush));
		CRect barRect(dx + (bm.bmWidth - SnrBar.cx) / 2, dy - SnrBar.cy - 2,
					dx + (bm.bmWidth + SnrBar.cx) / 2, dy - 1);
		dc->Rectangle(barRect);

		//Draw snr bar.
		dc->SelectObject((isInUse) ? &(s->inUseSnrBarBrush) : &(s->noUseSnrBarBrush));
		dc->SelectObject((isInUse) ? &(s->inUseSnrBarPen) : &(s->noUseSnrBarPen));
		barRect.SetRect(dx + (bm.bmWidth - SnrBar.cx) / 2, dy - 1 - (int)snrValue,
			dx + (bm.bmWidth + SnrBar.cx) / 2, dy - 1);
		dc->Rectangle(barRect);

		//Draw snr value text.
		COLORREF cr = (isInUse) ? s->inUseBarTextColor : s->noUseBarTextColor;
		if(isInUse && snrValue < 15)
		{
			cr = RGB(10, 10, 10);
		}
		dc->SetTextColor(cr);
		CString idText;
#if FLOAT_SNR
		idText.Format("%3.1f", snrValue);
#else
		idText.Format("%d", snrValue);
#endif
		barRect.SetRect(dx + (bm.bmWidth - SnrBar.cx) / 2, dy - 1 - SnrBar.cx,
					dx + (bm.bmWidth + SnrBar.cx) / 2, dy - 1);
		dc->SelectObject(&(s->barFont));	
		dc->DrawText(idText, barRect, DT_VCENTER | DT_CENTER);
	} 
	startId += sateCount;

	dc->SelectObject(oldFont);	
	dc->SelectObject(oldBrush);	
	dc->SelectObject(oldPen);	
}

//============================================================================
CSnrBarChartGps::CSnrBarChartGps(void) 
{
	m_startBarChart = StartGnssBarChart;
	m_minId = GpsIdStart;
	m_maxId = GpsIdEnd;
	m_titleText = "GPS";
	SetUISetting(&gpUI);
}

CSnrBarChartGps::~CSnrBarChartGps(void) 
{

}

BEGIN_MESSAGE_MAP(CSnrBarChartGps, CSnrBarChart)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CSnrBarChartGps::OnPaint()
{
  RefreshBarChart();
}

//============================================================================
CSnrBarChartGlonass::CSnrBarChartGlonass(void) 
{
	m_startBarChart = StartGnssBarChart;
	m_minId = GnssIdStart;
	m_maxId = GnssIdEnd;
	m_titleText = "GNSS";
	SetUISetting(&glUI);
}

CSnrBarChartGlonass::~CSnrBarChartGlonass(void) 
{

}

BEGIN_MESSAGE_MAP(CSnrBarChartGlonass, CSnrBarChart)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CSnrBarChartGlonass::OnPaint()
{
	RefreshBarChart();
}

//============================================================================
CSnrBarChartDual::CSnrBarChartDual(void) 
{
	gsvData2 = NULL;
	gsaData2 = NULL;
	ggaData2 = NULL;
	sateStatus2 = NULL;
	m_startBarChart = StartGnssBarChart;
	m_minId = GpsIdStart;
	m_maxId = GpsIdEnd;
#ifdef _NAVIC_CONVERT_
	m_titleText = "GPS / NAVIC";
#else
	m_titleText = "GPS / GNSS";
#endif
	SetUISetting(&gpUI);
	SetUISetting2(&glUI);
}

CSnrBarChartDual::~CSnrBarChartDual(void) 
{

}

BEGIN_MESSAGE_MAP(CSnrBarChartDual, CSnrBarChart)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CSnrBarChartDual::OnPaint()
{
	RefreshBarChart();
}

void CSnrBarChartDual::ShowBoxChart(CDC *dc)
{
	int n = 0;
	DrawSnr(dc, n, uiSetting, gsvData, gsaData, ggaData, sateStatus);
	DrawSnr(dc, n, uiSetting2, gsvData2, gsaData2, ggaData2, sateStatus2);
}
//============================================================================
static const CSize SnrBarL2(14, 50);
static const CPoint StartGnssBarChartL2(5, 52);

CSnrBarChartL2::CSnrBarChartL2() 
{
  Init(0);
}

CSnrBarChartL2::CSnrBarChartL2(int type) 
{
  Init(type);
}

CSnrBarChartL2::~CSnrBarChartL2(void) 
{

}

void CSnrBarChartL2::Init(int t)
{
	m_startBarChart = StartGnssBarChartL2;
	m_minId = 1;
	m_maxId = 31;
	m_titleText = "Beidou";
  if(t == 0)
  {
	  SetUISetting(&gpl1UI);
	  SetUISettingSub(&gpl2UI);
  }
  else if(t == 1)
  {
	  SetUISetting(&bdl1UI);
	  SetUISettingSub(&bdl2UI);
  }
}

BEGIN_MESSAGE_MAP(CSnrBarChartL2, CSnrBarChart)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CSnrBarChartL2::OnPaint()
{
	RefreshBarChart();
}

//#if (FLOAT_SNR)
//F32 CSnrBarChartL2::GetGsvSubSnr(Satellite* s, int id) 
//#else 
//int CSnrBarChartL2::GetGsvSubSnr(Satellite* s, int id) 
//#endif 
//{
//  if(s == NULL)
//    return 0;
//
//  for(int i = 0; i < MAX_SATELLITE; ++i)
//  {
//    if(s[i].SatelliteID == id)
//      return s[i].SNR;
//  }
//  return 0;
//}

void CSnrBarChartL2::ShowBoxChart(CDC *dc)
{
	int n = 0;
	DrawSnr(dc, n, uiSetting, uiSettingSub, gsvData, gsaData, ggaData, sateStatus);
}

void CSnrBarChartL2::DrawSnr(CDC *dc, int& startId, UISetting* s, UISetting* sSub, GPGSV* gsv, GPGSA* gsa, GPGGA* gga, 
                             Satellites* sate)
{
	BITMAP bm = {0};
	bm.bmWidth = bm.bmHeight = 20;
	int cellOffset = BarChartIdGap / 2;

	//Sort the satellite prn. 
  sate->Sort();
  int sateCount = sate->GetSateCount();
	//int sateCount = GetNoneZeroCount(sate);
	//int sateSubCount = GetNoneZeroCount(sateSub);
	//if(sateCount == 0 && sateSubCount == 0)
	//{
	//	return;
	//}

	//struct sort_pred {
	//	bool operator()(const std::pair<int,int> &left, const std::pair<int,int> &right) 
	//	{ return left.second < right.second; }
	//};

	//vector< pair<int, int> > sateArray(sateCount);
	//for(int i = 0; i < sateCount; ++i)
	//{
	//	sateArray[i] = make_pair(i, sate[i].SatelliteID);
	//}
	//std::sort(sateArray.begin(), sateArray.end(), sort_pred());

	//vector< pair<int, int> > sateSubArray(sateSubCount);
	//for(int i = 0; i < sateSubCount; ++i)
	//{
	//	sateSubArray[i] = make_pair(i, sateSub[i].SatelliteID);
	//}
	//std::sort(sateSubArray.begin(), sateSubArray.end(), sort_pred());

	CFont* oldFont = dc->SelectObject(&(s->idFont));	
	CBrush* oldBrush = dc->SelectObject(&(s->noUseSnrBarBrush));	
	CPen* oldPen = dc->SelectObject(&(s->noUseSnrBarPen));	

	for(int i = 0; i < sateCount; ++i)
	{
    const Satellite* pSate = sate->GetSateIndex(i);
		//int arrayIdx = sateArray[i].first;
		int id = pSate->SatelliteID;
    int dx = 0, dy = 0;
		int index = startId + i;

		if(index < MaxCountInRaw)
		{	
			dx = m_startBarChart.x + index * (BarChartIdGap + SnrBarL2.cx * 2);
			dy = m_startBarChart.y;
		}
		else
		{
			ASSERT(FALSE);
		}
		dx += cellOffset;

		bool isInUse = !CheckInUse(id, gsa);
    DrawBarChartId(dc, s, isInUse, dx + (SnrBarL2.cx * 2 - bm.bmWidth) / 2, dy, id); 

#if (FLOAT_SNR)
		F32 snrValue = 0, snrValue2 = 0;
#else
		U16 snrValue = 0, snrValue2 = 0;
#endif
		CString idText;
    CRect barRect;
    COLORREF cr;
    //======Draw L1 SNR Chart
    snrValue = pSate->snr[0];
    if(snrValue != INVALIDATE_SNR)
    {
		  //Draw gray rectangle out line.
		  dc->SelectObject(&(s->inUseSnrBarPen));
		  dc->SelectObject(&(s->noUseSnrBarBrush));
      barRect.SetRect(dx, dy - SnrBarL2.cy - 2,
					  dx + SnrBarL2.cx + 1, dy - 1);
		  dc->Rectangle(barRect);

		  //Draw L1 snr bar.
		  dc->SelectObject((isInUse) ? &(s->inUseSnrBarBrush) : &(s->noUseSnrBarBrush));
		  dc->SelectObject((isInUse) ? &(s->inUseSnrBarPen) : &(s->noUseSnrBarPen));
		  barRect.SetRect(dx, dy - 1 - (int)(snrValue + .5),
			  dx + SnrBarL2.cx + 1, dy - 1);
		  dc->Rectangle(barRect);

		  //Draw L1 snr value text.
		  cr = (isInUse) ? s->inUseBarTextColor : s->noUseBarTextColor;
		  if(isInUse && snrValue < 15)
		  {
			  cr = RGB(10, 10, 10);
		  }
		  dc->SetTextColor(cr);
  #if (FLOAT_SNR)
		  idText.Format("%3.1f", snrValue);
  #else
		  idText.Format("%d", snrValue);
  #endif
		  barRect.SetRect(dx, dy - 1 - SnrBarL2.cx, dx + SnrBarL2.cx + 1, dy - 1);
		  dc->SelectObject(&(s->barFont));	
		  dc->DrawText(idText, barRect, DT_VCENTER | DT_CENTER);
    }
    
    //======Draw L2 SNR Chart
    snrValue2 = pSate->snr[1];
    if(snrValue != INVALIDATE_SNR && snrValue2 != 0)
    {
      //Draw l2 gray rectangle out line.
		  dc->SelectObject(&(sSub->inUseSnrBarPen));
		  dc->SelectObject(&(sSub->noUseSnrBarBrush));
		  barRect.SetRect(dx + SnrBarL2.cx, dy - SnrBarL2.cy - 2,
					  dx + 2 * SnrBarL2.cx, dy - 1);
		  dc->Rectangle(barRect);

      //Draw L2 snr bar.
		  dc->SelectObject((isInUse) ? &(sSub->inUseSnrBarPen) : &(sSub->noUseSnrBarPen));
		  dc->SelectObject((isInUse) ? &(sSub->inUseSnrBarBrush) : &(sSub->noUseSnrBarBrush));
		  barRect.SetRect(dx + SnrBarL2.cx, dy - 1 - (int)(snrValue2 + 0.5),
			  dx +  2 * SnrBarL2.cx, dy - 1);
		  dc->Rectangle(barRect);

		  //Draw L2 snr value text.
		  cr = (isInUse) ? sSub->inUseBarTextColor : sSub->noUseBarTextColor;
		  if(isInUse && snrValue2 < 15)
		  {
			  cr = RGB(10, 10, 10);
		  }
		  dc->SetTextColor(cr);
  #if (FLOAT_SNR)
		  idText.Format("%3.1f", snrValue2);
  #else
		  idText.Format("%d", snrValue2);
  #endif
		  barRect.SetRect(dx + SnrBarL2.cx, dy - 1 - SnrBarL2.cx ,
					  dx +  2 * SnrBarL2.cx, dy - 1);
		  dc->SelectObject(&(sSub->barFont));	
		  dc->DrawText(idText, barRect, DT_VCENTER | DT_CENTER);
    }
	} 

	startId += sateCount;

	dc->SelectObject(oldFont);	
	dc->SelectObject(oldBrush);	
	dc->SelectObject(oldPen);	
}

//============================================================================
CSnrBarChartBeidou::CSnrBarChartBeidou(void) 
{
	m_startBarChart = StartGnssBarChart;
	m_minId = 1;
	m_maxId = 31;
	m_titleText = "Beidou";
	SetUISetting(&bdUI);
}

CSnrBarChartBeidou::~CSnrBarChartBeidou(void) 
{

}

BEGIN_MESSAGE_MAP(CSnrBarChartBeidou, CSnrBarChart)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CSnrBarChartBeidou::OnPaint()
{
	RefreshBarChart();
}

//============================================================================
CSnrBarChartGalileo::CSnrBarChartGalileo(void) 
{
	m_startBarChart = StartGnssBarChart;
	m_minId = 1;
	m_maxId = 31;
	m_titleText = "Galileo";
	SetUISetting(&gaUI);
}

CSnrBarChartGalileo::~CSnrBarChartGalileo(void) 
{

}

BEGIN_MESSAGE_MAP(CSnrBarChartGalileo, CSnrBarChart)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CSnrBarChartGalileo::OnPaint()
{
	RefreshBarChart();
}

//============================================================================
//static const CSize SnrBarL2(14, 50);
//static const CPoint StartGnssBarChartL2(5, 52);

CSnrBarChartDualL2::CSnrBarChartDualL2() 
{
  Init();
}

CSnrBarChartDualL2::~CSnrBarChartDualL2(void) 
{

}

void CSnrBarChartDualL2::Init()
{
	m_startBarChart = StartGnssBarChartL2;
	m_minId = 1;
	m_maxId = 31;
	m_titleText = "Gps/GLONASS";

  SetUISetting(&gpl1UI);
  SetUISettingSub(&gpl2UI);
  SetUISetting2(&gll1UI);
  SetUISetting2Sub(&gll2UI);

}

BEGIN_MESSAGE_MAP(CSnrBarChartDualL2, CSnrBarChartL2)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CSnrBarChartDualL2::OnPaint()
{
	RefreshBarChart();
}

void CSnrBarChartDualL2::ShowBoxChart(CDC *dc)
{
	int n = 0;
	DrawSnr(dc, n, uiSetting, uiSettingSub, gsvData, gsaData, ggaData, sateStatus);
	DrawSnr(dc, n, uiSetting2, uiSetting2Sub, gsvData2, gsaData2, ggaData2, sateStatus2);
}
//#if FLOAT_SNR
//F32 CSnrBarChartDualL2::GetGsv2SubSnr(int id)
//#else
//int CSnrBarChartDualL2::GetGsv2SubSnr(int id)
//#endif
//{
//  if(sateStatus2Sub == NULL)
//    return 0;
//
//  for(int i = 0; i < MAX_SATELLITE; ++i)
//  {
//    if(sateStatus2Sub[i].SatelliteID == id)
//      return sateStatus2Sub[i].SNR;
//  }
//  return 0;
//}
/*
void CSnrBarChartDualL2::DrawSnr(CDC *dc, int& startId, UISetting* s, GPGSV* gsv, GPGSA* gsa, GPGGA* gga, Satellite* sate)
{

  BITMAP bm = {0};
	bm.bmWidth = bm.bmHeight = 20;
	int cellOffset = BarChartIdGap / 2;

	//Sort the satellite prn. 
	int sateCount = GetNoneZeroCount(sate);
	if(sateCount==0)
	{
		return;
	}

	vector< pair<int, int> > sateArray(sateCount);
	for(int i=0; i<sateCount; ++i)
	{
		sateArray[i] = make_pair(i, sate[i].SatelliteID);
	}
	struct sort_pred {
		bool operator()(const std::pair<int,int> &left, const std::pair<int,int> &right) 
		{ return left.second < right.second; }
	};
	std::sort(sateArray.begin(), sateArray.end(), sort_pred());

	CFont* oldFont = dc->SelectObject(&(s->idFont));	
	CBrush* oldBrush = dc->SelectObject(&(s->noUseSnrBarBrush));	
	CPen* oldPen = dc->SelectObject(&(s->noUseSnrBarPen));	

	for(int i=0; i<sateCount; ++i)
	{
		int arrayIdx = sateArray[i].first;
		int id = sate[arrayIdx].SatelliteID;
		int dx = 0, dy = 0;
		int index = startId + i;

		if(index < MaxCountInRaw)
		{	
			dx = m_startBarChart.x + index * (BarChartIdGap + SnrBarL2.cx * 2);
			dy = m_startBarChart.y;
		}
		else
		{
			ASSERT(FALSE);
		}
		dx += cellOffset;

		bool isInUse = !CheckInUse(id, gsa);
    DrawBarChartId(dc, s, isInUse, dx + (SnrBarL2.cx * 2 - bm.bmWidth) / 2, dy, id); 
#if FLOAT_SNR
		F32 snrValue = sate[arrayIdx].SNR;
#else
		U16 snrValue = sate[arrayIdx].SNR;
#endif
		if(snrValue == INVALIDATE_SNR)
		{
			continue;
		}

    //======Draw L1 SNR Chart
		//Draw gray rectangle out line.
		dc->SelectObject(&(s->inUseSnrBarPen));
		dc->SelectObject(&(s->noUseSnrBarBrush));
		CRect barRect(dx, dy - SnrBarL2.cy - 2,
					dx + SnrBarL2.cx + 1, dy - 1);
		dc->Rectangle(barRect);

		//Draw snr bar.
		dc->SelectObject((isInUse) ? &(s->inUseSnrBarBrush) : &(s->noUseSnrBarBrush));
		dc->SelectObject((isInUse) ? &(s->inUseSnrBarPen) : &(s->noUseSnrBarPen));
		barRect.SetRect(dx, dy - 1 - (int)(snrValue + .5),
			dx + SnrBarL2.cx + 1, dy - 1);
		dc->Rectangle(barRect);

		//Draw snr value text.
		COLORREF cr = (isInUse) ? s->inUseBarTextColor : s->noUseBarTextColor;
		if(isInUse && snrValue < 15)
		{
			cr = RGB(10, 10, 10);
		}
		dc->SetTextColor(cr);
		CString idText;
#if FLOAT_SNR
		idText.Format("%3.1f", snrValue);
#else
		idText.Format("%d", snrValue);
#endif
		//barRect.SetRect(dx + (bm.bmWidth - SnrBarL2.cx) / 2 + sgap, dy - 1 - SnrBarL2.cx,
		//			dx + (bm.bmWidth + SnrBarL2.cx) / 2 + sgap, dy - 1);
		barRect.SetRect(dx, dy - 1 - SnrBarL2.cx,
					dx + SnrBarL2.cx + 1, dy - 1);
		dc->SelectObject(&(s->barFont));	
		dc->DrawText(idText, barRect, DT_VCENTER | DT_CENTER);

    //======Draw L2 SNR Chart
    snrValue = GetGsvSubSnr(id);
		//Draw l2 gray rectangle out line.
		dc->SelectObject(&(uiSettingSub->inUseSnrBarPen));
		dc->SelectObject(&(uiSettingSub->noUseSnrBarBrush));
		barRect.SetRect(dx + SnrBarL2.cx, dy - SnrBarL2.cy - 2,
					dx + 2 * SnrBarL2.cx, dy - 1);
		dc->Rectangle(barRect);
    //Draw l2 snr bar.
		dc->SelectObject((isInUse) ? &(uiSettingSub->inUseSnrBarPen) : &(uiSettingSub->noUseSnrBarPen));
		dc->SelectObject((isInUse) ? &(uiSettingSub->inUseSnrBarBrush) : &(uiSettingSub->noUseSnrBarBrush));
		barRect.SetRect(dx + SnrBarL2.cx, dy - 1 - (int)(snrValue + 0.5),
			dx +  2 * SnrBarL2.cx, dy - 1);
		dc->Rectangle(barRect);

		//Draw l2 snr value text.
		cr = (isInUse) ? uiSettingSub->inUseBarTextColor : uiSettingSub->noUseBarTextColor;
		if(isInUse && snrValue < 15)
		{
			cr = RGB(10, 10, 10);
		}
		dc->SetTextColor(cr);
#if FLOAT_SNR
		idText.Format("%3.1f", snrValue);
#else
		idText.Format("%d", snrValue);
#endif
		barRect.SetRect(dx + SnrBarL2.cx, dy - 1 - SnrBarL2.cx ,
					dx +  2 * SnrBarL2.cx, dy - 1);
		dc->SelectObject(&(uiSettingSub->barFont));	
		dc->DrawText(idText, barRect, DT_VCENTER | DT_CENTER);
	} 
	startId += sateCount;

	dc->SelectObject(oldFont);	
	dc->SelectObject(oldBrush);	
	dc->SelectObject(oldPen);	
}
*/