//--------------------------------------------------------------------------//
// iq . 2003/2008 . code for 64 kb intros by RGBA                           //
//--------------------------------------------------------------------------//

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#include <mmsystem.h>


long msys_timerGet( void )
{
    return( timeGetTime() );
}

void msys_timerSleep( long miliseconds )
{
    Sleep( miliseconds );
}
