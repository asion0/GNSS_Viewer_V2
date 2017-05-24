#pragma once


// NmeaChecksumCalDlg 

class NmeaChecksumCalDlg : public CDialog
{
	DECLARE_DYNAMIC(NmeaChecksumCalDlg)

public:
	NmeaChecksumCalDlg(CWnd* pParent = NULL);   
	virtual ~NmeaChecksumCalDlg();

//	enum { IDD = IDD_NMEA_CHECKSUM_CAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnEnChangeInput();

protected:
	CString GetFullNmeaString(const CString& s);

public:
	afx_msg void OnBnClickedSend();
};
