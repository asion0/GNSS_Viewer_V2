#pragma once
#include <vector>

typedef struct {
	double lat;
	double lon;
} LL1;

class CSkyTraqKml1
{
public:
	CSkyTraqKml1(void);
	~CSkyTraqKml1(void);

	void init(const char *name, int path_color);
	void set_filename(char *name);
	void push_one_point(double lon, double lat);
	void push_one_poi(double lon, double lat);
	void finish();
	
private:
	CFile FKml;
	std::vector<LL1> lst_poi;
	int line_color;
	LL1 start_point,last_point;
	bool iniWriteKml ;
	void WriteKMLini(CFile& Fnmea, double lon,double lat);
	void WriteKMLPath(CFile& Fnmea, double lon,double lat);
	void WritePOIPath(CFile& Fnmea , std::vector<LL1> *lst);
	void add_start_point(CFile& Fnmea);
	void add_end_point(CFile& Fnmea);
};

struct LL2;
class CSkyTraqKml2
{
public:
	CSkyTraqKml2(void);
	~CSkyTraqKml2(void);

	void init(const char *name, int path_color);
	void set_filename(char *name);
	void push_one_point(LL2 *lla);
	void push_one_poi(LL2 *lla);
	void finish();

private:
	CFile FKml;
	std::vector<LL2> lst_poi;
	std::vector<LL2> points;
	int line_color;
	
	void WriteKMLini(CFile& Fnmea);
	void WriteKMLPath(CFile& Fnmea, double lat,double lon);
	void WritePOIPath(CFile& Fnmea , std::vector<LL2> *lst);
	void add_start_point(CFile& Fnmea, std::vector<LL2> *lst);
	void add_end_point(CFile& Fnmea, std::vector<LL2> *lst);
	void WritePointPath(CFile& Fnmea, std::vector<LL2> *lst);
	void WritePointLine(CFile& Fnmea, std::vector<LL2> *lst);
	void WritePointLineEnd(CFile& Fnmea);
	void WriteKMLEnd(CFile& Fnmea);
};
