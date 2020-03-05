#ifndef ST_BASIC_CONST_INCLUDED
#define ST_BASIC_CONST_INCLUDED

// Bool constants
#ifndef FALSE
#define FALSE       0
#endif  // #ifndef FALSE
#ifndef TRUE
#define TRUE        (!FALSE)
#endif  // #ifndef TRUE

// Bit constants
#define BIT0        (0x01)
#define BIT1        (0x02)
#define BIT2        (0x04)
#define BIT3        (0x08)
#define BIT4        (0x10)
#define BIT5        (0x20)
#define BIT6        (0x40)
#define BIT7        (0x80)
#define BIT8        (0x0100)
#define BIT9        (0x0200)
#define BIT10       (0x0400)
#define BIT11       (0x0800)
#define BIT12       (0x1000)
#define BIT13       (0x2000)
#define BIT14       (0x4000)
#define BIT15       (0x8000)
#define BIT16       (0x00010000L)
#define BIT17       (0x00020000L)
#define BIT18       (0x00040000L)
#define BIT19       (0x00080000L)
#define BIT20       (0x00100000L)
#define BIT21       (0x00200000L)
#define BIT22       (0x00400000L)
#define BIT23       (0x00800000L)
#define BIT24       (0x01000000L)
#define BIT25       (0x02000000L)
#define BIT26       (0x04000000L)
#define BIT27       (0x08000000L)
#define BIT28       (0x10000000L)
#define BIT29       (0x20000000L)
#define BIT30       (0x40000000L)
#define BIT31       (0x80000000L)

// scientist constants
#define SPEED_OF_LIGHT  (299792458.0)

#define R2D  (57.2957795131)  // deg / rad

#ifdef MAKE_REPLAY
#define IS_MAKING_REPLAY     1
#else
#define IS_MAKING_REPLAY     0
#endif

#endif // #ifndef ST_BASIC_CONST_INCLUDED 
