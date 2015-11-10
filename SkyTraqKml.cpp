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

void CSkyTraqKml::Init(const char *name, int color, bool kml3d, bool bPointList, bool bNoPointText)
{
	kmlFile.Open(name, CFile::modeReadWrite | CFile::modeCreate);	
	lineColor = color;
	iniWriteKml = true;
	convert3d = kml3d;
	pointList = bPointList;
	noPointText = bNoPointText;
	if(pointList)
	{
		pls.RemoveAll();
		strPointList = "<Folder id=\"MyPoints\">"\
		"<Style id=\"PointStyle\"><IconStyle><color>ffffffff</color><Icon><href>http://maps.google.com/mapfiles/kml/shapes/open-diamond.png</href></Icon></IconStyle></Style>" \
		"<Style id=\"PointStyle2\"><IconStyle><color>ffff0000</color><Icon><href>http://maps.google.com/mapfiles/kml/shapes/open-diamond.png</href></Icon></IconStyle></Style>" \
		"<Style id=\"PointStyle3\"><IconStyle><color>ff00ff00</color><Icon><href>http://maps.google.com/mapfiles/kml/shapes/open-diamond.png</href></Icon></IconStyle></Style>" \
		"<name>Points</name>\r\n";
		pls.AddTail(strPointList);
		strPointList.Empty();
	}
}

void CSkyTraqKml::PushOnePoint(double lon, double lat, double alt, const CString& ts, QualityMode q)
{
	if(iniWriteKml)
	{
		start_point.lon = lon;
		start_point.lat = lat;
		start_point.alt = alt;
		WriteKMLini(kmlFile);
		iniWriteKml = false;
	}
	WriteKMLPath(kmlFile, lon, lat, alt, ts, q);

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
	str = "<Placemark id=\"Start Point\"><styleUrl>#POI_STYLE</styleUrl><name>Start Point</name><LookAt>";
	f.Write(str, (U16)str.GetLength());

	str.Format("<longitude>%012.9lf</longitude><latitude>%012.9lf</latitude></LookAt><Point>", start_point.lon, start_point.lat);
	f.Write(str, (U16)str.GetLength());

	str.Format("<coordinates>%012.9lf,%012.9lf</coordinates></Point></Placemark>",start_point.lon,start_point.lat);
	f.Write(str, (U16)str.GetLength());
}

void CSkyTraqKml::AddEndPoint(CFile& f)
{	
	CString str;
	str = "<Placemark id=\"End Point\"><styleUrl>#POI_STYLE</styleUrl><name>End Point</name><LookAt>";
	f.Write(str, (U16)str.GetLength());
	
	str.Format("<longitude>%012.9lf</longitude><latitude>%012.9lf</latitude></LookAt><Point>", last_point.lon, last_point.lat);
	f.Write(str, (U16)str.GetLength());

	str.Format("<coordinates>%012.9lf,%012.9lf</coordinates></Point></Placemark>", last_point.lon, last_point.lat),
	f.Write(str, (U16)str.GetLength());
}


void CSkyTraqKml::Finish()
{
	CString str;
	if(lst_poi.size() > 0)
	{
		str = "</coordinates></LineString></Placemark>";
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
			str = "</coordinates></LineString></Placemark>";
			kmlFile.Write(str, (U16)str.GetLength());
		}

		if(pointList)
		{
			pls.AddTail(strPointList);
			POSITION p = pls.GetHeadPosition();
			for(int i = 0; i < pls.GetCount(); ++i)
			{
				str = pls.GetNext(p);
				kmlFile.Write(str, (U16)str.GetLength());
			}
			str += "</Folder>";
			kmlFile.Write(str, (U16)str.GetLength());
		}

		AddStartPoint(kmlFile);
		AddEndPoint(kmlFile);

		if (!iniWriteKml)
		{
			str = "</Folder></kml>";
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
	str = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n<kml xmlns=\"http://earth.google.com/kml/2.1\">";
	str += "<Style id=\"POI_STYLE\"><IconStyle><color>ff00ff00</color><width>2</width><scale>1.1</scale>";
	str += "<Icon><href>http://maps.google.com/mapfiles/kml/pal3/icon21.png</href></Icon></IconStyle>\r\n</Style>";
	str += "<Folder id=\"Data logger\"><name>Data logger</name>";
	str += "<Placemark id=\"logger\"><styleUrl>#lineStyle</styleUrl><description>Plot Your Traveling Path</description><name>Trajectory</name>";
	str += "<visibility>1</visibility><open>0</open><Style><LineStyle><color>ff";
	f.Write(str, (UINT)str.GetLength());
	str.Format("%06x", lineColor);
	f.Write(str, (UINT)str.GetLength());
	str = "</color></LineStyle></Style><LineString><extrude>0</extrude><tessellate>1</tessellate>";
	if(convert3d)
	{
		str += "<altitudeMode>absolute</altitudeMode>";
	}
	str += "<coordinates>";
	f.Write(str, (UINT)str.GetLength());
}

void CSkyTraqKml::WriteKMLPath(CFile& f, double lon, double lat, double alt, const CString& ts, QualityMode q)
{
	CString str;
	str = "      ";
	f.Write(str, str.GetLength());
	str.Format("%012.9lf,%012.9lf,%07.3lf\r\n", lon, lat, (convert3d) ? alt : 2.0);
	f.Write(str, str.GetLength());

	if(pointList)
	{
		CString str, qMode;
		if(q==FixRTK)
		{
			qMode = "Fix RTK";
		}
		else if(q==FloatRTK)
		{
			qMode = "Float RTK";
		}		
		else
		{
			qMode = "";
		}
		if(noPointText)
		{
			strPointList += "<Placemark><name></name><description><![CDATA[";
		}
		else
		{
			strPointList += "<Placemark><name>" + ts + "</name><description><![CDATA[";
		}
		str.Format("lontitude: %012.9lf <br>latitude: %012.9lf<br>altitude: %07.3lf<br>Time: %s<br>RTK Mode: %s", lon, lat, alt, ts, qMode);
		strPointList += str;
		if(q==FixRTK)
		{
			strPointList += "]]></description><styleUrl>#PointStyle2</styleUrl><Point>";
		}
		else if(q==FloatRTK)
		{
			strPointList += "]]></description><styleUrl>#PointStyle3</styleUrl><Point>";
		}		
		else
		{
			strPointList += "]]></description><styleUrl>#PointStyle</styleUrl><Point>";
		}
		if(convert3d)
		{
			strPointList += "<extrude>1</extrude><altitudeMode>absolute</altitudeMode>";
		}
		else
		{
			strPointList += "<altitudeMode>clampToGround</altitudeMode>";
		}
		str.Format("<coordinates>%012.9lf,%012.9lf,%07.3lf</coordinates>", lon, lat, alt);
		strPointList += str;
		strPointList += "</Point></Placemark>\r\n";
	
		if(strPointList.GetLength() > 4096)
		{
			pls.AddTail(strPointList);
			strPointList.Empty();
		}

	}
}

void CSkyTraqKml::WritePOIPath(CFile& f, vector<LL1> *lst)
{
	vector<LL1>::iterator iter;
	int id = 1;
	CString str;

	for(iter=lst->begin(); iter != lst->end(); ++iter)
	{
		LL1 lla = *iter;
		str.Format("<Placemark id=\"POI%d\"><styleUrl>#POI_STYLE</styleUrl><name>POI%d</name><LookAt>", id, id++);
		f.Write(str, str.GetLength());
		str.Format("<longitude>%012.9lf</longitude><latitude>%012.9lf</latitude></LookAt><Point>\r\n",lla.lon, lla.lat);
		f.Write(str, str.GetLength());
		str.Format("<coordinates>%012.9lf,%012.9lf</coordinates></Point></Placemark>",lla.lon, lla.lat);
		f.Write(str, str.GetLength());
	}
	str = "</Folder>\r\n";
	f.Write(str, str.GetLength());
}
