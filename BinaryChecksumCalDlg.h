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
  enum Mode {
    CalcCheckSum,
    RawData
  };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
  void SetMode(Mode m) { m_mode = m; }
	afx_msg void OnEnChangeInput();
	afx_msg void OnBnClickedSend();

protected:
  Mode m_mode;
	virtual void OnOK();
};
