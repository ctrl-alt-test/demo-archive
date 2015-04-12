//
// Canon Zeus
//

#include "zeus.hh"

#include "sys/msys.h"
#include <GL/gl.h>

#include "anim.hh"
#include "basicTypes.hh"
#include "gear.hh"
#include "lsystem.hh"
#include "lsystemrule.hh"
#include "timing.hh"

namespace Zeus
{
  /*
  static Anim::Anim * lightningForwardAnim = NULL;
  static Anim::Anim * lightningBackwardAnim = NULL;
  */

  void addLightning(RenderList & renderList, date boltDate)
  {
    const date firstDuration = 200;
    const date secondDuration = 500;
    const int firstPropagation = 2;
    const int secondPropagation = 0;
    /*
    if (NULL == lightningBackwardAnim)
    {
      lightningForwardAnim = new Anim::Anim(4, firstDuration);
      lightningBackwardAnim = new Anim::Anim(4, secondDuration);

      glPushMatrix();
      glLoadIdentity();
      Anim::Keyframe growStart(0);
      Anim::Keyframe growOver(0.1f, 2.f);
      Anim::Keyframe growMiddle(0.2f);
      Anim::Keyframe growEnd(1.f, 0);
      glPopMatrix();

      lightningForwardAnim->add(growStart);
      lightningForwardAnim->add(growOver);
      lightningForwardAnim->add(growMiddle);
      lightningForwardAnim->add(growEnd);
      lightningForwardAnim->createCache();

      lightningBackwardAnim->add(growStart);
      lightningBackwardAnim->add(growOver);
      lightningBackwardAnim->add(growMiddle);
      lightningBackwardAnim->add(growEnd);
      lightningBackwardAnim->createCache();
    }
    */

    const char * X = "--X++++X[F]->-X[G]";
    const char * F = "+>+&FGF--^X";
    const char * G = "-^-FGF+&+F";
    const char * H = "H";

    const int iter = 4;
    const float angle = 14.f;
    const float reduction = 0.5f;
    const float forward = 2.5f;
    const int nb_cubes = 1;

    LSystem::System l(renderList, boltDate + firstDuration, boltDate,
		      Shader::texture, Anim::lightningForward,
		      Texture::thunderBolt,
		      angle, reduction, forward, nb_cubes,
		      firstPropagation, firstPropagation);
    LSystem::state lightning = l.iterate(X, F, G, H, iter);

    glPushMatrix();
    glTranslatef(0, 7.f, 0);
    glRotatef(0.1f * (msys_rand() % 3600), 0, 1.f, 0);
    glTranslatef(0, 5.f, 0);
    glRotatef(0.1f * (msys_rand() % 3600), 0, 0, 1.f);
    glScalef(0.05f, 0.05f, 0.05f);
    l.addStateToList(lightning);

    l.setAnimation(Anim::lightningBackward);
    l.setPropagation(boltDate + firstDuration + 200,
		     boltDate + firstDuration + 200 + secondDuration,
		     secondPropagation, secondPropagation);
    l.addStateToList(lightning);
    glPopMatrix();

    msys_mallocFree(lightning);
  }

  void addToList(RenderList & renderList, date birthDate, date deathDate)
  {
    const date lightningStartDate = birthDate + 5000;
    const date lightningEndDate = birthDate + 16000;
    const date warmUpDate = birthDate + 10000;
    const date warmUpEndDate = warmUpDate + 10000;

    // Tronc

    //
    // FIXME : tronc
    //

    // Cage

//     glPushMatrix();
//     glTranslatef(0, 100.f, 0);
//     Gear::addToList(renderList, deathDate, 0, 18, 18, 2, 1, 0.1f);
//     glTranslatef(0, 4.f, 0);
//     Gear::addToList(renderList, deathDate, 0, 18, 18, 20, 9, 0.1f);
//     glTranslatef(0, 40.f, 0);
//     Gear::addToList(renderList, deathDate, 0, 18, 18, 10, 1, 0.1f);
//     glPopMatrix();

    // Coeur
    glPushMatrix();
    glScalef(0.8f, 0.8f, 0.8f);
    glTranslatef(0, 20.f, 0);
    Gear::addToList(renderList, birthDate, deathDate, deathDate, Anim::zeusHeart, 6, 6, 15, 1, 0);
    glPopMatrix();

    // Satellite
    glPushMatrix();
//     glScalef(0.5f, 0.5f, 0.5f);
    glTranslatef(0, 6.f, 0);
    Gear::addToList(renderList, birthDate, deathDate, deathDate, Anim::zeusSatellite, 6, 6, 6, 3, 0.2f);
    glPopMatrix();

    // Bras
    glPushMatrix();
    glScalef(0.4f, 0.4f, 0.4f);
    glTranslatef(0, 35.f, 0);
    Gear::addToList(renderList, birthDate, deathDate, deathDate, Anim::zeusArm1, 8, 8, 20, 1, 0.1f);
    glPopMatrix();

    glPushMatrix();
    glScalef(0.4f, 0.4f, 0.4f);
    glTranslatef(0, 20.f, 0);
    Gear::addToList(renderList, birthDate, deathDate, deathDate, Anim::zeusArm2, 6, 6, 30, 1, 0.1f);
    glPopMatrix();

    glPushMatrix();
    glScalef(0.25f, 0.25f, 0.25f);
    glTranslatef(0, 12.f, 0);
    Gear::addToList(renderList, birthDate, deathDate, deathDate, Anim::zeusInner, 4, 4, 40, 1, 0);
    glPopMatrix();

    // Doigt
    glPushMatrix();
    glScalef(0.2f, 0.2f, 0.2f);
    Gear::addToList(renderList, birthDate, deathDate, deathDate, Anim::zeusFinger, 2, 2, 35, 1, 0);
    glPopMatrix();

    // Vortex
    glPushMatrix();
    glScalef(0.1f, 0.1f, 0.1f);

    Gear::addToList(renderList, warmUpDate, deathDate, deathDate, Anim::zeusVortex1, 16, 16, 1, 4, 0, Shader::default, Texture::highParticle);
    Gear::addToList(renderList, warmUpDate, deathDate, deathDate, Anim::zeusVortex2, 22, 22, 1, 6, 0, Shader::default, Texture::highParticle);
    Gear::addToList(renderList, warmUpDate, deathDate, deathDate, Anim::zeusVortex3, 28, 28, 1, 8, 0, Shader::default, Texture::highParticle);
    Gear::addToList(renderList, warmUpDate, deathDate, deathDate, Anim::zeusVortex4, 34, 34, 1, 10, 0, Shader::default, Texture::highParticle);
    Gear::addToList(renderList, warmUpDate, deathDate, deathDate, Anim::zeusVortex5, 40, 40, 1, 12, 0, Shader::default, Texture::highParticle);
    glPopMatrix();

    // Éclairs
    const unsigned int numberOfLightnings = 30;
    for (unsigned int i = 0; i < numberOfLightnings; ++i)
    {
      const float step = msys_powf((i / (float)numberOfLightnings), 1.5f);
      const date lightningDate = lightningStartDate + (date)((lightningEndDate - lightningStartDate) * step);
      addLightning(renderList, lightningDate);
    }
  }
}
