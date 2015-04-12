//--------------------------------------------------------------------------//
// iq . 2003 . code for the Paradise 64 kb intro by RGBA                    //
//--------------------------------------------------------------------------//

//#define WIN32_LEAN_AND_MEAN
//#define WIN32_EXTRA_LEAN
//#include <windows.h>
#include <string.h>

//static HBITMAP      hbmp;
//static HDC          hdc;
//static HDC          winhdc;

void FONT_Init( long h )
{
    //winhdc = (HDC)h;
}

void FONT_Start( unsigned char *buffer, int xres, int yres )
{
    //hdc = CreateCompatibleDC( winhdc );
    //hbmp =  CreateBitmap(xres,yres,1,32,buffer );
    //SelectObject( hdc, hbmp );
    //SetBkMode( hdc, TRANSPARENT );
}

void FONT_Stop( unsigned char *buffer, int xres, int yres )
{
    //GetBitmapBits( hbmp, xres*yres*4, buffer );
    //DeleteObject( hbmp );
}

void FONT_Print( char *str, int size, int xo, int yo )
{
    //SelectObject( hdc, CreateFont( size,0,0,0,0,0,0,0,0,OUT_TT_PRECIS*0,0,ANTIALIASED_QUALITY,0,"Times New Roman") );    
    //SetTextColor( hdc, 0x00ffffff );

    while( str[0] )
        {    
        //TextOut( hdc, xo, yo, str, strlen(str) );

        yo += 36;

        str+=1+strlen(str);
        if( str[0]=='\n') 
            {
            yo+=36; str++; 
            }
        }

}
