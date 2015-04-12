//
// Monstre entre le L-System et la forêt
//

#include "ramp.hh"
#include "sys/msys.h"
#include <GL/gl.h>

#include "animid.hh"
#include "camera.hh"
#include "lsystem.hh"
#include "lsystemrule.hh"
#include "renderable.hh"
#include "spline.h"
#include "textures.hh"
#include "timing.hh"

#define START_DATE	rampStartDate
#define END_DATE	rampEndDate
#define CUBE_JOURNEY	36000

#define CUBE_START	(START_DATE - 300)

namespace Ramp
{

  static const int rampIncrement = 30;

  static int count = 0;
  static RenderList *render = 0;

  static const char* l_x1 = "X[++G]>[-^F]&F<G+[^--H]H";
  static const char* l_x2 = "X>&F<G+H";
  static const char* l_f  = "FX";
  static const char* l_g  = "-GX";
  static const char* l_h  = "H+G";

  static const int iter = 7;
  static const float angle = 18.f;
  static const float reduction = 0.9f;
  static const float forward = 1.2f;
  static const unsigned char nb_cubes = 1;

  // Affichage d'un noeud du L-system
  static void renderNode(date birth, date death, unsigned depth)
  {
    count++;
    Renderable node(min(death, END_DATE), Shader::ramp, birth);
    node.setId(count);
    node.setAnimation(Anim::rampNode);
    render->add(node);
  }

  // Affichage Du cube, qui suit le L-system
  static void theCube(RenderList & renderList)
  {
    Renderable cube(END_DATE, Shader::color, START_DATE, 1.5f,
		    Cube::numberOfVertices, VBO::theCube);
    cube.setAnimation(Anim::rampCube);
    renderList.add(cube);
  }

  // Affichage du Cube en pleine transition
  static void addTransition(RenderList & renderList)
  {
    Renderable cube(rampTransitionEndDate, Shader::color, rampTransitionStartDate, 1.5f,
		    Cube::numberOfVertices, VBO::theCube);
    cube.setAnimation(Anim::rampTransition);
    renderList.add(cube);
  }

  // Le L-system
  static void addRamp(RenderList & renderList)
  {
    LSystem::System * l = NULL;
    LSystem::state userSystem = NULL;

    glPushMatrix();
    l = new LSystem::System(renderList, START_DATE + 8000, START_DATE,
			    Shader::ramp, Anim::rampNode, Texture::none,
			    angle, reduction, forward, nb_cubes,
			    rampIncrement, rampIncrement, renderNode);
    userSystem = l->iterate(l_x1, l_f, l_g, l_h, iter);
    l->addStateToList(userSystem);

    glPopMatrix();
  }

  void changeLight(date renderDate,
		   GLfloat * ambient,
		   GLfloat * diffuse,
		   GLfloat * specular,
		   GLfloat * position)
  {
    ambient[0] = 0.1f;
    ambient[1] = 0.1f;
    ambient[2] = 0.1f;
    ambient[3] = 1.f;

    diffuse[0] = 0.9f;
    diffuse[1] = 0.9f;
    diffuse[2] = 0.9f;
    diffuse[3] = 0.6f;
  }

  void create(RenderList & renderList)
  {
    render = &renderList;
    addRamp(renderList);
    theCube(renderList);
    addTransition(renderList);
  }
}
