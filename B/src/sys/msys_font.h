//--------------------------------------------------------------------------//
// iq . 2003/2008 . code for 64 kb intros by RGBA                           //
//--------------------------------------------------------------------------//

#ifndef _MSYS_FONT_H_
#define _MSYS_FONT_H_


void msys_fontInit( uint64 winhdc );
void msys_fontStart( unsigned char *buffer, int xres, int yres );
void msys_fontStop( unsigned char *buffer, int xres, int yres );
void msys_fontPrint( char *str, int size, int x, int y );

#endif
