#ifndef ROPE_HH_
# define ROPE_HH_

#include "renderlist.hh"
#include "vbodata.hh"
#include "anim.hh"

namespace Rope
{
  void create(RenderList & renderList);
  void changeLight(date renderDate,
		   GLfloat * ambient,
		   GLfloat * diffuse,
		   GLfloat * specular,
		   GLfloat * position);
  Anim::Anim * newCameraAnim();
}

#endif
