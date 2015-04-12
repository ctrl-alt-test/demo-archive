//
// Render list
//

#ifndef		RENDER_LIST_HH
# define	RENDER_LIST_HH

#include "basicTypes.hh"
#include "renderable.hh"
#include "shaderid.hh"
#include "textureid.hh"
#include "vector.hh"

class RenderList : public Array<Renderable>
{
public:
  RenderList(int max);
  ~RenderList();

  void moveInvisibleAtEnd();
  void sort();
  void fossoyeur(date renderDate);
  void renderSkyBox(date renderDate, const vector3f & cameraPosition);
  void renderSpinningCube(date renderDate);
  void render(date renderDate);

#if DEBUG

  static void RenderList::renderSingleCube(Shader::id shaderId, Texture::id textureId);

#endif // DEBUG

private:
  int _lastVisible;
  GLuint _fbo;
};

#endif		// RENDER_LIST_HH
