// PromIniGeneratorDlg.cpp
//

#include "stdafx.h"
#include "PromIniGeneratorDlg.h"
#include "Resource.h"
#include "GPSDlg.h"
#include "Serial.h"


// PromIniGeneratorDlg
IMPLEMENT_DYNAMIC(PromIniGeneratorDlg, CDialog)

PromIniGeneratorDlg::PromIniGeneratorDlg(LPCSTR pszPromPath, UIType t/* = V8*/, CWnd* pParent/*=NULL*/)
	: CDialog(IDD_PROM_INI_GEN, pParent)
{
  promPath = pszPromPath;
  type = t;
}

PromIniGeneratorDlg::~PromIniGeneratorDlg()
{
}

void PromIniGeneratorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(PromIniGeneratorDlg, CDialog)
  ON_EN_CHANGE(IDC_TAG_VALUE, &PromIniGeneratorDlg::OnEnChangeTagValue)
  ON_EN_CHANGE(IDC_PRODUCT_NAME, &PromIniGeneratorDlg::OnEnChangeProductName)
  ON_BN_CLICKED(IDOK, &PromIniGeneratorDlg::OnBnClickedOk)
  ON_BN_CLICKED(IDC_TAG, &PromIniGeneratorDlg::OnBnClickedEnableTag)
	ON_BN_CLICKED(IDC_RENAME, &PromIniGeneratorDlg::OnBnClickedRename)


END_MESSAGE_MAP()


// PromIniGeneratorDlg 
void PromIniGeneratorDlg::OnEnChangeProductName()
{
  UpdateUiStatus();
}

void PromIniGeneratorDlg::OnEnChangeTagValue()
{
  UpdateUiStatus();
}

void PromIniGeneratorDlg::UpdateUiStatus()
{
  if(((CButton*)GetDlgItem(IDC_TAG))->GetCheck())
  {
    GetDlgItem(IDC_TAG_ADDR)->EnableWindow((type == Phoenix) ? FALSE : TRUE);
    GetDlgItem(IDC_TAG_VALUE)->EnableWindow((type == Phoenix) ? FALSE : TRUE);
  }
  else
  {
    GetDlgItem(IDC_TAG_ADDR)->EnableWindow(FALSE);
    GetDlgItem(IDC_TAG_VALUE)->EnableWindow(FALSE);
  }

  CString txt;
  U32 value;

  GetDlgItem(IDC_TAG_ADDR)->GetWindowText(txt);
	value = ConvertCharToU32(txt);
  txt.Format("%08X", value ^ 0xAAAAAAAA);
  GetDlgItem(IDC_TAG_ADDR2)->SetWindowText(txt);

  GetDlgItem(IDC_TAG_VALUE)->GetWindowText(txt);
	value = ConvertCharToU32(txt);
  txt.Format("%04X", value ^ 0x55555555);
  GetDlgItem(IDC_TAG_VALUE2)->SetWindowText(txt);

  //ta = Convert.ToUInt32(tagAddress.Text, 16) ^ 0xaaaaaaaa;
  //tc = Convert.ToUInt32(tagContent.Text, 16) ^ 0x55555555;
  //STI_02.02.04-01.07.28_V8_2525BD-RTK_Master_CRC_018a_115200_20170712.bin
  CString product, kVer, sVer, rVer, crc;
  GetDlgItem(IDC_PRODUCT_NAME)->GetWindowText(product);
  GetDlgItem(IDC_K_VER)->GetWindowText(kVer);
  GetDlgItem(IDC_S_VER)->GetWindowText(sVer);
  GetDlgItem(IDC_R_VER)->GetWindowText(rVer);
  GetDlgItem(IDC_CRC)->GetWindowText(crc);

  txt.Format("STI_%s-%s_%s_CRC_%s_%d_%s.bin", 
    kVer,
    sVer,
    product,
    crc,
    g_setting.BaudrateValue(g_setting.GetBaudrateIndex()),
    rVer);
  GetDlgItem(IDC_NEW_FILENAME)->SetWindowText(txt);

}

BOOL PromIniGeneratorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

  CString txt;
  CGPSDlg::CmdErrorCode ack = CGPSDlg::Timeout;
  BinaryData ackData;
  //BinaryData swFeatureAck;
  U16 retCrc;
  U32 swFeature;
 
  enum PdType
  {
    PdGPS = 0x01,
    PdBD = 0x02,
    PdGlonass = 0x04,
    PdGalileo = 0x08,
    PdIRNSS = 0x10 
  };

  enum PdFreq
  {
    PdL1 = 0x01,
    PdL2 = 0x02,
    PdL3 = 0x04,
    PdL5 = 0x08,
    PdL6 = 0x10
  };

  PdType FwFeatureToPdType[] = 
  {
    PdGPS,  //Bit 0 : GPS L1 C/A
    PdGPS,  //Bit 1 : GPS L1C
    PdGPS,  //Bit 2 : GPS L2C
    PdGPS,  //Bit 3 : GPS L5
    PdBD,   //Bit 4 : BD B1I
    PdBD,   //Bit 5 : BD B1C
    PdBD,   //Bit 6 : BD B2A
    PdBD,   //BIT 7 : BD B2I
    PdBD,   //BIT 8 : BD B3I
    PdGlonass,  //BIT 9 : Glonass L1
    PdGlonass,  //BIT 10: Glonass L2
    PdGlonass,  //BIT 11: Glonass L3
    PdGalileo,  //BIT 12: Galileo E1
    PdGalileo,  //BIT 13: Galileo E5a
    PdGalileo,  //BIT 14: Galileo E5b
    PdGalileo,  //BIT 15: Galileo E6
    PdGPS,  //BIT 16: QZSS L1 C/A
    PdGPS,  //BIT 17: QZSS L1C
    PdGPS,  //BIT 18: QZSS L2C
    PdGPS,  //BIT 19: QZSS L5
    PdGPS,  //BIT 20: QZSS LEX
    PdGPS,  //BIT 21: SBAS L1
    PdIRNSS,//BIT 22: IRNSS L5
  };

  PdFreq FwFeatureToPdFreq[] = 
  {
    PdL1, //Bit 0 : GPS L1 C/A
    PdL1, //Bit 1 : GPS L1C
    PdL2, //Bit 2 : GPS L2C
    PdL5, //Bit 3 : GPS L5
    PdL1, //Bit 4 : BD B1I
    PdL1, //Bit 5 : BD B1C
    PdL2, //Bit 6 : BD B2A
    PdL2, //BIT 7 : BD B2I
    PdL3, //BIT 8 : BD B3I
    PdL1, //BIT 9 : Glonass L1
    PdL2, //BIT 10: Glonass L2
    PdL3, //BIT 11: Glonass L3
    PdL1, //BIT 12: Galileo E1
    PdL5, //BIT 13: Galileo E5a
    PdL5, //BIT 14: Galileo E5b
    PdL6, //BIT 15: Galileo E6
    PdL1, //BIT 16: QZSS L1 C/A
    PdL1, //BIT 17: QZSS L1C
    PdL2, //BIT 18: QZSS L2C
    PdL5, //BIT 19: QZSS L5
    PdL6, //BIT 20: QZSS LEX
    PdL1, //BIT 21: SBAS L1
    PdL5, //BIT 22: IRNSS L5
  };

  char PdTypeToPdCode[] = 
  {      //GI GA GL BD GP
    '#'  ,//0  0  0  0  0
    '0'  ,//0  0  0  0  1
    '1'  ,//0  0  0  1  0
    '5'  ,//0  0  0  1  1
    '2'  ,//0  0  1  0  0
    '6'  ,//0  0  1  0  1
    '9'  ,//0  0  1  1  0
    'F'  ,//0  0  1  1  1
    '3'  ,//0  1  0  0  0
    '7'  ,//0  1  0  0  1
    'A'  ,//0  1  0  1  0
    'G'  ,//0  1  0  1  1
    'C'  ,//0  1  1  0  0
    'I'  ,//0  1  1  0  1
    'M'  ,//0  1  1  1  0
    'P'  ,//0  1  1  1  1
    '4'  ,//1  0  0  0  0
    '8'  ,//1  0  0  0  1
    'B'  ,//1  0  0  1  0
    'H'  ,//1  0  0  1  1
    'D'  ,//1  0  1  0  0
    'J'  ,//1  0  1  0  1
    'L'  ,//1  0  1  1  0
    'Q'  ,//1  0  1  1  1
    'E'  ,//1  1  0  0  0
    'K'  ,//1  1  0  0  1
    'N'  ,//1  1  0  1  0
    'R'  ,//1  1  0  1  1
    'O'  ,//1  1  1  0  0
    'S'  ,//1  1  1  0  1
    'T'  ,//1  1  1  1  0
    'U'  ,//1  1  1  1  1
  };

  char PdFreqToPdCode[] = 
  {
         //L6 L5 L3 L2 L1
    '#',  //0  0  0  0  0
    '0',  //0  0  0  0  1
    '#',  //0  0  0  1  0
    '2',  //0  0  0  1  1
    '#',  //0  0  1  0  0
    '#',  //0  0  1  0  1
    '#',  //0  0  1  1  0
    '#',  //0  0  1  1  1
    '#',  //0  1  0  0  0
    '5',  //0  1  0  0  1
    '#',  //0  1  0  1  0
    '7',  //0  1  0  1  1
    '#',  //0  1  1  0  0
    '#',  //0  1  1  0  1
    '#',  //0  1  1  1  0
    '#',  //0  1  1  1  1
    '#',  //1  0  0  0  0
    '6',  //1  0  0  0  1
    '#',  //1  0  0  1  0
    '8',  //1  0  0  1  1
    '#',  //1  0  1  0  0
    '#',  //1  0  1  0  1
    '#',  //1  0  1  1  0
    '3',  //1  0  1  1  1
    '#',  //1  1  0  0  0
    '#',  //1  1  0  0  1
    '#',  //1  1  0  1  0
    '#',  //1  1  0  1  1
    '#',  //1  1  1  0  0
    '#',  //1  1  1  0  1
    '#',  //1  1  1  1  0
    '#',  //1  1  1  1  1
  };

  U08 pdType = 0, pdFreq = 0;
  const char *pdTypeString[] =
  {
    "GPS",
    "BD",
    "GL",
    "GALILEO",
    "NAVIC",
    "GPS + BD",
    "GPS + GL",
    "GPS + GALILEO",
    "GPS + NAVIC",
    "BD + GL",
    "BD + GALILEO",
    "BD + NAVIC",
    "GL + GALILEO",
    "GL + NAVIC",
    "GALILEO + NAVIC",
    "GPS + BD + GL",
    "GPS + BD + GALILEO",
    "GPS + BD + NAVIC",
    "GPS + GL + GALILEO",
    "GPS + GL + NAVIC",
    "GPS + GALILEO + NAVIC",
    "BD + GL + NAVIC",
    "BD + GL + GALILEO",
    "BD + GALILEO + NAVIC",
    "GL + GALILEO + NAVIC",
    "GPS + BD + GL + GALILEO",
    "GPS + BD + GL + NAVIC",
    "GPS +BD + GALILEO + NAVIC",
    "GPS + GL + GALILEO + NAVIC",
    "BD + GL + GALILEO + NAVIC",
    "GPS + BD + GL + GALILEO + NAVIC",
  };

  const char *pdFreqString[] =
  {
    "L1",
    "",
    "L1 + L2",
    "L1 +L2 + L3 + L6",
    "",
    "L1 + L5",
    "L1 + L6",
    "L1 + L2 + L5",
    "L1 + L2 + L6",
  };
  char cType, cFreq;

  if(type == V8)
  {
    GetDlgItem(IDC_CRC32_T)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_CRC32_T2)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_CRC32_16M)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_CRC32_8M)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_GNSSTYPE_T)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_GNSSTYPE)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_GNSSTYPE_C)->ShowWindow(SW_HIDE);
  }

  if(type == Phoenix)
  {
    GetDlgItem(IDC_TAG_ADDR2)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_TAG_VALUE2)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_TAG)->EnableWindow(FALSE);
    GetDlgItem(IDC_TAG_VALUE)->EnableWindow(FALSE);
  }

  ((CEdit*)GetDlgItem(IDC_TAG_VALUE))->SetLimitText(4);
  GetDlgItem(IDC_PROM_PATH)->SetWindowText(promPath);
  //if(!Utility::IsFileExist(promPath))
  //{
  //  ::AfxMessageBox("PROM file doesn't exist!");
  //  goto Error;
  //}
  U16 crcM = 0, crcS = 0;
  U32 crc32_16m = 0, crc32_8m = 0;
  U08 checkSum = 0;
	if(!Utility::CalcFirmwareCrc(promPath, 0, crcM, crcS, crc32_16m, crc32_8m, checkSum))
  {
    ::AfxMessageBox("PROM file can't open! Please select it in the Download panel first!");
    goto Error;
  }
  //Get Software CRC
  ack = CGPSDlg::gpsDlg->QuerySoftwareCrcSystemCode(CGPSDlg::Return, &retCrc);
  if(CGPSDlg::Ack != ack)
  {
    ::AfxMessageBox("Query software CRC failed!");
    goto Error;
  }
  if(retCrc != crcM && retCrc != crcS)
  {
    if(IDYES != ::AfxMessageBox("The CRC of the PROM file does not match the firmware!\r\n" \
      "Do you still want to generate an ini file?", MB_YESNO | MB_ICONSTOP))
    {
      goto Error;
    }
  }

  txt.Format("%04X", retCrc);
	GetDlgItem(IDC_CRC)->SetWindowText(txt);

  if(type == Phoenix)
  {
    U32 retCrc32;
    //Get Software CRC32
    ack = CGPSDlg::gpsDlg->QuerySoftwareCrc32SystemCode(CGPSDlg::Return, &retCrc32);
    if(CGPSDlg::Ack != ack)
    {
      if(IDYES != ::AfxMessageBox("Query software CRC32 failed!\r\n" \
      "Do you still want to generate an ini file?", MB_YESNO | MB_ICONSTOP))
      {
        goto Error;
      }
    }
    if(retCrc32 != crc32_16m && retCrc32 != crc32_8m)
    {
      if(IDYES != ::AfxMessageBox("The CRC32 of the PROM file does not match the firmware.\r\n" \
        "Do you still want to generate an ini file?", MB_YESNO | MB_ICONSTOP))
      {
        goto Error;
      }
      retCrc32 = 0;
    }

    txt.Format("%08X", crc32_16m);
	  GetDlgItem(IDC_CRC32_16M)->SetWindowText(txt);
    txt.Format("%08X", crc32_8m);
	  GetDlgItem(IDC_CRC32_8M)->SetWindowText(txt);

    ack = CGPSDlg::gpsDlg->QueryPhoenixSoftwareFeature(CGPSDlg::Return, &ackData);
    if(CGPSDlg::Ack != ack)
    {
      if(IDYES != ::AfxMessageBox("Query software feature failed!\r\n" \
      "Do you still want to generate an ini file?", MB_YESNO | MB_ICONSTOP))
      {
        goto Error;
      }
      swFeature = 1;
    }
    else
    {
      swFeature = ConvertLeonU32(ackData.Ptr(6));
    }
    for(int i = 0; i <= 22; ++i)
    {
      U32 f = 1 << i;
      if((f & swFeature) != 0)
      {
        pdType |= (U32)FwFeatureToPdType[i];
        pdFreq |= (U32)FwFeatureToPdFreq[i];
      }
    }

    cType = PdTypeToPdCode[pdType];
    if(cType == '#')
    {
      ::AfxMessageBox("GNSS type error!");
      goto Error;
    }

    cFreq = PdFreqToPdCode[pdFreq];
    if(cFreq == '#')
    {
      ::AfxMessageBox("GNSS frequency error!");
      goto Error;
    }
    txt.Format("%c%c", cFreq, cType);
    GetDlgItem(IDC_GNSSTYPE)->SetWindowText(txt);

    cType -= (cType <= '9') ? '0' : '7';
    cFreq -= (cFreq <= '9') ? '0' : '7';
    txt.Format("%s, %s", 
      pdFreqString[cFreq],
      pdTypeString[cType]); 
    GetDlgItem(IDC_GNSSTYPE_C)->SetWindowText(txt);
    txt = "Phoenix_SWID_";
  }
  else
  {
    txt = "V8_";
  }
  GetDlgItem(IDC_PRODUCT_NAME)->SetWindowText(txt);

  //Get Baud rate
  txt.Format("%0d", g_setting.BaudrateValue(g_setting.GetBaudrateIndex()));
  GetDlgItem(IDC_BAUD_RATE)->SetWindowText(txt);

  //Get Software version
  ack = CGPSDlg::gpsDlg->QuerySoftwareVersionSystemCode(CGPSDlg::Return, &ackData);
  if(CGPSDlg::Ack != ack)
  {
    ::AfxMessageBox("Query software version failed!");
    goto Error;
  }
	txt.Format("%02d.%02d.%02d", ackData[7], ackData[8], ackData[9]);
	GetDlgItem(IDC_K_VER)->SetWindowText(txt);
	txt.Format("%02d.%02d.%02d", ackData[11], ackData[12], ackData[13]);
	GetDlgItem(IDC_S_VER)->SetWindowText(txt);
	txt.Format("%04d%02d%02d", ackData[15] + 2000, ackData[16], ackData[17]);
	GetDlgItem(IDC_R_VER)->SetWindowText(txt);

  if(type == Phoenix)
  {
    U32 tagPos = 0;
    bool emptyTag = false;
    bool uniqueTag = Utility::CheckPromUniqueTag(promPath, tagPos, emptyTag);

    if(uniqueTag)
    {
      U32 tagAddr;
      ack = CGPSDlg::gpsDlg->QueryV9TagAddress(CGPSDlg::Return, &ackData);
      if(CGPSDlg::Ack != ack)
      {
        if(IDYES != ::AfxMessageBox("Query Phoenix tag address failed!\r\n" \
          "Do you still want to generate an ini file?", MB_YESNO | MB_ICONSTOP))
        {
          goto Error;
        }
        tagAddr = 0;
      }
      else
      {
        tagAddr = ConvertLeonU32(ackData.Ptr(6));
        if(tagAddr != tagPos)
        {
          if(IDYES != ::AfxMessageBox("The tag address of the PROM file does not match the firmware.\r\n" \
            "Do you still want to generate an ini file?", MB_YESNO | MB_ICONSTOP))
          {
            goto Error;
          }
          tagAddr = 0;
        }
      }
      txt.Format("%08X", tagAddr);
	    GetDlgItem(IDC_TAG_ADDR)->SetWindowText(txt);

      ack = CGPSDlg::gpsDlg->QueryV9Tag(CGPSDlg::Return, &ackData);
      if(CGPSDlg::Ack != ack)
      {
        if(IDYES != ::AfxMessageBox("Query Phoenix tag failed!\r\n" \
        "Do you still want to generate an ini file?", MB_YESNO | MB_ICONSTOP))
        {
          goto Error;
        }
        txt = "0000000000000000";
      }
      else
      {
        txt.Format("%02X%02X%02X%02X%02X%02X%02X%02X", 
          ackData[6], ackData[7], ackData[8], ackData[9], 
          ackData[10], ackData[11], ackData[12], ackData[13]);
      }
	    GetDlgItem(IDC_TAG_VALUE)->SetWindowText(txt);
    }
    ((CButton*)GetDlgItem(IDC_TAG))->SetCheck((BOOL)uniqueTag);
  }
  else
  {
    U32 tagValue, tagAddress = 0xFCFFC;
    ack = CGPSDlg::gpsDlg->QueryRegisterx(CGPSDlg::Return, tagAddress, &tagValue);
    if(CGPSDlg::Ack != ack)
    {
      ::AfxMessageBox("Query tag value failed!");
      goto Error;
    }
    txt.Format("%08X", tagAddress);
	  GetDlgItem(IDC_TAG_ADDR)->SetWindowText(txt);

    tagValue = (tagValue >> 16) & 0xFFFF;
    txt.Format("%04X", (U16)tagValue);
	  GetDlgItem(IDC_TAG_VALUE)->SetWindowText(txt);

    ((CButton*)GetDlgItem(IDC_TAG))->SetCheck(TRUE);
  }
  UpdateUiStatus();
  return TRUE;

Error:
  EndDialog(IDCANCEL);
	return TRUE;  // return TRUE unless you set the focus to a control
}

void PromIniGeneratorDlg::OnOK()
{
	CDialog::OnOK();
}

void PromIniGeneratorDlg::OnBnClickedEnableTag()
{
  UpdateUiStatus();
}

void PromIniGeneratorDlg::OnBnClickedRename()
{
  CString source, newName;

  GetDlgItem(IDC_PROM_PATH)->GetWindowText(source);
  GetDlgItem(IDC_NEW_FILENAME)->GetWindowText(newName);

  CString s1 = Utility::GetFileName(source);
  if(s1 == newName)
  {
    return;
  }

  BOOL ret = Utility::RenameFile(source, newName, TRUE, FALSE);
	if(!ret)
  {
		AfxMessageBox("Rename file failed!");
		return;
  }
  source = Utility::GetFilePath(source) + "\\" + newName;
  GetDlgItem(IDC_PROM_PATH)->SetWindowText(source);
}

void PromIniGeneratorDlg::OnBnClickedOk()
{
  CString source, newName, txt;
  OnBnClickedRename();

  GetDlgItem(IDC_PROM_PATH)->GetWindowText(source);
  newName = Utility::GetFilePathName(source) + ".ini";

  ::DeleteFile(newName);

  GetDlgItem(IDC_NEW_FILENAME)->GetWindowText(txt);
	if(!WritePrivateProfileString("Firmware", "Prom", txt, newName))
  {
    goto Error;
  }

  GetDlgItem(IDC_K_VER)->GetWindowText(txt);
	if(!WritePrivateProfileString("Firmware", "K_Version", txt, newName))
  {
    goto Error;
  }

  GetDlgItem(IDC_S_VER)->GetWindowText(txt);
	if(!WritePrivateProfileString("Firmware", "S_Version", txt, newName))
  {
    goto Error;
  }

  GetDlgItem(IDC_R_VER)->GetWindowText(txt);
	if(!WritePrivateProfileString("Firmware", "Rev", txt, newName))
  {
    goto Error;
  }

  GetDlgItem(IDC_CRC)->GetWindowText(txt);
	if(!WritePrivateProfileString("Firmware", "CRC", txt, newName))
  {
    goto Error;
  }

  if(type == Phoenix)
  {
    GetDlgItem(IDC_CRC32_16M)->GetWindowText(txt);
	  if(!WritePrivateProfileString("Firmware", "CRC32", txt, newName))
    {
      goto Error;
    }
    GetDlgItem(IDC_CRC32_8M)->GetWindowText(txt);
	  if(!WritePrivateProfileString("Firmware", "CRC32_8M", txt, newName))
    {
      goto Error;
    }
  }

  GetDlgItem(IDC_BAUD_RATE)->GetWindowText(txt);
	if(!WritePrivateProfileString("Firmware", "Baudrate", txt, newName))
  {
    goto Error;
  }

  if(type == Phoenix)
  {
    //;GNSSTYPE: L1 + L2, GPS + BD
    //GNSSTYPE=25
    CFile f;
    if(FALSE == f.Open(newName, CFile::modeNoTruncate | CFile::modeWrite))
    {
      goto Error;
    }
    f.SeekToEnd();                

    GetDlgItem(IDC_GNSSTYPE_C)->GetWindowText(txt);
    source.Format(";%s\r\n", txt);
    f.Write((LPCSTR)source, source.GetLength());

    GetDlgItem(IDC_GNSSTYPE)->GetWindowText(txt);
    source.Format("GNSSTYPE=%s\r\n", txt);
    f.Write((LPCSTR)source, source.GetLength());

    f.Close();
  }

  if(type == V8)
  { //Only V8/V6 ini needs Doppler information.
    txt = "00000000";
	  if(!WritePrivateProfileString("Firmware", "DopplerAddress", txt, newName))
    {
      goto Error;
    }
	  if(!WritePrivateProfileString("Firmware", "DopplerSize", txt, newName))
    {
      goto Error;
    }
  }
  if(type == Phoenix)
  {
    if(((CButton*)GetDlgItem(IDC_TAG))->GetCheck())
    {
      GetDlgItem(IDC_TAG_ADDR)->GetWindowText(txt);
      if(!WritePrivateProfileString("Firmware", "Address", txt, newName))
      {
        goto Error;
      }
      
      GetDlgItem(IDC_TAG_VALUE)->GetWindowText(txt);
      if(!WritePrivateProfileString("Firmware", "Value", txt, newName))
      {
        goto Error;
      }
    }
  }

  if(type == V8)
  {
    if(((CButton*)GetDlgItem(IDC_TAG))->GetCheck())
    {
      GetDlgItem(IDC_TAG_ADDR2)->GetWindowText(txt);
      if(!WritePrivateProfileString("Firmware", "Address", txt, newName))
      {
        goto Error;
      }
      
      GetDlgItem(IDC_TAG_VALUE2)->GetWindowText(txt);
      if(!WritePrivateProfileString("Firmware", "Value", txt, newName))
      {
        goto Error;
      }
    }
    else
    {
      txt = "00000000";
      if(!WritePrivateProfileString("Firmware", "Address", txt, newName))
      {
        goto Error;
      }
      if(!WritePrivateProfileString("Firmware", "Value", txt, newName))
      {
        goto Error;
      }
    }
  }

  ShellExecute(NULL, _T("open"), newName, NULL, NULL, SW_SHOWNORMAL);
  //EndDialog(IDOK);
  return;

Error:
  ::AfxMessageBox("Failed to generate ini file!");
}

