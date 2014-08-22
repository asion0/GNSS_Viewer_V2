#pragma once

// CGetEphemerisDlg 對話方塊

class CGetEphemerisDlg : public CDialog
{
	DECLARE_DYNAMIC(CGetEphemerisDlg)

public:
	CGetEphemerisDlg(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CGetEphemerisDlg();

	//BOOL IsGlonass() { return isGlonass; };	
	//void SetIsGlonass(BOOL bSet) { isGlonass = bSet; };	

	enum EphemerisType 
	{
		GpsEphemeris = 0,
		GlonassEphemeris,
		BeidouEphemeris,
		GalileoEphemeris
	};
	EphemerisType GetEphemerisType() { return ephType; }
	void SetEphemerisType(EphemerisType s) { ephType = s; };	

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援
	CString GetEphemerisNmae();

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
