
#include "algebra/vector3.hh"
#include "fboid.hh"
#include "fbos.hh"
#include "intro.hh"
#include "shaders.hh"
#include "textureid.hh"
#include "textures.hh"
#include "variable.hh"
#include "vbos.hh"

#include "tweakval.hh"
#include "sys/msys_glext.h"

namespace Storyboard
{
  Texture::id currentImage()
  {
    const int pics = Texture::storyboardEnd - Texture::storyboardStart;
    const int times[pics] = {
      _TV(     0),
      _TV(  2000),
      _TV( 12000),
      _TV( 17000),
      _TV( 22000),
      _TV( 25000),
      _TV( 32000),
      _TV( 40000),
      _TV( 47000),
      _TV( 86000),
      _TV(101000),
      _TV(114000),
      _TV(116000),
      _TV(126000),
      _TV(135000),
      _TV(169000),
      _TV(175000),
      _TV(192000),
      _TV(207000),
      _TV(222000),
      _TV(237000),
      _TV(238000),
      _TV(246000),
      _TV(254000),
      _TV(278000),
      _TV(294000)
    };

    Texture::id textureId = Texture::black;

    if (_TV(0))  // black background
      return textureId;

    for (int i = 0; i < pics && times[i] <= (int)intro.now.youtube; i++)
    {
      textureId = (Texture::id)(Texture::storyboardStart + i);
    }

    return textureId;
  }

  void renderFullScreen(FBO::id dest)
  {
    // On ne veut pas écrire dans la velocity map
    FBO::unUse();
    FBO::list[dest].use(true);

    // On va pourrir les matrices, donc on les sauve avant
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();

    // Même principe que applyPostProcessing dans intro.cpp
    Camera::orthoProj(1.f, 1.f);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_LIGHTING);
    glCullFace(GL_BACK);

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    // Même principe que _renderFullScreenImage dans fbos.cc
    VBO::Element::unUse();
    Shader::list[Shader::noPost].use();
    Texture::Unit& src = Texture::list[currentImage()];
    src.use(Texture::color);

    const float x = 2.f;//src.renderWidth() * 2.f / src.width();
    const float y = 2.f;//src.renderHeight() * 2.f / src.height();
    const float vertices[] =
      {
	0, 0, 0,   0,
	x, 0, 2.f, 0,
	0, y, 0, 2.f,
      };

    glVertexPointer(2, GL_FLOAT, 4 * sizeof(float), vertices + 2);
    glTexCoordPointer(2, GL_FLOAT, 4 * sizeof(float), vertices);

    glDrawArrays(GL_TRIANGLES, 0, 3);

    glDisableClientState(GL_TEXTURE_COORD_ARRAY);
    glDisableClientState(GL_VERTEX_ARRAY);

    // On rétablit les états
    glEnable(GL_DEPTH_TEST);

    // On rétablit les matrices
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();

    // On rétablit l'écriture dans toutes les MRT
    FBO::unUse();
    FBO::list[dest].use();
  }
}
