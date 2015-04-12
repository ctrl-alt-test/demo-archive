//--------------------------------------------------------------------------//
// iq . 2003/2008 . code for 64 kb intros by RGBA                           //
//--------------------------------------------------------------------------//

// #define XRES         1600
// #define YRES         900

// #define XRES         1152
// #define YRES         648

// 16/10
// #define XRES         1280
// #define YRES         800

// 5/4
// #define XRES         1280
// #define YRES         1024

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
#include "../../intro.h"
#include "../msys.h"
#include "../events.h"

#include "../../font.hh"
#include "../../loading.hh"
#include "../../textures.hh"

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

static const char wndclass[] = "B - Incubation";
static const char msg_error[] = "This test seems to have failed.\n" \
				"No memory?\n" \
				"No music?\n" \
				"No OpenGL 2.0 capable driver?\n\n" \
				"Anyway, a test that fails is\n" \
				"the opportunity of improvement. ;)";

//----------------------------------------------------------------------------

// n = [0..LOADING_STEPS]
static void loadbar( void *data, int n )
{
    win_info_t *info = (win_info_t*)data;
    char str[255];

    Loading::clearRendering(0);
    Loading::draw(n);

    // FIXME : je ne comprends pas pourquoi
    // sans cet appel la suite est cassée
    glLoadIdentity();

    SwapBuffers( info->hDC );
    msys_timerSleep(10);

    /*
    // draw text
    const char loadingPrompt[] = "Test de chargement en mode DEBUG...";
    const unsigned int length = sizeof(loadingPrompt) / sizeof(char);
    const char promptFont[] = "arial";

    SetBkMode(info->hDC, TRANSPARENT);
    SetTextColor(info->hDC, 0x00ffffff);
    SelectObject(info->hDC, CreateFont(44,0,0,0,0,0,0,0,0,0,0,ANTIALIASED_QUALITY,0,promptFont));

    // FIXME : comment sait-il que ça faisait 318 son texte ?
    TextOut(info->hDC, (XRES-318)>>1, (YRES-38)>>1, loadingPrompt, length);
    */

    sprintf(str, "%s | Loading... (%d%%)", wndclass, n / 2);
    SetWindowText( info->hWnd, str );

    MSG msg;
    if( PeekMessage(&msg,0,0,0,PM_REMOVE) )
    {
      if (msg.message == WM_QUIT) ExitProcess(0);
      TranslateMessage( &msg );
      DispatchMessage( &msg );
    }
}

//----------------------------------------------------------------------------

static win_info_t     wininfo;

event_info_t *getEvents( void )
{
    return &wininfo.events;
}

int getKeyPress( int key )
{
    int res;

    res = wininfo.events.keyb.press[key];
    wininfo.events.keyb.press[key] = 0;
    return res;
}


static void procMouse( void )
{
    POINT   p;
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
}

//----------------------------------------------------------------------------

static LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    int i;

    // salvapantallas
    if( uMsg==WM_SYSCOMMAND && (wParam==SC_SCREENSAVE || wParam==SC_MONITORPOWER) )
      return( 0 );

    // boton x o pulsacion de escape
    if( uMsg==WM_CLOSE || uMsg==WM_DESTROY ||
	(uMsg==WM_KEYDOWN && wParam==VK_ESCAPE) )
    {
      PostQuitMessage(0);
      return( 0 );
    }

    if (uMsg==WM_KEYDOWN)
      intro_key(wParam,
		wininfo.events.keyb.state[KEY_LSHIFT] != 0,
		wininfo.events.keyb.state[KEY_LCONTROL] != 0);

//     // Reload debug data
//     if (uMsg==WM_KEYDOWN && wParam==KEY_R)
//     {
//     }

    if( uMsg==WM_CHAR )
    {
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
            case VK_RSHIFT:   conv = KEY_RSHIFT;   break;
            case VK_RCONTROL: conv = KEY_RCONTROL; break;
            case VK_LSHIFT:   conv = KEY_LSHIFT;   break;
            case VK_LCONTROL: conv = KEY_LCONTROL; break;
      }

      for( i=KEY_A; i<=KEY_Z; i++ )
      {
	if( wParam==(WPARAM)('A'+i-KEY_A) )
	  conv = i;
	if( wParam==(WPARAM)('a'+i-KEY_A) )
	  conv = i;
      }

      wininfo.events.keyb.press[conv] = 1;

      if( wParam==VK_ESCAPE )
      {
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
        return( 0 );

    if( info->full )
    {
        msys_memset( &dmScreenSettings,0,sizeof(DEVMODE) );
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

    info->hWnd = CreateWindowEx( dwExStyle, wc.lpszClassName, wc.lpszClassName, dwStyle,
                               (GetSystemMetrics(SM_CXSCREEN)-rec.right+rec.left)>>1,
                               (GetSystemMetrics(SM_CYSCREEN)-rec.bottom+rec.top)>>1,
                               rec.right-rec.left, rec.bottom-rec.top, 0, 0, info->hInstance, 0 );

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
      t1 = intro.youtubeNow / 1000.f;
      m1 = msys_ifloorf(t1 / 60.f);
      s1 = msys_ifloorf(t1 - 60.f * (float)m1);
      c1 = msys_ifloorf(t1 * 100.f) % 100;

      t2 = intro.storyNow / 1000.f;
      m2 = msys_ifloorf(t2 / 60.f);
      s2 = msys_ifloorf(t2 - 60.f * (float)m2);
      c2 = msys_ifloorf(t2 * 100.f) % 100;
      sprintf(str, "%s | [%d fps - %d ms/f] - Youtube time %02d'%02d\"%02d (t=%dms) - Story time %02d'%02d\"%02d (t'=%dms)",
	      wndclass, fps, it - last_it, m1, s1, c1, intro.youtubeNow, m2, s2, c2, intro.storyNow);
      SetWindowText( info->hWnd, str );
    }
    last_it = it;
}

void calcula( void );

int WINAPI WinMain( HINSTANCE instance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
    MSG         msg;
    int         done=0;

//    calcula(); return( 0 );
    if( !msys_debugInit() )
        return 0;

    wininfo.hInstance = GetModuleHandle( 0 );

    //if( MessageBox( 0, "fullscreen?", info->wndclass, MB_YESNO|MB_ICONQUESTION)==IDYES )
    //   info->full++;

    if( !window_init(&wininfo) )
    {
        window_end( &wininfo );
        MessageBox( 0, msg_error,0,MB_OK|MB_ICONEXCLAMATION );
        return( 0 );
    }

    if( !msys_init((intptr)wininfo.hWnd) )
    {
        window_end( &wininfo );
        MessageBox( 0, msg_error,0,MB_OK|MB_ICONEXCLAMATION );
        return( 0 );
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

    if( !intro_init( XRES, YRES, 1, font, &pd ) )
    {
        window_end( &wininfo );
        MessageBox( 0, msg_error, 0, MB_OK|MB_ICONEXCLAMATION );
        return( 0 );
    }

    // Post loading
    {
      long pl = timeGetTime();
      long plt = 0;
      while (plt < POST_LOAD_DURATION)
      {
	Loading::clearRendering(plt);
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

    // Intro
    while( !done )
    {
        if( PeekMessage(&msg,0,0,0,PM_REMOVE) )
        {
            if( msg.message==WM_QUIT )
	      done=1;
	    TranslateMessage( &msg );
            DispatchMessage( &msg );
        }
        else
        {
            procMouse();
	    intro_mouse(wininfo.events.mouse,
			wininfo.events.keyb.state[KEY_LSHIFT] != 0,
			wininfo.events.keyb.state[KEY_LCONTROL] != 0);
            done = intro_do();

            static long to = 0; if( !to ) to=timeGetTime();
			long it = timeGetTime() - to;
            float t = 0.001f * float(it);
            SwapBuffers( wininfo.hDC );

            DrawTime( &wininfo, t, it );
        }
    }

    intro_end();

    delete font;

    window_end( &wininfo );

    msys_debugEnd();

    return( 0 );
}
