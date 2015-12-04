#include "StdAfx.h"
#include "Pic_Scatter.h"
#include "GPSDlg.h"
#include "UISetting.h"

ScatterData g_scatterData;
static void Log(CString f, int line, CString name = "", int data = 0)
{
	static char dbg_buf[64];
	sprintf_s(dbg_buf, "%s(%d) %s - %d\r\n", f, line, name, data);
	::OutputDebugString(dbg_buf);
}

void ScatterData::SetOrigin()
{
	double offset_x = 0;
	double offset_y = 0;	 
	
	LockEnuData();
	if(g_setting.specifyCenter && IS_DEBUG)
	{	//Only Internal Use version support specify center.
		m_lon = g_setting.scatterCenterLon;
		m_lat = g_setting.scatterCenterLat;
		if(g_setting.specifyCenterAlt)
		{
			ini_h = g_setting.scatterCenterAlt;
		}
	}
	else
	{
		m_lon = lon_deg;
		m_lat = lat_deg;
		offset_x  = current_x; 
		offset_y  = current_y;	
		ini_h = CGPSDlg::gpsDlg->m_gpggaMsg.Altitude + CGPSDlg::gpsDlg->m_gpggaMsg.GeoidalSeparation;
	}
	
	vector<double>::iterator enu_x_it, enu_y_it;
	for(enu_x_it = enu_x.begin(), enu_y_it = enu_y.begin(); 
		enu_x_it != enu_x.end(); 
		++enu_x_it, ++enu_y_it)
	{				
		*enu_x_it -= offset_x / 1000.0F;	
		*enu_y_it -= offset_y / 1000.0F;		
	}		
	UnlockEnuData();

	SetRotationMatrix();		
	int_N = WGS84_RA / (sqrt(1 - WGS84_E2 * sin(m_lat) * sin(m_lat)));		
    WGS84_X = (int_N + ini_h) * cos(m_lat) * cos(m_lon);
	WGS84_Y = (int_N + ini_h) * cos(m_lat) * sin(m_lon);
	WGS84_Z = (int_N * (1 - WGS84_E2) + ini_h) * sin(m_lat);	
}

void ScatterData::SetRotationMatrix()
{
	m_lon = Deg2Rad(m_lon);
	m_lat = Deg2Rad(m_lat);
		
	_WGS842NEU(0,0) = (F32)-sin(m_lon);
    _WGS842NEU(0,1) = (F32)cos(m_lon);
    _WGS842NEU(0,2) = 0;
    _WGS842NEU(1,0) = (F32)(-sin(m_lat)*cos(m_lon));
    _WGS842NEU(1,1) = (F32)(-sin(m_lat)*sin(m_lon));
    _WGS842NEU(1,2) = (F32)cos(m_lat);
    _WGS842NEU(2,0) = (F32)(cos(m_lat)*cos(m_lon));
    _WGS842NEU(2,1) = (F32)(cos(m_lat)*sin(m_lon));
    _WGS842NEU(2,2) = (F32)sin(m_lat);
}

void ScatterData::InitPos()
{
	long double lon, lat;
	ini_h = CGPSDlg::gpsDlg->m_gpggaMsg.Altitude + CGPSDlg::gpsDlg->m_gpggaMsg.GeoidalSeparation;
	if(g_setting.specifyCenter && IS_DEBUG)
	{
		m_lon = g_setting.scatterCenterLon;
		m_lat = g_setting.scatterCenterLat;
		if(g_setting.specifyCenterAlt)
		{
			ini_h = g_setting.scatterCenterAlt;
		}
	}
	else
	{
		m_lon = int(CGPSDlg::gpsDlg->m_gpggaMsg.Longitude / 100.0);
		m_lon += double(CGPSDlg::gpsDlg->m_gpggaMsg.Longitude - 
			int(CGPSDlg::gpsDlg->m_gpggaMsg.Longitude / 100.0) * 100.0) / 60;
		m_lat = int(CGPSDlg::gpsDlg->m_gpggaMsg.Latitude / 100);
		m_lat += double(CGPSDlg::gpsDlg->m_gpggaMsg.Latitude - 
			int(CGPSDlg::gpsDlg->m_gpggaMsg.Latitude/100)*100)/60;
	}

	inimaplondeg = int(CGPSDlg::gpsDlg->m_gpggaMsg.Longitude/100);
	inimaplonmin = (int)CGPSDlg::gpsDlg->m_gpggaMsg.Longitude-
		int(CGPSDlg::gpsDlg->m_gpggaMsg.Longitude/100)*100;
	inimaplonsec = (int)((CGPSDlg::gpsDlg->m_gpggaMsg.Longitude-
		(int)CGPSDlg::gpsDlg->m_gpggaMsg.Longitude)*60.0);

	inimaplatdeg = int(CGPSDlg::gpsDlg->m_gpggaMsg.Latitude/100);
	inimaplatmin = (int)CGPSDlg::gpsDlg->m_gpggaMsg.Latitude-
		int(CGPSDlg::gpsDlg->m_gpggaMsg.Latitude/100)*100;
	inimaplatsec = (int)((CGPSDlg::gpsDlg->m_gpggaMsg.Latitude-
		(int)CGPSDlg::gpsDlg->m_gpggaMsg.Latitude)*60.0);

	inimaplon = inimaplondeg+(double)inimaplonmin/60+(double)inimaplonsec/3600;
	inimaplat = inimaplatdeg+(double)inimaplatmin/60+(double)inimaplatsec/3600;

	lon = m_lon;
	lat = m_lat;

	if(g_setting.specifyCenter && IS_DEBUG)
	{

	}
	else
	{
		if(CGPSDlg::gpsDlg->m_gpggaMsg.Latitude_N_S == 'S')
			m_lat *= -1;
		if(CGPSDlg::gpsDlg->m_gpggaMsg.Longitude_E_W == 'W')
			m_lon *= -1;
	}

	SetRotationMatrix();
	int_N = WGS84_RA / (sqrt(1-WGS84_E2*sin(m_lat)*sin(m_lat)));
	WGS84_X = (int_N+ini_h)*cos(m_lat)*cos(m_lon);
	WGS84_Y = (int_N+ini_h)*cos(m_lat)*sin(m_lon);
	WGS84_Z = (int_N*(1-WGS84_E2)+ini_h)*sin(m_lat);			

	IniPos = false;
	m_isGetMapPos = true;

}

void ScatterData::AddLLAPoint(ScatterPoint* llaPoint)
{
	LockLlaData();
	llaPointSet.push_back(*llaPoint);	
	
	if((int)llaPointSet.size() > g_setting.scatterCount)
	{
		llaPointSet.erase(llaPointSet.begin());
	}
	UnlockLlaData();	
}

void ScatterData::SetENU(double lon, double lat, double h)
{
	double N = WGS84_RA/(sqrt(1 - WGS84_E2 * sin(lat) * sin(lat)));		
	wgs84_X =(long double)((N + h)*cos(lat)*cos(lon));
	wgs84_Y =(long double)((N + h)*cos(lat)*sin(lon));
	wgs84_Z =(long double)((N * (1 - WGS84_E2) + h)*sin(lat));

	POS_T pt = { 0 };
	LLA_T lla = { lat, lon, (F32)h };
	COO_geodetic_to_cartesian(&lla, &pt);

	ENU(0, 0) = (float)(pt.px - WGS84_X);
	ENU(1, 0) = (float)(pt.py - WGS84_Y);
	ENU(2, 0) = (float)(pt.pz - WGS84_Z);
	ENU = _WGS842NEU * ENU;			

	LockEnuData();				
	current_x = ENU(0,0);
	current_y = ENU(1,0);
	enu_x.push_back(ENU(0,0)/1000);
	enu_y.push_back(ENU(1,0)/1000);
	enu_x_mean  = 0;
	enu_y_mean  = 0;

	if(enu_x.size() >= (UINT)g_setting.scatterCount)
	{
		enu_x.erase(enu_x.begin());
		enu_y.erase(enu_y.begin());
	}

	vector<double>::iterator enu_x_it, enu_y_it;	
	for(enu_x_it = enu_x.begin(); enu_x_it != enu_x.end(); ++enu_x_it)
	{				
		enu_x_mean += *enu_x_it;
	}			
	enu_x_mean /= (enu_x.size());	
	unsigned long double sum_error_x = 0.0;
	unsigned long double sum_error_y = 0.0;
	for(enu_x_it = enu_x.begin();enu_x_it!=enu_x.end(); ++enu_x_it)
	{
		sum_error_x += ( (*enu_x_it-enu_x_mean)*(*enu_x_it-enu_x_mean) );
	}
	rms_x = sqrt(sum_error_x/enu_x.size());
	for(enu_y_it = enu_y.begin();enu_y_it!=enu_y.end(); ++enu_y_it)
	{				
		enu_y_mean += *enu_y_it;
	}
	enu_y_mean/=(enu_y.size());			
	for(enu_y_it = enu_y.begin();enu_y_it!=enu_y.end(); ++enu_y_it)
	{				
		sum_error_y += ( (*enu_y_it-enu_y_mean)*(*enu_y_it-enu_y_mean) );
	}			


	rms_y = sqrt(sum_error_y/enu_y.size());
	TwoDrms = (sqrt( rms_x*rms_x + rms_y*rms_y ))*1000;
	CEP =1.1774*((rms_x+rms_y)/2)*1000;
	UnlockEnuData();
}

#define ScatterXScale		50
#define ScatterYScale		50
#define PointSize			2.5f

CPic_Scatter::CPic_Scatter(void)
{
	int i=0, x=0, y=0;
	//定義點圖的範圍大小
	plot_x1 = 45; plot_y1 = 21;	//視窗左上角
	plot_x2 = plot_x1+200; plot_y2 = plot_y1+200;

	plot_cross_x = (plot_x1 + plot_x2) / 2;
	plot_cross_y = (plot_y1 + plot_y2) / 2;

	for(y=plot_y1; y<=plot_y2; y+=ScatterYScale)
	{
		VScatterScale[i].x = plot_x1 + 5;	
		VScatterScale[i].y = y + 2;
		i++;
	}
	
	i=0;	
	for(x=plot_x1;x<=plot_x2;x+=ScatterXScale)
	{	
		HScatterScale[i].x = x;
		HScatterScale[i].y = plot_y2;
		i++;
	}	

}
/*
	int i=0, x=0, y=0;
	//定義點圖的範圍大小
	plot_x1 = 45 ; plot_y1 = 21 ; 
	plot_x2 = plot_x1+160 ; plot_y2 = plot_y1+160 ;
	plot_x2 = plot_x1+160 ; plot_y2 = plot_y1+160 ;

	plot_cross_x = (plot_x1 + plot_x2) / 2;
	plot_cross_y = (plot_y1 + plot_y2) / 2;

	for(y=plot_y1;y<=plot_y2;y+=40)
	{
		VScatterScale[i].x=plot_x1+5;	
		VScatterScale[i].y=y+2;
		i++;
	}
	
	i=0;	
	for(x=plot_x1;x<=plot_x2;x+=40)
	{	
		HScatterScale[i].x=x;
		HScatterScale[i].y=plot_y2;
		i++;
	}
*/
CPic_Scatter::~CPic_Scatter(void)
{
}

BEGIN_MESSAGE_MAP(CPic_Scatter, CStatic)
	ON_WM_PAINT()
END_MESSAGE_MAP()

void CPic_Scatter::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	Refresh_ScatterChart(&dc);
}

void CPic_Scatter::Refresh_ScatterChart(CDC *scatter_dc)
{
	CRect rcScatter;
	GetClientRect(rcScatter);
	CMemDC memDC(*scatter_dc, &rcScatter);
	CBrush bkBrush(RGB(250, 250, 250));
	memDC.GetDC().FillRect(&rcScatter, &bkBrush);

	Create_scatterplot(&(memDC.GetDC()));
	Show_ScatterChart(&(memDC.GetDC()));
}

void CPic_Scatter::Create_scatterplot(CDC *dc)
{
	UISetting* s = &gpUI;
	CSize fontSize;

	CPen dotGray(PS_DOT,1,s->inUseSnrBarPenColor);
	CPen *oldPen = dc->SelectObject(&dotGray);	
	CBrush *oldBrush = dc->SelectObject(&(s->noUseSnrBarBrush));
	CFont *oldFont = dc->SelectObject(&(s->idFont));

	dc->Rectangle(plot_x1 , plot_y1 , plot_x2 , plot_y2);

	int x, y;
	for(y=plot_y1+ScatterXScale;y<plot_y2;y+=ScatterYScale)		
	{
		dc->MoveTo(plot_x1,y);
		dc->LineTo(plot_x2,y);
	}		
	for(x=plot_x1+ScatterXScale;x<plot_x2;x+=ScatterYScale)		
	{
		dc->MoveTo(x,plot_y1);
		dc->LineTo(x,plot_y2);
	}

	////畫十字線
	dc->SelectObject(&(s->inUseSnrBarPen));	
	dc->MoveTo(plot_x1,plot_cross_y);dc->LineTo(plot_x2,plot_cross_y);
	dc->MoveTo(plot_cross_x,plot_y1);dc->LineTo(plot_cross_x,plot_y2);	

	CFont newsFont;
	newsFont.CreatePointFont(120, _T("Arial Black"));
	dc->SelectObject(&newsFont);
	fontSize = CSize(12, 20);

	dc->SetTextColor(RGB(180, 180, 180));
	dc->DrawText("N", CRect(140, 22, 140+fontSize.cx, 21+fontSize.cy), DT_TOP | DT_CENTER);
	dc->DrawText("S", CRect(140, 200, 140+fontSize.cx, 200+fontSize.cy), DT_VCENTER | DT_CENTER);
	dc->DrawText("W", CRect(47, 110, 60+fontSize.cx, 110+fontSize.cy), DT_VCENTER | DT_LEFT);
	dc->DrawText("E", CRect(0, 110, 231+fontSize.cx, 110+fontSize.cy), DT_VCENTER | DT_RIGHT);

	dc->SetTextColor(RGB(50, 50, 50));
	dc->SelectObject(&(s->barFont));

	CFont rulerFont;
	rulerFont.CreatePointFont(90, _T("Arial"));
	dc->SelectObject(&rulerFont);
	fontSize = CSize(10, 18);

	int coor = CGPSDlg::gpsDlg->GetCoordinateSel();
	//Draw the ENU & LLA Coordinate
	if(coor==0)
	{
		CPen pen;
		double penRedColor = 200.0;
		double penBlueColor = 0.0;
		double add = 200.0 / g_scatterData.enu_x.size();

		g_scatterData.LockEnuData();
		vector<double>::iterator enu_x_it, enu_y_it;	
		for(enu_x_it = g_scatterData.enu_x.begin(), enu_y_it = g_scatterData.enu_y.begin(); 
			enu_x_it != g_scatterData.enu_x.end(); 
			++enu_x_it, ++enu_y_it)
		{		
			double x, y;
			x = *enu_x_it * 1000.0F;	    
			y = *enu_y_it * 1000.0F;				
			x = (x * 50 / g_scatterData.m_enuScale) + plot_cross_x;    
			y = plot_cross_y - (y * 50 / g_scatterData.m_enuScale);	
			pen.CreatePen(PS_SOLID, 1, RGB((int)penRedColor, 50, (int)penBlueColor));
			dc->SelectObject(&pen);
			if(x > plot_x1 && x < plot_x2 && y > plot_y1 && y < plot_y2)
			{
				dc->Ellipse((int)(x - PointSize) + 1, (int)(y - PointSize) + 1, (int)(x + PointSize) + 1, (int)(y + PointSize) + 1);
			}
			pen.DeleteObject();
			//penRedColor -= add;
			//penBlueColor += add;
		}		
		g_scatterData.UnlockEnuData();

		int j = 0;
		//int index = m_scale.GetCurSel();	
		int index = CGPSDlg::gpsDlg->GetScaleSel();	
		if(index == LB_ERR)
		{				
			return;
		}
		//int scaleTable[] = { 1, 2, 3, 5, 10, 20, 30, 40, 50, 100, 150, 200, 300};
		//int scale = scaleTable[index];

		j = 2;
		CString str;
		for(int i=0; i<5; ++i)
		{				
			x = HScatterScale[i].x;
			y = HScatterScale[i].y;
			
			//_itoa_s(scale * (i-2), IntToStr, sizeof(IntToStr), 10);
			if(g_scatterData.m_enuScale >= 1.0)
			{
				str.Format("%d", (int)(g_scatterData.m_enuScale * (i-2)));
			}
			else
			{
				str.Format("%.2f", (g_scatterData.m_enuScale * (i-2)));
			}
			//dc->TextOut(x-5, y+3, IntToStr);
			dc->DrawText(str, CRect(x - 25, y + 3, x + 30, y + 23), DT_VCENTER | DT_CENTER);

			x = VScatterScale[i].x;
			y = VScatterScale[i].y;

			//_itoa_s(scale * j, IntToStr, sizeof(IntToStr), 10);
			fontSize = CSize(60, 18);
			if(g_scatterData.m_enuScale >= 1.0)
			{
				str.Format("%d", (int)(g_scatterData.m_enuScale * j));
			}
			else
			{
				str.Format("%.2f", (g_scatterData.m_enuScale * j));
			}

			dc->DrawText(str, CRect(x-33, y-10, x-8, y-10+fontSize.cy), DT_VCENTER | DT_RIGHT);
			j--;
		}
		dc->DrawText("(m)", CRect(plot_x2-23, plot_y2+16, plot_x2+23, plot_y2+46), DT_VCENTER | DT_CENTER);
		dc->DrawText("(m)", CRect(plot_x1-58, plot_y1-22, plot_x1 + 2, plot_y1-2), DT_VCENTER | DT_RIGHT);
	}
	else
	{	
		CPen pen;
		pen.CreatePen(PS_SOLID, 1, RGB(200,50,0));
		dc->SelectObject(&pen);

		ScatterPoint llapoint;

		g_scatterData.LockLlaData();
		vector<ScatterPoint>::const_iterator llaPointSetIter;
		for(llaPointSetIter = g_scatterData.GetLLAPoint().begin(); 
			llaPointSetIter != g_scatterData.GetLLAPoint().end(); ++llaPointSetIter)
		{
			llapoint = *llaPointSetIter;
			llapoint.x += (S16)g_scatterData.mapoffset_x;
			llapoint.y += (S16)g_scatterData.mapoffset_y;
			//*llaPointSetIter = llapoint;
			x = ((llapoint.x - 560) / g_scatterData.m_llaScale) + 560;
			y = ((llapoint.y - 510) / g_scatterData.m_llaScale) + 510;

			if(x > plot_x1 && x < plot_x2 && 
				y > plot_y1 && y < plot_y2)
			{
				dc->Ellipse((int)(x - PointSize) + 1, (int)(y - PointSize) + 1,
					(int)(x + PointSize) + 1, (int)(y + PointSize) + 1);
			}
		}
		g_scatterData.UnlockLlaData();


		int index = CGPSDlg::gpsDlg->GetMapScaleSel();
		if(index==LB_ERR)
		{
			return;
		}
		int scaleTable[] = { 1, 2, 3, 5, 10, 1, 2, 3, 5, 10};
		int scale = scaleTable[index];

		//dc->TextOut(plot_x1+170,plot_y1+220,"Longitude");
		dc->DrawText("Longitude", CRect(plot_x1 + 145, plot_y1 + 200, 
			plot_x1 + 145 + 60, plot_y1 + 200 + 20), DT_VCENTER | DT_CENTER);
		//dc->TextOut(plot_x1-60,plot_y1-20,"Latitude");	
		dc->DrawText("Latitude", CRect(plot_x1 - 50, plot_y1 - 20,
			plot_x1 + 10, plot_y1 + 0), DT_VCENTER | DT_CENTER);

		CString str;
		CFont llaRulerFont;
		llaRulerFont.CreatePointFont(70, _T("Arial"));
		dc->SelectObject(&llaRulerFont);
		fontSize = CSize(10, 18);
		const int CoorXRulerOffst = 16;

		if(!g_scatterData.m_isGetMapPos)
		{
			for(int i = 0; i < 5; ++i)
			{
				//int a;
				CString hs;
				x = HScatterScale[i].x;
				y = HScatterScale[i].y;
				if(index <= 4)
				{                            
					int a = i * scale + 31;
					if(a < 0)
					{
						a += 60; 
						hs = "120 59'";
					}
					else if(a > 60)
					{ 
						a -= 60;
						hs = "120 1'";
					}
					else if(a == 60)
					{
						a = 0;
						hs = "120 0'";
					}
					else
					{
						hs = "120 0'";
					}
					//_itoa_s(a, IntToStr, sizeof(IntToStr), 10);
					//hs += IntToStr;
					//hs += "''";
					//dc->TextOut(x-15,y+8,hs);
					str.Format("%s%d''", hs, a);
					dc->DrawText(str, CRect(x - 15, y + 5, 
						x + 15, y + 25), DT_VCENTER | DT_CENTER);
					
					x = VScatterScale[i].x;
					y = VScatterScale[i].y;

					a = i * scale + 4;
					if(a < 0)
					{
						a += 60;
						hs = "24 46'";
					}
					else if(a > 60)
					{
						a -= 60;
						hs = "24 48'";
					}
					else if(a == 60)
					{
						a = 0;
						hs = "24 47'";
					}
					else
					{
						hs = "24 47'";
					}
					//_itoa_s(a, IntToStr, sizeof(IntToStr), 10);
					//hs += IntToStr;
					//hs += "''";
					//dc->TextOut(x-39,y-5,hs);
					str.Format("%s%d''", hs, a);
					dc->DrawText(str, CRect(x - 50, y - 7, 
						x - 9, y + 17), DT_VCENTER | DT_RIGHT);
				}
				else
				{							
					int a = i * scale;
					if(a < 0)
					{
						a += 60;
						hs = "119 ";
					}
					else if(a > 60)
					{ 
						a -= 60;
						hs = "121 ";
					}
					else if(a == 60)
					{	
						a = 0;
						hs = "120 ";
					}
					else
					{
						hs = "120 ";
					}
					//_itoa_s(a, IntToStr, sizeof(IntToStr), 10);
					//hs += IntToStr;
					//hs += "'";
					//dc->TextOut(x-15,y+8,hs);
					str.Format("%s%d'", hs, a);
					dc->DrawText(str, CRect(x - 15, y + 5, 
						x + 15, y + 25), DT_VCENTER | DT_CENTER);

					x=VScatterScale[i].x;
					y=VScatterScale[i].y;

					a = 50 - i * scale;
					if(a < 0)
					{
						a += 60;
						hs = "23 ";
					}
					else if(a > 60)
					{
						a -= 60;
						hs = "25 ";
					}
					else if(a == 60)
					{
						a = 0;
						hs = "24 ";
					}
					else
					{
						hs = "24 ";
					}
					//_itoa_s(a, IntToStr, sizeof(IntToStr), 10);
					//hs += IntToStr;
					//hs += "'";
					//dc->TextOut(x-30,y-5,hs);
					str.Format("%s%d''", hs, a);
					dc->DrawText(str, CRect(x - 50, y - 7, 
						x - 9, y + 17), DT_VCENTER | DT_RIGHT);
				}
			}			
		}
		else
		{
			for(int i = 0; i < 5; ++i)
			{
				CString hs;
				x=HScatterScale[i].x;
				y=HScatterScale[i].y;
				int deg, min, sec;
				if(index <= 4)
				{  
					min = g_scatterData.inimaplonmin;
					int a = g_scatterData.inimaplonsec - scale * 2 + (i * scale);

					if(a < 0)
					{
						a += 60;	
						min =  g_scatterData.inimaplonmin - 1; 
					}
					else if(a > 60)
					{
						a -= 60; 
						min =  g_scatterData.inimaplonmin + 1; 
					}
					else if(a == 60)
					{
						a = 0;	
					}

					hs.Format("%d %d'%d''",  g_scatterData.inimaplondeg, min, a);
					//dc->TextOut(x-15, y+8, hs);
					dc->DrawText(hs, CRect(x - 26, y + CoorXRulerOffst, 
						x + 10, y + CoorXRulerOffst + 16), DT_VCENTER | DT_CENTER);

					x=VScatterScale[i].x;
					y=VScatterScale[i].y;


					min =  g_scatterData.inimaplatmin;
					a =  g_scatterData.inimaplatsec+scale*2-(i*scale);
					if(a<0)
					{
						a+=60;	
						min= g_scatterData.inimaplatmin-1; 
					}
					else if(a>60)
					{
						a-=60;	 
						min= g_scatterData.inimaplatmin+1;
					}
					else if(a==60)
					{
						a=0;
					}

					hs.Format("%d %d'%d''",  g_scatterData.inimaplatdeg, min, a);
					//dc->TextOut(x-39, y-5, hs);
					dc->DrawText(hs, CRect(x - 48, y - 6, 
						x - 1, y - 6 + 16), DT_VCENTER | DT_CENTER);				
				}
				else
				{     
					deg= g_scatterData.inimaplondeg;
					int a= g_scatterData.inimaplonmin-scale*2+(i*scale);

					if(a<0)
					{
						a+=60; 
						deg= g_scatterData.inimaplondeg-1; 
					}
					else if(a>60)
					{ 
						a-=60; 
						deg= g_scatterData.inimaplondeg+1; 
					}
					else if(a==60)
					{
						a=0;	
					}
					sec =  g_scatterData.inimaplonsec;

					hs.Format("%d %d' %d''", deg, a, sec);
					//dc->TextOut(x-17,y + CoorXRulerOffst,hs);
					dc->DrawText(hs, CRect(x - 32, y + CoorXRulerOffst, 
						x + 16, y + CoorXRulerOffst + 16), DT_VCENTER | DT_CENTER);

					x=VScatterScale[i].x;
					y=VScatterScale[i].y;			
					deg= g_scatterData.inimaplatdeg;
					a= g_scatterData.inimaplatmin+scale*2-(i*scale);
					if(a<0)
					{ 
						a+=60; 
						deg= g_scatterData.inimaplatdeg-1;
					}
					else if(a>60)
					{
						a-=60; 
						deg= g_scatterData.inimaplatdeg+1;
					}
					else if(a==60)
					{
						a=0;	
					}

					sec= g_scatterData.inimaplatsec;
					hs.Format("%d %d'%d''", deg, a, sec);
					//dc->TextOut(x-30,y-5,hs);
					dc->DrawText(hs, CRect(x - 48, y - 6, 
						x - 1, y - 6 + 16), DT_VCENTER | DT_CENTER);				
				}
			}
		}
	}

	//if(IS_DEBUG)
	{
		DrawScatterInfo(dc);
	}
	dc->SelectObject(oldPen);
	dc->SelectObject(oldBrush);
	dc->SelectObject(oldFont);
}

void CPic_Scatter::DrawScatterInfo(CDC *dc)
{
	CString txt;
	
	dc->SetTextColor(RGB(30, 30, 30));
	const int txt1X = 52, txt1Y = 2;
	//const int txt2X = 150, txt2Y = 2;
	const int txtW = 212, txtH = 13;

	txt.Format("Point count : %d / %d", g_scatterData.GetLLAPoint().size(), g_setting.scatterCount);
	dc->DrawText(txt, CRect(txt1X, txt1Y, txt1X + txtW, txt1Y + txtH), DT_TOP | DT_LEFT);
	//txt.Format("CEP 50%% : %.4f", g_scatterData.CEP);
	//dc->DrawText(txt, CRect(txt2X, txt2Y, txt2X + txtW, txt2Y + txtH), DT_TOP | DT_LEFT);

}

void CPic_Scatter::DrawScatterAltitude(CDC *dc, double initH , double h)
{	
	UISetting* s = &gpUI;

	CPen *oldPen = dc->SelectObject(&(s->inUseSnrBarPen));
	CBrush br(s->inUseSnrBarPenColor);
	//CBrush *oldBrush = dc->SelectObject(&br);
	//CBrush *oldBrush = dc->SelectObject(&(gpUI.noUseSnrBarBrush));

	dc->MoveTo(247, 21); dc->LineTo(256, 21);
	dc->MoveTo(248, 71); dc->LineTo(257, 71);
	dc->MoveTo(247, 121); dc->LineTo(262, 121);
	dc->MoveTo(248, 171); dc->LineTo(257, 171);
	dc->MoveTo(247, 220); dc->LineTo(256, 220);

	//dc->FillRect(CRect(249, 21, 254, 221), &(s->inUseSnrBarBrush));	//h == (initH + 2 * g_scatterData.m_enuScale)
	//dc->FillRect(CRect(249, 121, 254, 221), &(s->inUseSnrBarBrush));	//h == initH
	//dc->FillRect(CRect(249, 220, 254, 221), &(s->inUseSnrBarBrush));	//h == (initH - 2 * g_scatterData.m_enuScale)
//g_scatterData.m_enuScale;
	dc->FillRect(CRect(249, 21, 254, 221), &br);	//h == (initH + 2 * g_scatterData.m_enuScale)
	double h1 = initH + 2 * g_scatterData.m_enuScale;
	const int barHeight = 200;
	int bar = (int)(barHeight * (h1 - h) / (4 * g_scatterData.m_enuScale));
	if(bar < 0)
	{
		bar = 0;
	}
	if(bar <= barHeight)
	{
		dc->FillRect(CRect(249, 21 + bar, 254, 221), &(s->inUseSnrBarBrush));	//h == (initH - 2 * g_scatterData.m_enuScale)
	}

	//dc->SelectObject(oldBrush);
	dc->SelectObject(oldPen);
}

void CPic_Scatter::Show_ScatterChart(CDC *dc)
{
	if(false==CGPSDlg::gpsDlg->IsFixed())
	{
		return;
	}

	if(CGPSDlg::gpsDlg->m_gpggaMsg.Longitude == 0.0)
	{
		g_scatterData.m_isGetMapPos = false;
		return;
	}

	int coor = CGPSDlg::gpsDlg->GetCoordinateSel();
	int index = 0;
	int mapindex = 0;
	if(coor == 0)
	{
		index = CGPSDlg::gpsDlg->GetScaleSel();
	}
	else if(coor == 1) 
	{
		mapindex = CGPSDlg::gpsDlg->GetMapScaleSel();
	}

	CPen pen;
	pen.CreatePen(PS_SOLID,1,RGB(50, 50, 255));
	CPen* oldPen = dc->SelectObject(&pen);

	CBrush clear;		
	clear.CreateSolidBrush(RGB(50, 50, 255));
	CBrush*oldBrush = dc->SelectObject(&clear);	        

	CFont font;
	font.CreatePointFont(90, _T("Arial"));
	CFont* oldFont = dc->SelectObject(&font);
	dc->SetTextColor(RGB(0,0,192));
	if(g_scatterData.IniPos)
	{	
		g_scatterData.InitPos();
	}
	else
	{
		int ENUOrigin_X = plot_cross_x;
		int ENUOrigin_Y = plot_cross_y;	


		//double h = CGPSDlg::gpsDlg->m_gpggaMsg.Altitude;

		double lon = int(CGPSDlg::gpsDlg->m_gpggaMsg.Longitude  / 100);
		lon += double(CGPSDlg::gpsDlg->m_gpggaMsg.Longitude -
			int(CGPSDlg::gpsDlg->m_gpggaMsg.Longitude / 100) * 100) / 60;
		double lat = int(CGPSDlg::gpsDlg->m_gpggaMsg.Latitude / 100);
		lat += double(CGPSDlg::gpsDlg->m_gpggaMsg.Latitude -
			int(CGPSDlg::gpsDlg->m_gpggaMsg.Latitude / 100) * 100) / 60;
		double alt = CGPSDlg::gpsDlg->m_gpggaMsg.Altitude + CGPSDlg::gpsDlg->m_gpggaMsg.GeoidalSeparation;

		g_scatterData.SetMapLocation(CGPSDlg::gpsDlg->m_gpggaMsg.Longitude, CGPSDlg::gpsDlg->m_gpggaMsg.Latitude);

		//long double map_x;
		//long double map_y;		
		double map_x = lon-g_scatterData.inimaplon;				   
		double map_y = lat-g_scatterData.inimaplat;				
		map_x*=(3600*50);				
		map_y*=(3600*50);			
		map_x=ENUOrigin_X+map_x;
		map_y=ENUOrigin_Y-map_y;

		ScatterPoint LLApoint;		
		LLApoint.x=(S16)map_x;
		LLApoint.y=(S16)map_y;
		g_scatterData.AddLLAPoint(&LLApoint);

		g_scatterData.LockEnuData();									
		g_scatterData.lon_deg = lon;
		g_scatterData.lat_deg = lat;
		g_scatterData.UnlockEnuData();

		map_x = lon - g_scatterData.inimaplon;				   
		map_y = lat - g_scatterData.inimaplat;

		map_x *= (3600 * 50 / g_scatterData.m_llaScale);				
		map_y *= (3600 * 50 / g_scatterData.m_llaScale);			
		map_x = ENUOrigin_X + map_x;
		map_y = ENUOrigin_Y - map_y;
	
		if(CGPSDlg::gpsDlg->m_gpggaMsg.Latitude_N_S == 'S')
			if(lat>0)lat*=-1;
		if(CGPSDlg::gpsDlg->m_gpggaMsg.Longitude_E_W == 'W')
			if(lon>0)lon*=-1;

		lon = Deg2Rad(lon);
		lat = Deg2Rad(lat);
		g_scatterData.SetENU(lon, lat, alt);

		double plot_x = (g_scatterData.ENU(0,0)*50/g_scatterData.m_enuScale)+ENUOrigin_X;
		double plot_y = ENUOrigin_Y-(g_scatterData.ENU(1,0)*50/g_scatterData.m_enuScale);

		if(coor==0)
		{
			if(plot_x>plot_x1 && plot_x<plot_x2 && plot_y>plot_y1 && plot_y<plot_y2)			   	
				dc->Ellipse((int)(plot_x-PointSize) + 1, (int)(plot_y-PointSize) + 1, (int)(plot_x+PointSize) + 1, (int)(plot_y+PointSize) + 1);
		}
		else
		{
			if(map_x>plot_x1 && map_x<plot_x2 && map_y>plot_y1 && map_y<plot_y2)
			{
				dc->Ellipse((int)(map_x-PointSize) + 1, (int)(map_y-PointSize) + 1, (int)(map_x+PointSize) + 1, (int)(map_y+PointSize) + 1);
			}
		}

		DrawScatterInfo(dc);
		if(IS_DEBUG)
		{
			DrawScatterAltitude(dc, g_scatterData.ini_h, alt);
		}

		//CString s;
		//s.Format("iniH : Alt - %8.2f : %8.2f\r\n", g_scatterData.ini_h, alt);
		//::OutputDebugString(s);

	}

	dc->SelectObject(oldPen);
	dc->SelectObject(oldBrush);
	dc->SelectObject(oldFont);
}


