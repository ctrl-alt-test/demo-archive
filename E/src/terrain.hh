//
// Terrain
//

#ifndef		TERRAIN_HH
# define	TERRAIN_HH

#include "renderlist.hh"

namespace Terrain
{
  void initCache();
  void addToList(RenderList & renderList, date deathDate);
}

#endif		// TERRAIN_HH
