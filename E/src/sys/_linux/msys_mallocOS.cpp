//--------------------------------------------------------------------------//
// iq . 2003 . code for the Paradise 64 kb intro by RGBA                    //
//--------------------------------------------------------------------------//

#include <malloc.h>
#include <string.h>
#ifdef DEBUG
#include <stdio.h>
#include <stdlib.h>
#endif
#include "../mmalloc.h"


//-----------------------------------------------------------------------------


void *mmalloc( long amount )
{
    void *ptr;

    ptr = malloc( amount );

    if( ptr )
        memset( ptr, 0, amount );

    return( ptr );
}

void mfree( void *ptr )
{
    free( ptr );
}


//-----------------------------------------------------------------------------

#ifdef DEBUG
void halt_program( char *str )
{
    printf( "Halt! %s\n" str );
    ::exit( 0 );
}
#endif


