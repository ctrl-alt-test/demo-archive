//
// Pool de meshes, pour éviter les allocations à répétition
//

#include "meshpool.hh"

#include "mesh.hh"
#include "array.hxx"

namespace Mesh
{
  MeshStruct pool[];
  const int poolSize = sizeof(pool) / sizeof(pool[0]);
  int meshId = 0;

  void initPool()
  {
    for (int i = 0; i < poolSize; ++i)
      pool[i].vertices.init(65536);
    meshId = 0;
  }

  void clearPool()
  {
    meshId = 0;
  }

  MeshStruct & getTempMesh()
  {
    assert(meshId < poolSize);
    pool[meshId].vertices.empty();
    return pool[meshId++];
  }

  void destroyPool()
  {
    for (int i = 0; i < poolSize; ++i)
    {
      msys_mallocFree(pool[i].vertices.elt);
      IFDBG(pool[i].vertices.max_size = 0;)
      IFDBG(pool[i].vertices.elt = 0;)
    }
    meshId = 0;
  }
}
