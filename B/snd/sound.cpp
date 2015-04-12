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

void init()
{
  player.Init();
  player.Open(theTune);

  dsInit(player.RenderProxy,&player,GetForegroundWindow());
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
