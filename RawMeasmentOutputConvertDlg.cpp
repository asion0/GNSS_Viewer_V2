// RawMeasmentOutputConvertDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "RawMeasmentOutputConvertDlg.h"
#include "GPS.h"

#define UWM_RAW_PROGRESS		(WM_USER + 0x1039)
// CRawMeasmentOutputConvertDlg 對話方塊
const int MaxProgress = 1000;

IMPLEMENT_DYNAMIC(CRawMeasmentOutputConvertDlg, CDialog)

CRawMeasmentOutputConvertDlg::CRawMeasmentOutputConvertDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_RAW_MEAS_OUT_CONV, pParent)
{
	m_convertRunning = false;
}

CRawMeasmentOutputConvertDlg::~CRawMeasmentOutputConvertDlg()
{
}

void CRawMeasmentOutputConvertDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CRawMeasmentOutputConvertDlg, CDialog)
	ON_BN_CLICKED(IDC_BROWSE, &CRawMeasmentOutputConvertDlg::OnBnClickedBrowse)
	ON_BN_CLICKED(IDC_GO, &CRawMeasmentOutputConvertDlg::OnBnClickedGo)
	ON_MESSAGE(UWM_RAW_PROGRESS, OnRawProgress)
	ON_WM_CLOSE()
END_MESSAGE_MAP()


// CRawMeasmentOutputConvertDlg 訊息處理常式

void CRawMeasmentOutputConvertDlg::OnBnClickedBrowse()
{
	CFileDialog dlgFile(TRUE, _T("dat"), NULL, 
		OFN_HIDEREADONLY, _T("Binary Files (*.dat)|*.dat|ALL Files (*.*)|*.*||"), this);
	
  	INT_PTR nResult = dlgFile.DoModal();
	m_filePath = dlgFile.GetPathName();
	if(nResult == IDOK)
	{
		GetDlgItem(IDC_PATH)->SetWindowText(m_filePath);
	}
	if(Utility::IsFileExist(m_filePath))
	{
		GetDlgItem(IDC_GO)->EnableWindow(TRUE);
	}
	else
	{
		GetDlgItem(IDC_GO)->EnableWindow(FALSE);
	}
}

DWORD GetBinary(void *buffer, DWORD bufferSize, CFile& f)
{	
	//CFile f(strFile, CFile::modeRead);
	
	U08* bufferIter = (U08*)buffer;
	DWORD totalSize = 0;
	DWORD failCount = 10;
	while(totalSize < bufferSize - 1)
	{ 
		DWORD dwBytesDoRead = 0;
		DWORD dwErrorFlags = 0;

		DWORD bytesinbuff = (DWORD)(f.GetLength() - f.GetPosition());
		while(bytesinbuff)
		{
			BOOL bb = ReadFile(f.m_hFile, bufferIter, 1, &dwBytesDoRead, 0);
			if(dwBytesDoRead <= 0)
			{	//Read fail.
				DWORD dwErr = ::GetLastError();
				continue;
			}

			if(totalSize > 0)
			{	//not first char.
				if(*bufferIter==0xa1 && *(bufferIter-1)==0xa0)
				{
					bufferIter -= totalSize;
					*bufferIter = 0xa0; 
					++bufferIter;
					*bufferIter = 0xa1; 
					++bufferIter;
					totalSize = 2;
					continue;
				}
				else if(*bufferIter==0x0a && *(bufferIter-1)==0x0d)
				{
					unsigned char *chk_ptr = bufferIter - totalSize;
					
					if (*chk_ptr == 0xa0)
					{
						int tmp_len = *(chk_ptr + 2);
						tmp_len = tmp_len << 8 | *(chk_ptr+3);
						if (totalSize == tmp_len + 6) 
						{
							*(bufferIter+1) = 0;
							return totalSize + 1;
						}
					}
					else
					{
						return totalSize;
					}
				}
			}
			++totalSize;
			if (totalSize >=  bufferSize - 1)
			{	//Check 
				*(bufferIter+1) = 0;
				break;
			}
				
			++bufferIter;
			--bytesinbuff;
		} //while(bytesinbuff)
		if(bytesinbuff==0)
		{
			return totalSize;
		}
	} //while(total < size - 1)
	return totalSize;
}

#include "BinaryMSG.h"
void BinaryProc(U08* buffer, int len, CFile& f)
{
	if(len <= 7)
	{
		return;
	}

	int packetLen = MAKEWORD(buffer[3], buffer[2]);
	if(packetLen + 7 > len) 
	{
		return;
	}

	U08 msgType = Cal_Checksum(buffer);	
	CString strOutput;
	switch(msgType)
	{
	case 0xDC:		// measurement time
		ShowMeasurementTime(buffer, true, &strOutput);
		break;
	case 0xDD:		// raw measurement
		ShowMeasurementChannel(buffer, true, &strOutput);
		//Show_Message(buffer,len);
		break;
	case 0xDE:		// SV_CH status
		ShowMeasurementSv(buffer, true, &strOutput);
		//Show_Message(buffer,len);
		break;
	case 0xDF:		// receiver state
		ShowReceiverNav(buffer, true, &strOutput);
		//Show_Message(buffer,len);
		break;
	case 0xE0:		// sub frame data
	case 0xE1:		// sub frame data
	case 0xE2:		// sub frame data
	case 0xE3:		// sub frame data
		//Show_Message(buffer,len);
		ShowSubframe(buffer, true, &strOutput);
		break;
	case BINMSG_ECEF_USER_PVT:
		ShowBinaryOutput(buffer, true, &strOutput);
//		ShowTime();
		break;
	case BINMSG_ERROR:
//		add_msgtolist("Unknown: " + theApp.GetHexString(buffer, len));	
		break;
	default:
//		add_msgtolist("Unknown: " + theApp.GetHexString(buffer, len));	
		break;
	}
	if(strOutput.GetLength() > 0)
	{
		f.Write((LPCTSTR)strOutput, strOutput.GetLength());
	}
}

bool cancelConvert = false;
UINT RawMeasmentOutputConvertThread(LPVOID pParam)
{
	CRawMeasmentOutputConvertDlg* pDlg = (CRawMeasmentOutputConvertDlg*)pParam;
	int length = 0;	

	static U08 buffer[1024] = {0};
	CFile f(pDlg->GetFilePath(), CFile::modeRead);
	CString strOutput = pDlg->GetFilePath();
	strOutput += ".txt";
	CFile fo(strOutput, CFile::modeWrite | CFile::modeCreate);
	//CProgressCtrl *p = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS1);
	//p->SetRange32(0, (int)f.GetLength());
	ULONGLONG total = f.GetLength();
	WPARAM progress = 0;
	pDlg->PostMessage(UWM_RAW_PROGRESS, progress);

	while(!cancelConvert)
	{	
		length = GetBinary(buffer, sizeof(buffer), f);
		if(progress != (WPARAM)(MaxProgress * (double)f.GetPosition() / total))
		{
			progress = (WPARAM)(MaxProgress * (double)f.GetPosition() / total);
			pDlg->PostMessage(UWM_RAW_PROGRESS, progress);
		}
		//p->SetPos((int)f.GetPosition());
		if(length == 0)
		{
			break;
		}
		buffer[length + 1] = 0;
		BinaryProc(buffer, length, fo);				
	}
	fo.Close();
	f.Close();
	pDlg->PostMessage(UWM_RAW_PROGRESS, 0, TRUE);
	return 0;
}

CWinThread* pThread = NULL;
void CRawMeasmentOutputConvertDlg::OnBnClickedGo()
{
	if(!m_convertRunning)
	{
		cancelConvert = false;
		pThread = AfxBeginThread(RawMeasmentOutputConvertThread, this);
		CProgressCtrl *p = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS1);
		p->SetRange32(0, MaxProgress);
		p->SetPos(0);
		m_convertRunning = true;
	}
	else
	{
		cancelConvert = true;
		::WaitForSingleObject(pThread->m_hThread, INFINITE);
		m_convertRunning = false;
	}
	UpdateConvertUI();

}

BOOL CRawMeasmentOutputConvertDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	GetDlgItem(IDC_MESSAGE)->SetWindowText("");

	OnBnClickedBrowse();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX 屬性頁應傳回 FALSE
}

LRESULT CRawMeasmentOutputConvertDlg::OnRawProgress(WPARAM wParam, LPARAM lParam)
{
	CProgressCtrl *p = (CProgressCtrl*)GetDlgItem(IDC_PROGRESS1);
	p->SetPos(wParam);

	CString strMsg;
	strMsg.Format("Converting %d / %d", (int)wParam, 1000);
	GetDlgItem(IDC_MESSAGE)->SetWindowText(strMsg);

	if(lParam == TRUE)
	{
		cancelConvert = true;
		m_convertRunning = false;
		strMsg.Format("Converting finished.");
		GetDlgItem(IDC_MESSAGE)->SetWindowText(strMsg);
	}
	UpdateConvertUI();
	return 0;
}

void CRawMeasmentOutputConvertDlg::UpdateConvertUI()
{
	if(m_convertRunning)
	{
		GetDlgItem(IDC_GO)->SetWindowText("Cancel");
		GetDlgItem(IDC_BROWSE)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_GO)->SetWindowText("Go");
		GetDlgItem(IDC_BROWSE)->EnableWindow(TRUE);
	}
}

void CRawMeasmentOutputConvertDlg::OnClose()
{
	// TODO: 在此加入您的訊息處理常式程式碼和 (或) 呼叫預設值
	if(m_convertRunning)
	{
		::AfxMessageBox("Converter still running!");
		return;
	}
	CDialog::OnClose();
}
