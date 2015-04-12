#ifndef		SOUND_H
# define	SOUND_H

// Pour (des)activer le son en debug
#ifdef DEBUG
#define SOUND 0
#else
#define SOUND 1 // NE PAS CHANGER
#endif

namespace Sound
{
  void init();
  void play(unsigned a_time = 0);
  void stop();
}

#endif		// SOUND_H
