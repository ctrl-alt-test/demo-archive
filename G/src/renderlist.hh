//
// Render list
//

#ifndef		RENDER_LIST_HH
# define	RENDER_LIST_HH

#include "array.hh"
#include "basicTypes.hh"
#include "camera.hh"
#include "node.hh"
#include "renderable.hh"

#if DEBUG

#include "material.hh"

#endif // DEBUG

class RenderList
{
public:
  static RenderList * createFromScene(Array<Node> & nodes);

  RenderList(int max);
  ~RenderList();

  int size() const;
  void add(Node & owner, const Renderable & renderable);

  void fossoyeur(date renderDate);
  void moveInvisibleAtEnd();
  void sort();

  void renderShadows(date renderDate);
  void render(date renderDate, const Camera::Camera & oldCamera);

#if DEBUG

  static void renderObject(VBO::id vboId, Material::Element material);

#endif // DEBUG

private:

#if DEBUG
public:
#endif // DEBUG

  void _generateFromTree(Node * tree);

  Array<RenderableContainer> _renderables;
  int _lastVisible;
  GLuint _fbo;
};

#endif		// RENDER_LIST_HH
