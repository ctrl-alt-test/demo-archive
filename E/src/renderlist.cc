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

void RenderList::render(date renderDate, const Transformation & oldCamera)
{
  OGL_ERROR_CHECK("RenderList::render, début");

  float oldCameraMatrix[16];
  computeInvMatrix(oldCameraMatrix, oldCamera.q, oldCamera.v);


  glEnable(GL_DEPTH_TEST);
  glCullFace(GL_BACK);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
  glEnableClientState(GL_COLOR_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);

  const int vTanLocation = oglGetAttribLocation(Shader::list[Shader::parallax].id(), "vTan");
//   OGL_ERROR_CHECK("RenderList::render, glGetAttribLocation()");
  if (vTanLocation >= 0)
  {
    oglEnableVertexAttribArray(vTanLocation);
//     OGL_ERROR_CHECK("RenderList::render, glEnableVertexAttribArray(%d)", vTanLocation);
  }

  Texture::list[Texture::ambientMap].use(4);
  Texture::list[Texture::trails].use(5);

  // Uniforms communs à tous les éléments
  Shader::state.time = renderDate;

  for (int i = 0; i <= _lastVisible; ++i)
  {
    Renderable & element = elt[i];

    Shader::list[element.shaderId()].use();
    Texture::list[element.textureId()].use(0);
    Texture::list[element.texture2Id()].use(1);
    Texture::list[element.bumpTextureId()].use(2);
    Texture::list[element.specularTextureId()].use(3);
    VBO::list[element.vboId()].use(vTanLocation);

    // Uniforms spécifiques au shader de l'élément
    Shader::setUniform1i(element.shaderId(), Shader::objectId, element.id());
    Shader::setUniform1f(element.shaderId(), Shader::textureFade, 0);

    glPushMatrix();
    glMultMatrixf(element.viewMatrix());

    //
    // Animation commentée, vu que tout est statique
    //
    /*
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

      Shader::setUniform1f(element.shaderId(), Shader::textureFade, t.textureFade);
    }
    */
    glScalef(element.size(), element.size(), element.size());


    // Uniforms spécifiques au shader de l'élément

    // Position en t - dt, pour le motion blur
    // Attention, écrit comme ça, ça ne tient pas compte de
    // l'animation de l'élément
    float oldModelView[16];
    glPushMatrix();
    glLoadMatrixf(oldCameraMatrix);
    glMultMatrixf(element.viewMatrix());
    glGetFloatv(GL_MODELVIEW_MATRIX, oldModelView);
    glPopMatrix();
    Shader::setUniform4fv(element.shaderId(), Shader::oldModelView, oldModelView);

    glDrawArrays(GL_QUADS, 0, element.numberOfVertices());
    glPopMatrix();
  }

  if (vTanLocation >= 0)
  {
    oglDisableVertexAttribArray(vTanLocation);
//     OGL_ERROR_CHECK("RenderList::render, glDisableVertexAttribArray(%d)", vTanLocation);
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

  int vTanLocation = oglGetAttribLocation(Shader::list[shaderId].id(), "vTan");
//   OGL_ERROR_CHECK("RenderList::renderSingleCube, glGetAttribLocation()");
  if (vTanLocation >= 0)
  {
    oglEnableVertexAttribArray(vTanLocation);
//     OGL_ERROR_CHECK("RenderList::renderSingleCube, glEnableVertexAttribArray(%d)", vTanLocation);
  }


  Shader::list[shaderId].use();
  Texture::list[textureId].use(0);
  Texture::list[Texture::none].use(1);
  Texture::list[Texture::testBump].use(2);
  Texture::list[Texture::none].use(3);
  VBO::list[VBO::singleCube].use(vTanLocation);

  Shader::setUniform1f(shaderId, Shader::textureFade, 0);

  glDrawArrays(GL_QUADS, 0, Cube::numberOfVertices);
//   OGL_ERROR_CHECK("RenderList::renderSingleCube, glDrawArrays()");

  if (vTanLocation >= 0)
  {
    oglDisableVertexAttribArray(vTanLocation);
//     OGL_ERROR_CHECK("RenderList::renderSingleCube, glDisableVertexAttribArray(%d)", vTanLocation);
  }
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
  glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
}

#endif // DEBUG
