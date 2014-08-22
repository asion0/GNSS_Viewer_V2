#pragma once

#if WITH_CONFIG_USB_BAUDRATE
// CConfig_USB 對話方塊

class CConfig_USB : public CDialog
{
	DECLARE_DYNAMIC(CConfig_USB)

public:
	CConfig_USB(CWnd* pParent = NULL);   // 標準建構函式
	virtual ~CConfig_USB();

	CComboBox m_Devices;

	BAUD_CONFIG_DATA	m_DefaultBaudConfigData;

	BAUD_CONFIG_DATA m_ROM_BaudConfigData;

	HANDLE	m_DeviceHandle;
// 對話方塊資料
	enum { IDD = IDD_CFG_USB };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支援

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedRefresh();
	afx_msg void OnBnClickedUsedRom();
	afx_msg void OnBnClickedUsedFlash();
};

#endif