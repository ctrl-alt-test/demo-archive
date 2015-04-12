#include <stdint.h>
#include <fcntl.h>  // open
#include <unistd.h> // close
#include <sys/ioctl.h>
#include <linux/soundcard.h>

#include "../../player/player.h"

//-----------------------------------------------------------------------------

static int dev_out, dmixer;

//-----------------------------------------------------------------------------

int msys_soundInit( void *buffer, int totallengthinbytes )
{
    dev_out = open(OSS_DEVICE_OUT, O_WRONLY);
    if(dev_out == -1)
	return 0;
    dmixer = open(OSS_DEVICE_MIXER, O_WRONLY);
    if(dmixer == -1)
	return 0;
    // Set standard WAV settings
    int val = AFMT_S16_LE;
    ioctl(dev_out, SNDCTL_DSP_SETFMT, &val);
    val = PLAYER_RATE;
    ioctl(dev_out, SNDCTL_DSP_SPEED, &val);
    val = 1;
    #if PLAYER_NUMCHANNELS==2
    ioctl(dev_out, SNDCTL_DSP_STEREO, &val);
    #else
    ioctl(dev_out, SNDCTL_DSP_MONO, &val);
    #endif

    // Set master and PCM volume to 90% (both stereo channels)
    #if PLAYER_NUMCHANNELS==2
    val = 90*256+90;
    #else
    val = 90;
    #endif
    ioctl(dmixer, SOUND_MIXER_WRITE_VOLUME, &val);
    ioctl(dmixer, SOUND_MIXER_WRITE_PCM, &val);

    // Close mixer device, no more used
    close(dmixer);
    dmixer = -1;

    return( 1 );
}   

void msys_soundStart( void )
{
    // TODO
    
}

void msys_soundEnd( void )
{
    close(dev_out);
}

long msys_soundGetPosInSamples( void )
{

    // TODO
	return 0;
}

/*
// Devuelve: -1 si ha habido un error
//           0 o mas indican el numero de bytes escritos
int OSS_OUT_Write(void *buffer, size_t buffer_bytes)
{
    int count;
    count = write(dev_out, buffer, buffer_bytes);

    return count;
}
*/

/*
uint32_t OSS_OUT_GetStatus(void)
{
    int val = AFMT_QUERY;
    ioctl(dev_out, SNDCTL_DSP_SETFMT, &val);

    #ifdef DEBUG
    printf("Format: %x\n", val);
    #endif

    ioctl(dev_out, SOUND_PCM_READ_RATE, &val);
    #ifdef DEBUG
    printf("Rate: %d\n", val);
    #endif
    return val;
}
*/