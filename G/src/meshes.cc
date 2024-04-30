#include "meshes.hh"

#include "array.hh"
#include "materials.hh"
#include "mesh/meshpool.hh"
#include "spline.hh"
#include "tweakval.hh"
#include "variable.hh"
#include "vbos.hh"

#include "sys/msys_libc.h"

namespace Meshes
{

// Mesh
#define MESH_LIST "../data/mesh/meshList.cc"
#define MESH_BUILDER_HEADER    1
#define MESH_BUILDER_INFO      2

// Prototypes for builders
#define MESH_EXPOSE MESH_BUILDER_HEADER
#include MESH_LIST
#undef MESH_EXPOSE

const meshFileInfo meshBuilders[] =
  {
    // Noms des fonctions génératrices
    #define MESH_EXPOSE MESH_BUILDER_INFO
    #include MESH_LIST
    #undef MESH_EXPOSE
  };
const int meshBuildersLength = ARRAY_LEN(meshBuilders);

void buildAll()
{
  for (int i = 0; i < meshBuildersLength; i++)
  {
    Mesh::clearPool();
    meshBuilders[i].cb();
    Loading::update();
  }
}

#if DEBUG
void reloadSingleMesh(const char *filename)
{
    int i;
    for (i = 0; ; i++)
    {
      assert(meshBuilders[i].file != NULL); // le fichier n'est pas dans la liste ?!
      const char * file = strstr(meshBuilders[i].file, "data");
      if (strcmp(file, filename) == 0) break;
    }

    Mesh::initPool();
    meshBuilders[i].cb();
    Mesh::destroyPool();
}
#endif

}
