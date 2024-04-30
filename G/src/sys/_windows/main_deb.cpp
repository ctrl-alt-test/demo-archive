//--------------------------------------------------------------------------//
// iq . 2003/2008 . code for 64 kb intros by RGBA                           //
//--------------------------------------------------------------------------//

// #define XRES         1600
// #define YRES         900

// #define XRES         1920
// #define YRES         1080

// #define XRES         1152
// #define YRES         648

// 16/9
// #define XRES         1440
// #define YRES         900

// 16/10
// #define XRES         1280
// #define YRES         800

// 5/4
// #define XRES         1280
// #define YRES         1024

// 16/9
#define XRES         1024
#define YRES         576

// #define XRES         800
// #define YRES         450

// #define XRES         400
// #define YRES         225

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#include <mmsystem.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <string.h>
#include <stdio.h>
#include "../../intro.hh"
#include "../msys.h"
#include "../events.h"

#include "../../font.hh"
#include "../../loading.hh"
#include "../../loadingsteps.hh"
#include "../../textures.hh"
#include "../../snd/sound.hh"

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
    event_info_t events;
} win_info_t;



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

static const char wndclass[] = DEMO_TITLE;
static const char* msg_errors[] = {
  "Could not create window.\nResolution problem maybe?",
  "Could not load OpenGL extensions.\nDo you have OpenGL 2.1 capable drivers?",
};
static const char * intro_msg_errors[] = {
  "Could not load soundtrack.",
  "Could not load midi.",
};

//----------------------------------------------------------------------------

win_info_t * loadbar_wininfo;
// n = [0..LOADING_STEPS]
namespace Loading {
  void clearScreen()
  {
    // Avant toute chose, un écran noir
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    SwapBuffers( loadbar_wininfo->hDC );
  }

  void update()
  {
    if (intro.initDone) return; // chargement fini

    win_info_t *info = loadbar_wininfo; // (win_info_t*)data;
    char str[255];

    int n = Loading::draw();

    SwapBuffers( info->hDC );
    msys_timerSleep(10);

    sprintf(str, "%s | Loading... (%d%%)", wndclass, (100 * n) / LOADING_STEPS);
    SetWindowText( info->hWnd, str );

    MSG msg;
    if( PeekMessage(&msg,0,0,0,PM_REMOVE) )
    {
      if (msg.message == WM_QUIT) ExitProcess(0);
      TranslateMessage( &msg );
      DispatchMessage( &msg );
    }
  }
}

//----------------------------------------------------------------------------

static win_info_t     wininfo;

event_info_t *getEvents( void )
{
    return &wininfo.events;
}

// int getKeyPress(KeyCode key )
// {
//     int res = wininfo.events.keyb.press[(int)key];
//     wininfo.events.keyb.press[(int)key] = 0;
//     return res;
// }


static void procMouse( bool mouseFocus )
{
    POINT   p;
    /*
    int     i;
    wininfo.events.keyb.state[KEY_LEFT]     = GetAsyncKeyState( VK_LEFT );
    wininfo.events.keyb.state[KEY_RIGHT]    = GetAsyncKeyState( VK_RIGHT );
    wininfo.events.keyb.state[KEY_UP]       = GetAsyncKeyState( VK_UP );
    wininfo.events.keyb.state[KEY_PGUP]     = GetAsyncKeyState( VK_PRIOR );
    wininfo.events.keyb.state[KEY_PGDOWN]   = GetAsyncKeyState( VK_NEXT );
    wininfo.events.keyb.state[KEY_DOWN]     = GetAsyncKeyState( VK_DOWN );
    wininfo.events.keyb.state[KEY_SPACE]    = GetAsyncKeyState( VK_SPACE );
    wininfo.events.keyb.state[KEY_RSHIFT]   = GetAsyncKeyState( VK_RSHIFT );
    wininfo.events.keyb.state[KEY_RCONTROL] = GetAsyncKeyState( VK_RCONTROL );
    wininfo.events.keyb.state[KEY_LSHIFT]   = GetAsyncKeyState( VK_LSHIFT );
    wininfo.events.keyb.state[KEY_LCONTROL] = GetAsyncKeyState( VK_LCONTROL );
    wininfo.events.keyb.state[KEY_1]        = GetAsyncKeyState( '1' );
    wininfo.events.keyb.state[KEY_2]        = GetAsyncKeyState( '2' );
    wininfo.events.keyb.state[KEY_3]        = GetAsyncKeyState( '3' );
    wininfo.events.keyb.state[KEY_4]        = GetAsyncKeyState( '4' );
    wininfo.events.keyb.state[KEY_5]        = GetAsyncKeyState( '5' );
    wininfo.events.keyb.state[KEY_6]        = GetAsyncKeyState( '6' );
    wininfo.events.keyb.state[KEY_7]        = GetAsyncKeyState( '7' );
    wininfo.events.keyb.state[KEY_8]        = GetAsyncKeyState( '8' );
    wininfo.events.keyb.state[KEY_9]        = GetAsyncKeyState( '9' );
    wininfo.events.keyb.state[KEY_0]        = GetAsyncKeyState( '0' );
    for( i=KEY_A; i<=KEY_Z; i++ )
      wininfo.events.keyb.state[i] = GetAsyncKeyState( 'A'+i-KEY_A );
    */

    //-------
    GetCursorPos( &p );

    wininfo.events.mouse.ox = wininfo.events.mouse.x;
    wininfo.events.mouse.oy = wininfo.events.mouse.y;
    wininfo.events.mouse.x = p.x;
    wininfo.events.mouse.y = p.y;
    wininfo.events.mouse.dx =  wininfo.events.mouse.x - wininfo.events.mouse.ox;
    wininfo.events.mouse.dy =  wininfo.events.mouse.y - wininfo.events.mouse.oy;

    wininfo.events.mouse.obuttons[0] = wininfo.events.mouse.buttons[0];
    wininfo.events.mouse.obuttons[1] = wininfo.events.mouse.buttons[1];
    wininfo.events.mouse.buttons[0] = GetAsyncKeyState(VK_LBUTTON);
    wininfo.events.mouse.buttons[1] = GetAsyncKeyState(VK_RBUTTON);

    wininfo.events.mouse.dbuttons[0] = wininfo.events.mouse.buttons[0] - wininfo.events.mouse.obuttons[0];
    wininfo.events.mouse.dbuttons[1] = wininfo.events.mouse.buttons[1] - wininfo.events.mouse.obuttons[1];

    if (!mouseFocus)
    {
      wininfo.events.mouse.dx = 0;
      wininfo.events.mouse.dy = 0;
      wininfo.events.mouse.dbuttons[0] = 0;
      wininfo.events.mouse.dbuttons[1] = 0;
    }
}

static KeyCode getKeyCode(WPARAM wParam)
{
  if (wParam >= VK_F1 && wParam <= VK_F12)
    return (KeyCode)(KEY_F1 + (int)(wParam - VK_F1));

  if (wParam >= '0' && wParam <= '9')
    return (KeyCode)(KEY_0 + (int)(wParam - (WPARAM)'0'));

  if (wParam >= 'A' && wParam <= 'Z')
    return (KeyCode)(KEY_A + (int)(wParam - (WPARAM)'A'));

  if (wParam >= 'a' && wParam <= 'z')
    return (KeyCode)(KEY_A + (int)(wParam - (WPARAM)'a'));

  switch (wParam)
  {
  case VK_LEFT:     return KEY_ARROW_LEFT;
  case VK_RIGHT:    return KEY_ARROW_RIGHT;
  case VK_UP:       return KEY_ARROW_UP;
  case VK_DOWN:     return KEY_ARROW_DOWN;
  case VK_PRIOR:    return KEY_PGUP;
  case VK_NEXT:     return KEY_PGDOWN;

  case VK_LSHIFT:   return KEY_LSHIFT;
  case VK_RSHIFT:   return KEY_RSHIFT;
  case VK_LCONTROL: return KEY_LCONTROL;
  case VK_RCONTROL: return KEY_RCONTROL;
  case VK_SPACE:    return KEY_SPACE;
  case VK_RETURN:   return KEY_RETURN;
  }

  if (wParam == 16) return KEY_LSHIFT; // FIXME
  if (wParam == 17) return KEY_LCONTROL; // FIXME

  //assert(false);
  return NUMBER_OF_KEYS;
}

//----------------------------------------------------------------------------

static LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
//     int i;

    // salvapantallas
    if( uMsg==WM_SYSCOMMAND && (wParam==SC_SCREENSAVE || wParam==SC_MONITORPOWER) )
    {
      DBG("Screensaver!");
      return( 0 );
    }

    // boton x o pulsacion de escape
    if (uMsg==WM_CLOSE || uMsg==WM_DESTROY ||
	(uMsg==WM_KEYDOWN && wParam==VK_ESCAPE))
    {
      if (uMsg==WM_CLOSE)
	DBG("Received close signal!");
      if (uMsg==WM_DESTROY)
	DBG("Received destroy signal!");
      if (uMsg==WM_KEYDOWN)
	DBG("User pressed Escape");
      PostQuitMessage(0);
      return( 0 );
    }

    if (uMsg==WM_KEYUP)
      wininfo.events.keyb.state[getKeyCode(wParam)] = false;

    if (uMsg==WM_KEYDOWN) {
      wininfo.events.keyb.state[getKeyCode(wParam)] = true;
      intro_key(getKeyCode(wParam),
		wininfo.events.keyb.state[KEY_LSHIFT] != 0,
		wininfo.events.keyb.state[KEY_LCONTROL] != 0);
    }

    if( uMsg==WM_CHAR )
    {
      /*
      int conv = 0;
      switch( wParam )
      {
            case VK_LEFT:     conv = KEY_LEFT;     break;
            case VK_RIGHT:    conv = KEY_RIGHT;    break;
            case VK_UP:       conv = KEY_UP;       break;
            case VK_PRIOR:    conv = KEY_PGUP;     break;
            case VK_NEXT:     conv = KEY_PGDOWN;   break;
            case VK_DOWN:     conv = KEY_DOWN;     break;
            case VK_SPACE:    conv = KEY_SPACE;    break;
            case VK_RETURN:   conv = KEY_RETURN;   break;
            case VK_RSHIFT:   conv = KEY_RSHIFT;   break;
            case VK_RCONTROL: conv = KEY_RCONTROL; break;
            case VK_LSHIFT:   conv = KEY_LSHIFT;   break;
            case VK_LCONTROL: conv = KEY_LCONTROL; break;
      }
      if (wParam >= VK_F1 && wParam <= VK_F12)
      {
	  conv = wParam + KEY_F1 - VK_F1;
      }

      for( i=KEY_A; i<=KEY_Z; i++ )
      {
	if( wParam==(WPARAM)('A'+i-KEY_A) )
	  conv = i;
	if( wParam==(WPARAM)('a'+i-KEY_A) )
	  conv = i;
      }

      wininfo.events.keyb.press[conv] = 1;
      */

      if( uMsg==WM_MOUSEWHEEL)
      {
	wininfo.events.mouse.dz = GET_WHEEL_DELTA_WPARAM(wParam);
      }

      if( wParam==VK_ESCAPE )
      {
	DBG("User pressed Escape");
	PostQuitMessage(0);
	return( 0 );
      }
    }

    return( DefWindowProc(hWnd,uMsg,wParam,lParam) );
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

    if( info->full )
    {
        ChangeDisplaySettings( 0, 0 );
		while( ShowCursor( 1 )<0 ); // show cursor
    }
}

static int window_init( win_info_t *info )
{
    unsigned int	PixelFormat;
    DWORD		dwExStyle, dwStyle;
    DEVMODE		dmScreenSettings;
    WNDCLASS		wc;
    RECT		rec;

    msys_memset( &wc, 0, sizeof(WNDCLASS) );
    wc.style         = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = info->hInstance;
    wc.lpszClassName = wndclass;
    //wc.hbrBackground=(HBRUSH)CreateSolidBrush(0x00785838);

    if( !RegisterClass(&wc) )
    {
      DBG("Could not register class");
      return( 0 );
    }

    if( info->full )
    {
        msys_memset( &dmScreenSettings,0,sizeof(DEVMODE) );
        dmScreenSettings.dmSize       = sizeof(DEVMODE);
        dmScreenSettings.dmFields     = DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;
        dmScreenSettings.dmBitsPerPel = 32;
        dmScreenSettings.dmPelsWidth  = XRES;
        dmScreenSettings.dmPelsHeight = YRES;

        if( ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
	{
	  DBG("Could not set display");
	  return( 0 );
	}

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

    info->hWnd = CreateWindowEx( dwExStyle, wc.lpszClassName, wc.lpszClassName, dwStyle,
                               (GetSystemMetrics(SM_CXSCREEN)-rec.right+rec.left)>>1,
                               (GetSystemMetrics(SM_CYSCREEN)-rec.bottom+rec.top)>>1,
                               rec.right-rec.left, rec.bottom-rec.top, 0, 0, info->hInstance, 0 );

    if( !info->hWnd )
    {
      DBG("No window handle!");
      return( 0 );
    }

    if( !(info->hDC=GetDC(info->hWnd)) )
    {
      DBG("No DC!");
      return( 0 );
    }

    if( !(PixelFormat=ChoosePixelFormat(info->hDC,&pfd)) )
    {
      DBG("Could not find a resolution!");
      return( 0 );
    }

    if( !SetPixelFormat(info->hDC,PixelFormat,&pfd) )
    {
      DBG("Could not set resolution!");
      return( 0 );
    }

    if( !(info->hRC=wglCreateContext(info->hDC)) )
    {
      DBG("Could not create GL context!");
      return( 0 );
    }

    if( !wglMakeCurrent(info->hDC,info->hRC) )
    {
      DBG("Could not select GL context!");
      return( 0 );
    }

    //SetForegroundWindow( info->hWnd );    // slightly higher priority
    //SetFocus( info->hWnd );               // sets keyboard focus to the window

    return( 1 );
}


//----------------------------------------------------------------------------

static void DrawTime( win_info_t *info, float t, long it )
{
    static int      frame=0;
    static float    to=0.0;
    static int      fps=0;
    static long     last_it = 0;
    char            str[255];
    float           t1, t2;
    int             m1, s1, c1;
    int             m2, s2, c2;

    if( t<0.0f) return;
    if( info->full ) return;

    frame++;
    if( (t-to)>1.0f )
    {
        fps = frame;
        to = t;
        frame = 0;
    }

    if( !(frame&3) )
    {
      t1 = intro.now.youtube / 1000.f;
      m1 = msys_ifloorf(t1 / 60.f);
      s1 = msys_ifloorf(t1 - 60.f * (float)m1);
      c1 = msys_ifloorf(t1 * 100.f) % 100;

      t2 = intro.now.story / 1000.f;
      m2 = msys_ifloorf(t2 / 60.f);
      s2 = msys_ifloorf(t2 - 60.f * (float)m2);
      c2 = msys_ifloorf(t2 * 100.f) % 100;

      const char * shaderStatus = NULL;
      switch (intro.debug.shaderStatus)
      {
      case Shader::ok:		shaderStatus = ""; break;
      case Shader::notReady:	shaderStatus = "Shaders not ready |"; break;
      case Shader::linkWarn:	shaderStatus = "Shader link WARNING |"; break;
      case Shader::compilWarn:	shaderStatus = "Shader WARNING |"; break;
      case Shader::linkError:	shaderStatus = "Shader link ERROR |"; break;
      case Shader::compilError:	shaderStatus = "Shader ERROR |"; break;
      }
      sprintf(str, "%s | %s [%d fps - %d ms/f] - Youtube time %02d'%02d\"%02d (t=%dms) - Story time %02d'%02d\"%02d (t'=%dms)%s",
	      wndclass,
	      shaderStatus,
	      fps, it - last_it, m1, s1, c1, intro.now.youtube, m2, s2, c2, intro.now.story, (intro.now.paused? " [PAUSED]" : ""));
      SetWindowText( info->hWnd, str );
    }
    last_it = it;
}

void calcula( void );

int WINAPI WinMain( HINSTANCE instance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
    MSG         msg;
    int         done=0;

    long startTime = timeGetTime();

//    calcula(); return( 0 );
    if( !msys_debugInit() )
        return 0;

    wininfo.hInstance = GetModuleHandle( 0 );
    //wininfo.full = 1;

    //if( MessageBox( 0, "fullscreen?", info->wndclass, MB_YESNO|MB_ICONQUESTION)==IDYES )
    //   info->full++;

    if( !window_init(&wininfo) )
    {
      DBG("Could not create window!");
      window_end( &wininfo );
      MessageBox( 0, msg_errors[0],0,MB_OK|MB_ICONEXCLAMATION );
      return 1;
    }
    DBG("Window created");

    if (msys_init((intptr)wininfo.hWnd) != NULL)
    {
      DBG("Could not initialize framework!");
      window_end( &wininfo );
      MessageBox( 0, msg_errors[1],0,MB_OK|MB_ICONEXCLAMATION );
      return 2;
    }
    DBG("Framework initialized");

    // Loading
    loadbar_wininfo = &wininfo;

    // Font
    Texture::createTextureList();
    int min = YRES;
    if (XRES < min)
      min = XRES;
    Font::Font * font = Font::Font::create(wininfo.hDC,
					   "Verdana", min / 8, FW_MEDIUM,
					   Texture::font);
    Font::Sticker * sticker = new Font::Sticker(wininfo.hDC, "Impact", 100, FW_MEDIUM);

    const int initResult = Intro::init(XRES, YRES, float(XRES) / float(YRES), true, font, sticker);
    if (initResult != 0)
    {
	DBG("Could not initialize intro!");
        window_end( &wininfo );
        MessageBox( 0, intro_msg_errors[initResult - 1],0,MB_OK|MB_ICONEXCLAMATION );
        return 3;
    }
    DBG("Intro loaded: %d ms", int(timeGetTime() - startTime));

    // Post loading
    {
      long pl = timeGetTime();
      long plt = 0;
      while (plt < POST_LOAD_DURATION)
      {
	Loading::drawPostLoaded(plt);
	SwapBuffers( wininfo.hDC );
	plt = timeGetTime() - pl;

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

    DBG("Loading animation finished: %d ms", int(timeGetTime() - startTime));

    // First frame (flush pipeline, cache, etc.)
    Intro::initSynchronization();
    Intro::mainLoop();
    SwapBuffers( wininfo.hDC );

#if SOUND
    Sound::init();
    Sound::play();
#else
    Sound::stop();
#endif // SOUND
    Intro::initSynchronization();

    DBG("Music started: %d ms", int(timeGetTime() - startTime));

    bool mouseFocus = false;

    // Intro
    while( !done )
    {
        if( PeekMessage(&msg,0,0,0,PM_REMOVE) )
        {
            if( msg.message==WM_QUIT )
	      done=1;
	    if( msg.message==WM_MOUSEMOVE )
	      mouseFocus = true;
	    TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
        {
	    const long startFrame = timeGetTime();
            procMouse(mouseFocus);
	    mouseFocus = false;
	    intro_mouse(wininfo.events.mouse,
			wininfo.events.keyb.state[KEY_LSHIFT] != 0,
			wininfo.events.keyb.state[KEY_LCONTROL] != 0);

	    static bool first = true;
	    if (first)
	    {
	      DBG("First frame start: %d ms", int(timeGetTime() - startTime));
	    }

            done = Intro::mainLoop();

	    if (first)
	    {
	      DBG("First frame end: %d ms", int(timeGetTime() - startTime));
	      first = false;
	    }

	    static long to = 0; if( !to ) to=timeGetTime();
	    long it = timeGetTime() - to;
            float t = 0.001f * float(it);
            SwapBuffers( wininfo.hDC );

            DrawTime( &wininfo, t, it );
	    const long duration = timeGetTime() - startFrame;
	    if (duration < 16) // 60 fps
	    {
	      msys_timerSleep(16 - duration);
	    }
        }
    }

    Intro::end();

    delete font;

    window_end( &wininfo );

    DBG("Intro finished");

    msys_debugEnd();

    return 0;
}
