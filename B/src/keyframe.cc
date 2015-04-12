//
// Keyframe
//

#include "keyframe.hh"

#include "sys/msys.h"
#include <GL/gl.h>

namespace Anim
{
  Keyframe::Keyframe(float date, float size,
		     bool use_spline, float textureFade):
    _date(date)
  {
    assert(date >= 0);
    assert(size >= 0.f);
    // La keyframe est créée avec la matrice de transformation en l'état
    float matrix[16];
    glGetFloatv(GL_MODELVIEW_MATRIX, matrix);
    _transformation.use_spline = use_spline;
    _transformation.s = size;
    _transformation.dof = DEFAULT_DOF;
    _transformation.textureFade = textureFade;
    _transformation.q = quaternionFromMatrix(matrix);
    _transformation.v.x = matrix[12];
    _transformation.v.y = matrix[13];
    _transformation.v.z = matrix[14];
  }

  Keyframe::Keyframe(float date, const Transformation & transformation):
    _date(date)
  {
    assert(date >= 0);
    _transformation = transformation;
  }

  Keyframe::~Keyframe()
  {
  }
}
