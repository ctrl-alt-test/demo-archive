//
// Toutes les animations utilisées
//

#include "anims.hh"

#include "animid.hh"
#include "bookshelf.hh"
#include "carousel.hh"
#include "clock.hh"
#include "drum.hh"
#include "musicbox.hh"
#include "node.hh"
#include "lsystem.hh"
#include "lousianne.hh"
#include "orgalame.hh"
#include "pascontent.hh"
#include "puppet.hh"
#include "spline.h"
#include "tweakval.h"
#include "workshop.hh"
#include "xylotrain.hh"

namespace Anim
{
  animFunc * list = NULL;
  Anim<Transformation> * nodeIndependantAnims = NULL;

  typedef void(*animBuilder)();

  static void nodeIndependantAnimation(const Node & node, date d)
  {
    Anim<Transformation> & animation = nodeIndependantAnims[node.animId()];
    Transformation t = animation.get(d);
    matrix4 animTransform = computeMatrix(t);

    // FIXME : c'est très laid comme façon de récupérer le résultat
    setToModelView(animTransform);
  }

#if DEBUG

  void checkAllAnimsAreLoaded()
  {
    for (unsigned int i = 1; i < numberOfAnims; ++i)
    {
      assert(list[i] != NULL);
      if (list[i] == nodeIndependantAnimation)
	assert(nodeIndependantAnims[i].hasFrames());
    }
  }

#endif

  // ==========================================================================

#if 0
  //
  // Exemple : rotation autour de y
  //
  // 3 points pour ne pas avoir d'ambiguité sur le sens de rotation,
  // un 4e pour boucler
  //
  static void buildExample1()
  {
    list[exmeple1] = nodeIndependantAnimation;
    Anim<Transformation> & anim = nodeIndependantAnims[example1];
    anim.setAnim(4, 0);

    Transformation t;
    glLoadIdentity();            getFromModelView(t); anim.add(0, t);
    glRotatef(120.f, 0, 1.f, 0); getFromModelView(t); anim.add(5000, t);
    glRotatef(120.f, 0, 1.f, 0); getFromModelView(t); anim.add(10000, t);
    glLoadIdentity();            getFromModelView(t); anim.add(15000, t);

    anim.createCache();
  }

  //
  // Exemple : mouvement de va et vient astable suivant x
  //
  static void buildExample2()
  {
    list[exemple2] = nodeIndependantAnimation;
    Anim<Transformation> & anim = nodeIndependantAnims[example2];
    anim.setAnim(5, 0);

    Transformation t;
    glLoadIdentity();        getFromModelView(t); anim.add(0, t);    anim.add(1000, t);
    glTranslatef(1.f, 0, 0); getFromModelView(t); anim.add(2000, t); anim.add(3000, t);
    glLoadIdentity();        getFromModelView(t); anim.add(4000, t);
  }
#endif

  // ==========================================================================

  //static const animBuilder builders[] =
  //  {
  //    buildTrain,
  //  };

  void loadAnims()
  {
    START_TIME_EVAL;
    OGL_ERROR_CHECK("loadAnims");

    IFDBG(if (list != NULL) delete[] list;)
    list = new animFunc[numberOfAnims];
    //nodeIndependantAnims = new Anim<Transformation>[numberOfAnims];

    //const unsigned int numberOfBuilders = sizeof(builders) / sizeof(animBuilder);
    //for (unsigned int i = 0; i < numberOfBuilders; ++i)
    //{
    //  DBG("loading anim %u...", i);
    //  builders[i]();
    //  Loading::update();
    //  OGL_ERROR_CHECK("loadAnims i = %u", i);
    //}

    list[carouselPhysics] = Carousel::physicsAnimation;

    list[door] = Workshop::doorAnimation;
    list[doorHandle] = Workshop::doorHandleAnimation;
    list[bookDominoFall] = BookShelf::bookDominoFall;
    list[lastBookDominoFall] = BookShelf::lastBookDominoFall;
    list[vibration] = MusicBox::vibrationAnimation;
    list[musicBoxRotation] = MusicBox::rotationAnimation;
    list[musicBoxCylRotation] = MusicBox::cylinderAnimation;
    list[musicBoxOpening] = MusicBox::openingAnimation;
    list[pendulum] = CatClock::pendulumAnimation;
    list[clockHand] = CatClock::clockHandAnimation;
    list[leftArm] = Puppet::leftArmAnimation;
    list[leftForearm] = Puppet::leftForearmAnimation;
    list[leftArmClap] = Puppet::leftArmClapAnimation;
    list[leftForearmClap] = Puppet::leftForearmClapAnimation;
    list[xyloArm] = Puppet::xyloArmAnimation;
    list[xyloForearm] = Puppet::xyloForearmAnimation;
    list[xyloStick] = Puppet::stickAnimation;
    list[head] = Puppet::headAnimation;
    list[hihat] = Puppet::hihatAnimation;
    list[wheelsRotation] = Xylotrain::wheelsAnimation;
    list[flute] = Lousianne::fluteAnimation;
    list[lousianneRotate] = Lousianne::rotateAnimation;
    list[lousianneRotateWheels] = Lousianne::rotateWheelsAnimation;
    list[orgalame] = Orgalame::keyAnimation;
    list[orgalameBlades] = Orgalame::bladeAnimation;
    list[train] = Xylotrain::trainAnimation;
    list[pcBody] = Pascontent::bodyAnimation;
    list[pcSlider] = Pascontent::sliderAnimation;
    list[pcPipe] = Pascontent::pipeAnimation;
    list[drumStick] = Drum::stickAnimation;

    Loading::update();

    IFDBG(checkAllAnimsAreLoaded();)

    END_TIME_EVAL("Anims creation");
  }
}
