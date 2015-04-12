//
// Texture des motifs lumineux
//

#ifndef		TRAILS_HH
# define	TRAILS_HH

#include "basicTypes.hh"

#if DEBUG
# define DEBUG_TRAILS 0
#else
# define DEBUG_TRAILS 0 // 0 et ne doit pas changer
#endif

namespace Trails
{
  void init();
  void addNote(int t, int note, int volume, int channel);
  void render(date renderDate);
}

#endif		// TRAILS_HH
