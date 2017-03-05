//
// Renderable thing
//

#ifndef		RENDERABLE_HH
# define	RENDERABLE_HH

#include "cube.hh"
#include "textureid.hh"
#include "shaderid.hh"
#include "vboid.hh"

class Renderable
{
  typedef void (*CustomUniformFct)(Shader::id, int id);

public:
  Renderable(Shader::id shaderId = Shader::parallax,
	     VBO::id vboId = VBO::singleCube);
  ~Renderable();

  int		sortHint() const;
  bool		visible() const;
  void		setVisible(bool visible);
//   bool		transparent() const;
  unsigned int	numberOfVertices() const;
  unsigned int	primitiveType() const;
  void		setPrimitiveType(unsigned int primitiveType);

  int		id() const;
  void		setId(int id);

  CustomUniformFct customSetUniform() const;
  void setCustomSetUniform(CustomUniformFct fct);

  Texture::id	textureId() const;
  Texture::id	texture2Id() const;
  Texture::id	bumpTextureId() const;
  Texture::id	specularTextureId() const;
  Shader::id	shaderId() const;
  VBO::id	vboId() const;

  Renderable&	setTextures(Texture::id textureId,
			    Texture::id bumpTextureId = Texture::flatBump,
			    Texture::id specularTextureId = Texture::none);
  void		setTexture2(Texture::id texture2Id);

  float		shininess() const;
  void		setShininess(unsigned char shininess);

private:
  int		_id;

  // Sur le GPU ce qui coûte le plus cher à changer, c'est le shader,
  // puis la texture, et enfin le VBO.

  // sortHint() va prendre ce bloc de 32 bits et le considérer comme un int
  // On est sur du little endian, donc on veut : VBO, texture, shader
  // --------------------------
  VBO::id	_vboId;
  Texture::id	_textureId;
  Texture::id	_bumpTextureId;
  Shader::id	_shaderId;
  // --------------------------

  // Et puis ici tant pis, ça n'intervient pas dans le tri
  Texture::id	_specularTextureId;
  Texture::id	_texture2Id;

  unsigned char _shininess;

  unsigned int	_numberOfVertices;
  unsigned int	_primitiveType;
  CustomUniformFct _customSetUniform;
};


#include "renderable.hxx"

class Node;

struct RenderableContainer
{
  Node * owner;
  const Renderable * renderable;
  bool isVisible;
//   matrix4 transformation;
};

#endif		// RENDERABLE_HH
