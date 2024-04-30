//
// Material information
//

#ifndef		MATERIAL_HXX
# define	MATERIAL_HXX

#define INLINE_ACCESSOR(t, x) inline t Element::x() const {return _##x;}

namespace Material
{
  inline
  int Element::sortHint() const
  {
    // On prend ce bloc de 32 bits et on le considére comme un int
    // On est sur du little endian, donc on veut : textures, puis shader
    return *((int*)&_textureId);
  }

  INLINE_ACCESSOR(Texture::id, textureId);
  INLINE_ACCESSOR(Texture::id, texture2Id);
  INLINE_ACCESSOR(Texture::id, bumpTextureId);
  INLINE_ACCESSOR(Texture::id, specularTextureId);
  INLINE_ACCESSOR(Shader::id, shaderId);
  INLINE_ACCESSOR(Shader::id, shadowShaderId);

  inline
  float Element::shininess() const
  {
    return 0.5f * float(_shininess);
  }
}

#undef INLINE_ACCESSOR

#endif		// MATERIAL_HXX
