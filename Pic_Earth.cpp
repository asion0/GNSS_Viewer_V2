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

	chkRect.SetRect(262, 170, 306, 190);
	m_gpCheck.Create( "GPS", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_LEFTTEXT,  
                   chkRect, this, 1023);  
	m_gpCheck.SetCheck(1);

	chkRect.SetRect(236, 190, 306, 210);
	m_glCheck.Create( "Glonass", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_LEFTTEXT,  
                   chkRect, this, 1024); 
	m_glCheck.SetCheck(1);

	chkRect.SetRect(244, 210, 306, 230);
	m_bdCheck.Create( "Beidou", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_LEFTTEXT,  
                   chkRect, this, 1025);  
	m_bdCheck.SetCheck(1);

	chkRect.SetRect(244, 230, 306, 250);
	m_gaCheck.Create( "Galileo", WS_CHILD | WS_VISIBLE | BS_AUTOCHECKBOX | BS_LEFTTEXT,  
                  chkRect, this, 1026);  
	m_gaCheck.SetCheck(1);
}

void CPic_Earth::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	if(!m_first)
	{
		m_first = true;
		InitCheck();
	}

	//CGPSDlg::gpsDlg->Refresh_EarthChart(&dc);
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
		DrawEarthSate(dc, &gpUI, CGPSDlg::gpsDlg->sate_gps, &CGPSDlg::gpsDlg->m_gpgsvMsg, &CGPSDlg::gpsDlg->m_gpgsaMsg, &CGPSDlg::gpsDlg->m_gpggaMsg);
	}

	if(m_glCheck.GetCheck())
	{
		DrawEarthSate(dc, &glUI, CGPSDlg::gpsDlg->sate_gnss, &CGPSDlg::gpsDlg->m_glgsvMsg, &CGPSDlg::gpsDlg->m_glgsaMsg, &CGPSDlg::gpsDlg->m_gpggaMsg);
	}

	if(m_bdCheck.GetCheck())
	{
		DrawEarthSate(dc, &bdUI, CGPSDlg::gpsDlg->sate_bd, &CGPSDlg::gpsDlg->m_bdgsvMsg, &CGPSDlg::gpsDlg->m_bdgsaMsg, &CGPSDlg::gpsDlg->m_gpggaMsg);
	}

	if(m_gaCheck.GetCheck())
	{
		DrawEarthSate(dc, &gaUI, CGPSDlg::gpsDlg->sate_ga, &CGPSDlg::gpsDlg->m_gagsvMsg, &CGPSDlg::gpsDlg->m_gagsaMsg, &CGPSDlg::gpsDlg->m_gpggaMsg);
	}

}

void CPic_Earth::DrawEarthSate(CDC* dc, UISetting* s, Satellite* sate, GPGSV* gsv, GPGSA* gsa, GPGGA* gga)
{
	int centerX = 250 / 2;
	int centerY = 250 / 2;
	int x = 0, y = 0;
	double ele = 0.0;	       
	int id = 0;

	for(int i=0; i<gsv->NumOfSate; ++i)
	{
		id = sate[i].SatelliteID;
		if(id == 0)
		{
			continue;	
		}
		//Elevation 仰角 0~90度
		//Azimuth 方位角 0~359度
		//Elevation = 75*cos(satellites[i].Elevation*PI/180);	
		U16 azimuth = sate[i].Azimuth;
		U16 elevation = sate[i].Elevation;
		U16 qtyInd = gga->GPSQualityIndicator;

		ele = 114.0 - ( 114.0 * elevation / 90.0);			
		x = (int)(ele * sin(azimuth * PI / 180.0));
		y = -(int)(ele * cos(azimuth * PI / 180.0));

		x += centerX;
		y += centerY;	

		//bool isInUse = IsFixed(qtyInd) && !CheckInUse(id, gsa);
		bool isInUse = !CheckInUse(id, gsa);
		CSnrBarChart::DrawBarChartId(dc, s, isInUse, x, y, id, true);
	}
}
