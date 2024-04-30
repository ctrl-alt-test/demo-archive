//
// Node, either renderable or physical, or both or none
// FIXME: trouver un meilleur nom

#ifndef		NODE_HXX
# define	NODE_HXX

#include "node.hh"

#include "anims.hh"

#if DEBUG
#include "anim.hh"
#include "anims.hh"
#endif

#include "sys/msys_libc.h"

#define INLINE_ACCESSOR(t, x) inline t Node::x() const {return _##x;}

INLINE_ACCESSOR(date, birthDate);
INLINE_ACCESSOR(date, deathDate);

inline
void Node::setDeathDate(date deathDate)
{
  _deathDate = deathDate;
}

INLINE_ACCESSOR(const matrix4 &, localTransform);
INLINE_ACCESSOR(const matrix4 &, globalTransform);
INLINE_ACCESSOR(const matrix4 &, oldTransform);

INLINE_ACCESSOR(const Node *, parent);
INLINE_ACCESSOR(const Renderable *, visiblePart);
INLINE_ACCESSOR(int, numberOfRenderables);

inline
void Node::attachToNode(Node * parent)
{
  _parent = parent;
}

inline
void Node::attachRenderable(Renderable & renderable)
{
  assert(_numberOfRenderables < MAX_NUMBER_OF_NODE_RENDERABLES);
  _visiblePart[_numberOfRenderables++] = renderable;
}

INLINE_ACCESSOR(Anim::id, animId);
INLINE_ACCESSOR(date, animationStepShift);
INLINE_ACCESSOR(date, animationStartDate);
INLINE_ACCESSOR(date, animationStopDate);

inline
void Node::setAnimation(Anim::animFunc func)
{
  assert(_animId == Anim::none);
  _animId = Anim::registerAnim(func);
}

inline
void Node::setAnimationStartDate(date startDate)
{
  _animationStartDate = startDate;
}

inline
void Node::setAnimationStepShift(date stepShift)
{
  _animationStepShift = stepShift;
}

inline
void Node::setAnimationStopDate(date stopDate)
{
  _animationStopDate = stopDate;
}

inline Node * & Node::parentUnsafe()
{
  return _parent;
}

#undef INLINE_ACCESSOR

#endif		// NODE_HXX
