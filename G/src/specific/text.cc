
#include "array.hxx"
#include "interpolation.hxx"
#include "intro.hh"
#include "materials.hh"
#include "mesh/mesh.hh"
#include "mesh/meshpool.hh"
#include "mesh/pave.hh"
#include "node.hxx"
#include "renderable.hxx"
#include "shaderprogram.hh"
#include "texgen/texture.hh"
#include "textureid.hh"
#include "textures.hh"
#include "textureunit.hxx"
#include "timing.hh"
#include "tweakval.hh"
#include "vbos.hh"

#include "sys/msys_glext.h"

#if DEBUG
#include <stdio.h>
#endif

namespace Text
{
  struct Message {
    int start;
    int duration;
    float xpos;
    float ypos;
    float zpos;
    float angle;
    VBO::id vbo;
    char* text;
  };
  Array<Message> messages;

  Texture::Channel * makeText(char * text)
  {
    Shader::Program::unUse();
    glEnable(GL_TEXTURE_2D);

    Texture::Unit burnt;
    burnt.setRenderTexture(_TV(512), _TV(256), false, true,
			   GL_RGBA, GL_RGBA, GL_UNSIGNED_BYTE,
			   GL_CLAMP, 8.f
			   DBGARG("Text"));

    glViewport(0, 0, burnt.width(), burnt.height());
    Camera::orthoProj(burnt.width(), burnt.height());

    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);

    intro.sticker->selectFont(_TV("Palatino Linotype"), _TV(48), _TV(400), TRUE);
    intro.sticker->print(text, _TV(0.f), _TV(100.f), 255, 255, 255);

    burnt.renderToTexture();

    // FIXME : on pourrit quand même le loading :-(
    // --------------------------------------
    glViewport(0, 0, intro.xres, intro.yres);
    glClear(GL_COLOR_BUFFER_BIT);
    // --------------------------------------

    unsigned char * r = NULL;
    unsigned char * g = NULL;
    unsigned char * b = NULL;
    unsigned char * a = NULL;
    burnt.dumpFromGL(&r, &g, &b, &a);

    return new Texture::Channel(burnt.width(), burnt.height(), a);
  }

  void generateMeshFromTexture(VBO::id vboid, Texture::Channel * text)
  {
    int count = 0; // pour le debug
    Mesh::MeshStruct res(655360);
    Mesh::MeshStruct & square = Mesh::getTempMesh();
    Mesh::Pave(_TV(1.f), _TV(1.f), _TV(0.f)).generateMesh(square, Mesh::pave_back);

    square.translate(_TV(0.f), _TV(0.f), 0.f);
    int dj = _TV(1), di = _TV(1);
    for (int j = 0; j < (int) text->Height(); j += dj)
      for (int i = 0; i < (int) text->Width(); i += di)
      {
        if ((*text)[text->UnsecureGetIndex(i, j)] < 0.1f)
          continue;

        count++;
        square.setId(j * 1000.f + i);
        res.add(square);
      }
    VBO::setupData(vboid, res.vertices DBGARG("text"));
  }

  // Progression pour l'animation du texte
  static float computeTransition(int objid)
  {
    int start = messages[objid].start;
    int length = 4000;
    return smoothStep(start, start + length, intro.now.story);
  }

  void init()
  {
    IFDBG(if (!intro.initDone))
      messages.init(20);
    messages.empty();

    // Mis dans une fonction pour pouvoir utiliser _TV
    Message list[] = {
      { _TV(54200), _TV(8000), _TV( -45.0f), _TV(-5.0f), _TV(5100.f), _TV(0.f), NEW_VBO(lyrics11), _TV("I decided") },
      { _TV(58100), _TV(8000), _TV(  -6.0f), _TV( 7.0f), _TV(6050.f), _TV(0.f), NEW_VBO(lyrics12), _TV("to be Me") },
      { _TV(60900), _TV(8000), _TV(-170.0f), _TV( 0.0f), _TV(6900.f), _TV(0.f), NEW_VBO(lyrics13), _TV("apologising") },
      { _TV(63000), _TV(8000), _TV( 170.0f), _TV( 0.0f), _TV(7600.f), _TV(0.f), NEW_VBO(lyrics14), _TV("disguising") },
      { _TV(65500), _TV(8000), _TV( -63.0f), _TV(-6.0f), _TV(8300.f), _TV(0.f), NEW_VBO(lyrics16), _TV("HARD") },
      { _TV(71000), _TV(8000), _TV(-200.0f), _TV( 0.0f), _TV(9700.f), _TV(0.f), NEW_VBO(lyrics15), _TV("no-one") },
      { _TV(74800), _TV(8000), _TV( -35.0f), _TV(-4.0f), _TV(10850.f),_TV(0.f), NEW_VBO(lyrics17), _TV("my Own way") },


      // Textes de fin
      { _TV(259500), _TV(5000), _TV(100.0f), _TV(-45.0f), _TV(1170.f),_TV(90.f),NEW_VBO(lyrics21), _TV("cake") },

      { _TV(265000), _TV(5000), _TV( 40.0f), _TV(  0.0f), _TV( 600.f),_TV(0.f), NEW_VBO(lyrics22), _TV("Ha!  Ha!  Ha!  Ha!") },
      { _TV(265400), _TV(5000), _TV(  0.0f), _TV(-10.0f), _TV( 800.f),_TV(0.f), NEW_VBO(lyrics23), _TV("Ha!    Ha!    Ha!") },
      { _TV(265800), _TV(5000), _TV(  0.0f), _TV(-20.0f), _TV( 900.f),_TV(0.f), NEW_VBO(lyrics24), _TV("Ha!   Ha!   Ha!   Ha!") },

      { _TV(268500), _TV(5000), _TV(-160.0f), _TV(-5.0f), _TV(1900.f),_TV(0.f), NEW_VBO(lyrics25), _TV("Nothing") },
      { _TV(269100), _TV(5000), _TV(-100.0f),_TV(-10.0f), _TV(2100.f),_TV(0.f), NEW_VBO(lyrics26), _TV("left") },

      { _TV(272950), _TV(5050), _TV(-60.0f), _TV(-50.0f), _TV(2920.f),_TV(0.f), NEW_VBO(lyrics27), _TV("BURN") },
    };

    for (int i = 0; i < ARRAY_LEN(list); i++)
      messages.add(list[i]);
  }

  Node * create(date startDate, date endDate)
  {
    IFDBG(if (intro.initDone)) init();
    Node * root = Node::New(startDate, endDate);

    for (int i = 0; i < messages.size; i++)
    {
      glLoadIdentity();
      glTranslatef(messages[i].xpos, messages[i].ypos, messages[i].zpos);
      glRotatef(messages[i].angle, 0.f, 1.f, 0.f);
      Node * node = Node::New(messages[i].start, messages[i].start + messages[i].duration);
      Renderable text(Material::text3D, messages[i].vbo);
      text.setId(i);
      text.setTransitionFct(computeTransition);
      node->attachRenderable(text);
      node->attachToNode(root);
    }
    return root;
  }

  void generateMeshes()
  {
    Mesh::clearPool();
    init();

    for (int i = 0; i < messages.size; i++)
    {
      Texture::Channel * text = makeText(messages[i].text);
      generateMeshFromTexture(messages[i].vbo, text);
      delete text;
    }
  }
}
