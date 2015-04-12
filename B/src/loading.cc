//
// The loading part
//

#include "loading.hh"

#include "sys/msys.h"
#include <GL/glu.h>

#include "cube.hh"
#include "shaders.hh"
#include "textureunit.hh"
#include "vbodata.hh"

// ============================================================================

namespace Loading
{
  void setupRendering(int xr, int yr)
  {
    glViewport(0, 0, xr, yr);
    glEnable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(39, float(xr) / float(yr), 0.1f, 500.f);
    glMatrixMode(GL_MODELVIEW);
  }

  void clearRendering(date renderDate)
  {
    const float step = renderDate / float(POST_LOAD_DURATION);
    const float color = 0.1f * (1.f - step) + 0.05f * step;
    glClearColor(color, color, color, 1.f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  }

#define CUBE_RES 8
#define CUBE_AREA (CUBE_RES * CUBE_RES)
#define CUBES (CUBE_AREA * CUBE_RES)

  unsigned char cubes[3 * CUBES];
  unsigned char heights[CUBE_AREA];

  static void generateHeights()
  {
    msys_memset(heights, 0, CUBE_AREA * sizeof(unsigned char));
    unsigned int count = 0;
    while (count < CUBES)
    {
      unsigned char x = msys_rand() % CUBE_RES;
      unsigned char y = msys_rand() % CUBE_RES;
      while (heights[CUBE_RES * y + x] >= CUBE_RES)
      {
	x = msys_rand() % CUBE_RES;
	y = msys_rand() % CUBE_RES;
      }
      const unsigned int index = 3 * count;
      cubes[index    ] = x;
      cubes[index + 1] = heights[CUBE_RES * y + x]++;
      cubes[index + 2] = y;
      count++;
    }
  }

  static void drawCube(int n, float sizeFactor)
  {
    // Shaders et cie.
    Texture::Unit::unUse();
    Shader::list[Shader::color].use();
    VBO::Element::unUse();

    // Lumière
    glEnable( GL_LIGHTING );
    GLfloat ambientLight[] = { 0.2f, 0.25f, 0.3f, 1.f };
    GLfloat diffuseLight0[] = { 0.8f, 0.75f, 0.7f, 1.f };
    GLfloat specularLight0[] = { 1.f, 1.f, 1.f, 1.f };
    GLfloat position0[] = { 10.f + 40.f,
			    10.f - 19.8f,
			    10.f + 70.f,
			    1.f };
    glLightfv( GL_LIGHT0, GL_AMBIENT, ambientLight );
    glLightfv( GL_LIGHT0, GL_DIFFUSE, diffuseLight0 );
    glLightfv( GL_LIGHT0, GL_SPECULAR, specularLight0 );
    glLightfv( GL_LIGHT0, GL_POSITION, position0 );
    glEnable( GL_LIGHT0 );

    // Position
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef(0, 0, -3.f);
    glRotatef(30.f, 1.f, 0, 0);
    glRotatef(20.f, 0, 1.f, 0);

    // Reproduction des erreurs de calcul
//     glTranslatef( 40.f, -14.8f,  70.f);
//     glTranslatef(0, -5.f, 0);
//     glTranslatef(0,  5.f, 0);
//     glTranslatef(-40.f,  14.8f, -70.f);


    const float shift = 1.f / CUBE_RES;
    const float scale = sizeFactor * shift;
    const unsigned int stop = (n * CUBES) / LOADING_STEPS;
    for (unsigned int j = 0; j < stop; ++j)
    {
      glPushMatrix();
      const unsigned int index = 3 * j;
      const float x = shift * cubes[index    ] - 0.4375f;
      const float y = shift * cubes[index + 1] - 0.4375f;
      const float z = shift * cubes[index + 2] - 0.4375f;
      glTranslatef(x, y, z);
      glScalef(scale, scale, scale);
      glBegin(GL_QUADS);
      for (unsigned int i = 0; i < Cube::numberOfVertices; ++i)
      {
	VBO::vertex vertex = Cube::vertices[i];
	glColor3f(0.5f + x + scale * vertex.x,
		  0.5f + y + scale * vertex.y,
		  0.5f + z + scale * vertex.z);
	glNormal3fv(&vertex.nx);
	glVertex3fv(&vertex.x);
      }
      glEnd();
      glPopMatrix();
    }
  }

  void draw(int n)
  {
    // FIXME : écrire ça plus proprement
    static bool initialiazed = false;
    if (!initialiazed)
    {
      generateHeights();
      initialiazed = true;
    }

    // Dessin
    drawCube(n, 0.8f);
  }

  void drawPostLoaded(date renderDate)
  {
    const float step = renderDate / float(POST_LOAD_DURATION);
    drawCube(LOADING_STEPS, 0.8f + 0.2f * step);
  }
}

// ============================================================================
