//--------------------------------------------------------------------------//
// iq . 2003 . code for the Paradise 64 kb intro by RGBA                    //
//--------------------------------------------------------------------------//
#define ANTI        1
#define XRES        800
#define YRES        600

#define ERRORMSG            // activa printado mensajes de error
#define LOADING             // activa pintado "wait while loading..."
#define AUTOCENTER          // activa centrado de la ventana

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#include <GL/gl.h>
#include <string.h>
#include "intro.h"
#include "sys/mmalloc.h"
#include "sys/font.h"
#include "sys/mdebug.h"
#include "raster.h"

//----------------------------------------------------------------------------

typedef struct
{
    //---------------
    HINSTANCE   hInstance;
    HDC         hDC;
    HGLRC       hRC;
    HWND        hWnd;
    //---------------
    int         full;
    //---------------
    char        wndclass[14];	// window class and title :)
    //---------------
}WININFO;


static PIXELFORMATDESCRIPTOR pfd =
    {
    sizeof(PIXELFORMATDESCRIPTOR),
    1,
    PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER,
    PFD_TYPE_RGBA,
    32,
    8, 0, 8, 0, 8, 0, 8, 0,
    0, 0, 0, 0, 0,  // accum
    32,             // zbuffer
    8,              // stencil!
    0,              // aux
    PFD_MAIN_PLANE,
    0, 0, 0, 0
    };

static WININFO wininfo = {  0,0,0,0,0,
							{'r','g','b','a','_','p','a','r','a','d','i','s','e',0}
                            };
#ifdef LOADING
static char fnt_wait[6]    = "arial";
static char msg_wait[22]   = "wait while loading...";
#endif
#ifdef ERRORMSG
static char msg_error[207] = "intro_init()!\n\n"\
                             "  no memory?\n"\
                             "  no music?\n"\
                             "  no GL_ARB_multitexture?\n"\
                             "  no GL_ARB_vertex_program?\n"\
                             "  no GL_ARB_fragment_program?\n"\
							 "  no GL_EXT_texture_lod_bias?\n"\
                             "  no GL_EXT_texture3D?\n"\
                             "  no GL_SGIS_generate_mipmap?";

static char tlt_error[6]   = "error";
#endif



//----------------------------------------------------------------------------



// n = [0..224]
void loadbar( int  n )
{
    WININFO *info = &wininfo;

    int     xo = 16*(XRES)>>8;
    int     y1 = (240*YRES)>>8;
    int     yo = y1-8;

    // draw background
    SelectObject( wininfo.hDC, CreateSolidBrush(0x0045302c) );
    Rectangle( wininfo.hDC, 0, 0, XRES, YRES );

    // draw text
    SetBkMode( wininfo.hDC, TRANSPARENT );
    SetTextColor( wininfo.hDC, 0x00ffffff );
    SelectObject( wininfo.hDC, CreateFont( 44,0,0,0,0,0,0,0,0,0,0,ANTIALIASED_QUALITY,0,fnt_wait) );
    TextOut( wininfo.hDC, (XRES-318)>>1, (YRES-38)>>1, msg_wait, 21 );
    //DrawText( info->hDC, msg_wait, 21, &rec, DT_CENTER|DT_VCENTER|DT_SINGLELINE );

    // draw bar
    SelectObject( wininfo.hDC, CreateSolidBrush(0x00705030) );
    Rectangle( wininfo.hDC, xo, yo, (240*XRES)>>8, y1 );
    SelectObject( wininfo.hDC, CreateSolidBrush(0x00f0d0b0) );
    Rectangle( wininfo.hDC, xo, yo, ((16+n)*XRES)>>8, y1 );
}



//----------------------------------------------------------------------------

static LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// salvapantallas
	if( uMsg==WM_SYSCOMMAND && (wParam==SC_SCREENSAVE || wParam==SC_MONITORPOWER) )
		return( 0 );

	// boton x o pulsacion de escape
	if( uMsg==WM_CLOSE || (uMsg==WM_KEYDOWN && wParam==VK_ESCAPE) )
		{
		PostQuitMessage(0);
        return( 0 );
		}

    return( DefWindowProc(hWnd,uMsg,wParam,lParam) );
}

static void window_end( WININFO *info )
{
    if( info->hRC )
        {
        wglMakeCurrent( 0, 0 );
        wglDeleteContext( info->hRC );
        }

    if( info->hDC  ) ReleaseDC( info->hWnd, info->hDC );
    if( info->hWnd ) DestroyWindow( info->hWnd );

    UnregisterClass( info->wndclass, info->hInstance );

    if( info->full )
        {
        ChangeDisplaySettings( 0, 0 );
		while( ShowCursor( 1 )<0 ); // show cursor
        }
}

static int window_init( WININFO *info )
{
	unsigned int	PixelFormat;
    DWORD			dwExStyle, dwStyle;
    DEVMODE			dmScreenSettings;
    WNDCLASS		wc;
    RECT			rec;

    memset( &wc, 0, sizeof(WNDCLASS) );
    wc.style         = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = info->hInstance;
    wc.lpszClassName = info->wndclass;
    //wc.hbrBackground=(HBRUSH)CreateSolidBrush(0x00785838);
	
    if( !RegisterClass(&wc) )
        return( 0 );

    if( info->full )
        {
        memset( &dmScreenSettings,0,sizeof(DEVMODE) );
        dmScreenSettings.dmSize       = sizeof(DEVMODE);
        dmScreenSettings.dmFields     = DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;
        dmScreenSettings.dmBitsPerPel = 32;
        dmScreenSettings.dmPelsWidth  = XRES;
        dmScreenSettings.dmPelsHeight = YRES;

        if( ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
            return( 0 );

        dwExStyle = WS_EX_APPWINDOW;
        dwStyle   = WS_VISIBLE | WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;

		while( ShowCursor( 0 )>=0 );	// hide cursor
        }
    else
        {
        dwExStyle = WS_EX_APPWINDOW | WS_EX_WINDOWEDGE;
        dwStyle   = WS_VISIBLE | WS_CAPTION | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU;
        }

    rec.left   = 0;
    rec.top    = 0;
    rec.right  = XRES;
    rec.bottom = YRES;

    AdjustWindowRect( &rec, dwStyle, 0 );

    #ifdef AUTOCENTER
        info->hWnd = CreateWindowEx( dwExStyle, wc.lpszClassName, wc.lpszClassName, dwStyle,
		//info->hWnd = CreateWindowEx( dwExStyle, "EDIT", wc.lpszClassName, dwStyle,
                                   (GetSystemMetrics(SM_CXSCREEN)-rec.right+rec.left)>>1,
                                   (GetSystemMetrics(SM_CYSCREEN)-rec.bottom+rec.top)>>1,
                                   rec.right-rec.left, rec.bottom-rec.top, 0, 0, info->hInstance, 0 );
    #else
        info->hWnd = CreateWindowEx( dwExStyle, wc.lpszClassName, wc.lpszClassName, dwStyle,
                                   0, 0, rec.right-rec.left, rec.bottom-rec.top,
                                   0, 0, info->hInstance, 0 );
    #endif

    if( !info->hWnd )
        return( 0 );

    if( !(info->hDC=GetDC(info->hWnd)) )
        return( 0 );

    if( !(PixelFormat=ChoosePixelFormat(info->hDC,&pfd)) )
        return( 0 );

    if( !SetPixelFormat(info->hDC,PixelFormat,&pfd) )
        return( 0 );

    if( !(info->hRC=wglCreateContext(info->hDC)) )
        return( 0 );

    if( !wglMakeCurrent(info->hDC,info->hRC) )
        return( 0 );
    
    //SetForegroundWindow( info->hWnd );    // slightly higher priority
    //SetFocus( info->hWnd );               // sets keyboard focus to the window
    
    return( 1 );
}

//----------------------------------------------------------------------------

static unsigned char *rbuf=0;
int externaltimer;
static int doframe( float *buffer, int xres, int yres, float t )
{
    unsigned char   *ptr;
    int             i;
    MSG				msg;
	int				done;

    //---------------------------------

//    glViewport( 0, 0, xres, yres );
    
	externaltimer = (int)(t*1000.0f);
	done = intro_do();

    glReadPixels( 0, 0, ANTI*xres, ANTI*yres, GL_RGBA, GL_UNSIGNED_BYTE, rbuf );

    ptr = rbuf;
    for( i=0; i<(ANTI*ANTI*xres*yres); i++ )
        {
        *buffer++ = ptr[0]*(1.0f/250.0f);
        *buffer++ = ptr[1]*(1.0f/250.0f);
        *buffer++ = ptr[2]*(1.0f/250.0f);
        ptr += 4;
        }

    //---------------------------------

    while( PeekMessage(&msg, wininfo.hWnd, 0, 0, PM_REMOVE) )
        {
		if( msg.message==WM_QUIT ) done=1;
        //TranslateMessage(&msg);
        DispatchMessage(&msg);
        }

    SwapBuffers( wininfo.hDC );

	return( done );
}

void OUT_Free( void )
{
    mfree( rbuf );
}


int OUT_Init( int xres, int yres )
{
    rbuf = (unsigned char*)mmalloc( ANTI*ANTI*xres*yres*4 );
    if( !rbuf )
        return( 0 );

    if( !RASTER_Config( xres, yres, ANTI,
                        //1.1f, 1.1f, 0.15f,   // gain, gamma, desa
						1.0f, 1.0f, 0.0f,   // gain, gamma, desa
                        1, 30.0f, 30*211, 0 ) )
        return( 0 );

    return( 1 );
}

//----------------------------------------------------------------------------

int WINAPI WinMain( HINSTANCE instance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
    int         done=0;
    WININFO     *info = &wininfo;

    info->hInstance = GetModuleHandle( 0 );


    if( !smalloc_init() )
        {
        #ifdef ERRORMSG
        MessageBox( 0, "window_init()!",tlt_error,MB_OK|MB_ICONEXCLAMATION );
        #endif
        return( 0 );
        }

    if( !window_init(info) )
        {
        window_end( info );
        #ifdef ERRORMSG
        MessageBox( 0, "window_init()!",tlt_error,MB_OK|MB_ICONEXCLAMATION );
        #endif
        return( 0 );
        }

    FONT_Init( (long)info->hDC );

    if( !intro_init( XRES, YRES, 1, (int)info->hWnd ) )
        {
        window_end( info );
        #ifdef ERRORMSG
        MessageBox( 0, msg_error, tlt_error, MB_OK|MB_ICONEXCLAMATION );
        #endif
        return( 0 );
        }


	if( !OUT_Init( XRES, YRES ) )
		{	
		#ifdef ERRORMSG
        MessageBox( 0, "OUT_Init()!",tlt_error,MB_OK|MB_ICONEXCLAMATION );
        #endif
		ExitProcess( 0 );
		}

    if( !RASTER_Do( doframe ) )
		{
        #ifdef ERRORMSG
        MessageBox( 0, "RASTER_Do()!",tlt_error,MB_OK|MB_ICONEXCLAMATION );
        #endif
		ExitProcess( 0 );
		}

/*
*/
    intro_end();

    window_end( info );

    smalloc_end();

    mdebug_close();

    return( 0 );
}
