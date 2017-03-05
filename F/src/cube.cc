//
// Définition du cube
//

#include "cube.hh"

#include "tweakval.h"

#define U 0.5f

// Normales
#define P 1.f // 0.816496580927726f
#define N 0.f // 0.40824829046386f

// Couleurs
#define C 1.f

// Textures
#define T 1.f

namespace Cube
{
#define V3(x, y, z) vector3f(x, y, z)

  vertex vertices[] =
    {
      // FIXME : tester les couleurs en unsigned int RGBA
      // Position     normale     tangente   couleur         uv de texture
      {V3(-U, -U, -U),  V3(-P, -N, -N),  V3( 0, 0,  P),  /* 1.f, 1.f, 1.f, */  0, 0, 0, 1.f},
      {V3(-U, -U,  U),  V3(-P, -N,  N),  V3( 0, 0,  P),  /* 1.f, 1.f, 1.f, */  T, 0, 0, 1.f},
      {V3(-U,  U,  U),  V3(-P,  N,  N),  V3( 0, 0,  P),  /* 1.f, 1.f, 1.f, */  T, T, 0, 1.f},
      {V3(-U,  U, -U),  V3(-P,  N, -N),  V3( 0, 0,  P),  /* 1.f, 1.f, 1.f, */  0, T, 0, 1.f},

      {V3( U, -U, -U),  V3( P, -N, -N),  V3( 0, 0, -P),  /* 1.f, 1.f, 1.f, */  T, 0, 0, 1.f},
      {V3( U,  U, -U),  V3( P,  N, -N),  V3( 0, 0, -P),  /* 1.f, 1.f, 1.f, */  T, T, 0, 1.f},
      {V3( U,  U,  U),  V3( P,  N,  N),  V3( 0, 0, -P),  /* 1.f, 1.f, 1.f, */  0, T, 0, 1.f},
      {V3( U, -U,  U),  V3( P, -N,  N),  V3( 0, 0, -P),  /* 1.f, 1.f, 1.f, */  0, 0, 0, 1.f},

      {V3(-U, -U, -U),  V3(-N, -P, -N),  V3( P, 0,  0),  /* 1.f, 1.f, 1.f, */  0, 0, 0, 1.f},
      {V3( U, -U, -U),  V3( N, -P, -N),  V3( P, 0,  0),  /* 1.f, 1.f, 1.f, */  T, 0, 0, 1.f},
      {V3( U, -U,  U),  V3( N, -P,  N),  V3( P, 0,  0),  /* 1.f, 1.f, 1.f, */  T, T, 0, 1.f},
      {V3(-U, -U,  U),  V3(-N, -P,  N),  V3( P, 0,  0),  /* 1.f, 1.f, 1.f, */  0, T, 0, 1.f},

      {V3(-U,  U, -U),  V3(-N,  P, -N),  V3(-P, 0,  0),  /* 1.f, 1.f, 1.f, */  T, 0, 0, 1.f},
      {V3(-U,  U,  U),  V3(-N,  P,  N),  V3(-P, 0,  0),  /* 1.f, 1.f, 1.f, */  T, T, 0, 1.f},
      {V3( U,  U,  U),  V3( N,  P,  N),  V3(-P, 0,  0),  /* 1.f, 1.f, 1.f, */  0, T, 0, 1.f},
      {V3( U,  U, -U),  V3( N,  P, -N),  V3(-P, 0,  0),  /* 1.f, 1.f, 1.f, */  0, 0, 0, 1.f},

      {V3(-U, -U, -U),  V3(-N, -N, -P),  V3(-P, 0,  0),  /* 1.f, 1.f, 1.f, */  T, 0, 0, 1.f},
      {V3(-U,  U, -U),  V3(-N,  N, -P),  V3(-P, 0,  0),  /* 1.f, 1.f, 1.f, */  T, T, 0, 1.f},
      {V3( U,  U, -U),  V3( N,  N, -P),  V3(-P, 0,  0),  /* 1.f, 1.f, 1.f, */  0, T, 0, 1.f},
      {V3( U, -U, -U),  V3( N, -N, -P),  V3(-P, 0,  0),  /* 1.f, 1.f, 1.f, */  0, 0, 0, 1.f},

      {V3(-U, -U,  U),  V3(-N, -N,  P),  V3( P, 0,  0),  /* 1.f, 1.f, 1.f, */  0, 0, 0, 1.f},
      {V3( U, -U,  U),  V3( N, -N,  P),  V3( P, 0,  0),  /* 1.f, 1.f, 1.f, */  T, 0, 0, 1.f},
      {V3( U,  U,  U),  V3( N,  N,  P),  V3( P, 0,  0),  /* 1.f, 1.f, 1.f, */  T, T, 0, 1.f},
      {V3(-U,  U,  U),  V3(-N,  N,  P),  V3( P, 0,  0),  /* 1.f, 1.f, 1.f, */  0, T, 0, 1.f},
    };

#if 0

  void debugCube()
  {
    VBO::list[VBO::singleCube].use(0);
    Shader::list[Shader::color].use();
    Shader::send1i(Shader::color, Shader::objectId, 0);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_COLOR_ARRAY);

    glDrawArrays(GL_QUADS, 0, Cube::numberOfVertices);

    glDisableClientState(GL_VERTEX_ARRAY);
    glDisableClientState(GL_COLOR_ARRAY);


    for (unsigned int i = 0; i < Cube::numberOfVertices; ++i)
    {
      vertex & p = Cube::myCube(i);
      glBegin(GL_LINES);

      glColor3ub(255,   0, 0);
      glVertex3f(p.x + 0.2f * p.nx,
		 p.y + 0.2f * p.ny,
		 p.z + 0.2f * p.nz);

      glVertex3f(p.x + 0.4f * p.nx,
		 p.y + 0.4f * p.ny,
		 p.z + 0.4f * p.nz);

      glColor3ub(255, 255, 0);
      glVertex3f(p.x + 0.2f * p.nx,
		 p.y + 0.2f * p.ny,
		 p.z + 0.2f * p.nz);

      glVertex3f(p.x + 0.2f * p.nx + 0.2f * p.tx,
		 p.y + 0.2f * p.ny + 0.2f * p.ty,
		 p.z + 0.2f * p.nz + 0.2f * p.tz);

      glEnd();
    }
  }

#endif // 0


  const unsigned int numberOfVertices = sizeof(vertices) / sizeof(vertex);
  const unsigned int numberOfVerticesPerFace = (sizeof(vertices) / sizeof(vertex)) / 6;
}
