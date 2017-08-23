// ConDauDlg.cpp 
//

#include "stdafx.h"
/*
#include "GPS.h"
#include "ConDauDlg.h"
#include "GPSDlg.h"

#define SMA  6370000000
#define IF1  2930000000

CConDauDlg* pDauDlg;
// CConDauDlg 
static U16 EllipseList;
static U08 EllipseIndex;
static S32 DeltaX;
static S32 DeltaY;
static S32 DeltaZ;
static U32 Semi_Major_Axis;
static U32 Inversed_Flattening;
static U08 attribute;

static EL el[24] = {
	{0, 0},
	{6377563396-SMA, 2993249646-IF1},
	{6377340189-SMA, 2993249646-IF1},
	{6378160000-SMA, 2982500000-IF1},
	{6377483865-SMA, 2991528128-IF1},
	{6377397155-SMA, 2991528128-IF1},
	{6378206400-SMA, 2949786982-IF1},
	{6378249145-SMA, 2934650000-IF1},
	{6377276345-SMA, 3008017000-IF1},
	{6377298556-SMA, 3008017000-IF1},
	{6377301243-SMA, 3008017000-IF1},
	{6377295664-SMA, 3008017000-IF1},
	{6377304063-SMA, 3008017000-IF1},
	{6377309613-SMA, 3008017000-IF1},
	{6378155000-SMA, 2983000000-IF1},
	{6378200000-SMA, 2983000000-IF1},
	{6378270000-SMA, 2970000000-IF1},
	{6378160000-SMA, 2982470000-IF1},
	{6378388000-SMA, 2970000000-IF1},
	{6378245000-SMA, 2983000000-IF1},
	{6378137000-SMA, 2982572221-IF1},
	{6378160000-SMA, 2982500000-IF1},
	{6378135000-SMA, 2982600000-IF1},
	{6378137000-SMA, 2982572235-IF1} 
};

static DRL datum[] = {
	{   0,    0,    0, el[23].a, el[23].I_F,23},
	{-118,  -14,  218, el[7].a,  el[7].I_F ,7},
	{-134,   -2,  210, el[7].a,  el[7].I_F,7 },
	{-165,  -11,  206, el[7].a,  el[7].I_F ,7},
	{-123,  -20,  220, el[7].a,  el[7].I_F ,7},
	{-166,  -15,  204, el[7].a,  el[7].I_F ,7},
	{-128,  -18,  224, el[7].a,  el[7].I_F ,7},
	{-161,  -14,  205, el[7].a,  el[7].I_F ,7},
	{ -43, -163,   45, el[19].a, el[19].I_F,19},
	{-150, -250,   -1, el[18].a, el[18].I_F,18},
	{-143, -236,    7, el[18].a, el[18].I_F,18},
	{-115,	 118, 426, el[6].a,  el[6].I_F ,6},
	{-491,  -22,  435, el[3].a,  el[3].I_F ,3},
	{-270,	 13,   62, el[7].a,  el[7].I_F ,7},
	{-138, -105, -289, el[7].a, el[7].I_F,7},
	{-153,	  -5, -292, el[7].a, el[7].I_F ,7},
	{-125, -108, -295, el[7].a, el[7].I_F,7},
	{-161,	 -73, -317, el[7].a, el[7].I_F ,7},
	{-143,	-90, -294, el[7].a, el[7].I_F,7},
	{-134,	-105, -295, el[7].a, el[7].I_F ,7},
	{-169,	-19, -278, el[7].a, el[7].I_F,7},
	{-147,	 -74, -283, el[7].a, el[7].I_F ,7},
	{-142,	-96, -293, el[7].a, el[7].I_F,7},
	{-160,	  -6, -302, el[7].a, el[7].I_F ,7},
	{-157,	 -2, -299, el[7].a, el[7].I_F,7},
	{-175,	 -23, -303, el[7].a, el[7].I_F ,7},
	{-205,	107,   53, el[18].a, el[18].I_F,18},
	{ 145,	 75, -272, el[18].a, el[18].I_F,18},
	{-320,	550, -494, el[18].a, el[18].I_F,18},
	{ 114,	-116,-333, el[18].a, el[18].I_F,18},
	{ 124, -234,  -25, el[18].a, el[18].I_F,18},
	{-133,  -48,  148, el[3].a,  el[3].I_F ,3},
	{-134,	-48,  149, el[3].a, el[3].I_F,3},
	{ -79,	-129,  145,el[7].a,  el[7].I_F,7},
	{-127, -769,  472, el[18].a, el[18].I_F,18},
	{ -73,	 213,  296, el[6].a, el[6].I_F,6},
	{-173,	253,   27, el[18].a, el[18].I_F,18},   
	{ 307,	 304, -318, el[18].a, el[18].I_F,18},
	{-384,	664,  -48, el[5].a,  el[5].I_F,5},
	{-104,	-129,  239, el[18].a, el[18].I_F,18},	
	{-148,	136,   90, el[18].a, el[18].I_F,18},
	{ 298,	-304, -375, el[18].a, el[18].I_F,18},
	{-136, -108, -292, el[7].a, el[7].I_F,7},
	{  -2,	 151,  181, el[6].a, el[6].I_F,6},
	{-263,	  6,  431, el[7].a, el[7].I_F,7},
	{ 175,	 -38,  113, el[18].a, el[18].I_F,18},	
	{-134,	229,  -29, el[18].a, el[18].I_F,18},
	{-206,	 172,	-6, el[18].a, el[18].I_F,18},
	{ -83,	 37,  124, el[7].a, el[7].I_F,7},     
	{ 260,	  12, -147, el[7].a, el[7].I_F,7},
	{-377,	681,  -50, el[5].a, el[5].I_F,5},
	{ 230,	-199, -752, el[18].a, el[18].I_F,18},	
	{ 211,	147,  111, el[18].a, el[18].I_F,18},
	{ 374,	 150,  588, el[5].a, el[5].I_F,5},
	{-104, -101, -140, el[18].a, el[18].I_F,18},   
	{-130,	-117, -151, el[18].a, el[18].I_F,18},
	{ -86,  -96, -120, el[18].a, el[18].I_F,18},   
	{ -86,	 -96, -120, el[18].a, el[18].I_F,18},
	{ -87,	-95, -120, el[18].a, el[18].I_F,18},   
	{ -84,	 -95, -130, el[18].a, el[18].I_F,18},
	{-117, -132, -164, el[18].a, el[18].I_F,18},   
	{ -97,	-103, -120, el[18].a, el[18].I_F,18},
	{ -97,	-88, -135, el[18].a, el[18].I_F,18},   
	{-107,	 -88, -149, el[18].a, el[18].I_F,18},
	{ -87,	-98, -121, el[18].a, el[18].I_F,18},   
	{ -87,	 -96, -120, el[18].a, el[18].I_F,18},
	{-103, -106, -141, el[18].a, el[18].I_F,18},   
	{ -84,	-107, -120, el[18].a, el[18].I_F,18},
	{-112,	-77, -145, el[18].a, el[18].I_F,18},   
	{ -86,	 -98, -119, el[18].a, el[18].I_F,18},
	{  -7,	215,  225, el[7].a,  el[7].I_F,7},
	{-133,	-321,	50, el[18].a,  el[18].I_F,18},	
	{  84,	-22,  209,el[18].a,  el[18].I_F,18},
	{-104,	 167,  -38,el[18].a,  el[18].I_F ,18},
	{-100, -248,  259, el[6].a,  el[6].I_F,6},
	{-403,	 684,	41, el[5].a,  el[5].I_F,5},
	{ 252, -209, -751, el[18].a,  el[18].I_F,18},  
	{-333,	-222,  114, el[18].a,  el[18].I_F,18},
	{ 653, -212,  449, el[4].a,  el[4].I_F,4},
	{ -73,	  46,  -86, el[18].a,  el[18].I_F,18},	
	{-156, -271, -189, el[18].a,  el[18].I_F,18},
	{-637,	-549, -203, el[18].a,  el[18].I_F,18},
	{ 282,  726,  254, el[8].a,  el[8].I_F,8},
	{ 295,	 736,  257, el[10].a,  el[10].I_F,10},
	{ 283,	682,  231, el[13].a,  el[13].I_F,13},
	{ 217,	 823,  299, el[8].a,  el[8].I_F,8},	
	{ 182,	915,  344, el[8].a,  el[8].I_F,8},
	{ 198,	 881,  317, el[8].a,  el[8].I_F,8},	
	{ 210,	814,  289, el[8].a,  el[8].I_F,8},
	{ -24,	 -15,	 5, el[17].a,  el[17].I_F,17},
	{ 506, -122,  611, el[2].a,  el[2].I_F,2},
	{-794,	 119, -298, el[18].a,  el[18].I_F,18},	
	{ 208, -435, -229, el[18].a,  el[18].I_F,18},
	{ 189,	 -79, -202, el[18].a,  el[18].I_F,18},
	{ -97,	787,   86,  el[8].a,  el[8].I_F,8},
	{ 145,	-187,  103, el[18].a,  el[18].I_F,18},
	{ -11,	851,	5, el[12].a,  el[12].I_F,12},
	{ 647,	1777,-1124, el[18].a,  el[18].I_F,18},
	{   0,	  0,	0,  el[20].a,  el[20].I_F,20},
	{  42,	 124,  147, el[6].a,  el[6].I_F,6},
	{-130,	 29,  364, el[7].a,  el[7].I_F,7},    
	{ -90,    40,	88, el[7].a,  el[7].I_F,7},
	{-133,	-77,  -51, el[6].a,  el[6].I_F,6},    
	{-133,	 -79,  -72, el[6].a,  el[6].I_F,6},
	{ -74, -130,   42, el[7].a,  el[7].I_F,7},    
	{  41,	-220, -134, el[7].a,  el[7].I_F,7},
	{ 639,	405,   60, el[5].a,  el[5].I_F,5},    
	{  31,	 146,	47, el[7].a,  el[7].I_F,7},
	{ 912,	-58, 1227, el[18].a,  el[18].I_F,18},
	{ -81,	 -84,  115, el[7].a,  el[7].I_F,7},	
	{ -92,	 -93,  122, el[7].a,  el[7].I_F,7},
	{ 174,	 359,  365, el[7].a,  el[7].I_F,7},	
	{-247,  -148,  369, el[7].a,  el[7].I_F,7},
	{-243,	-192,  477, el[7].a,  el[7].I_F,7},	
	{-249,  -156,  381, el[7].a,  el[7].I_F,7},
	{ -10,	 375,  165, el[18].a,  el[18].I_F,18},
	{  -5,	135,  172, el[6].a,  el[6].I_F,6},    
	{  -2,	 152,  149, el[6].a,  el[6].I_F,6},
	{   2,	204,  105, el[6].a,  el[6].I_F,6},    
	{  -4,	 154,  178, el[6].a,  el[6].I_F,6},
	{   1,	140,  165, el[6].a,  el[6].I_F,6},    
	{  -7,	 162,  188, el[6].a,  el[6].I_F,6},
	{  -9,	157,  184, el[6].a,  el[6].I_F,6}, 
	{ -22,   160,  190, el[6].a,  el[6].I_F,6},
	{   4,	159,  188, el[6].a,  el[6].I_F,6}, 
	{  -7,	 139,  181, el[6].a,  el[6].I_F,6},
	{   0,	125,  201, el[6].a,  el[6].I_F,6}, 
	{  -9,	 152,  178, el[6].a,  el[6].I_F,6},
	{  11,	114,  195, el[6].a,  el[6].I_F,6}, 
	{  -3,	 142,  183, el[6].a,  el[6].I_F,6},
	{   0,	125,  194, el[6].a,  el[6].I_F,6}, 
	{ -10,	 158,  187, el[6].a,  el[6].I_F,6},
	{  -8,	160,  176, el[6].a,  el[6].I_F,6},    
	{  -9,	 161,  179, el[6].a,  el[6].I_F,6},
	{  -8,	159,  175, el[6].a,  el[6].I_F,6},    
	{ -12,	 130,  190, el[6].a,  el[6].I_F,6},
	{   0,	  0,    0, el[20].a, el[20].I_F,20},   
	{  -2,	   0,	 4, el[20].a,  el[20].I_F,20},
	{   0,	  0,	0, el[20].a, el[20].I_F,20},  
	{   0,	   0,	 0, el[20].a,  el[20].I_F,20},
	{   1,	  1,   -1, el[20].a, el[20].I_F,20}, 
	{   0,	   0,	 0, el[20].a,  el[20].I_F,20},
	{-186,	-93,  310, el[7].a,  el[7].I_F,7}, 
	{ -425,	-169,	81,  el[18].a,  el[18].I_F,18},
	{-130,	110,  -13,  el[15].a,  el[15].I_F,15},
	{  89,	-279, -183,  el[6].a,  el[6].I_F,6}, 
	{  45, -290, -172,  el[6].a,  el[6].I_F,6},
	{  65,	-290, -190,  el[6].a,  el[6].I_F,6},	
	{  61, -285, -181,  el[6].a,  el[6].I_F,6},
	{  58,	-283, -182,  el[6].a,  el[6].I_F,6},
	{-346,	 -1,   224,  el[7].a,  el[7].I_F,7},
	{ 371,	-112,  434,  el[1].a,  el[1].I_F,1}, 	
	{ 371, -111,  434,  el[1].a,  el[1].I_F,1},
	{ 375,	-111,  431,  el[1].a,  el[1].I_F,1},	
	{ 384, -111,  425,  el[1].a,  el[1].I_F,1},
	{ 370,	-108,  434,  el[1].a,  el[1].I_F,1},
	{-307,	-92,  127,  el[18].a,  el[18].I_F,18},
	{ 185,  165,	42,  el[18].a,  el[18].I_F,18},
	{-106, -129,  165,  el[7].a,  el[7].I_F,7},
	{-148,	  51, -291,  el[18].a,  el[18].I_F,18},
	{-499, -249,  314,  el[18].a,  el[18].I_F,18}, 
	{-270,  188, -388,  el[18].a,  el[18].I_F,18},
	{-270,	183, -390,  el[18].a,  el[18].I_F,18}, 
	{-305,	 243, -442,  el[18].a,  el[18].I_F,18},
	{-282,	169, -371,  el[18].a,  el[18].I_F,18}, 
	{-278,	 171, -367,  el[18].a,  el[18].I_F,18},
	{-298,	159, -369,  el[18].a,  el[18].I_F,18}, 
	{-288,	 175, -376,  el[18].a,  el[18].I_F,18},
	{-279,	175, -379,  el[18].a,  el[18].I_F,18}, 
	{-295,	 173, -371,  el[18].a,  el[18].I_F,18},
	{  16,	196,   93,  el[18].a,  el[18].I_F,18},
	{  11,	  72, -101,  el[6].a,  el[6].I_F,6},
	{  28, -130,  -95,  el[19].a,  el[19].I_F,19},
	{-128,	-283,  22,  el[18].a,  el[18].I_F,18}, 
	{ 164,	138, -189,  el[18].a,  el[18].I_F,18},
	{  94,	-948,-1262,  el[18].a,  el[18].I_F,18}, 
	{-225,	-65,	9,  el[18].a,  el[18].I_F,18},
	{  28,	-121,  -77,  el[19].a,  el[19].I_F,19},
	{  23, -124,  -82,  el[19].a,  el[19].I_F,19},
	{  26,	-121,  -78,   el[19].a,  el[19].I_F,19},
	{  24, -124,  -82,    el[19].a,  el[19].I_F,19}, 
	{  15,	-130,  -84,    el[19].a,  el[19].I_F,19},
	{  24, -130,  -92,    el[19].a,  el[19].I_F,19}, 
	{  28,	-121,  -77,   el[19].a,  el[19].I_F,19},
	{ 589,	 76,  480,   el[5].a,  el[5].I_F,5},
	{ 170,	  42,	84,   el[18].a,  el[18].I_F,18},
	{-203,	141,   53,   el[18].a,  el[18].I_F,5}, 
	{-355,	  21,	72,   el[18].a,  el[18].I_F,18},
	{ 616,	 97, -251,   el[4].a,  el[4].I_F,4}, 
	{-289,	-124,	60,   el[18].a,  el[18].I_F,18},
	{ -88,	  4,  101,   el[7].a,  el[7].I_F,7},
	{ -62,   -1,  -37,   el[21].a,  el[21].I_F,21},
	{ -61,    2,  -48,  el[21].a,  el[21].I_F,21}, 
	{ -60,   -2,  -41,   el[21].a,  el[21].I_F,21},
	{ -75,	 -1,  -44,   el[21].a,  el[21].I_F,21}, 
	{ -44,	   6,  -36,   el[21].a,  el[21].I_F,21},
	{ -48,    3,  -44,  el[21].a,  el[21].I_F,21}, 
	{ -47,	  26,  -42,   el[21].a,  el[21].I_F,21},
	{ -53,	  3,  -47,   el[21].a,  el[21].I_F,21}, 
	{ -57,	   1,  -41,   el[21].a,  el[21].I_F,21},
	{ -61,	  2,  -33,   el[21].a,  el[21].I_F,21}, 
	{ -58,	   0,  -44,   el[21].a,  el[21].I_F,21},
	{ -45,	 12,  -33,   el[21].a,  el[21].I_F,21}, 
	{ -45,	   8,  -33,   el[21].a,  el[21].I_F,21},
	{   7,	-10,  -26,  el[14].a,  el[14].I_F,14}, 
	{-189,	-242,  -91,  el[18].a,  el[18].I_F,18},
	{-679,	669,  -48,   el[9].a,  el[7].I_F,9},
	{-148,  507,  685, el[5].a,  el[5].I_F,5},	
	{-148,	507,  685,  el[5].a,  el[5].I_F,5},
	{-158,  507,  676,  el[5].a,  el[5].I_F,5},	
	{-147,	506,  687,  el[5].a,  el[5].I_F,5},
	{-632,	 438, -609,  el[18].a,  el[18].I_F,18},
	{  51,	391,  -36,  el[7].a,  el[7].I_F,7},
	{-123,	-206,  219, el[7].a,  el[7].I_F,7},
	{ 276,	-57,  149,  el[18].a,  el[18].I_F,18},
	{ 102,	  52,  -38, el[16].a,  el[16].I_F,16},
	{   0,	  0,	0,  el[22].a,  el[22].I_F,22},
	{-155,	 171,	37,  el[18].a,  el[18].I_F,18},
	{-265,	120, -358,  el[18].a,  el[18].I_F,22},
};

UINT CigDauThread(LPVOID pParam)
{
	U08 messages[26];	
    int i;	    
	memset(messages, 0, 26); 
	messages[0]  =(U08)0xa0;
	messages[1]  =(U08)0xa1;
	messages[2]  =0;
	messages[3]  =19;
	messages[4]  =0x29; //msgid
	messages[5]  = EllipseList>>8 &0xff;
	messages[6]  = EllipseList    &0xff;
	messages[7]  = EllipseIndex;
	messages[8]  = DeltaX>>8      &0xff;
	messages[9]  = DeltaX         &0xff;
	messages[10]  = DeltaY>>8      &0xff;
	messages[11] = DeltaY         &0xff;
	messages[12] = DeltaZ>>8      &0xff;
	messages[13] = DeltaZ         &0xff;
	messages[14] = Semi_Major_Axis>>24      &0xff;
	messages[15] = Semi_Major_Axis>>16         &0xff;
	messages[16] = Semi_Major_Axis>>8      &0xff;
	messages[17] = Semi_Major_Axis         &0xff;
	messages[18] = Inversed_Flattening>>24      &0xff;
	messages[19] = Inversed_Flattening>>16         &0xff;
	messages[20] = Inversed_Flattening>>8      &0xff;
	messages[21] = Inversed_Flattening         &0xff;
	messages[22] = attribute;

	U08 checksum = 0;
	for(i=0;i< messages[3];i++) checksum^=messages[i+4];
	messages[23]=checksum; //checksum right	    
	messages[24]=(U08)0x0d;
	messages[25]=(U08)0x0a;	
	CGPSDlg::gpsDlg->ExecuteConfigureCommand(messages, 26, "Configure Datum successfully");
	return 0;
}

IMPLEMENT_DYNAMIC(CConDauDlg, CDialog)
CConDauDlg::CConDauDlg(CWnd* pParent)
	: CDialog(CConDauDlg::IDD, pParent)
{
}

CConDauDlg::~CConDauDlg()
{
}

void CConDauDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO1, m_ellipsoidlist);	
	DDX_Control(pDX, IDC_COMBO2, m_attribute);

	DDX_Control(pDX,IDC_DATUM_TEST,m_test);
}


BEGIN_MESSAGE_MAP(CConDauDlg, CDialog)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_DATUM_TEST, OnBnClickedDatumTest)
END_MESSAGE_MAP()


// CConDauDlg 

BOOL CConDauDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	pDauDlg=this;
	m_ellipsoidlist.SetCurSel(0);
	m_attribute.SetCurSel(0);

	m_test.ShowWindow((IS_DEBUG)?SW_SHOW : SW_HIDE);

	m_ellipsoidlist.ResetContent();
	//int s = sizeof(DatumList) / sizeof(DatumList[0]);
	for(int i=0; i<219; ++i)
	{
		if(DatumList[i][0]==NULL)
			break;
		m_ellipsoidlist.AddString(DatumList[i]);
	}
	m_ellipsoidlist.SetCurSel(0);
	return TRUE;  // return TRUE unless you set the focus to a control
}

void CConDauDlg::OnBnClickedOk()
{
	EllipseList         = m_ellipsoidlist.GetCurSel();	
	DeltaX              = datum[EllipseList].DeltaX;
	DeltaY              = datum[EllipseList].DeltaY;
  DeltaZ              = datum[EllipseList].DeltaZ;
	Semi_Major_Axis     = datum[EllipseList].Semi_Major_Axis;
	EllipseIndex        = datum[EllipseList].EllipsoidIndex;
	Inversed_Flattening = datum[EllipseList].Inversd_Flattening;
	attribute           = m_attribute.GetCurSel();
	AfxBeginThread(CigDauThread,0);
  //  _cprintf("%d %d %d",DeltaX,DeltaY,DeltaZ);
	OnOK();
}

static int datum_count;

UINT CigDauThread_Continue(LPVOID pParam)
{
	U08 messages[26];	
	int i, j;	    

	for(j=0;j<datum_count;j++)
	{
		DeltaX              = datum[j].DeltaX;
		DeltaY              = datum[j].DeltaY;
		DeltaZ              = datum[j].DeltaZ;
		Semi_Major_Axis     = datum[j].Semi_Major_Axis;
		EllipseIndex        = datum[j].EllipsoidIndex;
		Inversed_Flattening = datum[j].Inversd_Flattening;
		attribute = 0;

		memset(messages, 0, 26); 
		messages[0] = (U08)0xa0;
		messages[1] = (U08)0xa1;
		messages[2] = 0;
		messages[3] = 19;
		messages[4] = 0x29; //msgid
		messages[5] = EllipseList >>8 &0xff;
		messages[6] = EllipseList    &0xff;
		messages[7] = EllipseIndex;
		messages[8] = DeltaX>>8      &0xff;
		messages[9] = DeltaX         &0xff;
		messages[10] = DeltaY>>8      &0xff;
		messages[11] = DeltaY         &0xff;
		messages[12] = DeltaZ>>8      &0xff;
		messages[13] = DeltaZ         &0xff;
		messages[14] = Semi_Major_Axis>>24      &0xff;
		messages[15] = Semi_Major_Axis>>16         &0xff;
		messages[16] = Semi_Major_Axis>>8      &0xff;
		messages[17] = Semi_Major_Axis         &0xff;
		messages[18] = Inversed_Flattening>>24      &0xff;
		messages[19] = Inversed_Flattening>>16         &0xff;
		messages[20] = Inversed_Flattening>>8      &0xff;
		messages[21] = Inversed_Flattening         &0xff;
		messages[22] = attribute;

		U08 checksum = 0;
		for(i=0;i< messages[3];i++) checksum^=messages[i+4];
		messages[23]=checksum; //checksum right	    
		messages[24]=(U08)0x0d;
		messages[25]=(U08)0x0a;	
		CGPSDlg::gpsDlg->ClearQue();
		CGPSDlg::gpsDlg->SendToTarget(messages, 26, "Configure Datum successfully", true);	
	}

	CGPSDlg::gpsDlg->SetMode(); 
	CGPSDlg::gpsDlg->CreateGPSThread();
	return 0;
}


void CConDauDlg::OnBnClickedDatumTest()
{
	datum_count =m_ellipsoidlist.GetCount();
	::AfxBeginThread(CigDauThread_Continue, 0);

	OnOK();

	
}

*/