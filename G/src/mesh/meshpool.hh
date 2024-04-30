//
// Pool de meshes, pour éviter les allocations à répétition
//

#ifndef		MESH_POOL_HH
# define	MESH_POOL_HH

namespace Mesh
{
  struct MeshStruct;

  extern MeshStruct pool[24];
  extern int meshId;

  void initPool();
  void clearPool();
  MeshStruct & getTempMesh();
  void destroyPool();
}

#endif // MESH_POOL_HH
