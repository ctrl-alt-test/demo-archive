//
// Material information
//

#ifndef		MATERIAL_HH
# define	MATERIAL_HH

#include "textureid.hh"
#include "shaderid.hh"

namespace Material
{
  class Element
  {
  public:
    Element(Shader::id shaderId = Shader::parallax,
	    Shader::id shadowShaderId = Shader::staticGeomVSM,
	    unsigned char shininess = 10,
	    Texture::id textureId = Texture::none,
	    Texture::id bumpTextureId = Texture::flatBump,
	    Texture::id specularTextureId = Texture::none,
	    Texture::id texture2Id = Texture::none);

//     static Array<Element> & pool();
//     static Element & New();

    int		sortHint() const;

    Shader::id	shaderId() const;
    Shader::id	shadowShaderId() const;

    Texture::id	textureId() const;
    Texture::id	texture2Id() const;
    Texture::id	bumpTextureId() const;
    Texture::id	specularTextureId() const;

    float	shininess() const;

  private:

    // Sur le GPU ce qui coûte le plus cher à changer, c'est le
    // shader, puis la texture, et enfin le VBO.

    // sortHint() va prendre ce bloc de 32 bits et le considérer comme
    // un int. On est sur du little endian, donc on veut : textures,
    // puis shader.
    // --------------------------
    Texture::id	_textureId;
    Texture::id	_specularTextureId;
    Texture::id	_bumpTextureId;
    Shader::id	_shaderId;
    // --------------------------

    // Et puis ici tant pis, ça n'intervient pas dans le tri
    Shader::id	_shadowShaderId;
    Texture::id	_texture2Id;

    unsigned char _shininess;
  };
}

#endif		// MATERIAL_HH
