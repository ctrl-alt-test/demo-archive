//--------------------------------------------------------------------------//
// iq . 2003/2008 . code for 64 kb intros by RGBA                           //
//--------------------------------------------------------------------------//

// #define XRES         1600
// #define YRES         900

// #define XRES         1152
// #define YRES         648

//#define XRES         1024
//#define YRES         768 // 576
int XRES = 0;
int YRES = 0;

// #define XRES         800
// #define YRES         450

// #define XRES         400
// #define YRES         225

#ifdef A64BITS
#pragma pack(8) // VERY important, so WNDCLASS get's the correct padding and we don't crash the system
#endif

//#pragma check_stack(off)
//#pragma runtime_checks("", off)


#define ALLOWWINDOWED       // allow windowed mode

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#include <string.h>
#include "../../intro.h"
#include "../msys.h"
#include "../events.h"

#include "../../dialog.hh"
#include "../../font.hh"
#include "../../loading.hh"
#include "../../textures.hh"

//----------------------------------------------------------------------------

typedef struct
{
    HINSTANCE   hInstance;
    HWND        hWnd;
    HDC         hDC;
    HGLRC       hRC;
    int         full;
} win_info_t;

extern "C" int _fltused = 0;

static const PIXELFORMATDESCRIPTOR pfd =
{
    sizeof(PIXELFORMATDESCRIPTOR),
    1,
    PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER,
    PFD_TYPE_RGBA,
    32,
    8, 0, 8, 0, 8, 0, 8, 0,
    0, 0, 0, 0, 0,
    32,             // zbuffer
    0,              // stencil!
    0,
    PFD_MAIN_PLANE,
    0, 0, 0, 0
};

static DEVMODE screenSettings = { {0},
    #if _MSC_VER < 1400
    0,0,148,0,0x001c0000,{0},0,0,0,0,0,0,0,0,0,{0},0,32,XRES,YRES,0,0,      // Visual C++ 6.0
    #else
    0,0,156,0,0x001c0000,{0},0,0,0,0,0,{0},0,32,XRES,YRES,{0}, 0,           // Visuatl Studio 2005
    #endif
    #if(WINVER >= 0x0400)
    0,0,0,0,0,0,
    #if (WINVER >= 0x0500) || (_WIN32_WINNT >= 0x0400)
    0,0
    #endif
    #endif
    };

static const char wndclass[] = "B - Incubation";
static const char msg_error[] = "This test seems to have failed.\n" \
				"No memory?\n" \
				"No music?\n" \
				"No OpenGL 2.0 capable driver?\n\n" \
				"Anyway, a test that fails is\n" \
				"the opportunity of improvement. ;)";


//----------------------------------------------------------------------------

static int percent(char * buffer, int n)
{
  int l = 0;
  if (n > 9)
  {
    l = percent(buffer, n / 10);
  }
  buffer[l] = '0' + (n % 10);
  return l + 1;
}

// n = [0..LOADING_STEPS]
static void loadbar( void *data, int n )
{
    win_info_t *info = (win_info_t*)data;

    Loading::clearRendering(0);
    Loading::draw(n);

    // FIXME : je ne comprends pas pourquoi
    // sans cet appel la suite est cassée
    glLoadIdentity();

    SwapBuffers( info->hDC );
    msys_timerSleep(10);

    // draw text
    {
      const int baseTitleLength = sizeof(wndclass) / sizeof(char) - 1;
      char buffer[baseTitleLength + 8];
      for (unsigned int i = 0; i < baseTitleLength; ++i)
      {
	buffer[i] = wndclass[i];
      }
      buffer[baseTitleLength] = '\0';
      if (n < 200)
      {
	buffer[baseTitleLength] = ' ';
	buffer[baseTitleLength + 1] = '(';
	const int length = percent(buffer + baseTitleLength + 2, n / 2);
	buffer[baseTitleLength + 2 + length] = '%';
	buffer[baseTitleLength + 3 + length] = ')';
	buffer[baseTitleLength + 4 + length] = '\0';
      }

      SetWindowText(info->hWnd, buffer );
      /*
      SetBkMode( info->hDC, TRANSPARENT );
      SetTextColor( info->hDC, 0xff000000 );
      SelectObject( info->hDC, CreateFont( 44,0,0,0,0,0,0,0,0,0,0,ANTIALIASED_QUALITY,0,"arial") );
      TextOut( info->hDC, (XRES-318)>>1, (YRES-38)>>1, buffer, length );
      */
    }

    MSG msg;
    if( PeekMessage(&msg,0,0,0,PM_REMOVE) )
    {
      if (msg.message == WM_QUIT) ExitProcess(0);
      TranslateMessage( &msg );
      DispatchMessage( &msg );
    }
}

//----------------------------------------------------------------------------

static LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( uMsg==WM_SYSCOMMAND && (wParam==SC_SCREENSAVE || wParam==SC_MONITORPOWER) )
		return 0 ;

	if( uMsg==WM_CLOSE || (uMsg==WM_KEYDOWN && wParam==VK_ESCAPE) )
	{
		PostQuitMessage(0);
        return 0 ;
	}

    return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

static void window_end( win_info_t *info )
{
    if( info->hRC )
    {
        wglMakeCurrent( 0, 0 );
        wglDeleteContext( info->hRC );
    }

    if( info->hDC  ) ReleaseDC( info->hWnd, info->hDC );
    if( info->hWnd ) DestroyWindow( info->hWnd );

    UnregisterClass( wndclass, info->hInstance );

    #ifdef ALLOWWINDOWED
    if( info->full )
    #endif
    {
        ChangeDisplaySettings( 0, 0 );
		ShowCursor( 1 ); 
    }
}

static int window_init( win_info_t *info )
{
	unsigned int	PixelFormat;
    DWORD			dwExStyle, dwStyle;
    RECT			rec;
    WNDCLASSA		wc;

    info->hInstance = GetModuleHandle( 0 );

    msys_memset( &wc, 0, sizeof(WNDCLASSA) );

    wc.style         = CS_OWNDC;
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = info->hInstance;
    wc.lpszClassName = wndclass;

    if( !RegisterClass((WNDCLASSA*)&wc) )
        return( 0 );

    #ifdef ALLOWWINDOWED
    if( info->full )
    #endif
    {
        if( ChangeDisplaySettings(&screenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
            return( 0 );
        dwExStyle = WS_EX_APPWINDOW;
        dwStyle   = WS_VISIBLE | WS_POPUP | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		ShowCursor( 0 );
    }
    #ifdef ALLOWWINDOWED
    else
    {
        dwExStyle = WS_EX_APPWINDOW;// | WS_EX_WINDOWEDGE;
        dwStyle   = WS_VISIBLE | WS_CAPTION | WS_CLIPSIBLINGS | WS_CLIPCHILDREN | WS_SYSMENU;
    }
    #endif
    rec.left   = 0;
    rec.top    = 0;
    rec.right  = XRES;
    rec.bottom = YRES;

    #ifdef ALLOWWINDOWED
    AdjustWindowRect( &rec, dwStyle, 0 );
    info->hWnd = CreateWindowEx( dwExStyle, wc.lpszClassName, wc.lpszClassName, dwStyle,
                               (GetSystemMetrics(SM_CXSCREEN)-rec.right+rec.left)>>1,
                               (GetSystemMetrics(SM_CYSCREEN)-rec.bottom+rec.top)>>1,
                               rec.right-rec.left, rec.bottom-rec.top, 0, 0, info->hInstance, 0 );
    #else
    info->hWnd = CreateWindowEx( dwExStyle, wc.lpszClassName, wc.lpszClassName, dwStyle, 0, 0, 
                                 rec.right-rec.left, rec.bottom-rec.top, 0, 0, info->hInstance, 0 );
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
    
    SetForegroundWindow(info->hWnd);
    SetFocus(info->hWnd);

    return( 1 );
}


//----------------------------------------------------------------------------
#if 0
extern "C" extern int __cdecl _heap_init (int);
extern "C" extern int __cdecl _mtinit ( void );
extern "C" _CRTIMP int __cdecl _CrtSetCheckCount(int);
extern "C" extern int __cdecl _ioinit (void);
extern "C" extern int __cdecl _cinit (int);



/*
extern "C" extern int _heap_init(int);
extern "C" extern void _ioinit(void);
extern "C" extern void _cinit(void);

extern "C" extern void _mtinit(void);
*/

#include <rtcapi.h>
extern "C" extern void _RTC_Initialize(void);


int __cdecl MyErrorFunc(int, const wchar_t *, int, const wchar_t *, const wchar_t *, ...)
{
MessageBox(0,"q",0,0);
    return 0;
}


/*
// C version:
_RTC_error_fnW __cdecl _CRT_RTC_INITW(void *res0, void **res1, int res2, int res3, int res4)
{
    return &MyErrorFunc; 
}
*/

// C++ version:
extern "C" _RTC_error_fnW __cdecl _CRT_RTC_INITW(void *res0, void **res1, int res2, int res3, int res4)
{
    return &MyErrorFunc;
}

#include <winbase.h>

 // RunTmChk.lib
#endif

void entrypoint( void )
{
//void __security_init_cookie(void);
//__security_init_cookie();

// call C/C++ initializers
//_initterm(__xi_a, __xi_z);
//_initterm(__xc_a, __xc_z);
//HeapCreate( 0, 4096, 0 );
//_RTC_Initialize();
//_heap_init(1);
//_mtinit();
//_CrtSetCheckCount(TRUE);
//_RTC_Initialize();
//_ioinit();
//_cinit(TRUE);

    win_info_t     wininfo;
    MSG         msg;
    int         done=0;

    Dialog& dlg = Dialog::instance();
    dlg.run();
    if (!dlg.test())
      ExitProcess(0);
    wininfo.full = dlg.fullscreen();
    XRES = dlg.width();
    YRES = dlg.height();
    screenSettings.dmPelsWidth = XRES;
    screenSettings.dmPelsHeight = YRES;

    //#ifdef ALLOWWINDOWED
    //wininfo.full = ( MessageBox( 0, "fullscreen?", wndclass, MB_YESNO|MB_ICONQUESTION)==IDYES );
    //#endif

    if( !window_init(&wininfo) )
    {
        window_end( &wininfo );
        MessageBox( 0,msg_error,0,MB_OK|MB_ICONEXCLAMATION );
        ExitProcess( 0 );
    }

    if( !msys_init((intptr)wininfo.hWnd) )
    {
        window_end( &wininfo );
        MessageBox( 0,msg_error,0,MB_OK|MB_ICONEXCLAMATION );
        ExitProcess( 0 );
    }

    // Loading
    Loading::ProgressDelegate pd = { &wininfo, loadbar };

    // Font
    Texture::createTextureList();
    int min = YRES;
    if (XRES < min)
      min = XRES;
    Font * font = Font::create(wininfo.hDC, XRES, YRES,
			       "Georgia", min / 8, FW_MEDIUM,
			       Texture::font);

    if( !intro_init( XRES, YRES, dlg.sound(), font, &pd ) )
    {
        window_end( &wininfo );
        MessageBox( 0,msg_error,0,MB_OK|MB_ICONEXCLAMATION );
        ExitProcess( 0 );
    }

    // Post loading
    {
      long pl = msys_timerGet();
      long plt = 0;
      while (plt < POST_LOAD_DURATION)
      {
	DBG("%d", int(plt));
	Loading::clearRendering(plt);
	Loading::drawPostLoaded(plt);
	SwapBuffers( wininfo.hDC );
	plt = msys_timerGet() - pl;

	MSG msg;
	if( PeekMessage(&msg,0,0,0,PM_REMOVE) )
	{
	  if( msg.message==WM_QUIT )
	  {
	    done=1;
	    break;
	  }
	  TranslateMessage( &msg );
	  DispatchMessage( &msg );
	}
      }
    }

    // Intro
    while( !done )
    {
        while( PeekMessage(&msg,0,0,0,PM_REMOVE) )
        {
            if( msg.message==WM_QUIT ) { done=1; break; }
            DispatchMessage( &msg );
        }
        done |= intro_do();
        SwapBuffers( wininfo.hDC );
    }

    intro_end();

    window_end( &wininfo );
    ExitProcess( 0 );
}
