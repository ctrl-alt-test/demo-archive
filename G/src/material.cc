//
// Material information
//

#include "material.hh"

namespace Material
{
  Element::Element(Shader::id shaderId,
		   Shader::id shadowShaderId,
		   unsigned char shininess,
		   Texture::id textureId,
		   Texture::id bumpTextureId,
		   Texture::id specularTextureId,
		   Texture::id texture2Id):
  _textureId(textureId),
  _specularTextureId(specularTextureId),
  _bumpTextureId(bumpTextureId),
  _shaderId(shaderId),
  _shadowShaderId(shadowShaderId),
  _texture2Id(texture2Id),
  _shininess(shininess)
  {
  }
}
