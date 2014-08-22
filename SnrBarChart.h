#pragma once

struct GPGSA;
struct GPGSV;
struct GPGGA;
struct Satellite;
class CMaskedBitmap;
class UISetting;

class CSnrBarChart : public CStatic
{
public:
	enum SnrBarChartMode
	{
		Gps,
		Gnss,
		Beidou,
		Galileo
	};

	CSnrBarChart(void);
	~CSnrBarChart(void);

	void SetGsvData(GPGSV* gsv) { gsvData = gsv; };
	void SetGsaData(GPGSA* gsa) { gsaData = gsa; };
	void SetGgaData(GPGGA* gga) { ggaData = gga; };
	void SetSateStatus(Satellite* sata) { sateStatus = sata; };
//	void SetSnrBarChartMode(SnrBarChartMode mode) { barChartMode = mode; };
	void SetUISetting(UISetting* p) { uiSetting = p; }
	static void DrawBarChartId(CDC *dc, UISetting* s, bool isInUse, int x, int y, int id, bool inCenter = false);
	void SetCustomerID(U32 id) { customerID = id; };
protected:
	GPGSV* gsvData;
	GPGSA* gsaData;
	GPGGA* ggaData;
	Satellite* sateStatus;
	UISetting* uiSetting;
	U32	customerID;

	int m_minId;
	int m_maxId;	CString m_titleText;

	CPoint m_startBarChart;

	virtual void ShowBoxChart(CDC *dc);
	void DrawSnr(CDC *dc, int& startId, UISetting* s, GPGSV* gsv, GPGSA* gsa, 
				GPGGA* gga, Satellite* sate);
	void DrawSnrInRange(CDC *dc, int& startId, UISetting* s, GPGSV* gsv, 
				GPGSA* gsa, GPGGA* gga, Satellite* sate, int start, int end);

	void RefreshBarChart();
	void CreateBarChart(CDC *dc);


	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()
};

class CSnrBarChartGps : public CSnrBarChart
{
public:
	CSnrBarChartGps(void);
	~CSnrBarChartGps(void);

protected:
	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()
};

class CSnrBarChartGlonass : public CSnrBarChart
{
public:
	CSnrBarChartGlonass(void);
	~CSnrBarChartGlonass(void);

protected:
	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()
};


class CSnrBarChartGpsGlonass : public CSnrBarChart
{
public:
	CSnrBarChartGpsGlonass(void);
	~CSnrBarChartGpsGlonass(void);

	void SetGsvData2(GPGSV* gsv) { gsvData2 = gsv; };
	void SetGsaData2(GPGSA* gsa) { gsaData2 = gsa; };
	void SetGgaData2(GPGGA* gga) { ggaData2 = gga; };
	void SetSateStatus2(Satellite* sata) { sateStatus2 = sata; };
	void SetUISetting2(UISetting* p) { uiSetting2 = p; }

protected:
	GPGSV* gsvData2;
	GPGSA* gsaData2;
	GPGGA* ggaData2;
	Satellite* sateStatus2;
	UISetting* uiSetting2;

	virtual void ShowBoxChart(CDC *dc);
	void DrawGpsSnr(CDC *dc, int& end);
	void DrawGlonassSnr(CDC *dc, int& start);
	bool CheckInUse2(int id);

	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()
};

class CSnrBarChartBeidou : public CSnrBarChart
{
public:
	CSnrBarChartBeidou(void);
	~CSnrBarChartBeidou(void);

protected:
	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()
};

class CSnrBarChartGalileo : public CSnrBarChart
{
public:
	CSnrBarChartGalileo(void);
	~CSnrBarChartGalileo(void);



protected:
	afx_msg void OnPaint();

	DECLARE_MESSAGE_MAP()
};
