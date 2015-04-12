//--------------------------------------------------------------------------//
// iq . 2003/2008 . code for 64 kb intros by RGBA                           //
//--------------------------------------------------------------------------//


#include "msys_types.h"
#include "msys_malloc.h"

//--- static allocations --------------------------------------------------------------------------

#define AMOUNT      (32*1024*1024)

static char    *big_static_buffer = 0;
static char    *big_real_static_buffer = 0;
static intptr   big_static_position = 0;

int msys_mallocInit( void )
{
    big_real_static_buffer = (char*)msys_mallocAlloc( AMOUNT );
    if( !big_real_static_buffer )
        return( 0 );

    big_static_buffer = big_real_static_buffer;

    // align pointer to 32 bit
    intptr mo = (intptr)big_static_buffer & 31;
    if( mo ) mo = 32-mo;
    big_static_buffer = (char*)((intptr)big_static_buffer+mo);

    return( 1 );
}

void msys_mallocEnd( void )
{
    msys_mallocFree( big_real_static_buffer );
}



void *msys_mallocSAlloc( uint32 amount )
{
    char *ret = big_static_buffer+big_static_position;

    big_static_position += amount;

    return( (void*)ret );
}

/*
void *samalloc( long amount )
{
    // align pointer to 32 bit
    intptr mo = big_static_position & 31;
    if( mo ) mo = 32-mo;
    big_static_position += mo;

    return( smalloc(amount) );
}
*/

/*
void * __cdecl operator new( unsigned int size ) 
{ 
    return(msys_mallocAlloc(size)); 

}
void __cdecl operator delete( void *ptr ) 
{ 
    msys_mallocFree(ptr); 
}*/

