//--------------------------------------------------------------------------//
// iq . 2003/2008 . code for 64 kb intros by RGBA                           //
//--------------------------------------------------------------------------//

#include <math.h>
#include <string.h>



extern "C" float msys_sinf(  const float x )
{
    return sinf( x );
}

extern "C" float msys_cosf(  const float x )
{
    return cosf( x );
}

extern "C" float msys_sqrtf( const float x )
{
    return sqrtf(x);
}

extern "C" float msys_fabsf( const float x )
{
    return fabsf(x);
}

extern "C" float msys_tanf(  const float x )
{
    return tanf(x);
}

extern "C" void msys_sincosf( float x, float *r )
{
    r[0] = sinf( x );
    r[1] = cosf( x );    
}

extern "C" float msys_log2f( const float x )
{
    return logf( x ) / logf( 2.0f );
}

extern "C" float msys_expf( const float x )
{
    return expf( x );
}

extern "C" float msys_fmodf( const float x, const float y )
{
    return fmodf( x, y );
}

extern "C" float msys_powf( const float x, const float y )
{
    return powf( x, y );
}

extern "C" float msys_floorf( const float x )
{
    return floorf( x );
}

extern "C" int msys_ifloorf( const float x )
{
    return (int)floorf( x );
}

void msys_memset( void *dst, int val, int amount )
{
    memset( dst, val, amount );
}

void msys_memcpy( void *dst, const void *ori, int amount )
{
    memcpy( dst, ori, amount );
}

size_t msys_strlen(const char *str)
{
    return strlen(str);
}

char* msys_strcpy(char *dst, const char *src)
{
    return strcpy(dst, src);
}

char* msys_strncpy(char *dst, const char *src, size_t n)
{
    return strncpy(dst, src, n);
}

int msys_strcmp(const char *s1, const char *s2)
{
	return strcmp(s1, s2);
}

int msys_strncmp(const char *s1, const char *s2, size_t n)
{
	return strncmp(s1, s2, n);
}
