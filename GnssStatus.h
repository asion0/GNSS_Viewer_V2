#pragma once


// CGnssStatus
enum UpdateFlag : unsigned __int64  
{
  UpdateNone  = 0ULL,
  UpdateTime  = (1ULL << 63),
  UpdateDate  = (1ULL << 62),
  UpdateTotalCycleSlipped  = (1ULL << 61),
  EFlagFour   = (1ULL << 60)
};

inline UpdateFlag operator|(UpdateFlag lhs, const UpdateFlag &rhs);
inline UpdateFlag& operator |= (UpdateFlag& lhs, UpdateFlag rhs);

class CGnssStatus
{
public:
  enum LLFormat
  {
    DD, //Decimal Degrees
    DM, //Degrees Minutes
    DMS //Degrees Minutes Second
  };





  CGnssStatus();   //
	virtual ~CGnssStatus();
  void ClearStatus();
  void ClearUpdateFlag() { updateFlag = UpdateNone; }

  UpdateFlag GetUpdateFlag() { return updateFlag; }

  CString GetTimeString();
  CString GetDateString();
  UpdateFlag SetDateTimeViaWnTow(U16 wn, U32 tow);

  CString GetTotalCycleSlipped();
  UpdateFlag SetRoverTotalCycleSlipped(U16 count);
  UpdateFlag SetBaseTotalCycleSlipped(U16 count);





  int GetTTFFSeconds();

  CString GetLongitudeString(LLFormat llf);
  CString GetLatitudeString(LLFormat llf);
  D64 GetLongitude();
  D64 GetLatitude();

  enum AltFormat
  {
    Ellipsoidal,
    MeanSeaLevel,
  };
  CString GetAltitudeString(AltFormat af);
  D64 GetAltitude();
  bool GetGeoidSeparation(D64& geo);

  CString GetDirectionString();
  D64 GetDirection();

  CString GetSpeedString();
  D64 GetSpeed();

  CString GetHdopString();
  D64 GetHdop();

protected:
  UpdateFlag updateFlag;

  U16 date_y;
  U16 date_m;
  U16 date_d;

  U16 time_h;
  U16 time_m;
  F32 time_s;

  U16 roverTotalCycleSlipped;
  U16 baseTotalCycleSlipped;
	//DECLARE_MESSAGE_MAP()
};

