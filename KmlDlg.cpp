// KmlDlg.cpp 
//

#include "stdafx.h"
#include "GPS.h"
#include "KmlDlg.h"

//#include "GPSDlg.h"
#define UWM_PROGRESS	(WM_USER + 0x26)

CKmlDlg* CKmlDlg::kmlDlg = NULL;
HANDLE	hReadEvent;

// CKmlDlg 

IMPLEMENT_DYNAMIC(CKmlDlg, CDialog)
CKmlDlg::CKmlDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKmlDlg::IDD, pParent)
{
	kmlDlg = this;
}

CKmlDlg::~CKmlDlg()
{

}

void CKmlDlg::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  DDX_Control(pDX, IDC_EDIT1, m_directory);
  DDX_Control(pDX, IDC_EDIT2, m_alltxt);
  DDX_Control(pDX, IDC_CHECK76, m_selectall);
  DDX_Control(pDX, IDC_CHECK78, m_hselectall);
  DDX_Control(pDX, IDC_CHECK1, m_check[0]);
  DDX_Control(pDX, IDC_CHECK2, m_check[1]);
  DDX_Control(pDX, IDC_CHECK3, m_check[2]);
  DDX_Control(pDX, IDC_CHECK4, m_check[3]);
  DDX_Control(pDX, IDC_CHECK5, m_check[4]);
  DDX_Control(pDX, IDC_CHECK6, m_check[5]);
  DDX_Control(pDX, IDC_CHECK7, m_check[6]);
  DDX_Control(pDX, IDC_CHECK8, m_check[7]);
  DDX_Control(pDX, IDC_CHECK9, m_check[8]);
  DDX_Control(pDX, IDC_CHECK10, m_check[9]);	
  DDX_Control(pDX, IDC_CHECK11, m_check[10]);	
  DDX_Control(pDX, IDC_CHECK12, m_check[11]);	
  DDX_Control(pDX, IDC_CHECK13, m_check[12]);	
  DDX_Control(pDX, IDC_CHECK14, m_check[13]);
  DDX_Control(pDX, IDC_CHECK15, m_check[14]);	
  DDX_Control(pDX, IDC_CHECK16, m_check[15]);	
  DDX_Control(pDX, IDC_CHECK17, m_check[16]);	
  DDX_Control(pDX, IDC_CHECK18, m_check[17]);	
  DDX_Control(pDX, IDC_CHECK19, m_check[18]);	
  DDX_Control(pDX, IDC_CHECK20, m_check[19]);	
  DDX_Control(pDX, IDC_CHECK21, m_check[20]);	
  DDX_Control(pDX, IDC_CHECK22, m_check[21]);	
  DDX_Control(pDX, IDC_CHECK23, m_check[22]);	
  DDX_Control(pDX, IDC_CHECK24, m_check[23]);	
  DDX_Control(pDX, IDC_CHECK25, m_check[24]);	
  DDX_Control(pDX, IDC_CHECK26, m_check[25]);	
  DDX_Control(pDX, IDC_CHECK27, m_check[26]);	
  DDX_Control(pDX, IDC_CHECK28, m_check[27]);	
  DDX_Control(pDX, IDC_CHECK29, m_check[28]);	
  DDX_Control(pDX, IDC_CHECK30, m_check[29]);	
  DDX_Control(pDX, IDC_CHECK31, m_check[30]);	
  DDX_Control(pDX, IDC_CHECK32, m_check[31]);	
  DDX_Control(pDX, IDC_CHECK33, m_check[32]);	
  DDX_Control(pDX, IDC_CHECK34, m_check[33]);	
  DDX_Control(pDX, IDC_CHECK35, m_check[34]);	
  DDX_Control(pDX, IDC_CHECK36, m_check[35]);	
  DDX_Control(pDX, IDC_CHECK37, m_check[36]);	
  DDX_Control(pDX, IDC_CHECK38, m_check[37]);	
  DDX_Control(pDX, IDC_CHECK39, m_check[38]);	
  DDX_Control(pDX, IDC_CHECK40, m_check[39]);	
  DDX_Control(pDX, IDC_CHECK41, m_check[40]);	
  DDX_Control(pDX, IDC_CHECK42, m_check[41]);	
  DDX_Control(pDX, IDC_CHECK43, m_check[42]);	
  DDX_Control(pDX, IDC_CHECK44, m_check[43]);	
  DDX_Control(pDX, IDC_CHECK45, m_check[44]);	
  DDX_Control(pDX, IDC_CHECK46, m_check[45]);	
  DDX_Control(pDX, IDC_CHECK47, m_check[46]);	
  DDX_Control(pDX, IDC_CHECK48, m_check[47]);	
  DDX_Control(pDX, IDC_CHECK49, m_check[48]);	
  DDX_Control(pDX, IDC_CHECK50, m_check[49]);	
  DDX_Control(pDX, IDC_CHECK51, m_check[50]);	
  DDX_Control(pDX, IDC_CHECK52, m_check[51]);	
  DDX_Control(pDX, IDC_CHECK53, m_check[52]);	
  DDX_Control(pDX, IDC_CHECK54, m_check[53]);	
  DDX_Control(pDX, IDC_CHECK55, m_check[54]);	
  DDX_Control(pDX, IDC_CHECK56, m_check[55]);	
  DDX_Control(pDX, IDC_CHECK57, m_check[56]);	
  DDX_Control(pDX, IDC_CHECK58, m_check[57]);	
  DDX_Control(pDX, IDC_CHECK59, m_check[58]);	
  DDX_Control(pDX, IDC_CHECK60, m_check[59]);	
  DDX_Control(pDX, IDC_CHECK61, m_check[60]);
  DDX_Control(pDX, IDC_CHECK62, m_check[61]);	
  DDX_Control(pDX, IDC_CHECK63, m_check[62]);	
  DDX_Control(pDX, IDC_CHECK64, m_check[63]);	
  DDX_Control(pDX, IDC_CHECK65, m_check[64]);	
  DDX_Control(pDX, IDC_CHECK66, m_check[65]);	
  DDX_Control(pDX, IDC_CHECK67, m_check[66]);	
  DDX_Control(pDX, IDC_CHECK68, m_check[67]);	
  DDX_Control(pDX, IDC_CHECK69, m_check[68]);	
  DDX_Control(pDX, IDC_CHECK70, m_check[69]);
  DDX_Control(pDX, IDC_CHECK71, m_check[70]);	
  DDX_Control(pDX, IDC_CHECK72, m_check[71]);	
  DDX_Control(pDX, IDC_CHECK73, m_check[72]);	
  DDX_Control(pDX, IDC_CHECK74, m_check[73]);
  DDX_Control(pDX, IDC_CHECK75, m_check[74]);	
  DDX_Control(pDX, IDC_COMBO1, m_color);
  DDX_Control(pDX, IDC_PROGRESS1, m_progress);
  DDX_Control(pDX, IDC_FILE_LIST, m_fileList);
}


BEGIN_MESSAGE_MAP(CKmlDlg, CDialog)
	ON_BN_CLICKED(IDC_BPLAYER, OnBnClickedBplayer)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDC_CHECK76, OnBnClickedCheck76)
	ON_BN_CLICKED(IDC_CHECK78, OnBnClickedCheck78)
	ON_BN_CLICKED(IDC_CHECK_ALL, OnBnClickedCheckAll)
	ON_WM_LBUTTONDOWN()
	ON_MESSAGE(UWM_PROGRESS, OnProgress)
  ON_LBN_SELCHANGE(IDC_FILE_LIST, &CKmlDlg::OnLbnSelchangeFileList)
  ON_CLBN_CHKCHANGE(IDC_FILE_LIST, &CKmlDlg::OnLbnChkchangeFileList)
END_MESSAGE_MAP()

// CKmlDlg 

BOOL CKmlDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	for(int i=0;i<NumOfCheckBox;i++)
	{
		m_check[i].ShowWindow(SW_HIDE);
	}
	m_alltxt.ShowWindow(SW_HIDE);	

	isConvertFinish = false;
	m_color.SetCurSel(0);
  if(KML_USE_CHECKLISTBOX)
  {
    GetDlgItem(IDC_GROUP)->ShowWindow(SW_HIDE);
    GetDlgItem(IDC_PROMPT)->ShowWindow(SW_SHOW);
    GetDlgItem(IDC_CHECK_ALL)->ShowWindow(SW_SHOW);
    m_selectall.ShowWindow(SW_HIDE);
    m_hselectall.ShowWindow(SW_HIDE);
  }

	return TRUE;  // return TRUE unless you set the focus to a control
}
UINT ConvertToKml(LPVOID pParam)
{	
	CKmlDlg::kmlDlg->convert_all_file();
	//AfxEndThread(0);	
	return 0;
}
/*
UINT ContinueToConvert(LPVOID pParam)
{
	CKmlDlg::kmlDlg->SetConvertFinish(false);
	WaitForSingleObject(hReadEvent,INFINITE);
	if(hReadEvent)
	{	
		CloseHandle(hReadEvent);
		hReadEvent = NULL;
	}	
	if(CKmlDlg::kmlDlg->NoOfFile < CKmlDlg::kmlDlg->NumOfFile)	
	{
		CKmlDlg::kmlDlg->OnBnClickedOk();	
	}
	else 
	{
		CKmlDlg::kmlDlg->SetConvertFinish(true); 
		CKmlDlg::kmlDlg->OnBnClickedCheck76(); 
		CKmlDlg::kmlDlg->NoOfFile = 0;
	}

	return 0;
}
*/


void CKmlDlg::OnBnClickedBplayer()
{
	CString fileName;
	CFileDialog dlgFile(true, _T("*.txt"), NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, _T("*.txt|*.txt||"), this);

	INT_PTR nResult = dlgFile.DoModal();
	if(nResult != IDOK)
	{
		return;
	}
  
	fileName = dlgFile.GetPathName();
	m_directory.SetWindowText(fileName);
	sPath = Utility::GetFilePath(fileName) + "\\";
	for(int i = 0; i < NumOfCheckBox; ++i)
	{
		m_check[i].SetCheck(BST_UNCHECKED);
	}

  GetDataFiles(m_selectall.GetCheck() == BST_CHECKED);
	//NoOfFile = 0;
  if(KML_USE_CHECKLISTBOX)
  {
    ((CButton*)GetDlgItem(IDC_CHECK_ALL))->SetCheck(BST_CHECKED);
  }
  else
  {
    m_hselectall.SetCheck(BST_CHECKED);
  }
}

void CKmlDlg::GetDataFiles(BOOL autoChecked)
{
    CFileFind finder;
    CString sFilterName = sPath + "*.*";
    int num = 0;
    BOOL bExist = finder.FindFile(sFilterName);
    m_fileList.ResetContent();
    while (bExist) {
        bExist = finder.FindNextFile(); 
        if(finder.IsDirectory())
        {
          continue;
        }

        CString ext = Utility::GetFileExt(finder.GetFileName());
        ext.MakeLower();
        if(ext != "txt" && ext != "out")
        {
          continue;
        }
        FileName[num] = finder.GetFileName();
			  FilePath[num] = sPath + finder.GetFileName();
        if(autoChecked == FALSE && num < NumOfCheckBox)
        {
          if(KML_USE_CHECKLISTBOX)
          {
            m_fileList.AddString(FileName[num]);
            m_fileList.SetCheck(num, BST_CHECKED);
          }
          else
          {
            m_check[num].SetWindowText(FileName[num]);
            m_check[num].ShowWindow(SW_SHOW);
            m_check[num].SetCheck(BST_CHECKED);	
          }
        }
        ++num;
    }
    finder.Close();
	  NumOfFile = num + 1;

    if(!KML_USE_CHECKLISTBOX)
    {
      if(autoChecked)
      {
	      CString msg, txt;
        msg.Format("Total (#%d) output files selected.\r\n\r\n", NumOfFile);

	      for(int i = 0; i < NumOfFile; ++i)
	      {		
		      int x = i%10;
          txt.Format("%s%s", FileName[i], (i != 0 && (i % 10) == 0) ? " \r\n" : " ");
  	      msg += txt;
	      }
	      m_alltxt.SetWindowText(msg);
      }
      else
      {
	      for(int i = NumOfFile; i < NumOfCheckBox; ++i)
        {
          m_check[i].ShowWindow(SW_HIDE);
        }
      }
    }
}

void CKmlDlg::OnBnClickedOk()
{
	memset(&msg_gpgga, 0, sizeof(GPGGA));
	memset(&msg_gprmc, 0, sizeof(GPRMC));
	isConvertFinish = false;

  hReadEvent = ::CreateEvent(NULL, TRUE, FALSE, NULL);
	if(!ResetEvent(hReadEvent))
	{
		DWORD error = GetLastError();
	}

	CString fileName;	
	m_directory.GetWindowText(fileName);
	if(fileName.IsEmpty())
	{
	  AfxMessageBox("您尚未選擇任何檔案", MB_YESNOCANCEL | MB_ICONQUESTION);
    return;
	}

  if(KML_USE_CHECKLISTBOX)
  {
    int num = 0;
    for(int i = 0; i < m_fileList.GetCount(); ++i)
    {
      if(m_fileList.GetCheck(i) == BST_CHECKED)
      {
		    FileName[num] = FileName[i];
			  FilePath[num] = FilePath[i];
			  ++num;
      }
    }
		NumOfFile = num;
    for(int i = m_fileList.GetCount() - 1; i >= 0; --i)
    {
      if(m_fileList.GetCheck(i) == BST_UNCHECKED)
      {
        m_fileList.DeleteString(i);
      }
      else
      {
        m_fileList.SetCheck(i, BST_UNCHECKED);
      }
    }
    m_fileList.SetCurSel(-1);
  }
  else
  {
	  if(m_selectall.GetCheck() == BST_UNCHECKED)
	  {
      int numbers = 0;
		  for(int i=0; i<NumOfCheckBox; ++i)
		  {
			  if(m_check[i].GetCheck() == BST_CHECKED)
			  {
				  FileName[numbers] = FileName[i];
				  FilePath[numbers] = FilePath[i];
				  numbers++;
			  }
		  }
		  NumOfFile = numbers;
    }
  }

	if(!NumOfFile) 
	{
		return;
	}

  if(!KML_USE_CHECKLISTBOX)
  {
	  for(int i=0; i<NumOfCheckBox; ++i) 
	  {
		  m_check[i].SetCheck(BST_UNCHECKED);	
		  m_check[i].SetWindowText(FileName[i]);
	  }

	  for(int i=NumOfFile; i<NumOfCheckBox; ++i)
	  {
		  m_check[i].ShowWindow(SW_HIDE);
	  }
  }
	AfxBeginThread(ConvertToKml, 0);	
}

void CKmlDlg::convert_all_file()
{
	int i;
	CString temp;
	CKmlDlg::kmlDlg->PostMessage(UWM_PROGRESS, 1, 0);	//Show progress
	for(i = 0; i < NumOfFile; ++i)
	{
		CFileException ef;
		CFile f;
		try
		{
      if(KML_USE_CHECKLISTBOX)
      {
        m_fileList.SetCurSel(i);
      }

			if(!f.Open(FilePath[i], CFile::modeRead,&ef))
			{
				ef.ReportError();
				continue;
			}

			kml_filename = FilePath[i];
			int find = kml_filename.ReverseFind('.');
			kml_filename = kml_filename.Mid(0,find);   

			//Convert(f);
			Convert2(f);
			f.Close();

			temp.Format("%s OK", FileName[i]);
      if(KML_USE_CHECKLISTBOX)
      {
        //m_fileList.SetCurSel(i);
        m_fileList.DeleteString(i);
        m_fileList.InsertString(i, temp);
        m_fileList.SetCheck(i, BST_CHECKED);
      }
      else
      {
			  m_check[i].SetWindowText(temp);
			  m_check[i].SetCheck(BST_CHECKED);	
      }
		}
		catch(CFileException *fe)
		{
			fe->ReportError();
			fe->Delete();
		}	
	}
	CKmlDlg::kmlDlg->PostMessage(UWM_PROGRESS, 0, 1000);	//Hide progress
}

void CKmlDlg::OnBnClickedCheck76()
{
	char nmea[100];
	CString temp;
	for(int i=0;i<NumOfCheckBox;i++)m_check[i].ShowWindow(0);	
	m_alltxt.ShowWindow(1);	
	CString fileName;

	m_directory.GetWindowText(fileName);
	if(fileName.IsEmpty())	
	{
		return;	
	}

	if(m_selectall.GetCheck() == BST_UNCHECKED)
	{
		m_alltxt.ShowWindow(0);
		if(isConvertFinish)
		{
			for(int i=0;i<NumOfFile;i++)
			{
				m_check[i].ShowWindow(1);
				m_check[i].SetCheck(BST_CHECKED);
				fileName = "OK ";
				fileName+=FileName[i];
				m_check[i].SetWindowText(fileName);
			}

			for(int i=NumOfFile;i<NumOfCheckBox;i++)
			{
				m_check[i].ShowWindow(0);
			}
		} //if(IsConvertFinish)
		else
		{
			//GetDatFile();
      GetDataFiles(FALSE);
		} //if(IsConvertFinish)
	} //if(m_selectall.GetCheck() == BST_UNCHECKED)
	else if(m_selectall.GetCheck() == BST_CHECKED)
	{
		for(int i=0;i<NumOfCheckBox;i++)
		{
			m_check[i].ShowWindow(0);
		}
		m_alltxt.ShowWindow(1);	
		if(isConvertFinish)
		{
			sprintf_s(nmea, sizeof(nmea), "Total (#%d) txt files are selected.\r\n\r\n", NumOfFile);
			temp = nmea;
			temp += "successfully";					
			m_alltxt.SetWindowText(temp);
		}
		else
		{
			//GetAllDatFile();
      GetDataFiles(TRUE);
		}
	} //else if(m_selectall.GetCheck() == BST_CHECKED)
}

void CKmlDlg::OnBnClickedCheck78()
{
	m_alltxt.ShowWindow(0);	
	CString fileName;
	m_directory.GetWindowText(fileName);
	if(fileName == "")	return;	
	else
	{
		if(m_hselectall.GetCheck() == BST_UNCHECKED)
		{
			for(int i=0;i<NumOfFile;i++){m_check[i].SetCheck(BST_UNCHECKED);m_check[i].ShowWindow(1);m_check[i].SetWindowText(FileName[i]);}
			for(int i=NumOfFile;i<NumOfCheckBox;i++)m_check[i].ShowWindow(0);				
		}
		else if(m_hselectall.GetCheck() == BST_CHECKED)
		{
			for(int i=0;i<NumOfFile;i++){ m_check[i].SetCheck(BST_CHECKED);m_check[i].ShowWindow(1);m_check[i].SetWindowText(FileName[i]);}		
			for(int i=NumOfFile;i<NumOfCheckBox;i++)m_check[i].ShowWindow(0);					
		}
	}
}

void CKmlDlg::OnBnClickedCheckAll()
{
	CString fileName;
	m_directory.GetWindowText(fileName);
	if(fileName == "")	
  {
    return;
  }

  int c = ((CButton*)GetDlgItem(IDC_CHECK_ALL))->GetCheck();
  for(int i = 0; i < m_fileList.GetCount(); ++i)
  {
    m_fileList.SetCheck(i, (c) ? BST_CHECKED : BST_UNCHECKED);
  }
/*
		if(m_hselectall.GetCheck() == BST_UNCHECKED)
		{
			for(int i=0;i<NumOfFile;i++)
      {
        m_check[i].SetCheck(BST_UNCHECKED);
        m_check[i].ShowWindow(1);
        m_check[i].SetWindowText(FileName[i]);
      }
			for(int i=NumOfFile;i<NumOfCheckBox;i++)m_check[i].ShowWindow(0);				
		}
		else if(m_hselectall.GetCheck() == BST_CHECKED)
		{
			for(int i=0;i<NumOfFile;i++)
      { 
        m_check[i].SetCheck(BST_CHECKED);
        m_check[i].ShowWindow(1);
        m_check[i].SetWindowText(FileName[i]);
      }		
			for(int i=NumOfFile;i<NumOfCheckBox;i++)m_check[i].ShowWindow(0);					
		}
*/
}




int CKmlDlg::GET_NMEA_SENTENCE(CFile& file, unsigned char* buffer)
{
	unsigned char nmea[2];
	unsigned int   length=0;
	int nBytesRead = 0;
  do
	{
		memset(nmea,0,2);
		nBytesRead = file.Read(nmea,1);		
		*(buffer+length) = nmea[0];
		length += nBytesRead;
		if(nBytesRead == 0 || *(buffer+length-1)=='\n' && *(buffer+length-2)=='\r' || length == 200)
			break;		
	} while(1);
	return length;
}

U08 CKmlDlg::NMEA_PROC(const char* buffer, int offset)
{
	offset = NMEA::TrimTail(buffer, offset);
	U08 MsgType = NMEA::MessageType(buffer, offset);
	
	switch(MsgType)
	{
	case MSG_ERROR:
		break;	
	case MSG_GGA:
		nmea.ShowGPGGAmsg(msg_gpgga,buffer,offset);		
    break;	
	case MSG_RMC:
		nmea.ShowGPRMCmsg(msg_gprmc,buffer,offset);		
		break;	
	case MSG_GNS:		
		nmea.ShowGNSmsg(msg_gpgga, buffer, offset);
		break; 
	case MSG_GPGSA:
		nmea.ShowGNGSAmsg(msg_gpgsa, msg_glgsa, msg_bdgsa, msg_gagsa, msg_gigsa, buffer, offset);
		kml.msg_gpgsa = &msg_gpgsa;
		break;
	case MSG_GLGSA:
		nmea.ShowGLGSAmsg(msg_glgsa, buffer, offset);
		kml.msg_glgsa = &msg_glgsa;
		break;
	case MSG_GNGSA:
		nmea.ShowGNGSAmsg(msg_gpgsa, msg_glgsa, msg_bdgsa, msg_gagsa, msg_gigsa, buffer, offset);
		kml.msg_gpgsa = &msg_gpgsa;
		kml.msg_glgsa = &msg_glgsa;
		kml.msg_bdgsa = &msg_bdgsa;
		kml.msg_gagsa = &msg_gagsa;
		kml.msg_gigsa = &msg_gigsa;
		break;
	case MSG_BDGSA:
		nmea.ShowBDGSAmsg(msg_bdgsa, buffer, offset);
		kml.msg_bdgsa = &msg_bdgsa;
		break;
	case MSG_GAGSA:
		nmea.ShowGAGSAmsg(msg_gagsa, buffer, offset);
		kml.msg_gagsa = &msg_gagsa;
		break;
	case MSG_GIGSA:
		nmea.ShowGIGSAmsg(msg_gigsa, buffer, offset);
		kml.msg_gigsa = &msg_gigsa;
		break;
	case MSG_GPGSV:
		nmea.ShowGPGSVmsg2(msg_gpgsv, msg_glgsv, msg_bdgsv, msg_gagsv, msg_gigsv, buffer, offset);
		kml.msg_gpgsv = &msg_gpgsv;
		kml.satellites_gp = &nmea.satellites_gp;
		break;
	case MSG_GLGSV:
		nmea.ShowGLGSVmsg(msg_glgsv, buffer, offset);
		kml.msg_glgsv = &msg_glgsv;
		kml.satellites_gl = &nmea.satellites_gl;
		break;
	case MSG_BDGSV:
		nmea.ShowBDGSVmsg(msg_bdgsv, buffer, offset);
		kml.msg_bdgsv = &msg_bdgsv;
		kml.satellites_bd = &nmea.satellites_bd;
		break;
	case MSG_GAGSV:
		nmea.ShowGAGSVmsg(msg_gagsv, buffer, offset);
		kml.msg_gagsv = &msg_gagsv;
		kml.satellites_ga = &nmea.satellites_ga;
		break;
	case MSG_GIGSV:
		nmea.ShowGIGSVmsg(msg_gigsv, buffer, offset);
		kml.msg_gigsv = &msg_gigsv;
		kml.satellites_gi = &nmea.satellites_gi;
		break;
	case MSG_GNGSV:
		//nmea.ShowGAGSVmsg(msg_gagsv, buffer, offset);
		//kml.msg_gagsv = &msg_gagsv;
		//kml.satellites_ga = nmea.satellites_ga;
		break;

	default :
		break;
	}
	return MsgType;	
}

void CKmlDlg::Convert(CFile& f)
{
	int nBytesRead;
	UINT retCode = 0;	
	U08 nmea[200];
	CString temp;
	ULONGLONG  dwBytesRemaining = f.GetLength();
	U08 nmeaType;

	int write_count = 0;
	int file_tail = 0;
	int b3d = ((CButton*)GetDlgItem(IDC_3DKML))->GetCheck();
	int bPointList = ((CButton*)GetDlgItem(IDC_POINTLIST))->GetCheck();
	int bNoPointText = ((CButton*)GetDlgItem(IDC_NO_TITLE))->GetCheck();
	int bDetailInfo = ((CButton*)GetDlgItem(IDC_DETAIL_INFO))->GetCheck();
	int color_index = m_color.GetCurSel(); 
	//Red;Yellow;Blue;Green;
	const U32 colors[] = {0x0000ff, 0x00FFFF, 0xff0000, 0x00ff00};
	U32 color = colors[color_index];

	ut = Unknown;
	int lastProgress = 0;
	while(dwBytesRemaining)
	{	
		CString tmp_file;
		tmp_file.Format("%s%d%s", kml_filename,file_tail, ".kml");
		
		kml.Init(tmp_file, color, (b3d==1), (bPointList==1), (bNoPointText==1), (bDetailInfo==1));
		while(dwBytesRemaining)
		{
			int progress = (int)(((double)(f.GetLength() - dwBytesRemaining) / f.GetLength()) * 1000);
			if(progress != lastProgress)
			{
				lastProgress = progress;
				CKmlDlg::kmlDlg->PostMessage(UWM_PROGRESS, 1, lastProgress);	//Show progress
			}

			memset(nmea, 0, 200);		
			nBytesRead = GET_NMEA_SENTENCE(f, nmea);						
			nmeaType = NMEA_PROC((const char*)nmea, nBytesRead - 1);			    	
			dwBytesRemaining-=nBytesRead;	
			if((nmeaType == MSG_GGA || nmeaType == MSG_RMC || nmeaType == MSG_GNS) && WriteToFile(nmeaType)) 
			{
				write_count++;
				if ( write_count > 65000)
				{
					write_count = 0;
					file_tail++;
					break;
				}
			}
		}
		kml.Finish();
		CKmlDlg::kmlDlg->PostMessage(UWM_PROGRESS, 1, 1000);
		lastProgress = 1000;

	}	
	if(!SetEvent(hReadEvent))	
	{
		DWORD error = GetLastError();
	}
}

void CKmlDlg::Convert2(CFile& f)
{
	int nBytesRead;
	UINT retCode = 0;	
	U08 nmea[200];
	CString temp;
	ULONGLONG  dwBytesRemaining = f.GetLength();
	U08 nmeaType;

	int write_count = 0;
	int file_tail = 0;
	int b3d = ((CButton*)GetDlgItem(IDC_3DKML))->GetCheck();
	int bPointList = ((CButton*)GetDlgItem(IDC_POINTLIST))->GetCheck();
	int bNoPointText = ((CButton*)GetDlgItem(IDC_NO_TITLE))->GetCheck();
	int bDetailInfo = ((CButton*)GetDlgItem(IDC_DETAIL_INFO))->GetCheck();
	int color_index = m_color.GetCurSel(); 
	//Red;Yellow;Blue;Green;
	const U32 colors[] = {0x0000ff, 0x00FFFF, 0xff0000, 0x00ff00};
	U32 color = colors[color_index];

	ut = Unknown;
	int lastProgress = 0;
	while(dwBytesRemaining)
	{	
		CString tmp_file;
		tmp_file.Format("%s%d%s", kml_filename,file_tail, ".kml");
		
		kml.Init(tmp_file, color, (b3d==1), (bPointList==1), (bNoPointText==1), (bDetailInfo==1));
		while(dwBytesRemaining)
		{
			int progress = (int)(((double)(f.GetLength() - dwBytesRemaining) / f.GetLength()) * 1000);
			if(progress != lastProgress)
			{
				lastProgress = progress;
				CKmlDlg::kmlDlg->PostMessage(UWM_PROGRESS, 1, lastProgress);	//Show progress
			}

			memset(nmea, 0, 200);		
			nBytesRead = GET_NMEA_SENTENCE(f, nmea);						
			nmeaType = NMEA_PROC((const char*)nmea, nBytesRead - 1);			    	
			dwBytesRemaining-=nBytesRead;	
			if((nmeaType == MSG_GGA || nmeaType == MSG_RMC || nmeaType == MSG_GNS) && WriteToFile2(nmeaType)) 
			{
				write_count++;
				if ( write_count > 65000)
				{
					write_count = 0;
					file_tail++;
					break;
				}
			}
		}
		kml.Finish2();
		CKmlDlg::kmlDlg->PostMessage(UWM_PROGRESS, 1, 1000);
		lastProgress = 1000;

	}	
	if(!SetEvent(hReadEvent))	
	{
		DWORD error = GetLastError();
	}
}

double GetLon(D64 longitude, char ew)
{
	double lon = (int)(longitude / 100);	
	lon += double(longitude - (int)(longitude / 100) * 100) / 60;	
	if(ew == 'W')
	{
		lon *= -1;
	}
	return lon;
}

double GetLat(D64 latitude, char ns)
{
	double lat = (int)(latitude / 100);	
	lat += (double)(latitude - (int)(latitude / 100) * 100) / 60;	
	if(ns == 'S')
	{
		lat *= -1;
	}
	return lat;
}

bool CKmlDlg::WriteToFile(U08 type)
{	
	static U16 last_hh = 0;
	static U16 last_mm = 0;
	static F32 last_ss = -1;

	if(ut == Unknown)
	{
		if(IsFixed(msg_gpgga.GPSQualityIndicator))
			ut = UsingGGA;
		if(IsFixed(msg_gprmc.Status))
			ut = UsingRMC;
	}

	if(ut == UsingGGA && last_hh == msg_gpgga.Hour && last_mm == msg_gpgga.Min && last_ss == msg_gpgga.Sec)
	{
		return false;
	}
	if(ut == UsingRMC && last_hh == msg_gprmc.Hour && last_mm == msg_gprmc.Min && last_ss == msg_gprmc.Sec)
	{
		return false;
	}

	if(ut == UsingGGA && (type==MSG_GGA || type==MSG_GNS) && IsFixed(msg_gpgga.GPSQualityIndicator))
	{
    CString timeStr;
		timeStr.Format("%02d:%02d:%05.2f", msg_gpgga.Hour, msg_gpgga.Min, msg_gpgga.Sec);
		kml.PushOnePoint(GetLon(msg_gpgga.Longitude, msg_gpgga.Longitude_E_W), 
			GetLat(msg_gpgga.Latitude, msg_gpgga.Latitude_N_S), 
      msg_gpgga.Altitude, 
      timeStr, 
      GetGnssQualityMode(msg_gpgga.GPSQualityIndicator, (U08)msg_gpgsa.Mode, (U08)msg_glgsa.Mode, (U08)msg_gagsa.Mode, (U08)msg_bdgsa.Mode, (U08)msg_gigsa.Mode));
		last_hh = msg_gpgga.Hour;
		last_mm = msg_gpgga.Min;
		last_ss = msg_gpgga.Sec;
		return true;
	}
	else if (ut == UsingRMC && type==MSG_RMC && msg_gprmc.Status == 'A')
	{
		CString timeStr;
		timeStr.Format("%02d:%02d:%05.2f", msg_gprmc.Hour, msg_gprmc.Min, msg_gprmc.Sec);
		kml.PushOnePoint(GetLon(msg_gprmc.Longitude, msg_gprmc.Longitude_E_W), 
			GetLat(msg_gprmc.Latitude, msg_gprmc.Latitude_N_S), 
      msg_gpgga.Altitude, 
      timeStr, 
      GetGnssQualityMode(msg_gprmc.ModeIndicator));
		last_hh = msg_gprmc.Hour;
		last_mm = msg_gprmc.Min;
		last_ss = msg_gprmc.Sec;
		return true;
	}
	return false;
}


bool CKmlDlg::WriteToFile2(U08 type)
{	
	static U16 last_hh = 0;
	static U16 last_mm = 0;
	static F32 last_ss = -1;

	if(ut == Unknown)
	{
		if(IsFixed(msg_gpgga.GPSQualityIndicator))
			ut = UsingGGA;
		if(IsFixed(msg_gprmc.Status))
			ut = UsingRMC;
	}

	//if(ut == UsingGGA && last_hh == msg_gpgga.Hour && last_mm == msg_gpgga.Min && last_ss == msg_gpgga.Sec)
	//{
	//	return false;
	//}
	//if(ut == UsingRMC && last_hh == msg_gprmc.Hour && last_mm == msg_gprmc.Min && last_ss == msg_gprmc.Sec)
	//{
	//	return false;
	//}

	if((type == MSG_GGA || type == MSG_GNS) && IsFixed(msg_gpgga.GPSQualityIndicator))
	{
    CString timeStr;
		timeStr.Format("%02d:%02d:%05.2f", msg_gpgga.Hour, msg_gpgga.Min, msg_gpgga.Sec);
		kml.PushOnePoint2(GetLon(msg_gpgga.Longitude, msg_gpgga.Longitude_E_W), 
			GetLat(msg_gpgga.Latitude, msg_gpgga.Latitude_N_S), 
      msg_gpgga.Altitude, 
      NULL,
      NULL,
      timeStr, 
      GetGnssQualityMode(msg_gpgga.GPSQualityIndicator, (U08)msg_gpgsa.Mode, (U08)msg_glgsa.Mode, (U08)msg_gagsa.Mode, (U08)msg_bdgsa.Mode, (U08)msg_gigsa.Mode));
		last_hh = msg_gpgga.Hour;
		last_mm = msg_gpgga.Min;
		last_ss = msg_gpgga.Sec;
		return true;
	}
	else if (type == MSG_RMC && msg_gprmc.Status == 'A')
	{
		CString timeStr;
		timeStr.Format("%02d:%02d:%05.2f", msg_gprmc.Hour, msg_gprmc.Min, msg_gprmc.Sec);
		kml.PushOnePoint2(GetLon(msg_gprmc.Longitude, msg_gprmc.Longitude_E_W), 
			GetLat(msg_gprmc.Latitude, msg_gprmc.Latitude_N_S), 
      msg_gpgga.Altitude, 
      &msg_gprmc.SpeedKnots,
      &msg_gprmc.TrueCourse,
      timeStr,
      GetGnssQualityMode(msg_gprmc.ModeIndicator));
		last_hh = msg_gprmc.Hour;
		last_mm = msg_gprmc.Min;
		last_ss = msg_gprmc.Sec;
		return true;
	}
	return false;
}


void CKmlDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
	CDialog::OnLButtonDown(nFlags, point);
}

BOOL CKmlDlg::PreTranslateMessage(MSG* pMsg)
{
	return CDialog::PreTranslateMessage(pMsg);
}

BOOL CKmlDlg::OnCommand(WPARAM wParam, LPARAM lParam)
{
	if(wParam ==  IDC_CHECK1 ||	wParam ==  IDC_CHECK2 ||
	   wParam ==  IDC_CHECK3 ||	wParam ==  IDC_CHECK4 ||
	   wParam ==  IDC_CHECK5 || wParam ==  IDC_CHECK6 ||
	 wParam ==  IDC_CHECK7 ||
	 wParam ==  IDC_CHECK8 ||
	 wParam ==  IDC_CHECK9 ||
	 wParam ==  IDC_CHECK10 ||
	 wParam ==  IDC_CHECK11 ||
	 wParam ==  IDC_CHECK12 ||
	 wParam ==  IDC_CHECK13 ||
	 wParam ==  IDC_CHECK14 ||
	 wParam ==  IDC_CHECK15 ||
	 wParam ==  IDC_CHECK16 ||
	 wParam ==  IDC_CHECK17 ||
	 wParam ==  IDC_CHECK18 ||
	 wParam ==  IDC_CHECK19 ||
	 wParam ==   IDC_CHECK20 ||
	 wParam ==  IDC_CHECK21 ||
	 wParam ==  IDC_CHECK22 ||
	 wParam ==   IDC_CHECK23 ||
	 wParam ==   IDC_CHECK24 ||
	 wParam ==   IDC_CHECK25 ||
	 wParam ==   IDC_CHECK26 ||
	 wParam ==   IDC_CHECK27 ||
	 wParam ==  IDC_CHECK28 ||
	 wParam ==   IDC_CHECK29 ||
	 wParam ==   IDC_CHECK30 ||
	 wParam ==   IDC_CHECK31 ||
	 wParam ==   IDC_CHECK32 ||
	 wParam ==  IDC_CHECK33 ||
	 wParam ==   IDC_CHECK34 ||
	 wParam ==   IDC_CHECK35 ||
	 wParam ==  IDC_CHECK36 ||
	 wParam ==   IDC_CHECK37 ||
	 wParam ==   IDC_CHECK38 ||
	 wParam ==   IDC_CHECK39 ||
	 wParam ==   IDC_CHECK40 ||
	 wParam ==   IDC_CHECK41 ||
	 wParam ==  IDC_CHECK42 ||
	 wParam ==   IDC_CHECK43 ||
	 wParam ==   IDC_CHECK44 ||
	 wParam ==  IDC_CHECK45 ||
	 wParam ==   IDC_CHECK46 ||
	 wParam ==   IDC_CHECK47 ||
	 wParam ==   IDC_CHECK48 ||
	 wParam ==   IDC_CHECK49 ||
	 wParam ==   IDC_CHECK50 ||
	 wParam ==  IDC_CHECK51 ||
	 wParam ==   IDC_CHECK52 ||
	 wParam ==   IDC_CHECK53 ||
	 wParam ==   IDC_CHECK54 ||
	 wParam ==  IDC_CHECK55 ||
	 wParam ==  IDC_CHECK56 ||
	 wParam ==   IDC_CHECK57 ||
	 wParam ==   IDC_CHECK58 ||
	 wParam ==  IDC_CHECK59 ||
	 wParam ==   IDC_CHECK60 ||
	 wParam ==   IDC_CHECK61 ||
	 wParam ==   IDC_CHECK62 ||
	 wParam ==   IDC_CHECK63 ||
	 wParam ==   IDC_CHECK64 ||
	 wParam ==   IDC_CHECK65 ||
	 wParam ==   IDC_CHECK66 ||
	 wParam ==   IDC_CHECK67 ||
	 wParam ==   IDC_CHECK68 ||
	 wParam ==  IDC_CHECK69 ||
	 wParam ==   IDC_CHECK70 ||
	 wParam ==   IDC_CHECK71 ||
	 wParam ==   IDC_CHECK72 ||
	 wParam ==  IDC_CHECK73 ||
	 wParam ==   IDC_CHECK74 ||
	 wParam ==   IDC_CHECK75)
	{
		m_hselectall.SetCheck(BST_UNCHECKED);

	}

	return CDialog::OnCommand(wParam, lParam);
}

LRESULT CKmlDlg::OnProgress(WPARAM wParam, LPARAM lParam) 
{
    BOOL bShowProgress = (BOOL)wParam;
    int bProgress = (int)lParam;
	m_progress.ShowWindow((wParam) ?SW_SHOW :SW_HIDE);
	m_progress.SetRange(0, 1000);
	m_progress.SetPos((int)lParam);
    return 0L;
}

void CKmlDlg::OnLbnSelchangeFileList()
{
  int s = m_fileList.GetCurSel();
  int c = m_fileList.GetCheck(s);
  m_fileList.SetCheck(s, (c) ? BST_UNCHECKED : BST_CHECKED);
  if(c == BST_UNCHECKED)
  {
    ((CButton*)GetDlgItem(IDC_CHECK_ALL))->SetCheck(BST_UNCHECKED);
  }
}

void CKmlDlg::OnLbnChkchangeFileList()
{
  int s = m_fileList.GetCurSel();
  int c = m_fileList.GetCheck(s);
  m_fileList.SetCheck(s, (c) ? BST_UNCHECKED : BST_CHECKED);
  if(c == BST_UNCHECKED)
  {
    ((CButton*)GetDlgItem(IDC_CHECK_ALL))->SetCheck(BST_UNCHECKED);
  }
}
