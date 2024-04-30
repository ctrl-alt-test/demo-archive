//
// Générateur de pavés
//

#ifndef		PAVE_HH
# define	PAVE_HH

#include "mesh.hh"

namespace Mesh
{
  enum PaveFaces : unsigned char
    {
      pave_left   = 1 << 0,
      pave_right  = 1 << 1,
      pave_bottom = 1 << 2,
      pave_top    = 1 << 3,
      pave_back   = 1 << 4,
      pave_front  = 1 << 5,
      pave_all    = 0xff,
    };

  class Pave
  {
  public:

    Pave(float sizeX, float sizeY, float sizeZ):
      _sizeX(sizeX),
      _sizeY(sizeY),
      _sizeZ(sizeZ)
    {
    }

    void generateMesh(MeshStruct & dest, PaveFaces faces = pave_all);

  private:
    float _sizeX;
    float _sizeY;
    float _sizeZ;
  };
}

#endif // PAVE_HH
