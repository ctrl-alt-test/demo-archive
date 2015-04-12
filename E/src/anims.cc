//
// Toutes les animations utilisées
//

#include "animid.hh"
#include "anims.hh"
#include "lsystem.hh"
#include "spline.h"
#include "timing.hh" // FIXME : Virer
#include "renderlist.hh" // FIXME : dû à lsystem

namespace Anim
{
  Anim * list = NULL;

  static void buildExample();

  typedef void(*animBuilder)();

  const animBuilder builders[] =
    {
      buildExample,
    };

#if DEBUG
  void checkAllAnimsAreLoaded()
  {
    for (unsigned int i = 1; i < numberOfAnims; ++i)
      if (list[i].size < 2)
      {
	assert(false);
      }
  }
#endif

  void loadAnims(Loading::ProgressDelegate * pd, int low, int high)
  {
    START_TIME_EVAL;

    const unsigned int numberOfBuilders = sizeof(builders) / sizeof(animBuilder);
    const int ldStep = (high - low) / (numberOfBuilders + 1);
    int currentLd = low;

    list = new Anim[numberOfAnims];

    for (unsigned int i = 0; i < numberOfBuilders; ++i)
    {
      DBG("loading anim %u...", i);
      builders[i]();
      pd->func(pd->obj, currentLd); currentLd += ldStep;
      OGL_ERROR_CHECK("loadAnims i = %u", i);
    }

    pd->func(pd->obj, high);

#if DEBUG
    checkAllAnimsAreLoaded();
#endif

    END_TIME_EVAL("Anims creation");
  }

  // FIXME : destruction des animations


  // ==========================================================================

  static void buildExample()
  {
    /*
    Anim & anim = list[example];
    anim.setAnim(3, 2000, 0);

    glPushMatrix();
    glLoadIdentity();
    anim.add(Keyframe(0));
    glTranslatef(0, 1.f, 0);
    anim.add(Keyframe(0.5f));
    glLoadIdentity();
    anim.add(Keyframe(1.f));
    glPopMatrix();
    */
  }
}
