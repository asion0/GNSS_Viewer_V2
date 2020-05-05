#pragma once

// CCommonConfigDlg
class CCommonConfigDlg : public CDialog
{
	DECLARE_DYNAMIC(CCommonConfigDlg)

public:
	CCommonConfigDlg(UINT nIDTemplate, CWnd* pParent = NULL);
	virtual ~CCommonConfigDlg();

	virtual void DoCommand() = 0;
	virtual INT_PTR DoDirect(int type);
	virtual BOOL OnInitDialog();
protected:
	CButton *pCancelBtn;
	CButton *pAcceptBtn;

	DECLARE_MESSAGE_MAP()
};

// CConfigDGPS
class CConfigDGPS : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CConfigDGPS)
public:
	CConfigDGPS(CWnd* pParent = NULL);   
	virtual ~CConfigDGPS() {};

	virtual void DoCommand();

	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();

protected:
	BOOL m_bEnable;
	int m_nOverdueSeconds;
	int m_nAttribute;	
	
	DECLARE_MESSAGE_MAP()
};

// CConfigSmoothMode
class CConfigSmoothMode : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CConfigSmoothMode)
public:
	CConfigSmoothMode(CWnd* pParent = NULL);   
	virtual ~CConfigSmoothMode() {};

	virtual void DoCommand();

	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();

protected:
	BOOL m_bEnable;
	int m_nAttribute;	
	
	DECLARE_MESSAGE_MAP()
};

// CConfigTimeStamping
class CConfigTimeStamping : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CConfigTimeStamping)
public:
	CConfigTimeStamping(CWnd* pParent = NULL);   
	virtual ~CConfigTimeStamping() {};

	virtual void DoCommand();

	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();

protected:
	BOOL m_bEnable;
	BOOL m_bTiggerMode;
	int m_nAttribute;	
	
	DECLARE_MESSAGE_MAP()
};

// CConfigSBAS
class CConfigSBAS : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CConfigSBAS)
public:
	CConfigSBAS(CWnd* pParent = NULL);   
	virtual ~CConfigSBAS() {};

	virtual void DoCommand();

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedEnableWaas();
	afx_msg void OnBnClickedEnableEgnos();
	afx_msg void OnBnClickedEnableMsas();
	afx_msg void OnBnClickedEnableGagan();
	afx_msg void OnBnClickedEnableAll();

	virtual BOOL OnInitDialog();

protected:
	BOOL m_bEnable;
	BOOL m_bRanging;
	BOOL m_bCorrection;
	int m_nUraMask;
	int m_nTrackingChannel;
	BOOL m_bWAAS;
	BOOL m_bEGNOS;
	BOOL m_bMSAS;
	BOOL m_bGAGAN;
	BOOL m_bAll;
	int m_nAttribute;

	DECLARE_MESSAGE_MAP()
};

// CConfigSBAS2
class CConfigSBAS2 : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CConfigSBAS2)
public:
	CConfigSBAS2(CWnd* pParent = NULL);   
	virtual ~CConfigSBAS2() {};

 enum CommandMode
  {
    ConfigSBAS2Old,
    ConfigSBAS2New,
  };

	virtual void DoCommand();

protected:  //members
	BOOL m_bEnable;
	BOOL m_bRanging;
	BOOL m_bCorrection;
	int m_nUraMask;
	int m_nTrackingChannel;
	BOOL m_bWAAS;
	BOOL m_bEGNOS;
	BOOL m_bMSAS;
	BOOL m_bGAGAN;
	BOOL m_bAll;
	int m_nAttribute;
  CommandMode cmdMode;

//#if(!NEW_SBAS2)
  BOOL m_bWaasSpec;
	BOOL m_bEgnosSpec;
	BOOL m_bMsasSpec;
	BOOL m_bGaganSpec;
//#endif

  U08 m_u11, m_u12, m_u13;
  U08 m_u21, m_u22, m_u23;
  U08 m_u31, m_u32, m_u33;
  U08 m_u41, m_u42, m_u43;
protected:  //functions
  void UpdateStatus();
  int GetUserTaqble(U08& u1, U08& u2, U08& u3, UINT id1,  UINT id2, UINT id3);

	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedEnableWaas();
	afx_msg void OnBnClickedEnableEgnos();
	afx_msg void OnBnClickedEnableMsas();
	afx_msg void OnBnClickedEnableGagan();
	afx_msg void OnBnClickedEnableAll();
	afx_msg void OnBnClickedWaasSpecify();
	afx_msg void OnBnClickedEgnosSpecify();
	afx_msg void OnBnClickedMsasSpecify();
	afx_msg void OnBnClickedGaganSpecify();
	afx_msg void OnBnClickedApplyDefault();

	virtual BOOL OnInitDialog();

	DECLARE_MESSAGE_MAP()
};

// CConfigSAGPS
class CConfigSAEE : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CConfigSAEE)
public:
	CConfigSAEE(CWnd* pParent = NULL);   
	virtual ~CConfigSAEE() {};

	virtual void DoCommand();

	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();

protected:
	int m_nEnable;
	int m_nAttribute;

	DECLARE_MESSAGE_MAP()
};

// CConfigQZSS
class CConfigQZSS : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CConfigQZSS)
public:
	CConfigQZSS(CWnd* pParent = NULL);   
	virtual ~CConfigQZSS() {};

	virtual void DoCommand();

	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();

protected:
	BOOL m_bEnable;
	int m_nTrackingChannel;
	int m_nAttribute;

	DECLARE_MESSAGE_MAP()
};

// CConfigInterferenceDetectControl
class CConfigInterferenceDetectControl : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CConfigInterferenceDetectControl)
public:
	CConfigInterferenceDetectControl(CWnd* pParent = NULL);   
	virtual ~CConfigInterferenceDetectControl() {};

	virtual void DoCommand();

	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();

protected:
	BOOL m_nMode;
	int m_nAttribute;

	DECLARE_MESSAGE_MAP()
};

// CConfigNMEABinaryOutputDestination
class CConfigNMEABinaryOutputDestination : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CConfigNMEABinaryOutputDestination)
public:
	CConfigNMEABinaryOutputDestination(CWnd* pParent = NULL);   
	virtual ~CConfigNMEABinaryOutputDestination() {};

	virtual void DoCommand();

	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();

protected:
	BOOL m_nMode;
	int m_nAttribute;

	DECLARE_MESSAGE_MAP()
};

// CConfigParameterSearchEngineNumber
class CConfigParameterSearchEngineNumber : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CConfigParameterSearchEngineNumber)
public:
	CConfigParameterSearchEngineNumber(CWnd* pParent = NULL);   
	virtual ~CConfigParameterSearchEngineNumber() {};

	virtual void DoCommand();

	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	afx_msg void OnCbnSelchangeMode();

protected:
	BOOL m_nMode;
	int m_nAttribute;

	DECLARE_MESSAGE_MAP()
};

// CConfigPositionFixNavigationMask
class CConfigPositionFixNavigationMask : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CConfigPositionFixNavigationMask)
public:
	CConfigPositionFixNavigationMask(CWnd* pParent = NULL);   
	virtual ~CConfigPositionFixNavigationMask() {};

	virtual void DoCommand();

	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
protected:
	int m_nMask1;
	int m_nMask2;
	int m_nAttribute;

	DECLARE_MESSAGE_MAP()
};

// ConfigRefTimeToGpsTimeDlg
class ConfigRefTimeToGpsTimeDlg : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(ConfigRefTimeToGpsTimeDlg)
public:
	ConfigRefTimeToGpsTimeDlg(CWnd* pParent = NULL);   
	virtual ~ConfigRefTimeToGpsTimeDlg() {};

	virtual void DoCommand();

	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();

protected:
	BOOL m_isEnable;
	int m_nYear;
	int m_nMonth;
	int m_nDay;
	int m_attribute;

	DECLARE_MESSAGE_MAP()
};

// ConfigGnssConstellationTypeDlg
class ConfigGnssConstellationTypeDlg : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(ConfigGnssConstellationTypeDlg)
public:
	ConfigGnssConstellationTypeDlg(CWnd* pParent = NULL);   
	virtual ~ConfigGnssConstellationTypeDlg() {};

	virtual void DoCommand();

	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();

protected:
	U16 m_mode;
	U08 m_attribute;

	DECLARE_MESSAGE_MAP()
};

// ConfigBinaryMeasurementDataOutDlg
class ConfigBinaryMeasurementDataOutDlg : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(ConfigBinaryMeasurementDataOutDlg)
public:
	ConfigBinaryMeasurementDataOutDlg(CWnd* pParent = NULL);   
	virtual ~ConfigBinaryMeasurementDataOutDlg() {};

	virtual void DoCommand();
	virtual BOOL OnInitDialog();

  afx_msg void OnBnClickedMeasTime();
  afx_msg void OnBnClickedRawMeas();
  afx_msg void OnBnClickedExtRawMeas();
	afx_msg void OnBnClickedOk();
protected:
	U08 m_rate;
	U08 m_measTime;
	U08 m_rawMeas;
	U08 m_svChStatus;
	U08 m_rcvChStatus;
	U08 m_extRawMeas;
	U08 m_subFrame;
	U08 m_attribute;

	BOOL m_newCmd;
	void UpdateStatus();
  BOOL CheckLegal();

	DECLARE_MESSAGE_MAP()
};

// CConfigLeapSeconds 
class CConfigLeapSeconds : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CConfigLeapSeconds)
public:
	CConfigLeapSeconds(CWnd* pParent = NULL);   
	virtual ~CConfigLeapSeconds() {};

	virtual void DoCommand();

	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();

protected:
	U08 m_nLeapSeconds;
	int m_nAttribute;	
	
	DECLARE_MESSAGE_MAP()
};

// CConfigPowerMode 

class CConfigPowerMode : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CConfigPowerMode)
public:
	CConfigPowerMode(CWnd* pParent = NULL);   
	virtual ~CConfigPowerMode() {};

	virtual void DoCommand();

	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();

protected:
	U16 m_nPowerMode;
	int m_nAttribute;	
	
	DECLARE_MESSAGE_MAP()
};

// CConfigParamSearchEngineSleepCRiteria
class CConfigParamSearchEngineSleepCriteria : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CConfigParamSearchEngineSleepCriteria)
public:
	CConfigParamSearchEngineSleepCriteria(CWnd* pParent = NULL);   
	virtual ~CConfigParamSearchEngineSleepCriteria() {};

	virtual void DoCommand();

	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();

protected:
	U08 m_nTrackedNumber;
	int m_nAttribute;	
	
	DECLARE_MESSAGE_MAP()
};

// CConfigDatumIndex
class CConfigDatumIndex : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CConfigDatumIndex)
public:
	CConfigDatumIndex(CWnd* pParent = NULL);   
	virtual ~CConfigDatumIndex() {};

	virtual void DoCommand();

	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();

protected:
	U16 m_nDatumIndex;
	int m_nAttribute;	
	
	DECLARE_MESSAGE_MAP()
};

// CSUP800EraseUserDataDlg
class CSUP800EraseUserDataDlg : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CSUP800EraseUserDataDlg)
public:
	CSUP800EraseUserDataDlg(CWnd* pParent = NULL);   
	virtual ~CSUP800EraseUserDataDlg() {};

	virtual void DoCommand();

	afx_msg void OnBnClickedErase();
	virtual BOOL OnInitDialog();

protected:
	U08 m_nSector;
	
	DECLARE_MESSAGE_MAP()
};


// CSUP800WriteUserDataDlg
class CSUP800WriteUserDataDlg : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CSUP800WriteUserDataDlg)
public:
	CSUP800WriteUserDataDlg(CWnd* pParent = NULL);   
	virtual ~CSUP800WriteUserDataDlg() {};

	virtual void DoCommand();

	afx_msg void OnEnChangeInput();
	afx_msg void OnBnClickedWrite();
	afx_msg void OnBnClickedLoad();
	virtual BOOL OnInitDialog();

protected:
	U08 m_nSector;
	UINT16 m_nOffset;
	BinaryData m_binData;

	DECLARE_MESSAGE_MAP()
};


// CSUP800ReadUserDataDlg
class CSUP800ReadUserDataDlg : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CSUP800ReadUserDataDlg)
public:
	CSUP800ReadUserDataDlg(CWnd* pParent = NULL);   
	virtual ~CSUP800ReadUserDataDlg() {};

	virtual void DoCommand();

	afx_msg void OnEnChangeInput();
	afx_msg void OnBnClickedRead();
	afx_msg void OnBnClickedSave();
	virtual BOOL OnInitDialog();

protected:
	U08 m_nSector;
	U16 m_nOffset;
	U16 m_nDataSize;

	void ReadUserData();
	
	DECLARE_MESSAGE_MAP()
};

// CConfigureSignalDisturbanceStatusDlg 
class CConfigureSignalDisturbanceStatusDlg : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CConfigureSignalDisturbanceStatusDlg)
public:
	CConfigureSignalDisturbanceStatusDlg(CWnd* pParent = NULL);   
	virtual ~CConfigureSignalDisturbanceStatusDlg() {};

	virtual void DoCommand();

	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();

protected:
	U08 m_nOperationType;

	DECLARE_MESSAGE_MAP()
};

// CConfigureGpsUtcLeapSecondsInUtcDlg
class CConfigureGpsUtcLeapSecondsInUtcDlg : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CConfigureGpsUtcLeapSecondsInUtcDlg)
public:
	CConfigureGpsUtcLeapSecondsInUtcDlg(CWnd* pParent = NULL);   
	virtual ~CConfigureGpsUtcLeapSecondsInUtcDlg() {};

	virtual void DoCommand();

	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();

protected:
	U16 m_nYear;
	U08 m_nMonth;
	S08 m_nLeapSeconds;
	S08 m_nInsertSecond;
	U08 m_attribute;

	DECLARE_MESSAGE_MAP()
};

// CConfigNoisePowerControlDlg
class CConfigNoisePowerControlDlg : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CConfigNoisePowerControlDlg)
public:
	CConfigNoisePowerControlDlg(CWnd* pParent = NULL);   
	virtual ~CConfigNoisePowerControlDlg() {};

	virtual void DoCommand();
	virtual BOOL OnInitDialog();

	afx_msg void OnCbnSelchangeMode();
	afx_msg void OnBnClickedOk();

protected:
	U08 m_nMode;
	U08 m_nDefault;
	U32 m_nValue;
	U08 m_attribute;

	DECLARE_MESSAGE_MAP()
};

// ConfigPowerSavingParametersRomDlg
class ConfigPowerSavingParametersRomDlg : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(ConfigPowerSavingParametersRomDlg)
public:
	ConfigPowerSavingParametersRomDlg(CWnd* pParent = NULL);   
	virtual ~ConfigPowerSavingParametersRomDlg() {};

	virtual void DoCommand();

	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	void SetRomMode(bool b) { m_bRomMode = b; }

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 

	bool m_bRomMode;
	S16 m_param[9];
	U08 m_attribute;

	DECLARE_MESSAGE_MAP()
};

// CIqPlot 
class CIqPlot : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CIqPlot)
public:
	CIqPlot(CWnd* pParent = NULL);   
	virtual ~CIqPlot() {};

	virtual void DoCommand();
	afx_msg void OnBnClickedOk();
	afx_msg void OnCbnSelChangeGnssType();

	virtual BOOL OnInitDialog();

protected:
	static U08 m_gnssType;
	static U08 m_gnssChannel;
	static U16 m_nmeaSvid;
	static U08 m_rate;
	static BOOL m_bEnable;

  afx_msg void GetGnssTypeAndChannelFromCombo();
  afx_msg void CIqPlot::OnCbnSelChangeGnssChannel();
  void SetComboFromGnssTypeAndChannel(bool setType, bool setChannel);

	DECLARE_MESSAGE_MAP()
};

// CConfigGeofencing
class CConfigGeofencing : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CConfigGeofencing)
public:
	CConfigGeofencing(CWnd* pParent = NULL);   
	virtual ~CConfigGeofencing() {};

	virtual void DoCommand();
	virtual BOOL OnInitDialog();
	void SetDataNo(int n) { m_no = n; }
protected:
	struct Point
	{
		double lon;
		double lat;
	};
	CListBox m_points;	
	vector<double> lons;
	vector<double> lats;
	U08 m_attribute;
	U08 m_no;

	bool AddPoint(const CString s);

	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedAddPoint();
	afx_msg void OnBnClickedAddPoints();
	afx_msg void OnBnClickedClearAll();
	afx_msg void OnBnClickedCopyResult();
	DECLARE_MESSAGE_MAP()
};

// CConfigRtkMode 
class CConfigRtkMode : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CConfigRtkMode)
public:
	CConfigRtkMode(CWnd* pParent = NULL);   
	virtual ~CConfigRtkMode() {};

	virtual void DoCommand();
	afx_msg void OnBnClickedOk();

	virtual BOOL OnInitDialog();

protected:
	U08 m_mode;
	U08 m_attribute;

	DECLARE_MESSAGE_MAP()
};

// CConfigRtkMode
class CConfigRtkParameters : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CConfigRtkParameters)
public:
	CConfigRtkParameters(CWnd* pParent = NULL);   
	virtual ~CConfigRtkParameters() {};

	virtual void DoCommand();
	afx_msg void OnBnClickedOk();

	virtual BOOL OnInitDialog();

protected:
	U16 m_param01;
	U16 m_param02;
	U16 m_param03;
	U16 m_param04;
	U32 m_param05;
	U32 m_param06;
	U32 m_param07;
	U32 m_param08;
	U08 m_param09;
	U08 m_param10;
	U08 m_attribute;

	DECLARE_MESSAGE_MAP()
};

// CConfigRtkReset
class CConfigRtkReset : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CConfigRtkReset)
public:
	CConfigRtkReset(CWnd* pParent = NULL);   
	virtual ~CConfigRtkReset() {};

	virtual void DoCommand();
	afx_msg void OnBnClickedOk();

	virtual BOOL OnInitDialog();

protected:
	U16 m_mode;

	DECLARE_MESSAGE_MAP()
};

// CConfigRtkMode2 
class CConfigRtkMode2 : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CConfigRtkMode2)
public:
	CConfigRtkMode2(CWnd* pParent = NULL);
	virtual ~CConfigRtkMode2() {};

  enum CommandMode
  {
    CfgRtkOprtFctnOld,
    CfgRtkOprtFctn,
    CfgBasePosition,
    CfgTiming
  };

	virtual void DoCommand();
	afx_msg void OnBnClickedOk();
	afx_msg void OnCbnSelChangeRtkMode();
	afx_msg void OnCbnSelChangeBaseOpt();
	afx_msg void OnCbnSelChangeRoverOpt();

	virtual BOOL OnInitDialog();
  void SetCommandMode(CommandMode m) { cmdMode = m; };
protected:
	U08 m_rtkMode;
	U08 m_baseOpt;
	U08 m_roverOpt;
	U08 m_pkbOpt;
	U32 m_srvValue1;
	U32 m_srvValue2;
	D64 m_sttValue1;
	D64 m_sttValue2;
	F32 m_sttValue3;
	F32 m_mvbLength;
	U08 m_attribute;
	CommandMode cmdMode;
  void AdjustUI();
	void UpdateStatus();
	DECLARE_MESSAGE_MAP()
};

// CConfigMessageOut 
class CConfigMessageOut : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CConfigMessageOut)
public:
	CConfigMessageOut(CWnd* pParent = NULL);   
	virtual ~CConfigMessageOut() {};

	virtual void DoCommand();
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	virtual INT_PTR DoDirect(int type);

protected:
	U08 m_nType;
	U08 m_nAttribute;

	DECLARE_MESSAGE_MAP()
};

// CConfigSubSecRegister
class CConfigSubSecRegister : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CConfigSubSecRegister)
public:
	CConfigSubSecRegister(CWnd* pParent = NULL);   
	virtual ~CConfigSubSecRegister() {};

	virtual void DoCommand();
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();

protected:
	U08 m_nMs;
	U08 m_nNs;
	U08 m_nPllDiv;

	DECLARE_MESSAGE_MAP()
};
/*
// CConfigTiming 
class CConfigTiming : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CConfigTiming)
public:
	CConfigTiming(CWnd* pParent = NULL);   
	virtual ~CConfigTiming() {};

	virtual void DoCommand();
	afx_msg void OnBnClickedOk();
	afx_msg void OnCbnSelChangeMode();

	virtual BOOL OnInitDialog();

protected:
	U08 m_timingMode;

	U32 m_srvValue1;
	U32 m_srvValue2;
	double m_sttValue1;
	double m_sttValue2;
	float m_sttValue3;
	U08 m_attribute;

	void UpdateStatus();

	DECLARE_MESSAGE_MAP()
};
*/
// CConfigTimingCableDelay
class CConfigTimingCableDelay : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CConfigTimingCableDelay)
public:
	CConfigTimingCableDelay(CWnd* pParent = NULL);   
	virtual ~CConfigTimingCableDelay() {};

	virtual void DoCommand();
	afx_msg void OnBnClickedOk();

	virtual BOOL OnInitDialog();

protected:
	U32 m_delay;
	U08 m_attribute;

	DECLARE_MESSAGE_MAP()
};

// CConfigGpsMeasurementMode 
class CConfigGpsMeasurementMode : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CConfigGpsMeasurementMode)
public:
	CConfigGpsMeasurementMode(CWnd* pParent = NULL);   
	virtual ~CConfigGpsMeasurementMode() {};

	virtual void DoCommand();
	afx_msg void OnBnClickedOk();

	virtual BOOL OnInitDialog();

protected:
	U08 m_mode;
	U08 m_attribute;

	DECLARE_MESSAGE_MAP()
};

// CConfigPscmDeviceAddress 
class CConfigPscmDeviceAddress : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CConfigPscmDeviceAddress)
public:
	CConfigPscmDeviceAddress(CWnd* pParent = NULL);   
	virtual ~CConfigPscmDeviceAddress() {};

	virtual void DoCommand();
	afx_msg void OnBnClickedOk();

	virtual BOOL OnInitDialog();

protected:
	U08 m_num;

	DECLARE_MESSAGE_MAP()
};

// CConfigPscmLatLonFractionalDigits
class CConfigPscmLatLonFractionalDigits : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CConfigPscmLatLonFractionalDigits)
public:
	CConfigPscmLatLonFractionalDigits(CWnd* pParent = NULL);   
	virtual ~CConfigPscmLatLonFractionalDigits() {};

	virtual void DoCommand();
	afx_msg void OnBnClickedOk();

	virtual BOOL OnInitDialog();

protected:
	U08 m_num;

	DECLARE_MESSAGE_MAP()
};

// CConfigVeryLowSpeed 
class CConfigVeryLowSpeed : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CConfigVeryLowSpeed)
public:
	CConfigVeryLowSpeed(CWnd* pParent = NULL);   
	virtual ~CConfigVeryLowSpeed() {};

	virtual void DoCommand();

	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();

protected:
	int m_nEnable;
	int m_nAttribute;

	DECLARE_MESSAGE_MAP()
};

// CConfigDofunUniqueId 
class CConfigDofunUniqueId : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CConfigDofunUniqueId)
public:
	CConfigDofunUniqueId(CWnd* pParent = NULL);   
	virtual ~CConfigDofunUniqueId() {};

	virtual void DoCommand();

	afx_msg void OnEnChangeInput();
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	virtual INT_PTR DoDirect(int type);

	enum { UniqueIdLength = 16 };
protected:
	//U08 m_uniqueId[UniqueIdLength];
	BinaryData binData;
	int m_nMode;

	DECLARE_MESSAGE_MAP()
};

// CConfigPstiInterval
class CConfigPstiInterval : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CConfigPstiInterval)
public:
	CConfigPstiInterval(CWnd* pParent = NULL);   
	virtual ~CConfigPstiInterval() {};

	virtual void DoCommand();

	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();

  enum MessageType
  {
    Psti,
    Navic,
  };
  void SetPsti(int id, MessageType t = Psti) { m_nType = t; m_nPstiId = id; }

protected:
  void AdjustValue(int nPos, CScrollBar* pScrollBar);

  MessageType m_nType;
	int m_nPstiId;
	int m_nPstiInterval;
	int m_nAttribute;

	DECLARE_MESSAGE_MAP()
};

// CRtkOnOffSv 
class CRtkOnOffSv : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CRtkOnOffSv)
public:
	CRtkOnOffSv(CWnd* pParent = NULL);
	virtual ~CRtkOnOffSv() {};
  enum SvMode 
  {
    RtkSvGps,
    RtkSvSbasQzss,
    RtkSvGlonass,
    RtkSvBeidou2,
    RtkSvGalileo
  };
  void SetSvMode(SvMode s) { m_svMode = s; };

	virtual void DoCommand();
	afx_msg void OnBnClickedCheckAll();
	afx_msg void OnBnClickedUncheckAll();
	afx_msg void OnBnClickedRead();
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();

protected:
  const static UINT svId[];
	U08 m_svMode;
	U32 m_svReadAddr;
	U32 m_svWriteAddr;
	U32 m_svData;

	void UpdateStatus();

	DECLARE_MESSAGE_MAP()
};

// CConfigRtkReferencePosition 
class CConfigRtkReferencePosition : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CConfigRtkReferencePosition)
public:
	CConfigRtkReferencePosition(CWnd* pParent = NULL);   
	virtual ~CConfigRtkReferencePosition() {};

	virtual void DoCommand();
	afx_msg void OnBnClickedOk();

	virtual BOOL OnInitDialog();

protected:
	double m_sttValue1;
	double m_sttValue2;
	float m_sttValue3;

	DECLARE_MESSAGE_MAP()
};

// ConfigRtcmMeasurementDataOutDlg
class ConfigRtcmMeasurementDataOutDlg : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(ConfigRtcmMeasurementDataOutDlg)
public:
	ConfigRtcmMeasurementDataOutDlg(CWnd* pParent = NULL);   
	virtual ~ConfigRtcmMeasurementDataOutDlg() {};

	virtual void DoCommand();
	virtual BOOL OnInitDialog();

  afx_msg void OnBnClickedField2();
	afx_msg void OnBnClickedOk();
protected:
	U08 m_field2;
	U08 m_field3;
	U08 m_field4;
	U08 m_field5;
	U08 m_field6;
	U08 m_field7;
	U08 m_field8;
	U08 m_field9;
	U08 m_field10;
	U08 m_attribute;
  U16 cnstMode;
	void UpdateStatus();

	DECLARE_MESSAGE_MAP()
};

// CConfigWatchTrackback
class CConfigWatchTrackback : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CConfigWatchTrackback)
public:
	CConfigWatchTrackback(CWnd* pParent = NULL);   
	virtual ~CConfigWatchTrackback() {};

	virtual void DoCommand();
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedOk();
protected:
//	U08 m_attribute;
//  U16 cnstMode;
//	void UpdateStatus();

	DECLARE_MESSAGE_MAP()
};

// CDataLogConfigureDlg
class CDataLogConfigureDlg : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CDataLogConfigureDlg)
public:
  enum CmdType
  {
    Cmd17h,
    Cmd6448h,
    Cmd7A0C0Dh,
  };

	CDataLogConfigureDlg(CWnd* pParent = NULL, CmdType cmd = Cmd17h);   
	virtual ~CDataLogConfigureDlg() {};

	virtual void DoCommand();
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedOk();
  afx_msg void OnBnClickedEnable();
protected:
  CmdType m_cmd;
  U08 m_enable;
	S32 m_maxT;
  S32 m_minT;
	S32 m_maxD;
	S32 m_minD;
	S32 m_maxV;
	S32 m_minV;
  U08 m_fifoMode;

	void UpdateStatus();

	DECLARE_MESSAGE_MAP()
};

// CPositionUpdateRateDlg
class CPositionUpdateRateDlg : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CPositionUpdateRateDlg)
public:
 	CPositionUpdateRateDlg(CWnd* pParent = NULL);   
	virtual ~CPositionUpdateRateDlg() {};

	virtual void DoCommand();
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedOk();
protected:
  U08 m_rate;
	U08 m_attribute;

	DECLARE_MESSAGE_MAP()
};

// CDrUpdateRateDlg
class CDrUpdateRateDlg : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CDrUpdateRateDlg)
public:
 	CDrUpdateRateDlg(CWnd* pParent = NULL);   
	virtual ~CDrUpdateRateDlg() {};

	virtual void DoCommand();
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedOk();
protected:
  U08 m_rate;
	U08 m_attribute;

	DECLARE_MESSAGE_MAP()
};

// CDrRawRateDlg
class CDrRawRateDlg : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CDrRawRateDlg)
public:
 	CDrRawRateDlg(CWnd* pParent = NULL);   
	virtual ~CDrRawRateDlg() {};

	virtual void DoCommand();
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedOk();
protected:
  U08 m_rate;
	U08 m_attribute;

	DECLARE_MESSAGE_MAP()
};

// CDrMemsNoiseLevelDlg
class CDrMemsNoiseLevelDlg : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CDrMemsNoiseLevelDlg)
public:
 	CDrMemsNoiseLevelDlg(CWnd* pParent = NULL);   
	virtual ~CDrMemsNoiseLevelDlg() {};

	virtual void DoCommand();
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedOk();
protected:
  D64 m_accNoise;
	D64 m_gyroNoise;

	DECLARE_MESSAGE_MAP()
};

// CAdrOdometerScalingFactorDlg
class CAdrOdometerScalingFactorDlg : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CAdrOdometerScalingFactorDlg)
public:
 	CAdrOdometerScalingFactorDlg(CWnd* pParent = NULL);   
	virtual ~CAdrOdometerScalingFactorDlg() {};

	virtual void DoCommand();
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedOk();
protected:
  F32 m_scalingFactor;

	DECLARE_MESSAGE_MAP()
};

// CConfigRtkSlaveBaud
class CConfigRtkSlaveBaud : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CConfigRtkSlaveBaud)
public:
	CConfigRtkSlaveBaud(CWnd* pParent = NULL);   
	virtual ~CConfigRtkSlaveBaud() {};

	virtual void DoCommand();
	afx_msg void OnBnClickedOk();

	virtual BOOL OnInitDialog();

protected:
	int m_rate;
  int m_nAttribute;

	DECLARE_MESSAGE_MAP()
};

// C1ppsOutputModeDlg
class C1ppsOutputModeDlg : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(C1ppsOutputModeDlg)
public:
 	C1ppsOutputModeDlg(CWnd* pParent = NULL);   
	virtual ~C1ppsOutputModeDlg() {};

	virtual void DoCommand();
	virtual BOOL OnInitDialog();

	afx_msg void OnBnClickedNoOutput();
	afx_msg void OnBnClickedOutputHaveGpsTime();
	afx_msg void OnBnClickedOutputAlign();
	afx_msg void OnBnClickedAlignToGps();
	afx_msg void OnBnClickedAlignToUtc();
	afx_msg void OnBnClickedAlignToNavic();
	afx_msg void OnBnClickedOk();
protected:
  U08 m_mode;
  U08 m_align;
	U08 m_attribute;

	DECLARE_MESSAGE_MAP()
};

// CConfigureRfIc 
/*
class CConfigureRfIc : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CConfigureRfIc)
public:
	CConfigureRfIc(CWnd* pParent = NULL);
	virtual ~CConfigureRfIc() {};

	virtual void DoCommand();
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();

protected:
	U08 m_type;
	U32 m_reg[Setting::RFIC_REG_SIZE];
  static UINT staticCtrlList[Setting::RFIC_REG_SIZE];
  static UINT editCtrlList[Setting::RFIC_REG_SIZE];

	afx_msg void OnCbnSelChangeRtkMode();

	DECLARE_MESSAGE_MAP()
};
*/
// CConfigRtkGlCpifBias
class CConfigRtkGlCpifBias : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CConfigRtkGlCpifBias)
public:
	CConfigRtkGlCpifBias(CWnd* pParent = NULL);   
	virtual ~CConfigRtkGlCpifBias() {};

	virtual void DoCommand();
	afx_msg void OnBnClickedOk();
	afx_msg void OnCbnSelchangeMode();
  afx_msg void OnBnClickedMask();
	virtual BOOL OnInitDialog();

protected:
	U08 m_mode;
	U08 m_freqMask;
	D64 m_l1Cifb;
	D64 m_l2Cifb;
	U08 m_rev01;
	U32 m_rev02;
  U08 m_attribute;
	
  void UpdateStatus();
	DECLARE_MESSAGE_MAP()
};

// CConfigCpuBoostMode 
class CConfigCpuBoostMode : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CConfigCpuBoostMode)
public:
	CConfigCpuBoostMode(CWnd* pParent = NULL);   
	virtual ~CConfigCpuBoostMode() {};

	virtual void DoCommand();

	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();

protected:
	int m_nEnable;
	int m_nAttribute;

	DECLARE_MESSAGE_MAP()
};

// CConfigureAlphaKeyDlg 
class CConfigureAlphaKeyDlg : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CConfigureAlphaKeyDlg)
public:
	CConfigureAlphaKeyDlg(CWnd* pParent = NULL);   
	virtual ~CConfigureAlphaKeyDlg() {};

  enum CommandType
  {
    AlphaKey,
    V9AesTag
  };
  
  void SetCommandType(CommandType t) { m_cmdType = t; }

	virtual void DoCommand();

	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();

  enum { KeyLength = 16 };

protected:
  CommandType m_cmdType;
	afx_msg void OnEnChangeKey();

	BinaryData m_keyData;

	DECLARE_MESSAGE_MAP()
};

// CDumpMemoryDlg 
class CDumpMemoryDlg : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CDumpMemoryDlg)
public:
	CDumpMemoryDlg(CWnd* pParent = NULL);   
	virtual ~CDumpMemoryDlg() {};

	virtual void DoCommand();

	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();

  BOOL GetStatus() { return m_status; }
  U32 GetAddress() { return m_startAddress; }
  U32 GetSize() { return m_dumpSize; }
protected:
	U32 m_startAddress;
  U32 m_dumpSize;
  BOOL m_status;

	DECLARE_MESSAGE_MAP()
};

// CConfigQueryPstiIntervalDlg 
class CConfigQueryPstiIntervalDlg : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CConfigQueryPstiIntervalDlg)
public:
	CConfigQueryPstiIntervalDlg(CWnd* pParent = NULL);   
	virtual ~CConfigQueryPstiIntervalDlg() {};

  virtual void DoCommand();
	virtual BOOL OnInitDialog();

  enum { KeyLength = 16 };

protected:
	afx_msg void OnBnClickedOk();
  afx_msg void OnBnClickedQuery();

  U08 m_msgId;
  U08 m_interval;
  U08 m_attribute;

	DECLARE_MESSAGE_MAP()
};

// CConfigRegisterDlg
class CConfigRegisterDlg : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CConfigRegisterDlg)
public:
	CConfigRegisterDlg(CWnd* pParent = NULL);   
	virtual ~CConfigRegisterDlg() {};

  enum CmdType
  {
    Register,
    IoRegister,
    RfIcRegister,
  };

  virtual void DoCommand();
	virtual BOOL OnInitDialog();
  virtual void OnCancel();
	virtual void OnOk();

	void SetCommandType(CmdType t) { m_type = t; }
  static bool SetRegisters(CmdType type, vector< pair<U32, U32> >& regData);
protected:
  CmdType m_type;
  vector< pair<U32, U32> > m_regData;
  int GetRegistersData();
  void RestoreRegistersData();
  void UpdateUiSetting(bool isSave);

	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
  afx_msg void OnClose();
	afx_msg void OnBnClickedGet();
	afx_msg void OnBnClickedSet();

	afx_msg void OnBnClickedGetAll();
	afx_msg void OnBnClickedSetAll();
  afx_msg void OnBnClickedClose();
	//afx_msg void OnBnClickedCopyResult();
	DECLARE_MESSAGE_MAP()
};

// CConfigV9PowerSave 
class CConfigV9PowerSave : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CConfigV9PowerSave)
public:
	CConfigV9PowerSave(CWnd* pParent = NULL);   
	virtual ~CConfigV9PowerSave() {};

  enum DdType {
    DD_RTC_Timer_30,
    DD_RTC_Timer_60,
    DD_RTC_Timer_90,
    DD_RTC_Timer_120,
  };
  enum PsType {
    PS_RTC_Timer,
    PS_IO_Toggle,
    PS_Power_Cycle,
  };

  virtual void DoCommand();
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();
	virtual INT_PTR DoDirect(int t);

protected:
	DdType m_ddType;
	PsType m_psType;
	int m_timeout;

	DECLARE_MESSAGE_MAP()
};

class CConfigRtkElevationAndCnrMask : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CConfigRtkElevationAndCnrMask)
public:
	CConfigRtkElevationAndCnrMask(CWnd* pParent = NULL);   
	virtual ~CConfigRtkElevationAndCnrMask() {};

	virtual void DoCommand();
	afx_msg void OnBnClickedOk();
	virtual BOOL OnInitDialog();

protected:
	S08 m_ele;
	U08 m_cnr;
	U08 m_gcnr;
	S08 m_ele2;
	U08 m_cnr2;
	U08 m_gcnr2;
  U08 m_attribute;

	DECLARE_MESSAGE_MAP()
};

class CConfigCustomStringIntervalDlg : public CCommonConfigDlg
{
	DECLARE_DYNAMIC(CConfigCustomStringIntervalDlg)
public:
  enum { NmeaStringSize = 3 };

	CConfigCustomStringIntervalDlg(CWnd* pParent = NULL);   
	virtual ~CConfigCustomStringIntervalDlg() {};

	virtual void DoCommand();
	virtual BOOL OnInitDialog();

	afx_msg void OnCbnSelEndNmeaString();
  afx_msg void OnBnClickedQuery();
	afx_msg void OnBnClickedOk();

  void UpdateInterval();
protected:
	U08 m_nmeaId[NmeaStringSize];
	U08 m_interval;
  U08 m_attribute;
  CString m_title;
	DECLARE_MESSAGE_MAP()
};

