//
// Définition du cube
//

#include "cube.hh"

#include "vertex.hh"
#include "tweakval.hh"

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
#if VERTEX_ATTR_POSITION
# define POS(x, y, z) vector3f(x, y, z),
#else
# define POS(x, y, z)
#endif

#if VERTEX_ATTR_NORMAL
# define NOR(x, y, z) vector3f(x, y, z),
#else
# define NOR(x, y, z)
#endif

#if VERTEX_ATTR_TANGENT
# define TAN(x, y, z) vector3f(x, y, z),
#else
# define TAN(x, y, z)
#endif

#if VERTEX_ATTR_COLOR // FIXME : tester les couleurs en unsigned int RGBA
# define COL(r, g, b) r, g, b,
#else
# define COL(r, g, b)
#endif

#if VERTEX_ATTR_TEX_COORD
# define UV(s, t, r, q) s, t, r, q,
#else
# define UV(s, t, r, q)
#endif

#if VERTEX_ATTR_BARY_COORD
# define BAR(d0, d1, d2, d3) d0, d1, d2, d3,
#else
# define BAR(d0, d1, d2, d3)
#endif

#if VERTEX_ATTR_ID
# define ID0 0,
#else
# define ID0
#endif

  vertex vertices[] =
    {
      // Position       normale          tangente        couleur             coord de texture  coord barycentriques
      {POS(-U, -U, -U)  NOR(-P, -N, -N)  TAN( 0, 0,  P)  COL(1.f, 1.f, 1.f)  UV(0, 0, 0, 1.f)  BAR(1.f, 1.f,   0,   0) ID0},
      {POS(-U, -U,  U)  NOR(-P, -N,  N)  TAN( 0, 0,  P)  COL(1.f, 1.f, 1.f)  UV(T, 0, 0, 1.f)  BAR(  0, 1.f, 1.f,   0) ID0},
      {POS(-U,  U,  U)  NOR(-P,  N,  N)  TAN( 0, 0,  P)  COL(1.f, 1.f, 1.f)  UV(T, T, 0, 1.f)  BAR(  0,   0, 1.f, 1.f) ID0},
      {POS(-U,  U, -U)  NOR(-P,  N, -N)  TAN( 0, 0,  P)  COL(1.f, 1.f, 1.f)  UV(0, T, 0, 1.f)  BAR(1.f,   0,   0, 1.f) ID0},

      {POS( U, -U, -U)  NOR( P, -N, -N)  TAN( 0, 0, -P)  COL(1.f, 1.f, 1.f)  UV(T, 0, 0, 1.f)  BAR(1.f, 1.f,   0,   0) ID0},
      {POS( U,  U, -U)  NOR( P,  N, -N)  TAN( 0, 0, -P)  COL(1.f, 1.f, 1.f)  UV(T, T, 0, 1.f)  BAR(  0, 1.f, 1.f,   0) ID0},
      {POS( U,  U,  U)  NOR( P,  N,  N)  TAN( 0, 0, -P)  COL(1.f, 1.f, 1.f)  UV(0, T, 0, 1.f)  BAR(  0,   0, 1.f, 1.f) ID0},
      {POS( U, -U,  U)  NOR( P, -N,  N)  TAN( 0, 0, -P)  COL(1.f, 1.f, 1.f)  UV(0, 0, 0, 1.f)  BAR(1.f,   0,   0, 1.f) ID0},

      {POS(-U, -U, -U)  NOR(-N, -P, -N)  TAN( P, 0,  0)  COL(1.f, 1.f, 1.f)  UV(0, 0, 0, 1.f)  BAR(1.f, 1.f,   0,   0) ID0},
      {POS( U, -U, -U)  NOR( N, -P, -N)  TAN( P, 0,  0)  COL(1.f, 1.f, 1.f)  UV(T, 0, 0, 1.f)  BAR(  0, 1.f, 1.f,   0) ID0},
      {POS( U, -U,  U)  NOR( N, -P,  N)  TAN( P, 0,  0)  COL(1.f, 1.f, 1.f)  UV(T, T, 0, 1.f)  BAR(  0,   0, 1.f, 1.f) ID0},
      {POS(-U, -U,  U)  NOR(-N, -P,  N)  TAN( P, 0,  0)  COL(1.f, 1.f, 1.f)  UV(0, T, 0, 1.f)  BAR(1.f,   0,   0, 1.f) ID0},

      {POS(-U,  U, -U)  NOR(-N,  P, -N)  TAN(-P, 0,  0)  COL(1.f, 1.f, 1.f)  UV(T, 0, 0, 1.f)  BAR(1.f, 1.f,   0,   0) ID0},
      {POS(-U,  U,  U)  NOR(-N,  P,  N)  TAN(-P, 0,  0)  COL(1.f, 1.f, 1.f)  UV(T, T, 0, 1.f)  BAR(  0, 1.f, 1.f,   0) ID0},
      {POS( U,  U,  U)  NOR( N,  P,  N)  TAN(-P, 0,  0)  COL(1.f, 1.f, 1.f)  UV(0, T, 0, 1.f)  BAR(  0,   0, 1.f, 1.f) ID0},
      {POS( U,  U, -U)  NOR( N,  P, -N)  TAN(-P, 0,  0)  COL(1.f, 1.f, 1.f)  UV(0, 0, 0, 1.f)  BAR(1.f,   0,   0, 1.f) ID0},

      {POS(-U, -U, -U)  NOR(-N, -N, -P)  TAN(-P, 0,  0)  COL(1.f, 1.f, 1.f)  UV(T, 0, 0, 1.f)  BAR(1.f, 1.f,   0,   0) ID0},
      {POS(-U,  U, -U)  NOR(-N,  N, -P)  TAN(-P, 0,  0)  COL(1.f, 1.f, 1.f)  UV(T, T, 0, 1.f)  BAR(  0, 1.f, 1.f,   0) ID0},
      {POS( U,  U, -U)  NOR( N,  N, -P)  TAN(-P, 0,  0)  COL(1.f, 1.f, 1.f)  UV(0, T, 0, 1.f)  BAR(  0,   0, 1.f, 1.f) ID0},
      {POS( U, -U, -U)  NOR( N, -N, -P)  TAN(-P, 0,  0)  COL(1.f, 1.f, 1.f)  UV(0, 0, 0, 1.f)  BAR(1.f,   0,   0, 1.f) ID0},

      {POS(-U, -U,  U)  NOR(-N, -N,  P)  TAN( P, 0,  0)  COL(1.f, 1.f, 1.f)  UV(0, 0, 0, 1.f)  BAR(1.f, 1.f,   0,   0) ID0},
      {POS( U, -U,  U)  NOR( N, -N,  P)  TAN( P, 0,  0)  COL(1.f, 1.f, 1.f)  UV(T, 0, 0, 1.f)  BAR(  0, 1.f, 1.f,   0) ID0},
      {POS( U,  U,  U)  NOR( N,  N,  P)  TAN( P, 0,  0)  COL(1.f, 1.f, 1.f)  UV(T, T, 0, 1.f)  BAR(  0,   0, 1.f, 1.f) ID0},
      {POS(-U,  U,  U)  NOR(-N,  N,  P)  TAN( P, 0,  0)  COL(1.f, 1.f, 1.f)  UV(0, T, 0, 1.f)  BAR(1.f,   0,   0, 1.f) ID0},
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
