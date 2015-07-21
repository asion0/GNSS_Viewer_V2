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
	void Init(const char *name, int path_color, bool kml3d = false);
	void PushOnePoint(double lon, double lat, double alt);
	void PushOnePoi(double lon, double lat, double alt);
	void Finish();
	
private:
	CFile kmlFile;
	bool iniWriteKml;
	bool convert3d;
	int lineColor;

	std::vector<LL1> lst_poi;
	LL1 start_point, last_point;
	void WriteKMLini(CFile& kmlFile);
	void WriteKMLPath(CFile& kmlFile, double lon, double lat, double alt);
	void WritePOIPath(CFile& kmlFile , std::vector<LL1> *lst);
	void AddStartPoint(CFile& kmlFile);
	void AddEndPoint(CFile& kmlFile);
};
