/*******************************************************************************
 * Copyright (c) 2006 SkyTraq Technology, Inc. Hsinchu, Taiwan
 * All Rights Reserved.
 *
 * file name: waas_parity.h
 * Initial: justin
 * Description: CRC24Q parity check function
 ******************************************************************************/
#ifndef CRC24Q_PARITY_INCLUDED
#define CRC24Q_PARITY_INCLUDED

//---------- head file inclusions --------------------------
//#include "st_basic_type.h"
#include "st_basic_const.h"

//---------- constant definitions --------------------------

//---------- macro definitions -----------------------------

//---------- type definitions ------------------------------

// --------- extern function declarations ------------------
#ifdef __cplusplus
extern "C" {
#endif

// U32 array input, the data can be arbitrary number of bits
S16 CRC24Q_check( const U32 dws_ary[], S32 n_bits );
U32 CRC24Q_calc( const U32 dws_ary[], S32 n_bits );

// U08 array input, the data can only be 8xN bits
S16 CRC24Q_check_bytes_array( const U08 bytes_ary[], S32 n_bytes );
U32 CRC24Q_calc_bytes_array( const U08 bytes_ary[], S32 n_bytes );

#ifdef __cplusplus
}
#endif

#endif //#ifndef CRC24Q_PARITY_INCLUDED
