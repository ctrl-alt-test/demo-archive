#ifndef RAMP_HH_
# define RAMP_HH_

#include "renderlist.hh"
#include "vbodata.hh"
#include "anim.hh"

namespace Ramp
{
  void create(RenderList & renderList);
  void changeLight(date renderDate,
		   GLfloat * ambient,
		   GLfloat * diffuse,
		   GLfloat * specular,
		   GLfloat * position);
}

#endif
