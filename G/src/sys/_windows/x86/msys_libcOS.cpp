//--------------------------------------------------------------------------//
// iq . 2003/2008 . code for 64 kb intros by RGBA                           //
//--------------------------------------------------------------------------//

#include "../../msys_debug.h"
#include "../../msys_libc.h"

float msys_log2f( const float x )
{
    float res;

    _asm fld    dword ptr [x]
    _asm fld1
    _asm fxch   st(1)
    _asm fyl2x
    _asm fstp   dword ptr [res]

    return res;
}

float msys_expf( const float x )
{
    float res;

    _asm fld     dword ptr [x]
    _asm fldl2e
    _asm fmulp   st(1), st(0)
    _asm fld1
    _asm fld     st(1)
    _asm fprem
    _asm f2xm1
    _asm faddp   st(1), st(0)
    _asm fscale
    _asm fxch    st(1)
    _asm fstp    st(0)
    _asm fstp    dword ptr [res]

    return res;
}

float msys_fmodf( const float x, const float y )
{
    float res;

    _asm fld     dword ptr [y]
    _asm fld     dword ptr [x]
    _asm fprem
    _asm fxch    st(1)
    _asm fstp    st(0)
    _asm fstp    dword ptr [res]

    return res;
}

float msys_powf( const float x, const float y )
{
    float res;

    _asm fld     dword ptr [y]
    _asm fld     dword ptr [x]
    _asm fyl2x
    _asm fld1
    _asm fld     st(1)
    _asm fprem
    _asm f2xm1
    _asm faddp   st(1), st(0)
    _asm fscale
    _asm fxch
    _asm fstp    st(0)
    _asm fstp    dword ptr [res];

    return res;
}

static short opc1 = 0x043f ;     // floor

int msys_ifloorf( const float x )
{
    int res;
    short tmp;

    _asm fstcw   word  ptr [tmp]
    _asm fld     dword ptr [x]
    _asm fldcw   word  ptr [opc1]
    _asm fistp   dword ptr [res]
    _asm fldcw   word  ptr [tmp]

    return res;
}


// float -> int
// Source : http://www.stereopsis.com/FPU.html
/*
const double _double2fixmagic = 68719476736.0*1.5;     //2^36 * 1.5,  (52-_shiftamt=36) uses limited precisicion to floor
const int _shiftamt = 16;                    //16.16 fixed point representation,

#if BigEndian_
	#define iexp_				0
	#define iman_				1
#else
	#define iexp_				1
	#define iman_				0
#endif //BigEndian_

int msys_floor(double val)
{
  val = val + _double2fixmagic;
  return ((int*)&val)[iman_] >> _shiftamt; 
}

int msys_floorf(float val)
{
  return msys_trunc((double)val);
}
*/

void msys_memset( void *dst, int val, int amount )
{
    _asm mov edi, dst
    _asm mov eax, val
    _asm mov ecx, amount
    _asm rep stosb
}

/*
void msys_memcpy( void *dst, const void *ori, int amount )
{
    _asm mov edi, dst
    _asm mov esi, ori
    _asm mov ecx, amount
    _asm rep movsb
}

int msys_strlen( const char *ori )
{
    int res;

    _asm mov esi, ori
    _asm xor ecx, ecx
    _asm myloop:
    _asm    mov al, [esi]
    _asm    inc esi
    _asm    inc ecx
    _asm    test al, al
    _asm    jnz myloop
    _asm dec ecx
    _asm mov [res], ecx

    return res;    
}
*/
size_t msys_strlen( const char *str )
{
    size_t n; for( n=0; str[n]; n++ ); return n;
}

char* msys_strcpy(char *dst, const char *src)
{
    size_t i; for( i=0; src[i]; dst[i] = src[i++] ); dst[i] = '\0'; return dst;
}

char* msys_strncpy(char *dst, const char *src, size_t n)
{
    size_t i; for( i=0; i<n && src[n]; dst[n] = src[n++]); dst[i] = '\0'; return dst;
}

int msys_strcmp(const char *s1, const char *s2)
{
	while(*s1 && (*s1==*s2))
		s1++,s2++;
	return *(const unsigned char*)s1-*(const unsigned char*)s2;
}

int msys_strncmp(const char *s1, const char *s2, size_t n)
{
	while(n--)
		if(*s1++!=*s2++)
			return *(unsigned char*)(s1 - 1) - *(unsigned char*)(s2 - 1);
	return 0;
}
