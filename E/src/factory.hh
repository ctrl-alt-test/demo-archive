//
// Intérieur de l'usine
//

#ifndef		FACTORY_HH
# define	FACTORY_HH

#include "array.hh"
#include "mesh.hh"
#include "renderlist.hh"
#include "vbodata.hh"

namespace Factory
{
  extern Array<vertex> floorChunk;
//   extern Array<vertex> ceilChunk;
  extern Array<vertex> buildingsChunk;

  void generateMeshes();

  void create(RenderList & renderList);

  void changeLightAndFog(date renderDate,
			 GLfloat * ambient,
			 GLfloat * diffuse,
			 GLfloat * specular,
			 GLfloat * fogColor);
}

#endif		// FACTORY_HH
