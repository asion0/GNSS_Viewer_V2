// VerifyFwDlg.cpp : 實作檔
//

#include "stdafx.h"
#include "VerifyFwDlg.h"
#include "GPS.h"


// CVerifyFwDlg 對話方塊

IMPLEMENT_DYNAMIC(CVerifyFwDlg, CDialog)

CVerifyFwDlg::CVerifyFwDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_VERIFY_FW, pParent)
{

}

CVerifyFwDlg::~CVerifyFwDlg()
{
}

void CVerifyFwDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CVerifyFwDlg, CDialog)
	ON_BN_CLICKED(IDC_SELECT, &CVerifyFwDlg::OnBnClickedSelect)
	ON_BN_CLICKED(ID_GO, &CVerifyFwDlg::OnBnClickedGo)
END_MESSAGE_MAP()


// CVerifyFwDlg 訊息處理常式
bool CVerifyFwDlg::AutoFill(const CString& s)
{
	CStringArray sa, sb;
	int position = 0;
    CString seperator = _T("_");
	CString token = s.Tokenize(seperator, position);
    while(!token.IsEmpty())
    {
        // Get next token.
        token = s.Tokenize(seperator, position);
		if(position > 0 )
		{
			sa.Add(token);
		}
    }
	if (sa.GetCount() < 7)
    {
        return false;
    }
	
    //Get Version
    seperator = _T("- .");
	position = 0;
	CString strParam = sa.GetAt(0);
    token = strParam.Tokenize(seperator, position);
    while(!token.IsEmpty())
    {
		if(position > 0 )
		{
			sb.Add(token);
		}
        // Get next token.
        token = strParam.Tokenize(seperator, position);
    }
	if (sb.GetCount() != 6)
    {
        return false;
    }

	//Parsing Version
	int v1 = atoi(sb.GetAt(0));
	int v2 = atoi(sb.GetAt(1));
	int v3 = atoi(sb.GetAt(2));
	CString txt;
	txt.Format("%2d.%02d.%2d", v1, v2, v3);
	GetDlgItem(IDC_KERNELVER)->SetWindowText(txt);

	v1 = atoi(sb.GetAt(3));
	v2 = atoi(sb.GetAt(4));
	v3 = atoi(sb.GetAt(5));
	txt.Format("%2d.%02d.%2d", v1, v2, v3);
	GetDlgItem(IDC_SWVER)->SetWindowText(txt);

	//Parsing Baud Rate
	strParam = sa.GetAt(sa.GetCount() - 2);
	GetDlgItem(IDC_BAUDRATE)->SetWindowText(strParam);

	//Parsing Build date
	strParam = sa.GetAt(sa.GetCount() - 1);
	GetDlgItem(IDC_REV)->SetWindowText(strParam);

	//Parsing CRC
	strParam = sa.GetAt(sa.GetCount() - 3);
	if(strParam.GetLength() != 4)
	{
		return false;
	}
	GetDlgItem(IDC_CRC)->SetWindowText(strParam);
	return true;
}

void CVerifyFwDlg::OnBnClickedSelect()
{
	CFileDialog fd(true,"*.bin",NULL,OFN_FILEMUSTEXIST| OFN_HIDEREADONLY,"*.bin|*.bin||");
	if(fd.DoModal() == IDOK)
	{
		CString strPath = fd.GetPathName();
		GetDlgItem(IDC_FILEPATH)->SetWindowText(fd.GetFolderPath());
		GetDlgItem(IDC_FILENAME)->SetWindowText(fd.GetFileName());
		if(AutoFill(fd.GetFileName()))
		{
			GetDlgItem(ID_GO)->EnableWindow(TRUE);
		}
		else
		{
			::AfxMessageBox("Can not pasing file name!");
			GetDlgItem(ID_GO)->EnableWindow(FALSE);
		}
	}
}

void CVerifyFwDlg::OnBnClickedGo()
{
	// TODO: 在此加入控制項告知處理常式程式碼
}
