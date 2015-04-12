//
// Écran fait de cubes
//

#include "screen.hh"

#include "sys/msys.h"
#include <GL/gl.h>

#include "animid.hh"
#include "textureid.hh"
#include "shaderid.hh"
#include "timing.hh"
#include "vboid.hh"
#include "vbobuild.hh"
#include "vector.hh"

#define START_DATE	screenStartDate
#define END_DATE	screenEndDate

#define ARRIVAL_DURATION 8000

#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 45

#define PIXEL_SPACE 6.f

#define PIXEL_CUBE_X 28.f
#define PIXEL_CUBE_Y 10.f

namespace Screen
{
  unsigned int numberOfScreenVertices = 0;
  VBO::vertex * screen = NULL;
  VBO::vertex * redCube = NULL;
  VBO::vertex * greenCube = NULL;
  VBO::vertex * blueCube = NULL;

  static void generateScreenMesh()
  {
    const vector3f r = {1.f, 0, 0};
    const vector3f g = {0, 1.f, 0};
    const vector3f b = {0, 0, 1.f};
    numberOfScreenVertices = (3 * SCREEN_WIDTH * SCREEN_HEIGHT) * Cube::numberOfVertices;
    screen = (VBO::vertex *)msys_mallocAlloc(numberOfScreenVertices * sizeof(VBO::vertex));
    for (unsigned int j = 0; j < SCREEN_HEIGHT; ++j)
      for (unsigned int i = 0; i < SCREEN_WIDTH; ++i)
      {
	const unsigned int index = 3 * (j * SCREEN_WIDTH + i);
	vector3f p = {PIXEL_SPACE * float(i), 0, PIXEL_SPACE * float(j)};
	VBO::addCubeToChunk(screen + index * Cube::numberOfVertices, p, r);
	p.x += 1.1f;
	VBO::addCubeToChunk(screen + (index + 1) * Cube::numberOfVertices, p, g);
	p.x += 1.1f;
	VBO::addCubeToChunk(screen + (index + 2) * Cube::numberOfVertices, p, b);
      }
  }

  static void generatePixelMeshes()
  {
    redCube = (VBO::vertex *)msys_mallocAlloc(Cube::numberOfVertices * sizeof(VBO::vertex));
    greenCube = (VBO::vertex *)msys_mallocAlloc(Cube::numberOfVertices * sizeof(VBO::vertex));
    blueCube = (VBO::vertex *)msys_mallocAlloc(Cube::numberOfVertices * sizeof(VBO::vertex));
    for (unsigned int vertex = 0; vertex < Cube::numberOfVertices; ++vertex)
    {
      redCube[vertex] = Cube::vertices[vertex];
      VBO::vertex & red = redCube[vertex];
      red.r = 1.f;
      red.g = 0;
      red.b = 0;
      greenCube[vertex] = Cube::vertices[vertex];
      VBO::vertex & green = greenCube[vertex];
      green.r = 0;
      green.g = 1.f;
      green.b = 0;
      blueCube[vertex] = Cube::vertices[vertex];
      VBO::vertex & blue = blueCube[vertex];
      blue.r = 0;
      blue.g = 0;
      blue.b = 1.f;
    }
  }

  void generateMeshes()
  {
    generateScreenMesh();
    generatePixelMeshes();
  }

  void createScreen(RenderList & renderList)
  {
    /*
    // Encore un joli memory leak
    Anim::Anim * arriveAnim = new Anim::Anim(22, ARRIVAL_DURATION, 0);
    glPushMatrix();
    glLoadIdentity();

    // Montée en douceur
    for (unsigned int i = 0; i <= 20; ++i)
    {
      const float t = i * 0.05f;

      glPushMatrix();
      glLoadIdentity();
      glTranslatef(0, -5.f * (1.f - t) * (1.f - t), 0);
      arriveAnim->add(Anim::Keyframe(0.7f * t));
      glPopMatrix();

      ++i;
    }

    // État arrêté à la fin
    arriveAnim->add(Anim::Keyframe(1.f));

    glPopMatrix();
    */

    // Cube et ses deux voisins
    glPushMatrix();
    glTranslatef(PIXEL_SPACE * PIXEL_CUBE_X, 0, PIXEL_SPACE * PIXEL_CUBE_Y);
    {
      Renderable redPixel(START_DATE + ARRIVAL_DURATION, Shader::pixel,
			  START_DATE, 1.f, Cube::numberOfVertices, VBO::redPixel);
      redPixel.setTextures(Texture::darkGray);
      renderList.add(redPixel);
    }

    glTranslatef(1.1f, 0, 0);
    {
      Renderable greenPixel(START_DATE + ARRIVAL_DURATION, Shader::pixel,
			    START_DATE, 1.f, Cube::numberOfVertices, VBO::greenPixel);
      greenPixel.setTextures(Texture::darkGray);
      greenPixel.setAnimation(Anim::joinScreen);//, START_DATE);
      renderList.add(greenPixel);
    }

    glTranslatef(1.1f, 0, 0);
    {
      Renderable bluePixel(START_DATE + ARRIVAL_DURATION, Shader::pixel,
			   START_DATE, 1.f, Cube::numberOfVertices, VBO::bluePixel);
      bluePixel.setTextures(Texture::darkGray);
      renderList.add(bluePixel);
    }
    glPopMatrix();

    // Écran complet
    Renderable screen(END_DATE, Shader::pixel, START_DATE + ARRIVAL_DURATION, 1.f, numberOfScreenVertices, VBO::screen);
    screen.setTextures(Texture::screenCubeRender);
    renderList.add(screen);
  }
}
