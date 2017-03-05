//
// Renderable thing
//

#include "renderable.hh"
#include "vbos.hh"

#include "sys/msys.h"
#include <GL/gl.h>

Renderable::Renderable(Shader::id shaderId,
		       VBO::id vboId):
//   _transparent(false),
  _id(0),
  _shaderId(shaderId),
  _textureId(Texture::none),
  _texture2Id(Texture::none),
  _bumpTextureId(Texture::flatBump),
  _specularTextureId(Texture::none),
  _vboId(vboId),
  _shininess(10),
  _primitiveType(GL_QUADS),
  _customSetUniform(NULL)
{
  _numberOfVertices = VBO::elementSize[vboId];
  assert(_numberOfVertices > 0); // si ça pète, c'est que VBO::setupData(vboId, ...) n'a pas été appelé
}

Renderable::~Renderable()
{
}

Renderable& Renderable::setTextures(Texture::id textureId,
				    Texture::id bumpTextureId,
				    Texture::id specularTextureId)
{
  _textureId = textureId;
  _texture2Id = textureId;
  _bumpTextureId = bumpTextureId;
  _specularTextureId = specularTextureId;
  return *this;
}
