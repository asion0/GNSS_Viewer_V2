#include "StdAfx.h"
#include "SkytraqKml.h"
#include "GPSDlg.h"

using namespace std;
CSkyTraqKml::CSkyTraqKml(void)
{
	iniWriteKml = false;
}

CSkyTraqKml::~CSkyTraqKml(void)
{

}

void CSkyTraqKml::Init(const char *name, int color, bool kml3d)
{
	kmlFile.Open(name, CFile::modeReadWrite | CFile::modeCreate);	
	lineColor = color;
	iniWriteKml = true;
	convert3d = kml3d;
}

void CSkyTraqKml::PushOnePoint(double lon, double lat, double alt)
{
	if(iniWriteKml)
	{
		start_point.lon = lon;
		start_point.lat = lat;
		start_point.alt = alt;
		WriteKMLini(kmlFile);
		iniWriteKml = false;
	}
	WriteKMLPath(kmlFile, lon, lat, alt);

	last_point.lat = lat;
	last_point.lon = lon;
	last_point.alt = alt;
}

void CSkyTraqKml::PushOnePoi(double lon, double lat, double alt)
{
	LL1 tmpLla;
	tmpLla.lat = lat;
	tmpLla.lon = lon;
	tmpLla.alt = alt;
	lst_poi.push_back(tmpLla);
}

void CSkyTraqKml::AddStartPoint(CFile& f)
{	
	CString str;
	str = "<Placemark id=\"Start Point\">\r\n    <styleUrl>#POI_STYLE</styleUrl>\r\n    <name>Start Point</name>\r\n    <LookAt>\r\n";
	f.Write(str, (U16)str.GetLength());

	str.Format("    <longitude>%.6f</longitude>\r\n    <latitude>%.6f</latitude>\r\n    </LookAt>\r\n        <Point>\r\n", start_point.lon, start_point.lat);
	f.Write(str, (U16)str.GetLength());

	str.Format("        <coordinates>%.6f,%.6f</coordinates>\r\n    </Point>\r\n</Placemark>\r\n",start_point.lon,start_point.lat);
	f.Write(str, (U16)str.GetLength());
}

void CSkyTraqKml::AddEndPoint(CFile& f)
{	
	CString str;
	str = "<Placemark id=\"End Point\">\r\n    <styleUrl>#POI_STYLE</styleUrl>\r\n    <name>End Point</name>\r\n    <LookAt>\r\n";
	f.Write(str, (U16)str.GetLength());
	
	str.Format("    <longitude>%.6f</longitude>\r\n    <latitude>%.6f</latitude>\r\n    </LookAt>\r\n        <Point>\r\n", last_point.lon, last_point.lat);
	f.Write(str, (U16)str.GetLength());

	str.Format("        <coordinates>%.6f,%.6f</coordinates>\r\n    </Point>\r\n</Placemark>\r\n", last_point.lon, last_point.lat),
	f.Write(str, (U16)str.GetLength());
}


void CSkyTraqKml::Finish()
{
	CString str;
	if(lst_poi.size() > 0)
	{
		str = "    </coordinates>\r\n  </LineString>\r\n</Placemark>\r\n";
		kmlFile.Write(str, (U16)str.GetLength());

		WritePOIPath(kmlFile, &lst_poi);

		str = "</kml>";
		kmlFile.Write(str, (U16)str.GetLength());

		lst_poi.clear();
	}
	else
	{
		if (!iniWriteKml)
		{
			str = "    </coordinates>\r\n  </LineString>\r\n</Placemark>\r\n";
			kmlFile.Write(str, (U16)str.GetLength());
		}
		AddStartPoint(kmlFile);
		AddEndPoint(kmlFile);

		if (!iniWriteKml)
		{
			str = "</Folder>\r\n</kml>\r\n";
			kmlFile.Write(str, (U16)str.GetLength());
		}
	}
	kmlFile.Close();
}

void CSkyTraqKml::WriteKMLini(CFile& f)
{
	/*<?xml version="1.0" encoding="UTF-8"?>
	<kml xmlns="http://earth.google.com/kml/2.1">*/
	CString str;
	str = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n<kml xmlns=\"http://earth.google.com/kml/2.1\">\r\n";
	str += "<Style id=\"POI_STYLE\">\r\n    <IconStyle>\r\n    <color>ff00ff00</color>\r\n    <scale>1.1</scale>\r\n";
	str += "    <Icon><href>http://maps.google.com/mapfiles/kml/pal3/icon21.png</href></Icon>\r\n    </IconStyle>\r\n</Style>\r\n";
	str += "<Folder id=\"Data logger\">\r\n    <name>Data logger</name>\r\n";
	str += "<Placemark id=\"logger\">\r\n  <styleUrl>#lineStyle</styleUrl>\r\n   <description>Plot Your Traveling Path</description>\r\n  <name>Trajectory</name>\r\n";
	str += "    <visibility>1</visibility>\r\n  <open>0</open>\r\n    <Style>\r\n    <LineStyle>\r\n      <color>ff";
	f.Write(str, (UINT)str.GetLength());
	str.Format("%06x", lineColor);
	f.Write(str, (UINT)str.GetLength());
	str = "</color>\r\n    </LineStyle>\r\n    </Style>\r\n    <LineString>\r\n    <extrude>0</extrude>\r\n    <tessellate>1</tessellate>\r\n";
	if(convert3d)
	{
		str += "    <altitudeMode>absolute</altitudeMode>\r\n";
	}
	str += "    <coordinates>\r\n";
	f.Write(str, (UINT)str.GetLength());
}

void CSkyTraqKml::WriteKMLPath(CFile& f, double lon, double lat, double alt)
{
	CString str;
	str = "      ";
	f.Write(str, str.GetLength());

	
	str.Format("%f,%f,%f\r\n", lon, lat, (convert3d) ? alt : 2);
	f.Write(str, str.GetLength());
}

void CSkyTraqKml::WritePOIPath(CFile& f, vector<LL1> *lst)
{
	vector<LL1>::iterator iter;
	int id = 1;
	CString str;

	for(iter=lst->begin(); iter != lst->end(); ++iter)
	{
		LL1 lla = *iter;
		str.Format("<Placemark id=\"POI%d\">\r\n    <styleUrl>#POI_STYLE</styleUrl>\r\n    <name>POI%d</name>\r\n    <LookAt>\r\n", id, id++);
		f.Write(str, str.GetLength());
		str.Format("    <longitude>%.6f</longitude>\r\n    <latitude>%.6f</latitude>\r\n    </LookAt>\r\n        <Point>\r\n",lla.lon, lla.lat);
		f.Write(str, str.GetLength());
		str.Format("        <coordinates>%.6f,%.6f</coordinates>\r\n    </Point>\r\n</Placemark>\r\n",lla.lon, lla.lat);
		f.Write(str, str.GetLength());
	}
	str = "</Folder>\r\n";
	f.Write(str, str.GetLength());
}
