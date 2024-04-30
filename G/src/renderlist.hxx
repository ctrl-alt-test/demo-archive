//
// Render list
//

#ifndef		RENDERLIST_HXX
# define	RENDERLIST_HXX

#include "renderlist.hh"

inline
int RenderList::size() const
{
  return _renderables.size;
}

inline
void RenderList::add(Node & owner, const Renderable & renderable)
{
  const RenderableContainer container = {&owner, &renderable, false};
  _renderables.add(container);
}

#endif		// RENDERLIST_HXX
