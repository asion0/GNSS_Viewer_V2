#pragma once

// CGetEphemerisDlg ¶ô

class CGetEphemerisDlg : public CDialog
{
	DECLARE_DYNAMIC(CGetEphemerisDlg)

public:
	CGetEphemerisDlg(CWnd* pParent = NULL);   
	virtual ~CGetEphemerisDlg();

	//BOOL IsGlonass() { return isGlonass; };	
	//void SetIsGlonass(BOOL bSet) { isGlonass = bSet; };	

	enum EphemerisType 
	{
		GpsEphemeris = 0,
		GlonassEphemeris,
		BeidouEphemeris,
		GalileoEphemeris,
    NavicEphemeris,
    GpsQzssEphemeris,
    QzssEphemeris
	};
	EphemerisType GetEphemerisType() { return ephType; }
	void SetEphemerisType(EphemerisType s) { ephType = s; };	

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 
	CString GetEphemerisName();

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();	
	afx_msg void OnBnClickedBrowse();

private:
	CComboBox m_ephems;
	CString m_fileName;
	CEdit m_file;
	EphemerisType ephType;
};
