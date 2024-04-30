//
// Renderable thing
//

#include "renderable.hh"

#include "materials.hh"
#include "vbos.hh"

#include "sys/msys_glext.h"

static float noTransitionFct(int) { return 0.f; }

Renderable::Renderable(Material::id materialId,
		       VBO::id vboId):
//   _transparent(false),
  _id(0),
  _material(Material::list[materialId]),
//   _materialId(0),
  _vboId(vboId),
  _primitiveType(GL_QUADS),
  _transitionFct(noTransitionFct)
{
  _numberOfVertices = VBO::elementSize[vboId];
  assert(_numberOfVertices > 0); // si ça pète, c'est que VBO::setupData(vboId, ...) n'a pas été appelé
}
