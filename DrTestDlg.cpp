// DrTestDlg.cpp
//

#include "stdafx.h"
#include "DrTestDlg.h"
#include "Resource.h"
#include "GPSDlg.h"
#include "Serial.h"

const UINT DrTestTimerId = 1;
// DrTestDlg 

IMPLEMENT_DYNAMIC(DrTestDlg, CDialog)

DrTestDlg::DrTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_DR_TEST, pParent)
{
  state = Idle;
}

DrTestDlg::~DrTestDlg()
{
}

void DrTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(DrTestDlg, CDialog)
  ON_WM_TIMER()
  ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_START, &DrTestDlg::OnBnClickedStart)
END_MESSAGE_MAP()

// DrTestDlg 
BOOL DrTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
  CGPSDlg::gpsDlg->TempBoostBaudrate(FALSE);
  OnBnClickedStart();
	return TRUE;  // return TRUE unless you set the focus to a control
}

void DrTestDlg::OnOK()
{
	CDialog::OnOK();
}

void DrTestDlg::OnBnClickedStart()
{ 
  switch(state)
  {
  case Idle:
    queryCount = 0;
    SetTimer(DrTestTimerId, 1000, NULL);
    GetDlgItem(IDC_START)->SetWindowText("Stop");
    ::DisplayStatic(this, IDC_QUERY_COUNT, "%d", queryCount);
    state = Started;
    break;
  case Started:
    KillTimer(DrTestTimerId);
    GetDlgItem(IDC_START)->SetWindowText("Start");
    state = Idle;
    break;
  case Querying:
    break;
  } 
}

void DrTestDlg::OnDestroy()
{

    if(state == Started)
    {
      KillTimer(DrTestTimerId);
      state = Idle;
    }
    CGPSDlg::gpsDlg->TempBoostBaudrate(TRUE);

  	CDialog::OnDestroy();
}

void DrTestDlg::OnTimer(UINT_PTR nIDEvent)
{
	BinaryCommand cmd(3);
	cmd.SetU08(1, 0x7A);
	cmd.SetU08(2, 0x08);
	cmd.SetU08(3, 0x7F);

  queryCount++;
  DisplayStatic(this, IDC_QUERY_COUNT, "%d", queryCount);

	BinaryData ackCmd;
  CGPSDlg::CmdErrorCode rsp = CGPSDlg::gpsDlg->ExcuteBinaryCommand(0x7A, 0x08, &cmd, &ackCmd, 900, true);
  switch(rsp)
  {
  case CGPSDlg::Ack:
    GetDlgItem(IDC_QUERY_RESPONSE)->SetWindowText("ACK");
    break;
  case CGPSDlg::NACK:
    GetDlgItem(IDC_QUERY_RESPONSE)->SetWindowText("NACK");
    break;
  case CGPSDlg::FormatError:
    GetDlgItem(IDC_QUERY_RESPONSE)->SetWindowText("Format Error");
    break;
  case CGPSDlg::Timeout:
    GetDlgItem(IDC_QUERY_RESPONSE)->SetWindowText("Timeout");
    break;
  default:
    GetDlgItem(IDC_QUERY_RESPONSE)->SetWindowText("Unknown");
    break;
  }

  if(CGPSDlg::Ack != rsp)
	{
    return CDialog::OnTimer(nIDEvent);
  }

  GetDlgItem(IDC_HAS_SENSOR)->SetWindowText((ackCmd[7]) ? "1" : "0");
  GetDlgItem(IDC_HAS_BARO)->SetWindowText((ackCmd[8]) ? "1" : "0");
  GetDlgItem(IDC_HAS_ODO)->SetWindowText((ackCmd[9]) ? "1" : "0");

  DisplayStatic(this, IDC_AG_X, "%f", ConvertLeonFloat(ackCmd.Ptr(10)));
  DisplayStatic(this, IDC_AG_Y, "%f", ConvertLeonFloat(ackCmd.Ptr(14)));
  DisplayStatic(this, IDC_AG_Z, "%f", ConvertLeonFloat(ackCmd.Ptr(18)));

  DisplayStatic(this, IDC_GB_X, "%f", ConvertLeonFloat(ackCmd.Ptr(22)));
  DisplayStatic(this, IDC_GB_Y, "%f", ConvertLeonFloat(ackCmd.Ptr(26)));
  DisplayStatic(this, IDC_GB_Z, "%f", ConvertLeonFloat(ackCmd.Ptr(30)));

  switch(ackCmd[34])
  {
  case 'N':
    GetDlgItem(IDC_GYRO_CALIB_IND)->SetWindowText("Not Calibrated");
    break;
  case 'C':
    GetDlgItem(IDC_GYRO_CALIB_IND)->SetWindowText("Calibrated");
    break;
  default:
    GetDlgItem(IDC_GYRO_CALIB_IND)->SetWindowText("??????????");
    break;
  }

  DisplayStatic(this, IDC_AA_X, "%f", ConvertLeonFloat(ackCmd.Ptr(35)));
  DisplayStatic(this, IDC_AA_Y, "%f", ConvertLeonFloat(ackCmd.Ptr(39)));
  DisplayStatic(this, IDC_AA_Z, "%f", ConvertLeonFloat(ackCmd.Ptr(43)));

  DisplayStatic(this, IDC_AB_X, "%f", ConvertLeonFloat(ackCmd.Ptr(47)));
  DisplayStatic(this, IDC_AB_Y, "%f", ConvertLeonFloat(ackCmd.Ptr(51)));
  DisplayStatic(this, IDC_AB_Z, "%f", ConvertLeonFloat(ackCmd.Ptr(55)));

  DisplayStatic(this, IDC_ODO_PLS_COUNT, "%u", MAKELONG(MAKEWORD(ackCmd[62], ackCmd[61]), MAKEWORD(ackCmd[60], ackCmd[59])));
  DisplayStatic(this, IDC_ODO_FB_STS, "%d", ackCmd[63]);
  DisplayStatic(this, IDC_ODO_SCL_FCT, "%f", ConvertLeonFloat(ackCmd.Ptr(64)));
  DisplayStatic(this, IDC_ODO_DST, "%f", ConvertLeonFloat(ackCmd.Ptr(68)));

  DisplayStatic(this, IDC_PRESSURE, "%f", ConvertLeonFloat(ackCmd.Ptr(72)));
  DisplayStatic(this, IDC_REF_PRESSURE, "%f", ConvertLeonFloat(ackCmd.Ptr(76)));
  DisplayStatic(this, IDC_RAW_ALTITUDE, "%f", ConvertLeonFloat(ackCmd.Ptr(80)));
  DisplayStatic(this, IDC_ALT_EPS, "%f", ConvertLeonFloat(ackCmd.Ptr(84)));

  switch(ackCmd[88])
  {
  case 'N':
    GetDlgItem(IDC_SENSOR_CALIB_IND)->SetWindowText("Not Calibrated");
    break;
  case 'C':
    GetDlgItem(IDC_SENSOR_CALIB_IND)->SetWindowText("Calibrated");
    break;
  default:
    GetDlgItem(IDC_SENSOR_CALIB_IND)->SetWindowText("??????????");
    break;
  }

  switch(ackCmd[89])
  {
  case 'A':
    GetDlgItem(IDC_SENSOR1_TYPE)->SetWindowText("Accelerometer");
    break;
  case 'B':
    GetDlgItem(IDC_SENSOR1_TYPE)->SetWindowText("Barometer");
    break;
  case 'G':
    GetDlgItem(IDC_SENSOR1_TYPE)->SetWindowText("Gyroscope");
    break;
  case 'I':
    GetDlgItem(IDC_SENSOR1_TYPE)->SetWindowText("Inertial Sensor");
    break;
  case 'T':
    GetDlgItem(IDC_SENSOR1_TYPE)->SetWindowText("Standalone Temperature Sensor");
    break;
  default:
    GetDlgItem(IDC_SENSOR1_TYPE)->SetWindowText("None");
    break;
  }
  DisplayStatic(this, IDC_SENSOR1_TEMP, "%f", ConvertLeonFloat(ackCmd.Ptr(90)));

  switch(ackCmd[94])
  {
  case 'A':
    GetDlgItem(IDC_SENSOR2_TYPE)->SetWindowText("Accelerometer");
    break;
  case 'B':
    GetDlgItem(IDC_SENSOR2_TYPE)->SetWindowText("Barometer");
    break;
  case 'G':
    GetDlgItem(IDC_SENSOR2_TYPE)->SetWindowText("Gyroscope");
    break;
  case 'I':
    GetDlgItem(IDC_SENSOR2_TYPE)->SetWindowText("Inertial Sensor");
    break;
  case 'T':
    GetDlgItem(IDC_SENSOR2_TYPE)->SetWindowText("Standalone Temperature Sensor");
    break;
  default:
    GetDlgItem(IDC_SENSOR2_TYPE)->SetWindowText("None");
    break;
  }
  DisplayStatic(this, IDC_SENSOR2_TEMP, "%f", ConvertLeonFloat(ackCmd.Ptr(95)));

  CDialog::OnTimer(nIDEvent);
}

