#include "StdAfx.h"
#include "geoid.h"

// 2.5 degrees table
static const S08 south_euro[9][21] = 
{
	{59,57,55,51,47,46,47,48,48,47,46,45,42,36,32,32,28,23,18,13,12},
	{51,52,52,50,49,48,49,50,48,48,49,46,45,42,40,33,30,26,24,19,14},
	{49,44,44,47,47,53,51,53,42,42,44,46,45,42,38,33,30,25,19,13,14},
	{53,57,54,51,51,51,47,46,49,50,44,43,46,46,43,38,29,24,24,20,14},
	{51,56,56,54,52,48,45,45,48,45,47,40,35,43,45,44,40,37,35,36,33},
	{50,56,51,54,50,45,42,41,44,44,41,29,24,32,40,38,37,38,36,34,26},
	{42,39,42,52,51,47,38,38,38,37,35,28,22,8,17,4,4,18,20,27,21},
	{42,48,54,51,44,41,32,30,32,33,31,28,29,26,10,6,11,15,18,19,15},
	{44,49,47,42,36,31,28,26,28,32,32,32,29,25,22,18,17,18,19,16,12}
};
// 2.5 degrees table
static const S08 east_asia[13][9] = 
{
	{17,20,22,22,23,23,19,21,22},
	{19,20,24,29,25,25,20,21,21},
	{22,24,30,27,29,30,26,28,13},
	{24,25,28,29,33,26,21,9,16},
	{23,25,29,32,37,29,16,22,18},
	{27,29,32,37,41,23,21,21,14},
	{29,34,37,40,34,16,25,21,15},
	{32,31,34,38,42,20,29,23,17},
	{29,29,36,40,43,23,31,25,20},
	{26,34,37,42,45,43,35,30,25},
	{34,37,38,44,47,40,39,36,30},
	{36,38,41,45,48,46,30,37,36},
	{39,40,43,47,49,50,47,33,39}
};
// 5 degrees table
static const S08 north_america[9][19] =
{
	{10,12,13,5,1,-7,-14,-22,-30,-35,-39,-46,-46,-46,-42,-29,-21,-7,6},
	{11,6,1,-5,-8,-11,-14,-19,-26,-29,-32,-39,-45,-47,-45,-36,-27,-16,-8},
	{1,-4,-11,-16,-19,-16,-16,-14,-18,-18,-22,-28,-35,-40,-40,-36,-26,-21,-12},
	{-12,-17,-23,-27,-26,-26,-17,-12,-8,-14,-23,-27,-33,-34,-35,-32,-26,-20,-14},
	{-20,-25,-31,-34,-34,-34,-21,-17,-16,-14,-26,-34,-34,-35,-33,-35,-35,-34,-26},
	{-22,-27,-32,-38,-40,-41,-35,-31,-23,-19,-28,-30,-27,-30,-31,-42,-44,-38,-31},
	{-15,-21,-28,-34,-40,-43,-42,-35,-29,-24,-22,-26,-26,-28,-32,-45,-51,-44,-40},
	{-11,-17,-24,-32,-40,-45,-47,-42,-34,-17,-15,-31,-26,-25,-26,-40,-52,-49,-48},
	{-7,-14,-23,-31,-39,-44,-47,-42,-34,-23,-9,-19,-10,-16,-20,-26,-45,-61,-48}
};
// 5 degree table
static const S08 north_euro[5][11] = 
{
	{58,52,51,47,43,39,29,24,20,16,12},
	{59,53,51,43,41,33,27,17,18,18,14},
	{57,55,47,44,41,31,21,19,18,17,14},
	{58,56,46,41,41,35,26,22,18,16,11},
	{59,55,47,47,48,46,42,32,28,18,12}
};
// 5 degree table
static const  S08 west_asia[7][7] =  
{
	{-43,-39,-29,-16,-2,7,17},
	{-49,-42,-33,-14,0,13,22},
	{-50,-44,-28,-11,3,18,23},
	{-37,-41,-29,-14,3,19,29},
	{-34,-37,-28,-10,7,23,29},
	{-36,-31,-21,-2,17,29,34},
	{-36,-26,-11,6,21,34,39}
};
// 5 degree table
static const S08 australia[9][11] =  
{
	{52,63,68,72,76,70,64,58,52,42,36},
	{39,50,54,66,64,63,63,61,65,59,49},
	{23,33,45,56,58,55,57,61,63,61,51},
	{4,15,26,35,47,43,48,49,53,52,51},
	{-14,2,15,24,33,30,34,36,45,46,46},
	{-27,-10,2,10,20,17,20,26,31,38,29},
	{-27,-21,-14,-1,-2,3,5,12,20,15,21},
	{-24,-20,-17,-15,-11,-9,-6,-2,8,6,4},
	{-22,-19,-18,-16,-15,-16,-14,-11,-10,-11,-15}
};
// 5 degree table
static const S08 singapore[5][4] =
{
	{-46,-24,-8, 12},
	{-41,-14, 4, 24},
	{-36, -9,15, 33},
	{-41,-17,10, 30},
	{-46,-26,-6,  0}
};  

// 10 degree table
static const S08 upper[9][37] =
{
	{13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13},
	{3,1,-2,-3,-3,-3,-1,3,1,5,9,11,19,27,31,34,33,34,33,34,28,23,17,13,9,4,4,1,-2,-2,0,2,3,2,1,1,3},
	{2,2,1,-1,-3,-7,-14,-24,-27,-25,-19,3,24,37,47,60,61,58,51,43,29,20,12,5,-2,-10,-14,-12,-10,-14,-12,-6,-2,3,6,4,2},
	{2,9,17,10,13,1,-14,-30,-39,-46,-42,-21,6,29,49,65,60,57,47,41,21,18,14,7,-3,-22,-29,-32,-32,-26,-15,-2,13,17,19,6,2},
	{-8,8,8,1,-11,-19,-16,-18,-22,-35,-40,-26,-12,24,45,63,62,59,47,48,42,28,12,-10,-19,-33,-43,-42,-43,-29,-2,17,23,22,6,2,-8},
	{-12,-10,-13,-20,-31,-34,-21,-16,-26,-34,-33,-35,-26,2,33,59,52,51,52,48,35,40,33,-9,-28,-39,-48,-59,-50,-28,3,23,37,18,-1,-11,12},
	{-7,-5,-8,-15,-28,-40,-42,-29,-22,-26,-32,-51,-40,-17,17,31,34,44,36,28,29,17,12,-20,-15,-40,-33,-34,-34,-28,7,29,43,20,4,-6,-7},
	{5,10,7,-7,-23,-39,-47,-34,-9,-10,-20,-45,-48,-32,-9,17,25,31,31,26,15,6,1,-29,-44,-61,-67,-59,-36,-11,21,39,49,39,22,10,5},
	{13,12,11,2,-11,-28,-38,-29,-10,3,1,-11,-41,-42,-16,3,17,33,22,23,2,-3,-7,-36,-59,-90,-95,-63,-24,12,53,60,58,46,36,26,13}
};
// 10 degree table
static const S08 south_america[5][3] =
{
	{1,-11,-41},
	{14,10,-15},
	{1,32,4},
	{7,35,20},
	{9,32,16}
};
// 20 degree table
static const S08 lower[6][19] =
{
	{13,11,-11,-38,-10,1,-41,-16,17,22,2,-7,-59,-95,-24,53,58,36,13},
	{36,11,-1,-10,-11,1,4,-13,4,12,-2,-25,-38,-75,-26,35,68,64,36},
	{46,5,-8,-10,-4,9,16,-8,12,22,34,14,15,-9,-37,-23,15,34,46},
	{-15,-18,-17,-10,-8,6,13,3,20,25,34,45,38,28,-1,-22,-18,-14,-15},
	{-61,-61,-49,-38,-25,-6,4,4,6,16,17,20,26,16,-1,-29,-46,-54,-61},
	{-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30,-30}
};

inline void SettingValue(double& inv_area, double& lat_table, double& lon_table, 
		U16& lat_ndx, U16& lon_ndx, double& lat_ratio, double& lon_ratio, 
		const double area, const double lat, const double lon, double ndx, 
		double ratio)
{
	inv_area = area;
	lat_table = lat;
	lon_table = lon;
	lat_ndx = (U16)(lat * ndx);
	lon_ndx = (U16)(lon * ndx);
	lat_ratio = lat - lat_ndx * ratio;
	lon_ratio = lon - lon_ndx * ratio;
}

geoid::geoid(void)
{
}

geoid::~geoid(void)
{
}


double geoid::find_geoid_height(double lat_table, double lon_table)
{
	double lat_ratio    = 0;
	double lon_ratio    = 0;
	double inv_area     = 0;
	U16 lat_ndx      = 0;
	U16 lon_ndx      = 0;
	double geoid_height = 0;
	S08 a,b,c,d;

	//(1)south_euro (2.5 deg grid)
	if( (40 <= lat_table) && (lat_table < 60) && (170 <= lon_table) && (lon_table < 220) )
	{
		SettingValue(inv_area, lat_table, lon_table, lat_ndx, lon_ndx, lat_ratio, lon_ratio, 
						0.16F, 40.0F, 170.0F, 0.4, 2.5F); 

		a = south_euro[lat_ndx][lon_ndx];
		b = south_euro[lat_ndx+1][lon_ndx];
		c = south_euro[lat_ndx][lon_ndx+1];
		d = south_euro[lat_ndx+1][lon_ndx+1];
		geoid_height = cal_geoid_height( a, b, c, d, 2.5, lat_ratio, lon_ratio, inv_area );    

	}
	//(2)east_asia (2.5 deg grid)
	else if( (40 <= lat_table) && (lat_table < 70) && (310 <= lon_table) && (lon_table < 330) )
	{
		SettingValue(inv_area, lat_table, lon_table, lat_ndx, lon_ndx, lat_ratio, lon_ratio, 
						0.16F, 40.0F, 310.0F, 0.4, 2.5F); 

		a = east_asia[lat_ndx][lon_ndx];
		b = east_asia[lat_ndx+1][lon_ndx];
		c = east_asia[lat_ndx][lon_ndx+1];
		d = east_asia[lat_ndx+1][lon_ndx+1];
		geoid_height = cal_geoid_height( a, b, c, d, 2.5, lat_ratio, lon_ratio, inv_area );

	}               
	//(3)north_america( 5 deg grid )
	else if( (30 <= lat_table) && (lat_table < 70) && (30 <= lon_table) && (lon_table < 120) )
	{
		SettingValue(inv_area, lat_table, lon_table, lat_ndx, lon_ndx, lat_ratio, lon_ratio, 
						0.04F, 30.0F, 30.0F, 0.2, 5.0F); 

		a = north_america[lat_ndx][lon_ndx];
		b = north_america[lat_ndx+1][lon_ndx];
		c = north_america[lat_ndx][lon_ndx+1];
		d = north_america[lat_ndx+1][lon_ndx+1];
		geoid_height = cal_geoid_height( a, b, c, d, 5.0, lat_ratio, lon_ratio, inv_area );

	}
	//(4)north_euro ( 5 deg grid )
	else if( (20 <= lat_table) && (lat_table < 40) && (170 <= lon_table) && (lon_table < 220) )
	{
		SettingValue(inv_area, lat_table, lon_table, lat_ndx, lon_ndx, lat_ratio, lon_ratio, 
						0.04F, 20.0F, 170.0F, 0.2, 5.0F); 

		a = north_euro[lat_ndx][lon_ndx];
		b = north_euro[lat_ndx+1][lon_ndx];
		c = north_euro[lat_ndx][lon_ndx+1];
		d = north_euro[lat_ndx+1][lon_ndx+1];
		geoid_height = cal_geoid_height( a, b, c, d, 5.0, lat_ratio, lon_ratio, inv_area );

	}
	//(5)west_asia ( 5 deg grid )
	else if( (40 <= lat_table) && (lat_table < 70) && (280 <= lon_table) && (lon_table < 310) )
	{
		SettingValue(inv_area, lat_table, lon_table, lat_ndx, lon_ndx, lat_ratio, lon_ratio, 
						0.04F, 40.0F, 280.0F, 0.2, 5.0F); 

		a = west_asia[lat_ndx][lon_ndx];
		b = west_asia[lat_ndx+1][lon_ndx];
		c = west_asia[lat_ndx][lon_ndx+1];
		d = west_asia[lat_ndx+1][lon_ndx+1];
		geoid_height = cal_geoid_height( a, b, c, d, 5.0, lat_ratio, lon_ratio, inv_area );                 
	}
	//(6)australia ( 5 deg_grid )
	else if( (100 <= lat_table) && (lat_table < 140) && (310 <= lon_table) && (lon_table < 360) )
	{
		SettingValue(inv_area, lat_table, lon_table, lat_ndx, lon_ndx, lat_ratio, lon_ratio, 
						0.04F, 100.0F, 310.0F, 0.2, 5.0F); 

		a = australia[lat_ndx][lon_ndx];
		b = australia[lat_ndx+1][lon_ndx];
		c = australia[lat_ndx][lon_ndx+1];
		d = australia[lat_ndx+1][lon_ndx+1];
		geoid_height = cal_geoid_height( a, b, c, d, 5.0, lat_ratio, lon_ratio, inv_area );

	}
	else if( (80 <= lat_table) && (lat_table < 100) && (275 <= lon_table) && (lon_table < 290) )
	{
		SettingValue(inv_area, lat_table, lon_table, lat_ndx, lon_ndx, lat_ratio, lon_ratio, 
						0.04F, 80.0F, 275.0F, 0.2, 5.0F); 

		a = singapore[lat_ndx][lon_ndx];
		b = singapore[lat_ndx+1][lon_ndx];
		c = singapore[lat_ndx][lon_ndx+1];
		d = singapore[lat_ndx+1][lon_ndx+1];
		geoid_height = cal_geoid_height( a, b, c, d, 5.0, lat_ratio, lon_ratio, inv_area );

	}
	else
	{
		if( (0 <= lat_table) && (lat_table < 80) && (0 <= lon_table) && (lon_table < 360) )
		{
			SettingValue(inv_area, lat_table, lon_table, lat_ndx, lon_ndx, lat_ratio, lon_ratio, 
							0.01F, 0.0F, 0.0F, 0.1, 10.0F); 

			a = upper[lat_ndx][lon_ndx];
			b = upper[lat_ndx+1][lon_ndx];
			c = upper[lat_ndx][lon_ndx+1];
			d = upper[lat_ndx+1][lon_ndx+1];
			geoid_height = cal_geoid_height( a, b, c, d, 10.0, lat_ratio, lon_ratio, inv_area );

		}
		//south_america ( 10 deg grid )
		else if( (80 <= lat_table) && (lat_table < 120) && (100 <= lon_table) && (lon_table < 120) )
		{
			SettingValue(inv_area, lat_table, lon_table, lat_ndx, lon_ndx, lat_ratio, lon_ratio, 
							0.01F, 80.0F, 100.0F, 0.1, 10.0F); 

			a = south_america[lat_ndx][lon_ndx];
			b = south_america[lat_ndx+1][lon_ndx];
			c = south_america[lat_ndx][lon_ndx+1];
			d = south_america[lat_ndx+1][lon_ndx+1];
			geoid_height = cal_geoid_height( a, b, c, d, 10.0, lat_ratio, lon_ratio, inv_area );

		}
		else if( (80 < lat_table) && (lat_table < 180) && (0 <= lon_table) && (lon_table < 360) )
		{
			SettingValue(inv_area, lat_table, lon_table, lat_ndx, lon_ndx, lat_ratio, lon_ratio, 
							0.0025F, 80.0F, 0.0F, 0.05, 20.0F); 

			a = lower[lat_ndx][lon_ndx];
			b = lower[lat_ndx+1][lon_ndx];
			c = lower[lat_ndx][lon_ndx+1];
			d = lower[lat_ndx+1][lon_ndx+1];
			geoid_height = cal_geoid_height( a, b, c, d, 20.0, lat_ratio, lon_ratio, inv_area );

		}
	}
	return geoid_height;
}

double geoid::cal_geoid_height( S08 a, S08 b, S08 c, S08 d, double grid_deg, double lat_ratio, double lon_ratio, double inv_area )
{
	double geoid_height = a*(grid_deg-lat_ratio)*(grid_deg-lon_ratio)*inv_area
		+ b*(lat_ratio)*(grid_deg-lon_ratio)*inv_area
		+ c*(grid_deg-lat_ratio)*(lon_ratio)*inv_area
		+ d*(lat_ratio)*(lon_ratio)*inv_area;  
	return geoid_height;                 
}

double geoid::convert_lat(double lat)
{
	if( lat == 0 )
	{
		return 90.0;
	}

	return ( -lat*(R2D) + 90 );
}


double geoid::convert_lon(double lon)
{
	if( lon == 0 )
	{
		return 180.0;
	}

	return ( lon * (R2D) + 180.0 );
}

double geoid::GEO_calc_geoid_height( double lat, double lon )
{
	//convert to table input 
	double lat_table = convert_lat( lat );
	double lon_table = convert_lon( lon );

	return find_geoid_height( lat_table, lon_table );  
}
