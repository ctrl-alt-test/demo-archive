#pragma once

#ifdef ENABLE_PLAYER_64KLANG2
#ifdef _WIN32
# include "windows.h"
#else
// Linux
#endif

namespace Sound
{
	class MusicPlayer64k2
	{
	public:
		bool IsSupported();
		void Init();
		void Play(int timeInMs = 0);
		void Stop();
	private:
#ifdef _WIN32
		HWAVEOUT	_hWaveOut;
#endif
		bool		_isPlaying;
	};
}

#else // !ENABLE_PLAYER_64KLANG2

namespace Sound
{
	class MusicPlayer64k2
	{
	public:
		bool IsSupported(){ return false; }
		void Init() {}
		void Play(int timeInMs = 0) {}
		void Stop() {}
	};
}

#endif // !ENABLE_PLAYER_64KLANG2

extern Sound::MusicPlayer64k2 MUSIC_PLAYER;
