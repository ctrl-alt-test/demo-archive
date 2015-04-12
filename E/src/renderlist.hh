//
// Render list
//

#ifndef		RENDER_LIST_HH
# define	RENDER_LIST_HH

#include "array.hh"
#include "basicTypes.hh"
#include "renderable.hh"
#include "shaderid.hh"
#include "textureid.hh"
#include "transformation.hh"
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
  void render(date renderDate, const Transformation & oldCamera);

#if DEBUG

  static void RenderList::renderSingleCube(Shader::id shaderId = Shader::none,
					   Texture::id textureId = Texture::none);

#endif // DEBUG

private:
  int _lastVisible;
  GLuint _fbo;
};

#endif		// RENDER_LIST_HH
