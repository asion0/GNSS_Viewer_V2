#pragma once


// BinaryChecksumCalDlg 對話方塊
class BinaryData;

class BinaryChecksumCalDlg : public CDialog
{
	DECLARE_DYNAMIC(BinaryChecksumCalDlg)

public:
	BinaryChecksumCalDlg(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~BinaryChecksumCalDlg();

	int FindNextNoneSpaceChar(LPCSTR pszInput, bool forward = true);
	bool ConvertHexToBinary(LPCSTR pszInput, BinaryData& binData);
// 對話方塊資料
//	enum { IDD = IDD_BIN_CHECKSUM_CAL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnEnChangeInput();
protected:
	virtual void OnOK();

public:
	afx_msg void OnBnClickedSend();
};
