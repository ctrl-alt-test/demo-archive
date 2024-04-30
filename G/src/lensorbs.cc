//
// Dust on the lens
//

#include "lensorbs.hh"

#include "array.hxx"
#include "camera.hh"
#include "fbos.hh"
#include "intro.hh"
#include "randomness.hh"
#include "shaders.hh"
#include "textures.hh"
#include "vbos.hh"

#include "sys/msys_glext.h"

#define NUMBER_OF_BIG_POINTS 100
#define NUMBER_OF_SMALL_POINTS 800
#define NUMBER_OF_POINTS (NUMBER_OF_BIG_POINTS + NUMBER_OF_SMALL_POINTS)
#define SIZE_OF_BIG_POINT 0.25f
#define SIZE_OF_SMALL_POINT 0.015f

namespace PostProcessing
{
  LensOrbs::LensOrbs():
    _points(NUMBER_OF_POINTS)
  {
    Rand rand;

    //
    // FIXME : il faudrait tenir compte du rapport xres/yres pour avoir
    // une répartition uniforme
    //
    for (int i = 0; i < NUMBER_OF_BIG_POINTS; ++i)
    {
      const float x = rand.fgen();
      const float y = rand.fgen();
      const float size = rand.fgen();
      vector3f point(x, y, size);
      _points.add(point);
    }

    for (int i = 0; i < NUMBER_OF_SMALL_POINTS; ++i)
    {
      const float x = rand.fgen();
      const float y = rand.fgen();
      const float size = -rand.fgen(0.f, 1.f);
      vector3f point(x, y, size);
      _points.add(point);
    }

    assert(_points.size == NUMBER_OF_POINTS);
  }

  void LensOrbs::apply()
  {
    assert(_points.size == NUMBER_OF_POINTS);

    FBO::unUse();
    VBO::Element::unUse();

    Shader::list[Shader::lensOrbs].use();
    Texture::list[Texture::roundBokeh].use(Texture::albedo);
    Texture::list[Texture::glowBuffer].use(Texture::glow);
    FBO::list[FBO::baseRender].use(true);

    Camera::orthoProj(1.f, 1.f);

    glDisable(GL_DEPTH_TEST);
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
