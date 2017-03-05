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
#include "../../intro.hh"
#include "../msys.h"
#include "wglext.h"
#include "../events.h"

#include "../../dialog.hh"
#include "../../font.hh"
#include "../../loading.hh"
#include "../../loadingsteps.hh"
#include "../../textures.hh"
#include "../../../snd/sound.h"

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

static const char wndclass[] = DEMO_TITLE;
static const char* msg_error[2] = {
  "Could not create window.\nResolution problem maybe?",
  "Could not load OpenGL extensions.\nDo you have OpenGL 2.1 capable drivers?",
};

//----------------------------------------------------------------------------

win_info_t s_wininfo;

static LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
  if( uMsg==WM_SYSCOMMAND &&
      (wParam==SC_SCREENSAVE || wParam==SC_MONITORPOWER) )
    return 0;

  if( uMsg==WM_CLOSE || (uMsg==WM_KEYDOWN && wParam==VK_ESCAPE) )
  {
    PostQuitMessage(0);
    return 0;
  }

  return DefWindowProc(hWnd,uMsg,wParam,lParam);
}

static void window_end()
{
  if( s_wininfo.hRC )
  {
    wglMakeCurrent( 0, 0 );
    wglDeleteContext( s_wininfo.hRC );
  }

  if( s_wininfo.hDC  ) ReleaseDC( s_wininfo.hWnd, s_wininfo.hDC );
  if( s_wininfo.hWnd ) DestroyWindow( s_wininfo.hWnd );

  UnregisterClass( wndclass, s_wininfo.hInstance );

#ifdef ALLOWWINDOWED
  if( s_wininfo.full )
#endif
  {
    ChangeDisplaySettings( 0, 0 );
    ShowCursor( 1 );
  }
}

static void processMessages()
{
  MSG msg;
  while (PeekMessage(&msg,0,0,0,PM_REMOVE))
  {
    if (msg.message == WM_QUIT)
    {
      // Kill
      intro_end();
      window_end();
      ExitProcess(0);
    }
    TranslateMessage( &msg );
    DispatchMessage( &msg );
  }
}

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
namespace Loading {
  void clearScreen()
  {
    // Avant toute chose, un écran noir
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    SwapBuffers( s_wininfo.hDC );
  }

  void update()
  {
    int n = Loading::draw();

    SwapBuffers( s_wininfo.hDC );
    msys_timerSleep(10);

    // Window title
    {
      const int baseTitleLength = sizeof(wndclass) / sizeof(char) - 1;
      char buffer[baseTitleLength + 8];
      for (unsigned int i = 0; i < baseTitleLength; ++i)
      {
	buffer[i] = wndclass[i];
      }
      buffer[baseTitleLength] = '\0';
      if (n <= LOADING_STEPS)
      {
	buffer[baseTitleLength] = ' ';
	buffer[baseTitleLength + 1] = '(';
	const int length = percent(buffer + baseTitleLength + 2, (100 * n) / LOADING_STEPS);
	buffer[baseTitleLength + 2 + length] = '%';
	buffer[baseTitleLength + 3 + length] = ')';
	buffer[baseTitleLength + 4 + length] = '\0';
      }

      SetWindowText(s_wininfo.hWnd, buffer );
      /*
      SetBkMode( s_wininfo.hDC, TRANSPARENT );
      SetTextColor( s_wininfo.hDC, 0xff000000 );
      SelectObject( s_wininfo.hDC, CreateFont( 44,0,0,0,0,0,0,0,0,0,0,ANTIALIASED_QUALITY,0,"arial") );
      TextOut( s_wininfo.hDC, (XRES-318)>>1, (YRES-38)>>1, buffer, length );
      */
    }

    processMessages();
  }
}

//----------------------------------------------------------------------------

static int window_init()
{
  unsigned int	PixelFormat;
  DWORD		dwExStyle, dwStyle;
  RECT		rec;
  WNDCLASSA	wc;

  s_wininfo.hInstance = GetModuleHandle( 0 );

  msys_memset( &wc, 0, sizeof(WNDCLASSA) );

  wc.style         = CS_OWNDC;
  wc.lpfnWndProc   = WndProc;
  wc.hInstance     = s_wininfo.hInstance;
  wc.lpszClassName = wndclass;

  if( !RegisterClass((WNDCLASSA*)&wc) )
    return( 0 );

#ifdef ALLOWWINDOWED
  if( s_wininfo.full )
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
  s_wininfo.hWnd = CreateWindowEx( dwExStyle, wc.lpszClassName, wc.lpszClassName, dwStyle,
				   (GetSystemMetrics(SM_CXSCREEN)-rec.right+rec.left)>>1,
				   (GetSystemMetrics(SM_CYSCREEN)-rec.bottom+rec.top)>>1,
				   rec.right-rec.left, rec.bottom-rec.top, 0, 0, s_wininfo.hInstance, 0 );
#else
  s_wininfo.hWnd = CreateWindowEx( dwExStyle, wc.lpszClassName, wc.lpszClassName, dwStyle, 0, 0,
				   rec.right-rec.left, rec.bottom-rec.top, 0, 0, s_wininfo.hInstance, 0 );
#endif

  // Synchro verticale, si disponible
  PFNWGLSWAPINTERVALEXTPROC wglSwapInterval = (PFNWGLSWAPINTERVALEXTPROC)wglGetProcAddress("wglSwapIntervalEXT");
  if (wglSwapInterval)
  {
    wglSwapInterval(1);
  }

  if( !s_wininfo.hWnd )
    return( 0 );

  if( !(s_wininfo.hDC=GetDC(s_wininfo.hWnd)) )
    return( 0 );

  if( !(PixelFormat=ChoosePixelFormat(s_wininfo.hDC,&pfd)) )
    return( 0 );

  if( !SetPixelFormat(s_wininfo.hDC,PixelFormat,&pfd) )
    return( 0 );

  if( !(s_wininfo.hRC=wglCreateContext(s_wininfo.hDC)) )
    return( 0 );

  if( !wglMakeCurrent(s_wininfo.hDC,s_wininfo.hRC) )
    return( 0 );

  SetForegroundWindow(s_wininfo.hWnd);
  SetFocus(s_wininfo.hWnd);

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

    int         done=0;

    Dialog& dlg = Dialog::instance();
    dlg.run();
    if (!dlg.test())
      ExitProcess(0);
    s_wininfo.full = dlg.fullscreen();
    XRES = dlg.width();
    YRES = dlg.height();
    screenSettings.dmPelsWidth = XRES;
    screenSettings.dmPelsHeight = YRES;


    if( !window_init() )
    {
        window_end();
        MessageBox( 0,msg_error[0],0,MB_OK|MB_ICONEXCLAMATION );
        ExitProcess( 0 );
    }

    if( !msys_init((intptr)s_wininfo.hWnd) )
    {
        window_end();
        MessageBox( 0,msg_error[1],0,MB_OK|MB_ICONEXCLAMATION );
        ExitProcess( 0 );
    }

    // Font
    Texture::createTextureList();
    int min = YRES;
    if (XRES < min)
      min = XRES;
    Font::Font * font = Font::Font::create(s_wininfo.hDC,
					   "Verdana", min / 8, FW_MEDIUM,
					   Texture::font);
    Font::Sticker * sticker = new Font::Sticker(s_wininfo.hDC, "Impact", 100, FW_MEDIUM);

    intro_init(XRES, YRES, dlg.ratio(), true, font, sticker);

    // Post loading
    {
      long pl = msys_timerGet();
      long plt = 0;
      while (plt < POST_LOAD_DURATION)
      {
	DBG("%d", int(plt));
	Loading::drawPostLoaded(plt);
	SwapBuffers( s_wininfo.hDC );
	plt = msys_timerGet() - pl;

	processMessages();
      }
    }

    SetWindowText(s_wininfo.hWnd, wndclass );

    // First frame (flush pipeline, cache, etc.)
    intro_init_synchronization();
    intro_do();
    SwapBuffers( s_wininfo.hDC );

#if SOUND
    Sound::play();
#endif // SOUND
    intro_init_synchronization();

    // Intro
    while( !done )
    {
      processMessages();
      done |= intro_do();
      SwapBuffers( s_wininfo.hDC );
    }

    intro_end();
    window_end();
    ExitProcess( 0 );
}
