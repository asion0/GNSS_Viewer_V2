#pragma once

#define utcTimeSize		7
#define utcDateSize		7
#define utcLonSize		12
#define utcLatSize		12
struct rmc_nmea
{
	char utctime[utcTimeSize];
	char utcdate[utcDateSize];
	char lon[utcLonSize];
	char lat[12];
	char n_s;
	char e_w;
	float speed;
	int mode;
};

class log2nmea
{
public:
	log2nmea(void);
	~log2nmea(void);

	void set_LoggFile(const char *file);
	void convert2nmea();

protected:
	const char *m_filename;

	int poi_c;

	bool conv2rmc(char *src, char *dst, int dstSize);

	void parse_rmc(char *src, rmc_nmea *rmc);
	void parse_date(char* src, char *dst, int dstSize);
	void parse_time(char* src,char *dst, int dstSize);
	bool parse_lonlat(double src, char *dst, int dstSize);
	void parse_symbol(char *src, int src_len, char *dst, int *len_dst, char symbol);
	void add_chksum(char *src, int size);
};
