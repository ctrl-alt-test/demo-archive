//
// The loading part
//

#include "loading.hh"

#include "sys/msys.h"
#include <GL/gl.h>
#include <GL/glu.h>

#include "cube.hh"
#include "interpolation.hh"
#include "shaders.hh"
#include "textureunit.hh"
#include "vbodata.hh"

// ============================================================================

namespace Loading
{
  void setupRendering(int xr, int yr)
  {
    glViewport(0, 0, xr, yr);
    glDisable(GL_DEPTH_TEST);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    const float factor = float(yr) / float(xr);
    gluOrtho2D(-1.f, 1.f, -factor, factor);
    glMatrixMode(GL_MODELVIEW);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
  }

  void clearRendering(date renderDate)
  {
    glClearColor(0, 0, 0, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);
  }

  static void drawSquare(float size, float x, float r, float g, float b, float level)
  {
    glColor4f(r, g, b, 0.05f);
    glPushMatrix();
    glTranslatef(x, 0, 0);
    glScalef(size, size, size);
    const int max = int(20 * level);
    for (int i = 0; i < max; ++i)
    {
      const float factor = 1.002f;
      glScalef(factor, factor, factor);
      glDrawArrays(GL_QUADS, 0, 4);
    }
    glPopMatrix();
  }

  static void drawSquares(float step)
  {
    // Shaders et cie.
    Texture::Unit::unUse();
    Shader::Program::unUse();
    VBO::Element::unUse();

    // Lumière
    glDisable(GL_LIGHTING);

    // Position
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();


    float color1 = 3.f * step;
    float color2 = 0;
    float color3 = 0;

    if (3.f * step > 1.f)
    {
      color1 = 1.f;
      color2 = 3.f * step - 1.f;

      if (3.f * step > 2.f)
      {
	color2 = 1.f;
	color3 = 3.f * step - 2.f;
	if (step > 1.f)
	{
	  color1 = color2 = color3 = 1.f - smoothStep(1.f, 2.f, step);
	}
      }
    }

    float vertices[] = {
      -1.f, -1.f,
      -1.f,  1.f,
       1.f,  1.f,
       1.f, -1.f,
    };

    glVertexPointer(2, GL_FLOAT, 0, vertices);
    glEnableClientState(GL_VERTEX_ARRAY);

    const float size = 0.1f;
    const float shift = 0.5f + 0.25f * size;
    drawSquare(size, -shift, 136.f/255.f, 155.f/255.f, 196.f/255.f, color1);
    drawSquare(size,      0, 235.f/255.f, 209.f/255.f, 158.f/255.f, color2);
    drawSquare(size,  shift, 255.f/255.f, 252.f/255.f, 247.f/255.f, color3);

    glDisableClientState(GL_VERTEX_ARRAY);
  }

  void draw(int n)
  {
    // Dessin
    drawSquares((float)n / (float)LOADING_STEPS);
  }

  void drawPostLoaded(date renderDate)
  {
    const float step = renderDate / float(POST_LOAD_DURATION);
    drawSquares(1.f + step);
  }
}

// ============================================================================
