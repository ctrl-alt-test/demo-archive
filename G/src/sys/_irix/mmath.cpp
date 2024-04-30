#include <math.h>

#ifdef __cplusplus
extern "C"
{
#endif

float mfmod( float x, float y )
{
    return( fmodf( x, y ) );
}

float mpow( float x, float y )
{
    return( powf( x, y ) );
}

int mifloor( float x )
{
    return( (int)floorf(x) );
}

int mlog2( float x )
{
    return( log10f(x)*3.32192809f );
}

#ifdef __cplusplus
}
#endif
