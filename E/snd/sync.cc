#include "../src/sys/msys_debug.h"
#include "../src/trails.hh"

namespace Sync {

  static bool synchro_initialized = false;
	void playNote(int note, int vol, int channel, int time)
	{
	  if (!synchro_initialized)
	  {
	    if (vol != 0)
	    {
	      DBG("%6d: note %3d, %3d, %2d", time, note, vol, channel);
	      Trails::addNote(time, note, vol, channel);
	    }
	    if (time > 198345)
	      synchro_initialized = true;
	  }
	  return;
	}

	void changeInstr(int ch, int instr)
	{
#if DEBUG
	  if (!synchro_initialized)
	  {
	    DBG("instru %2d, %2d", ch, instr);
	  }
#endif // DEBUG
		return;
	}
}
