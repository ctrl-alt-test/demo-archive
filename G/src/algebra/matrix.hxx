//
// Matrice 4x4
//

#ifndef		MATRIX_HXX
# define	MATRIX_HXX

#include "matrix.hh"

#include "sys/msys_glext.h"

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
  oglActiveTexture(GL_TEXTURE0 + u);
  glLoadMatrixf(src.m);
}

#endif // MATRIX_HXX
