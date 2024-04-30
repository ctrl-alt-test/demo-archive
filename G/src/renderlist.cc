//
// Render list
//

#include "renderlist.hxx"

#include "anim.hh"
#include "anims.hh"
#include "array.hxx"
#include "mesh/cube.hh"
#include "fbos.hh"
#include "intro.hh"
#include "light.hh"
#include "algebra/matrix.hxx"
#include "material.hxx"
#include "node.hxx"
#include "renderable.hxx"
#include "shaderProgram.hh"
#include "shaders.hh"
#include "textures.hh"
#include "textureunit.hh"
#include "tweakval.hh"
#include "vbodata.hh"
#include "vbos.hh"
#include "physic_test.hh"

#include "sys/msys_debug.h"

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
static Shader::id _setStatesForElement(const Renderable & element)
{
  VBO::list[element.vboId()].use();

  const Material::Element & material = element.material();

  Texture::list[material.textureId()].use(Texture::albedo);
  Texture::list[material.texture2Id()].use(Texture::albedo2);
  Texture::list[material.bumpTextureId()].use(Texture::normal);
  Texture::list[material.specularTextureId()].use(Texture::material);

  Shader::id shaderId = material.shaderId();

#if !STATIC_SHADERS
  if (intro.debug.renderMode != releaseRender)
  {
    switch (intro.debug.renderMode)
    {
    case whiteLightRender: shaderId = Shader::debugWhiteLight; break;
    case normalsRender:    shaderId = Shader::debugNormals; break;
    case lightingRender:   shaderId = Shader::debugLight; break;
    }
  }
#endif // !STATIC_SHADERS

  Shader::uniforms[Shader::objectId].set(element.id());
  Shader::uniforms[Shader::shininess].set(element.material().shininess());
  Shader::uniforms[Shader::trans].set(element.transition());

  Shader::list[shaderId].use();

  return shaderId;
}

static void _setStatesForElementShadowRendering(const Renderable & element)
{
  VBO::list[element.vboId()].use();

  const Material::Element & material = element.material();

  // Normalement ça n'a pas trop de sens pour les ombres,
  // mais on ne sait pas comment c'est utilisé dans le shader.
  Texture::list[material.textureId()].use(Texture::albedo);
  Texture::list[material.texture2Id()].use(Texture::albedo2);
  Texture::list[material.bumpTextureId()].use(Texture::normal);
  Texture::list[material.specularTextureId()].use(Texture::material);

  const Shader::id shaderId = material.shadowShaderId();

  Shader::uniforms[Shader::objectId].set(element.id());
  Shader::uniforms[Shader::trans].set(element.transition());
  Shader::list[shaderId].use();

  return;
}

void RenderList::render(date renderDate, const Camera::Camera & oldCamera)
{
  OGL_ERROR_CHECK("RenderList::render, début");

  Shader::uniforms[Shader::time].set(renderDate);

  glCullFace(GL_BACK);
  vertex::activateRenderStates();

//   Texture::list[Texture::ambientMap].use(Texture::ambient);
  Texture::list[Texture::varianceShadowMap].use(Texture::shadow);

  matrix4 oldCameraMatrix = computeInvMatrix(oldCamera.t);
  matrix4 cameraMatrix;
  getFromModelView(cameraMatrix);
  Light::placeLights(cameraMatrix);

  for (int i = 0; i <= _lastVisible; ++i)
  {
    const Renderable & element = *(_renderables[i].renderable);
    Shader::uniforms[Shader::birthDate].set(_renderables[i].owner->birthDate());

    matrix4 oldModelView = oldCameraMatrix * _renderables[i].owner->oldTransform();
    Shader::uniforms[Shader::oldModelView].set(oldModelView);
    _setStatesForElement(element);

    const matrix4 & objectMatrix = _renderables[i].owner->globalTransform();
    setToModelView(cameraMatrix * objectMatrix);
    Light::placeProjector(objectMatrix);

    DBG_IF_FLAG(renderDrawcall) glDrawArrays(element.primitiveType(), 0, element.numberOfVertices());
  }

#if DEBUG
  // Test de physique
  //glColor3ub(255, 0, 0);
  //phy_draw();
  //glColor3ub(255, 255, 255);
#endif

  vertex::deactivateRenderStates();
}

void RenderList::renderShadows(date renderDate)
{
  OGL_ERROR_CHECK("RenderList::renderShadows, début");

  Shader::uniforms[Shader::time].set(renderDate);

  glCullFace(GL_BACK);
  vertex::activateRenderStates();

  matrix4 lightMatrix;
  getFromModelView(lightMatrix); // Point de vue de la lumière

  for (int i = 0; i <= _lastVisible; ++i)
  {
    const Renderable & element = *(_renderables[i].renderable);
    Shader::uniforms[Shader::birthDate].set(_renderables[i].owner->birthDate());
    _setStatesForElementShadowRendering(element);

    const matrix4 & objectMatrix = _renderables[i].owner->globalTransform();
    setToModelView(lightMatrix * objectMatrix);

    DBG_IF_FLAG(shadowMapDrawcall) glDrawArrays(element.primitiveType(), 0, element.numberOfVertices());
  }

  vertex::deactivateRenderStates();
}

#if DEBUG

void RenderList::renderObject(VBO::id vboId, Material::Element material)
{
  glCullFace(GL_BACK);

  vertex::activateRenderStates();

  VBO::list[vboId].use();
  Texture::list[material.textureId()].use(Texture::albedo);
  Texture::list[material.texture2Id()].use(Texture::albedo2);
  Texture::list[material.bumpTextureId()].use(Texture::normal);
  Texture::list[material.specularTextureId()].use(Texture::material);

  Shader::uniforms[Shader::shininess].set(material.shininess());
  matrix4 modelView;
  getFromModelView(modelView);
  Shader::uniforms[Shader::oldModelView].set(modelView);
  Shader::list[material.shaderId()].use();

  glDrawArrays(GL_QUADS, 0, Cube::numberOfVertices);

  vertex::deactivateRenderStates();
}

#endif // DEBUG
