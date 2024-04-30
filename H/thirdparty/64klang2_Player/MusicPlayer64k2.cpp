#include "MusicPlayer64k2.hh"

Sound::MusicPlayer64k2 MUSIC_PLAYER;

using namespace Sound;

#ifdef ENABLE_PLAYER_64KLANG2

#ifdef _WIN32
# include <intrin.h>
# include <mmreg.h>
# include <windows.h>
#endif

// some song information
#include "Synth.h"
#define INCLUDE_NODES
#include "64k2Patch.h"
#include "64k2Song.h"

#define SAMPLE_RATE 44100
#define SAMPLE_TYPE float
static SAMPLE_TYPE lpSoundBuffer[MAX_SAMPLES * 2 + 44100 * 60]; // add safety buffer for 60s 

static WAVEFORMATEX WaveFMT =
{
	WAVE_FORMAT_IEEE_FLOAT,
	2, // channels
	SAMPLE_RATE, // samples per sec
	SAMPLE_RATE * sizeof(SAMPLE_TYPE) * 2, // bytes per sec
	sizeof(SAMPLE_TYPE) * 2, // block alignment;
	sizeof(SAMPLE_TYPE) * 8, // bits per sample
	0 // extension not needed
};

static WAVEHDR WaveHDR =
{
	(LPSTR)lpSoundBuffer,
	MAX_SAMPLES * sizeof(SAMPLE_TYPE) * 2,			// MAX_SAMPLES*sizeof(float)*2(stereo)
	0,
	0,
	0,
	0,
	0,
	0
};

bool MusicPlayer64k2::IsSupported()
{
	int CPUInfo[4];
	__cpuid(CPUInfo, 1);
	if ((CPUInfo[2] & 0x80000) == 0)
	{
		return false;
	}
	return true;
}

void MusicPlayer64k2::Init()
{
	_64klang_Init(SynthStream, SynthNodes, SynthMonoConstantOffset, SynthStereoConstantOffset, SynthMaxOffset);
	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)_64klang_Render, lpSoundBuffer, 0, 0);
	waveOutOpen(&_hWaveOut, WAVE_MAPPER, &WaveFMT, NULL, 0, CALLBACK_NULL);
}

void MusicPlayer64k2::Play(int timeInMs)
{
	Stop();
	int shift = (long long)timeInMs * SAMPLE_RATE / 1000 * 2;
	int len = (MAX_SAMPLES * 2 - shift) * sizeof(SAMPLE_TYPE);
	if (len < 0)
		return;
	WaveHDR.lpData = (LPSTR)(lpSoundBuffer + shift);
	WaveHDR.dwBufferLength = len;
	waveOutPrepareHeader(_hWaveOut, &WaveHDR, sizeof(WaveHDR));
	waveOutWrite(_hWaveOut, &WaveHDR, sizeof(WaveHDR));
	_isPlaying = true;
}

void MusicPlayer64k2::Stop()
{
	if (_isPlaying)
	{
		waveOutReset(_hWaveOut);
		_isPlaying = false;
	}
}

#endif // ENABLE_PLAYER_64KLANG2
