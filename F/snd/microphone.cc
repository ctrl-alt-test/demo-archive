#include "../src/vector.hh"
#include "sync.hh"

namespace Sync {
  extern int chanToInstr[16];
};

namespace Microphone
{
  static vector3f position;

  static vector3f instrumentsPositions[NUMBER_OF_INSTR];

  void setPosition(vector3f & pos)
  {
    position = pos;
  }

  void setInstrumentPosition(Instrument::Id instr, vector3f & pos)
  {
    instrumentsPositions[instr] = pos;
  }

  void getVolumePanning(int channel, int time, char ret[2])
  {
    int instr = Sync::chanToInstr[channel];
    vector3f ipos = instrumentsPositions[instr];
    float n = norm(position - ipos);
    float vol = max(0.f, min(127.f, n * 0.5f));
    ret[0] = (char) vol;
  }
}
