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
#include "sync.hh"

static HANDLE stdout;
static V2MPlayer player;

extern "C" const sU8 theTune[];


namespace Sound {

static void synchro()
{
    // Test si la synchro a déjà ete calculee
    IFDBG(if (Sync::sheet[0].max_size > 0) return;)

    // play ne joue pas de son ici, car DirectSound n'est pas encore initialise
    player.Play();
    player.Synchronizing = true;

    // nombre de notes maximum pour chaque instru
    for (int i = 0; i < NUMBER_OF_INSTR; i++)
      Sync::sheet[i].init(5000);

    // on parcourt la musique pour recuperer les notes
    while (player.IsPlaying())
    player.Tick();

    for (int i = 0; i < NUMBER_OF_INSTR; i++)
    {
      Array<Sync::Note> & arr = Sync::sheet[i];
      if (arr.size > 0) {
        DBG("Instrument %d: %d notes (from %d to %d)", i, arr.size, arr[0].time, arr.last().time);
      }
    }

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
  // on attend que la musique commence réellement à jouer (temps de latence)
  while (dsGetCurSmp() < 0.f) { }
}

void stop()
{
  dsClose();
  player.Close();
}

}
