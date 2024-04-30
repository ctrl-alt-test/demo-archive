//--------------------------------------------------------------------------//
// iq . 2003 . code for the Paradise 64 kb intro by RGBA                    //
//--------------------------------------------------------------------------//

#include <malloc.h>
#include <string.h>
#include "../../types.h"

//-----------------------------------------------------------------------------


void *mmalloc( long amount )
{
    void *ptr;

    ptr = malloc( amount );

    memset( ptr, 0, amount );

    return( ptr );
}

void mfree( void *ptr )
{
    free( ptr );
}

