//--------------------------------------------------------------------------//
// iq / rgba  .  tiny codes  .  2008                                        //
//--------------------------------------------------------------------------//

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#include <mmsystem.h>
#include <GL/gl.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include "../config.h"
#include "../intro.h"

#define IQ_MZK 0
#define FKLANG_MZK 1

#if IQ_MZK
#include "../mzk.h"
#endif
#if FKLANG_MZK
#include "mmsystem.h"
#include "mmreg.h"
#include "../../../snd/cj-lovepotionnumber4-4klang.h"
#endif

#define SOUND 1


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
    char        wndclass[4];	// window class and title :)
    //---------------
}WININFO;



static const PIXELFORMATDESCRIPTOR pfd =
    {
    sizeof(PIXELFORMATDESCRIPTOR),
    1,
    PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER,
    PFD_TYPE_RGBA,
    32,
    0, 0, 0, 0, 0, 0, 8, 0,
    0, 0, 0, 0, 0,  // accum
    32,             // zbuffer
    0,              // stencil!
    0,              // aux
    PFD_MAIN_PLANE,
    0, 0, 0, 0
    };

static WININFO wininfo = {  0,0,0,0,0,
							{'i','q','_',0}
                            };

#if IQ_MZK
static const int wavHeader[11] = {
    0x46464952,
    MZK_NUMSAMPLES*2+36,
    0x45564157,
    0x20746D66,
    16,
    WAVE_FORMAT_PCM|(MZK_NUMCHANNELS<<16),
    MZK_RATE,
    MZK_RATE*MZK_NUMCHANNELS*sizeof(short),
    (MZK_NUMCHANNELS*sizeof(short))|((8*sizeof(short))<<16),
    0x61746164,
    MZK_NUMSAMPLES*sizeof(short)
    };
#endif
#if FKLANG_MZK
// define this if you have a multicore cpu and can spare ~10 bytes for realtime playback
// undef for sound precalc
#define USE_SOUND_THREAD

// MAX_SAMPLES gives you the number of samples for the whole song. we always produce stereo samples, so times 2 for the buffer
SAMPLE_TYPE	lpSoundBuffer[MAX_SAMPLES*2];
HWAVEOUT	hWaveOut;

#pragma data_seg(".wavefmt")
WAVEFORMATEX WaveFMT =
{
#ifdef FLOAT_32BIT
	WAVE_FORMAT_IEEE_FLOAT,
#else
	WAVE_FORMAT_PCM,
#endif
    2, // channels
    SAMPLE_RATE, // samples per sec
    SAMPLE_RATE*sizeof(SAMPLE_TYPE)*2, // bytes per sec
    sizeof(SAMPLE_TYPE)*2, // block alignment;
    sizeof(SAMPLE_TYPE)*8, // bits per sample
    0 // extension not needed
};

#pragma data_seg(".wavehdr")
WAVEHDR WaveHDR =
{
	(LPSTR)lpSoundBuffer,
	MAX_SAMPLES*sizeof(SAMPLE_TYPE)*2,			// MAX_SAMPLES*sizeof(float)*2(stereo)
	0,
	0,
	0,
	0,
	0,
	0
};

MMTIME MMTime =
{
	TIME_SAMPLES,
	0
};

#pragma code_seg(".initsnd")
void  InitSound()
{
#ifdef USE_SOUND_THREAD
	// thx to xTr1m/blu-flame for providing a smarter and smaller way to create the thread :)
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)_4klang_render, lpSoundBuffer, 0, 0);
#else
	_4klang_render(lpSoundBuffer);
#endif
	waveOutOpen			( &hWaveOut, WAVE_MAPPER, &WaveFMT, NULL, 0, CALLBACK_NULL );
	waveOutPrepareHeader( hWaveOut, &WaveHDR, sizeof(WaveHDR) );
	waveOutWrite		( hWaveOut, &WaveHDR, sizeof(WaveHDR) );
}
#endif

//==============================================================================================


static LRESULT CALLBACK WndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// salvapantallas
	if( uMsg==WM_SYSCOMMAND && (wParam==SC_SCREENSAVE || wParam==SC_MONITORPOWER) )
		return( 0 );

	// boton x o pulsacion de escape
	if( uMsg==WM_CLOSE || uMsg==WM_DESTROY || (uMsg==WM_KEYDOWN && wParam==VK_ESCAPE) )
		{
		PostQuitMessage(0);
        return( 0 );
		}

    if( uMsg==WM_CHAR )
        {
        if( wParam==VK_ESCAPE )
            {
            PostQuitMessage(0);
            return( 0 );
            }
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
		ShowCursor( 1 );
    }
}

static int window_init( WININFO *info )
{
	unsigned int	PixelFormat;
    DWORD			dwExStyle, dwStyle;
    DEVMODE			dmScreenSettings;
    RECT			rec;

    WNDCLASS		wc;

    ZeroMemory( &wc, sizeof(WNDCLASS) );
    wc.style         = CS_OWNDC|CS_HREDRAW|CS_VREDRAW;
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = info->hInstance;
    wc.lpszClassName = info->wndclass;

    if( !RegisterClass(&wc) )
        return( 0 );

    if( info->full )
    {
        dmScreenSettings.dmSize       = sizeof(DEVMODE);
        dmScreenSettings.dmFields     = DM_BITSPERPEL|DM_PELSWIDTH|DM_PELSHEIGHT;
        dmScreenSettings.dmBitsPerPel = 32;
        dmScreenSettings.dmPelsWidth  = XRES;
        dmScreenSettings.dmPelsHeight = YRES;
        if( ChangeDisplaySettings(&dmScreenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL)
            return( 0 );
        dwExStyle = WS_EX_APPWINDOW;
        dwStyle   = WS_VISIBLE | WS_POPUP;// | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
		ShowCursor( 0 );
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

    info->hWnd = CreateWindowEx( dwExStyle, wc.lpszClassName, "D - Four", dwStyle,
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

    return( 1 );
}


//==============================================================================================

#if IQ_MZK
static short myMuzik[MZK_NUMSAMPLESC+22];
#endif

//==============================================================================================

int WINAPI WinMain( HINSTANCE instance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow )
{
    MSG         msg;
    int         done=0;
    WININFO     *info = &wininfo;

    info->hInstance = GetModuleHandle( 0 );

    //if( MessageBox( 0, "fullscreen?", info->wndclass, MB_YESNO|MB_ICONQUESTION)==IDYES ) info->full++;

    if( !window_init(info) )
    {
        window_end( info );
        MessageBox( 0, "window_init()!","error",MB_OK|MB_ICONEXCLAMATION );
        return( 0 );
    }

    intro_init(wininfo.hDC);

#if SOUND

#if IQ_MZK
    mzk_init( myMuzik+22 );
    memcpy( myMuzik, wavHeader, 44 );
    if( !sndPlaySound( (const char*)&myMuzik, SND_ASYNC|SND_MEMORY ) )
    {
        window_end( info );
        MessageBox( 0, "mzk???", "error", MB_OK|MB_ICONEXCLAMATION );
        return( 0 );
    }
#endif

#if FKLANG_MZK
		InitSound();
#endif


#endif

    while( !done )
    {
      static long to = 0; if( !to ) to=timeGetTime();
      long it = timeGetTime() - to;

      while( PeekMessage(&msg,0,0,0,PM_REMOVE) )
      {
	if( msg.message==WM_QUIT ) done=1;
	TranslateMessage( &msg );
	DispatchMessage( &msg );
      }
      {
	static int last_it = 0;
	int m = it / 60000;
	int s = (it / 1000) % 60;
	int c = (it / 10) % 100;
// 	int fps = 1000 / t;
	char title[255];
	sprintf(title, "D - Four | [%d fps - %d ms/f] - %02d'%02d\"%02d (t=%dms)",
		1000/(1 + it - last_it), it - last_it, m, s, c, it);
	SetWindowText( info->hWnd, title );

	last_it = it;
      }
      intro_do( it );

#if SOUND
#if IQ_MZK
        if( t>(MZK_DURATION*1000) ) done = 1;
#endif
#if FKLANG_MZK
		waveOutGetPosition(hWaveOut, &MMTime, sizeof(MMTIME));
//		float aha = (&_4klang_envelope_buffer)[((MMTime.u.sample >> 8) << 5) + 2*2+0];
		if (MMTime.u.sample > MAX_SAMPLES)
			break;
#endif
#endif
        SwapBuffers( info->hDC );
        }

#if SOUND

#if IQ_MZK
	sndPlaySound( 0, 0 );
#endif

#endif

    window_end( info );

    return( 0 );
}
