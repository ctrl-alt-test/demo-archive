//
// Dust on the lens
//

#include "lensorbs.hh"

#include "sys/msys.h"
#include <GL/gl.h>
#include <GL/glu.h>

#include "camera.hh"
#include "fbos.hh"
#include "intro.hh"
#include "shaders.hh"
#include "textures.hh"
#include "vbos.hh"

#define NUMBER_OF_BIG_POINTS 100
#define NUMBER_OF_SMALL_POINTS 800
#define NUMBER_OF_POINTS (NUMBER_OF_BIG_POINTS + NUMBER_OF_SMALL_POINTS)
#define SIZE_OF_BIG_POINT 0.3f
#define SIZE_OF_SMALL_POINT 0.01f

namespace PostProcessing
{
  LensOrbs::LensOrbs():
    _points(NUMBER_OF_POINTS)
  {
    //
    // FIXME : il faudrait tenir compte du rapport xres/yres pour avoir
    // une répartition uniforme
    //
    for (int i = 0; i < NUMBER_OF_BIG_POINTS; ++i)
    {
      const float x = msys_frand();
      const float y = msys_frand();
      vector3f point(x, y, SIZE_OF_BIG_POINT * intro.xres);
      _points.add(point);
    }

    for (int i = 0; i < NUMBER_OF_SMALL_POINTS; ++i)
    {
      const float x = msys_frand();
      const float y = msys_frand();
      vector3f point(x, y, SIZE_OF_SMALL_POINT * intro.xres);
      _points.add(point);
    }

    assert(_points.size == NUMBER_OF_POINTS);
  }

  void LensOrbs::apply()
  {
    assert(_points.size == NUMBER_OF_POINTS);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);

    Camera::orthoProj(1.f, 1.f);

    VBO::Element::unUse();

    Texture::Unit& shape = Texture::list[Texture::irisBokeh];
    Texture::Unit& glow = Texture::list[FBO::list[FBO::postProcessDownscale8].renderTexture()];
    shape.use(Texture::albedo);
    glow.use(Texture::glow);

    Shader::list[Shader::lensOrbs].use();

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glEnable(GL_POINT_SPRITE);
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    glBegin(GL_POINTS);
    for (int i = 0; i < NUMBER_OF_POINTS; ++i)
    {
      glVertex3fv(&_points[i].x);
    }
    glEnd();
    glDisable(GL_POINT_SPRITE);
    glDisable(GL_BLEND);
  }
}
