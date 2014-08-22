// Config_USB.cpp : 實作檔
//


#include "stdafx.h"
#include "GPS.h"
#include "Config_USB.h"

#if WITH_CONFIG_USB_BAUDRATE

// CConfig_USB 對話方塊

IMPLEMENT_DYNAMIC(CConfig_USB, CDialog)

CConfig_USB::CConfig_USB(CWnd* pParent /*=NULL*/)
	: CDialog(CConfig_USB::IDD, pParent)
{

}

CConfig_USB::~CConfig_USB()
{
}

void CConfig_USB::DoDataExchange(CDataExchange* pDX)
{
	DDX_Control(pDX,IDC_CBO_DEVICE,m_Devices);
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CConfig_USB, CDialog)
	ON_BN_CLICKED(IDC_REFRESH, &CConfig_USB::OnBnClickedRefresh)
	ON_BN_CLICKED(IDC_USED_ROM, &CConfig_USB::OnBnClickedUsedRom)
	ON_BN_CLICKED(IDC_USED_FLASH, &CConfig_USB::OnBnClickedUsedFlash)
END_MESSAGE_MAP()


// CConfig_USB 訊息處理常式

BOOL CConfig_USB::OnInitDialog()
{
	CDialog::OnInitDialog();

	m_DefaultBaudConfigData[0].BaudRate = 15000000;
	m_DefaultBaudConfigData[0].BaudGen = 0xFFF0;
	m_DefaultBaudConfigData[0].Prescaler = 1;
	m_DefaultBaudConfigData[0].Timer0Reload = 0xFFFA;

	m_DefaultBaudConfigData[1].BaudRate = 15000000;
	m_DefaultBaudConfigData[1].BaudGen = 0xFFF0;
	m_DefaultBaudConfigData[1].Prescaler = 1;
	m_DefaultBaudConfigData[1].Timer0Reload = 0xFFFA;

	m_DefaultBaudConfigData[2].BaudRate = 12000000;
	m_DefaultBaudConfigData[2].BaudGen = 0xFFEC;
	m_DefaultBaudConfigData[2].Prescaler = 1;
	m_DefaultBaudConfigData[2].Timer0Reload = 0xFFF8;

	m_DefaultBaudConfigData[3].BaudRate = 921600;
	m_DefaultBaudConfigData[3].BaudGen = 0xFFE6;
	m_DefaultBaudConfigData[3].Prescaler = 1;
	m_DefaultBaudConfigData[3].Timer0Reload = 0xFFF6;

	m_DefaultBaudConfigData[4].BaudRate = 576000;
	m_DefaultBaudConfigData[4].BaudGen = 0xFFD6;
	m_DefaultBaudConfigData[4].Prescaler = 1;
	m_DefaultBaudConfigData[4].Timer0Reload = 0xFFF0;

	m_DefaultBaudConfigData[5].BaudRate = 500000;
	m_DefaultBaudConfigData[5].BaudGen = 0xFFD0;
	m_DefaultBaudConfigData[5].Prescaler = 1;
	m_DefaultBaudConfigData[5].Timer0Reload = 0xFFEE;

	m_DefaultBaudConfigData[6].BaudRate = 460800;
	m_DefaultBaudConfigData[6].BaudGen = 0xFFCC;
	m_DefaultBaudConfigData[6].Prescaler = 1;
	m_DefaultBaudConfigData[6].Timer0Reload = 0xFFEC;

	m_DefaultBaudConfigData[7].BaudRate = 256000;
	m_DefaultBaudConfigData[7].BaudGen = 0xFFA2;
	m_DefaultBaudConfigData[7].Prescaler = 1;
	m_DefaultBaudConfigData[7].Timer0Reload = 0xFFDC;

	m_DefaultBaudConfigData[8].BaudRate = 250000;
	m_DefaultBaudConfigData[8].BaudGen = 0xFFA0;
	m_DefaultBaudConfigData[8].Prescaler = 1;
	m_DefaultBaudConfigData[8].Timer0Reload = 0xFFDC;

	m_DefaultBaudConfigData[9].BaudRate = 230400;
	m_DefaultBaudConfigData[9].BaudGen = 0xFF98;
	m_DefaultBaudConfigData[9].Prescaler = 1;
	m_DefaultBaudConfigData[9].Timer0Reload = 0xFFD9;

	m_DefaultBaudConfigData[10].BaudRate = 153600;
	m_DefaultBaudConfigData[10].BaudGen = 0xFF64;
	m_DefaultBaudConfigData[10].Prescaler = 1;
	m_DefaultBaudConfigData[10].Timer0Reload = 0xFFC5;

	m_DefaultBaudConfigData[11].BaudRate = 128000;
	m_DefaultBaudConfigData[11].BaudGen = 0xFF44; //Originally FF45 in firmware, but changed to FF44 Sep 7 2004
	m_DefaultBaudConfigData[11].Prescaler = 1;
	m_DefaultBaudConfigData[11].Timer0Reload = 0xFFB9;

	m_DefaultBaudConfigData[12].BaudRate = 115200;
	m_DefaultBaudConfigData[12].BaudGen = 0xFF30;
	m_DefaultBaudConfigData[12].Prescaler = 1;
	m_DefaultBaudConfigData[12].Timer0Reload = 0xFFB2;

	m_DefaultBaudConfigData[13].BaudRate = 76800;
	m_DefaultBaudConfigData[13].BaudGen = 0xFEC8;
	m_DefaultBaudConfigData[13].Prescaler = 1;
	m_DefaultBaudConfigData[13].Timer0Reload = 0xFF8B;

	m_DefaultBaudConfigData[14].BaudRate = 64000;
	m_DefaultBaudConfigData[14].BaudGen = 0xFE89;
	m_DefaultBaudConfigData[14].Prescaler = 1;
	m_DefaultBaudConfigData[14].Timer0Reload = 0xFF73;

	m_DefaultBaudConfigData[15].BaudRate = 57600;
	m_DefaultBaudConfigData[15].BaudGen = 0xFE5F;
	m_DefaultBaudConfigData[15].Prescaler = 1;
	m_DefaultBaudConfigData[15].Timer0Reload = 0xFF63;

	m_DefaultBaudConfigData[16].BaudRate = 56000;
	m_DefaultBaudConfigData[16].BaudGen = 0xFE53;
	m_DefaultBaudConfigData[16].Prescaler = 1;
	m_DefaultBaudConfigData[16].Timer0Reload = 0xFF5F;

	m_DefaultBaudConfigData[17].BaudRate = 51200;
	m_DefaultBaudConfigData[17].BaudGen = 0xFE2B;
	m_DefaultBaudConfigData[17].Prescaler = 1;
	m_DefaultBaudConfigData[17].Timer0Reload = 0xFF50;

	m_DefaultBaudConfigData[18].BaudRate = 38400;
	m_DefaultBaudConfigData[18].BaudGen = 0xFD8F;
	m_DefaultBaudConfigData[18].Prescaler = 1;
	m_DefaultBaudConfigData[18].Timer0Reload = 0xFF15;

	m_DefaultBaudConfigData[19].BaudRate = 28800;
	m_DefaultBaudConfigData[19].BaudGen = 0xFCBF;
	m_DefaultBaudConfigData[19].Prescaler = 1;
	m_DefaultBaudConfigData[19].Timer0Reload = 0xFEC7;

	m_DefaultBaudConfigData[20].BaudRate = 19200;
	m_DefaultBaudConfigData[20].BaudGen = 0xFB1E;
	m_DefaultBaudConfigData[20].Prescaler = 1;
	m_DefaultBaudConfigData[20].Timer0Reload = 0xFE2B;

	m_DefaultBaudConfigData[21].BaudRate = 16000;
	m_DefaultBaudConfigData[21].BaudGen = 0xFA24;
	m_DefaultBaudConfigData[21].Prescaler = 1;
	m_DefaultBaudConfigData[21].Timer0Reload = 0xFDCD;

	m_DefaultBaudConfigData[22].BaudRate = 14400;
	m_DefaultBaudConfigData[22].BaudGen = 0xF97D;
	m_DefaultBaudConfigData[22].Prescaler = 1;
	m_DefaultBaudConfigData[22].Timer0Reload = 0xFD8E;

	m_DefaultBaudConfigData[23].BaudRate = 9600;
	m_DefaultBaudConfigData[23].BaudGen = 0xF63C;
	m_DefaultBaudConfigData[23].Prescaler = 1;
	m_DefaultBaudConfigData[23].Timer0Reload = 0xFC56;

	m_DefaultBaudConfigData[24].BaudRate = 7200;
	m_DefaultBaudConfigData[24].BaudGen = 0xF2FB;
	m_DefaultBaudConfigData[24].Prescaler = 1;
	m_DefaultBaudConfigData[24].Timer0Reload = 0xFB1E;

	m_DefaultBaudConfigData[25].BaudRate = 4800;
	m_DefaultBaudConfigData[25].BaudGen = 0xEC78;
	m_DefaultBaudConfigData[25].Prescaler = 1;
	m_DefaultBaudConfigData[25].Timer0Reload = 0xF8AD;

	m_DefaultBaudConfigData[26].BaudRate = 4000;
	m_DefaultBaudConfigData[26].BaudGen = 0xE890;
	m_DefaultBaudConfigData[26].Prescaler = 1;
	m_DefaultBaudConfigData[26].Timer0Reload = 0xF736;

	m_DefaultBaudConfigData[27].BaudRate = 2400;
	m_DefaultBaudConfigData[27].BaudGen = 0xD8F0;
	m_DefaultBaudConfigData[27].Prescaler = 1;
	m_DefaultBaudConfigData[27].Timer0Reload = 0xF15A;

	m_DefaultBaudConfigData[28].BaudRate = 1800;
	m_DefaultBaudConfigData[28].BaudGen = 0xCBEB;
	m_DefaultBaudConfigData[28].Prescaler = 1;
	m_DefaultBaudConfigData[28].Timer0Reload = 0xEC78;

	m_DefaultBaudConfigData[29].BaudRate = 1200;
	m_DefaultBaudConfigData[29].BaudGen = 0xB1E0;
	m_DefaultBaudConfigData[29].Prescaler = 1;
	m_DefaultBaudConfigData[29].Timer0Reload = 0xE2B4;

	m_DefaultBaudConfigData[30].BaudRate = 600;
	m_DefaultBaudConfigData[30].BaudGen = 0x63C0;
	m_DefaultBaudConfigData[30].Prescaler = 1;
	m_DefaultBaudConfigData[30].Timer0Reload = 0xC568;

	m_DefaultBaudConfigData[31].BaudRate = 300;
	m_DefaultBaudConfigData[31].BaudGen = 0xB1E0;
	m_DefaultBaudConfigData[31].Prescaler = 4;
	m_DefaultBaudConfigData[31].Timer0Reload = 0x8AD0;

	memcpy(&m_ROM_BaudConfigData[0],&m_DefaultBaudConfigData[0],sizeof(m_ROM_BaudConfigData));

	m_ROM_BaudConfigData[9].BaudGen = 0xFFA8;		// 230400
	m_ROM_BaudConfigData[12].BaudGen = 0xFF51;		// 115200
	m_ROM_BaudConfigData[15].BaudGen = 0xFEA2;		// 57600
	m_ROM_BaudConfigData[18].BaudGen = 0xFDF3;		// 38400
	m_ROM_BaudConfigData[20].BaudGen = 0xFBE6;		// 19200
	m_ROM_BaudConfigData[23].BaudGen = 0xF7CD;		// 9600
	m_ROM_BaudConfigData[25].BaudGen = 0xEF9B;		// 4800
	
	OnBnClickedRefresh();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX 屬性頁應傳回 FALSE
}

void CConfig_USB::OnBnClickedRefresh()
{
	DWORD numDevices;

	// Obtain the number of devices connected
	CP210x_GetNumDevices(&numDevices);

	// Reset the content of our list
	m_Devices.ResetContent();

	// Loop through each device, and put its product string in the list
	for (DWORD i = 0; i < numDevices; i++)
	{
		CP210x_DEVICE_STRING serialString;

		CP210x_GetProductString(i, serialString, CP210x_RETURN_FULL_PATH);
		m_Devices.AddString(serialString);
	}

	// Set our list to select the first device in it
	m_Devices.SetCurSel(0);
}

void CConfig_USB::OnBnClickedUsedRom()
{
	// TODO: Add your control notification handler code here
	BAUD_CONFIG_DATA	m_BaudConfigData;
	// Check our current device selection
	if (m_Devices.GetCurSel() > -1)
	{
		// If we are selecting a valid device, open it
		if (CP210x_Open(m_Devices.GetCurSel(), &m_DeviceHandle) == CP210x_SUCCESS)
		{
			// Obtain the part num, only CP2102 and up will work
			BYTE version;
			if (CP210x_GetPartNumber(m_DeviceHandle, &version) == CP210x_SUCCESS)
			{
				if (version == CP210x_CP2101_VERSION)
				{
					MessageBox("CP2101 is not supported in this application", "Error", MB_OK | MB_ICONEXCLAMATION);
				}
				else
				{
					CP210x_SetBaudRateConfig(m_DeviceHandle, m_ROM_BaudConfigData);
					
					CP210x_GetBaudRateConfig(m_DeviceHandle, m_BaudConfigData);
				}
			}

			CP210x_Close(m_DeviceHandle);
		}
	}
}

void CConfig_USB::OnBnClickedUsedFlash()
{
	// TODO: Add your control notification handler code here
	BAUD_CONFIG_DATA	m_BaudConfigData;
	// Check our current device selection
	if (m_Devices.GetCurSel() > -1)
	{
		// If we are selecting a valid device, open it
		if (CP210x_Open(m_Devices.GetCurSel(), &m_DeviceHandle) == CP210x_SUCCESS)
		{
			// Obtain the part num, only CP2102 and up will work
			BYTE version;
			if (CP210x_GetPartNumber(m_DeviceHandle, &version) == CP210x_SUCCESS)
			{
				if (version == CP210x_CP2101_VERSION)
				{
					MessageBox("CP2101 is not supported in this application", "Error", MB_OK | MB_ICONEXCLAMATION);
				}
				else
				{
					CP210x_SetBaudRateConfig(m_DeviceHandle, m_DefaultBaudConfigData);
					
					CP210x_GetBaudRateConfig(m_DeviceHandle, m_BaudConfigData);
				}
			}

			CP210x_Close(m_DeviceHandle);
		}
	}
}

#endif


