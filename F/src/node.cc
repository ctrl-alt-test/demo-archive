//
// Node, either renderable or physical, or both or none
// FIXME: trouver un meilleur nom

#include "node.hh"

#include "anims.hh"

// définition du placement new
inline void * __cdecl  operator new(size_t size, void* ptr) { return ptr; }
inline void __cdecl  operator delete(void* ptr, void* x) { }

Array<Node> & Node::pool()
{
  const int maxNodes = 10000;
  static Array<Node> nodesPool(maxNodes);
  return nodesPool;
}

Node * Node::New(date birthDate, date deathDate)
{
  Node * ret = & (pool().getNew());
  return new(ret) Node(birthDate, deathDate);
}

Node::Node(date birthDate, date deathDate):
  _birthDate(birthDate),
  _deathDate(deathDate),
  _parent(NULL),
  _numberOfRenderables(0),
  _animId(Anim::none),
  _animationStartDate(birthDate),
  _animationStopDate(deathDate),
  _animationStepShift(0)
{
  assert(_birthDate <= _deathDate);

  // Le node est créé avec la matrice de transformation en l'état
  getFromModelView(_localTransform);
}

Node::~Node()
{
}

void Node::attachRenderableNode(Renderable & renderable)
{
  Node * n = Node::New(_birthDate, _deathDate);
  n->attachRenderable(renderable);
  n->attachToNode(this);
}

//
// Fusionne les renderables qui peuvent l'être, en appliquant leur
// transformation finale et omettant l'animation
//

// Array<RenderableContainer> * Node::mergeVisiblePart() const
// {
//   Array<RenderableContainer> * wholeVisiblePart = gatherVisiblePart();
//   wholeVisiblePart.sort();

//   int count = 1;
//   int sortHint = wholeVisiblePart[0].renderable->sortHint();
//   for (int i = 1; i < wholeVisiblePart.size; ++i)
//     if (sortHint != wholeVisiblePart[i].renderable->sortHint())
//     {
//       sortHint = wholeVisiblePart[i].renderable->sortHint();
//       ++count;
//     }


//   Array<RenderableContainer> * flat = new Array<RenderableContainer>(count);
//   count = 0;
//   sortHint = wholeVisiblePart[0].renderable->sortHint();

//   addTransformedVisiblePart(flat[count], wholeVisiblePart[0]);

//   for (int i = 1; i < wholeVisiblePart.size; ++i)
//   {
//     if (sortHint != wholeVisiblePart[i].renderable->sortHint())
//     {
//       sortHint = wholeVisiblePart[i].renderable->sortHint();
//       ++count;
//       flat[count] = new Renderable(); // FIXME : taille ?
//     }
//     flat[count]->addTransformed(wholeVisiblePart[i]);
//   }

//   delete wholeVisiblePart;

//   return flat;
// }

// ============================================================================

//
// Prend une transformation locale, et applique l'animation en evalDate
//
void Node::_applyAnimToTransform(matrix4 & t, date evalDate) const
{
  const date animDate = min(evalDate, _animationStopDate) - _animationStartDate + _animationStepShift;

  glLoadIdentity();
  Anim::list[_animId](*this, animDate);
  matrix4 animTransform;
  getFromModelView(animTransform);

  t = t * animTransform;
}

//
// Calcule l'état d'un unique noeud (vie, mort, animation, matrices)
// -- le parent DOIT avoir été mis à jour avant
// -- pourrit la matrice MODELVIEW au passage
//
void Node::update(date evalDate)
{
  if (evalDate < _birthDate || evalDate > _deathDate)
    return;

  // Mglobale = Mparente * Mlocale * Manim

  // Transformation locale
  _globalTransform = _localTransform;
  _oldTransform = _localTransform;

  // Transformation due à l'animation
  if (_animId != Anim::none && _animationStartDate <= evalDate)
  {
    _applyAnimToTransform(_globalTransform, evalDate);
    _applyAnimToTransform(_oldTransform, evalDate - 10);
  }

  // Transformation due aux parents
  if (_parent != NULL)
  {
    _globalTransform = _parent->_globalTransform * _globalTransform;
    _oldTransform = _parent->_oldTransform * _oldTransform;
  }
}

// ============================================================================
