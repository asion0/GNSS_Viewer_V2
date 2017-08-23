
#pragma once

#ifndef _DATALOG_H_
#define _DATALOG_H_
#include "stdafx.h"

typedef struct
{
	U08  Type;
	U32  WNO;
	U32  TOW;
	U16  DTOW;
	S32  ECEF_X;
	S32  ECEF_Y;
	S32  ECEF_Z;
	S16  DECEF_X;
	S16  DECEF_Y;
	S16  DECEF_Z;
	float  V;

}POS_FIX_REC;

struct LogFlashInfo1
{
	U32 log_flash_current_prt;
	U16 sector_left;//full or #of entry	
	U16 total_sector;//	
	U32 max_time;
	U32 min_time;
	U32 max_distance;
	U32 min_distance;
	U32 max_speed;
	U32 min_speed;
	U08 datalog_enable;
	U08 fifo_mode;
	U32 poi_entry;
	U08 autolog_full;
	U08 poi_full;
	U32 reserve1;
	U32 reserve2;
	U32 reserve3;
	U32 reserve4;
	U32 reserve5;
	U32 reserve6;
	U32 reserve7;
	U32 reserve8;
	U32 reserve9;
	U32 reserve10;
	U32 reserve11;
	U32 reserve12;
	U32 reserve13;
	U08 checksum;
	#if defined SOARCOM
	S08 password[8]; 
	#endif
};

typedef struct 
{
	U16 word[9];
} FixFull_T;

typedef struct 
{
	U16 word[4];
}FixCompact_T;

typedef struct{
	U16 word[10];
}FixMultiHz_T;

typedef struct 
{
	U16 word;
}FIX_NONE;


#endif