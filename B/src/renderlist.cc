//
// Render list
//

#include "renderlist.hh"

#include "sys/msys.h"
#include <GL/gl.h>
#include <GL/glu.h>

#include "anim.hh"
#include "anims.hh"
#include "cube.hh"
#include "fbos.hh"
#include "keyframe.hh"
#include "textureid.hh"
#include "textureunit.hh"
#include "textures.hh"
#include "timeline.hh"
#include "shaderProgram.hh"
#include "shaders.hh"
#include "timing.hh"
#include "vbodata.hh"
#include "vboid.hh"
#include "vbos.hh"

unsigned int treadmillVBO = 0;

RenderList::RenderList(int max):
  Array<Renderable>(max),
  _lastVisible(0)
{
}

RenderList::~RenderList()
{
}

void RenderList::fossoyeur(date renderDate)
{
  int i = 0;
  while (i < size)
    if (renderDate < elt[i].deathDate())
    {
      elt[i].setVisible(renderDate >= elt[i].birthDate() &&
			elt[i].size() != 0);
      ++i;
    }
    else
    {
#if 0 // DEBUG

      elt[i].setVisible(false);
      ++i;

#else

      // FIXME : c'est sous optimal car en faisant remove, on va
      // échanger l'élément avec un élément en fin de tableau, et donc
      // invisible. Il faudrait organiser le tableau différemment.
      remove(i);

#endif // !DEBUG
    }
}

void RenderList::moveInvisibleAtEnd()
{
  // FIXME : vérifier que ça marche
  int firstInvisible = 0;
  _lastVisible = size - 1;
  if (size == 0)
  {
    return;
  }
  int swapped = 0;
  while (firstInvisible < _lastVisible)
  {
    if (!elt[firstInvisible].visible() && elt[_lastVisible].visible())
    {
      Renderable swap(elt[firstInvisible]);
      elt[firstInvisible] = elt[_lastVisible];
      elt[_lastVisible] = swap;
      ++firstInvisible;
      --_lastVisible;
      ++swapped;
    }
    while (firstInvisible <= _lastVisible && elt[firstInvisible].visible())
      ++firstInvisible;
    while (firstInvisible <= _lastVisible && !elt[_lastVisible].visible())
      --_lastVisible;
  }
}

void RenderList::sort()
{
  if (_lastVisible <= 0)
  {
    return;
  }
  _copyAndQuickSort(0, _lastVisible);
}

void RenderList::renderSkyBox(date renderDate, const vector3f & cameraPosition)
{
  Texture::id wallsTextureId = Texture::none;
  Texture::id roofTextureId = Texture::none;

  Texture::id wallsTextureId2 = Texture::none;
  Texture::id roofTextureId2 = Texture::none;

  if (Timeline::setSkybox(renderDate,
			  wallsTextureId, roofTextureId,
			  wallsTextureId2, roofTextureId2) == false)
    return;

  glDisable(GL_DEPTH_TEST);
  glCullFace(GL_FRONT);

  Shader::list[Shader::texture].use();

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);

  glPushMatrix();
  glTranslatef(cameraPosition.x, cameraPosition.y, cameraPosition.z);
  glScalef(50.f, 50.f, 50.f);

  Texture::list[wallsTextureId].use(0);
  Texture::list[wallsTextureId2].use(1);
  VBO::list[VBO::cubeWalls].use(0);
  glDrawArrays(GL_QUADS, 0, 5 * Cube::numberOfVerticesPerFace);

  Texture::list[roofTextureId].use(0);
  Texture::list[roofTextureId2].use(1);
  VBO::list[VBO::cubeRoof].use(0);
  glDrawArrays(GL_QUADS, 0, Cube::numberOfVerticesPerFace);

  glPopMatrix();

  glDisableClientState(GL_VERTEX_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

void RenderList::renderSpinningCube(date renderDate)
{
  const unsigned int width = Texture::list[Texture::screenCubeRender].renderWidth();
  const unsigned int height = Texture::list[Texture::screenCubeRender].renderHeight();

  glViewport(0, 0, width, height);

  glClearColor(0.05f, 0.05f, 0.05f, 1.f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//   glCullFace(GL_BACK);
  glEnable(GL_DEPTH_TEST);

  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
//   gluPerspective(39, float(width) / float(height), 1.f, 10.f);
  gluPerspective(39, 16.f / 9.f, 1.f, 10.f);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  glTranslatef(0, 0, -3.f);
  glRotatef(30.f, 1.f, 0, 0);
  glRotatef(renderDate * 0.1f, 0, 1.f, 0);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);

  Shader::list[Shader::color].use();
  Shader::unicast1i(Shader::color, Shader::objectId, 0);
  Texture::list[Texture::none].use(0);
  VBO::list[VBO::theCube].use(0);
  glDrawArrays(GL_QUADS, 0, Cube::numberOfVertices);

//  Texture::list[Texture::screenCubeRender].renderToTexture();

  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
}

void RenderList::render(date renderDate)
{
  glEnable(GL_DEPTH_TEST);
  glCullFace(GL_BACK);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);

  const int vTanIndex = oglGetAttribLocation(Shader::list[Shader::bump].id(), "vTan");
  assert(vTanIndex != -1);
  oglEnableVertexAttribArray(vTanIndex);


  Shader::broadcast1i(Shader::time, renderDate);
  Shader::unicast1f(Shader::default, Shader::textureFade, 0);
  Shader::unicast1f(Shader::bump, Shader::textureFade, 0);

  for (int i = 0; i <= _lastVisible; ++i)
  {
    Renderable & element = elt[i];

    Shader::list[element.shaderId()].use();
    if (element.shaderId() == Shader::color ||
        element.shaderId() == Shader::building ||
	element.shaderId() == Shader::ramp ||
	element.shaderId() == Shader::conway ||
	element.shaderId() == Shader::tree ||
        element.shaderId() == Shader::rope)
    {
      Shader::unicast1i(element.shaderId(), Shader::objectId, element.id());
    }

    Texture::list[element.textureId()].use(0);
    Texture::list[element.texture2Id()].use(1);
    Texture::list[element.bumpTextureId()].use(2);
    Texture::list[element.specularTextureId()].use(3);

    VBO::list[element.vboId()].use(vTanIndex);

    glPushMatrix();
    glMultMatrixf(element.viewMatrix());
    if (element.animId() != Anim::none &&
	element.animationStartDate() <= renderDate)
    {
      Anim::Anim & animation = Anim::list[element.animId()];

      // FIXME : une optim peut-être possible serait d'avoir une
      // animation "pas d'anim" plutôt qu'un if
      const date animDate = renderDate < element.animationStopDate() ? renderDate : element.animationStopDate();
      const date animLocalDate = animDate - element.animationStartDate();
      const Transformation & t = (animation.cached() ?
				  animation.cachedGetTransformation(animLocalDate, element.animationStepShift()) :
				  animation.getTransformation(animLocalDate, element.animationStepShift()));
      float matrix[16];
      computeMatrix(matrix, t.q, t.v);
      glMultMatrixf(matrix);
      glScalef(t.s, t.s, t.s);

      if (element.shaderId() == Shader::default ||
	  element.shaderId() == Shader::bump)
      {
	Shader::unicast1f(element.shaderId(), Shader::textureFade, t.textureFade);
      }
    }
    glScalef(element.size(), element.size(), element.size());
    glDrawArrays(GL_QUADS, 0, element.numberOfVertices());
    glPopMatrix();
  }

  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
}

#if DEBUG

void RenderList::renderSingleCube(Shader::id shaderId, Texture::id textureId)
{
  glEnable(GL_DEPTH_TEST);
  glCullFace(GL_BACK);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);

  Shader::list[shaderId].use();
  Texture::list[textureId].use(0);
  Texture::list[Texture::none].use(1);
  Texture::list[Texture::none].use(2);
  Texture::list[Texture::none].use(3);

  VBO::list[VBO::singleCube].use(0);
  glDrawArrays(GL_QUADS, 0, Cube::numberOfVertices);

  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
}

#endif // DEBUG
