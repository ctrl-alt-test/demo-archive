//--------------------------------------------------------------------------//
// iq . 2003/2008 . code for 64 kb intros by RGBA                           //
//--------------------------------------------------------------------------//

#ifndef _MSYS_THREAD_H_
#define _MSYS_THREAD_H_

typedef void (*THREAD_FUNC)( void *data );

long msys_threadNew( THREAD_FUNC func );
void msys_threadDelete( int id);

#endif