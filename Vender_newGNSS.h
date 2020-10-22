#pragma once

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
// .94 20140515 Add [Query / Configure Datum Index] in Venus 8 menu. Request from Andrew
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
// .140 Add [Noise Power Control] command in [Venus 8].
// .141 Adjust all configure dialog in [Venus 8]. Fixed download V6 fail issue.
// .142 Fix external download in binary command issue.
// .143 Support 3D KML convert.
// .144 Remove [Query / Configure Power Saving Parameters(Rom)] and modify [Query / Configure Power Saving Parameters]
// .145 [Conveter/KML] support GNS message.
// .146 Modify Antenna Detection command for fw spec.
// .147 Add GAGAN option in [Query / Configure SBAS].
// .148 Fixed GAGAN option error issue.
// .149 20150720 Add iq plot functions.
// .150 20150721 Update IQPlot for Windows XP.
// .151 20150804 Add SWCFG_VENDOR_GNSS_TAKUJI version for Oliver's customer upgrade.
// .152 20150804 Modify SWCFG_VENDOR_GNSS_TAKUJI version add tag.
// .153 20150805 Add Master/Slave function.
// .154 20150807 KML support Point List, and Scatter support 0.05m, 0.01m.
// .155 20150807 KML point list support name tag in UTC time.
// .156 20150817 Setup scatter center support 7 decimal Point.
// .157 20150819 Support Fix RTK and Float RTK quality mode and modify KML converter.
// .158 20150819 Support Fix RTK and Float RTK quality mode and modify KML converter.
// .159 20150824 Modify [Configure Timing] for RTK.
// .160 20150831 Release for Customer Release.
// .161 20150911 KML remove point title.
// .162 20150917 Add Geo-Fencing command.
// .163 20150922 Add [Default Timeout] in setup dialog. Request from Andrew and Justin.
// .164 20150923 Fixed 0x64, 0x03 size error issue. Report by Andrew.
// .165 20150923 Add RTK mode command. Request from Andrew.
// .166 20150925 Fix altitude issue for Scatter.
// .167 20151001 IQ Plot support multi process.
// .168 20151006 Fixed ShowBinaryOutput() issue.
// .169 20151013 Add %012.9lf to KML converter. Request from Ken
// .170 20151016 Remove REBOOT notify for scatter center changed. Request from Oliver
// .171 20151026 Fix WGS84 XYZ not update issue. Report by forum user.
// .172 20151029 Scatter will not change the center point after re-connect or restart, request from Oliver
// .173 20151110 Add [Query/Configure RTK Parameters], [Reset RTK Engine] in [Venus 8 / RTK] menu.
// .173 20151110 Fixed command ack lose issue when its contant 0xa0 0xa1.
// .174 20151120 Modify Lat/Lon display.
// .175 20151126 Merge com and baudrate setting to g_setting structure. 
// .175 20151126 Modify [Configure Position Update Rate] change baudrate rule.
// .176 20151126 Modify Scatter Count default value for slow computer. Request from Andrew
// .177 20151127 KML converter support GSA / GSV. Request from Michael
// .177 20151130 Add RTK field and modify 2DRMS & CEP50% display.
// .177 20151130 Add _MORE_INFO_ define and new profile [GNSS_General_RTK][GNSS_NMEAPLAYER_RTK].
// .178 20151204 Fixed baud rate not restore after download.
// .179 20151208 Fixed KML converter GSA issue.
// .180 20151209 Fixed KML converter Beidou issue.
// .181 20151217 Remove Geo-fecing in General version. request from Oliver.
// .181 20151221 Fixed degree symbol display in English issue, report from fourm user.
// .182 20151224 Add [Query Binary Measurement Data Out] in binary menu. Request form Andrew and Ryan.
// .184 20151230 Modify ALWAYS_USE_EXTERNAL_SREC to 1, using 115200 dl srec. Request form Leo by 錦輪  
// .185 20160105 SAINTMAX customer version QWP002001-1050104. Request form Leo
// .186 20160112 SAINTMAX customer version QWP002001-1050104. Request form Leo
// .187 20160120 Player fixed set origin error(In west earth) and timing control issue, Request form Oliver and Andrew.
// .188 20160123 Modify some typo text, Request from forum user.
// .189 20160126 Send 1 Hz cmd after config to RTK base mode, Request from Oliver.
// .190 20160202 Add SPECIAL_TEST for eten special command test, Request from Leo and Andrew.
// .191 20160203 Add SPECIAL_TEST random test, Request from Leo and Andrew.
// .192 20160225 Modify GeoFencing to support multiple Geo-Fencing polgons. Request from Oliver and Andrew.
// .193 20160226 Brench Kalman version, add RTK baseline display. Request from Leo.
// .194 20160301 Show baseline to three decimal. Request from Leo.
// .195 20160310 Support RTK Mode and operational function. Request from Andrew and Ryan Yang.
// .196 20160317 Reconstruction Configure Message Type functions and modify QueryRtkMode2. Request from Ryan Yang and Ken.
// .197 20160323 Modify RTK static mode altitude prompt text. Request from Ryan Yang.
// .197 20160324 Fixed scan port / baudrate bugs.
// .197 20160330 ETEN customize version, add PSTM menu, Request from Leo.
// .198 20160331 Add Utility function [Read 0x50000000 to File], Request from Andrew.
// .199 20160331 KML Converter support DR Mode, Request from Ken.
// .199 20160411 Add binary output player in Player version, Request from Leo.
// .200 20160412 Fix download issue when raw mode download will fail, Report from Andrew and Ken.
// .200 20160413 Fix crash issue when UI fast operation, Report from Andrew and Ryan.
// .201 20160414 Read prom bin fast when download, Report from Ken.
// .202 20160419 Support DJI NAVIGATION DATA MESSAGE(ID: 0x7A, CUSTOMER ID: 0xB, SID: 0x80), Report from Andrew, Roger.
// .203 20160422 Modify RTK Mode and operational function, add Baseline length, requast from Andrew, Ryan.
// .203 20160422 Remove GPS Measurement Mode command, requast from Andrew, Oliver.
// .204 20160428 Add FormatError ACK support, requast from Andrew.
// .205 20160429 Fix Ack error issue, requast from Ken.
// .206 20160509 Add command [Pinning Of Kernel Very Low Speed] to all version, requast from Ryan.
// .207 20160511 Clear PSTI032 info; Correct Lon/Lat display, requast from Ken.
// .208 20160512 Setup dialog add recentScatterCenter list, requast from Ken.
// .208 20160512 Binary Measurement Data Out support 8Hz, requast from Andrew.
// .208 20160516 Remove Query / Configure Datum in Binary menu, requast from Andrew.
// .209 20160527 Modify [Pinning Of Kernel Very Low Speed] text, requast from Andrew.
// .210 20160615 Add SWCFG_VENDOR_NSHP_FIX_TOOL for NS-HP user patch, requast from Andrew and Oliver.
// .211 20160616 Add Write 0x50000000 to a File for Kayyui, requast from Andrew.
// .212 20160704 Support GNGSV token for special customer output, requast from Leo.
// .213 20160707 Add new command DEVICE UNIQUE ID for Dofun, requast from Andrew and Michael Chuang.
// .214 20160712 Add IQPlot Log function, requast from Terrance.
// .215 20160719 RTK pacth tool version, requast from Andrew.
// .215 20160720 Reorganization resource ID, requast from Alex.
// .216 20160728 Add [Configure/Query PSTI030/PSTI032 Interval] command, requast from Andrew and forum user.
// .216 20160728 Disable [Configure Proprietary NMEA] in Customer Release version, requast from Andrew and Alex.
// .217 20160728 Add tooltips in response window, Reauest from Alex.
// .217 20160801 Remove [Surveying and mapping] in Configure Navigation Mode, requast from Alex.
// .217 20160808 Fix SUP800 parsing issue in multi-hz, requast from Andrew.
// .217 20160808 Add [Quadcopter] in Configure Navigation Mode, requast from Terrance.
// .218 20160809 New Configure geo-fencing data UI, requast from Ken.
// .219 20160810 SUP800 multi-hz maxuim baud rate in 230400, requast from Andrew.
// .220 20160816 Add CWQX version, baudrate support 1200, 2400, requast from Andrew, Leo
// .221 20160830 Remove SHOW_ELEV_AND_CNR_MASK_IN_GEN.
// .221 20160901 Remove restart after [Configure Position Update Rate]. request from Andrew.
// .222 20160905 Modify Zenlane protocol. request from Leo.
// .223 20160905 Modify Zenlane protocol. request from Leo.
// .224 20160905 Modify block transfer in Zenlane protocol. request from Leo.
// .225 20160907 Add [Configure/Query PSTI0004 Interval] command, requast from Andrew.
// .226 20160914 Modify for Glonass RTK version(support 0xE5 output, change bin output format), requast from Andrew, Ryan.
// .226 20160914 1. Add new command to “Re-calculate GLONASS IFB”, the structure is as follow:
// .226 20160914    A0 A1 00 02 6B 04 6F 0D 0A
// .226 20160914 2. Modify binary command 0x1E to add Extended_ Raw_Meas Enabling field.
// .226 20160914 3. Modify binary command 0x89 to add Extended_ Raw_Meas Enabling field.
// .226 20160914 4. Parse EXT_RAW_MEAS – Extended Raw Measurement Data v.1 (0xE5), the protocol is defined in attached file.
// .227 20160919 Add [UART Pass through] to RTK menu, request from Andrew and Ryan.
// .228 20160930 Add Ins DR menu, request from Andrew and Gin.
// .229 20161004 Modify InsDR menu message, request from Andrew and Gin.
// .230 20161007 Add RTK Enable/Disable GPS SV mechanism functions, request from Ryan and Andrew.
// .231 20161013 Modify ellipsoid altitude and sea level altitude from U32 to S32, request from Ken and Andrew.
// .232 20161014 NMEA Player support only RMC no GGA, request from Andrew.
// .233 20161018 Release L2 support and fix PSTI issue, request from Patrick and Ken.
// .234 20161018 Fix L2 size error, request from Patrick.
// .235 20161020 Add Save Host Log function from Andrew.
// .236 20161026 Add Configure RTK Reference Static Started Position, request from Andrew and Ryan.
// .237 20161103 Add [Check all] and [Uncheck all] buttons in RTK On/OFF SV channel, request from Andrew.
// .237 20161103 Redesign all save function for more stable, request from Andrew.
// .238 20161116 Add [HostLog Output To NMEA] in Convert menu, request from Terrance and Andrew.
// .239 20161117 Add [Get 16-IO Register] and [Configure 16-IO Register] in Binary menu, request from Patrick.
// .240 20161123 Fixed [Get 16-IO Register] and [Configure 16-IO Register] id error issue, request from Patrick.
// .241 20161123 Add [Query RTCM Measurement Data Output] and [Configure RTCM Measurement Data Output] in RTK menu, request from Ryan.
// .242 20161123 Fix 0xA8 speed issue, request from Andrew.
// .243 20161124 Modify [RTCM Measurement Data Output] for checking setting value and module type, request from Andrew and Ryan.
// .244 20161124 Modify [RTCM Measurement Data Output] for UI status issue, request from Ken.
// .245 20161128 Add [Write Clock Offset], request from Oliver.
// .246 20161130 Add [INS DR Test], request from Angus.
// .247 20161201 Add Raw menu and add BASE POSITION commands, request from Ryan.
// .248 20161202 Fixed some bugs in BASE POSITION commands.
// .249 20161202 Fixed some text in BASE POSITION commands.
// .250 20161212 Add RTK precisely kinematic base mode, request from Ryan.
// .251 20161212 Parser supports RAW+NMEA, [Configure RTK Mode And Operational Function] add description, request from Ryan.
// .252 20161228 New Save HostLog function.
// .253 20170104 Fixed compatibility issues with Windows XP, reprot from Andrew.
// .254 20170105 Display time when 0xE5 output, reprot from Ryan.
// .254 20170105 Fix NMEA sentence error issue in Binary output mode, report from Leo.
// .254 20170106 Add [Query Clock Offset] in Binary menu, request from Oliver.
// .255 20170106 New parser for multiple message RTCM_NEW_PARSER.
// .256 20170112 Add [Show Beidou Almanac] in menu, requast from forum user.
// .257 20170117 Add SHOW_COM_SELECT flag for show COM port select, requast from forum Leo.
// .258 20170117 Add RTCM / Ublox convert, requast from Roger and Andrew.
// .259 20170220 Move INS DR menu to customer release version, requast from Oliver.
// .260 20170120 Fix some crash issue in parser, requast from Alex.
// .260 20170224 Add DR prodeuction commands in INS DR menu, requast from Angus.
// .261 20170309 Fix KML converter bugs in quality mode, requast from Kevin.
// .262 20170309 Fix KML converter bugs (GSA parsing) in quality mode, requast from Kevin.
// .263 20170315 Add GPS L2 support(GPGSV2), requast from Eric.
// .264 20170322 Automatically fill in fields, requast from Ryan and Ken.
// .265 20170328 Support new 0xA8 format, size is 0x3F, requast from Andrew.
// .266 20170328 Fix 0xA8 time seconds not continuious issue, requast from Andrew.
// .267 20170405 Add all baud rate in download panel, requast from Leo.
// .268 20170407 XN120 change SNR test range (38 to 36), requast from Leo.
// .269 20170407 Add [Configure / Query SBAS Advance] and [Query SBAS Default], requast from Kevin.
// .270 20170413 Fix NMEA Parser bug, report from Ardrew.
// .271 20170413 Modify some Query / Configure SBAS Advance strings, requast from Kevin.
// .272 20170421 Modify [Reset RTK Engine], requast from Ryan.
// .273 20170426 Add [Read 0x60000000 to a File (1KB)], requast from Patrick.
// .273 20170426 Fix issue in GetBinaryAck(), when receive 0xA0, 0xA1 in content will no response. 
// .274 20170524 Test EON Flash Loader and modify some flow.
// .275 20170525 Support EON Flash download and check chipmode.
// .276 20170531 Add LITEON customer version, request from Oliver.
// .277 20170531 Support EON Flash download and protected 3/4 flassh, request from Patrick.
// .278 20170608 Add profile for NAVIC support, request from Oliver and Terrance.
// .279 20170608 Add FLOAT_SNR to support NMEA GSV output flaot in CN, request from Oliver.
// .280 20170622 Navic support NMEA 183 4.1, modify GSA/GSV parser, request from Oliver.
// .281 20170622 Fix NMEA Parser bugs, request from Terrance.
// .282 20170623 Support master/slave download, request from Oliver.
// .283 20170628 Navic version support KML converter, request from Ken.
// .284 20170630 Official release for internal use.
// .285 20170710 Add NAV_MOWING_MODE, request from Leo and Ryan.
// .286 20170710 Support DataLog with crc16, request from Andrew.
// .287 20170712 Support PSTI033 parsing and Query/Configure commands, request from Oliver.
// .288 20170714 Move Liteon Debug Mode to RTK menu and Customer Release version, request from Leo.
// .289 20170814 Fix slave download issue, request from Ken, Andrew.
// .290 20170815 define NEW_SBAS2 for SBAS Advance commands changed, request from Kevin
// .291 20170821 Support NMEA 4.1 L2 Beidou and GLONASS, request from Eric
// .292 20170823 Fix new SBAS2 bugs, request from Kevin
// .293 20170824 Change Ublox converter format, request from Roger
// .294 20170912 Turn off message output before download at 4800 bps, request from Patrick
// .295 20170922 Fix issues in Navic NMEA GSA / GSV, request from Terrance
// .296 20171002 Convert KML support speed and direction, request from Andrew
// .297 20171006 Add Query / Configure DR Predict Rate, request from Jason
// .298 20171006 Fix KML converter issue one addional point when continuious convertion, request from Andrew
// .299 20171024 Ignored illegal NACK message when back from INSDR pass-through, request from Andrew
// .299 20171024 Disable SHOW_COM_SELECT, request from Oliver
// .300 20171102 Fixed NMEA Player play faster after reboot, report from Oliver
// .300 20171107 Support COM port number > 100, request from Oliver
// .301 20171127 Add RTK Slave Baud Rate and Clear RTK Slave Backup Data, request from Andrew and Neil
// .302 20171128 Fix [Configure RTK Slave Baud Rate] bug, report by Ken
// .303 20171130 Show DR Information, request from Andrew
// .304 20171215 Show Time in double format.
// .305 20180120 Fixed download bug, report by Terrence
// .306 20180207 Add Telit Host Log, request from Jason
// .307 20180212 Add _NAVIC_SUPPORT, request from Oliver and Terrance
// .307 20180213 Add PSTI 063, 065, 067, 068, 070 and DR RAW Sensor data output rate, request from Jason
// .308 20180213 Add 0 Hz to DR RAW Sensor data output rate, request from Jason
// .309 20180221 No boost rate change after configure update rate, request from Jason
// .310 20180222 Modify query DR RAW Sensor data output rate return command, request from Jason
// .311 20180223 Fix issue : pure NAVIC show 3D when gsa is 3D, report from Terrance
// .312 20180314 Support NAVIC base, auto send HostLog on command when using Save HostLog, request from Terrance
// .312 20180314 Add Setup to Customer Release version, support DR Rate 4 and 8, request from Terrance and Jason
// .313 20180322 RAW Converter support 0xE5, request from Andrew
// .314 20180327 Add Configure / Query Noise Power Control to Customer Release, request from Leo
// .315 20180416 Add Configure PPS_MODE_SUPPORT_ALIGN_TO_NAVIC, request from Terrance / Oliver
// .316 20180416 Add Query [Query 1PPS Outout Mode], request from Terrance / Oliver
// .317 20180423 Update IQ_Plot to 1.0.0.3 for NAVIC support, request from Terrance
// .318 20180502 Fix specify center can't use in Customer Release version, report by Terrance
// .319 20180507 Add "Navic" to IQ Plot type combo box item, request from Terrance
// .320 20180516 Add "IO Tester", request from Alex
// .321 20180523 Add "IO Tester", request from Alex
// .322 20180528 Delay upload SREC for new V9 FPGA RX size 64, request from Kevin
// .323 20180605 Check an OEM fw (ChinaTower) for force internal loader download, request from Leo
// .324 20180607 Upgrade tool for "CRC_018a_115200_20170712", request from forum customer
// .325 20180613 Fixed "Query Cable Delay" shows negative issue , report by Terrance
// .326 20180621 Fixed "Detect NMEA sentence error" and "Download Test" bugs.
// .327 20180723 Fixed "DataLog Configure" issue for Leo's customer.
// .328 20180725 Add "UAV Binary" in [Configure Message Type], request from Austin.
// .329 20180730 Add "Read 0x40000000 to File" in Utility menu, request from Patrick.
// .330 20180730 Modify "Read 0x40000000 to File" to write 512 KB, request from Patrick.
// .331 20180806 Add [V9 Configure RF IC] in Utility menu, request from Patrick and Terrance.
// .332 20180806 Fixed issue in [V9 Configure RF IC], request from Patrick and Terrance.
// .333 20180807 Modify [V9 Configure RF IC] to support XND2202, request from Patrick and Terrance.
// .334 20180829 Add [Query/Configure ISR CPU Clock Boost Mode], request from Eric.
// .334 20180829 Add [Query/Configure RTK GLONASS Carrier-Phase Inter-Frequency Bias], request from Neil.
// .335 20180830 Add [Test Alpha RTK IO], request from Angus.
// .336 20180830 Fix issue in [Query RTK GLONASS Carrier-Phase Inter-Frequency Bias], request from Ken.
// .337 20180905 Add [Dump Memory To File], request from Patrick.
// .338 20180906 Modify [Configure/Query GNSS Constellation Type] to add NAVIC, request from Terrance.
// .339 20180910 Remove [Factory Default No Reboot], request from Roger and Andrew.
// .340 20180914 Add [V9 Configure RF IC] and [V9 Configure IIR] in Utility menu, request from Patrick and Terrance.
// .341 20180914 Modify [Query Alpha Unique ID] output string format.
// .342 20180919 Add [Configure/Query NAVIC Message Interval] in Venus 8 menu,  request from Terrance.
// .343 20181023 Add [Temporarily Activate License] in Alpha menu, request from Angus.
// .344 20181205 Modify QZSS upper prn from 197 to 200, request from Kevin.
// .345 20181221 Support V9 Tag download, request from Andrew.
// .345 20190109 Add 3 calibration functions in Alpha menu, request from Leo.
// .345 20190116 NMEA PlaYER check time / date field length avoid crash, report from Leo.
// .346 20190119 Update download loader for 0x64, 0x1b command.
// .347 20190201 Support JAXA 0xEF binary message, request from Kevin.
// .348 20190219 Add "Show all unknown binary command" in Setting for show command return, request from Andrew
// .348 20190219 Configure / Query PSTI message interval, request from Jason
// .349 20190225 Add "V9 Internal RF" type in [Utility/Configure RF IC], request from Patrick
// .350 20190226 Modify "V9 Internal RF" type in [Utility/Configure RF IC], request from Eric
// .351 20190226 Fixed [Query 1PPS Pulse Width] wrong command, request from Alex
// .352 20190312 Add [V9 Configure Power Register], request from Patrick
// .353 20190312 Add Attribute field in ConfigRefTimeToGpsTimeDlg, request from Ken, Andrew
// .353 20190313 Add [Get / Set Multiple Registers] and remove 16-IO Register items, request from Patrick
// .354 20190319 Add [Export to file] in [V9 Configure RF IC], request from Ming-Jen
// .355 20190320 Modify [Get / Set RF IC Register] for SWID, request from Ming-Jen, Eric
// .356 20190320 Modify [Get / Set Mutiple RF IC Registers] for SWID, request from Ming-Jen, Eric
// .357 20190325 Add [V9 Power Save By RTC Timer] for SWID, request from Ming-Jen
// .358 20190328 Fixed [V9 Power Save By RTC Timer] reconnect issue, report by Ming-Jen
// .359 20190409 Fixed [Log Decompress] rollover issue, add new option in setup for assign rollover cycle, report from Andrew
// .359 20190409 Add [Configure V9 RF Clock to GPIO0] for SWID, request from Ming-Jen
// .360 20190416 Support GALILEO and multiple frequency, request from Andrew, Neil and Oliver
// .360 20190416 Support GB token for BEIDOU, request from Andrew, Neil and Oliver
// .361 20190416 NAVIC system ID from 4 to 5, report from Neil
// .362 20190416 Fix GPS Chart issue, report from Andrew and Neil
// .363 20190417 Fix Earth view issue, report from Eric
// .364 20190418 Fix GSV parsing signal ID bugs, report from Eric
// .365 20190418 Fix GNS parsing bugs, report from Eric
// .366 20190426 Merge BD, GA and GI snr chart display, report from Oliver
// .367 20190506 Fixed GA NMEA 4.11 issue, report from Eric
// .368 20190506 Fixed parser memory issue, report from SWID
// .369 20190509 Fixed multiple frequency parser memory issue, report from SWID
// .369 20190509 Fixed IqPlot not close when Viewer closed issue, report from Eric
// .369 20190509 Fixed DataLog menu functions, report from Andrew
// .369 20190509 Add [DR MEMS Noise Level] function, rquest from Andrew / Roger.
// .370 20190509 Move [DR MEMS Noise Level] to InternalUse only , rquest from Jason.
// .371 20190513 Modify [DR MEMS Noise Level] in scientific notation , rquest from Jason, Roger.
// .372 20190515 Add [Configure/Query ADR Odometer Scaling Factor] in INS DR menu, rquest from Roger, Andrew.
// .373 20190605 Add [IQ Plot] to Devloper version], rquest from Oliver and Ming-Jen.
// .374 20190618 Fixed RTCM display issue, report by Andrew.
// .375 20190625 Customize for LockheedMartin(_LOCKHEED_MARTIN_), rquest from Oliver.  //released special version
// .376 20190704 Add [PROM ini Generator] in Utility menu, rquest from Andrew.
// .377 20190704 Modify GPS Ephemeris channel from 32 to 37 for QZSS, rquest from Andrew.
// .378 20190704 Special version (AUTO_GET_CLOCK_OFFSET) for show clock offset every 3 seconds, rquest from Oliver and Ming-Jen.
// .379 20190725 Modify pseudorange unit changed to 1 meter for LockheedMartin(_LOCKHEED_MARTIN_), rquest from Oliver.  //released special version
// .380 20190806 Support Query Extened Id for new Tag, rquest from Andrew. 
// .381 20190822 Remove [Signal Disturbance Test] from Venus 8 menu, rquest from Neil. 
// .381 20190827 Support multi frequency raw 0xE5, rquest from Neil / Andrew / Ming-Jen. 
// .382 20190906 Support NCGSA and NCGSV for Quectel LC79D, request from Oliver. 
// .383 20190912 Merge Get/Set Ephemeris codes 
// .384 20190912 Auto AGPS for NavIC General version, request from Oliver
// .385 20190930 Show cycle slip from 0xE5 message, request from Oliver
// .386 20191001 Fix issue when use NMEA mode show all channel is in cycle slip, report by Ken
// .387 20191007 Fix attribute issue in Configure RTK Mode and Operational Function, report by Jim
// .387 20191007 Add Binary Command Tester, request from Andrew
// .387 20191007 New IQ Plot command and message
// .388 20191008 Add [Set Ephemeris File Continuously], request from Andrew
// .388 20191008 Fixed dual frequency raw earth and snr view bugs, report from Ken
// .389 20191008 Fixed dual frequency raw earth and snr view bugs, report from Ken
// .390 20191028 Add NAVSPARK_MINI_GPIO_QUERY for a NavSpark customer, report from Oliver
// .391 20191104 V9 Prom ini generator, request from Andrew
// .392 20191111 Fix a GGA parsing issue for 和芯星通, request from Oliver, Andrew
// .393 20191113 Modify Configure RF IC by SWIC new spec, request from Ming-Jen
// .394 20191114 Modify Configure RF IC by SWIC new spec, request from Ming-Jen
// .395 20191120 Add Test AGPS Server in AGPS menu, request from Andrew
// .395 20191125 Automatic detect v9 aes tag prom binary file and use 0x64 0x1b command to download, request from Andrew
// .396 20191129 Dual-freq RTCM support: 1077, 1087, 1107, 1117, 1127, request from Ken and Neil
// .397 20191212 Update loader_v8_add_tag_20191212113345 for QFN40 V9 with EON16M/8M, Alex released
// .397 20191212 Add Configure RF Clock Out, request from Ming-Jen, V8 Final
// .398 20191217 Update loader_v8_add_tag_20191212113345 for V9 EVB W25Q80, report from Ken
// .399 20191220 Update loader_v8_add_tag_20191220144037 for LZMA download, Alex released
// .400 20191227 Add Hot key for cold start and RTK status notify, request from Austin
// .401 20191231 Support LC79D NMEA with dual freq, request from Oliver, Ken
// .402 20200102 Fixed BD12 display issue, report from Andrew
// .404 20200107 Decode DR special message, report from Roger
// .405 20200108 Fixed Phoenix PROM ini generator issues, report from Andrew
// .406 20200116 Update loader_v8_add_tag_20191220144037 for tag support EON 16M, report from Andrew
// .407 20200204 Add RTCM Galileo MSM 7 1097 option in Configure RTCM Measurement Data Out, request from Neil
// .408 20200205 Add RTCM Galileo MSM 7 1097 option in Query RTCM Measurement Data Out, request from Neil
// .409 20200206 Update loader_v8_add_tag_20200206145058 for Alpha Key issue, report from Andrew
// .409 20200207 Add [Test Alpha+ RTK GPIO], request from Angus
// .410 20200214 Fixed issue of Raw Galileo display, report from Andrew
// .411 20200214 Add [Enable/Disable Galileo SV mechanism] in RTK menu, request from Ken
// .412 20200219 Make a wide version for Galileo satellites display
// .413 20200227 Add [RTK Elevation and CNR Mask] functions, require from Neil and Andrew
// .001 20200302 Update loader_v8_add_tag_20200302154408.srec for new download commands, request from Andrew
// .002 20200302 Update loader_v8_add_tag_20200302154408.srec for new download commands, request from Andrew
// .003 20200312 Update v9DownloadLoaderAddTag-20200312175555.srec for download issues, request from Alex
// .004 20200323 Modify Phoenix menu for Security board commands, request from Alex
// .004 20200324 Add GNSS_General_SwidCustomer version, request from Ming-Jen
// .005 20200325 *Update v9DownloadLoaderAddTag-20200325154841.srec, detect prom size and add some download error message, request from Andrew
// .005 20200407 Support UBX NAV-PVT and NAV_CLOCK message, request from Roger
// .006 20200408 Support Gelileo RTCM1097 message and disable Configure / Query RTK Mode, request from forum user and Roger
// .007 20200408 Support 0xE6, 0xE7, 0xE8 message and Add Configure RTK GLONASS Carrier-Phase Inter-Frequency Bias to Customer Release, request from Andrew and Oliver
// .008 20200413 Modify Beidou signal string table, request from Andrew
// .009 20200414 Player show unknown RTCM, request from Andrew
// .009 20200415 Parsing GNTHS message, request from Oliver
// .010 20200422 Add ProductionTest PX100, request from Alex
// .011 20200428 Add NMEA String Interval menu functions, request from Andrew/Alex
// .012 20200506 Fixed calc CRC bug when prom size > 1Mb , report from Andrew
// .012 20200507 Modify 0xE5, 0xE7, 0xE8 parser, report from Oliver, Ken
// .012 20200508 Support RTCM 1033 (Display only), request from Andrew
// .013 20200604 Add Host Data Dump On/Off, request from Andrew/Jason
// .014 20200605 Fixed cycle slip display, report from Ken
// .015 20200608 Remove QZSS 1 - 3 restriction, request from Ken
// .016 20200611 Max Geo-Fencing number to 16, request from Andrew
// .016 20200611 Add PROUCTION_TEST_200611 for Angus production, request from Oliver
// .017 20200611 Add PSTI007 for Geo-Fencing, request from Andrew
// .018 20200611 Modify Query Geofecing Data timeout, request from Andrew
// .019 20200617 Add Query RTC in Utility menu for Developer version, request from Ming-Jen
// .020 20200620 Fix RTCM decode issue, report from Ken
// .021 20200621 Use QueryGnssConstellationType in Configure RTCM dialg, report from Ken
// .022 20200622 Fixed player bug when only GLRMC, report from Ken
// .023 20200629 Fixed timeout issue, report from Andrew
// .024 20200707 Adjust time out for ROM mode at 9600 bps, report from Jason
// .025 20200708 Add [Query / Configure RTK Precisely Kinematic Base serial port baud rate] commands, report from Andrew
// .026 20200804 Add datum "CGCS2000" as index 221, request from Andrew
// .026 20200805 Support 0xE9 time stamp when raw output, request from Andrew, Oliver
// .026 20200811 Change IQ Plot GALILEO GAL-E5b channel from 3 to 2, request from Eric
// .026 20200811 Add [Configure Tracking Module Parameter Setting], request from Iyan and Andrew
// .026 20200811 SkyTraq raw message 0xE5 doesn't use for draw earth and snr, request Andrew when multi-Hz
// .027 20200812 Change [Configure 1PPS Pulse Width] range from (1~100000) to (1000~500000), request Ken
// .028 20200813 Different spec on [Configure 1PPS Pulse Width] and [Configure Cable Delay] for SWID Developer version, request Ken / Jason
// .029 20200828 Modify [Configure / Query RTCM Measurement Data Out] for RTCM ephemeris, request from Yu-Hsien and Andrew
// .029 20200828 Add [Configure RTK Functions] in RTK menu, request from Ken
// .030 20200828 Supports show 33 satellites in a row of SNR chart, request from Ken
// .030 20200828 Supports continuous GSA (supports more than 12 sates in use), request from Ken
// .031 20200902 Modify [Configure Tracking Module Parameter Setting] UI, request from Iyan
// .033 20200903 Modify [Configure Tracking Module Parameter Setting] UI, request from Iyan
// .034 20200908 Fixed Configure Mode to base mode, parser will stop issue, request from Ken
// .035 20200909 Add NTRIP function, request from Oliver
// .036 20200929 Add TCPIP Server and TCPIP connection, request from Oliver / Andrew
// .036 20200929 Add [Configure / Query Ifree Mode] in Phoenix menu, request from Jason
// .037 20200930 Release for TCPIP / NTRIP functions, request from Oliver
// .038 20201006 Fixed COM port adding bug and show all unknowk message bug, report from Ken and Andrew

#define SW_FUN_DATALOG		        0x0001
#define SW_FUN_AGPS				        0x0002
#define SW_FUN_DR				          0x0004

#define SOFTWARE_FUNCTION		      (SW_FUN_DATALOG | SW_FUN_AGPS)
#define IS_DEBUG				          0
//title.Format("%s %s V%s for %s", APP_CAPTION, APP_TITLE, APP_VERSION, APP_MODULE);
#define APP_CAPTION				        "GNSS Viewer"
#define APP_TITLE				          ""              //Internal Use, Customer Release, NMEA Player...
#define APP_VERSION				        "2.1.038"
#define APP_MODULE				        "Phoenix"

#define	Sktyraq					          0x0000
#define SWID					            0x0001		//重慶西南集成電路設計
#define Ericsson				          0x0004
#define OlinkStar				          0x4F4C
#define Eten					            0x000A

//For customer upgrade
#define UPGRADE_DOWNLOAD		      0
#define UPGRADE_CRC				        0x0000
#define UPGRADE_DUEDATE_Y		      0
#define UPGRADE_DUEDATE_M		      0
#define UPGRADE_DUEDATE_D		      0
#define UPGRADE_ADD_TAG			      0
#define UPGRADE_TAG_VALUE		      0

//Default download boost baudrate
#define BAUDRATE_DEFAULT		      5		//8=921600,7=460800,6=230400,5=115200,4=57600,3=38400,2=19200,1=9600,0=4800
#define FIRMWARE_DOWNLOAD		      1   //Define FIRMWARE_DOWNLOAD 0 to disable download UI in Viewer
#define MAX_SCATTER_COUNT		      300 //Default scatter count
#define ODOMETER_SUPPORT		      0   //Show "Reset Odometer" button
#define ACTIVATE_MINIHOMER		    0
#define NMEA_INPUT				        0
#define GSA_MAX_SATELLITE		      12
#define SHOW_BINARY_DATA		      1   //Display [Show Binary Data] in menu
#define ALWAYS_USE_EXTERNAL_SREC	0   //Using external SREC file directly, no prompt.
#define ECOMPASS_CALIBRATION	    0   //Show eCompass calibration UI in Viewer
#define TIMING_MODE				        1   //20170531, Alex modified from 0 to 1.
#define _TAB_LAYOUT_			        0   //Information has multiple pages
#define OPEN_PINNING_RESERVE	    1   //Show [Disable] in position pining UI
//#define _V8_SUPPORT				      1
#define MERGE_BD_GA_GI	          1   //Merge BD, GA and GI SNR chart in bottom chart
#define GPS_EPHEMERIS_CHANNEL     32  //32 or 37, if 37, it's include QZSS
#define GLONASS_EPHEMERIS_CHANNEL 24
#define GALILEO_EPHEMERIS_CHANNEL 24
#define BEIDOU_EPHEMERIS_CHANNEL  37  
#define NAVIC_EPHEMERIS_CHANNEL   14  
#define GPS_QZSS_EPHEMERIS_CHANNEL  37 
#define QZSS_EPHEMERIS_CHANNEL    5
#define AUTO_GET_CLOCK_OFFSET     0

#define GG12A					            0
#define SHOW_CLOCK_OFFSET		      0
#define SHOW_NOISE				        0
#define DATA_POI				          0
#define CUSTOMER_DOWNLOAD		      0
#define CUSTOMER_ID				        Sktyraq

//NMEA PRN parsing type
//Type 0 - GL: 65~96; GP: 1~64, 183~188, 193~200; GI: 240 ~ 300; BD: others
//Type 1 - GL: 201~300; GP: 101~200; BD: 1~100; GA: 301~400
//Type 2 - GP: 1~85; BD: others
//Type 3 - GL: 65~96; GP: 1~64, 183~188, 193~200; BD: others; Don't care GPS_183_188 flag.
#define NMEA_PRN_TYPE			        0
#define TIMING_MONITORING		      1
#define TIMING_OUTPUT_ALIGN		    1
#define	SHOW_ERROR_NMEA_NOTIFY	  0
#define	SHOW_ERROR_TIME_NOTIFY	  0
#define	MORE_ENU_SCALE			      1
#define	GPS_183_188				        1   //PRN 183 ~ 188 for QZSS system
#define	MODULE_SUP_800		        0
#define	RESOURCE_LOADER_ID	      0		//specify a special download loader for customer.
#define CHECK_SAEE_MULTIHZ_ON	    0		//Check SAEE and Multi-Hz can't both on.
#define INVERT_LON_LAT			      1		//Final spec for GeoFecing spec.
#define _MORE_INFO_				        0		//More information field for RTK
#define RAW_MENU				          1		//Show RAW menu 20161201 request from Ryan
#define RTK_MENU				          1		//Show RTK menu
#define INS_DR_MENU				        1	  //Show INS_DR menu
#define EXTRA_WIDE                1   //Window has extra wide size
#if (EXTRA_WIDE)
 #define EXTRAS_WIDE_SIZE         4
 #define CLIENT_WIDTH			        (1008 + EXTRAS_WIDE_SIZE)	//Viewer window client width
#else
 #define CLIENT_WIDTH			        1008	//Viewer window client width
#endif
#define CLIENT_HEIGHT			        678	//Viewer window client height
#define AUTO_QUERY_VERSION		    0
#define DOWNLOAD_IMMEDIATELY	    0		//start download immediately when nmea come in.
#define SPECIAL_TEST			        0		//Test ETEN case 20160202
#define GEO_FENCING_CMD			      1		//0 - old geo-fencing cmd, 1 - new geo-fencing cmd
#define _SHOW_RTK_BASELINE_		    0
#define SHOW_PATCH_MENU			      0		//Use for user patch.
#define DOFUN_UNIQUE_ID			      0		//Show DOFUN UNIQUE ID menu
#define DEVELOPER_VERSION         0		//Developer version.
#define SPECIAL_BAUD_RATE			    0		//For RTK GLONASS EVK, baud rate * 1.5
#define CUSTOMER_CWQX_160815  		0   //上海長望氣象, Add 1200, 2400 bps, maximum 115200 bps
#define FIX_DOWNLOAD_115200   		0   //Download can only use 115200 bps
#define CUSTOMER_ZENLANE_160808  	0   //善領科技, Add customized commands
#define SUPPORT_BDL2_GSV2         0   //Support BDL2 information in GSV2 token.
#define RTCM_NEW_PARSER         	1   //Use RTCM new parser.
#define SHOW_COM_SELECT           0   //Show COM port select in [Configure Serial Port]
#define SMALL_UI                  0   //Small UI, No Earth View, Scatter and menu
#define KML_USE_CHECKLISTBOX      1   //Use CCheckListBox in CKmlDlg
#define LITEON_CUSTOMIZE          0   //Use LITEON special version
//#define FLOAT_SNR                 0   //Use FLOAT SNR in GSV token
#define NAV_MOWING_MODE           0   //Add mowing machine mode in Navigation Mode
#define USE_EXTERNAL_DN_BIN_CMD   1   //Download external loader use binary command
#define NEW_SBAS2                 1   //Remove user define submask field in SBAS2 commands.
#define _NAVIC_SUPPORT_           1   //Show NAVIC satellites in Galileo position
#define NO_BOOST_UPDATE_RATE_CHANGE 1 //Doesn't change baudrate after update rate change
#define PPS_MODE_SUPPORT_ALIGN_TO_NAVIC 1 //Show ALIGN_TO_NAVIC in Configure 1PPS Output Mode
#define MICROSATELLITE_PATCH      0   //For Microsatellite upgrade
#define ENABLE_AUTO_AGPS          0   //Do autmatic AGPS when push cold start button
#define NAVSPARK_MINI_GPIO_QUERY  0   //For a navspark-mini customer to query GPIO status
#define _NEIL_TEMP_VER_           0
#define SHOW_RF_CONFIG            0   //For SWID show RF Configuration
#define NO_PHOENIX_MENU           0   //For SWID 
#define PROUCTION_TEST_200611     0   //For Angus production test in 2020/06/11 


////////////////////////////////////////////////////////////////////////////////////////////
#if defined(SWCFG_VENDOR_GNSS_GENERAL_SWID)  ////20200324 Request from Ming-Jen for SWID
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef APP_MODULE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef _MORE_INFO_
 #undef _TAB_LAYOUT_
 #undef SHOW_RF_CONFIG
 #undef NO_PHOENIX_MENU
 #undef SOFTWARE_FUNCTION

 #define APP_CAPTION			          "GNSS Viewer"
 #define APP_TITLE				          "Customer Release"
 #define APP_MODULE                 "Phoenix"
 #define GNSS_VIEWER			          1
 #define IS_DEBUG				            0
 #define BAUDRATE_DEFAULT		        7
 #define TIMING_MODE			          1
 #define _MORE_INFO_			          0		//Please define _MORE_INFO_ in resource preprocessor for rc2.
 #define _TAB_LAYOUT_			          1		//Please define _TAB_LAYOUT_ in resource preprocessor for rc2.
 #define SHOW_RF_CONFIG             1
 #define NO_PHOENIX_MENU            1
 #define SOFTWARE_FUNCTION		      (SW_FUN_AGPS)

#elif defined(SWCFG_VENDOR_GNSS_GENERAL_NAVSPARK_MINI_GPIO)  //20191028
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef _MORE_INFO_
 #undef _TAB_LAYOUT_
  #undef NAVSPARK_MINI_GPIO_QUERY

 #define APP_CAPTION			          "GNSS Viewer Ns"
 #define APP_TITLE				          "Customer Release"
 #define GNSS_VIEWER			          1
 #define IS_DEBUG				            0
 #define BAUDRATE_DEFAULT		        7
 #define TIMING_MODE			          1
 #define _MORE_INFO_			          0		//Please define _MORE_INFO_ in resource preprocessor for rc2.
 #define _TAB_LAYOUT_			          1		//Please define _TAB_LAYOUT_ in resource preprocessor for rc2.
 #define NAVSPARK_MINI_GPIO_QUERY		1   //Request from Oliver to provide a query command for customer 20191028

#elif defined(SWCFG_VENDOR_GNSS_GENERAL_NAVIC)  //20190918
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef _MORE_INFO_
 #undef _TAB_LAYOUT_
 #undef ENABLE_AUTO_AGPS

 #define APP_CAPTION			          "GNSS Viewer"
 #define APP_TITLE				          "NavIC Customer Release"
 #define GNSS_VIEWER			          1
 #define IS_DEBUG				            0
 #define BAUDRATE_DEFAULT		        7
 #define TIMING_MODE			          1
 #define _MORE_INFO_			          0		//Please define _MORE_INFO_ in resource preprocessor for rc2.
 #define _TAB_LAYOUT_			          1		//Please define _TAB_LAYOUT_ in resource preprocessor for rc2.
 #define ENABLE_AUTO_AGPS			      1

#elif defined(SWCFG_VENDOR_LOCKHEED_MARTIN_GENERAL)
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef _MORE_INFO_
 #undef _TAB_LAYOUT_

 #define APP_CAPTION			          "GNSS Viewer"
 #define APP_TITLE				          "Customer Lockheed Martin"
 #define GNSS_VIEWER			          1
 #define IS_DEBUG				            0
 #define BAUDRATE_DEFAULT		        7
 #define TIMING_MODE			          1
 #define _MORE_INFO_			          0		//Please define _MORE_INFO_ in resource preprocessor for rc2.
 #define _TAB_LAYOUT_			          1		//Please define _TAB_LAYOUT_ in resource preprocessor for rc2.
 #define _LOCKHEED_MARTIN_          1

#elif defined(SWCFG_VENDOR_GENERAL_DR_NEW)
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef SHOW_ERROR_NMEA_NOTIFY
 #undef MAX_SCATTER_COUNT
 #undef _MORE_INFO_
 #undef _TAB_LAYOUT_
 #undef SOFTWARE_FUNCTION

 #define APP_CAPTION			          "GNSS Viewer"
 #define APP_TITLE				          "Internal Use DR"
 #define GNSS_VIEWER			          1
 #define IS_DEBUG				            1
 #define BAUDRATE_DEFAULT		        7
 #define TIMING_MODE			          1
 #define SHOW_ERROR_NMEA_NOTIFY     1
 #define MAX_SCATTER_COUNT		      1000
 #define _MORE_INFO_			          0		//Please define _MORE_INFO_ in resource preprocessor for rc2.
 #define _TAB_LAYOUT_			          1		//Please define _TAB_LAYOUT_ in resource preprocessor for rc2.
 #define SOFTWARE_FUNCTION		      (SW_FUN_DATALOG | SW_FUN_AGPS | SW_FUN_DR)

#elif defined(SWCFG_VENDOR_MICROSATTELLITE_UPGRADE)   //20180518 Request from Oliver
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE_DEFAULT
 #undef MAX_SCATTER_COUNT
 #undef CLIENT_WIDTH
 #undef CLIENT_HEIGHT
 #undef SHOW_PATCH_MENU
 #undef SHOW_PATCH_MENU		
 #undef UPGRADE_DOWNLOAD		
 #undef UPGRADE_CRC			
 #undef UPGRADE_DUEDATE_Y		
 #undef UPGRADE_DUEDATE_M	
 #undef UPGRADE_DUEDATE_D	
 #undef UPGRADE_ADD_TAG	
 #undef UPGRADE_TAG_VALUE	
 #undef MICROSATELLITE_PATCH

 #define APP_CAPTION			          "Microsatellite Upgrade Tool"
 #define APP_TITLE				          ""
 #define GNSS_VIEWER			          1
 #define IS_DEBUG				            0
 #define BAUDRATE_DEFAULT		        6
 #define MAX_SCATTER_COUNT		      100
 #define CLIENT_WIDTH			          292		//Viewer window client width
 #define CLIENT_HEIGHT			        398		//Viewer window client height
 #define SHOW_PATCH_MENU		        1
 #define UPGRADE_DOWNLOAD		        0
 #define UPGRADE_CRC			          0xFFFFFFFF
 #define UPGRADE_DUEDATE_Y		      0
 #define UPGRADE_DUEDATE_M		      0
 #define UPGRADE_DUEDATE_D		      0
 #define UPGRADE_ADD_TAG		        1
 #define UPGRADE_TAG_VALUE		      0xA001
 #define MICROSATELLITE_PATCH       1

#elif defined(SWCFG_VENDOR_NAVIC_INTERNALUSE) //20170608 for Oliver demo
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE_DEFAULT
 #undef SHOW_ERROR_NMEA_NOTIFY
 #undef MAX_SCATTER_COUNT
 #undef _TAB_LAYOUT_

 #define APP_CAPTION			          "GNSS Viewer(NavIC)"
 #define APP_TITLE				          "Internal Use"
 #define GNSS_VIEWER			          1
 #define IS_DEBUG				            1
 #define BAUDRATE_DEFAULT		        7
 #define SHOW_ERROR_NMEA_NOTIFY     1
 #define MAX_SCATTER_COUNT		      1000
 #define _TAB_LAYOUT_			          1		//Please define _TAB_LAYOUT_ in resource preprocessor for rc2.
 #define _NAVIC_CONVERT_			      1	  //Please add _NAVIC_CONVERT_ in resource prheader

#elif defined(SWCFG_VENDOR_NAVIC_GENERAL)
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE_DEFAULT
 #undef _TAB_LAYOUT_

 #define APP_CAPTION			          "GNSS Viewer(NavIC)"
 #define APP_TITLE				          "Customer Release"
 #define GNSS_VIEWER			          1
 #define IS_DEBUG				            0
 #define BAUDRATE_DEFAULT		        7
 #define _TAB_LAYOUT_			          1		//Please define _TAB_LAYOUT_ in resource preprocessor for rc2.
 #define _NAVIC_CONVERT_			      1	  //Please add _NAVIC_CONVERT_ in resource prheader

#elif defined(SWCFG_VENDOR_GNSS_LITEON_170531) //20170531 for LITEON
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef APP_MODULE
 #undef BAUDRATE_DEFAULT
 #undef _TAB_LAYOUT_
 #undef LITEON_CUSTOMIZE

 #define APP_CAPTION			          "GNSS Viewer"
 #define APP_TITLE				          "Customer Release"
 #define APP_MODULE				          ""
 #define BAUDRATE_DEFAULT		        5
 #define _TAB_LAYOUT_			          1 //Please define _TAB_LAYOUT_ in resource preprocessor for rc2.
 #define LITEON_CUSTOMIZE           1

#elif defined(SWCFG_VENDOR_GNSS_XN120_TESTER_BEIDOU) //20170308 for Leo
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef APP_MODULE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef _MORE_INFO_
 #undef _TAB_LAYOUT_
 #undef SMALL_UI
 #undef CLIENT_WIDTH
 #undef CLIENT_HEIGHT

 #define APP_CAPTION			          "XN120 Tester Viewer"
 #define APP_TITLE				          "(GPS + BEIDOU2)"
 #define APP_MODULE				          ""
 #define GNSS_VIEWER			          1
 #define IS_DEBUG				            0
 #define BAUDRATE_DEFAULT		        7
 #define TIMING_MODE			          1
 #define _MORE_INFO_			          0		//Please define _MORE_INFO_ in resource preprocessor for rc2.
 #define _TAB_LAYOUT_			          0		//Please define _TAB_LAYOUT_ in resource preprocessor for rc2.
 #define SMALL_UI			              1
 #define CLIENT_WIDTH			          577
 #define CLIENT_HEIGHT			        341
 #define XN120_TESTER
 #define XN120_TESTER_BEIDOU

#elif defined(SWCFG_VENDOR_GNSS_XN120_TESTER) //20170308 for Leo
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef APP_MODULE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef _MORE_INFO_
 #undef _TAB_LAYOUT_
 #undef SMALL_UI
 #undef CLIENT_WIDTH
 #undef CLIENT_HEIGHT

 #define APP_CAPTION			          "XN120 Tester Viewer"
 #define APP_TITLE				          "(GPS Only)"
 #define APP_MODULE				          ""
 #define GNSS_VIEWER			          1
 #define IS_DEBUG				            0
 #define BAUDRATE_DEFAULT		        7
 #define TIMING_MODE			          1
 #define _MORE_INFO_			          0		//Please define _MORE_INFO_ in resource preprocessor for rc2.
 #define _TAB_LAYOUT_			          0		//Please define _TAB_LAYOUT_ in resource preprocessor for rc2.
 #define SMALL_UI			              1
 #define CLIENT_WIDTH			          577
 #define CLIENT_HEIGHT			        341
 #define XN120_TESTER

#elif defined(SWCFG_VENDOR_GNSS_CUSTOMER_PRODUCTION_OLIVER20161128) //20161128 for Oliver
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef APP_VERSION				    
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef _MORE_INFO_
 #undef _TAB_LAYOUT_

 #define APP_CAPTION			          "GNSS Viewer"
 #define APP_TITLE				          "Customer Release"
 #define APP_VERSION			          "2.1.245"

 #define GNSS_VIEWER			          1
 #define IS_DEBUG				            0
 #define BAUDRATE_DEFAULT	          7
 #define TIMING_MODE			          1
 #define _MORE_INFO_			          0		//Please define _MORE_INFO_ in resource preprocessor for rc2.
 #define _TAB_LAYOUT_			          1		//Please define _TAB_LAYOUT_ in resource preprocessor for rc2.
 #define PRODUCTION_OLIVER20161128
 #define CUSTOMIZE_COSDSTART_BUTTON

#elif defined(SWCFG_VENDOR_GNSS_NMEAPLAYER_BDL2) //20161018 For Patrick and Evan
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef NMEA_INPUT
 #undef SHOW_ERROR_NMEA_NOTIFY
 #undef CLIENT_WIDTH
 #undef CLIENT_HEIGHT
 #undef _MORE_INFO_
 #undef _TAB_LAYOUT_
 #undef SUPPORT_BDL2_GSV2

 #define APP_CAPTION			          "GNSS Viewer"
 #define APP_TITLE				          "NMEA Player"
 #define GNSS_VIEWER			          1
 #define IS_DEBUG				            0
 #define BAUDRATE_DEFAULT		        7
 #define TIMING_MODE			          1
 #define NMEA_INPUT				          1
 #define SHOW_ERROR_NMEA_NOTIFY     1
 #define CLIENT_WIDTH			          1008	
 #define CLIENT_HEIGHT			        614
 #define _MORE_INFO_  		          0		//Please define _MORE_INFO_ in resource preprocessor for rc2.
 #define _TAB_LAYOUT_			          1		//Please define _TAB_LAYOUT_ in resource preprocessor for rc2.
 #define SUPPORT_BDL2_GSV2          1

#elif defined(SWCFG_VENDOR_GNSS_BDL2_NMEA)  //20161018 For Patrick and Evan
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef SHOW_ERROR_NMEA_NOTIFY
 #undef _MORE_INFO_
 #undef _TAB_LAYOUT_
 #undef SUPPORT_BDL2_GSV2

 #define APP_CAPTION			          "GNSS Viewer"
 #define APP_TITLE				          "Internal Use L2"
 #define GNSS_VIEWER			          1
 #define IS_DEBUG				            1
 #define BAUDRATE_DEFAULT		        7
 #define TIMING_MODE			          1
 #define SHOW_ERROR_NMEA_NOTIFY     1
 #define _MORE_INFO_			          0		//Please define _MORE_INFO_ in resource preprocessor for rc2.
 #define _TAB_LAYOUT_			          1		//Please define _TAB_LAYOUT_ in resource preprocessor for rc2.
 #define SUPPORT_BDL2_GSV2          1

#elif defined(SWCFG_VENDOR_GNSS_GENERAL_ZENLANE_160808)  //For 善領科技, 20160808, Request from Leo
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef _MORE_INFO_
 #undef _TAB_LAYOUT_
 #undef CUSTOMER_ZENLANE_160808

 #define APP_CAPTION			          "GNSS Viewer"
 #define APP_TITLE				          "Zenlane Release"
 #define GNSS_VIEWER			          1
 #define IS_DEBUG				            0
 #define BAUDRATE_DEFAULT		        7
 #define TIMING_MODE			          1
 #define _MORE_INFO_				        0   //Please define _MORE_INFO_ in resource preprocessor for rc2.
 #define _TAB_LAYOUT_			          1		//Please define _TAB_LAYOUT_ in resource preprocessor for rc2.
 #define CUSTOMER_ZENLANE_160808	  1

#elif defined(SWCFG_VENDOR_GNSS_GENERAL_CWQX_160815)  //For 上海長望氣象, 20160815, Request from Leo, Andrew
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef _MORE_INFO_
 #undef _TAB_LAYOUT_
 #undef CUSTOMER_CWQX_160815
 #undef FIX_DOWNLOAD_115200

 #define APP_CAPTION			          "GNSS Viewer"
 #define APP_TITLE				          "CWQX Release"
 #define GNSS_VIEWER			          1
 #define IS_DEBUG				            0
 #define BAUDRATE_DEFAULT		        7
 #define TIMING_MODE			          1
 #define _MORE_INFO_			          0		//Please define _MORE_INFO_ in resource preprocessor for rc2.
 #define _TAB_LAYOUT_			          1		//Please define _TAB_LAYOUT_ in resource preprocessor for rc2.
 #define CUSTOMER_CWQX_160815			  1
 #define FIX_DOWNLOAD_115200			  1

#elif defined(SWCFG_VENDOR_GNSS_SUP800_NMEAPLAYER)
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef NMEA_INPUT
 #undef SHOW_ERROR_NMEA_NOTIFY
 #undef CLIENT_WIDTH
 #undef CLIENT_HEIGHT
 #undef MODULE_SUP_800

 #define APP_CAPTION			          "GNSS Viewer"
 #define APP_TITLE				          "SUP800 Player"
 #define GNSS_VIEWER			          1
 #define IS_DEBUG				            0
 #define BAUDRATE_DEFAULT		        7
 #define TIMING_MODE			          1
 #define NMEA_INPUT				          1
 #define SHOW_ERROR_NMEA_NOTIFY     1
 #define CLIENT_WIDTH			          1008	
 #define CLIENT_HEIGHT			        614
 #define MODULE_SUP_800             1

#elif defined(SWCFG_VENDOR_GNSS_SPECIAL_BAUDRATE)
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef _TAB_LAYOUT_
 #undef DEVELOPER_VERSION
 #undef SPECIAL_BAUD_RATE

 #define APP_CAPTION			          "GNSS Viewer"
 #define APP_TITLE				          "Special Baud Rate"
 #define GNSS_VIEWER			          1
 #define IS_DEBUG				            0
 #define BAUDRATE_DEFAULT		        5
 #define TIMING_MODE			          1
 #define _TAB_LAYOUT_			          1		//Please define _TAB_LAYOUT_ in resource preprocessor for rc2.
 #define DEVELOPER_VERSION	              1
 #define SPECIAL_BAUD_RATE	        1

#elif defined(SWCFG_VENDOR_GNSS_DEVELOPER)
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef _TAB_LAYOUT_
 #undef DEVELOPER_VERSION

 #define APP_CAPTION			          "GNSS Viewer"
 #define APP_TITLE				          "Developer Release"
 #define GNSS_VIEWER			          1
 #define IS_DEBUG				            0
 #define BAUDRATE_DEFAULT		        7
 #define TIMING_MODE			          1
 #define _TAB_LAYOUT_			          1		//Please define _TAB_LAYOUT_ in resource preprocessor for rc2.
 #define DEVELOPER_VERSION	              1

#elif defined(SWCFG_VENDOR_GNSS_CUSTOMER_DOFUN_20160707)
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef _MORE_INFO_
 #undef _TAB_LAYOUT_
 #undef DOFUN_UNIQUE_ID

 #define APP_CAPTION			          "GNSS Viewer"
 #define APP_TITLE				          "Unique ID Release"
 #define GNSS_VIEWER			          1
 #define IS_DEBUG				            0
 #define BAUDRATE_DEFAULT		        7
 #define TIMING_MODE			          1
 #define _MORE_INFO_			          0		//Please define _MORE_INFO_ in resource preprocessor for rc2.
 #define _TAB_LAYOUT_			          1		//Please define _TAB_LAYOUT_ in resource preprocessor for rc2.
 #define DOFUN_UNIQUE_ID		        1		

#elif defined(SWCFG_VENDOR_NSHP_FIX_TOOL)
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE_DEFAULT
 #undef MAX_SCATTER_COUNT
 #undef CLIENT_WIDTH
 #undef CLIENT_HEIGHT
 #undef SHOW_PATCH_MENU
 #undef SHOW_PATCH_MENU		
 #undef UPGRADE_DOWNLOAD		
 #undef UPGRADE_CRC			
 #undef UPGRADE_DUEDATE_Y		
 #undef UPGRADE_DUEDATE_M	
 #undef UPGRADE_DUEDATE_D	
 #undef UPGRADE_ADD_TAG	
 #undef UPGRADE_TAG_VALUE	

 #define APP_CAPTION			          "RTK Patch Tool"
 #define APP_TITLE				          ""
 #define GNSS_VIEWER			          1
 #define IS_DEBUG				            0
 #define BAUDRATE_DEFAULT		        6
 #define MAX_SCATTER_COUNT		      100
 #define CLIENT_WIDTH			          292		//Viewer window client width
 #define CLIENT_HEIGHT			        398		//Viewer window client height
 #define SHOW_PATCH_MENU		        1
 #define UPGRADE_DOWNLOAD		        0
 #define UPGRADE_CRC			          0x2FFE
 #define UPGRADE_DUEDATE_Y		      2019
 #define UPGRADE_DUEDATE_M		      6
 #define UPGRADE_DUEDATE_D		      7
 #define UPGRADE_ADD_TAG		        1
 #define UPGRADE_TAG_VALUE		      0xA012
 #define ALPHA_UUID_PATCH           1

#elif defined(SWCFG_VENDOR_GNSS_NMEAPLAYER_NEW)
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef NMEA_INPUT
 #undef SHOW_ERROR_NMEA_NOTIFY
 //#undef CLIENT_WIDTH
 #undef CLIENT_HEIGHT
 #undef _MORE_INFO_
 #undef _TAB_LAYOUT_

 #define APP_CAPTION			          "GNSS Viewer"
 #define APP_TITLE				          "NMEA Player"
 #define GNSS_VIEWER			          1
 #define IS_DEBUG				            0
 #define BAUDRATE_DEFAULT		        7
 #define TIMING_MODE			          1
 #define NMEA_INPUT				          1
 #define SHOW_ERROR_NMEA_NOTIFY     1
 //#define CLIENT_WIDTH			          1008
 #define CLIENT_HEIGHT			        614
 #define _MORE_INFO_			          0		//Please define _MORE_INFO_ in resource preprocessor for rc2.
 #define _TAB_LAYOUT_			          1		//Please define _TAB_LAYOUT_ in resource preprocessor for rc2.

#elif defined(SWCFG_VENDOR_GNSS_GENERAL_NEW)
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef _MORE_INFO_
 #undef _TAB_LAYOUT_

 #define APP_CAPTION			          "GNSS Viewer"
 #define APP_TITLE				          "Customer Release"
 #define GNSS_VIEWER			          1
 #define IS_DEBUG				            0
 #define BAUDRATE_DEFAULT		        7
 #define TIMING_MODE			          1
 #define _MORE_INFO_			          0		//Please define _MORE_INFO_ in resource preprocessor for rc2.
 #define _TAB_LAYOUT_			          1		//Please define _TAB_LAYOUT_ in resource preprocessor for rc2.

#elif defined(SWCFG_VENDOR_GNSS_INTERNALUSE_NEW)
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef SHOW_ERROR_NMEA_NOTIFY
 #undef MAX_SCATTER_COUNT
 #undef _MORE_INFO_
 #undef _TAB_LAYOUT_

 #define APP_CAPTION			          "GNSS Viewer"
 #define APP_TITLE				          "Internal Use"
 #define GNSS_VIEWER			          1
 #define IS_DEBUG				            1
 #define BAUDRATE_DEFAULT		        7
 #define TIMING_MODE			          1
 #define SHOW_ERROR_NMEA_NOTIFY     1
 #define MAX_SCATTER_COUNT		      1000
 #define _MORE_INFO_			          0		//Please define _MORE_INFO_ in resource preprocessor for rc2.
 #define _TAB_LAYOUT_			          1		//Please define _TAB_LAYOUT_ in resource preprocessor for rc2.

#elif defined(SWCFG_VENDOR_GNSS_ADDTAG)
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef SMALL_UI
 #undef CLIENT_WIDTH
 #undef CLIENT_HEIGHT
 #undef AUTO_QUERY_VERSION

 #define APP_CAPTION			          "GNSS Viewer"
 #define APP_TITLE				          "SANITMAX Release"
 #define GNSS_VIEWER			          1
 #define IS_DEBUG				            0
 #define BAUDRATE_DEFAULT		        7
 #define TIMING_MODE			          1
 #define SMALL_UI			              1
 #define CLIENT_WIDTH			          577
 #define CLIENT_HEIGHT			        341
 #define AUTO_QUERY_VERSION		      1

#elif defined(SWCFG_VENDOR_GNSS_ETEN20160330)	//Request from Leo
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef APP_MODULE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef _MORE_INFO_
 #undef _SHOW_RTK_BASELINE_
 #undef CUSTOMER_ID
 #undef RTK_MENU

 #define APP_CAPTION			          "GNSS Viewer"
 #define APP_TITLE				          "Customer Release"
 #define APP_MODULE				          "Venus 8"
 #define GNSS_VIEWER			          1
 #define IS_DEBUG				            0
 #define BAUDRATE_DEFAULT		        7
 #define TIMING_MODE			          1
 #define _MORE_INFO_			          0		//Please define _MORE_INFO_ for rc2 too.
 #define _SHOW_RTK_BASELINE_	      0		//Please define SHOW_RTK_BASELINE for rc2 too.
 #define CUSTOMER_ID			          Eten
 #define RTK_MENU				            0

#elif defined(SWCFG_VENDOR_GNSS_KALMAN)
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef APP_MODULE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef _MORE_INFO_
 #undef _SHOW_RTK_BASELINE_

 #define APP_CAPTION			          "GNSS Viewer"
 #define APP_TITLE				          "Customer Release"
 #define APP_MODULE				          "Venus 8 RTK"
 #define GNSS_VIEWER			          1
 #define IS_DEBUG				            0
 #define BAUDRATE_DEFAULT		        7
 #define TIMING_MODE			          1
 #define _MORE_INFO_			          1		//Please define _MORE_INFO_ for rc2 too.
 #define _SHOW_RTK_BASELINE_	      1		//Please define SHOW_RTK_BASELINE for rc2 too.

#elif defined(SWCFG_VENDOR_GNSS_SAINTMAX)
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef SMALL_UI
 #undef CLIENT_WIDTH
 #undef CLIENT_HEIGHT
 #undef AUTO_QUERY_VERSION

 #define APP_CAPTION			          "GNSS Viewer"
 #define APP_TITLE				          "SANITMAX Release"
 #define GNSS_VIEWER			          1
 #define IS_DEBUG				            0
 #define BAUDRATE_DEFAULT		        7
 #define TIMING_MODE			          1
 #define SMALL_UI			              1
 #define CLIENT_WIDTH			          577
 #define CLIENT_HEIGHT			        341
 #define AUTO_QUERY_VERSION		      1
 #define SAINTMAX_UI			          1

#elif defined(SWCFG_VENDOR_GNSS_NMEAPLAYER_RTK)	//20151130 request from Oliver
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef APP_MODULE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef NMEA_INPUT
 #undef SHOW_ERROR_NMEA_NOTIFY
 #undef _MORE_INFO_
 #undef CLIENT_WIDTH
 #undef CLIENT_HEIGHT

 #define APP_CAPTION			          "GNSS Viewer"
 #define APP_TITLE				          "NMEA Player"
 #define APP_MODULE				          "Venus 8 RTK"
 #define GNSS_VIEWER			          1
 #define IS_DEBUG				            0
 #define BAUDRATE_DEFAULT		        7
 #define TIMING_MODE			          1
 #define NMEA_INPUT				          1
 #define SHOW_ERROR_NMEA_NOTIFY     1
 #define _MORE_INFO_				        1		//Please define _MORE_INFO_ for rc2 too.
 #define CLIENT_WIDTH			          1008
 #define CLIENT_HEIGHT			        614

#elif defined(SWCFG_VENDOR_GNSS_RTK)	//20151130 request from Oliver
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef APP_MODULE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef _MORE_INFO_

 #define APP_CAPTION			          "GNSS Viewer"
 #define APP_TITLE			            "Customer Release"
 #define APP_MODULE				          "Venus 8 RTK"
 #define GNSS_VIEWER			          1
 #define IS_DEBUG				            0
 #define BAUDRATE_DEFAULT		        7
 #define TIMING_MODE			          1
 #define _MORE_INFO_			          1		//Please define _MORE_INFO_ for rc2 too.

#elif defined(SWCFG_VENDOR_GNSS_TAKUJI)
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef UPGRADE_DOWNLOAD
 #undef UPGRADE_CRC
 #undef UPGRADE_DUEDATE_Y		
 #undef UPGRADE_DUEDATE_M		
 #undef UPGRADE_DUEDATE_D		
 #undef UPGRADE_ADD_TAG
 #undef UPGRADE_TAG_VALUE

 #define APP_CAPTION			          "GNSS Viewer"
 #define APP_TITLE				          "Takuji Release"
 #define GNSS_VIEWER			          1
 #define IS_DEBUG				            0
 #define BAUDRATE_DEFAULT		        7
 #define TIMING_MODE			          1
 #define UPGRADE_DOWNLOAD		        1
 #define UPGRADE_CRC			          0xce72
 #define UPGRADE_DUEDATE_Y		      2015
 #define UPGRADE_DUEDATE_M		      8
 #define UPGRADE_DUEDATE_D		      31
 #define UPGRADE_ADD_TAG		        1
 #define UPGRADE_TAG_VALUE		      0x0A01

#elif defined(SWCFG_VENDOR_GNSS_SWID_NEW_DOWNLOAD)
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef RESOURCE_LOADER_ID

 #define APP_CAPTION			          "GNSS Viewer"
 #define APP_TITLE				          "Customer Release"
 #define GNSS_VIEWER			          1
 #define IS_DEBUG				            0
 #define BAUDRATE_DEFAULT		        7
 #define TIMING_MODE			          1
 #define RESOURCE_LOADER_ID	        1351

#elif defined(SWCFG_VENDOR_GNSS_SUP800_GENERAL)
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef SHOW_ERROR_NMEA_NOTIFY
 #undef MODULE_SUP_800
 #undef MAX_SCATTER_COUNT

 #define APP_CAPTION			          "GNSS Viewer"
 #define APP_TITLE				          "SUP800 Customer Release"
 #define GNSS_VIEWER			          1
 #define IS_DEBUG				            0
 #define BAUDRATE_DEFAULT		        7
 #define TIMING_MODE			          1
 #define SHOW_ERROR_NMEA_NOTIFY     1
 #define MODULE_SUP_800		          1     //Please also define _MODULE_SUP_800_ in resource header
 #define MAX_SCATTER_COUNT          100

#elif defined(SWCFG_VENDOR_GNSS_SUP800)
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef SHOW_ERROR_NMEA_NOTIFY
 #undef MODULE_SUP_800

 #define APP_CAPTION			          "GNSS Viewer"
 #define APP_TITLE				          "SUP800 Tester"
 #define GNSS_VIEWER			          1
 #define IS_DEBUG				            1
 #define BAUDRATE_DEFAULT		        7
 #define TIMING_MODE			          1
 #define SHOW_ERROR_NMEA_NOTIFY     1
 #define MODULE_SUP_800		          1

#elif defined(SWCFG_VENDOR_GNSS_GENERAL_OLINKSTAR_PROTECT)
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef CUSTOMER_DOWNLOAD
 #undef DefaultCuteomer

 #define APP_CAPTION			          "GNSS Viewer"
 #define APP_TITLE				          "OLinkStar Release"
 #define GNSS_VIEWER			          1
 #define IS_DEBUG				            0
 #define BAUDRATE_DEFAULT		        5
 #define TIMING_MODE			          1
 #define CUSTOMER_DOWNLOAD		      1
 #define DefaultCuteomer		        OlinkStar

#elif defined(SWCFG_VENDOR_GNSS_GENERAL_ERICSSON)
//ERICSSON from 重慶西南集成電路設計
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef CUSTOMER_DOWNLOAD
 #undef DefaultCuteomer
 #undef TIMING_MONITORING
 #undef TIMING_OUTPUT_ALIGN
 #undef NMEA_PRN_TYPE

 #define APP_CAPTION			          "GNSS Viewer"
 #define APP_TITLE				          "Ericsson Release"
 #define GNSS_VIEWER			          1
 #define IS_DEBUG				            0
 #define BAUDRATE_DEFAULT		        5
 #define TIMING_MODE			          1
 #define CUSTOMER_DOWNLOAD		      0
 #define DefaultCuteomer		        Ericsson
 #define TIMING_MONITORING		      0
 #define TIMING_OUTPUT_ALIGN	      0
 #define NMEA_PRN_TYPE			        2

#elif defined(SWCFG_VENDOR_GNSS_GENERAL_RDRNSS_PROTECT)
//SWID 重慶西南集成電路設計
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef CUSTOMER_DOWNLOAD
 #undef DefaultCuteomer
 #undef TIMING_MONITORING
 #undef TIMING_OUTPUT_ALIGN

 #define APP_CAPTION			          "GNSS Viewer"
 #define APP_TITLE				          "RDRNSS Release"
 #define GNSS_VIEWER			          1
 #define IS_DEBUG				            0
 #define BAUDRATE_DEFAULT		        5
 #define TIMING_MODE			          1
 #define CUSTOMER_DOWNLOAD		      0
 #define DefaultCuteomer		        SWID
 #define TIMING_MONITORING	        0
 #define TIMING_OUTPUT_ALIGN	      0

#elif defined(SWCFG_VENDOR_GNSS_GENERAL_SWID_PROTECT)
//SWID 重慶西南集成電路設計
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef CUSTOMER_DOWNLOAD
 #undef DefaultCuteomer
 #undef TIMING_MONITORING
 #undef TIMING_OUTPUT_ALIGN

 #define APP_CAPTION			          "GNSS Viewer"
 #define APP_TITLE				          "SWID Release"
 #define GNSS_VIEWER			          1
 #define IS_DEBUG				            0
 #define BAUDRATE_DEFAULT		        5
 #define TIMING_MODE			          1
 #define CUSTOMER_DOWNLOAD		      0
 #define DefaultCuteomer			      SWID
 #define TIMING_MONITORING	        0
 #define TIMING_OUTPUT_ALIGN	      0

#elif defined(SWCFG_VENDOR_GNSS_INTERNALUSE_BDV8)
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef SHOW_ERROR_NMEA_NOTIFY
 #undef MAX_SCATTER_COUNT
 #undef _MORE_INFO_
 #undef _TAB_LAYOUT_

 #define APP_CAPTION			          "GNSS Viewer"
 #define APP_TITLE				          "Internal Use"
 #define GNSS_VIEWER			          1
 #define IS_DEBUG				            1
 #define BAUDRATE_DEFAULT		        7
 #define TIMING_MODE			          1
 #define SHOW_ERROR_NMEA_NOTIFY     1
 #define MAX_SCATTER_COUNT		      1000
 #define _MORE_INFO_			          0		//Please define _MORE_INFO_ in resource preprocessor for rc2.
 #define _TAB_LAYOUT_			          1		//Please define _TAB_LAYOUT_ in resource preprocessor for rc2.

#elif defined(SWCFG_VENDOR_GNSS_INTERNALUSE_GPS_183)
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef SHOW_ERROR_NMEA_NOTIFY
 #undef NMEA_PRN_TYPE

 #define APP_CAPTION			          "GNSS Viewer"
 #define APP_TITLE				          "Internal Use GPS 183-188"
 #define GNSS_VIEWER			          1
 #define IS_DEBUG				            1
 #define BAUDRATE_DEFAULT		        7
 #define TIMING_MODE			          1
 #define SHOW_ERROR_NMEA_NOTIFY     1
 #define NMEA_PRN_TYPE			        3

#elif defined(SWCFG_VENDOR_GNSS_BDV8)
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE

 #define APP_CAPTION			          "GNSS Viewer"
 #define APP_TITLE				          "Customer Release"
 #define GNSS_VIEWER			          1
 #define IS_DEBUG				            0
 #define BAUDRATE_DEFAULT		        7
 #define TIMING_MODE			          1

#elif defined(SWCFG_VENDOR_GNSS_FKHL)
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef NMEA_PRN_TYPE

 #define APP_CAPTION			          "GNSS Viewer"
 #define APP_TITLE				          "Customer Release FKHL"
 #define GNSS_VIEWER			          1
 #define IS_DEBUG				            0
 #define BAUDRATE_DEFAULT		        7
 #define TIMING_MODE			          1
 #define NMEA_PRN_TYPE			        1

#elif defined(SWCFG_VENDOR_GNSS_FKHL_Test)
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef NMEA_PRN_TYPE
 #undef NMEA_INPUT

 #define APP_CAPTION			          "GNSS Viewer"
 #define APP_TITLE				          "Customer Release FKHL"
 #define GNSS_VIEWER			          1
 #define IS_DEBUG				            0
 #define BAUDRATE_DEFAULT		        7
 #define TIMING_MODE			          1
 #define NMEA_PRN_TYPE			        1
 #define NMEA_INPUT				          1

#elif defined(SWCFG_VENDOR_GNSS_BDV8_NMEAPLAYER)
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef NMEA_INPUT
 #undef SHOW_ERROR_NMEA_NOTIFY
 #undef CLIENT_WIDTH
 #undef CLIENT_HEIGHT

 #define APP_CAPTION			          "GNSS Viewer"
 #define APP_TITLE				          "NMEA Player"
 #define GNSS_VIEWER			          1
 #define IS_DEBUG				            0
 #define BAUDRATE_DEFAULT		        7
 #define TIMING_MODE			          1
 #define NMEA_INPUT				          1
 #define SHOW_ERROR_NMEA_NOTIFY     1
 #define CLIENT_WIDTH			          1008	
 #define CLIENT_HEIGHT			        614

#elif defined(SWCFG_VENDOR_GNSS_V8)
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE

 #define APP_CAPTION			          "GNSS Viewer"
 #define APP_TITLE				          "Internal Use"
 #define GNSS_VIEWER			          1
 #define IS_DEBUG				            1
 #define BAUDRATE_DEFAULT		        6
 #define TIMING_MODE			          1

#elif defined(SWCFG_VENDOR_CREATETAG)
 #undef APP_CAPTION
 #undef APP_TITLE
 #undef GNSS_VIEWER
 #undef IS_DEBUG
 #undef BAUDRATE_DEFAULT
 #undef TIMING_MODE
 #undef _CREATE_LICENSE_TAG_

 #define APP_CAPTION			          "GNSS Viewer"
 #define APP_TITLE				          "Create Tag"
 #define GNSS_VIEWER				        1
 #define IS_DEBUG					          0
 #define BAUDRATE_DEFAULT			      7
 #define TIMING_MODE				        1
 #define _CREATE_LICENSE_TAG_		    1

#elif defined(SWCFG_VENDOR_GENERAL_DR)
 #undef APP_TITLE				
 #undef BAUDRATE_DEFAULT
 #undef SOFTWARE_FUNCTION

 #define APP_TITLE				          "Customer Release DR"
 #define BAUDRATE_DEFAULT		        5
 #define SOFTWARE_FUNCTION		      (SW_FUN_DATALOG | SW_FUN_AGPS | SW_FUN_DR)

#else
 #error Unknow SWCFG_VENDER_TYPE!!!
#endif
