// CGnssStatus.cpp
//

#include "stdafx.h"
#include "GnssStatus.h"

// CGnssStatus
UpdateFlag operator | (UpdateFlag lhs, const UpdateFlag &rhs)
{
    return static_cast<UpdateFlag>(static_cast<unsigned __int64>(lhs) | static_cast<unsigned __int64>(rhs));
}

UpdateFlag& operator |= (UpdateFlag& lhs, UpdateFlag rhs)
{
    lhs = static_cast<UpdateFlag>(static_cast<unsigned __int64>(lhs) | static_cast<unsigned __int64>(rhs));
    return lhs;
}

CGnssStatus::CGnssStatus()
{
    ClearStatus();
}

CGnssStatus::~CGnssStatus()
{
}

void CGnssStatus::ClearStatus()
{
  updateFlag = UpdateNone;
  date_y = date_m = date_d = 0;
  time_h = time_m = 0;
  time_s = 0;
  roverTotalCycleSlipped = baseTotalCycleSlipped = 0;
}

UpdateFlag CGnssStatus::SetDateTimeViaWnTow(U16 wn, U32 tow)
{
  UpdateFlag f = UpdateNone;
	UtcTime utc;
	UtcConvertGpsToUtcTime(wn, (tow / 1000.0), &utc);
  if(utc.year != date_y || utc.month != date_m  || utc.day != date_d)
  {
    date_y = utc.year;
    date_m = utc.month;
    date_d = utc.day;
    f |= UpdateDate;
  }
  if(utc.hour != time_h || utc.minute != time_m  || utc.sec != time_s)
  {
    time_h = utc.hour;
    time_m = utc.minute;
    time_s = utc.sec;
    f |= UpdateTime;
  }
  return f;
}

UpdateFlag CGnssStatus::SetRoverTotalCycleSlipped(U16 count)
{
  UpdateFlag f = UpdateNone;
  if(count != roverTotalCycleSlipped)
  {
    roverTotalCycleSlipped = count;
    f |= UpdateTotalCycleSlipped;
  }
  return f;
}

UpdateFlag CGnssStatus::SetBaseTotalCycleSlipped(U16 count)
{
  UpdateFlag f = UpdateNone;
  if(count != baseTotalCycleSlipped)
  {
    baseTotalCycleSlipped = count;
    f |= UpdateTotalCycleSlipped;
  }
  return f;
}
