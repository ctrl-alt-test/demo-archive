//
// Transition entre la forêt et l'écran
//

#include "rope.hh"

#include "sys/msys.h"
#include <GL/gl.h>

#include "camera.hh"
#include "interpolation.hh"
#include "lsystem.hh"
#include "lsystemrule.hh"
#include "renderable.hh"
#include "spline.h"
#include "textures.hh"
#include "timing.hh"

#define START_DATE	elevationStartDate
#define END_DATE	elevationEndDate

#define CUBE_START	(START_DATE - 500)
#define CUBE_JOURNEY	30000

#define ROPE_START      (START_DATE - 5000)

#define CAMERA_START	(CUBE_START + 2000)
#define CAMERA_JOURNEY	34500
#define CAMERA_FOV	60.f

#define ROPE_INCREMENT	140

namespace Rope
{
  static RenderList *render = 0;
  /*
  static Anim::Anim *animNode = 0;
  */

  static const char* l_x1 = "X[&F]F[+++F]F";
  static const char* l_x2 = "XFF";
  static const char* l_f  = "F+^G";
  static const char* l_g1 = "G[&&X]-&H";
  static const char* l_g2 = "G-&H";
  static const char* l_h  = "H-^F";

  static const int iter = 7;
  static const float angle = 10.f;
  static const float reduction = 0.7f;
  static const float forward = 1.5f;
  static const unsigned char nb_cubes = 1;


  // Affichage d'un noeud du L-system
  static void renderNode(date birth, date death, unsigned)
  {
    Renderable node(death, Shader::rope, birth);
    node.setTextures(Texture::moss,
		     Texture::mossBump,
		     Texture::mossSpecular);
    node.setAnimation(Anim::ropeNode);
    node.setId(birth);
    render->add(node);
  }

  // Trajectoire de la camera (a partir d'un L-system)
  Anim::Anim * newCameraAnim()
  {
    RenderList ropeList(10000);
    {
      LSystem::System * l =
	new LSystem::System(ropeList, END_DATE, START_DATE,
			    Shader::none, Anim::none, Texture::none,
			    angle, reduction, forward, nb_cubes,
			    2, 0);
      LSystem::state expended = l->iterate(l_x2, l_f, l_g2, l_h, iter);
      l->addStateToList(expended);
      msys_mallocFree(expended);
      delete l;
    }

    const int skip = 40;
    const date lastDate = ropeList.last().birthDate();
    Anim::Anim * anim = new Anim::Anim(1 + lastDate / skip, CAMERA_JOURNEY);

    const int forwardTarget = 9;
    float lastAngle1 = 0;
    float lastAngle2 = 0;
    for (int i = 0; i < ropeList.size - forwardTarget; i += skip)
    {
      glPushMatrix();
      float mtx[16];
      msys_memcpy(mtx, ropeList.elt[i].viewMatrix(), sizeof mtx);

      const float *lookMtx = ropeList.elt[i + forwardTarget].viewMatrix();
      vector3f look = {lookMtx[12], lookMtx[13], lookMtx[14]};
      Camera::lookAt(mtx, look);
      glLoadIdentity();
      //const float angle1 = lastAngle1 + 40.f * msys_sfrand();
      //lastAngle1 = angle1;
      //const float dist = 5.f + 5.f * msys_frand();
      float angle1 = 45.f;
      float dist = 2.f;
      //glTranslatef(dist * msys_sinf(angle1), dist * msys_cosf(angle1), 0.f);
      glMultMatrixf(mtx);
      //const float angle2 = lastAngle2 + 10.f * msys_sfrand();
      //lastAngle2 = angle2;
      //glRotatef(angle2, 0, 0, 1.f);
      glTranslatef(0, 0.2f, 0.2f);
      anim->add(Anim::Keyframe((float)i / (ropeList.size - 1), CAMERA_FOV, true));
      glPopMatrix();
    }
    return anim;
  }

  // Affichage Du cube, qui suit le L-system
  static void theCube(RenderList & renderList)
  {
    Renderable cube(END_DATE, Shader::color, CUBE_START,
		    1.f, Cube::numberOfVertices, VBO::theCube);
    cube.setAnimation(Anim::ropeCube);
    renderList.add(cube);
  }

  // Le L-system
  static void addRope(RenderList & renderList)
  {
    LSystem::System * l =
      new LSystem::System(renderList, END_DATE, ROPE_START,
			  Shader::rope, Anim::ropeNode, Texture::moss,
			  angle, reduction, forward, nb_cubes,
			  ROPE_INCREMENT, 0, renderNode);
    LSystem::state expended = l->iterate(l_x1, l_f, l_g1, l_h, iter);

    glPushMatrix();
    l->addStateToList(expended);
    glPopMatrix();

    msys_mallocFree(expended);
    delete l;
  }

  void create(RenderList & renderList)
  {
    render = &renderList;
    glPushMatrix();
    glTranslatef(4.f+43.f, -3.f, 4.f+55.f);
    addRope(renderList);
    theCube(renderList);
    glPopMatrix();
  }

  void changeLight(date renderDate,
		   GLfloat * ambient,
		   GLfloat * diffuse,
		   GLfloat * specular,
		   GLfloat * position)
  {
    const float weight = smoothStep((float)elevationStartDate, (float)elevationStartDate + 5000.f, (float)renderDate);

    // Lumière de l'élévation (transition depuis celle de la forêt)
    ambient[0] = mix(0.7f * 1.f  , 0.5f, weight);
    ambient[1] = mix(0.7f * 0.87f, 0.5f, weight);
    ambient[2] = mix(0.7f * 0.83f, 0.5f, weight);
    ambient[3] = mix(        0.9f, 0.9f, weight);

    diffuse[0] = mix(0.3f, 0.5f, weight);
    diffuse[1] = mix(0.3f, 0.5f, weight);
    diffuse[2] = mix(0.3f, 0.5f, weight);
    diffuse[3] = mix(0.7f, 0.5f, weight);
  }
}
