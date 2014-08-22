#pragma once


struct ScatterPoint
{
	S16 x;
	S16 y;
};

class ScatterData 
{
public:
	ScatterData() 
	{	
		ClearData();
	};
	~ScatterData() {};

	void SetOrigin();

	CCriticalSection _GET_ENU_POINT_CS;
	CCriticalSection _GET_LLA_POINT_CS;	

	vector<float> enu_x;	
	vector<float> enu_y;

	float m_enuScale;
	int m_llaScale;

	float mapoffset_x;
	float mapoffset_y;
	bool  m_isGetMapPos;

	int inimaplondeg, inimaplonmin, inimaplonsec, inimaplatdeg, inimaplatmin, inimaplatsec;
	long double  inimaplon,    inimaplat,    initiallon,   initiallat;
	bool IniPos;
	double m_lon;
	double m_lat;
	long double WGS84_X;//initial position
	long double WGS84_Y;//initial position
	long double WGS84_Z;//initial position
	float ini_h; //initial data	    
	double int_N; //initial data
	CCriticalSection _GETENUPOINTCS;
	double lon_deg;
	double lat_deg;
	Matrix _WGS842NEU;
	Matrix ENU;
	float current_x;
	float current_y;
    int maplondeg_t,  maplonmin_t,  maplonsec_t,  maplatdeg_t,  maplatmin_t,  maplatsec_t;
    long double  maplon_t, maplat_t, lon_t, lat_t, pmlon,  pmlat;
	long double X;
	long double Y;
	long double Z;
	double      CEP;
    double      TwoDrms;
	long double rms_x;
    long double rms_y;
	float enu_y_mean;
	float enu_x_mean;

	void ClearData()
	{
		_WGS842NEU.SetSize(3, 3);	//
		m_lon = 0;////
		m_lat = 0;////
		mapoffset_x = 0;
		mapoffset_y = 0;	
		m_isGetMapPos = false;	

		m_lon = 0.0;	
		m_lat = 0.0;
		WGS84_X = 0.0;	
		WGS84_Y = 0.0;	
		WGS84_Z = 0.0;
		ini_h = 0.0;	
		int_N = 0.0;
		
		X = Y = Z = 0.0;
		ENU.SetSize(3, 1);
		ENU(0, 0) = 0.0;
		ENU(1, 0) = 0.0;
		ENU(2, 0) = 0.0;
		IniPos = true;
		CEP = 0.0;  
		TwoDrms = 0.0;
		rms_x = 0.0;	
		rms_y = 0.0;
		enu_y_mean = enu_x_mean = 0.0F;
		Clear();
	}

	void ChangeCoordinateLLA()
	{
		//_cprintf("==%f, %f , %f , %f",inimaplon, maplon_t, inimaplat, maplat_t);
		//_GETNMEA0183CS.Lock();
		if( inimaplondeg != maplondeg_t || inimaplonmin != maplonmin_t || 
			inimaplonsec != maplonsec_t || inimaplatdeg != maplatdeg_t || 
			inimaplatmin != maplatmin_t || inimaplatsec != maplatsec_t)
		{
			int dif_x,dif_y;			
			dif_x = maplonsec_t - inimaplonsec;
			dif_y = maplatsec_t - inimaplatsec;
			mapoffset_x = (float)(50*(-1*dif_x));
			mapoffset_y = (float)(50*(dif_y));			
		}
		else
		{
			mapoffset_x=0;
			mapoffset_y=0;
		}
		inimaplon     = maplon_t;
		inimaplat     = maplat_t;		
		inimaplondeg  = maplondeg_t;
		inimaplonmin  = maplonmin_t;
		inimaplonsec  = maplonsec_t;
		inimaplatdeg  = maplatdeg_t;
		inimaplatmin  = maplatmin_t;
		inimaplatsec  = maplatsec_t;
		//_GETNMEA0183CS.Unlock();		
	}

	void ChangeENUScale(int index)
	{
#if MORE_ENU_SCALE
		const float enuTable[] = { 0.1F, 0.2F, 0.5F, 1.0F, 2.0F, 3.0F, 5.0F, 
			10.0F, 20.0F, 30.0F, 40.0F, 50.0F, 100.0F, 150.0F, 200.0F, 300.0F };
#else
		const int enuTable[] = { 1, 2, 3, 5, 10, 20, 30, 40, 50, 100, 150, 200, 300 };
#endif
		m_enuScale = enuTable[index];
	}
	void ChangeLLAScale(int index)
	{
		const int llaTable[] = { 1, 2, 3, 5, 10, 60, 120, 180, 300, 600 };
		mapoffset_x = 0;
		mapoffset_y = 0;
		m_llaScale = llaTable[index];
	}
	void SetMapLocation(double lon, double lat)
	{
		maplondeg_t = int(lon/100);
		maplonmin_t = (int)lon-int(lon/100)*100;
		maplonsec_t = (int)((lon-(int)lon)*60.0);

		maplatdeg_t = int(lat/100);
		maplatmin_t = (int)lat-int(lat/100)*100;
		maplatsec_t = (int)((lat-(int)lat)*60.0);

		maplon_t = maplondeg_t+(double)maplonmin_t/60+(double)maplonsec_t/3600;
		maplat_t = maplatdeg_t+(double)maplatmin_t/60+(double)maplatsec_t/3600;  
	}
	void Clear()
	{
		_GET_LLA_POINT_CS.Lock();			
		llaPointSet.clear();			
		_GET_LLA_POINT_CS.Unlock();		

		_GET_ENU_POINT_CS.Lock();
		enu_x.clear();
		enu_y.clear();	
		_GET_ENU_POINT_CS.Unlock();
	}

	void SetENU(double lon, double lat, double h);
	void InitPos();
	void AddLLAPoint(ScatterPoint* llaPoint);
	const vector<ScatterPoint>& GetLLAPoint() const
	{ return llaPointSet; };

protected:
	vector<ScatterPoint> llaPointSet;	

	void SetRotationMatrix();

};
extern ScatterData g_scatterData;

class CPic_Scatter : public CStatic
{
public:
	CPic_Scatter(void);
	~CPic_Scatter(void);
	
protected:
	void Refresh_ScatterChart(CDC *scatter_dc);
	void Create_scatterplot(CDC *dc);
	void Show_ScatterChart(CDC *dc);


	int plot_x1,plot_x2,plot_y1,plot_y2;
	int plot_cross_x,plot_cross_y;
    CPoint HScatterScale[5];
    CPoint VScatterScale[5];



	DECLARE_MESSAGE_MAP()
	afx_msg void OnPaint();
};
