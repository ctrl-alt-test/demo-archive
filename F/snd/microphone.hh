#ifndef	MICROPHONE_HH
# define MICROPHONE_HH
# include "../src/vector.hh"
# include "sync.hh"

namespace Microphone
{
  void setPosition(vector3f & pos);
  void setInstrumentPosition(Instrument::Id instr, vector3f & pos);
  void getVolumePanning(int channel, int time, char ret[2]);
}

#endif
