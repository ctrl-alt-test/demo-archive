//
// Matrice 4x4
//

#ifndef		MATRIX_H
# define	MATRIX_H

#include "sys/msys.h"
#include <GL/gl.h>
#include "quaternion.hh"
#include "transformation.hh"
#include "textureunit.hh"
#include "vector.hh"

struct matrix4
{
  float m[16];
};

matrix4 operator * (const matrix4 & a, const matrix4 & b);

matrix4 computeMatrix(const Transformation & t);
matrix4 computeInvMatrix(const Transformation & t);
Transformation computeTransformation(const matrix4 & a);

void invMatrix(matrix4 & a);

inline
void getFromProjection(matrix4 & dst)
{
  glGetFloatv(GL_PROJECTION_MATRIX, dst.m);
}

inline
void getFromModelView(matrix4 & dst)
{
  glGetFloatv(GL_MODELVIEW_MATRIX, dst.m);
}

inline
void setToModelView(const matrix4 & src)
{
  glMatrixMode(GL_MODELVIEW);
  glLoadMatrixf(src.m);
}

inline
void setToTextureMatrix(const matrix4 & src, Texture::usage u)
{
  glMatrixMode(GL_TEXTURE);
  oglActiveTextureARB(GL_TEXTURE0 + u);
  glLoadMatrixf(src.m);
}

void applyMatrixToVector(const matrix4 & m, vector3f & v);
void applyMatrixToPoint(const matrix4 & m, point3f & v);
matrix4 rotationMatrix(const vector3f & axis, const float angle);

matrix4 rotationMatrix(const vector3f & Xaxis,
		       const vector3f & Yaxis,
		       const vector3f & Zaxis);

#endif		// MATRIX_H
