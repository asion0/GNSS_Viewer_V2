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

void CSnrBarChart::DrawSnr(CDC *dc, int& startId, UISetting* s, GPGSV* gsv, GPGSA* gsa, GPGGA* gga, Satellite* sate)
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

		//int id = sate[i].SatelliteID;
		int arrayIdx = sateArray[i].first;
		int id = sate[arrayIdx].SatelliteID;
		int dx = 0, dy = 0;
		int index = startId + i;

		if(id==0)
		{
			int a = 0;
		}
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

		//bool isInUse = IsFixed(gga->GPSQualityIndicator) && !CheckInUse(id, gsa);
		bool isInUse = !CheckInUse(id, gsa);
		DrawBarChartId(dc, s, isInUse, dx, dy, id); 

		U16 snrValue = sate[arrayIdx].SNR;
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
		barRect.SetRect(dx + (bm.bmWidth - SnrBar.cx) / 2, dy - 1 - snrValue,
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
		idText.Format("%d", snrValue);
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

void CSnrBarChart::DrawSnrInRange(CDC *dc, int& startId, UISetting* s, GPGSV* gsv, GPGSA* gsa, GPGGA* gga, Satellite* sate, int start, int end)
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
		//int id = sate[i].SatelliteID;
		int arrayIdx = sateArray[i].first;
		int id = sate[arrayIdx].SatelliteID;
		if(id<start || id>end)
		{
			continue;
		}

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

		//bool noUsed = CheckInUse(id, gsa);
		//bool isFixed = IsFixed(ggaData->GPSQualityIndicator);
		bool isInUse = IsFixed(gga->GPSQualityIndicator) && !CheckInUse(id, gsa);

		//CMaskedBitmap snrIcon;
		//dc->SelectObject(&(s->idFont));	
		//snrIcon.LoadBitmap(isInUse ? s->inUseSateBmpId : s->noUseSateBmpId);
		DrawBarChartId(dc, s, isInUse, dx, dy, id); 
		if(id==0)
		{
			int a = 0;
		}
		//snrIcon.DeleteObject();

		U16 snrValue = sate[arrayIdx].SNR;
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
		barRect.SetRect(dx + (bm.bmWidth - SnrBar.cx) / 2, dy - 1 - snrValue,
			dx + (bm.bmWidth + SnrBar.cx) / 2, dy - 1);
		dc->Rectangle(barRect);

		//Draw snr value text.
		dc->SetTextColor((isInUse) ? s->inUseBarTextColor : s->noUseBarTextColor);
		CString idText;
		idText.Format("%d", snrValue);
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
	//if(gsvData)
//	{
		RefreshBarChart();
//	}
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
CSnrBarChartGpsGlonass::CSnrBarChartGpsGlonass(void) 
{
	gsvData2 = NULL;
	gsaData2 = NULL;
	ggaData2 = NULL;
	sateStatus2 = NULL;
	m_startBarChart = StartGnssBarChart;
	m_minId = GpsIdStart;
	m_maxId = GpsIdEnd;
	m_titleText = "GPS / GNSS";
	SetUISetting(&gpUI);
	SetUISetting2(&glUI);
}

CSnrBarChartGpsGlonass::~CSnrBarChartGpsGlonass(void) 
{

}

BEGIN_MESSAGE_MAP(CSnrBarChartGpsGlonass, CSnrBarChart)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CSnrBarChartGpsGlonass::OnPaint()
{
	RefreshBarChart();
}

void CSnrBarChartGpsGlonass::ShowBoxChart(CDC *dc)
{
	int n = 0;
	DrawSnr(dc, n, uiSetting, gsvData, gsaData, ggaData, sateStatus);
	DrawSnr(dc, n, uiSetting2, gsvData2, gsaData2, ggaData2, sateStatus2);
}
//============================================================================
CSnrBarChartBeidouL2::CSnrBarChartBeidouL2(void) 
{
	m_startBarChart = StartGnssBarChart;
	m_minId = 1;
	m_maxId = 31;
	m_titleText = "Beidou";
	SetUISetting(&bdUI);
	SetUISetting2(&gaUI);
}

CSnrBarChartBeidouL2::~CSnrBarChartBeidouL2(void) 
{

}

BEGIN_MESSAGE_MAP(CSnrBarChartBeidouL2, CSnrBarChart)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CSnrBarChartBeidouL2::OnPaint()
{
	RefreshBarChart();
}

int CSnrBarChartBeidouL2::GetGsv2Snr(int id)
{
  if(sateStatus2 == NULL)
    return 0;

  for(int i = 0; i < MAX_SATELLITE; ++i)
  {
    if(sateStatus2[i].SatelliteID == id)
      return sateStatus2[i].SNR;
  }
  return 0;
}

void CSnrBarChartBeidouL2::DrawSnr(CDC *dc, int& startId, UISetting* s, GPGSV* gsv, GPGSA* gsa, GPGGA* gga, Satellite* sate)
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
			dx = m_startBarChart.x + index * (BarChartIdGap + bm.bmWidth * 2);
			dy = m_startBarChart.y;
		}
		else
		{
			ASSERT(FALSE);
		}
		dx += cellOffset;

		//bool isInUse = IsFixed(gga->GPSQualityIndicator) && !CheckInUse(id, gsa);
		bool isInUse = !CheckInUse(id, gsa);
		DrawBarChartId(dc, s, isInUse, dx + bm.bmWidth / 2, dy, id); 

		U16 snrValue = sate[arrayIdx].SNR;
		if(snrValue == INVALIDATE_SNR)
		{
			continue;
		}
    const int sgap = 1;
		//Draw gray rectangle out line.
		dc->SelectObject(&(s->inUseSnrBarPen));
		dc->SelectObject(&(s->noUseSnrBarBrush));
		CRect barRect(dx + (bm.bmWidth - SnrBar.cx) / 2 + sgap, dy - SnrBar.cy - 2,
					dx + (bm.bmWidth + SnrBar.cx) / 2 + sgap, dy - 1);
		dc->Rectangle(barRect);

		//Draw snr bar.
		dc->SelectObject((isInUse) ? &(s->inUseSnrBarBrush) : &(s->noUseSnrBarBrush));
		dc->SelectObject((isInUse) ? &(s->inUseSnrBarPen) : &(s->noUseSnrBarPen));
		barRect.SetRect(dx + (bm.bmWidth - SnrBar.cx) / 2 + sgap, dy - 1 - snrValue,
			dx + (bm.bmWidth + SnrBar.cx) / 2 + sgap, dy - 1);
		dc->Rectangle(barRect);

		//Draw snr value text.
		COLORREF cr = (isInUse) ? s->inUseBarTextColor : s->noUseBarTextColor;
		if(isInUse && snrValue < 15)
		{
			cr = RGB(10, 10, 10);
		}
		dc->SetTextColor(cr);
		CString idText;
		idText.Format("%d", snrValue);
		barRect.SetRect(dx + (bm.bmWidth - SnrBar.cx) / 2 + sgap, dy - 1 - SnrBar.cx,
					dx + (bm.bmWidth + SnrBar.cx) / 2 + sgap, dy - 1);
		dc->SelectObject(&(s->barFont));	
		dc->DrawText(idText, barRect, DT_VCENTER | DT_CENTER);

    snrValue = GetGsv2Snr(id);
		//Draw l2 gray rectangle out line.
		dc->SelectObject(&(uiSetting2->inUseSnrBarPen));
		dc->SelectObject(&(uiSetting2->noUseSnrBarBrush));
		barRect.SetRect(dx + (bm.bmWidth - SnrBar.cx) / 2 + bm.bmWidth - sgap, dy - SnrBar.cy - 2,
					dx + (bm.bmWidth + SnrBar.cx) / 2 + bm.bmWidth - sgap, dy - 1);
		dc->Rectangle(barRect);
    //Draw l2 snr bar.
		dc->SelectObject((isInUse) ? &(uiSetting2->inUseSnrBarBrush) : &(uiSetting2->noUseSnrBarBrush));
		dc->SelectObject((isInUse) ? &(uiSetting2->inUseSnrBarPen) : &(uiSetting2->noUseSnrBarPen));
		barRect.SetRect(dx + (bm.bmWidth - SnrBar.cx) / 2 + bm.bmWidth - sgap, dy - 1 - snrValue,
			dx + (bm.bmWidth + SnrBar.cx) / 2 + bm.bmWidth - sgap, dy - 1);
		dc->Rectangle(barRect);

		//Draw l2 snr value text.
		cr = (isInUse) ? uiSetting2->inUseBarTextColor : uiSetting2->noUseBarTextColor;
		if(isInUse && snrValue < 15)
		{
			cr = RGB(10, 10, 10);
		}
		dc->SetTextColor(cr);
		idText.Format("%d", snrValue);
		barRect.SetRect(dx + (bm.bmWidth - SnrBar.cx) / 2 + bm.bmWidth - sgap, dy - 1 - SnrBar.cx ,
					dx + (bm.bmWidth + SnrBar.cx) / 2 + bm.bmWidth - sgap, dy - 1);
		dc->SelectObject(&(uiSetting2->barFont));	
		dc->DrawText(idText, barRect, DT_VCENTER | DT_CENTER);
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
