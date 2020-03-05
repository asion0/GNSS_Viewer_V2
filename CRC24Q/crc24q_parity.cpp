/*******************************************************************************
 * Copyright (c) 2006 SkyTraq Technology, Inc. Hsinchu, Taiwan
 * All Rights Reserved.
 *
 * file name: crc24q_parity.c ( CRC-24Q, Q for Qualcomm Corporation )
 * Initial: justin
 ******************************************************************************/
//---------- head file inclusions -------------------------
//#include "st_basic_macro.h"
//#include "st_assert.h"
#include "stdafx.h"
#include "crc24q_parity.h"

//---------- local definitions ----------------------------
#define CRC24_POLY 0x1864cfbL

//---------- local types ----------------------------------

//---------- static global variables ----------------------
//U16 Crc24_Q_tab[384];    // produced by Generate_crc24_tab(  )

const U16 Crc24_Q_table[384] = // total = 768 ( bytes )
{
  0x0000,  0x0086,  0x4cfb,  0x8ad5,  0x0d0c,  0x99f6,  0x93e6,  0xe115,
  0xaa1a,  0x1933,  0xec9f,  0x7f17,  0xa181,  0x3927,  0xcdc2,  0x2b54,
  0x34ad,  0x18cf,  0x3267,  0xd8b4,  0x2b23,  0xb8b2,  0xd53e,  0xfe2e,
  0xc54e,  0x8943,  0x0272,  0x4f9b,  0x84c9,  0xd77f,  0x56a8,  0x68d0,
  0xe493,  0xdc7d,  0x655a,  0x319e,  0x64cf,  0xb0e2,  0x834b,  0xee1a,
  0xbd68,  0x5646,  0xf729,  0x5171,  0x65aa,  0x7dfc,  0x5cfb,  0xb0a7,
  0x0cd1,  0xe98a,  0x9d12,  0x8604,  0xe400,  0x481f,  0x9f37,  0x0819,
  0x7bf3,  0x15e2,  0x0593,  0xaefe,  0xad50,  0xd02b,  0x1c2b,  0x2785,
  0xdda1,  0xc926,  0x3eb6,  0x31b8,  0xfaca,  0xb463,  0x3c32,  0x2fc7,
  0xc99f,  0x604f,  0xd39b,  0x434a,  0x6dc5,  0x0696,  0x5a79,  0x81dc,
  0x357a,  0xd0ac,  0x8c56,  0xe077,  0x681e,  0x59ee,  0x52a2,  0xe2cb,
  0x5464,  0x87af,  0xfbf8,  0xb87d,  0xb443,  0x712d,  0xb5f7,  0x614e,
  0x19a3,  0xd29f,  0xef29,  0x9376,  0xdf15,  0x3a24,  0x8a45,  0x330c,
  0x09c8,  0x0090,  0x3e86,  0xdcc5,  0xb822,  0xeb3e,  0x6e10,  0x32f7,
  0xe6b4,  0xbb1d,  0x2bc4,  0x0aad,  0x88f1,  0xa111,  0x0727,  0x5dfc,
  0xdced,  0x5b5a,  0xa1a0,  0x5638,  0x56d0,  0x74ad,  0x4f0b,  0xbac9,
  0x4741,  0xc5de,  0xb743,  0x924c,  0x7d6c,  0x62fb,  0x2099,  0xf7b9,
  0x6f71,  0xf594,  0xee8a,  0x8368,  0xc678,  0x645f,  0x8ee2,  0x1375,
  0x1572,  0x3b93,  0x3ec0,  0x9fa7,  0x3619,  0xebcd,  0x8694,  0xda00,
  0xd821,  0x0c41,  0xd78a,  0x0d2c,  0xb4f3,  0x0232,  0xbff9,  0x3e26,
  0x0fb8,  0x6af4,  0x2715,  0xe3a1,  0x5918,  0xadc0,  0xee2b,  0x8c15,
  0xd03c,  0xb256,  0x7049,  0x5ae9,  0xbfdc,  0xa544,  0x43da,  0x53c5,
  0x96a8,  0xc90f,  0x5e4f,  0x43a5,  0x71bd,  0x8bf7,  0xf170,  0xfb68,
  0x867d,  0x247d,  0xe25b,  0x6a64,  0x1791,  0x688e,  0x67ee,  0xc29c,
  0x3347,  0xa4b5,  0x0b5f,  0xb992,  0xa93f,  0xde52,  0xa0a1,  0x4526,
  0xedbe,  0x2a74,  0x48ac,  0x38b3,  0x92c6,  0x9d14,  0x8a66,  0x1813,
  0x909e,  0x5f6b,  0x0120,  0x7c87,  0x6c87,  0x8bf5,  0x710d,  0xb98a,
  0xf609,  0x2d70,  0x45d6,  0x7cdc,  0x20fa,  0x90db,  0x65ef,  0xcce3,
  0xa337,  0xef3a,  0xc169,  0x763a,  0x5788,  0x14d1,  0xc4ef,  0xdd5d,
  0x195b,  0x11e2,  0xc46e,  0xf542,  0x220e,  0x4ebb,  0xf8c8,  0xf703,
  0x3f96,  0x4db9,  0xdab6,  0xb543,  0x4033,  0x0fbb,  0xac70,  0xac2a,
  0x3c57,  0x26a5,  0xa1a0,  0xe95a,  0x9e17,  0x7418,  0x5b8f,  0x14c2,
  0x7992,  0x8e82,  0x0df1,  0x958b,  0xbd6e,  0x8724,  0x9801,  0x6863,
  0xfad8,  0xc47c,  0x943f,  0x700d,  0xc9f6,  0x4132,  0x693e,  0x25ef,
  0x72de,  0xe3eb,  0x2865,  0xa7d3,  0x5b59,  0xfddd,  0x1506,  0xd18c,
  0xf057,  0xc00b,  0xc8bf,  0x1c4e,  0xf3e7,  0x426a,  0x11c4,  0x26ea,
  0x2ae4,  0x76ac,  0xa88d,  0xa031,  0x7b26,  0x7d80,  0xb902,  0x973f,
  0x4e6c,  0x33d7,  0x9ab5,  0x9b61,  0x8b65,  0x4f0d,  0x29b4,  0x01b0,
  0x4287,  0xfcb9,  0x1883,  0xae9e,  0xcf55,  0x9256,  0xa314,  0x1a58,
  0xefaa,  0xff69,  0xe604,  0x657f,  0xf2e3,  0x3309,  0x7c4c,  0x1efa,
  0x00e5,  0xf699,  0x1370,  0xd5e8,  0x4e2b,  0xc6c8,  0x673d,  0xc4fe,
  0xcb42,  0xb230,  0xddcd,  0x275b,  0x81dc,  0x5718,  0x2ad1,  0x54d1,
  0x2635,  0x9fa0,  0x7964,  0xace0,  0x922a,  0xac69,  0xb5d3,  0x7e33,
  0x9f85,  0x3f06,  0x73b9,  0x4a88,  0x87b4,  0xa601,  0xf85d,  0x0d61,
  0xab8b,  0x2d50,  0x1452,  0x4792,  0x1ebc,  0x9e87,  0x4a18,  0xcbb1,
  0xe37b,  0x1665,  0x37ed,  0x69ae,  0x1bef,  0xe2e0,  0x709d,  0xf7f6,
  0xd10c,  0xfa48,  0xfa7c,  0x0401,  0x42fa,  0x2fc4,  0xb6d4,  0xc82f,
  0x224e,  0x63d9,  0xd11c,  0xce57,  0x5035,  0x5bc9,  0xc3dd,  0x8538
};

//---------- static function declarations -----------------
static U32 crc24q_calculate_by_tab( const U32 dws_ary[], S32 n_bits );
static U32 get_value_from_crc24q_table( S16 input );
static void transform_u08_ary_to_u32_ary( const U08 bytes_ary[], S32 n_bytes, U32 dws_ary[], S32* n_bits_p );

#if 0 //the result of this func is the same as crc24q_calculate_by_tab(), save it for future use.
//void Generate_crc24q_tab( void );
static U32 crc24q_parity_calculate( const U32 *dws_p, S32 n_bits );
#endif

//---------- functions definition --------------------------------------------//

//---------------------------------------------------------
// CRC24Q_check_for_rtcm_3_x : check data words parity ( shift one bit once )
//
// Input: bytes_ary - data words array, each word contains 8 bits
//        n_bytes - total number of bytes
//
// Return : TRUE - crc 24Q parity check success ( message is intact )
//          FALSE - crc 24Q parity check failed ( message has been corrupted )
//---------------------------------------------------------
S16 CRC24Q_check_bytes_array( const U08 bytes_ary[], S32 n_bytes )
{
  U32 dws_ary[512]={0};
  S32 n_bits=0;
  //( (n_bytes >> 2) <= NELEMS( dws_ary ) ); //if > 256, dws[] size need to be changed.

  transform_u08_ary_to_u32_ary(bytes_ary, n_bytes, dws_ary, &n_bits);

  if( crc24q_calculate_by_tab( dws_ary, n_bits )& 0x00ffffffUL )
    return FALSE;
  return TRUE;
}

//---------------------------------------------------------
// CRC24Q_calc_bytes_array : get data words parity ( shift one bit once )
//
// Input: bytes_ary - data words array, each word contains 8 bits
//        n_bytes - total number of bytes
//
// Return : crc 24Q parity (total 24bits which is put in the LSB of a 32bits word)
//---------------------------------------------------------
U32 CRC24Q_calc_bytes_array( const U08 bytes_ary[], S32 n_bytes )
{
  U32 dws_ary[512]={0};
  S32 n_bits=0;
  //assert( (n_bytes >> 2) <= NELEMS( dws_ary ) ); //if > 256, dws[] size need to be changed.

  transform_u08_ary_to_u32_ary(bytes_ary, n_bytes, dws_ary, &n_bits);

  U32 crc = crc24q_calculate_by_tab( dws_ary, n_bits );
  return crc;
}

//---------------------------------------------------------
// CRC24Q_check : check data words parity ( use table driven algorithm )
//
// Input: dws_ary - data words array, each word contains 32 bits
//        n_bits - total number of bits
// Return : TRUE - crc 24Q parity check success ( message is intact )
//          FALSE - crc 24Q parity check failed ( message has been corrupted )
//---------------------------------------------------------
S16 CRC24Q_check( const U32 dws_ary[], S32 n_bits )
{
  if( crc24q_calculate_by_tab( dws_ary, n_bits )& 0x00ffffffUL )
    return FALSE;
  return TRUE;
}

//---------------------------------------------------------
// CRC24Q_get : get data words parity ( use table driven algorithm )
//
// Input: dws_ary - data words array, each word contains 32 bits
//        n_bits - total number of bits
// Return : crc 24Q parity (total 24bits which is put in the LSB of a 32bits word)
//---------------------------------------------------------
U32 CRC24Q_calc( const U32 dws_ary[], S32 n_bits )
{
  U32 crc = crc24q_calculate_by_tab( dws_ary, n_bits );
  return crc;
}

//---------- static functions definition -------------------------------------//
#if 0 //the result of this func is the same as crc24q_parity_calculate(), save it for future use and check.
//---------------------------------------------------------
// crc24q_parity_calculate : calculate data words parity
//
// Input: dws_ary - data words array, each word contains 32 bits
//        n_bits - total number of bits

// Return : crc 24Q parity (total 24bits which is put in the LSB of a 32bits word)
//---------------------------------------------------------
static U32 crc24q_parity_calculate( const U32 dws_ary[], S32 n_bits )
{
  S16 i,j ;
  U32 reg = 0 ;                      // only LSB 24 bits
  S16 word_ndx = (n_bits >> 5);           // each word has 32 bits
  S16 bit_ndx = (n_bits & 0x1f);          // ( 0 ~ 31 )

  // step 1 : input complete words(32 bits) into register & calculate one bit at a time
  for( i = 0 ; i < word_ndx ; i++ )
  {
    for( j = 31 ; j >= 0 ; j-- )
    {
      reg <<= 1 ;

      if( ( dws_ary[i]>>j )&( BIT0 ) ) // check input bit = 1 or 0
        reg ^= BIT0 ;

      if( reg & BIT24 )              // check output bit = 1 or 0
        reg ^= CRC24_POLY;
    }
  }

  // step 2 : input last uncompleted word into register, one bit at a time
  for( j = 31 ; j >= ( 32 - bit_ndx ) ; j-- )
  {
    reg <<= 1 ;

    if( ( dws_ary[word_ndx]>>j )&( BIT0 ) )
      reg ^= BIT0 ;

    if( reg & BIT24 )
      reg ^= CRC24_POLY;
  }

  // final :
  U32 crc = reg & 0x00ffffffUL;     // only LSB 24 bits
  return crc;
}
#endif

//---------------------------------------------------------
// crc24q_calculate_by_tab : calculate data words parity ( use table driven algorithm )
//
// Input: dws_ary - data words array, each word contains 32 bits
//        n_bits - total number of bits

// Return : crc 24Q parity (total 24bits which is put in the LSB of a 32bits word)
//---------------------------------------------------------
static U32 crc24q_calculate_by_tab( const U32 dws_ary[], S32 n_bits )
{
  S16 i, j;
  U32 value;                     // index value shifted out from register
  U32 reg = 0;                   // only LSB 24 bits

  S16 word_ndx = (S16)(n_bits >> 5);       // each word has 32 bits
  S16 char_ndx = (n_bits & 0x1f)>>3;  // char number of the last uncomplete word ( 0 ~ 3 )
  S16 bit_ndx = (n_bits & 0x07);      // bit number of the last uncomplete char ( 0 ~ 8)

  // step 1 : input complete words(32 bits) into register & calculate one byte at a time
  for( i = 0 ; i < word_ndx ; i++ )
  {
    for( j = 3 ; j >= 0 ; j-- )
    {
      value = 0;
      reg = ( reg<<8 ) | ( (dws_ary[i]>>(j*8))&0x000000ffUL );
      value = ( reg >> 24 );

      if( value )
        reg ^= get_value_from_crc24q_table((S16)value);
    }
  }

  // step 2 : input complete chars(8 bits) into register & calculate one byte at a time
  for( j = 3 ; j >= ( 4 - char_ndx ) ; j-- )
  {
    value = 0;
    reg = ( reg<<8 ) | ( (dws_ary[word_ndx]>>(j*8))&0x000000ffUL );
    value = ( reg >> 24 );

    if( value )
      reg ^= get_value_from_crc24q_table((S16)value);
  }

  // step 3 : input last uncompleted word into register, one bit at a time
  for( j = 7 ; j >= ( 8 - bit_ndx ) ; j-- )
  {
    reg <<= 1 ;

    if( ( (dws_ary[word_ndx]>>((3-char_ndx)*8))>>j )&( BIT0 ) )
      reg ^= BIT0 ;

    if( reg & BIT24 )
      reg ^= CRC24_POLY;
  }

  // final :
  U32 crc = reg & 0x00ffffffUL;     // only LSB 24 bits
  return crc;
}

//---------------------------------------------------------
// get_value_from_crc24q_table :
//
// Input : input - index value shifted out from register
// Output : corrosponding value gotten by index value from " U16 Crc24_Q_tab[384] "
//---------------------------------------------------------
static U32 get_value_from_crc24q_table( S16 input )
{
  U32 value = 0;
  S16 a_ndx = 0, b_ndx = 0, c_ndx = 0;

  //  a_ndx  v        0           1    <-- b_ndx
  //          ----------- -----------
  //         |   input   |   input   |
  //          ----------- -----------
  //                    ||
  //                    vv
  //          ------- ------- -------
  //         |  U16  |  U16  |  U16  |   U16 Crc24_Q_tab[384]
  //          ------- ------- -------
  //         ^
  //         c_ndx

  a_ndx = input/2;
  b_ndx = input - a_ndx*2;
  c_ndx = a_ndx*3;

  if( b_ndx == 0 )
    value = ( ((U32)Crc24_Q_table[c_ndx])<<8 ) | ( ((U32)Crc24_Q_table[c_ndx+1])>>8 );
  //value = ( ((U32)Crc24_Q_tab[c_ndx])<<8 ) | ( ((U32)Crc24_Q_tab[c_ndx+1])>>8 );

  if( b_ndx == 1 )
    value = ( ((U32)Crc24_Q_table[c_ndx+1])<<16 ) | ( ((U32)Crc24_Q_table[c_ndx+2])&0x0000ffff );
  //value = ( ((U32)Crc24_Q_tab[c_ndx+1])<<16 ) | ( ((U32)Crc24_Q_tab[c_ndx+2])&0x0000ffff );

  value &= 0x00ffffff;

  return value;
}

//---------------------------------------------------------
static void transform_u08_ary_to_u32_ary( const U08 bytes_ary[], S32 n_bytes, U32 dws_ary[], S32* n_bits_p )
{
  S16 i;
  S16 n_word = (S16)(n_bytes >> 2);
  S16 remain_n_bytes = n_bytes & 0x3;

  // complete 4 bytes
  for( i=0;i<n_word;i++ )
  {
    dws_ary[i] = ( ((((U32)(bytes_ary[i*4+0]))<<24) & 0xff000000) |
                   ((((U32)(bytes_ary[i*4+1]))<<16) & 0x00ff0000) |
                   ((((U32)(bytes_ary[i*4+2]))<< 8) & 0x0000ff00) |
                   ((((U32)(bytes_ary[i*4+3]))<< 0) & 0x000000ff)   );
    *n_bits_p += 32;
  }

  // remain byte : 0 ~ 3, only deal with 1 ~ 3
  if( remain_n_bytes > 0 )
  {
    for( i=1; i<=remain_n_bytes; i++ )
    {
      //U32 tmp = (U32)bytes_ary[n_word*4+i-1];
      dws_ary[n_word] |= ((((U32)bytes_ary[n_word*4+i-1]) & 0xff) << (32 - 8*i));
      *n_bits_p += 8;
    }
  }
}

//---------------------------------------------------------
// Generate_crc24q_tab : generate crc 24Q table stored into " U16 Crc24_Q_tab[384] "
//
// Description : Pre-computed table which contain table driven algorithm works shift
//               by one byte at a time and examining the byte that is shifted out.
//
#if 0
void Generate_crc24q_tab( void )
{
  S16 i,j;
  U32 crc24_Q_tem[256];

  for( j = 0 ; j < 256 ; j++ )
  {
    U08 buf = 0;         // byte value which shifted out from register
    U08 xor_flag = 0 ;   // record bit positon had been Xor-ed with CRC24_POLY
    U32 out = 0;         // 24 LSB only
    U08 poly = 0xc3;     // 8 MSB of CRC24_POLY
    U08 mask = 0x80;

    //init
    buf = j;

    // step1 : record bit positon had been Xor-ed with CRC24_POLY
    for( i = 0 ; i < 8 ; i++ )
    {
      if( buf & (mask>>i) )
      {
        buf ^= (poly>>i);
        xor_flag |= (mask>>i);
      }
    }

    // step2 : calculate corrosponding value & store into U32 crc24_Q_tem[]
    for( i = 0 ; i < 8 ; i++ )
    {
      if( xor_flag & (mask>>i) )
        out ^= ( CRC24_POLY<<(7-i) );
    }
    out &= 0x00ffffff;
    crc24_Q_tem[j] = out;//

  }

  // step 3 : store U32 crc24_Q_tem[256] into U16 Crc24_Q_tab[384]
  //
  //  c_ndx  v
  //          ----------- -----------
  //         |  24 bits  |  24 bits  |   U32 crc24_Q_tem[256]
  //          ----------- -----------
  //                    ||
  //                    vv
  //          ------- ------- -------
  //         |  U16  |  U16  |  U16  |   U16 Crc24_Q_tab[384]
  //          ------- ------- -------
  //         ^      0       1       2   <-- b_ndx
  //         a_ndx

  for( i = 0 ; i < 384 ; i++ )
  {
    S16 a_ndx = 0;
    S16 b_ndx = 0;
    S16 c_ndx = 0;

    a_ndx = i/3;
    b_ndx = i - a_ndx*3; // 0 ~ 2
    c_ndx = a_ndx*2;

    if( b_ndx == 0 )
      Crc24_Q_tab[i] = (U16)( (crc24_Q_tem[c_ndx]>>8)&0x0000ffff );

    if( b_ndx == 2 )
      Crc24_Q_tab[i] = (U16)( crc24_Q_tem[c_ndx+1]&0x0000ffff );

    if( b_ndx == 1 )
      Crc24_Q_tab[i] = (U16)(( (crc24_Q_tem[c_ndx]<<8)&0x0000ff00 ) | ( (crc24_Q_tem[c_ndx+1]>>16)&0x000000ff ));
  }
}
#endif
