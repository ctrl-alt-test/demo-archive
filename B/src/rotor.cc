//
// Moteur de l'usine
//

#include "rotor.hh"

#include "sys/msys.h"
#include <GL/gl.h>

#include "animid.hh"

namespace Rotor
{
  const date pistonCycleDuration = 800;
  extern const float pistonDebatement = 2.f;

  void addToList(RenderList & renderList, date birthDate, date deathDate, date period,
		 unsigned int length, Shader::id shaderId)
  {
    // Pistons
    glPushMatrix();
    glRotatef(45.f, 1.f, 0, 0);
    glScalef(0.5f, 0.5f, 0.5f);
    for (unsigned int i = 0; i < length; ++i)
    {
      //
      // FIXME : factoriser
      //

      glPushMatrix();
      glTranslatef(float(2 * i), 0.5f, 0);
      {
	Renderable head(deathDate, shaderId, birthDate);
	head.setAnimation(Anim::rotorPistonCycle);
	head.setAnimationStepShift((i % 4) / 4.f);
	renderList.add(head);
      }
      glTranslatef(0, 1.0f, 0);
      for (unsigned int j = 0; j < 20; ++j)
      {
	Renderable block(deathDate, shaderId, birthDate, 0.25f);
	block.setAnimation(Anim::rotorPistonCycle);
	block.setAnimationStepShift((i % 4) / 4.f);
	renderList.add(block);
	glTranslatef(0, 0.25f, 0);
      }
      glPopMatrix();


      glPushMatrix();
      glRotatef(-90.f, 1.f, 0, 0);
      glTranslatef(float(2 * i), 0.5f, 0);
      {
	Renderable head(deathDate, shaderId, birthDate);
	head.setAnimation(Anim::rotorPistonCycle);
	head.setAnimationStepShift((i % 4) / 4.f + 0.5f);
	renderList.add(head);
      }
      glTranslatef(0, 1.0f, 0);
      for (unsigned int j = 0; j < 20; ++j)
      {
	Renderable block(deathDate, shaderId, birthDate, 0.25f);
	block.setAnimation(Anim::rotorPistonCycle);
	block.setAnimationStepShift((i % 4) / 4.f + 0.5f);
	renderList.add(block);
	glTranslatef(0, 0.25f, 0);
      }
      glPopMatrix();
    }
    glPopMatrix();

    // Bielles
  }
}
