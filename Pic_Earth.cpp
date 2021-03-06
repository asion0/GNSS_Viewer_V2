#include "StdAfx.h"
#include "Pic_Earth.h"
#include "GPSDlg.h"
#include "UISetting.h"
#include "SnrBarChart.h"

CPic_Earth::CPic_Earth()
{
	m_first = false;
}

CPic_Earth::~CPic_Earth(void)
{

}

BEGIN_MESSAGE_MAP(CPic_Earth, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CPic_Earth::InitCheck()
{
	CRect chkRect;

	chkRect.SetRect(262, 150, 306, 170);
	m_gpCheck.Create( "GPS", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_LEFTTEXT | BS_RIGHT,  
                   chkRect, this, 1023);  
	m_gpCheck.SetCheck(1);

#ifdef _NAVIC_CONVERT_
	chkRect.SetRect(250, 170, 306, 190);
	m_glCheck.Create( "NavIC", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_LEFTTEXT | BS_RIGHT,  
                   chkRect, this, 1024); 
#else
	chkRect.SetRect(236, 170, 306, 190);
	m_glCheck.Create( "Glonass", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_LEFTTEXT | BS_RIGHT,  
                   chkRect, this, 1024); 
#endif
	m_glCheck.SetCheck(1);

	chkRect.SetRect(244, 190, 306, 210);
	m_bdCheck.Create( "Beidou", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_LEFTTEXT | BS_RIGHT,  
                   chkRect, this, 1025);  
	m_bdCheck.SetCheck(1);

	chkRect.SetRect(244, 210, 306, 230);
	m_gaCheck.Create( "Galileo", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_LEFTTEXT | BS_RIGHT,  
                  chkRect, this, 1026);  
	m_gaCheck.SetCheck(1);

  chkRect.SetRect(244, 230, 306, 250);
	m_giCheck.Create( "NavIC", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_LEFTTEXT | BS_RIGHT,  
                  chkRect, this, 1026);
	m_giCheck.SetCheck(1);


#ifdef _NAVIC_SUPPORT_
	//m_gaCheck.SetCheck(0);
 // m_gaCheck.ShowWindow(SW_HIDE);
	m_giCheck.SetCheck(1);
#else
	m_gaCheck.SetCheck(1);
	m_giCheck.SetCheck(0);
  m_giCheck.ShowWindow(SW_HIDE);
#endif

}

void CPic_Earth::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	if(!m_first)
	{
		m_first = true;
		InitCheck();
	}
	Refresh_EarthChart(&dc);
}

void CPic_Earth::Refresh_EarthChart(CDC *dc)
{
	UINT usedEarth = g_setting.earthBitmap;
	static bool firstFixed = false;
	UINT earthA[] = { IDB_EARTH1, IDB_EARTH2, IDB_EARTH3, IDB_EARTH4, IDB_EARTH5, IDB_EARTH6 };
	CxImage img, img2;

	img.LoadResource(FindResource(NULL, MAKEINTRESOURCE(IDB_EARTH), RT_BITMAP), 
		CXIMAGE_FORMAT_BMP);

	if(!firstFixed && CGPSDlg::gpsDlg->IsFixed())
	{
		double lon = CGPSDlg::gpsDlg->m_gpggaMsg.Longitude / 100.0;
    char lon_ew = CGPSDlg::gpsDlg->m_gpggaMsg.Longitude_E_W;
    char lat_ns = CGPSDlg::gpsDlg->m_gpggaMsg.Latitude_N_S;
    if(lon == 0.0 && lon_ew == 0 && lat_ns == 0)
    {
		  lon = CGPSDlg::gpsDlg->m_gprmcMsg.Longitude / 100.0;
      lon_ew = CGPSDlg::gpsDlg->m_gprmcMsg.Longitude_E_W;
      lat_ns = CGPSDlg::gpsDlg->m_gprmcMsg.Latitude_N_S;
    }

		lon *= (lon_ew == 'W') ? -1.0 : 1.0;
		lon += 180.0;
		if(lon > 11 && lon < 142)	//Americas
		{
			usedEarth = (lat_ns == 'N') ? 4 : 5;
			firstFixed = true;
			g_setting.earthBitmap = usedEarth;
		}
		else if(lon > 142.0 && lon < 251.0)	//Europe
		{
			usedEarth = (lat_ns == 'N') ? 2 : 3;
			firstFixed = true;
			g_setting.earthBitmap = usedEarth;
		}
		else
		{
			usedEarth = (lat_ns == 'N') ? 0 : 1;
			firstFixed = true;
			g_setting.earthBitmap = usedEarth;
		}
	}

	img2.LoadResource(FindResource(NULL, MAKEINTRESOURCE(earthA[usedEarth]), "PNG"), 
		CXIMAGE_FORMAT_PNG);

	if(!img.IsValid())
	{
		return;
	}

	CRect rc(0,0,img.GetWidth(), img.GetHeight());
	CRgn rgn;
	POINT pts[] = { {0, 0}, {312, 0}, {312, 170}, {0, 170}, {245, 170}, {245, 190}, {232, 190}, {232, 247}, {0, 247} };
	const int ptsSize = sizeof(pts) / sizeof(pts[0]);

	rgn.CreatePolygonRgn(pts, ptsSize, ALTERNATE);
	dc->SelectClipRgn(&rgn);

	CMemDC memDC(*dc, rc);
	img.Draw(memDC.GetDC().GetSafeHdc(), 0, 0);
	img2.Draw(memDC.GetDC().GetSafeHdc(), 8, 8);
	memDC.GetDC().SetBkMode(TRANSPARENT);	

	Show_EarthChart(&(memDC.GetDC()));
}

void CPic_Earth::Show_EarthChart(CDC *dc)
{
	if(m_gpCheck.GetCheck())
	{
		DrawEarthSate(dc, &gpUI, &CGPSDlg::gpsDlg->sate_gp, &CGPSDlg::gpsDlg->m_gpgsvMsg, &CGPSDlg::gpsDlg->m_gpgsaMsg, &CGPSDlg::gpsDlg->m_gpggaMsg);
	}

	if(m_glCheck.GetCheck())
	{
		DrawEarthSate(dc, &glUI, &CGPSDlg::gpsDlg->sate_gl, &CGPSDlg::gpsDlg->m_glgsvMsg, &CGPSDlg::gpsDlg->m_glgsaMsg, &CGPSDlg::gpsDlg->m_gpggaMsg);
	}

	if(m_bdCheck.GetCheck())
	{
		DrawEarthSate(dc, &bdUI, &CGPSDlg::gpsDlg->sate_bd, &CGPSDlg::gpsDlg->m_bdgsvMsg, &CGPSDlg::gpsDlg->m_bdgsaMsg, &CGPSDlg::gpsDlg->m_gpggaMsg);
	}

	if(m_gaCheck.GetCheck())
	{
		DrawEarthSate(dc, &gaUI, &CGPSDlg::gpsDlg->sate_ga, &CGPSDlg::gpsDlg->m_gagsvMsg, &CGPSDlg::gpsDlg->m_gagsaMsg, &CGPSDlg::gpsDlg->m_gpggaMsg);
	}

	if(m_giCheck.GetCheck())
	{
		DrawEarthSate(dc, &giUI, &CGPSDlg::gpsDlg->sate_gi, &CGPSDlg::gpsDlg->m_gigsvMsg, &CGPSDlg::gpsDlg->m_gigsaMsg, &CGPSDlg::gpsDlg->m_gpggaMsg);
	}
}

void CPic_Earth::DrawEarthSate(CDC* dc, UISetting* s, Satellites* sate, GPGSV* gsv, GPGSA* gsa, GPGGA* gga)
{
	const int centerX = 250 / 2;
	const int centerY = 250 / 2;
  const double radius = 114;
	int x = 0, y = 0;
	int id = 0;

	for(int i = 0; i < sate->GetSateCount(); ++i)
	{
		//id = sate[i].prn;
    const Satellite* pSate = sate->GetSateIndex(i);
    id = pSate->GetPrn();
		if(!pSate->IsInUsePrn())
		{
			continue;	
		}
		//Elevation 0~90 degrees
		//Azimuth  0~359 degrees
		//Elevation = 75 * cos(satellites[i].Elevation * PI / 180);	
		U16 azi = pSate->GetAzi();
		U16 ele = pSate->GetEle();
		double e = radius - (radius * ele / 90.0);			
		x = centerX + (int)(e * sin(azi * PI / 180.0));
		y = centerY - (int)(e * cos(azi * PI / 180.0));

		bool isInUse = !CheckInUse(id, gsa);

#ifdef _NAVIC_CONVERT_
		CSnrBarChart::DrawBarChartId(dc, s, isInUse, x, y, 
      (NMEA::GetGNSSSystem(id) == NMEA::Glonass) ? id - 64 : id, true);
#else
		CSnrBarChart::DrawBarChartId(dc, s, isInUse, x, y, id, true);
#endif
	}
}
