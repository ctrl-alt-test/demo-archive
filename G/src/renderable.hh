//
// Renderable thing
//

#ifndef		RENDERABLE_HH
# define	RENDERABLE_HH

#include "material.hh"
#include "materialid.hh"
#include "vbos.hh"

class Renderable
{
  typedef float (*TransitionFct)(int id);

public:
  Renderable(Material::id materialId = Material::none,
	     VBO::id vboId = VBO_(singleCube));

  int		sortHint() const;
  bool		visible() const;
  void		setVisible(bool visible);
//   bool		transparent() const;

  const Material::Element & material() const;
//   int		materialId() const;

  VBO::id	vboId() const;
  unsigned int	numberOfVertices() const;
  unsigned int	primitiveType() const;
  void		setPrimitiveType(unsigned int primitiveType);

  int		id() const;
  void		setId(int id);

  float		transition() const;
  void		setTransitionFct(TransitionFct fct);

private:
  int		_id;

//   int		_materialId;
  Material::Element	_material;

  VBO::id	_vboId;
  unsigned int	_numberOfVertices;
  unsigned int	_primitiveType;
  TransitionFct _transitionFct;
};

class Node;

struct RenderableContainer
{
  Node * owner;
  const Renderable * renderable;
  bool isVisible;
//   matrix4 transformation;
};

#endif		// RENDERABLE_HH
