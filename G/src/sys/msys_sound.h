//--------------------------------------------------------------------------//
// iq . 2003/2008 . code for 64 kb intros by RGBA                           //
//--------------------------------------------------------------------------//

#ifndef _MSYS_SOUND_H_
#define _MSYS_SOUND_H_

int  msys_soundInit( void *buffer, int totallengthinbytes );
void msys_soundStart( void );
void msys_soundEnd( void );
long msys_soundGetPosInSamples( void );

#endif