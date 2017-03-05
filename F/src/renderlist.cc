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
#include "intro.hh"
#include "light.hh"
#include "shaderProgram.hh"
#include "shaders.hh"
#include "textures.hh"
#include "textureunit.hh"
#include "vbodata.hh"
#include "vboid.hh"
#include "vbos.hh"
#include "physic_test.hh"

RenderList * RenderList::createFromScene(Array<Node> & nodes)
{
  int listSize = 0;
  for (int i = 0; i < nodes.size; ++i)
  {
    listSize += nodes[i].numberOfRenderables();
  }
  assert(listSize > 0);
  RenderList * renderList = new RenderList(listSize);

  for (int j = 0; j < nodes.size; ++j)
  {
    Node & node = nodes[j];
    for (int i = 0; i < node.numberOfRenderables(); ++i)
    {
      RenderableContainer visiblePart = { &node, &(node.visiblePart()[i]), true };
      renderList->_renderables.add(visiblePart);
    }
  }

  return renderList;
}

RenderList::RenderList(int max):
  _renderables(max),
  _lastVisible(0)
{
  DBG("Creating a renderlist with %d elements capacity", max);
}

RenderList::~RenderList()
{
}

//
// Représentation de la liste de renderable :
//
//         visibles              invisibles      supprimés
// [ooooooooooooooooooooooooooo..............//////////////////]
//  ^                         ^              ^                ^
//  0                    _lastVisible       size          max_size - 1
//
// Pour supprimer un élément, on le remplace par celui en (size - 1)
// Problème : c'était donc un élément invisible, qu'il va falloir échanger
// à nouveau !
//
//
// TODO :
// Mettre les éléments invisibles au début
//
//    invisibles           visibles              supprimés
// [..............ooooooooooooooooooooooooooo//////////////////]
//  ^             ^                          ^                ^
//  0       _firstVisible                   size          max_size - 1
//
// (modification en cours dans la fonction update() au dessous)
//

/*
void RenderList::update(date renderDate)
{
  // 1 : on dégage les éléments morts
  int i = 0;
  while (i < size())
    if (renderDate < _renderables[i].owner->deathDate())
    {
      _renderables[i].isVisible = (renderDate >= _renderables[i].owner->birthDate());
      ++i;
    }
    else
    {
      _renderables.remove(i);
    }

  // 2 : on sépare les invisibles des visibles
  int begin = 0;
  int end = size() - 1;
  while (begin  end)
  {
    if (_renderables[begin].isVisible && !_renderables[end].isVisible)
    {
      _renderables.swap(begin, end);
      ++begin;
      --end;
    }
    while (begin <= end && !_renderables[begin].isVisible)
      ++begin;
    while (begin <= end && _renderables[end].isVisible)
      --end;
  }
  _firstVisible = end;

  // 3 : on trie les visibles
  _renderables._copyAndQuickSort(_firstVisible, size() - 1);
}
*/

void RenderList::fossoyeur(date renderDate)
{
  int i = 0;
  while (i < size())
    if (renderDate < _renderables[i].owner->deathDate())
    {
      _renderables[i].isVisible = (renderDate >= _renderables[i].owner->birthDate());
      ++i;
    }
    else
    {
#if DEBUG

      _renderables[i].isVisible = false;
      ++i;

#else

      // FIXME : Voir commentaire et code commenté au dessus.
      _renderables.remove(i);

#endif // !DEBUG
    }
}

void RenderList::moveInvisibleAtEnd()
{
  // FIXME : vérifier que ça marche
  int firstInvisible = 0;
  _lastVisible = size() - 1;
  if (size() == 0)
  {
    return;
  }
  int swapped = 0;
  while (firstInvisible < _lastVisible)
  {
    if (!_renderables[firstInvisible].isVisible &&
	_renderables[_lastVisible].isVisible)
    {
      RenderableContainer swap(_renderables[firstInvisible]);
      _renderables[firstInvisible] = _renderables[_lastVisible];
      _renderables[_lastVisible] = swap;
      ++firstInvisible;
      --_lastVisible;
      ++swapped;
    }
    while (firstInvisible <= _lastVisible &&
	   _renderables[firstInvisible].isVisible)
      ++firstInvisible;

    while (firstInvisible <= _lastVisible &&
	   !_renderables[_lastVisible].isVisible)
      --_lastVisible;
  }
}

void RenderList::sort()
{
  if (_lastVisible <= 0)
  {
    return;
  }
  _renderables._copyAndQuickSort(0, _lastVisible);
}

//
// Le but c'est que les éléments ont été triés pour que les états
// soient regroupés et que ces use() soient le plus souvent sans effet
//
static void _setStatesForElement(const Renderable & element)
{
#if !STATIC_SHADERS
    if (intro.debugRenderMode != 0)
    {
      if (intro.debugRenderMode == 1)
	Shader::list[Shader::debugWhiteLight].use();
      if (intro.debugRenderMode == 2)
	Shader::list[Shader::debugNormals].use();
      if (intro.debugRenderMode == 3)
	Shader::list[Shader::debugLight].use();
    }
    else
#endif // !STATIC_SHADERS
    {
      Shader::list[element.shaderId()].use();
    }
    VBO::list[element.vboId()].use();
    Texture::list[element.textureId()].use(Texture::albedo);
    Texture::list[element.bumpTextureId()].use(Texture::normal);
    Texture::list[element.specularTextureId()].use(Texture::material);
}

static void _setMotionInfo(Shader::id shaderId, const Node & owner,
			   const matrix4 & oldCameraMatrix)
{
    // Position en t - dt, pour le motion blur
    matrix4 oldModelView = oldCameraMatrix * owner.oldTransform();
    Shader::setUniformM4fv(shaderId, Shader::oldModelView, oldModelView.m);
}

void RenderList::renderShadows(date renderDate)
{
  OGL_ERROR_CHECK("RenderList::renderShadows, début");

  glCullFace(GL_BACK);
  glEnableClientState(GL_VERTEX_ARRAY);

  // Uniforms communs à tous les éléments
  Shader::state.time = renderDate;

  matrix4 lightMatrix;
  getFromModelView(lightMatrix); // Point de vue de la lumière

  for (int i = 0; i <= _lastVisible; ++i)
  {
    const Renderable & element = *(_renderables[i].renderable);
//     Shader::list[element.shaderId()].use(); // Version ombre
    VBO::list[element.vboId()].use();

    // Uniforms spécifiques au shader de l'élément
//     Shader::setUniform1i(element.shaderId(), Shader::objectId, element.id());

    setToModelView(lightMatrix * _renderables[i].owner->globalTransform());
    glDrawArrays(element.primitiveType(), 0, element.numberOfVertices());
  }

  glDisableClientState(GL_VERTEX_ARRAY);
}

void RenderList::render(date renderDate, const Camera::Camera & oldCamera)
{
  OGL_ERROR_CHECK("RenderList::render, début");

  glCullFace(GL_BACK);
  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
//   glEnableClientState(GL_COLOR_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  oglEnableVertexAttribArray(Shader::vTan);

//   Texture::list[Texture::ambientMap].use(Texture::ambient);
  Texture::list[Texture::varianceShadowMap].use(Texture::shadow);

  // Uniforms communs à tous les éléments
  Shader::state.time = renderDate;

  matrix4 oldCameraMatrix = computeInvMatrix(oldCamera.t);
  matrix4 cameraMatrix;
  getFromModelView(cameraMatrix);

  for (int i = 0; i <= _lastVisible; ++i)
  {
    const Renderable & element = *(_renderables[i].renderable);
    _setStatesForElement(element);

    Shader::id shaderId = element.shaderId();
#if !STATIC_SHADERS
    if (intro.debugRenderMode != 0)
    {
      if (intro.debugRenderMode == 1)
	shaderId = Shader::debugWhiteLight;
      if (intro.debugRenderMode == 2)
	shaderId = Shader::debugNormals;
      if (intro.debugRenderMode == 3)
	shaderId = Shader::debugLight;
    }
#endif // !STATIC_SHADERS

    // Uniforms spécifiques au shader de l'élément
    Shader::setUniform1i(shaderId, Shader::objectId, element.id());
    Shader::setUniform1f(shaderId, Shader::shininess, element.shininess());
    Shader::setUniform1f(shaderId, Shader::trans, 0.f);
    if (element.customSetUniform() != NULL)
      element.customSetUniform()(shaderId, element.id());
    //Shader::setUniform1f(shaderId, Shader::textureFade, 0);
    // FIXME : texture fade
    // Shader::setUniform1f(shaderId, Shader::textureFade, t.textureFade);

    _setMotionInfo(shaderId, *(_renderables[i].owner), oldCameraMatrix);

    setToModelView(cameraMatrix * _renderables[i].owner->globalTransform());
    Light::placeLights(_renderables[i].owner->globalTransform());

    glDrawArrays(element.primitiveType(), 0, element.numberOfVertices());
  }

#if DEBUG
  // Test de physique
  //glColor3ub(255, 0, 0);
  //phy_draw();
  //glColor3ub(255, 255, 255);
#endif

  oglDisableVertexAttribArray(Shader::vTan);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
//   glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
}

#if DEBUG

void RenderList::renderObject(VBO::id vboId, Shader::id shaderId, Texture::id textureId)
{
  glCullFace(GL_BACK);

  glEnableClientState(GL_VERTEX_ARRAY);
  glEnableClientState(GL_NORMAL_ARRAY);
//   glEnableClientState(GL_COLOR_ARRAY);
  glEnableClientState(GL_TEXTURE_COORD_ARRAY);
  oglEnableVertexAttribArray(Shader::vTan);

  Shader::list[shaderId].use();
  Texture::list[textureId].use(Texture::albedo);
  Texture::list[Texture::flatBump].use(Texture::normal);
  Texture::list[Texture::none].use(Texture::material);
  VBO::list[vboId].use();

  Shader::setUniform1f(shaderId, Shader::textureFade, 0);

  matrix4 modelView;
  getFromModelView(modelView);
  Shader::setUniformM4fv(shaderId, Shader::oldModelView, modelView.m);

  glDrawArrays(GL_QUADS, 0, Cube::numberOfVertices);

  oglDisableVertexAttribArray(Shader::vTan);
  glDisableClientState(GL_TEXTURE_COORD_ARRAY);
//   glDisableClientState(GL_COLOR_ARRAY);
  glDisableClientState(GL_NORMAL_ARRAY);
  glDisableClientState(GL_VERTEX_ARRAY);
}

#endif // DEBUG
