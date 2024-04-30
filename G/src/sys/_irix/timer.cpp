#include <unistd.h>
#include <sys/time.h>



//static struct timeval to;
//static int started = 0;

float TIMER_Get( void )
{
    double long t;
    double ret;
    struct timeval now;//, res;

    gettimeofday(&now, 0);

    //timersub( &now, &to, &res );

    //t = (res.tv_sec*1000) + (res.tv_usec/1000);
    t = (now.tv_sec*1000) + (now.tv_usec/1000);
    ret = .001*(double)t;

    return( (float)ret );
}



void TIMER_Sleep( long miliseconds )
{
	struct timeval tv;
	tv.tv_usec = (miliseconds % 1000) * 1000;
	tv.tv_sec = miliseconds / 1000;
	select(0, 0, 0, 0, &tv);
}
