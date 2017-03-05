#include <unistd.h>
#include <pthread.h>
#include <limits.h>
#include <sys/errno.h>

#include "../msys_thread.h"

long msys_threadNew( THREAD_FUNC func )
{
    pthread_t       id;
/*
    pthread_attr_t  attr;

    pthread_attr_init( &attr );
    pthread_attr_setdetachstate( &attr, PTHREAD_CREATE_JOINABLE );
    pthread_attr_setscope( &attr, PTHREAD_SCOPE_SYSTEM );
*/

    pthread_create( &id, NULL, (void *(*)(void*))func, NULL );

    
    //if( pthread_create( &id, &attr, func, (void*)sth ) )
    //	return( -1 );

    return( (long)id );
}



void msys_threadDelete( int id)
{
    //CloseHandle( (HANDLE)id );
}
