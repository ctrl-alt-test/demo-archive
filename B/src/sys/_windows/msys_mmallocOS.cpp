//--------------------------------------------------------------------------//
// iq . 2003/2008 . code for 64 kb intros by RGBA                           //
//--------------------------------------------------------------------------//

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#include "../msys_types.h"

void *msys_mallocAlloc( uint32 amount )
{
    return( GlobalAlloc( GMEM_ZEROINIT, amount ) );
}

void msys_mallocFree( void *ptr )
{
    GlobalFree( ptr );
}

