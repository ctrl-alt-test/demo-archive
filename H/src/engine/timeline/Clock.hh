#pragma once

namespace Timeline
{
	class Clock
	{
	public:
		long				realCurrentTime;
#ifdef ENABLE_PAUSE
		long				pauseDate;
		long				timeOffset;
		bool				isPaused;
#endif //ENABLE_PAUSE

		Clock();

		void Update(long currentTime);
		long YoutubeTime() const;

#ifdef ENABLE_PAUSE
		void Play();
		void Pause();
		void SetPaused(bool paused);
		void TogglePause();
		void RelativeSeek(int delta);
		void AbsoluteSeek(int time);
#endif // ENABLE_PAUSE
	};
}
