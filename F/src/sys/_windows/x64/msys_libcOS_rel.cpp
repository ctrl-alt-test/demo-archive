//--------------------------------------------------------------------------//
// iq . 2003/2008 . code for 64 kb intros by RGBA                           //
//--------------------------------------------------------------------------//

void msys_memset( void *dst, int val, int amount )
{
    for( int i=0; i<amount; i++ ) ((char*)dst)[i] = val;
}

void msys_memcpy( void *dst, const void *ori, int amount )
{
    for( int i=0; i<amount; i++ ) ((char*)dst)[i] = ((char*)ori)[i];
}

int msys_strlen( const char *str )
{
    int n; for( n=0; str[n]; n++ ); return n;
}