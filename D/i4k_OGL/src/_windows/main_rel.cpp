//--------------------------------------------------------------------------//
// iq / rgba  .  tiny codes  .  2008                                        //
//--------------------------------------------------------------------------//

#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#include <mmsystem.h>
#include "../intro.h"
#include "../config.h"

#define IQ_MZK 0
#define FKLANG_MZK 1

#if IQ_MZK
#include "../mzk.h"
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
#include "mmsystem.h"
#include "mmreg.h"
#include "../../../snd/cj-lovepotionnumber4-4klang.h"
// define this if you have a multicore cpu and can spare ~10 bytes for realtime playback
// undef for sound precalc
// #define USE_SOUND_THREAD

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


static const PIXELFORMATDESCRIPTOR pfd = {
    sizeof(PIXELFORMATDESCRIPTOR), 1, PFD_DRAW_TO_WINDOW|PFD_SUPPORT_OPENGL|PFD_DOUBLEBUFFER, PFD_TYPE_RGBA,
    32, 0, 0, 0, 0, 0, 0, 8, 0, 0, 0, 0, 0, 0, 32, 0, 0, PFD_MAIN_PLANE, 0, 0, 0, 0 };

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

#ifdef __cplusplus
extern "C"
{
#endif
int  _fltused = 0;
#ifdef __cplusplus
}
#endif

#if IQ_MZK
static short myMuzik[MZK_NUMSAMPLESC+22];
#endif

//----------------------------------------------------------------------------

void entrypoint( void )
{
    // full screen
    if( ChangeDisplaySettings(&screenSettings,CDS_FULLSCREEN)!=DISP_CHANGE_SUCCESSFUL) return;
    ShowCursor( 0 );
    // create window
    HWND hWnd = CreateWindow( "static",0,WS_POPUP|WS_VISIBLE|WS_MAXIMIZE,0,0,0,0,0,0,0,0);
    if( !hWnd ) return;
    HDC hDC = GetDC(hWnd);
    // initalize opengl
    if( !SetPixelFormat(hDC,ChoosePixelFormat(hDC,&pfd),&pfd) ) return;
    wglMakeCurrent(hDC,wglCreateContext(hDC));

    // init intro
    if( !intro_init(hDC) ) return;

    // init mzk
#if IQ_MZK
    mzk_init( myMuzik+22 );
    memcpy( myMuzik, wavHeader, 44 );
#endif

#if FKLANG_MZK
    InitSound();
#endif

	// play mzk
#if IQ_MZK
    sndPlaySound( (const char*)&myMuzik, SND_ASYNC|SND_MEMORY );
#endif

    // play intro
    long t;
    long to = timeGetTime();
    do
    {
        t = timeGetTime() - to;
        intro_do( t );
        wglSwapLayerBuffers( hDC, WGL_SWAP_MAIN_PLANE ); //SwapBuffers( hDC );
#if IQ_MZK
#define MUSIC_NOT_FINISHED_TEST (t<(MZK_DURATION*1000));
#endif
#if FKLANG_MZK
		waveOutGetPosition(hWaveOut, &MMTime, sizeof(MMTIME));
// 		float aha = (&_4klang_envelope_buffer)[((MMTime.u.sample >> 8) << 5) + 2*2+0];
#define MUSIC_NOT_FINISHED_TEST (MMTime.u.sample < MAX_SAMPLES)
#endif
#ifndef MUSIC_NOT_FINISHED_TEST
#define MUSIC_NOT_FINISHED_TEST true
#endif
    }while ( !GetAsyncKeyState(VK_ESCAPE) && MUSIC_NOT_FINISHED_TEST);


#ifdef CLEANDESTROY
    sndPlaySound(0,0);
    ChangeDisplaySettings( 0, 0 );
    ShowCursor(1);
#endif

    ExitProcess(0);
}
