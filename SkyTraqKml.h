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
	void RealPushOnePoint2(double lon, double lat, double alt, const F32* speed, const F32* degree, const CString& ts, QualityMode q);
  void PushOnePoint2(double lon, double lat, double alt, const F32* speed, const F32* degree, const CString& ts, QualityMode q);
	void PushOnePoi(double lon, double lat, double alt);
	void Finish();
	void Finish2();
	
  GPGSA *msg_gpgsa;
  GPGSA *msg_glgsa;
  GPGSA *msg_bdgsa;
  GPGSA *msg_gagsa;
  GPGSA *msg_gigsa;

  GPGSV *msg_gpgsv;
  GPGSV *msg_glgsv;
  GPGSV *msg_bdgsv;
  GPGSV *msg_gagsv;
  GPGSV *msg_gigsv;

	Satellites* satellites_gp;
	Satellites* satellites_gl;
	Satellites* satellites_bd;
	Satellites* satellites_ga;
	Satellites* satellites_gi;
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
	void WriteKMLPath2(CFile& kmlFile, double lon, double lat, double alt, const F32* speed, const F32* degree, const CString& ts, QualityMode q);
	void WritePOIPath(CFile& kmlFile , std::vector<LL1> *lst);
	void AddStartPoint(CFile& kmlFile);
	void AddEndPoint(CFile& kmlFile);
	char CheckGsa(int p, GPGSA *gsa);
	CString GenerateSatelliteTable(Satellites* s, GPGSA *gsa);
	CString GetSatelliteInfo();

};
