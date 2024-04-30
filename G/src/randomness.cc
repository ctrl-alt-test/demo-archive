//
// Random value generator
//

#include "randomness.hh"

int Rand::igen()
{
  _seed = _seed * 0x343FD + 0x269EC3;
  return ((_seed >> 16) & 32767);
}
