#include "stdafx.h"
#include "GPS.h"
#include "MaskedBitmap.h"
#include "GPSDlg.h"
#include "UISetting.h"
#include "SnrBarChart.h"
#include "Pic_Earth.h"

static CSize GpsBarChartBg(685, 170);
static int StartGpsBarChartX = 3;
static int GpsBarChartGapX = 22;

static CSize GnssBarChartBg(685, 94);
static int StartGnssBarChartX = 3;
static int StartGnssBarChartY = 170;
static int GnssBarChartGapX = 22;

static int MaxGpsId = 61;
static int HalfMaxGpsId = (MaxGpsId + 1) / 2;
static int GnssIdStart = 65;

void CGPSDlg::Refresh_EarthChart(CDC *earth_dc)
{
	static bool firstFixed = false;
	UINT earthA[] = { IDB_EARTH1, IDB_EARTH2, IDB_EARTH3, IDB_EARTH4, IDB_EARTH5, IDB_EARTH6 };
	CxImage img, img2;
	img.LoadResource(FindResource(NULL, MAKEINTRESOURCE(IDB_EARTH), RT_BITMAP), 
		CXIMAGE_FORMAT_BMP);

	if(!firstFixed && CGPSDlg::gpsDlg->IsFixed())
	{
		double lon = CGPSDlg::gpsDlg->m_gpggaMsg.Longitude / 100.0;
		lon *= (CGPSDlg::gpsDlg->m_gpggaMsg.Longitude_E_W=='W') ? -1.0 : 1.0;
		lon += 180.0;
		if(lon > 11 && lon < 142)	//Americas
		{
			usedEarth = (CGPSDlg::gpsDlg->m_gpggaMsg.Latitude_N_S=='N') ? 4 : 5;
			firstFixed = true;
			g_setting.earthBitmap = usedEarth;
		}
		else if(lon > 142.0 && lon < 251.0)	//Europe
		{
			usedEarth = (CGPSDlg::gpsDlg->m_gpggaMsg.Latitude_N_S=='N') ? 2 : 3;
			firstFixed = true;
			g_setting.earthBitmap = usedEarth;
		}
		else
		{
			usedEarth = (CGPSDlg::gpsDlg->m_gpggaMsg.Latitude_N_S=='N') ? 0 : 1;
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
	CMemDC memDC(*earth_dc, rc);
	img.Draw(memDC.GetDC().GetSafeHdc(), 0, 0);
	img2.Draw(memDC.GetDC().GetSafeHdc(), 8, 8);
	memDC.GetDC().SetBkMode(TRANSPARENT);	

	Show_EarthChart(&(memDC.GetDC()));
}

CGPSDlg::PrnType CGPSDlg::GetPrnType(int id)
{
	if(id <= 51 || (id >= 193 && id <= 197))
	{
		return Gps;
	}

	if(id >= 65 && id <= 96)
	{
		return Glonass;
	}

	return Unknown;
}

void DrawEarthSate(CDC* dc, UISetting* s, Satellite* sate, GPGSV* gsv, GPGSA* gsa, GPGGA* gga)
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

		bool isInUse = IsFixed(qtyInd) && !CheckInUse(id, gsa);
		CSnrBarChart::DrawBarChartId(dc, s, isInUse, x, y, id, true);
	}
}



void CGPSDlg::Show_EarthChart(CDC *dc)
{
	if(pic_earth->m_gpCheck.GetCheck())
	{
		DrawEarthSate(dc, &gpUI, sate_gps, &m_gpgsvMsg, &m_gpgsaMsg, &m_gpggaMsg);
	}

	if(pic_earth->m_glCheck.GetCheck())
	{
		DrawEarthSate(dc, &glUI, sate_gnss, &m_glgsvMsg, &m_glgsaMsg, &m_gpggaMsg);
	}

	if(pic_earth->m_bdCheck.GetCheck())
	{
		DrawEarthSate(dc, &bdUI, sate_bd, &m_bdgsvMsg, &m_bdgsaMsg, &m_gpggaMsg);
	}

	if(pic_earth->m_gaCheck.GetCheck())
	{
		DrawEarthSate(dc, &gaUI, sate_ga, &m_gagsvMsg, &m_gagsaMsg, &m_gpggaMsg);
	}

}



int CGPSDlg::CreateSubMenu(const HMENU hMenu, const MenuItemEntry* menuItemTable, LPCSTR pszSubMenuText)
{
	HMENU hMenuPopup = CreateMenu();

	const MenuItemEntry *p = menuItemTable;
	while(p->type || p->id || p->pszText || p->subMenu)
	{
		if(!p->showOption)
		{
			++p;
			continue;
		}
		if(MF_SEPARATOR == p->type)
		{
			AppendMenu(hMenuPopup, MF_SEPARATOR, 0, NULL);
		}
		else if(MF_STRING == p->type)
		{
			AppendMenu(hMenuPopup, MF_STRING,  p->id, p->pszText);
		}
		else if(MF_POPUP == p->type)
		{
			CreateSubMenu(hMenuPopup, p->subMenu, p->pszText);
		}
		else
		{
			ASSERT(FALSE);
		}
		++p;
	}
	AppendMenu(hMenu, MF_POPUP, (UINT_PTR)hMenuPopup, pszSubMenuText);
	return 0;
}

void CGPSDlg::DrawGnssSatellite(CDC* dc, int id, int centerX, int centerY)
{
	for(int i=0;i<m_glgsvMsg.NumOfSate;i++)
	{
		id=sate_gnss[i].SatelliteID;

		bool noUsed = true;
		for(int nums=0;nums <MAX_SATELLITE;nums++)
		{
			if(id == m_glgsaMsg.SatelliteID[nums])
			{
				noUsed = false;
				break;
			}
		}

		int snrValue = sate_gnss[i].SNR;
		//畫地球上面的衛星
		if(id==0 || id == 64)
		{
			continue;	
		}
		//Elevation = ((70*sate[i].Elevation)-6300)/(-90);			
		//Elevation = 70*cos(satellites[i].Elevation*PI/180);			
		double elevation = 75-(75*sate_gnss[i].Elevation/90);			
		int x = (int)(elevation*sin(sate_gnss[i].Azimuth*PI/180));
		int y = -(int)(elevation*cos(sate_gnss[i].Azimuth*PI/180));

		x += centerX - 10;
		y += centerY - 10;	


		CMaskedBitmap m_sate;		
		BITMAP bm;

		if(IsFixed() && !noUsed)
			m_sate.LoadBitmap(IDB_GL_ACT);
		else
			m_sate.LoadBitmap(IDB_GL_DIS);

		m_sate.GetBitmap(&bm);
		m_sate.DrawTransparent(dc,x,y,RGB(255,255,255));

		DeleteObject(&bm);
		m_sate.DeleteObject();

		if(IsFixed() && !noUsed)
			dc->SetTextColor( RGB(255,255,255) );
		else
			dc->SetTextColor( RGB(255,255,255) );	

		if(id > 64)
		{
			CString idText;
			idText.Format("%d", id);
			dc->TextOut((int)(x+5), y+1, idText);
		}
	}
}

void CGPSDlg::DrawBdSatellite(CDC* dc, int id, int centerX, int centerY)
{
	for(int i=0;i<m_bdgsvMsg.NumOfSate;i++)
	{
		id=sate_bd[i].SatelliteID;

		bool noUsed = true;
		for(int nums=0;nums <MAX_SATELLITE;nums++)
		{
			if(id == m_bdgsaMsg.SatelliteID[nums])
			{
				noUsed = false;
				break;
			}
		}

		int snrValue = sate_bd[i].SNR;
		//畫地球上面的衛星
		if(id==0 || id == 64)
		{
			continue;	
		}
		//Elevation = ((70*sate[i].Elevation)-6300)/(-90);			
		//Elevation = 70*cos(satellites[i].Elevation*PI/180);			
		double elevation = 75-(75*sate_bd[i].Elevation/90);			
		int x = (int)(elevation*sin(sate_bd[i].Azimuth*PI/180));
		int y = -(int)(elevation*cos(sate_bd[i].Azimuth*PI/180));

		x += centerX - 10;
		y += centerY - 10;	


		CMaskedBitmap m_sate;		
		BITMAP bm;

		if(IsFixed() && !noUsed)
			m_sate.LoadBitmap(IDB_BD_ACT);
		else
			m_sate.LoadBitmap(IDB_BD_DIS);

		m_sate.GetBitmap(&bm);
		m_sate.DrawTransparent(dc,x,y,RGB(255,255,255));

		DeleteObject(&bm);
		m_sate.DeleteObject();

		if(IsFixed() && !noUsed)
			dc->SetTextColor( RGB(255,255,255) );
		else
			dc->SetTextColor( RGB(255,255,255) );	

		//if(id > 64)
		{
			CString idText;
			idText.Format("%d", id);
			dc->TextOut((int)(x+5), y+1, idText);
		}
	}	//for(int i=0;i<m_bdgsvMsg.NumOfSate;i++)
}

void CGPSDlg::DrawGaSatellite(CDC* dc, int id, int centerX, int centerY)
{
	for(int i=0;i<m_gagsvMsg.NumOfSate;i++)
	{
		id=sate_ga[i].SatelliteID;

		bool noUsed = true;
		for(int nums=0;nums <MAX_SATELLITE;nums++)
		{
			if(id == m_gagsaMsg.SatelliteID[nums])
			{
				noUsed = false;
				break;
			}
		}

		int snrValue = sate_ga[i].SNR;
		//畫地球上面的衛星
		if(id==0 || id == 64)
		{
			continue;	
		}
		//Elevation = ((70*sate[i].Elevation)-6300)/(-90);			
		//Elevation = 70*cos(satellites[i].Elevation*PI/180);			
		double elevation = 75-(75*sate_ga[i].Elevation/90);			
		int x = (int)(elevation*sin(sate_ga[i].Azimuth*PI/180));
		int y = -(int)(elevation*cos(sate_ga[i].Azimuth*PI/180));

		x += centerX - 10;
		y += centerY - 10;	


		CMaskedBitmap m_sate;		
		BITMAP bm;

		if(IsFixed() && !noUsed)
			m_sate.LoadBitmap(IDB_GA_ACT);
		else
			m_sate.LoadBitmap(IDB_GA_DIS);

		m_sate.GetBitmap(&bm);
		m_sate.DrawTransparent(dc,x,y,RGB(255,255,255));

		DeleteObject(&bm);
		m_sate.DeleteObject();

		if(IsFixed() && !noUsed)
			dc->SetTextColor( RGB(255,255,255) );
		else
			dc->SetTextColor( RGB(255,255,255) );	

		//if(id > 64)
		{
			CString idText;
			idText.Format("%d", id);
			dc->TextOut((int)(x+5), y+1, idText);
		}
	}	//for(int i=0;i<m_bdgsvMsg.NumOfSate;i++)
}



