#include <unistd.h>
#include <sys/time.h>

long msys_timerGet( void )
{
    double long t;

    struct timeval now, res;

    gettimeofday(&now, 0);

    t = (now.tv_sec*1000) + (now.tv_usec/1000);

    return( (long)t );
}



void msys_timerSleep( long miliseconds )
{
	struct timeval tv;
	tv.tv_usec = (miliseconds % 1000) * 1000;
	tv.tv_sec = miliseconds / 1000;
	select(0, 0, 0, 0, &tv);
}
