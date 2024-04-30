//--------------------------------------------------------------------------//
// iq . 2003/2008 . code for 64 kb intros by RGBA                           //
//--------------------------------------------------------------------------//

#ifndef _MSYS_LIBC_H_
#define _MSYS_LIBC_H_

#include "msys_types.h"

#define PI 3.1415926535897932384626433832795f
#define RAD_TO_DEG (180.f / PI)
#define DEG_TO_RAD (PI / 180.f)

#define ARRAY_LEN(arr) (sizeof(arr) / sizeof(arr[0]))

template<class T> const T& msys_min(const T& a, const T& b) { return (a < b) ? a : b; }
template<class T> const T& msys_max(const T& a, const T& b) { return (b < a) ? a : b; }


#ifndef WINDOWS

    #include <stdlib.h>
    #include <string.h>
    #include <math.h>
    #define msys_sinf(a)        sinf(a)
    #define msys_cosf(a)        cosf(a)
    #define msys_acosf(a)       acosf(a)
    #define msys_sqrtf(a)       sqrtf(a)
    #define msys_atanf(a)       atanf(a)
    #define msys_atan2f(a,b)    atan2f(a,b)
    #define msys_tanf(a)        tanf(a)
    #define msys_fabsf(a)       fabsf(a)
    #define msys_logf(a)        logf(a)
    #define msys_log10f(a)      log10f(a)
    #define msys_expf(a)        expf(a)
    #define msys_powf(a, b)     powf(a, b)
    #define msys_ifloorf(a)     ((int) (a))

    #define msys_memset(a,b,c)  memset(a,b,c)
    #define msys_memcpy(a,b,c)  memcpy(a,b,c)
    #define msys_memdup(src, size) memcpy(malloc(size), src, size)

    #define msys_strlen(a)      strlen(a)
    #define msys_strcpy(a,b)    strcpy(a,b)
    #define msys_strncpy(a,b,n) strncpy(a,b,n)
    #define msys_strcmp(a,b)    strcmp(a,b)
    #define msys_strncmp(a,b,n) strncmp(a,b,n)

#else

    #ifdef A32BITS

        #if 0
        MSYS_INLINE float msys_sinf( const float x) { float r; _asm fld  dword ptr [x];
                                                               _asm fsin;
                                                               _asm fstp dword ptr [r];
                                                               return r; }
        MSYS_INLINE float msys_cosf( const float x) { float r; _asm fld  dword ptr [x];
                                                               _asm fcos;
                                                               _asm fstp dword ptr [r];
                                                               return r; }
        MSYS_INLINE float msys_sqrtf(const float x) { float r; _asm fld  dword ptr [x];
                                                               _asm fsqrt;
                                                               _asm fstp dword ptr [r];
                                                               return r; }
        MSYS_INLINE float msys_fabsf(const float x) { float r; _asm fld  dword ptr [x];
                                                               _asm fabs;
                                                               _asm fstp dword ptr [r];
                                                               return r; }
        MSYS_INLINE float msys_tanf( const float x) { float r; _asm fld  dword ptr [x];
                                                               _asm fptan;
                                                               _asm fstp st(0)
                                                               _asm fstp dword ptr [r];
                                                               return r; }
        #else
        #include <math.h>

        #define msys_sinf(a)       sinf(a)
        #define msys_cosf(a)       cosf(a)
        #define msys_sqrtf(a)      sqrtf(a)
        #define msys_fabsf(a)      fabsf(a)
        #define msys_atanf(a)      atanf(a)
        #define msys_atan2f(a,b)   atan2f(a,b)
        #define msys_tanf(a)       tanf(a)
        #endif

        MSYS_INLINE float msys_acosf( const float x) // Approximation, récupérée sur Internet
        {
          float fV      = msys_fabsf(x);
          float fRoot   = msys_sqrtf(1.f-fV);
          float fResult = -0.0187293f;

          fResult *= fV;
          fResult += 0.0742610f;
          fResult *= fV;
          fResult -= 0.2121144f;
          fResult *= fV;
          //fResult += 1.5707288f;
          fResult += 1.5707963267948966192313216916398f;
          fResult *= fRoot;

          return (x<0.0f) ? PI - fResult : fResult;
        }

        MSYS_INLINE void msys_sincosf( float x, float *r ) {   _asm fld dword ptr [x];
                                                               _asm fsincos;
                                                               _asm fstp dword ptr [r+0];
                                                               _asm fstp dword ptr [r+4];
                                                               }
        float  msys_log2f( const float x );
        float  msys_expf( const float x );
        float  msys_fmodf( const float x, const float y );
        float  msys_powf( const float x, const float y );
        float  msys_floorf( const float x );
        int    msys_ifloorf( const float x );

        void   msys_memset( void *dst, int val, int amount );
        void*  msys_memcpy( void *dst, const void *ori, size_t amount );
        void*  msys_memdup(const void *src, size_t size);

        size_t msys_strlen( const char *str );
        char*  msys_strcpy(char *dst, const char *src);
        char*  msys_strncpy(char *dst, const char *src, size_t n);
        int    msys_strcmp(const char *s1, const char *s2);
        int    msys_strncmp(const char *s1, const char *s2, size_t n);
    #else
        // x64
        extern "C" float msys_sinf(  const float x );
        extern "C" float msys_cosf(  const float x );
        extern "C" float msys_acosf( const float x );
        extern "C" float msys_sqrtf( const float x );
        extern "C" float msys_fabsf( const float x );
        extern "C" float msys_tanf(  const float x );
        extern "C" void  msys_sincosf( float x, float *r );
        extern "C" float msys_log2f( const float x );
        extern "C" float msys_expf( const float x );
        extern "C" float msys_fmodf( const float x, const float y );
        extern "C" float msys_powf( const float x, const float y );
        extern "C" float msys_floorf( const float x );
        extern "C" int   msys_ifloorf( const float x );

        extern "C" void   msys_memset( void *dst, int val, int amount );
        extern "C" void   msys_memcpy( void *dst, const void *ori, int amount );
        extern "C" void*  msys_memdup(const void *src, size_t size);

        extern "C" size_t msys_strlen( const char *str );
        extern "C" char*  msys_strcpy(char *dst, const char *src);
        extern "C" char*  msys_strncpy(char *dst, const char *src, size_t n);
        extern "C" int    msys_strcmp(const char *s1, const char *s2);
        extern "C" int    msys_strncmp(const char *s1, const char *s2, size_t n);

    #endif
#endif
#endif
