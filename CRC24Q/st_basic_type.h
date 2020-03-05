
#ifndef ST_BASIC_TYPE_INCLUDED
#define ST_BASIC_TYPE_INCLUDED

// for inline definitions reference:
//   https://www.greenend.org.uk/rjk/tech/inline.html
#ifndef EXTERN_INLINE
#if defined(__GNUC__) && !defined(__GNUC_STDC_INLINE__)
#define EXTERN_INLINE   extern inline  // extern inline only support in older gcc
#else
#define EXTERN_INLINE   inline
#endif
#endif

// define standard type limits
#include <limits.h>

#define S08_MIN   SCHAR_MIN
#define S08_MAX   SCHAR_MAX
#define U08_MAX   UCHAR_MAX

#define S16_MIN   SHRT_MIN
#define S16_MAX   SHRT_MAX
#define U16_MAX   USHRT_MAX

#define S32_MIN   (-S32_MAX - 1)
#define S32_MAX   2147483647
#define U32_MAX   4294967295U

// boolean types, it can contains TRUE or FALSE only
// the BOOL08 is an 8 bits version type, it used in global variable or struct
// element only (to save memory usage.) To using boolean type for local
// variables or function parameters, use the standard 'BOOL' version
typedef int                       BOOL;
typedef char                      BOOL08;
// standard types
typedef int                       SINT;
typedef unsigned int              UINT;
typedef char                      S08;
typedef unsigned char             U08;
typedef short int                 S16;
typedef unsigned short int        U16;
#if (S32_MAX == INT_MAX)  // the INT_MAX is defined in <limits.h>
typedef int                       S32;
typedef unsigned int              U32;
#else
typedef long int                  S32;
typedef unsigned long int         U32;
#endif
typedef float                     F32;
typedef double                    D64;
typedef long long int             S64;
typedef unsigned long long int    U64;

typedef SINT                     *PSINT;
typedef UINT                     *PUINT;
typedef S08                      *PS08;
typedef U08                      *PU08;
typedef S16                      *PS16;
typedef U16                      *PU16;
typedef S32                      *PS32;
typedef U32                      *PU32;
typedef F32                      *PF32;
typedef D64                      *PD64;
typedef long long int            *PS64;
typedef unsigned long long int   *PU64;

// ECEF position type
typedef struct {
    D64 px;
    D64 py;
    D64 pz;
} POS_T;

// ECEF velocity type
typedef struct {
    F32 vx;
    F32 vy;
    F32 vz;
} VEL_T;

typedef VEL_T *VEL_PT;
/*
typedef struct {
    D64 vx;
    D64 vy;
    D64 vz;
} VEL_D64_T;

typedef VEL_D64_T *VEL_D64_T;
u
*/

typedef struct {
  F32 ax;
  F32 ay;
  F32 az;
} ACC_T;

// position in latitude, longitude and altitude
typedef struct {
    D64 lat;
    D64 lon;
    D64 alt;
} LLA_T;
typedef LLA_T *LLA_PT;

// position in latitude, longitude and height, this is an alternative
// name of LLA_T
typedef struct {
    D64 lat;
    D64 lon;
    D64 h;
} LLH_T;

typedef LLH_T *LLH_PT;

// the relative pointer in ENU coordinate
typedef struct {
  D64 pe;
  D64 pn;
  D64 pu;
} POS_ENU_T;

// Velocity in geodetic frame (ENU)
typedef struct {
    F32 ve;
    F32 vn;
    F32 vu;
} VEL_ENU_T;

typedef VEL_ENU_T *VEL_ENU_PT;

// Velocity in geodetic frame (NED)
typedef struct {
    D64 vn;
    D64 ve;
    D64 vd;
} VEL_G_T;

typedef VEL_G_T *VEL_G_PT;

// Velocity in navigation (WA) frame
typedef struct {
    D64 vx;
    D64 vy;
    D64 vd;
} VEL_N_T;

typedef VEL_N_T *VEL_N_PT;

#endif  // #ifndef ST_TYPE_INCLUDED
