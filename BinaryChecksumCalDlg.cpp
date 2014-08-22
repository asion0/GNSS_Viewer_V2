// BinaryChecksumCalDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "BinaryChecksumCalDlg.h"
#include "Resource.h"
#include "GPSDlg.h"
#include "Serial.h"


// BinaryChecksumCalDlg 對話方塊

IMPLEMENT_DYNAMIC(BinaryChecksumCalDlg, CDialog)

BinaryChecksumCalDlg::BinaryChecksumCalDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_BIN_CHECKSUM_CAL, pParent)
{

}

BinaryChecksumCalDlg::~BinaryChecksumCalDlg()
{
}

void BinaryChecksumCalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(BinaryChecksumCalDlg, CDialog)
	ON_EN_CHANGE(IDC_CONTEXT, &BinaryChecksumCalDlg::OnEnChangeInput)
	ON_BN_CLICKED(IDC_SEND, &BinaryChecksumCalDlg::OnBnClickedSend)
END_MESSAGE_MAP()


// BinaryChecksumCalDlg 訊息處理常式

BOOL BinaryChecksumCalDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此加入額外的初始化
	//IDC_CHECKSUM, IDC_CONTEXT, IDC_BIN_SIZE
	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX 屬性頁應傳回 FALSE
}

void BinaryChecksumCalDlg::OnOK()
{
	// TODO: 在此加入特定的程式碼和 (或) 呼叫基底類別

	CDialog::OnOK();
}

void BinaryChecksumCalDlg::OnEnChangeInput()
{
	// TODO:  如果這是 RICHEDIT 控制項，控制項將不會
	// 傳送此告知，除非您覆寫 CDialog::OnInitDialog()
	// 函式和呼叫 CRichEditCtrl().SetEventMask()
	// 讓具有 ENM_CHANGE 旗標 ORed 加入遮罩。
 	CString strInput;
	GetDlgItem(IDC_CONTEXT)->GetWindowText(strInput);

	//GetDlgItem(IDC_CHECKSUM)->SetWindowText(strInput + "_");
	BinaryData binData;
	if(!ConvertHexToBinary(strInput, binData))
	{
		GetDlgItem(IDC_BIN_SIZE)->SetWindowText("Invalidate Format!");
		GetDlgItem(IDC_CHECKSUM)->SetWindowText("Invalidate Format!");
		return;
	}

	U08 checkSum = 0;
	for(int i=0; i<binData.Size() ; ++i)
	{
		checkSum ^= binData[i];
	}

	CString strOutput;
	strOutput.Format("%02X %02X", HIBYTE(binData.Size()), LOBYTE(binData.Size()));
	GetDlgItem(IDC_BIN_SIZE)->SetWindowText(strOutput);
	strOutput.Format("%02X", checkSum);
	GetDlgItem(IDC_CHECKSUM)->SetWindowText(strOutput);
}

int BinaryChecksumCalDlg::FindNextNoneSpaceChar(LPCSTR pszInput, bool forward)
{
	int index = (forward) ? 0 : strlen(pszInput) - 1;

	if(index < 0)
	{
		return index;
	}

	while(pszInput[index]==' ')
	{
		index += (forward) ? 1 : -1;
	}
	index += (forward) ? 0 : 1;

	return index ;
}	

bool CheckHex(LPCSTR pszInput)
{
	for(const char* c=pszInput; *c!=0; ++c)
	{
		if(*c >= '0' && *c <= '9')
			continue;
		if(*c >= 'a' && *c <= 'f')
			continue;
		if(*c >= 'A' && *c <= 'F')
			continue;
		return false;
	}
	return true;
}

bool BinaryChecksumCalDlg::ConvertHexToBinary(LPCSTR pszInput, BinaryData& binData)
{
	CString strInput(pszInput);
	if(strInput.GetLength()<2)
	{
		return false;
	}
	strInput.MakeLower();

	int index = FindNextNoneSpaceChar(strInput);
	//Trim the space character in head.
	if(index > 0)
	{
		strInput = strInput.Right(strInput.GetLength() - index);
	}

	if(strInput.GetLength() == 0)
	{
		return false;
	}

	//Trim the space character in tail.
	index = FindNextNoneSpaceChar(strInput, false);
	if(index < strInput.GetLength())
	{
		strInput = strInput.Left(index);
	}

	int i = 0;	// size of output binary.
	int n = 0;	// index of input string
	BinaryData b(strlen(pszInput) / 2);

	while(n < strInput.GetLength())
	{
		CString token = strInput.Tokenize(" ", n);
		if(!CheckHex(token))
		{
			return false;
		}
		long data = strtol(token, NULL, 16);
		if(data > 255)
		{
			return false;
		}
		*b.GetBuffer(i++) = (char)data;
	}

	if(i)
	{
		binData.Alloc(i);
		memcpy(binData.GetBuffer(), b.Ptr(), i);
	}
	return true;
}

void BinaryChecksumCalDlg::OnBnClickedSend()
{
	// TODO: 在此加入控制項告知處理常式程式碼
	if(!CGPSDlg::gpsDlg->m_isConnectOn)
	{
		AfxMessageBox("Please connect to Sky Traq GPS");
		return;
	}

	CString strInput;
	GetDlgItem(IDC_CONTEXT)->GetWindowText(strInput);

	BinaryData binData;
	if(!ConvertHexToBinary(strInput, binData))
	{
		::AfxMessageBox("Invalidate Format!");
		return;
	}

	BinaryCommand cmd(binData);
	U08* pCmd = cmd.GetBuffer();
	int inSize = cmd.Size();
	CGPSDlg::gpsDlg->m_serial->SendData(pCmd, inSize);
}
