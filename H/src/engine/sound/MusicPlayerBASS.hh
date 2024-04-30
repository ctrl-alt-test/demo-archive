#ifndef MUSIC_PLAYER_BASS_HH
#define MUSIC_PLAYER_BASS_HH

namespace Sound
{
	class MusicPlayerBASS
	{
	public:
		bool Init(const char* mp3Filename);
		void Play(unsigned a_time = 0);
		void Stop();

	private:
		unsigned long m_channelHandle;
	};
}

#endif // MUSIC_PLAYER_BASS_HH
