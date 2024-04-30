#include "Clock.hh"

using namespace Timeline;

Clock::Clock():
	realCurrentTime(0)
#ifdef ENABLE_PAUSE
	,
	pauseDate(0),
	timeOffset(0),
	isPaused(false)
#endif // ENABLE_PAUSE
{
}

void Clock::Update(long currentTime)
{
	realCurrentTime = currentTime;
}

long Clock::YoutubeTime() const
{
#ifdef ENABLE_PAUSE
	return (isPaused ? pauseDate : realCurrentTime) + timeOffset;
#else // !ENABLE_PAUSE
	return realCurrentTime;
#endif // !ENABLE_PAUSE
}

#ifdef ENABLE_PAUSE

void Clock::Play()
{
	if (!isPaused)
	{
		return;
	}
	isPaused = false;

	RelativeSeek(pauseDate - realCurrentTime);
}

void Clock::Pause()
{
	if (isPaused)
	{
		return;
	}
	isPaused = true;
	pauseDate = realCurrentTime;
}

void Clock::SetPaused(bool paused)
{
	if (isPaused != paused)
	{
		TogglePause();
	}
}

void Clock::TogglePause()
{
	if (isPaused)
	{
		Play();
	}
	else
	{
		Pause();
	}
}

void Clock::RelativeSeek(int delta)
{
	timeOffset += delta;

	long youtubeTime = YoutubeTime();
	if (youtubeTime < 0)
	{
		timeOffset -= youtubeTime;
	}
}

void Clock::AbsoluteSeek(int time)
{
	pauseDate = realCurrentTime;
	timeOffset = time - realCurrentTime;
	RelativeSeek(0);
}

#endif // ENABLE_PAUSE
