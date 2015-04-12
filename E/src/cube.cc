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
  vertex vertices[] =
    {
      // FIXME : tester les couleurs en unsigned int RGBA
      // Position     normale     tangente   couleur         uv de texture
      {-U, -U, -U,  -P, -N, -N,   0, 0,  P,  1.f, 1.f, 1.f,  0, 0, 0, 1.f},
      {-U, -U,  U,  -P, -N,  N,   0, 0,  P,  1.f, 1.f, 1.f,  T, 0, 0, 1.f},
      {-U,  U,  U,  -P,  N,  N,   0, 0,  P,  1.f, 1.f, 1.f,  T, T, 0, 1.f},
      {-U,  U, -U,  -P,  N, -N,   0, 0,  P,  1.f, 1.f, 1.f,  0, T, 0, 1.f},

      { U, -U, -U,   P, -N, -N,   0, 0, -P,  1.f, 1.f, 1.f,  T, 0, 0, 1.f},
      { U,  U, -U,   P,  N, -N,   0, 0, -P,  1.f, 1.f, 1.f,  T, T, 0, 1.f},
      { U,  U,  U,   P,  N,  N,   0, 0, -P,  1.f, 1.f, 1.f,  0, T, 0, 1.f},
      { U, -U,  U,   P, -N,  N,   0, 0, -P,  1.f, 1.f, 1.f,  0, 0, 0, 1.f},

//       {-U, -U, -U,  -N, -P, -N,   P, 0,  0,  1.f, 1.f, 1.f,  0, 0, 0, 1.f},
//       { U, -U, -U,   N, -P, -N,   P, 0,  0,  1.f, 1.f, 1.f,  T, 0, 0, 1.f},
//       { U, -U,  U,   N, -P,  N,   P, 0,  0,  1.f, 1.f, 1.f,  T, T, 0, 1.f},
//       {-U, -U,  U,  -N, -P,  N,   P, 0,  0,  1.f, 1.f, 1.f,  0, T, 0, 1.f},

      {-U,  U, -U,  -N,  P, -N,  -P, 0,  0,  1.f, 1.f, 1.f,  T, 0, 0, 1.f},
      {-U,  U,  U,  -N,  P,  N,  -P, 0,  0,  1.f, 1.f, 1.f,  T, T, 0, 1.f},
      { U,  U,  U,   N,  P,  N,  -P, 0,  0,  1.f, 1.f, 1.f,  0, T, 0, 1.f},
      { U,  U, -U,   N,  P, -N,  -P, 0,  0,  1.f, 1.f, 1.f,  0, 0, 0, 1.f},

      {-U, -U, -U,  -N, -N, -P,  -P, 0,  0,  1.f, 1.f, 1.f,  T, 0, 0, 1.f},
      {-U,  U, -U,  -N,  N, -P,  -P, 0,  0,  1.f, 1.f, 1.f,  T, T, 0, 1.f},
      { U,  U, -U,   N,  N, -P,  -P, 0,  0,  1.f, 1.f, 1.f,  0, T, 0, 1.f},
      { U, -U, -U,   N, -N, -P,  -P, 0,  0,  1.f, 1.f, 1.f,  0, 0, 0, 1.f},

      {-U, -U,  U,  -N, -N,  P,   P, 0,  0,  1.f, 1.f, 1.f,  0, 0, 0, 1.f},
      { U, -U,  U,   N, -N,  P,   P, 0,  0,  1.f, 1.f, 1.f,  T, 0, 0, 1.f},
      { U,  U,  U,   N,  N,  P,   P, 0,  0,  1.f, 1.f, 1.f,  T, T, 0, 1.f},
      {-U,  U,  U,  -N,  N,  P,   P, 0,  0,  1.f, 1.f, 1.f,  0, T, 0, 1.f},
    };

#if 0

  vertex myCube(unsigned int i)
  {
    vertex vertices[]=
      {
	{-U, -U, -U,  -P, -N, -N,  _TV(0.f), _TV(0.f), _TV( P),  0, 0, 0,  0, 0},
	{-U, -U,  U,  -P, -N,  N,  _TV(0.f), _TV(0.f), _TV( P),  0, 0, C,  T, 0},
	{-U,  U,  U,  -P,  N,  N,  _TV(0.f), _TV(0.f), _TV( P),  0, C, C,  T, T},
	{-U,  U, -U,  -P,  N, -N,  _TV(0.f), _TV(0.f), _TV( P),  0, C, 0,  0, T},

	{ U, -U, -U,   P, -N, -N,  _TV(0.f), _TV(0.f), _TV(-P),  C, 0, 0,  T, 0},
	{ U,  U, -U,   P,  N, -N,  _TV(0.f), _TV(0.f), _TV(-P),  C, C, 0,  T, T},
	{ U,  U,  U,   P,  N,  N,  _TV(0.f), _TV(0.f), _TV(-P),  C, C, C,  0, T},
	{ U, -U,  U,   P, -N,  N,  _TV(0.f), _TV(0.f), _TV(-P),  C, 0, C,  0, 0},

	{-U, -U, -U,  -N, -P, -N,  _TV( P), _TV(0.f), _TV(0.f),  0, 0, 0,  0, 0},
	{ U, -U, -U,   N, -P, -N,  _TV( P), _TV(0.f), _TV(0.f),  C, 0, 0,  T, 0},
	{ U, -U,  U,   N, -P,  N,  _TV( P), _TV(0.f), _TV(0.f),  C, 0, C,  T, T},
	{-U, -U,  U,  -N, -P,  N,  _TV( P), _TV(0.f), _TV(0.f),  0, 0, C,  0, T},

	{-U,  U, -U,  -N,  P, -N,  _TV(-P), _TV(0.f), _TV(0.f),  0, C, 0,  T, 0},
	{-U,  U,  U,  -N,  P,  N,  _TV(-P), _TV(0.f), _TV(0.f),  0, C, C,  T, T},
	{ U,  U,  U,   N,  P,  N,  _TV(-P), _TV(0.f), _TV(0.f),  C, C, C,  0, T},
	{ U,  U, -U,   N,  P, -N,  _TV(-P), _TV(0.f), _TV(0.f),  C, C, 0,  0, 0},

	{-U, -U, -U,  -N, -N, -P,  _TV(-P), _TV(0.f), _TV(0.f),  0, 0, 0,  T, 0},
	{-U,  U, -U,  -N,  N, -P,  _TV(-P), _TV(0.f), _TV(0.f),  0, C, 0,  T, T},
	{ U,  U, -U,   N,  N, -P,  _TV(-P), _TV(0.f), _TV(0.f),  C, C, 0,  0, T},
	{ U, -U, -U,   N, -N, -P,  _TV(-P), _TV(0.f), _TV(0.f),  C, 0, 0,  0, 0},

	{-U, -U,  U,  -N, -N,  P,  _TV( P), _TV(0.f), _TV(0.f),  0, 0, C,  0, 0},
	{ U, -U,  U,   N, -N,  P,  _TV( P), _TV(0.f), _TV(0.f),  C, 0, C,  T, 0},
	{ U,  U,  U,   N,  N,  P,  _TV( P), _TV(0.f), _TV(0.f),  C, C, C,  T, T},
	{-U,  U,  U,  -N,  N,  P,  _TV( P), _TV(0.f), _TV(0.f),  0, C, C,  0, T},
      };
    return vertices[i];
  }

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
