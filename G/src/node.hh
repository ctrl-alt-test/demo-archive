//
// Node, either renderable or physical, or both or none
//

#ifndef		NODE_HH
# define	NODE_HH

#include "anims.hh"
#include "array.hh"
#include "basicTypes.hh"
#include "renderable.hh"
#include "algebra/matrix.hh"
#include "renderable.hh"

#define MAX_NUMBER_OF_NODE_RENDERABLES 4

class Node
{
public:
  ~Node();

  Node(date birthDate, date deathDate);
  static Array<Node> & pool();
  static Node * New(date birthDate, date deathDate);

  const matrix4 & localTransform() const;
  const matrix4 & globalTransform() const;
  const matrix4 & oldTransform() const;

  date		birthDate() const;
  date		deathDate() const;
  void		setDeathDate(date deathDate);

  const Node *	parent() const;
  Node * &      parentUnsafe(); // hack
  const Renderable *	visiblePart() const;
  int		numberOfRenderables() const;

  void		attachToNode(Node * parent);
  void		attachRenderable(Renderable & renderable);
  void		attachRenderableNode(Renderable & renderable);

  Anim::id	animId() const;
  date		animationStepShift() const;
  date		animationStartDate() const;
  date		animationStopDate() const;

  void          setAnimation(Anim::animFunc func);
  void		setAnimationStepShift(date stepShift);
  void		setAnimationStartDate(date startDate);
  void		setAnimationStopDate(date stopDate);

  void		update(date evalDate);

protected:
  matrix4	_computeAnimatedLocalTransform(date evalDate) const;

private:
  Node();

  date		_birthDate;
  date		_deathDate;

  matrix4	_localTransform;
  matrix4	_globalTransform;
  matrix4	_oldTransform;

  Node *	_parent;
  Renderable	_visiblePart[MAX_NUMBER_OF_NODE_RENDERABLES];
  char		_numberOfRenderables;

  Anim::id	_animId;
  date		_animationStepShift;
  date		_animationStartDate;
  date		_animationStopDate;
};

#endif		// NODE_HH
