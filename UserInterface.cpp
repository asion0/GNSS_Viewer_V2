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
/*
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




*/