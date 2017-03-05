//
// Renderable thing
//

#ifndef		RENDERABLE_HXX
# define	RENDERABLE_HXX

#include "sys/msys.h"

#define INLINE_ACCESSOR(t, x) inline t Renderable::x() const {return _##x;}

inline
int Renderable::sortHint() const
{
  // On prend ce bloc de 32 bits et on le considére comme un int
  // On est sur du little endian, donc on veut : VBO, texture, shader
  return *((int*)&_vboId);
}

//
// identifiants de l'objet, de textures, de shader, de VBO
//

INLINE_ACCESSOR(int, id);

inline
void Renderable::setId(int id)
{
  _id = id;
}


INLINE_ACCESSOR(Renderable::CustomUniformFct, customSetUniform);

inline
void Renderable::setCustomSetUniform(Renderable::CustomUniformFct fct)
{
  _customSetUniform = fct;
}

INLINE_ACCESSOR(Texture::id, textureId);
INLINE_ACCESSOR(Texture::id, texture2Id);
INLINE_ACCESSOR(Texture::id, bumpTextureId);
INLINE_ACCESSOR(Texture::id, specularTextureId);
INLINE_ACCESSOR(Shader::id, shaderId);
INLINE_ACCESSOR(VBO::id, vboId);

/*
inline
void Renderable::setTexture(Texture::id textureId)
{
  _textureId = textureId;
}

inline
void Renderable::setBumpTexture(Texture::id bumpTextureId)
{
  _bumpTextureId = bumpTextureId;
}

inline
void Renderable::setSpecularTexture(Texture::id specularTextureId)
{
  _specularTextureId = specularTextureId;
}
*/

inline
void Renderable::setTexture2(Texture::id texture2Id)
{
  _texture2Id = texture2Id;
}


inline
float Renderable::shininess() const
{
  return 0.5f * float(_shininess);
}

inline
void Renderable::setShininess(unsigned char shininess)
{
  _shininess = shininess;
}

INLINE_ACCESSOR(unsigned int, numberOfVertices);

INLINE_ACCESSOR(unsigned int, primitiveType);

inline
void Renderable::setPrimitiveType(unsigned int primitiveType)
{
  _primitiveType = primitiveType;
}

#undef INLINE_ACCESSOR

#endif		// RENDERABLE_HXX
