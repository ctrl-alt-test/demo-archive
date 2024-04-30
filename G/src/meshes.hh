#ifndef		MESHES_HH
# define	MESHES_HH
# include "basicTypes.hh"

namespace Meshes
{
  void buildAll();
  void reloadSingleMesh(const char *filename);

  struct meshFileInfo
  {
    callback cb;
#if DEBUG
    const char* file;
#endif
  };

  extern const meshFileInfo meshBuilders[];
  extern const int meshBuildersLength;
}

#endif
