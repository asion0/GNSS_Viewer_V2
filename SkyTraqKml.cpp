#include "StdAfx.h"
#include "SkytraqKml.h"
#include "GPSDlg.h"

using namespace std;
CSkyTraqKml1::CSkyTraqKml1(void)
{
	
}

CSkyTraqKml1::~CSkyTraqKml1(void)
{
}

void CSkyTraqKml1::set_filename( char *name )
{
	FKml.Open(name,CFile::modeReadWrite|CFile::modeCreate);	
}

void CSkyTraqKml1::push_one_point(double lon,double lat)
{
	if(iniWriteKml)
	{
		start_point.lon = lon;
		start_point.lat = lat;
		WriteKMLini(FKml,lon, lat);
		iniWriteKml= false;
	}
	WriteKMLPath(FKml,lon, lat);

	last_point.lat = lat;
	last_point.lon = lon;
}

void CSkyTraqKml1::add_start_point(CFile& Fnmea )
{	
	char buff[1024];

		
	sprintf_s(buff, sizeof(buff), "<Placemark id=\"Start Point\">\r\n    <styleUrl>#POI_STYLE</styleUrl>\r\n    <name>Start Point</name>\r\n    <LookAt>\r\n");
	Fnmea.Write(buff,(U16)strlen(buff));
	sprintf_s(buff, sizeof(buff), "    <longitude>%.6f</longitude>\r\n    <latitude>%.6f</latitude>\r\n    </LookAt>\r\n        <Point>\r\n",start_point.lon,start_point.lat);
	Fnmea.Write(buff,(U16)strlen(buff));
	sprintf_s(buff, sizeof(buff), "        <coordinates>%.6f,%.6f</coordinates>\r\n    </Point>\r\n</Placemark>\r\n",start_point.lon,start_point.lat),
		Fnmea.Write(buff, (U16)strlen(buff));

	
	//Fnmea.Write(buff,(U16)strlen(buff));
}

void CSkyTraqKml1::add_end_point(CFile& Fnmea )
{	
	char buff[1024];


	sprintf_s(buff, sizeof(buff), "<Placemark id=\"End Point\">\r\n    <styleUrl>#POI_STYLE</styleUrl>\r\n    <name>End Point</name>\r\n    <LookAt>\r\n");
	Fnmea.Write(buff,(U16)strlen(buff));
	sprintf_s(buff, sizeof(buff), "    <longitude>%.6f</longitude>\r\n    <latitude>%.6f</latitude>\r\n    </LookAt>\r\n        <Point>\r\n",last_point.lon,last_point.lat);
	Fnmea.Write(buff,(U16)strlen(buff));
	sprintf_s(buff, sizeof(buff), "        <coordinates>%.6f,%.6f</coordinates>\r\n    </Point>\r\n</Placemark>\r\n",last_point.lon,last_point.lat),
		Fnmea.Write(buff,(U16)strlen(buff));

	//Fnmea.Write(buff,(U16)strlen(buff));
}

void CSkyTraqKml1::push_one_poi(double lon, double lat)
{
	LL1 tmp_lla;
	tmp_lla.lat = lat;
	tmp_lla.lon = lon;
	lst_poi.push_back(tmp_lla);
}

void CSkyTraqKml1::finish()
{
	char title[500];

	
	if ( lst_poi.size() > 0 )
	{
		
		strcpy_s(title, sizeof(title), "    </coordinates>\r\n  </LineString>\r\n</Placemark>\r\n");
		FKml.Write(title,(U16)strlen(title));

		WritePOIPath(FKml,&lst_poi);
		strcpy_s(title, sizeof(title), "</kml>");
		FKml.Write(title,(U16)strlen(title));

		lst_poi.clear();
	}
	else
	{
		if (!iniWriteKml)
		{
			strcpy_s(title, sizeof(title), "    </coordinates>\r\n  </LineString>\r\n</Placemark>\r\n");
			FKml.Write(title,(U16)strlen(title));
		}
		add_start_point(FKml);
		add_end_point(FKml);


		if (!iniWriteKml)
		{
			//strcpy_s(title, sizeof)title), "    </coordinates>\r\n  </LineString>\r\n</Placemark>\r\n</Folder>\r\n</kml>\r\n");
			strcpy_s(title, sizeof(title), "</Folder>\r\n</kml>\r\n");
			FKml.Write(title,(U16)strlen(title));
		}

	}


	FKml.Close();
}

void CSkyTraqKml1::WriteKMLini(CFile& Fnmea,double lon,double lat)
{
	/*<?xml version="1.0" encoding="UTF-8"?>
	<kml xmlns="http://earth.google.com/kml/2.1">*/
	char title[1024];
	CString temp;
	strcpy_s(title, sizeof(title), "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n<kml xmlns=\"http://earth.google.com/kml/2.1\">\r\n");
	Fnmea.Write(title,(U16)strlen(title));


	strcpy_s(title, sizeof(title), "<Style id=\"POI_STYLE\">\r\n    <IconStyle>\r\n    <color>ff00ff00</color>\r\n    <scale>1.1</scale>\r\n");
	Fnmea.Write(title,(U16)strlen(title));
	strcpy_s(title, sizeof(title), "    <Icon><href>http://maps.google.com/mapfiles/kml/pal3/icon21.png</href></Icon>\r\n    </IconStyle>\r\n</Style>\r\n");
	Fnmea.Write(title,(U16)strlen(title));

	strcpy_s(title,"<Folder id=\"Data logger\">\r\n    <name>Data logger</name>\r\n");
	Fnmea.Write(title,(U16)strlen(title));

	char title8[]="<Placemark id=\"logger\">\r\n  <styleUrl>#lineStyle</styleUrl>\r\n   <description>Plot Your Traveling Path</description>\r\n  <name>Trajectory</name>\r\n";
	Fnmea.Write(title8,sizeof(title8)-1);	

	//int color = 0xff0000;
	temp.Format("%s%06x%s","    <visibility>1</visibility>\r\n  <open>0</open>\r\n    <Style>\r\n    <LineStyle>\r\n      <color>ff",line_color,"</color>\r\n    </LineStyle>\r\n    </Style>\r\n    <LineString>\r\n    <extrude>1</extrude>\r\n    <tessellate>1</tessellate>\r\n    <coordinates>\r\n");
	//temp="    <visibility>1</visibility>\r\n  <open>0</open>\r\n    <Style>\r\n    <LineStyle>\r\n      <color>ff0000ff</color>\r\n    </LineStyle>\r\n    </Style>\r\n    <LineString>\r\n    <extrude>1</extrude>\r\n    <tessellate>1</tessellate>\r\n    <coordinates>\r\n";
	Fnmea.Write(temp,temp.GetLength());
}

void CSkyTraqKml1::WriteKMLPath(CFile& Fnmea,double lon,double lat)
{
	string temp;	
	char CoordinateToString[50];
	const char t[] = "      ";
	Fnmea.Write(t, sizeof(t) - 1);
	sprintf_s(CoordinateToString, sizeof(CoordinateToString), "%f", lon);
	temp = CoordinateToString;
	Fnmea.Write(CoordinateToString, (U16)temp.length());
	const char title[] = ",";
	Fnmea.Write(title, sizeof(title) - 1);
	sprintf_s(CoordinateToString, sizeof(CoordinateToString), "%f", lat);
	temp = CoordinateToString;
	Fnmea.Write(CoordinateToString,(U16)temp.length());
	const char title1[] = ",2\r\n";
	Fnmea.Write(title1, sizeof(title1) - 1);
}

void CSkyTraqKml1::WritePOIPath(CFile& Fnmea ,vector<LL1> *lst )
{
	vector<LL1>::iterator iter;
	int id = 1;
	char buff[1024];

	for(iter=lst->begin(); iter != lst->end(); ++iter)
	{
		LL1 lla = *iter;
		sprintf_s(buff, sizeof(buff), "<Placemark id=\"POI%d\">\r\n    <styleUrl>#POI_STYLE</styleUrl>\r\n    <name>POI%d</name>\r\n    <LookAt>\r\n",id,id++);
		Fnmea.Write(buff, (U16)strlen(buff));
		sprintf_s(buff, sizeof(buff), "    <longitude>%.6f</longitude>\r\n    <latitude>%.6f</latitude>\r\n    </LookAt>\r\n        <Point>\r\n",lla.lon,lla.lat);
		Fnmea.Write(buff, (U16)strlen(buff));
		sprintf_s(buff, sizeof(buff), "        <coordinates>%.6f,%.6f</coordinates>\r\n    </Point>\r\n</Placemark>\r\n",lla.lon,lla.lat),
			Fnmea.Write(buff, (U16)strlen(buff));
	}
	strcpy_s(buff, sizeof(buff), "</Folder>\r\n");
	Fnmea.Write(buff, (U16)strlen(buff));
}

void CSkyTraqKml1::init(const char *name, int path_color)
{
	FKml.Open(name,CFile::modeReadWrite|CFile::modeCreate);	
	line_color = path_color;
	iniWriteKml = TRUE;
}

//----------------------------------------------------------------------------
CSkyTraqKml2::CSkyTraqKml2(void)
{
	
}

CSkyTraqKml2::~CSkyTraqKml2(void)
{
}

void CSkyTraqKml2::set_filename( char *name )
{
	FKml.Open(name,CFile::modeReadWrite|CFile::modeCreate);	
}

void CSkyTraqKml2::push_one_point(LL2 *lla)
{
	//if(iniWriteKml)
	//{
	//	start_point.lon = lon;
	//	start_point.lat = lat;
	//	WriteKMLini(FKml,lon, lat);
	//	iniWriteKml= false;
	//}
	//WriteKMLPath(FKml,lon, lat);

	//last_point.lat = lat;
	//last_point.lon = lon;

	//LL tmp_lla;
	//tmp_lla.lat = lat;tmp_lla.lon =lon;
	points.push_back(*lla);
}

void CSkyTraqKml2::add_start_point(CFile& Fnmea ,vector<LL2> *lst)
{	
	char buff[1024];

	LL2 lla = *(lst->begin());
		
	sprintf_s(buff, sizeof(buff), "<Placemark id=\"Start Point\">\r\n    <styleUrl>#POI_STYLE</styleUrl>\r\n    <name>Start Point</name>\r\n    <LookAt>\r\n");
	Fnmea.Write(buff,(U16)strlen(buff));
	sprintf_s(buff, sizeof(buff), "    <longitude>%.6f</longitude>\r\n    <latitude>%.6f</latitude>\r\n    </LookAt>\r\n        <Point>\r\n",lla.lon,lla.lat);
	Fnmea.Write(buff,(U16)strlen(buff));
	sprintf_s(buff, sizeof(buff), "        <coordinates>%.6f,%.6f</coordinates>\r\n    </Point>\r\n</Placemark>\r\n",lla.lon,lla.lat),
	Fnmea.Write(buff,(U16)strlen(buff));

	
	//Fnmea.Write(buff,(U16)strlen(buff));
}

void CSkyTraqKml2::add_end_point(CFile& Fnmea ,vector<LL2> *lst )
{	
	char buff[1024];

	LL2 lla = *(lst->end()-1);
	sprintf_s(buff, sizeof(buff),"<Placemark id=\"End Point\">\r\n    <styleUrl>#POI_STYLE</styleUrl>\r\n    <name>End Point</name>\r\n    <LookAt>\r\n");
	Fnmea.Write(buff,(U16)strlen(buff));
	sprintf_s(buff, sizeof(buff),"    <longitude>%.6f</longitude>\r\n    <latitude>%.6f</latitude>\r\n    </LookAt>\r\n        <Point>\r\n",lla.lon,lla.lat);
	Fnmea.Write(buff,(U16)strlen(buff));
	sprintf_s(buff, sizeof(buff),"        <coordinates>%.6f,%.6f</coordinates>\r\n    </Point>\r\n</Placemark>\r\n",lla.lon,lla.lat),
		Fnmea.Write(buff,(U16)strlen(buff));

	//Fnmea.Write(buff,(U16)strlen(buff));
}

void CSkyTraqKml2::push_one_poi(LL2 *lla)
{
	//LL tmp_lla;
	//tmp_lla.lat = lat;tmp_lla.lon =lon;
	lst_poi.push_back(*lla);
}

void CSkyTraqKml2::WritePointLine(CFile& Fnmea ,vector<LL2> *lst)
{
	vector<LL2>::iterator iter;
	int id = 1;
//	char buff[1024];

	
	for(iter = lst->begin();iter != lst->end();iter++)
	{
		LL2 lla = *iter;

		WriteKMLPath(FKml,lla.lat, lla.lon);
	}
}

void CSkyTraqKml2::WritePointLineEnd(CFile& Fnmea )
{
	char title[500];
	strcpy_s(title, sizeof(title), "    </coordinates>\r\n  </LineString>\r\n</Placemark>\r\n");
	Fnmea.Write(title,(U16)strlen(title));
}

void CSkyTraqKml2::WriteKMLEnd(CFile& Fnmea )
{
	char title[500];
	strcpy_s(title, sizeof(title),"</Folder>\r\n</Document>\r\n</kml>\r\n");
	Fnmea.Write(title,(U16)strlen(title));
}

void CSkyTraqKml2::finish()
{
	if (points.size()<=0) return;

	WriteKMLini(FKml);

	WritePointLine(FKml,&points);

	WritePointLineEnd(FKml);

	add_start_point(FKml,&points);

	if (points.size()>1)
	{
		add_end_point(FKml,&points);
	}
	
	WritePointPath(FKml,&points);

	if ( lst_poi.size() > 0 )
	{
		WritePOIPath(FKml,&lst_poi);
		lst_poi.clear();
	}

	WriteKMLEnd(FKml);
	points.clear();

	FKml.Close();
}

void CSkyTraqKml2::WriteKMLini(CFile& Fnmea)
{
	/*<?xml version="1.0" encoding="UTF-8"?>
	<kml xmlns="http://earth.google.com/kml/2.1">*/
	char title[1024];
	CString temp;
	strcpy_s(title, sizeof(title), "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\r\n<kml xmlns=\"http://earth.google.com/kml/2.1\"><Document>\r\n");
	Fnmea.Write(title,(U16)strlen(title));

	strcpy_s(title, sizeof(title), "<Style id=\"POINT_STYLE\">\r\n    <IconStyle>\r\n    <color>ff0000ff</color>\r\n    <scale>1.0</scale>\r\n");
	Fnmea.Write(title,(U16)strlen(title));
	strcpy_s(title, sizeof(title), "    <Icon><href>http://www.markus-bader.de/MB-Freeware/gpsposicon_rect.png</href></Icon>\r\n    </IconStyle>\r\n</Style>\r\n");
	Fnmea.Write(title,(U16)strlen(title));


	strcpy_s(title, sizeof(title),"<Style id=\"POI_STYLE\">\r\n    <IconStyle>\r\n    <color>ff00ff00</color>\r\n    <scale>1.1</scale>\r\n");
	Fnmea.Write(title,(U16)strlen(title));
	strcpy_s(title, sizeof(title),"    <Icon><href>http://maps.google.com/mapfiles/kml/shapes/flag.png</href></Icon>\r\n    </IconStyle>\r\n</Style>\r\n");
	Fnmea.Write(title,(U16)strlen(title));

	strcpy_s(title, sizeof(title),"<Folder id=\"Data logger\">\r\n    <name>Data logger</name>\r\n");
	Fnmea.Write(title,(U16)strlen(title));

	char title8[]="<Placemark id=\"logger\">\r\n  <styleUrl>#lineStyle</styleUrl>\r\n   <description>Plot Your Traveling Path</description>\r\n  <name>Trajectory</name>\r\n";
	Fnmea.Write(title8,sizeof(title8)-1);	

	temp.Format("%s%06x%s","    <visibility>1</visibility>\r\n  <open>0</open>\r\n    <Style>\r\n    <LineStyle>\r\n      <color>ff",line_color,"</color>\r\n    </LineStyle>\r\n    </Style>\r\n    <LineString>\r\n    <extrude>1</extrude>\r\n    <tessellate>1</tessellate>\r\n    <coordinates>\r\n");
	Fnmea.Write(temp,temp.GetLength());
}

void CSkyTraqKml2::WriteKMLPath(CFile& Fnmea,double lat,double lon)
{
	string temp;	
	char CoordinateToString[50];
	char t[]="      ";
	Fnmea.Write(t,sizeof(t)-1);
	sprintf_s(CoordinateToString, sizeof(CoordinateToString),"%f",lon);
	temp=CoordinateToString;
	Fnmea.Write(CoordinateToString,(U16)temp.length());
	char title[]=",";
	Fnmea.Write(title,sizeof(title)-1);
	sprintf_s(CoordinateToString, sizeof(CoordinateToString),"%f",lat);
	temp=CoordinateToString;
	Fnmea.Write(CoordinateToString,(U16)temp.length());
	char title1[]=",2\r\n";
	Fnmea.Write(title1,sizeof(title1)-1);
}

void CSkyTraqKml2::WritePointPath(CFile& Fnmea ,vector<LL2> *lst )
{

	vector<LL2>::iterator iter;
	int id = 1;
	char buff[1024];

	for(iter = lst->begin();iter != lst->end();iter++)
	{
		LL2 lla = *iter;
		sprintf_s(buff, sizeof(buff), "<Placemark>\r\n    <styleUrl>#POINT_STYLE</styleUrl>\r\n");
		Fnmea.Write(buff,(U16)strlen(buff));
		sprintf_s(buff, sizeof(buff), "<description><![CDATA[<TABLE><TR><TD width=200>%02d:%02d:%.2f UTC<BR>speed:%.1f<BR>height:%.1f</TD></TR></TABLE>]]></description>\r\n    <LookAt>\r\n",lla.utc.hour,lla.utc.minute,lla.utc.sec,lla.speed,lla.alt);
		Fnmea.Write(buff,(U16)strlen(buff));
		sprintf_s(buff, sizeof(buff), "    <longitude>%.6f</longitude>\r\n    <latitude>%.6f</latitude>\r\n    </LookAt>\r\n        <Point>\r\n",lla.lon,lla.lat);
		Fnmea.Write(buff,(U16)strlen(buff));
		sprintf_s(buff, sizeof(buff), "        <coordinates>%.6f,%.6f</coordinates>\r\n    </Point>\r\n</Placemark>\r\n",lla.lon,lla.lat),
		Fnmea.Write(buff,(U16)strlen(buff));
	}
}


void CSkyTraqKml2::WritePOIPath(CFile& Fnmea ,vector<LL2> *lst )
{

	vector<LL2>::iterator iter;
	int id = 1;
	char buff[1024];

	for(iter = lst->begin();iter != lst->end();iter++)
	{
		LL2 lla = *iter;
		sprintf_s(buff, sizeof(buff), "<Placemark id=\"POI%d\">\r\n    <styleUrl>#POI_STYLE</styleUrl>\r\n    <name>POI%d</name>\r\n    <LookAt>\r\n",id,id++);
		Fnmea.Write(buff,(U16)strlen(buff));
		sprintf_s(buff, sizeof(buff), "    <longitude>%.6f</longitude>\r\n    <latitude>%.6f</latitude>\r\n    </LookAt>\r\n        <Point>\r\n",lla.lon,lla.lat);
		Fnmea.Write(buff,(U16)strlen(buff));
		sprintf_s(buff, sizeof(buff), "        <coordinates>%.6f,%.6f</coordinates>\r\n    </Point>\r\n</Placemark>\r\n",lla.lon,lla.lat),
			Fnmea.Write(buff,(U16)strlen(buff));
	}
	strcpy_s(buff, sizeof(buff),"</Folder>\r\n");
	Fnmea.Write(buff,(U16)strlen(buff));

}

void CSkyTraqKml2::init(const char *name,int path_color )
{
	FKml.Open(name,CFile::modeReadWrite|CFile::modeCreate);	
	line_color = path_color;
}