#pragma once


// BinaryChecksumCalDlg
class BinaryData;

class BinaryChecksumCalDlg : public CDialog
{
	DECLARE_DYNAMIC(BinaryChecksumCalDlg)

public:
	BinaryChecksumCalDlg(CWnd* pParent = NULL);   // 
	virtual ~BinaryChecksumCalDlg();
//	enum { IDD = IDD_BIN_CHECKSUM_CAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnEnChangeInput();
protected:
	virtual void OnOK();

public:
	afx_msg void OnBnClickedSend();
};
