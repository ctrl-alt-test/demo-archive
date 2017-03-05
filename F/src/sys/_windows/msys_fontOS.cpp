//--------------------------------------------------------------------------//
// iq . 2003/2008 . code for 64 kb intros by RGBA                           //
//--------------------------------------------------------------------------//

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#include "../msys_libc.h"

static HBITMAP      hbmp;
static HDC          hdc;
static HDC          winhdc;

void msys_fontInit( uint64 h )
{
    winhdc = (HDC)h;
}

void msys_fontStart( unsigned char *buffer, int xres, int yres )
{
    hdc = CreateCompatibleDC( winhdc );
    hbmp =  CreateBitmap(xres,yres,1,32,buffer );
    SelectObject( hdc, hbmp );
    SetBkMode( hdc, TRANSPARENT );
}

void msys_fontStop( unsigned char *buffer, int xres, int yres )
{
    GetBitmapBits( hbmp, xres*yres*4, buffer );
    DeleteObject( hbmp );
}

void msys_fontPrint( char *str, int size, int xo, int yo )
{
    SelectObject( hdc, CreateFont( size,0,0,0,0,0,0,0,0,OUT_TT_PRECIS*0,0,ANTIALIASED_QUALITY,0,"Times New Roman") );    

    SetTextColor( hdc, 0x00ffffff );

    while( str[0] )
    {    
        TextOut( hdc, xo, yo, str, msys_strlen(str) );
        yo += 36;
        str+=1+strlen(str);
        if( str[0]=='\n') 
        {
            yo+=36; str++; 
        }
    }
}
