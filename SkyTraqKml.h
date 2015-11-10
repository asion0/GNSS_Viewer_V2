#pragma once
#include <vector>

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
	void Init(const char *name, int color, bool kml3d = false, bool bPointList = false, bool bNoPointText = false);
	void PushOnePoint(double lon, double lat, double alt, const CString& ts, QualityMode q);
	void PushOnePoi(double lon, double lat, double alt);
	void Finish();
	
private:
	CFile kmlFile;
	bool iniWriteKml;
	bool convert3d;
	bool pointList;
	bool noPointText;
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
};
