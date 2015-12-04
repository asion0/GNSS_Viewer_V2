#pragma once
#include <vector>
#include "NMEA.h"

typedef struct 
{
	double lat;
	double lon;
	double alt;
} LL1;

class CSkyTraqKml
{
public:
	CSkyTraqKml(void);
	~CSkyTraqKml(void);

	//void SetFileName(const char *name);
	//void Init(const char *name, int path_color, bool kml3d = false);
	void Init(const char *name, int color, bool kml3d = false, bool bPointList = false, bool bNoPointText = false, bool bDetailInfo = false);
	void PushOnePoint(double lon, double lat, double alt, const CString& ts, QualityMode q);
	void PushOnePoi(double lon, double lat, double alt);
	void Finish();
	
    GPGSA *msg_gpgsa;
    GPGSA *msg_glgsa;
    GPGSA *msg_bdgsa;
    GPGSA *msg_gagsa;
	
    GPGSV *msg_gpgsv;
    GPGSV *msg_glgsv;
    GPGSV *msg_bdgsv;
    GPGSV *msg_gagsv;

	Satellite* satellites_gps;
	Satellite* satellites_gnss;
	Satellite* satellites_bd;
	Satellite* satellites_ga;
private:
	CFile kmlFile;
	bool iniWriteKml;
	bool convert3d;
	bool pointList;
	bool noPointText;
	bool detailInfo;
	int lineColor;
	CString strPointList;
	CList<CString, CString> pls;

	std::vector<LL1> lst_poi;
	LL1 start_point, last_point;
	void WriteKMLini(CFile& kmlFile);
	void WriteKMLPath(CFile& kmlFile, double lon, double lat, double alt, const CString& ts, QualityMode q);
	void WritePOIPath(CFile& kmlFile , std::vector<LL1> *lst);
	void AddStartPoint(CFile& kmlFile);
	void AddEndPoint(CFile& kmlFile);
	char CheckGsa(int p, GPGSA *gsa);
	CString GenerateSatelliteTable(Satellite* s, GPGSA *gsa);
	CString GetSatelliteInfo();

};
