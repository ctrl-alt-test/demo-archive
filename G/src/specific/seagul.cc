
#include "algebra/vector2.hxx"
#include "algebra/vector3.hxx"
#include "array.hxx"
#include "interpolation.hxx"
#include "intro.hh"
#include "materials.hh"
#include "mesh/mesh.hh"
#include "mesh/meshpool.hh"
#include "node.hxx"
#include "randomness.hh"
#include "renderable.hh"
#include "textureid.hh"
#include "textures.hh"
#include "tweakval.hh"
#include "vbos.hh"

#include "sys/msys.h"

#define NUMSEAGULS 1000

namespace Seagul
{
  struct seagulControl
  {
    vector3f p0;
    float circleRadius;
    date circleDuration;
    date circleDateOffset;
    date flapDuration;
    date flapDateOffset;
  };
  Array<seagulControl> seagulControls;
  Array<float> textureData;

  void generateMesh()
  {
    Mesh::clearPool();

    const float exageration = _TV(3.f);

    //
    // 1-2 5---6
    // | | |  /
    // 0-3 4-7
    //
    // FIXME: pourquoi ça fait une aile gauche alors que ça devrait
    // être l'aile droite ?
    const vector2f points[] = {
      vector2f(_TV(0.0f), _TV(-0.35f)) * exageration,
      vector2f(_TV(0.0f), _TV( 0.00f)) * exageration,
      vector2f(_TV(0.6f), _TV( 0.05f)) * exageration,
      vector2f(_TV(0.6f), _TV(-0.30f)) * exageration,

      vector2f(_TV(0.0f), _TV(-0.30f)) * exageration,
      vector2f(_TV(0.0f), _TV( 0.05f)) * exageration,
      vector2f(_TV(0.8f), _TV( 0.02f)) * exageration,
      vector2f(_TV(0.53f), _TV(-0.16f)) * exageration,
    };

    Mesh::MeshStruct & wing = Mesh::getTempMesh();
    wing.generate2DShape(points, 4, true, true);
    wing.computeBaryCoord();
//     SETUP_VBO(seagulWing, wing);

    Mesh::MeshStruct & tip = Mesh::getTempMesh();
    tip.generate2DShape(points + 4, 4, true, true);
    tip.computeBaryCoord();
//     SETUP_VBO(seagulWingTip, tip);

    // La première mouette est à part
    Mesh::MeshStruct & seaguls = Mesh::getTempMesh();
    for (int i = 5; i < NUMSEAGULS; ++i)
    {
      wing.setId(i * 4.f);       seaguls.add(wing);
      tip.setId(i * 4.f + 1.f);  seaguls.add(tip);
      wing.setId(i * 4.f + 2.f); seaguls.add(wing);
      tip.setId(i * 4.f + 3.f);  seaguls.add(tip);
    }

    seaguls.setColor(Material::beautifulWhite);
    SETUP_VBO(seaguls, seaguls);

    Mesh::MeshStruct & singleSeagul = Mesh::getTempMesh();
    wing.setId(0.f); singleSeagul.add(wing);
    tip.setId(1.f); singleSeagul.add(tip);
    wing.setId(2.f); singleSeagul.add(wing);
    tip.setId(3.f); singleSeagul.add(tip);
    singleSeagul.setColor(Material::beautifulWhite);
    SETUP_VBO(seagul, singleSeagul);

    Mesh::MeshStruct & seagulGroup = Mesh::getTempMesh();
    for (int i = 1; i < 5; i++)
    {
      wing.setId(i * 4.f);       seagulGroup.add(wing);
      tip.setId(i * 4.f + 1.f); seagulGroup.add(tip);
      wing.setId(i * 4.f + 2.f); seagulGroup.add(wing);
      tip.setId(i * 4.f + 3.f); seagulGroup.add(tip);
    }
    seagulGroup.setColor(Material::beautifulWhite);
    SETUP_VBO(seagulGroup, seagulGroup);
  }

  void init()
  {
    seagulControls.init(NUMSEAGULS);
    textureData.init(NUMSEAGULS * 4 * 16);
  }

  Node * create(date startDate, date endDate, date wingAnimShift)
  {
    IFDBG(if (!intro.initDone))
      init();

    Rand rand;
    Node * node = Node::New(startDate, endDate);
    Renderable seaguls(Material::retroSeagul, VBO_(seaguls));
    node->attachRenderable(seaguls);

    seagulControls.size = NUMSEAGULS;
    // Les premières sont à part
    vector3f pos[] = {
      vector3f(_TV(0.f),   _TV(0.f),  _TV(0.f)),
      vector3f(_TV(-15.f), _TV(6.f),  _TV(-5.f)),
      vector3f(_TV(-11.f), _TV(3.f),  _TV(-3.f)),
      vector3f(_TV(8.f),   _TV(1.f),  _TV(-2.f)),
      vector3f(_TV(16.f),  _TV(4.f),  _TV(-6.f)),
    };
    for (int i = 0; i < 5; ++i)
    {
      seagulControl & sc0 = seagulControls[i];

      sc0.p0 = pos[i];

      sc0.circleRadius = _TV(0.f);
      sc0.circleDuration = _TV(0);
      sc0.circleDateOffset = _TV(15000);

      sc0.flapDuration = rand.igen(900, 1300);
      sc0.flapDateOffset = rand.igen() % sc0.flapDuration;
    }

    for (int i = 5; i < NUMSEAGULS; ++i)
    {
      seagulControl & sc = seagulControls[i];

      sc.p0 = vector3f(rand.fgen(-500.f, 500.f),
		       rand.fgen(5.f, 100.f),
		       rand.fgen(-200.f, 500.f));

      sc.circleRadius = rand.fgen(50.f, 150.f);
      sc.circleDuration = rand.igen(20000, 50000);
      sc.circleDateOffset = rand.igen() % sc.circleDuration;

      sc.flapDuration = rand.igen(900, 1300);
      sc.flapDateOffset = rand.igen() % sc.flapDuration;
    }

    return node;
  }

#undef _TV
#define _TV(x) x

  void circleAnimation(date d, int cycleDuration, float circleRadius)
  {
    const float progression = ((int)d % cycleDuration) / float(cycleDuration);

    glRotatef(360.f * progression, 0, 1.f, 0);
    glTranslatef(-circleRadius, 0, 0);
  }

  static float wingCycle(float x)
  {
    while (x < 0.f) x += 1.f;
    while (x > 1.f) x -= 1.f;
    const float midPoint = _TV(0.35f);
    return x < midPoint ? interpolate(x, 0.f, midPoint) : 1.f - interpolate(x, midPoint, 1.f);
  }

  void wingAnimation(date d, float direction, int cycleDuration)
  {
    // Cas particulier : mouettes au repos pendant les premières secondes
    const float idleCycles = (float)_TV(27);
    const float idlePosition = _TV(0.65f);
    const bool isIdle = float(d) / float(cycleDuration) < idleCycles + idlePosition;

    const float progression = isIdle ? idlePosition : ((int)d % cycleDuration) / float(cycleDuration);

    const float min = _TV(-15.f);
    const float max = _TV(45.f);
    const float angle = _TV(1) ?
      smoothMix(min, max, wingCycle(progression)) :
      mix(min, max, msys_sinf(1.57079632679489662f * wingCycle(progression)));
    glRotatef(angle, 0, 0, direction);
  }

  void wingTipAnimation(date d, float direction, int cycleDuration)
  {
    // Cas particulier : mouettes au repos pendant les premières secondes
    const float idleCycles = (float)_TV(27);
    const float idlePosition = _TV(0.3f);
    const bool isIdle = float(d) / float(cycleDuration) < idleCycles + idlePosition;

    const float progression = isIdle ? idlePosition : ((int)d % cycleDuration) / float(cycleDuration);

    const float progressionOffset = _TV(0.2f);
    const float min = _TV(8.f);
    const float max = _TV(-55.f);
    const float angle = _TV(1) ?
      smoothMix(min, max, wingCycle(progression + progressionOffset)) :
      mix(min, max, msys_sinf(1.57079632679489662f * wingCycle(progression)));
    glRotatef(angle, 0, 0, direction);
  }

  inline void leftWingAnimation(date d, int cycleDuration) { return wingAnimation(d, -1.f, cycleDuration); }
  inline void rightWingAnimation(date d, int cycleDuration) { return wingAnimation(d, 1.f, cycleDuration); }

  inline void leftWingTipAnimation(date d, int cycleDuration) { return wingTipAnimation(d, -1.f, cycleDuration); }
  inline void rightWingTipAnimation(date d, int cycleDuration) { return wingTipAnimation(d, 1.f, cycleDuration); }

  void textureAnimation(date d)
  {
    textureData.size = NUMSEAGULS * 4 * 16;
    float * p = textureData.elt;

//     msys_memset(data, 0, sizeof(data));

    for (int i = 0; i < NUMSEAGULS; ++i)
    {
//       if (i > 1) break;

      const seagulControl & sc = seagulControls[i];

      glLoadIdentity();
      glTranslatef(sc.p0.x, sc.p0.y, sc.p0.z);
      if (sc.circleDuration > 0)
      {
        circleAnimation(sc.circleDateOffset + d,
		        sc.circleDuration,
		        sc.circleRadius);
      }

      glPushMatrix();
      rightWingAnimation(sc.flapDateOffset + d,
			 sc.flapDuration);
      glGetFloatv(GL_MODELVIEW_MATRIX, p);
      p += 16;

      glTranslatef(1.8f, 0, 0);
      rightWingTipAnimation(sc.flapDateOffset + d,
			    sc.flapDuration);
      glGetFloatv(GL_MODELVIEW_MATRIX, p);
      p += 16;

      glPopMatrix();
      glRotatef(180.f, 0, 0, 1.f);
      leftWingAnimation(sc.flapDateOffset + d,
			sc.flapDuration);
      glGetFloatv(GL_MODELVIEW_MATRIX, p);
      p += 16;

      glTranslatef(1.8f, 0, 0);
      leftWingTipAnimation(sc.flapDateOffset + d,
			   sc.flapDuration);
      glGetFloatv(GL_MODELVIEW_MATRIX, p);
      p += 16;
    }

    Texture::Unit & texture = Texture::list[Texture::seagulAnimationsMap];
    texture.updateDynamicTexture(0, 0, 16/4, NUMSEAGULS * 4, GL_FLOAT, textureData.elt);
  }
}
