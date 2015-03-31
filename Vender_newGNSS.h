#pragma once

#define SW_FUN_DATALOG			0x0001
#define SW_FUN_AGPS				0x0002
#define SW_FUN_DR				0x0004

// .17 Change Beidou Ephemeris format. Modify V8 AGPS Command.
// .18 Fix altitude parsing error when its value is negative.
// .18 Add warning message when download image is not exist.
// .18 [Set Ephemeris] will use the previous saved file automatically.
// .18 Earth View can hide satellite in the specified system.
// .19 Fix rmc speed display error.
// .19 Add setting dialog for adjust delay before binsize.
// .20 Add new download command, fix rmc speed display, set no nmea then download will no nmea output.
// .21 Mutex between [Configure Position Update Rate] and [Configure SAEE].
// .22 Show 12 Channel GPGSV info in theirs GNSS system.
// .23 Fix NMEA Parser error when it has two GNGSA statements.
// .24 Fix NMEA Parser error when it has no GPGGA statements (Unicore pure BD mode).
// .25 Fix [Query Power Saving Parameter] timeout issue.
// .26 Add tag loader process. 
// .26 Hide com port selection in [Configure Serial Port] dialog when using Customer Release version.
// .27 Add [Query NMEA Talker ID] in menu [Binary]. Modify VTG interval default to 1 second.
// .28 Support new loader srec BINSIZE / BINSIZ2 / BINSIZ3 command.
// .28 Fix lon, lat, alt dispear issue after reconnection.
// .29 Remove Query / Configure 1PPS NMEA Delay, and remove AGPS item in general version.
// .30 Add Query / Configure Power Saving Parameter(Rom), and modified Query / Configure Power Saving Parameter in Binary menu.
// .31 NMEA Player added PreprocessInputLine function for normalizing the nmea string.
// .32 Fixed Beidou nmea status bugs, the gsa information is not clear after reconnection.
// .33 Change some strings and menu items based on meeting conclusions. 
// .34 Change some strings and menu items based on meeting conclusions. 
// .35 Fixed Log Decompress and KML Conver open file dialog style. 
// .36 Added GNSS Doze Mode command in Venus 8.
// .37 Added SWCFG_VENDOR_GNSS_GENERAL_OLINKSTAR_PROTECT.
// .38 Modify CheckOlinkstarFirmware() rule.
// .39 Modify "Skytraq" string for customer.
// .39 Remove 1PPS clock pluse and modify Set GLONASS Time Correction.
// .40 Modify AGPS function in General version.
// .41 Add support add tag version for OLinkStart version
// .42 Remove "Skytraq" in About dialog.
// .43 Using "OLINKSTAR IMAGE" for external loader download.
// .44 Fixed CSerial::GetString in Prolific can't download issue.
// .45 Add [Configure / Query 1PPS Frequency Output].
// .46~.47 Add support add tag version for SWID version.
// .48 Add SPI download option and modify GetString time interval.
// .49 Add setting [Boost Get/Set Ephemeris] for disable baud rate change in Get / Set Ephemeris.
// .50 Modify [Query/Configure SBAS] function, using combo box for Ranging options.
// .51 Fix [Query Cable Delay] in wrong unit issue.
// .52 Add SWCFG_VENDOR_GNSS_GENERAL_FKHL, and new NMEA_PRN_TYPE define.
// .53 Fix Query Navigation Mode issue.
// .54 Fix ShowGPGSVmsg2 parsing bugs.
// .55 Add Ericsson customer branch.
// .56 Fix first nmea no output issue.
// .57 Add TIMING_MONITORING,TIMING_OUTPUT_ALIGN, SHOW_ELEV_AND_CNR_MASK_IN_GEN for customize version.
// .58 Clean NMEA data after output mode change.
// .59 Make Scatter View larger and show last point in blue.
// .60 Add [Query / Configure Binary Measurement Data Out] in Binary menu.
// .61 Fix scatter view delay issue. Modify Beidou Prn range for Ericsson Version.
// .62 Change SHOW_ELEV_AND_CNR_MASK_IN_GEN default to 1.
// .63 Add NMEA error detection function in respond window.
// .64 Fix error NMEA detect function issue.
// .65 Support multi hz datalog.
// .66 Query Customer ID display in hex format.
// .67 Add start address in DataLog decompress.
// .68 Fix start address issue in DataLog decompress.
// .69 Fix leap seconds issue and second issue in DataLog decompress.
// .70 Add Enu scale 0.1m, 0.2m, 0.5m. Defined by MORE_ENU_SCALE.
// .71 Add "Send" button in [NMEA Checksum Calc] and [Binary Checksum Calc] dialog.
// .72 Add Serial Number Command in OLinkStar version.
// .73 Scatter scale will not change after reconnected.
// .74 Add 2 seconds delay after plug-in detected.
// .75 Adjust removing device message show time to 3500 ms.
// .76 Change [Configure Binary Measurement Data Out], [Query Binary Measurement Data Out]
// .76 Add [Query DGPS], [Configure DGPS].
// .76 Add [Query Carrier Smooth Mode], [Configure Carrier Smooth Mode].
// .76 Add [Query Time Stamping], [Configure Time Stamping].
// .76 Improve time out response. Update date no check.
// .77 Change [Configure DGPS], [Configure Carrier Smooth Mode], [Configure Time Stamping].
// .78 Add [Query GPS Time] and [Configure GPS/UTC Leap Seconds] in Venus 8 menu.
// .79 Fix QueryTimeStamping tigger mode error.
// .80 Fix QueryTimeStamping valid error.
// .81 Fix QueryTimeStamping Sub Time of Week error.
// .82 Save NMEA support raw measurement binary output decode.
// .83 Block save NMEA for multi hz.
// .84 Add Surveying and mapping in Navigation Mode.
// .85 Add time sync in NMEA Player.
// .86 Support [Enable All] in SBAS Command.
// .86 Add Test External SREC in utility.
// .87 Add Parameter Search Engine Sleep Critria command.
// .88 Modify Parameter Search Engine Sleep Critria command.
// .89 Modify Configure Timming command packet size.
// .90 Setting add [Download Testing] and [Log Response].
// .91 Support Gaileo NMEA.
// .92 Add binary message error detection.
// .93 Add [Configure Binary Measurement Data Out] in Customer Release Version.
// .94 Add Query/Configure Datum Index in Venus 8 menu.
// .95 Add [Save Binary] in File menu, it can record any output from device.
// .96 Fix [Save Binary] issue.
// .97 Version and Boot Status in Information will be updated after user query them in menu.
// .98 Fix garbage codes shown after NMEA sentences issue.
// .99 Fix [Configure DGPS] packet size error issue.
// .100 Fix NMEA output un-smooth behavior when configue the position update rate higher than 20Hz.
// .101 Add version extension string after kernel version, add [Query SHA1 String] in [Binary].
// .102 Add function [Query/Configure Search Engine Number] to customer release version.
// .103 Add SUP800 Tester configure, and modify 1pps Frequency Output default value.
// .104 Modify PSTI 004_001. parser spec, add valide field.
// .105 Adjust Frequency default to 10M in Configure 1PPS Frequency Output.
// .106 Save NMEA can support all binary output message description.
// .107 Add SUP800 Customer Release branch.
// .108 KML Convert support both GGA and RMC, and add progress bar when converting.
// .109 Fix Speed and Direction maybe show 0.0 and never update after re-connection.
// .110 Add Query GNSS Constellation Capability function.
// .111 Fix KML Converter bug, corrected segmentation conditions.
// .112 Fix CUSTOMER_ID define error.
// .113 Switch Earth bitmap by coordinates.
// .114 Change KR Ver to SW Ver
// .115 Add [Query SHA1 String] to General version.
// .116 Add [Raw Measurement Binary Convert] in [Convert] menu.
// .117 Fix Power Mode in [Configure Power Mode] can't change issue.
// .118 Fix Fixed Raw Measurement output parsing issue.
// .119 Add specify Scatter Center Point function in Setup.
// .120 Add SWCFG_VENDOR_GNSS_SWID_NEW_DOWNLOAD.
// .121 Add Set / Query UART Pass-through in Venus8 Menu.
// .122 Add GPSDO sub-menu in Venus8 Menu.
// .123 Add GPSDO sub-menu in Venus8 Menu.
// .124 Modify GPSDO sub-menu in Venus8 Menu.
// .125 Modify KML converter output default folder to same of source.
// .125 Fixed binary output satellite prn duplicate issue.
// .126 Add Signal Disturbance Test menu in [Venus 8] menu.
// .127 Modify Signal Disturbance Test command.
// .128 Fixed NMEA Player crash in XP when scatter starting draw point.
// .129 Support special NMEA(RMC and GGA), add [Query / Configure Interference Detect Control] to General.
// .130 Support [Get / Set Beidou almanac].
// .131 Never change baud rate when configure serial port fail.
// .132 Fixed DataLog Decompress crash bug when date number < 10.
// .133 Add [Configure GPS/UTC Leap Seconds in UTC] and download in external loader when boot in ROM code.
// .134 Extend timeout when query version before download.
// .135 Disable check SAEE and Multi-Hz can't be both on.
// .136 Add [Configure / Query Position Fix Navgation Mask] to Customer Release version.
// .137 Fix [Query CRC] fail issue and [Configure Position Update Rate] NACK issue.
// .138 Add [GPSDO Download] panel.
// .139 Fix DataLog read fail issue

//#define SOFTWARE_FUNCTION		(SW_FUN_DATALOG | SW_FUN_AGPS | SW_FUN_DR)
#define SOFTWARE_FUNCTION		(SW_FUN_DATALOG | SW_FUN_AGPS)
#define IS_DEBUG				0
#define APP_CAPTION				"GNSS Viewer"
#define APP_VERSION				"2.0.139"
#define APP_TITLE				""
#define GNSS_VIEWER				0
#define GPS_VIEWER				0

//enum CustomerId	{
#define	Sktyraq					0x0000
#define SWID					0x0001		//重慶西南集成電路設計
#define Ericsson				0x0004
#define OlinkStar				0x4F4C
//

//application init default
#define BAUDRATE				5		//5=115200,4=57600,3=38400,2=19200,1=9600,0=4800
//change baudrate default
#define BAUDRATE_DEFAULT		5		//5=115200,4=57600,3=38400,2=19200,1=9600,0=4800
#define FIRMWARE_DOWNLOAD			1
//#define CHECK_COM_REGISTER		0
//#define LIMIT_SCATTER_PLOT		1
#define MAX_SCATTER_COUNT			100

//#define NOFIXED_CHECK				0
//#define DRAW_COURSE					0

//#define DATALOG_TRIP				0
#define ODOMETER_SUPPORT			0
#define BINARY_MESSAGE_SUPPORT		0
#define BINARY_MESSAGE_INTERVAL		0
//#define BINARY_MESSAGE_NI			1

#define ACTIVATE_MINIHOMER		0
#define NMEA_INPUT				0

//#define RESTORE_CLK_OFFSET	0

#define GSA_MAX_SATELLITE		12
//#define	GNSS_VIEWER			1
//PRE_LOADER defined the prom.bin is merged with smaller loader
//#define PRE_LOADER			1
//Download using resend protocol
#define RESEND_DOWNLOAD			1
//Display binary command in and ack in respond view.
#define _SHOW_BINARY_DATA_		1

//Using external SREC file directly, no prompt.
#define _ALWAYS_USE_EXTERNAL_SREC_	0
//Does not use pre-loader in download firmware.
#define _DIRECTLY_DOWNLOAD_		0
//Add a tag for DR Firmware.
#define _CREATE_LICENSE_TAG_	0
//for [Reset Motion Sensor] Command
#define RESET_MOTION_SENSOR		0
#define ECOMPASS_CALIBRATION	0
#define TIMING_MODE				0

//#define _V8_DOWNLOAD_			0
#define _BAUTRATE_IDX_6_		891200
#define PACIFIC					0
//#define SJA						0
#define OPEN_PINNING_RESERVE	0
#define RESET_MOTION_SENSOR		0
//#define AVID					0
//#define NI						0
//#define SOARCOMM				0
//#define LG						0
//#define IQIBLA					0
#define _V8_SUPPORT				0
#define GG12A					0
//#define SUPPORT_CLEAR_LOGIN_PASSWORD	0
//#define WITH_CONFIG_USB_BAUDRATE	0
#define SHOW_CLOCK_OFFSET		0
#define SHOW_NOISE				0
#define DATA_POI				0
#define TWIN_DATALOG			0
//#define _UNIT_TESTING_			1
//#define _UNICORE_NMEA_			1
#define CUSTOMER_DOWNLOAD		0
#define CUSTOMER_ID				Sktyraq
#define NMEA_PRN_TYPE			0
#define TIMING_MONITORING	1
#define TIMING_OUTPUT_ALIGN	1
#define	SHOW_ELEV_AND_CNR_MASK_IN_GEN	1
#define	SHOW_ERROR_NMEA_NOTIFY	0
#define	MORE_ENU_SCALE			1
#define	GPS_183_188				1
#define	_MODULE_SUP_800_		0
#define	_USE_RESOURCE_LOADER_	0
#define	_RESOURCE_LOADER_ID_	0
#define CHECK_SAEE_MULTIHZ_ON	0		//Check SAEE and Multi-Hz can't both on.
////////////////////////////////////////////////////////////////////////////////////////////

#if defined(SWCFG_VENDOR_GNSS_SWID_NEW_DOWNLOAD)
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef OPEN_PINNING_RESERVE
 #undef BINARY_MESSAGE_INTERVAL
 #undef _USE_RESOURCE_LOADER_
 #undef _DIRECTLY_DOWNLOAD_
 #undef _V8_SUPPORT
 #undef _RESOURCE_LOADER_ID_

 #define APP_CAPTION			"GNSS Viewer"
 #define APP_TITLE				"Customer Release"
 #define GNSS_VIEWER			1
 #define IS_DEBUG				0
 #define BAUDRATE				5
 #define BAUDRATE_DEFAULT		7
 #define TIMING_MODE			1
 #define OPEN_PINNING_RESERVE	1
 #define BINARY_MESSAGE_INTERVAL	1
 #define _USE_RESOURCE_LOADER_	1
 #define _DIRECTLY_DOWNLOAD_	1
 #define _V8_SUPPORT			1
 #define _RESOURCE_LOADER_ID_	1351

#elif defined(SWCFG_VENDOR_GNSS_SUP800_GENERAL)
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef OPEN_PINNING_RESERVE
 #undef BINARY_MESSAGE_INTERVAL
// #undef _ALWAYS_USE_EXTERNAL_SREC_
 #undef _DIRECTLY_DOWNLOAD_
 #undef _V8_SUPPORT
 #undef SHOW_ERROR_NMEA_NOTIFY
 #undef _MODULE_SUP_800_

 #define APP_CAPTION			"GNSS Viewer"
 #define APP_TITLE				"SUP800 Customer Release"
 #define GNSS_VIEWER			1
 #define IS_DEBUG				0
 #define BAUDRATE				5
 #define BAUDRATE_DEFAULT		7
 #define TIMING_MODE			1
 #define OPEN_PINNING_RESERVE	1
 #define BINARY_MESSAGE_INTERVAL	1
 #define _DIRECTLY_DOWNLOAD_	1
 #define _V8_SUPPORT			1
 #define SHOW_ERROR_NMEA_NOTIFY 1
 #define _MODULE_SUP_800_		1

#elif defined(SWCFG_VENDOR_GNSS_SUP800)
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef OPEN_PINNING_RESERVE
 #undef BINARY_MESSAGE_INTERVAL
// #undef _ALWAYS_USE_EXTERNAL_SREC_
 #undef _DIRECTLY_DOWNLOAD_
 #undef _V8_SUPPORT
 #undef SHOW_ERROR_NMEA_NOTIFY
 #undef _MODULE_SUP_800_

 #define APP_CAPTION			"GNSS Viewer"
 #define APP_TITLE				"SUP800 Tester"
 #define GNSS_VIEWER			1
 #define IS_DEBUG				1
 #define BAUDRATE				5
 #define BAUDRATE_DEFAULT		7
 #define TIMING_MODE			1
 #define OPEN_PINNING_RESERVE	1
 #define BINARY_MESSAGE_INTERVAL	1
 //#define _ALWAYS_USE_EXTERNAL_SREC_	1
 #define _DIRECTLY_DOWNLOAD_	1
 #define _V8_SUPPORT			1
 #define SHOW_ERROR_NMEA_NOTIFY 1
 #define _MODULE_SUP_800_		1

#elif defined(SWCFG_VENDOR_GNSS_GENERAL_OLINKSTAR_PROTECT)
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef OPEN_PINNING_RESERVE
 #undef BINARY_MESSAGE_INTERVAL
 #undef _DIRECTLY_DOWNLOAD_
 #undef _V8_SUPPORT
 #undef CUSTOMER_DOWNLOAD
 #undef DefaultCuteomer

 #define APP_CAPTION			"GNSS Viewer"
 #define APP_TITLE				"OLinkStar Release"
 #define GNSS_VIEWER			1
 #define IS_DEBUG				0
 #define BAUDRATE				5
 #define BAUDRATE_DEFAULT		5
 #define TIMING_MODE			1
 #define OPEN_PINNING_RESERVE	1
 #define BINARY_MESSAGE_INTERVAL	1
 #define _DIRECTLY_DOWNLOAD_	1
 #define _V8_SUPPORT			1
 #define CUSTOMER_DOWNLOAD		1
 #define DefaultCuteomer		OlinkStar

#elif defined(SWCFG_VENDOR_GNSS_GENERAL_ERICSSON)
//ERICSSON from 重慶西南集成電路設計
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef OPEN_PINNING_RESERVE
 #undef BINARY_MESSAGE_INTERVAL
 #undef _DIRECTLY_DOWNLOAD_
 #undef _V8_SUPPORT
 #undef CUSTOMER_DOWNLOAD
 #undef DefaultCuteomer
 #undef TIMING_MONITORING
 #undef TIMING_OUTPUT_ALIGN
 #undef SHOW_ELEV_AND_CNR_MASK_IN_GEN
 #undef NMEA_PRN_TYPE

 #define APP_CAPTION			"GNSS Viewer"
 #define APP_TITLE				"Ericsson Release"
 #define GNSS_VIEWER			1
 #define IS_DEBUG				0
 #define BAUDRATE				5
 #define BAUDRATE_DEFAULT		5
 #define TIMING_MODE			1
 #define OPEN_PINNING_RESERVE	1
 #define BINARY_MESSAGE_INTERVAL	1
 #define _DIRECTLY_DOWNLOAD_	1
 #define _V8_SUPPORT			1
 #define CUSTOMER_DOWNLOAD		0
 #define DefaultCuteomer		Ericsson
 #define TIMING_MONITORING		0
 #define TIMING_OUTPUT_ALIGN	0
 #define SHOW_ELEV_AND_CNR_MASK_IN_GEN	1
 #define NMEA_PRN_TYPE			2

#elif defined(SWCFG_VENDOR_GNSS_GENERAL_RDRNSS_PROTECT)
//SWID 重慶西南集成電路設計
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef OPEN_PINNING_RESERVE
 #undef BINARY_MESSAGE_INTERVAL
 #undef _DIRECTLY_DOWNLOAD_
 #undef _V8_SUPPORT
 #undef CUSTOMER_DOWNLOAD
 #undef DefaultCuteomer
 #undef TIMING_MONITORING
 #undef TIMING_OUTPUT_ALIGN
 #undef SHOW_ELEV_AND_CNR_MASK_IN_GEN

 #define APP_CAPTION			"GNSS Viewer"
 #define APP_TITLE				"RDRNSS Release"
 #define GNSS_VIEWER			1
 #define IS_DEBUG				0
 #define BAUDRATE				5
 #define BAUDRATE_DEFAULT		5
 #define TIMING_MODE			1
 #define OPEN_PINNING_RESERVE	1
 #define BINARY_MESSAGE_INTERVAL	1
 #define _DIRECTLY_DOWNLOAD_	1
 #define _V8_SUPPORT			1
 #define CUSTOMER_DOWNLOAD		0
 #define DefaultCuteomer		SWID
 #define TIMING_MONITORING	0
 #define TIMING_OUTPUT_ALIGN	0
 #define SHOW_ELEV_AND_CNR_MASK_IN_GEN 1

#elif defined(SWCFG_VENDOR_GNSS_GENERAL_SWID_PROTECT)
//SWID 重慶西南集成電路設計
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef OPEN_PINNING_RESERVE
 #undef BINARY_MESSAGE_INTERVAL
 #undef _DIRECTLY_DOWNLOAD_
 #undef _V8_SUPPORT
 #undef CUSTOMER_DOWNLOAD
 #undef DefaultCuteomer
 #undef TIMING_MONITORING
 #undef TIMING_OUTPUT_ALIGN
 #undef SHOW_ELEV_AND_CNR_MASK_IN_GEN

 #define APP_CAPTION			"GNSS Viewer"
 #define APP_TITLE				"SWID Release"
 #define GNSS_VIEWER			1
 #define IS_DEBUG				0
 #define BAUDRATE				5
 #define BAUDRATE_DEFAULT		5
 #define TIMING_MODE			1
 #define OPEN_PINNING_RESERVE	1
 #define BINARY_MESSAGE_INTERVAL	1
 #define _DIRECTLY_DOWNLOAD_	1
 #define _V8_SUPPORT			1
 #define CUSTOMER_DOWNLOAD		0
 #define DefaultCuteomer			SWID
 #define TIMING_MONITORING	0
 #define TIMING_OUTPUT_ALIGN	0
 #define SHOW_ELEV_AND_CNR_MASK_IN_GEN 1

#elif defined(SWCFG_VENDOR_GPS_INTERNALUSE)
 #undef APP_CAPTION	
 #undef APP_TITLE	
 #undef GPS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef OPEN_PINNING_RESERVE
 #undef BINARY_MESSAGE_INTERVAL

 #define APP_CAPTION			"GPS Viewer"
 #define APP_TITLE				"Internal Use"
 #define GPS_VIEWER				1
 #define IS_DEBUG				1
 #define BAUDRATE				5
 #define BAUDRATE_DEFAULT		5
 #define TIMING_MODE			1
 #define OPEN_PINNING_RESERVE	1
 #define BINARY_MESSAGE_INTERVAL	1

#elif defined(SWCFG_VENDOR_GPS_DR_INTERNALUSE)
 #undef APP_CAPTION	
 #undef APP_TITLE	
 #undef GPS_VIEWER
 #undef SOFTWARE_FUNCTION
 #undef IS_DEBUG
 #undef BAUDRATE
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef OPEN_PINNING_RESERVE
 #undef BINARY_MESSAGE_INTERVAL

 #define APP_CAPTION			"GPS Viewer"
 #define APP_TITLE				"DR Internal Use"
 #define GPS_VIEWER				1
 #define SOFTWARE_FUNCTION		(SW_FUN_DATALOG | SW_FUN_AGPS | SW_FUN_DR)

 #define IS_DEBUG				1
 #define BAUDRATE				5
 #define BAUDRATE_DEFAULT		5
 #define TIMING_MODE			1
 #define OPEN_PINNING_RESERVE	1
 #define BINARY_MESSAGE_INTERVAL	1

#elif defined(SWCFG_VENDOR_GPS_V8)
 #undef APP_CAPTION	
 #undef APP_TITLE	
 #undef GPS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE
 #undef BAUDRATE_DEFAULT
 #undef OPEN_PINNING_RESERVE
 #undef _ALWAYS_USE_EXTERNAL_SREC_
 #undef _DIRECTLY_DOWNLOAD_
 #undef TIMING_MODE
 //#undef _V8_DOWNLOAD_
 //#undef _BAUTRATE_IDX_6_
 #undef _V8_SUPPORT
 #undef BINARY_MESSAGE_INTERVAL

 #define APP_CAPTION			"GPS Viewer"
 #define APP_TITLE				"Internal Use"
 #define GPS_VIEWER				1
 #define IS_DEBUG				1
 #define BAUDRATE				5
 #define BAUDRATE_DEFAULT		5
 #define OPEN_PINNING_RESERVE	1
 #define _ALWAYS_USE_EXTERNAL_SREC_	1
 #define _DIRECTLY_DOWNLOAD_	1
 #define TIMING_MODE			1
 //#define _V8_DOWNLOAD_			1
 //#define _BAUTRATE_IDX_6_		460800
 #define _V8_SUPPORT				1
 #define BINARY_MESSAGE_INTERVAL	1

#elif defined(SWCFG_VENDOR_GNSS_INTERNALUSE_BDV8)
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef OPEN_PINNING_RESERVE
 #undef BINARY_MESSAGE_INTERVAL
// #undef _ALWAYS_USE_EXTERNAL_SREC_
 #undef _DIRECTLY_DOWNLOAD_
 #undef _V8_SUPPORT
 #undef SHOW_ERROR_NMEA_NOTIFY

 #define APP_CAPTION			"GNSS Viewer"
 #define APP_TITLE				"Internal Use"
 #define GNSS_VIEWER			1
 #define IS_DEBUG				1
 #define BAUDRATE				5
 #define BAUDRATE_DEFAULT		7
 #define TIMING_MODE			1
 #define OPEN_PINNING_RESERVE	1
 #define BINARY_MESSAGE_INTERVAL	1
 //#define _ALWAYS_USE_EXTERNAL_SREC_	1
 #define _DIRECTLY_DOWNLOAD_	1
 #define _V8_SUPPORT			1
 #define SHOW_ERROR_NMEA_NOTIFY 1

#elif defined(SWCFG_VENDOR_GNSS_INTERNALUSE_GPS_183)
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef OPEN_PINNING_RESERVE
 #undef BINARY_MESSAGE_INTERVAL
// #undef _ALWAYS_USE_EXTERNAL_SREC_
 #undef _DIRECTLY_DOWNLOAD_
 #undef _V8_SUPPORT
 #undef SHOW_ERROR_NMEA_NOTIFY
 #undef NMEA_PRN_TYPE

 #define APP_CAPTION			"GNSS Viewer"
 #define APP_TITLE				"Internal Use GPS 183-188"

 #define GNSS_VIEWER			1
 #define IS_DEBUG				1
 #define BAUDRATE				5
 #define BAUDRATE_DEFAULT		7
 #define TIMING_MODE			1
 #define OPEN_PINNING_RESERVE	1
 #define BINARY_MESSAGE_INTERVAL	1
 //#define _ALWAYS_USE_EXTERNAL_SREC_	1
 #define _DIRECTLY_DOWNLOAD_	1
 #define _V8_SUPPORT			1
 #define SHOW_ERROR_NMEA_NOTIFY 1
 #define NMEA_PRN_TYPE			3

#elif defined(SWCFG_VENDOR_GNSS_BDV8)
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef OPEN_PINNING_RESERVE
 #undef BINARY_MESSAGE_INTERVAL
// #undef _ALWAYS_USE_EXTERNAL_SREC_
 #undef _DIRECTLY_DOWNLOAD_
 #undef _V8_SUPPORT

 #define APP_CAPTION			"GNSS Viewer"
 #define APP_TITLE				"Customer Release"
 #define GNSS_VIEWER			1
 #define IS_DEBUG				0
 #define BAUDRATE				5
 #define BAUDRATE_DEFAULT		7
 #define TIMING_MODE			1
 #define OPEN_PINNING_RESERVE	1
 #define BINARY_MESSAGE_INTERVAL	1
 //#define _ALWAYS_USE_EXTERNAL_SREC_	1
 #define _DIRECTLY_DOWNLOAD_	1
 #define _V8_SUPPORT			1

#elif defined(SWCFG_VENDOR_GNSS_FKHL)
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef OPEN_PINNING_RESERVE
 #undef BINARY_MESSAGE_INTERVAL
// #undef _ALWAYS_USE_EXTERNAL_SREC_
 #undef _DIRECTLY_DOWNLOAD_
 #undef _V8_SUPPORT
 #undef NMEA_PRN_TYPE

 #define APP_CAPTION			"GNSS Viewer"
 #define APP_TITLE				"Customer Release FKHL"
 #define GNSS_VIEWER			1
 #define IS_DEBUG				0
 #define BAUDRATE				5
 #define BAUDRATE_DEFAULT		7
 #define TIMING_MODE			1
 #define OPEN_PINNING_RESERVE	1
 #define BINARY_MESSAGE_INTERVAL	1
 //#define _ALWAYS_USE_EXTERNAL_SREC_	1
 #define _DIRECTLY_DOWNLOAD_	1
 #define _V8_SUPPORT			1
 #define NMEA_PRN_TYPE			1

#elif defined(SWCFG_VENDOR_GNSS_FKHL_Test)
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef OPEN_PINNING_RESERVE
 #undef BINARY_MESSAGE_INTERVAL
// #undef _ALWAYS_USE_EXTERNAL_SREC_
 #undef _DIRECTLY_DOWNLOAD_
 #undef _V8_SUPPORT
 #undef NMEA_PRN_TYPE
 #undef NMEA_INPUT

 #define APP_CAPTION			"GNSS Viewer"
 #define APP_TITLE				"Customer Release FKHL"
 #define GNSS_VIEWER			1
 #define IS_DEBUG				0
 #define BAUDRATE				5
 #define BAUDRATE_DEFAULT		7
 #define TIMING_MODE			1
 #define OPEN_PINNING_RESERVE	1
 #define BINARY_MESSAGE_INTERVAL	1
 //#define _ALWAYS_USE_EXTERNAL_SREC_	1
 #define _DIRECTLY_DOWNLOAD_	1
 #define _V8_SUPPORT			1
 #define NMEA_PRN_TYPE			1
 #define NMEA_INPUT				1

#elif defined(SWCFG_VENDOR_GNSS_BDV8_NMEAPLAYER)
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef OPEN_PINNING_RESERVE
 #undef BINARY_MESSAGE_INTERVAL
 #undef _DIRECTLY_DOWNLOAD_
 #undef _V8_SUPPORT
 #undef NMEA_INPUT
 #undef SHOW_ERROR_NMEA_NOTIFY

 #define APP_CAPTION			"GNSS Viewer"
 #define APP_TITLE				"NMEA Player"
 #define GNSS_VIEWER			1
 #define IS_DEBUG				0
 #define BAUDRATE				5
 #define BAUDRATE_DEFAULT		7
 #define TIMING_MODE			1
 #define OPEN_PINNING_RESERVE	1
 #define BINARY_MESSAGE_INTERVAL	1
 #define _DIRECTLY_DOWNLOAD_	1
 #define _V8_SUPPORT			1
 #define NMEA_INPUT				1
 #define SHOW_ERROR_NMEA_NOTIFY 1

#elif defined(SWCFG_VENDOR_GNSS_V8)
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef OPEN_PINNING_RESERVE
 #undef BINARY_MESSAGE_INTERVAL
// #undef _ALWAYS_USE_EXTERNAL_SREC_
 #undef _DIRECTLY_DOWNLOAD_
 #undef _V8_SUPPORT

 #define APP_CAPTION			"GNSS Viewer"
 #define APP_TITLE				"Internal Use"
 #define GNSS_VIEWER			1
 #define IS_DEBUG				1
 #define BAUDRATE				5
 #define BAUDRATE_DEFAULT		6
 #define TIMING_MODE			1
 #define OPEN_PINNING_RESERVE	1
 #define BINARY_MESSAGE_INTERVAL	1
 //#define _ALWAYS_USE_EXTERNAL_SREC_	1
 #define _DIRECTLY_DOWNLOAD_	1
 #define _V8_SUPPORT			1

#elif defined(SWCFG_VENDOR_CREATETAG)
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef OPEN_PINNING_RESERVE
 #undef BINARY_MESSAGE_INTERVAL
// #undef _ALWAYS_USE_EXTERNAL_SREC_
 #undef _DIRECTLY_DOWNLOAD_
 #undef _V8_SUPPORT
 #undef _CREATE_LICENSE_TAG_

 #define APP_CAPTION			"GNSS Viewer"
 #define APP_TITLE				"Create Tag"
 #define GNSS_VIEWER				1
 #define IS_DEBUG					0
 #define BAUDRATE					5
 #define BAUDRATE_DEFAULT			7
 #define TIMING_MODE				1
 #define OPEN_PINNING_RESERVE		1
 #define BINARY_MESSAGE_INTERVAL	1
 //#define _ALWAYS_USE_EXTERNAL_SREC_	1
 #define _DIRECTLY_DOWNLOAD_		1
 #define _V8_SUPPORT				1
 #define _CREATE_LICENSE_TAG_		1

#elif defined(SWCFG_VENDOR_GENERAL_DR)
 #undef APP_TITLE				
 #undef BAUDRATE
 #undef BAUDRATE_DEFAULT
 #undef SOFTWARE_FUNCTION
 #undef OPEN_PINNING_RESERVE

 #define APP_TITLE				"Customer Release DR"
 #define BAUDRATE				5
 #define BAUDRATE_DEFAULT		5
 #define OPEN_PINNING_RESERVE	1
 #define SOFTWARE_FUNCTION		(SW_FUN_DATALOG | SW_FUN_AGPS | SW_FUN_DR)

#else
 #error Unknow SWCFG_VENDER_TYPE!!!
#endif

#if ((GNSS_VIEWER || GPS_VIEWER)==0)
 #error Unknow GNSS_VIEWER or GPS_VIEWER!!!
#endif
