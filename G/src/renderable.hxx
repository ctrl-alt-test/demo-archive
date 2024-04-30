//
// Renderable thing
//

#ifndef		RENDERABLE_HXX
# define	RENDERABLE_HXX

#define INLINE_ACCESSOR(t, x) inline t Renderable::x() const {return _##x;}

inline
int Renderable::sortHint() const
{
  // On prend ce bloc de 32 bits et on le considére comme un int
  // On est sur du little endian, donc on veut : VBO, texture, shader
  return *((int*)&_vboId);
}

INLINE_ACCESSOR(int, id);

inline
void Renderable::setId(int id)
{
  _id = id;
}

// INLINE_ACCESSOR(int, materialId);

// inline
// void Renderable::setMaterialId(int materialId)
// {
//   _materialId = materialId;
// }

INLINE_ACCESSOR(const Material::Element &, material);

inline
float Renderable::transition() const
{
  assert(_transitionFct != NULL);
  return _transitionFct(_id);
}

inline
void Renderable::setTransitionFct(Renderable::TransitionFct fct)
{
  assert(fct != NULL);
  _transitionFct = fct;
}

INLINE_ACCESSOR(VBO::id, vboId);
INLINE_ACCESSOR(unsigned int, numberOfVertices);
INLINE_ACCESSOR(unsigned int, primitiveType);

inline
void Renderable::setPrimitiveType(unsigned int primitiveType)
{
  _primitiveType = primitiveType;
}

#undef INLINE_ACCESSOR

#endif		// RENDERABLE_HXX
