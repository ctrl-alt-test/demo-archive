//--------------------------------------------------------------------------//
// iq . 2003/2008 . code for 64 kb intros by RGBA                           //
//--------------------------------------------------------------------------//

#ifndef _MSYS_TYPES_H_
#define _MSYS_TYPES_H_



// ---- types --------------------------------------------

#ifdef WINDOWS
 typedef __int64     	        sint64;
 typedef unsigned __int64       uint64;
#endif
#ifdef IRIX
 typedef   signed long          sint64;
 typedef unsigned long          uint64;
#endif
#ifdef LINUX
 #ifdef A64BITS
  typedef   signed long          sint64;
  typedef unsigned long          uint64;
 #else
  typedef          long long int sint64;
  typedef unsigned long long int uint64;
 #endif
#endif

typedef unsigned int    uint32;
typedef   signed int    sint32;
typedef unsigned short  uint16;
typedef   signed short  sint16;
typedef unsigned char   uint8;
typedef   signed char   sint8;

#ifdef A32BITS
    typedef uint32 intptr;
#endif
#ifdef A64BITS
    typedef uint64 intptr;
#endif

// ---- types --------------------------------------------

#ifdef WINDOWS
    #ifdef __cplusplus
    #define MSYS_INLINE  __forceinline
    #else
    #define MSYS_INLINE
    #endif
    #define MSYS_ALIGN16 __declspec(align(16))
#else
    #define MSYS_ALIGN16 __attribute__ ((aligned(16))) 
    #define MSYS_INLINE  inline
#endif

#endif

