#pragma once

#include "StdAfx.h"

class geoid
{
public:
	geoid(void);
	~geoid(void);
	double GEO_calc_geoid_height( double lat, double lon );
	double convert_lon(double lon);
	double convert_lat(double lat);
	double cal_geoid_height( S08 a, S08 b, S08 c, S08 d, double grid_deg, double lat_ratio, double lon_ratio, double inv_area );
	double find_geoid_height( double lat_table, double lon_table );
private:

};
