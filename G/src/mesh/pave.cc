//
// Générateur de pavés
//

#include "pave.hh"

#include "cube.hh"

using namespace Mesh;

void Pave::generateMesh(MeshStruct & dest, PaveFaces faces)
{
  for (unsigned int i = 0; i < Cube::numberOfVertices; i++)
    if (((int) faces & (1 << (i / 4))) != 0)
    {
      vertex v = Cube::vertices[i];
#if VERTEX_ATTR_POSITION
      v.p.x *= _sizeX;
      v.p.y *= _sizeY;
      v.p.z *= _sizeZ;
#endif
      dest.vertices.add(v);
    }
}
