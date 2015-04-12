/*************************************************************************************/
/*************************************************************************************/
/**                                                                                 **/
/**  Tinyplayer - TibV2 example                                                     **/
/**  written by Tammo 'kb' Hinrichs 2000-2008                                       **/
/**  This file is in the public domain                                              **/
/**  "Patient Zero" is (C) Melwyn+LB 2005, do not redistribute                      **/
/**                                                                                 **/
/*************************************************************************************/
/*************************************************************************************/

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "../src/sys/msys.h"
#include "../src/sys/msys_debug.h"
#include "v2mplayer.h"
#include "libv2.h"

static HANDLE stdout;
static V2MPlayer player;

void print(const char *bla)
{
	unsigned long bw;
	int len=-1;
	while (bla[++len]);
	WriteFile(stdout,bla,len,&bw,0);
}

////////////////////////////////////////////////////////////////////////////////////////////////////

extern "C" const sU8 theTune[];


namespace Sound {

static void synchro()
{
	// play ne joue pas de son ici, car DirectSound n'est pas encore initialise
  player.Play();
	player.Synchronizing = true;

	// on parcourt la musique pour recuperer les notes
	while (player.IsPlaying())
    player.Tick();

	player.Synchronizing = false;
	player.Stop();
}

void init()
{
  START_TIME_EVAL;

  player.Init();
  player.Open(theTune);

  synchro(); // synchro doit etre avant dsInit
  dsInit(player.RenderProxy,&player,GetForegroundWindow());

  END_TIME_EVAL("Sound setup");
}

void play(unsigned a_time)
{
  player.Play(a_time);
}

void stop()
{
  dsClose();
  player.Close();
}

}
