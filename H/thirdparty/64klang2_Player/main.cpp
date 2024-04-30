#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include "windows.h"
#include "mmsystem.h"
#include "mmreg.h"

////////////////////////////////////////////////
// defines
////////////////////////////////////////////////

#define WAV_EXPORT
#define WAIT_FOR_IT
#define CHECK_SSE41
#ifdef CHECK_SSE41
#include <intrin.h>
#endif

////////////////////////////////////////////////
// sound
////////////////////////////////////////////////

// some song information
#include "Synth.h"
#define INCLUDE_NODES
#include "64k2Patch.h"
#include "64k2Song.h"

#define SAMPLE_RATE 44100
#define SAMPLE_TYPE float
SAMPLE_TYPE lpSoundBuffer[MAX_SAMPLES*2 + 44100*60]; // add safety buffer for 60s 
HWAVEOUT hWaveOut;

/////////////////////////////////////////////////////////////////////////////////
// initialized data
/////////////////////////////////////////////////////////////////////////////////

WAVEFORMATEX WaveFMT =
{
	WAVE_FORMAT_IEEE_FLOAT,
    2, // channels
    SAMPLE_RATE, // samples per sec
    SAMPLE_RATE*sizeof(SAMPLE_TYPE)*2, // bytes per sec
    sizeof(SAMPLE_TYPE)*2, // block alignment;
    sizeof(SAMPLE_TYPE)*8, // bits per sample
    0 // extension not needed
};

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

#ifdef WAV_EXPORT
char ExportWaveHeader[44] =
{
	'R', 'I', 'F', 'F',
	0, 0, 0, 0,				// filled below
	'W', 'A', 'V', 'E',
	'f', 'm', 't', ' ',
	16, 0, 0, 0,
	3, 0,
	2, 0,
	0x44, 0xac, 0, 0,
	0x20, 0x62, 0x05, 0,
	8, 0,
	32, 0,
	'd', 'a', 't', 'a',
	0, 0, 0, 0				// filled below
};
#endif

/////////////////////////////////////////////////////////////////////////////////
// crt emulation
/////////////////////////////////////////////////////////////////////////////////

extern "C" 
{
	int _fltused = 1;
}

char* infotext =
	"64klang2 Synthesizer and Player by Gopher/Alcatraz (2017)\n\n"
	"Song   : Don't do the Trump\n"
	"Author : Virgill\n"
	"Length : 3:38\n\n"
	"Precalc is 10s. If your CPU is too slow just enjoy the silence at some point :)\n"
	"In any case: wait for the program to finish,you'll get a exemusic.wav file then\n\n"
	"PlayPos: ";

#ifdef CHECK_SSE41
char* checktext = "Error: No SSE4.1 capable CPU detected. Enjoy the silence.";
#endif

/////////////////////////////////////////////////////////////////////////////////
// entry point for the executable if msvcrt is not used
/////////////////////////////////////////////////////////////////////////////////

#if defined _DEBUG
void main(void)
#else
void mainCRTStartup(void)
#endif
{
	// check cpu info for sse4.1 support
#ifdef CHECK_SSE41
	int CPUInfo[4];
	__cpuid(CPUInfo, 1);
	if ((CPUInfo[2] & 0x80000) == 0)
	{
		WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), checktext, strlen(checktext), NULL, NULL);
		Sleep(5000);
		ExitProcess(0);
	}
#endif

	// write info text to console
	WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), infotext, strlen(infotext), NULL, NULL);

	// init synth and start filling the buffer 
	_64klang_Init(SynthStream, SynthNodes, SynthMonoConstantOffset, SynthStereoConstantOffset, SynthMaxOffset);
#ifndef _DEBUG 
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)_64klang_Render, lpSoundBuffer, 0, 0);
	Sleep(10000);
#else
	_64klang_Render(lpSoundBuffer);
#endif	

	// start audio playback
	waveOutOpen(&hWaveOut, WAVE_MAPPER, &WaveFMT, NULL, 0, CALLBACK_NULL);
	waveOutPrepareHeader(hWaveOut, &WaveHDR, sizeof(WaveHDR));
	waveOutWrite(hWaveOut, &WaveHDR, sizeof(WaveHDR));
	DWORD lastUpdate = 0;
	do
	{
		waveOutGetPosition(hWaveOut, &MMTime, sizeof(MMTIME));
		if ((MMTime.u.sample - lastUpdate) >= MAX_SAMPLES / 70)
		{
			lastUpdate = MMTime.u.sample;
			WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), "#", 1, NULL, NULL);
		}
		Sleep(128);
	} while ((MMTime.u.sample < MAX_SAMPLES) && !GetAsyncKeyState(VK_ESCAPE));
	WriteFile(GetStdHandle(STD_OUTPUT_HANDLE), "\n", 1, NULL, NULL);

	// write a wav file in the end
#ifdef WAV_EXPORT
#ifdef WAIT_FOR_IT
	do
	{
		Sleep(128);
	} while (!_64klang_RenderDone());
#endif
	HANDLE hFile = CreateFileA("exemusic.wav", // name of the write
                       GENERIC_WRITE,          // open for writing
                       0,                      // do not share
                       NULL,                   // default security
                       CREATE_ALWAYS,          // create new file only
                       FILE_ATTRIBUTE_NORMAL,  // normal file
                       NULL);                  // no attr. template
	
	// init wave header	
	*((DWORD*)(&ExportWaveHeader[4])) = MAX_SAMPLES*2*sizeof(SAMPLE_TYPE)+36;	// size of the rest of the file in bytes
	*((DWORD*)(&ExportWaveHeader[40])) = MAX_SAMPLES*2*sizeof(SAMPLE_TYPE);		// size of raw sample data to come
	// write wave header
	WriteFile(	hFile,			// open file handle
				ExportWaveHeader,		// start of data to write
				44,				// number of bytes to write
				&lastUpdate,			// number of bytes that were written
				NULL);			// no overlapped structure
	WriteFile(	hFile,			// open file handle
				lpSoundBuffer,		// start of data to write
				MAX_SAMPLES*2*sizeof(SAMPLE_TYPE),				// number of bytes to write
				&lastUpdate,			// number of bytes that were written
				NULL);			// no overlapped structure

	CloseHandle(hFile);
#endif

	// done
	ExitProcess(0);
}
