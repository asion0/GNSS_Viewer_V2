// ConvertBinary2KML.cpp : 實作檔
//

#include "stdafx.h"
#include "GPS.h"
#include "ConvertBinary2KML.h"

#define NumOfCheckBox 75
CConvertBinary2KML* pKDlg;

HANDLE	hBainryReadEvent;
// CConvertBinary2KML 對話方塊

extern float FixedPointToSingle(U32 FixedPointValue, int NumberOfFractionalBits);
extern unsigned char Cal_Checksum(U08* pt);


IMPLEMENT_DYNAMIC(CConvertBinary2KML, CDialog)

CConvertBinary2KML::CConvertBinary2KML(CWnd* pParent /*=NULL*/)
	: CDialog(CConvertBinary2KML::IDD, pParent)
{

}

CConvertBinary2KML::~CConvertBinary2KML()
{
}

void CConvertBinary2KML::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_EDIT1, m_directory);
	DDX_Control(pDX, IDC_EDIT2, m_alltxt);
	DDX_Control(pDX, IDC_CHECK76, m_selectall);
	DDX_Control(pDX, IDC_CHECK78, m_hselectall);
	DDX_Control(pDX, IDC_CHECK1, m_check[0]);
	DDX_Control(pDX, IDC_CHECK2, m_check[1]);
	DDX_Control(pDX, IDC_CHECK3, m_check[2]);
	DDX_Control(pDX, IDC_CHECK4, m_check[3]);
	DDX_Control(pDX, IDC_CHECK5, m_check[4]);
	DDX_Control(pDX, IDC_CHECK6, m_check[5]);
	DDX_Control(pDX, IDC_CHECK7, m_check[6]);
	DDX_Control(pDX, IDC_CHECK8, m_check[7]);
	DDX_Control(pDX, IDC_CHECK9, m_check[8]);
	DDX_Control(pDX, IDC_CHECK10, m_check[9]);	
	DDX_Control(pDX, IDC_CHECK11, m_check[10]);	
	DDX_Control(pDX, IDC_CHECK12, m_check[11]);	
	DDX_Control(pDX, IDC_CHECK13, m_check[12]);	
	DDX_Control(pDX, IDC_CHECK14, m_check[13]);
	DDX_Control(pDX, IDC_CHECK15, m_check[14]);	
	DDX_Control(pDX, IDC_CHECK16, m_check[15]);	
	DDX_Control(pDX, IDC_CHECK17, m_check[16]);	
	DDX_Control(pDX, IDC_CHECK18, m_check[17]);	
	DDX_Control(pDX, IDC_CHECK19, m_check[18]);	
	DDX_Control(pDX, IDC_CHECK20, m_check[19]);	
	DDX_Control(pDX, IDC_CHECK21, m_check[20]);	
	DDX_Control(pDX, IDC_CHECK22, m_check[21]);	
	DDX_Control(pDX, IDC_CHECK23, m_check[22]);	
	DDX_Control(pDX, IDC_CHECK24, m_check[23]);	
	DDX_Control(pDX, IDC_CHECK25, m_check[24]);	
	DDX_Control(pDX, IDC_CHECK26, m_check[25]);	
	DDX_Control(pDX, IDC_CHECK27, m_check[26]);	
	DDX_Control(pDX, IDC_CHECK28, m_check[27]);	
	DDX_Control(pDX, IDC_CHECK29, m_check[28]);	
	DDX_Control(pDX, IDC_CHECK30, m_check[29]);	
	DDX_Control(pDX, IDC_CHECK31, m_check[30]);	
	DDX_Control(pDX, IDC_CHECK32, m_check[31]);	
	DDX_Control(pDX, IDC_CHECK33, m_check[32]);	
	DDX_Control(pDX, IDC_CHECK34, m_check[33]);	
	DDX_Control(pDX, IDC_CHECK35, m_check[34]);	
	DDX_Control(pDX, IDC_CHECK36, m_check[35]);	
	DDX_Control(pDX, IDC_CHECK37, m_check[36]);	
	DDX_Control(pDX, IDC_CHECK38, m_check[37]);	
	DDX_Control(pDX, IDC_CHECK39, m_check[38]);	
	DDX_Control(pDX, IDC_CHECK40, m_check[39]);	
	DDX_Control(pDX, IDC_CHECK41, m_check[40]);	
	DDX_Control(pDX, IDC_CHECK42, m_check[41]);	
	DDX_Control(pDX, IDC_CHECK43, m_check[42]);	
	DDX_Control(pDX, IDC_CHECK44, m_check[43]);	
	DDX_Control(pDX, IDC_CHECK45, m_check[44]);	
	DDX_Control(pDX, IDC_CHECK46, m_check[45]);	
	DDX_Control(pDX, IDC_CHECK47, m_check[46]);	
	DDX_Control(pDX, IDC_CHECK48, m_check[47]);	
	DDX_Control(pDX, IDC_CHECK49, m_check[48]);	
	DDX_Control(pDX, IDC_CHECK50, m_check[49]);	
	DDX_Control(pDX, IDC_CHECK51, m_check[50]);	
	DDX_Control(pDX, IDC_CHECK52, m_check[51]);	
	DDX_Control(pDX, IDC_CHECK53, m_check[52]);	
	DDX_Control(pDX, IDC_CHECK54, m_check[53]);	
	DDX_Control(pDX, IDC_CHECK55, m_check[54]);	
	DDX_Control(pDX, IDC_CHECK56, m_check[55]);	
	DDX_Control(pDX, IDC_CHECK57, m_check[56]);	
	DDX_Control(pDX, IDC_CHECK58, m_check[57]);	
	DDX_Control(pDX, IDC_CHECK59, m_check[58]);	
	DDX_Control(pDX, IDC_CHECK60, m_check[59]);	
	DDX_Control(pDX, IDC_CHECK61, m_check[60]);
	DDX_Control(pDX, IDC_CHECK62, m_check[61]);	
	DDX_Control(pDX, IDC_CHECK63, m_check[62]);	
	DDX_Control(pDX, IDC_CHECK64, m_check[63]);	
	DDX_Control(pDX, IDC_CHECK65, m_check[64]);	
	DDX_Control(pDX, IDC_CHECK66, m_check[65]);	
	DDX_Control(pDX, IDC_CHECK67, m_check[66]);	
	DDX_Control(pDX, IDC_CHECK68, m_check[67]);	
	DDX_Control(pDX, IDC_CHECK69, m_check[68]);	
	DDX_Control(pDX, IDC_CHECK70, m_check[69]);
	DDX_Control(pDX, IDC_CHECK71, m_check[70]);	
	DDX_Control(pDX, IDC_CHECK72, m_check[71]);	
	DDX_Control(pDX, IDC_CHECK73, m_check[72]);	
	DDX_Control(pDX, IDC_CHECK74, m_check[73]);
	DDX_Control(pDX, IDC_CHECK75, m_check[74]);	
	DDX_Control(pDX, IDC_COMBO1, m_color);

}


BEGIN_MESSAGE_MAP(CConvertBinary2KML, CDialog)
	ON_BN_CLICKED(IDC_BPLAYER, &CConvertBinary2KML::OnBnClickedBplayer)
	ON_BN_CLICKED(IDC_CHECK76, &CConvertBinary2KML::OnBnClickedCheck76)
	ON_BN_CLICKED(IDC_CHECK78, &CConvertBinary2KML::OnBnClickedCheck78)
	ON_BN_CLICKED(IDOK, &CConvertBinary2KML::OnBnClickedOk)
END_MESSAGE_MAP()


// CConvertBinary2KML 訊息處理常式

BOOL CConvertBinary2KML::OnInitDialog()
{
	CDialog::OnInitDialog();

	pKDlg = this;
	for(int i=0;i<NumOfCheckBox;i++)
	{
		m_check[i].ShowWindow(0);
	}
	m_alltxt.ShowWindow(0);	
	IsConvertFinish = false;
	m_color.SetCurSel(0);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX 屬性頁應傳回 FALSE
}

void CConvertBinary2KML::OnBnClickedBplayer()
{
	CString fileName;	
	U08 temp[255] = {0};

	CString a;	
	CFileDialog dlgFile(false, _T("dat"), fileName, OFN_HIDEREADONLY , _T("ALL Files (*.dat)|*.dat||"), this);
	dlgFile.GetOFN().lpstrFile = fileName.GetBuffer(1000);
	dlgFile.GetOFN().nMaxFile = 1000;	
	INT_PTR nResult = dlgFile.DoModal();
	fileName.ReleaseBuffer();
	if(nResult == IDOK)
	{
		int i,x;
		m_directory.SetWindowText(fileName);
		m_directory.GetWindowText(fileName);	
		for(i = fileName.GetLength();i>=0;i--)
		{
			TRACE("%c",fileName[i]);
			if(fileName[i] == '\\')	break;			
		}
		for( x = 0;x<i+1;x++)
		{
			temp[x]=fileName[x];
		}
		temp[x] = 0;
		sPath = temp;
		fileName.ReleaseBuffer();

		for(int i=0;i<NumOfCheckBox;i++)
		{
			m_check[i].SetCheck(0);
		}
		if(m_selectall.GetCheck() == BST_UNCHECKED)
			GetDatFile();
		else if(m_selectall.GetCheck() == BST_CHECKED)
			GetAllDatFile();
	}
	NoOfFile = 0;
	m_hselectall.SetCheck( BST_CHECKED);
}

void CConvertBinary2KML::GetDatFile()
{
	for(int i=0;i<NumOfCheckBox;i++)
	{
		m_check[i].ShowWindow(1);
	}
	num=0;
	//	CString sFilterName = sPath + sPathName; // 完整的路徑以及可用 "?" 或 "*" 表示的檔案名稱, 例如 "*.*" 表示所有檔案
	CString sFilterName = sPath+"*.dat"; 

	HANDLE hFind = NULL;
	WIN32_FIND_DATA fd;
	CString sTemp = "  ";
	if ((hFind = ::FindFirstFile ((LPCTSTR) sFilterName, &fd)) != INVALID_HANDLE_VALUE)
	{
		if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) // 檢查所搜尋到的是否為目錄名稱
		{
			FileName[num] = fd.cFileName;
			FilePath[num] = sPath + fd.cFileName; // fd.cFileName只有檔名, 請自行合併路徑後使用
			if(num<NumOfCheckBox)
				m_check[num].SetWindowText(FileName[num]);			
			while (::FindNextFile(hFind, &fd))
			{
				num++;
				if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					FileName[num] = fd.cFileName;
					FilePath[num] = sPath + fd.cFileName;
					if(num<NumOfCheckBox)
					{
						m_check[num].SetWindowText(FileName[num]);
						//	break;
					}
				}
			}
		}
		FindClose(hFind);
	}
	NumOfFile = num+1;
	for(int i=0;i<NumOfFile;i++) m_check[i].SetCheck(BST_CHECKED);	
	for(int i=NumOfFile;i<NumOfCheckBox;i++)m_check[i].ShowWindow(0);

}

void CConvertBinary2KML::GetAllDatFile()
{
	char msg[300];
	num=0;
	//	CString sFilterName = sPath + sPathName; // 完整的路徑以及可用 "?" 或 "*" 表示的檔案名稱, 例如 "*.*" 表示所有檔案
	CString sFilterName = sPath+"*.dat"; 	
	HANDLE hFind = NULL;
	WIN32_FIND_DATA fd;
	//CString sTemp = "  ";
	if ((hFind = ::FindFirstFile ((LPCTSTR) sFilterName, &fd)) != INVALID_HANDLE_VALUE)
	{
		if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) // 檢查所搜尋到的是否為目錄名稱
		{
			FileName[num] = fd.cFileName;
			FilePath[num] = sPath + fd.cFileName; // fd.cFileName只有檔名, 請自行合併路徑後使用				
			while (::FindNextFile(hFind, &fd))
			{
				num++;
				if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					FileName[num] = fd.cFileName;
					FilePath[num] = sPath + fd.cFileName;						
				}
			}
		}
		FindClose(hFind);
	}	
	NumOfFile = num+1;
	CString MSG;
	sprintf_s(msg, sizeof(msg), "Total (#%d) txt files selected.\r\n\r\n",NumOfFile);
	MSG = msg;
	for(int i=0;i<NumOfFile;i++)
	{		
		int x = i%10;
		if(i!=0 && !x)
			sprintf_s(msg, sizeof(msg), "%s \r\n",FileName[i]);
		else
			sprintf_s(msg, sizeof(msg), "%s ",FileName[i]);		
		MSG += msg;
	}
	m_alltxt.SetWindowText(MSG);
}


void CConvertBinary2KML::OnBnClickedCheck76()
{
	char nmea[100];
	CString temp;
	for(int i=0;i<NumOfCheckBox;i++)m_check[i].ShowWindow(0);	
	m_alltxt.ShowWindow(1);	
	CString fileName;

	m_directory.GetWindowText(fileName);
	if(fileName == "")	return;	
	else
	{
		if(m_selectall.GetCheck() == BST_UNCHECKED)
		{
			m_alltxt.ShowWindow(0);
			if(IsConvertFinish)
			{
				//for(int i=0;i<NumOfCheckBox;i++)
				//{
				//	if (m_check[i].GetCheck())
				//	{
				//		m_check[i].ShowWindow(1);
				//		m_check[i].SetCheck(BST_CHECKED);
				//		fileName = "OK ";
				//		fileName+=FileName[i];
				//		m_check[i].SetWindowText(fileName);
				//	}else
				//	{
				//		m_check[i].ShowWindow(0);
				//	}
				//}
				for(int i=0;i<NumOfFile;i++)
				{
					m_check[i].ShowWindow(1);
					m_check[i].SetCheck(BST_CHECKED);
					fileName = "OK ";
					fileName+=FileName[i];
					m_check[i].SetWindowText(fileName);
				}
				for(int i=NumOfFile;i<NumOfCheckBox;i++)m_check[i].ShowWindow(0);
			}
			else
				GetDatFile();
		}
		else if(m_selectall.GetCheck() == BST_CHECKED)
		{
			for(int i=0;i<NumOfCheckBox;i++)
			{
				m_check[i].ShowWindow(0);
			}
			m_alltxt.ShowWindow(1);	
			if(IsConvertFinish)
			{
				sprintf_s(nmea, sizeof(nmea), "Total (#%d) txt files selected.\r\n\r\n",NumOfFile);
				temp = nmea;
				temp += "Successful";					
				m_alltxt.SetWindowText(temp);
			}
			else
				GetAllDatFile();
		}
	}
}

void CConvertBinary2KML::OnBnClickedCheck78()
{
	m_alltxt.ShowWindow(0);	
	CString fileName;
	m_directory.GetWindowText(fileName);
	if(fileName == "")	return;	
	else
	{
		if(m_hselectall.GetCheck() == BST_UNCHECKED)
		{
			for(int i=0;i<NumOfFile;i++){m_check[i].SetCheck(BST_UNCHECKED);m_check[i].ShowWindow(1);m_check[i].SetWindowText(FileName[i]);}
			for(int i=NumOfFile;i<NumOfCheckBox;i++)m_check[i].ShowWindow(0);				
		}
		else if(m_hselectall.GetCheck() == BST_CHECKED)
		{
			for(int i=0;i<NumOfFile;i++){ m_check[i].SetCheck(BST_CHECKED);m_check[i].ShowWindow(1);m_check[i].SetWindowText(FileName[i]);}		
			for(int i=NumOfFile;i<NumOfCheckBox;i++)m_check[i].ShowWindow(0);					
		}
	}
}

int CConvertBinary2KML::GET_NMEA_SENTENCE(CFile& file, unsigned char* buffer)
{
	unsigned char nmea[2];
	unsigned int   length=0;
	int nBytesRead = 0;
	do
	{
		memset(nmea,0,2);
		nBytesRead = file.Read(nmea,1);		
		*(buffer+length) = nmea[0];
		length += nBytesRead;
		if(nBytesRead == 0 || *(buffer+length-1)=='\n' && *(buffer+length-2)=='\r' || length == 200)
			break;		
	}while(1);
	return length;
}

void  CConvertBinary2KML::convert_gps_time_to_utc( S16 wn, D64 tow, UTC_T* utc_p )
{
	// default leap secs has passed away
	const S16 DEFAULT_PRIOR_LEAP_SECS = 15;
	// GPS Time start at 1980 Jan. 5/6 mid-night
	const S16 INIT_UTC_YEAR = 1980;     

	const S16 DAYS_PER_YEAR = 365;
	const S16 DAYS_PER_4_YEARS = (DAYS_PER_YEAR*4 + 1);  // plus one day for leap year

	const S16 day_of_year_month_table[] = 
	{ 0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365 };
	const S16 day_of_leap_year_month_table[] = 
	{ 0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366 };

	S16 i;
	S32 tow_int;
	D64 tow_frac;
	S32 total_utc_sec, total_utc_day;
	S32 passed_leap_days, passed_utc_years,
		day_of_utc_year, leap_days_of_prev_years;
	S32 sec_of_day;
	const S16* month_tbl_p = day_of_year_month_table;

	tow_int  = (U32)floor( tow );
	tow_frac = tow - (D64) tow_int;
	total_utc_sec = 604800L*wn + tow_int - DEFAULT_PRIOR_LEAP_SECS;
	total_utc_day = total_utc_sec / 86400L;
	sec_of_day = total_utc_sec - 86400L * total_utc_day;

	passed_leap_days = (total_utc_day + DAYS_PER_4_YEARS 
		- day_of_leap_year_month_table[2] + 5) / DAYS_PER_4_YEARS;
	passed_utc_years = (total_utc_day + 5 - passed_leap_days) / 365;
	leap_days_of_prev_years = (passed_utc_years + 3) / 4;

	day_of_utc_year = total_utc_day + 5 - passed_utc_years*DAYS_PER_YEAR
		- leap_days_of_prev_years;

	utc_p->year = 1980 + (S16)passed_utc_years;
	if( (utc_p->year & 0x3) == 0 )  // utc->year % 4
		month_tbl_p = day_of_leap_year_month_table;  // this year is leap year

	for( i=1; i<13; i++ )
		if( day_of_utc_year < month_tbl_p[i] )
			break;

	utc_p->month = i;
	utc_p->day   = (S16)day_of_utc_year - month_tbl_p[i-1] + 1;

	utc_p->hour = (S16)(sec_of_day / 3600);
	if( utc_p->hour > 23 )
		utc_p->hour = 23;

	utc_p->minute = (S16)((sec_of_day - utc_p->hour*3600) / 60);
	if( utc_p->minute > 59 )
		utc_p->minute = 59;

	utc_p->sec = (F32)(sec_of_day - utc_p->hour*3600L - utc_p->minute*60L)
		+ (F32)tow_frac;
}

void CConvertBinary2KML::Show_NI_Timing(U08 *buff)
{
	unsigned long long nanoseconds;
	double seconds;
//	U08 leap_second;
	U32 wn;
	double tow;
	UTC_T utc;
	U32 temp1,temp2;
	U08 status,pvt_mode,fixed_mode;

	U08 *ptr = buff;
	ptr+=5;
	temp1 = ptr[0]<<24 | ptr[1]<<16 | ptr[2]<<8 | ptr[3];
	temp2 = ptr[4]<<24 | ptr[5]<<16 | ptr[6]<<8 | ptr[7];
	nanoseconds = temp1;
	nanoseconds = nanoseconds<<32 | temp2;

	seconds = nanoseconds / 1000000000.0 - 315964819;

	if (seconds>0)
	{
		wn = (S16)(seconds / 604800);
		tow = seconds - 604800*wn;
		convert_gps_time_to_utc((S16)wn, tow, &utc);
	}else
	{
		utc.year = 0;
		utc.month = 0;
		utc.day = 0;
		utc.hour = 0;
		utc.minute = 0;
		utc.sec = 0;
	}

	msg_gprmc.Year = utc.year;
	msg_gprmc.Month = utc.month;
	msg_gprmc.Day = utc.day;

	msg_gprmc.Hour = msg_gpgga.Hour = utc.hour;
	msg_gprmc.Min = msg_gpgga.Min = utc.minute;
	msg_gprmc.Sec = msg_gpgga.Sec = utc.sec;

	status = ptr[9];
	pvt_mode = status & 0x03;
	fixed_mode = (status>>2) & 0x03;

	if (fixed_mode <= 1)
	{
		TRACE("");
	}
}

void CConvertBinary2KML::Show_NI_PVT(U08 *buff)
{
	U08 seq_nu,fixed_type;
	U32 temp;
	U16 mtemp;
	F32 ftemp;
	U08 *ptr = buff;

	ptr+=5;
	seq_nu = (*ptr) & 0x3F;
	fixed_type = (*ptr) >>6;
	//msg_gpgsacopy.Mode = fixed_type;
	msg_gpgga.GPSQualityIndicator = 0x2F + fixed_type;
	msg_gprmc.Status = (fixed_type>1) ? 'A' : 'V';
	ptr++;
	temp = ptr[0]<<24 | ptr[1]<<16 | ptr[2]<<8 | ptr[3];
	ftemp = FixedPointToSingle(temp,20);
	msg_gpgga.Latitude = (int)(ftemp)*100+(ftemp-(int)ftemp)*60;
	msg_gpgga.Latitude_N_S = msg_gpgga.Latitude >= 0 ? 'N' : 'S';
	msg_gprmc.Latitude = msg_gpgga.Latitude;
	msg_gprmc.Latitude_N_S = msg_gpgga.Latitude_N_S;

	ptr+=4;
	temp = ptr[0]<<24 | ptr[1]<<16 | ptr[2]<<8 | ptr[3];
	ftemp = FixedPointToSingle(temp,20);
	msg_gpgga.Longitude = (int)(ftemp)*100+(ftemp-(int)ftemp)*60;
	msg_gpgga.Longitude_E_W = msg_gpgga.Longitude >= 0 ? 'E' : 'W';
	msg_gprmc.Longitude = msg_gpgga.Longitude;
	msg_gprmc.Longitude_E_W = msg_gpgga.Longitude_E_W;

	ptr+=4;
	temp = ptr[0]<<24 | ptr[1]<<16 | ptr[2]<<8 | ptr[3];
	msg_gpgga.Altitude = FixedPointToSingle(temp,7);
	ptr+=4;
	temp = ptr[0]<<24 | ptr[1]<<16 | ptr[2]<<8 | ptr[3];
	msg_gpvtg.SpeedKmPerHur = FixedPointToSingle(temp,12) * 3.6F;
	msg_gprmc.SpeedKnots = msg_gpvtg.SpeedKmPerHur / 1.852F;
	ptr+=4;
	mtemp = ptr[0]<<8 | ptr[1];
	msg_gprmc.TrueCourse = FixedPointToSingle(mtemp,5);

	int sec_dd,sec_mm;
	int lat_dd,lat_mm;
	int lon_dd,lon_mm;
	int speed_dd=0,speed_mm=0;
	int course_dd=0,course_mm=0;

	sec_dd = (int)msg_gprmc.Sec;
	sec_mm = (int)((msg_gprmc.Sec - sec_dd) * 100);

	if (msg_gprmc.Latitude_N_S == 'S')
	{
		lat_dd = -1*(int)msg_gprmc.Latitude;
		lat_mm = (int)((msg_gprmc.Latitude - msg_gprmc.Latitude) * 10000);
	}
	else
	{
		lat_dd = (int)msg_gprmc.Latitude ;
		lat_mm = (int)((msg_gprmc.Latitude-msg_gprmc.Latitude)*10000);
	}
	
	if (msg_gprmc.Longitude_E_W == 'W') 
	{
		lon_dd = -1*(int)msg_gprmc.Longitude ;
		lon_mm = (int)((msg_gprmc.Longitude-msg_gprmc.Longitude)*10000);
	}else
	{
		lon_dd = (int)msg_gprmc.Longitude ;
		lon_mm = (int)((msg_gprmc.Longitude-msg_gprmc.Longitude)*10000);
	}



	speed_dd = (int)msg_gprmc.SpeedKnots;
	speed_mm = (int)((msg_gprmc.SpeedKnots-msg_gprmc.SpeedKnots)*10);
	char msg[256];
	int len = sprintf_s(msg, sizeof(msg), "$GPRMC,%02d%02d%02d.%03d,%c,%02d.%04d,%c,%03d.%04d,%c,%03d.%01d,%03d.%01d,%02d%02d%02d,,,A",
		msg_gprmc.Hour,msg_gprmc.Min,sec_dd,sec_mm,msg_gprmc.Status,lat_dd,lat_mm,msg_gprmc.Latitude_N_S,
		lon_dd,lon_mm,msg_gprmc.Longitude_E_W,speed_dd,speed_mm,course_dd,course_mm,msg_gprmc.Day,msg_gprmc.Month,(msg_gprmc.Year == 0)? 0: msg_gprmc.Year-2000);

	char checksum = 0;	
	int i;

	for(i=1;i<len;i++)
	{		
		checksum^=msg[i];
	}	

	len = sprintf_s(msg, sizeof(msg), "%s*%02X\r\n",msg,checksum);

	fwrite(msg,1,len,nmea_file);

	//add2message(msg,len);

}

bool CConvertBinary2KML::BINARY_PROC(unsigned char* buffer,int len)
{
	static bool has_time = false;
	unsigned char MSG_TYPE;
	int tmp_len = *(buffer+2);
	tmp_len = tmp_len<<8 | *(buffer+3);

	if (tmp_len+7 > len) 
		return false;
	MSG_TYPE = Cal_Checksum(buffer);	
	//char message[512];


	switch(MSG_TYPE)
	{
		// ----------------- NI Binary Message ---------------------
	case 0xD2:
		Show_NI_Timing(buffer);
		has_time = true;
		break;
	case 0xD3:
		Show_NI_PVT(buffer);
		if (has_time) return true;
		break;
	case 0xD4:
		break;
	default:
		TRACE("MSG_TYPE=%d\r\n",MSG_TYPE);
		if (buffer[1] != 0xA1)
		{
			TRACE("0xA1 missing\r\n");
		}
		// ---------------------------------------------------------
	}

	return false;

}

void CConvertBinary2KML::WriteToFile()
{	
	//#if IS_DEBUG
	if (msg_gprmc.Status == 'A')
	{
		lon = int(msg_gprmc.Longitude/100);	
		lon+=double(msg_gprmc.Longitude-int(msg_gprmc.Longitude/100)*100)/60;	

		lat = int(msg_gprmc.Latitude/100);	
		lat+=double(msg_gprmc.Latitude-int(msg_gprmc.Latitude/100)*100)/60;	

		LL2 kml_lla;
		kml_lla.lat = lat;
		kml_lla.lon = lon;
		kml_lla.alt = 0;
		kml_lla.utc.hour = msg_gprmc.Hour;
		kml_lla.utc.minute = msg_gprmc.Min;
		kml_lla.utc.sec = msg_gprmc.Sec;
		kml_lla.speed = msg_gprmc.SpeedKnots * 1.852;

		kml.push_one_point(&kml_lla);
	}
	else if(msg_gpgga.GPSQualityIndicator==49 || msg_gpgga.GPSQualityIndicator==50 || msg_gpgga.GPSQualityIndicator==54)
	{
		//#endif
		lon = int(msg_gpgga.Longitude/100);	
		lon+=double(msg_gpgga.Longitude-int(msg_gpgga.Longitude/100)*100)/60;	

		lat = int(msg_gpgga.Latitude/100);	
		lat+=double(msg_gpgga.Latitude-int(msg_gpgga.Latitude/100)*100)/60;	

		LL2 kml_lla;
		kml_lla.lat = lat;
		kml_lla.lon = lon;
		kml_lla.alt = msg_gpgga.Altitude;
		kml_lla.utc.hour = msg_gpgga.Hour;
		kml_lla.utc.minute = msg_gpgga.Min;
		kml_lla.utc.sec = msg_gpgga.Sec;
		kml_lla.speed = 0;
		kml.push_one_point(&kml_lla);
	}
}

void CConvertBinary2KML::Convert()
{
	int    nBytesRead;
	UINT   retCode = 0;	
	U08    nmea[200];
	CString temp;
	DWORD  dwBytesRemaining = (DWORD)Fsource.GetLength();

	int write_count = 0;
	int file_tail = 0;
	U32 color;

	int color_index = m_color.GetCurSel();
	//Red;Yellow;Blue;Green;
	if (color_index==0)
		color = 0x0000ff;
	else if (color_index == 1)
		color = 0x00FFFF;
	else if (color_index == 2)
		color = 0xff0000;
	else if (color_index == 3)
		color = 0x00ff00;

	while(dwBytesRemaining)
	{	
		CString tmp_file ;
		tmp_file.Format("%s%d%s",kml_filename,file_tail,".kml");
		kml.init(tmp_file,color);

		tmp_file.Format("%s%d%s",kml_filename,file_tail,".txt");
		fopen_s(&nmea_file, tmp_file, "wb+");

		while(dwBytesRemaining)
		{
			memset(nmea,0,200);		
			nBytesRead = GET_NMEA_SENTENCE(Fsource,nmea);	
			dwBytesRemaining-=nBytesRead;	
			if (nmea[0] == 0xA0 && nmea[1] == 0xA1)
			{

				bool ret = BINARY_PROC(nmea,nBytesRead);

				
				if (ret) 
				{
					WriteToFile();

					write_count++;
					if ( write_count > 65000)
					{
						write_count = 0;
						file_tail++;
						break;
					}
				}
			}
		}
		kml.finish();

		fclose(nmea_file);
	}	

	Fsource.Close();
	//Fnmea.Close();
	IsFSourceOpen = false;	
	if(!SetEvent(hBainryReadEvent))	DWORD error = GetLastError();
}


void CConvertBinary2KML::convert_all_file()
{
	int i;
	CString temp;
	for(i=0;i<NumOfFile;i++)
	{
		CFileException ef;
		try
		{		        
			if(!Fsource.Open(FilePath[i],CFile::modeRead,&ef))
			{
				ef.ReportError();
				continue;
			}
			IsFSourceOpen = true;

			kml_filename = FileName[i];
			int find = kml_filename.Find(".");
			kml_filename = kml_filename.Mid(0,find);   

			Convert();

			temp.Format("%s OK",FileName[i]);
			m_check[i].SetWindowText(temp);
			m_check[i].SetCheck(BST_CHECKED);	
		}
		catch(CFileException *fe)
		{
			fe->ReportError();
			fe->Delete();
		}	
	}
}

UINT BinaryToKml(LPVOID pParam)
{	
	pKDlg->convert_all_file();
	//AfxEndThread(0);	
	return 0;
}

void CConvertBinary2KML::OnBnClickedOk()
{
	memset(&msg_gpgga,0,sizeof(GPGGA));
	inilog = true;
	IsConvertFinish = false;
	CString fileName;	
	char name[100];
	memset(name,0,100);
	hBainryReadEvent= CreateEvent(NULL, TRUE, FALSE, NULL);
	if(!ResetEvent(hBainryReadEvent))	DWORD error = GetLastError();
	m_directory.GetWindowText(fileName);
	if(fileName == "")
	{
		int ret = AfxMessageBox("您尚未選擇任何dat檔",MB_YESNOCANCEL|MB_ICONQUESTION   ,0);		
	}
#if 1 

	if(m_selectall.GetCheck() == BST_UNCHECKED)
	{
		int numbers= 0;
		for(int i=0;i<NumOfCheckBox;i++)
		{
			if(m_check[i].GetCheck() == BST_CHECKED)
			{
				FileName[numbers]  =  FileName[i];
				FilePath[numbers]  =  FilePath[i];
				numbers++;
			}
		}
		NumOfFile = numbers;
	}
	if(!NumOfFile) return;


	for(int i=0;i<NumOfCheckBox;i++) 
	{
		m_check[i].SetCheck(BST_UNCHECKED);	
		m_check[i].SetWindowText(FileName[i]);
	}

	for(int i=NumOfFile;i<NumOfCheckBox;i++)m_check[i].ShowWindow(0);

	//NoOfFile++;
	AfxBeginThread(BinaryToKml,0);	
#endif
	//AfxBeginThread(ContinueToConvert , 0);
}
