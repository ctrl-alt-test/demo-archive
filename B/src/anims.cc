//
// Toutes les animations utilisées
//

#include "animid.hh"
#include "anims.hh"
#include "city.hh"
#include "forest.hh"
#include "lsystem.hh"
#include "spline.h"
#include "timing.hh" // FIXME : Virer
#include "renderlist.hh" // FIXME : dû à lsystem
#include "starfield.hh"

// include pour les constantes
#include "factory.hh"
#include "rotor.hh"

namespace Anim
{
  Anim * list = NULL;

  // Usine
  static void buildRotorPistonCycle();
  static void buildTreadmillAnims();
  static void buildFallAnim();
  static void buildMoveToTreadmillAnim();
  static void buildArrivalCubesAnim();
  static void buildFakeBeamCubesAnim();
  static void buildGrab1Anim();
  static void buildGrab2Anim();
  static void buildGrabValidatedAnim();
  static void buildGrabbed1Anim();
  static void buildGrabbed2Anim();
  static void buildGrabbedValidatedAnim();
  static void buildBeforeTestCubesAnim();
  static void buildCubeValidatorCycle();
  static void buildGoToPitAnim();
  static void buildPitObjectAnim();

  static void buildZeusAnims();
  static void buildLightningAnim();
  static void buildSphereDieAnim();

  static void buildDance1Anim();
  static void buildDance2Anim();

  // Ville
  static void buildSmokeAnim();
  static void buildWagonAnim();
  static void buildCarAnim();
  static void buildRailsAnim();
  static void buildGrowingBuilding();
  static void buildTetris();
  static void buildCityWalk1();

  // Pong
  static void buildPongAnims();

  // Starfield
  static void buildStarfieldAnims();
  static void buildStarfieldCubeAnim();

  // Ramp
  static void buildRampTransitionAnim();
  static void buildRampNodeAnim();
  static void buildRampCubeAnim();

  // Forêt
  static void buildArrivingForestCubeAnim();
  static void buildGrowingTreeAnim();
  static void buildLeafAnim();
  static void buildForestWalk();
  static void buildForestJumpingCube();

  // Rope
  static void buildRopeNodeAnim();
  static void buildRopeCubeAnim();

  // Écran
  static void buildJoinScreenAnim();

  typedef void(*animBuilder)();

  const animBuilder builders[] =
    {
      buildRotorPistonCycle,
      buildTreadmillAnims,
      buildFallAnim,
      buildMoveToTreadmillAnim,
      buildArrivalCubesAnim,
      buildFakeBeamCubesAnim,
      buildGrab1Anim,
      buildGrab2Anim,
      buildGrabValidatedAnim,
      buildGrabbed1Anim,
      buildGrabbed2Anim,
      buildGrabbedValidatedAnim,
      buildBeforeTestCubesAnim,
      buildCubeValidatorCycle,
      buildGoToPitAnim,
      buildPitObjectAnim,

      buildZeusAnims,
      buildLightningAnim,
      buildSphereDieAnim,

      buildDance1Anim,
      buildDance2Anim,

      buildSmokeAnim,
      buildCarAnim,
      buildWagonAnim,
      buildRailsAnim,
      buildGrowingBuilding,
      buildTetris,
      buildCityWalk1,

      buildPongAnims,

      buildStarfieldAnims,
      buildStarfieldCubeAnim,

      buildRampTransitionAnim,
      buildRampNodeAnim,
      buildRampCubeAnim,

      buildArrivingForestCubeAnim,
      buildGrowingTreeAnim,
      buildLeafAnim,
      buildForestWalk,
      buildForestJumpingCube,

      buildRopeNodeAnim,
      buildRopeCubeAnim,

      buildJoinScreenAnim
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
  }

  // FIXME : destruction des animations



  // ==========================================================================

  static void buildRotorPistonCycle()
  {
    Anim & anim = list[rotorPistonCycle];
    anim.setAnim(3, Rotor::pistonCycleDuration, 0);

    glPushMatrix();
    glLoadIdentity();
    anim.add(Keyframe(0));
    glTranslatef(0, Rotor::pistonDebatement, 0);
    anim.add(Keyframe(0.5f));
    glLoadIdentity();
    anim.add(Keyframe(1.f));
    glPopMatrix();
  }

  //
  // Cycle rectangulaire
  //
  static void buildSquareCycle(id animId, date duration, float length, float width, bool vertical)
  {
    const float invPerimeter = 0.5f / (length + width);

    glPushMatrix();
    glLoadIdentity();
    Keyframe step1(0);
    Keyframe step5(1.f);

    if (vertical)
    {
      glTranslatef(0, length, 0);
    }
    else
    {
      glTranslatef(0, 0, length);
    }
    Keyframe step2(length * invPerimeter);

    glTranslatef(width, 0, 0);
    Keyframe step3(0.5f);

    glLoadIdentity();
    glTranslatef(width, 0, 0);
    Keyframe step4((2 * length + width) * invPerimeter);
    glPopMatrix();

    Anim & anim = list[animId];
    anim.setAnim(5, duration, 0);
    anim.add(step1);
    anim.add(step2);
    anim.add(step3);
    anim.add(step4);
    anim.add(step5);
  }

  static void buildTreadmillAnims()
  {
    buildSquareCycle(slowTreadmill, Factory::arrivalTreadmillCycleDuration, 0.5f, 0.5f, true);
    buildSquareCycle(fastTreadmill, Factory::testTreadmillCycleDuration, 0.5f, 0.5f, true);

    const date pitTrackDuration = 1000 * (Factory::pitLength + Factory::pitWidth);
    buildSquareCycle(pitTrack, pitTrackDuration,
		     float(Factory::pitLength),
		     float(Factory::pitWidth), false);
  }

  //
  // Chute du cube du début
  //
  static void buildFallAnim()
  {
    Anim & anim = list[fall];
    anim.setAnim(7, 2000);

    glPushMatrix();

    // Temps d'attente suspendu
    glLoadIdentity();
    glTranslatef(0, 5.f, 0);
    anim.add(Keyframe(0));
    glPopMatrix();

    // Chute de 5m : 1.01s, arrondi à 1s ici
    for (unsigned int i = 0; i <= 5; ++i)
    {
      const float t = i * 0.2f;

      glPushMatrix();
      glLoadIdentity();
      glTranslatef(0, 5.f * (1.f - t * t), 0);
      anim.add(Keyframe(0.5f + 0.5f * t));
      glPopMatrix();
    }
  }

  //
  // Déplacement du cube vers le premier tapis
  //
  static void buildMoveToTreadmillAnim()
  {
    Anim & anim = list[moveToTreadmill];
    anim.setAnim(3, 6000);

    glPushMatrix();
    glLoadIdentity();
    anim.add(Keyframe(0));
    anim.add(Keyframe(0.25f));
    glTranslatef(0, 0, 20.f);
    anim.add(Keyframe(1.f));
    glPopMatrix();
  }

  //
  // Cubes sur le premier tapis
  //
  static void buildArrivalCubesAnim()
  {
    Anim & anim = list[arrivalCubes];
    anim.setAnim(2, 8000, 0);

    glPushMatrix();
    glLoadIdentity();
    anim.add(Keyframe(0));
    glTranslatef(55.f, 0, 0);
    anim.add(Keyframe(1.f));
    glPopMatrix();
  }

  //
  // Cubes sur faux tapis
  //
  static void buildFakeBeamCubesAnim()
  {
    // FIXME : cette animation n'avait pas encore été testée
    Anim & anim = list[fakeBeamCubes];
    anim.setAnim(2, 24000, 0);

    glPushMatrix();
    glLoadIdentity();
    anim.add(Keyframe(0));
    glTranslatef(70.f, 0, 0);
    anim.add(Keyframe(1.f));
    glPopMatrix();
  }

  //
  // Appareil de saisie des cubes du tapis du bas
  //
  static void buildGrab1Anim()
  {
    Anim & anim = list[grab1];
    anim.setAnim(7, 2000, 0);

    glPushMatrix();
    glLoadIdentity();
    Keyframe grab0(0);
    Keyframe grab6(1.f);
    glTranslatef(0, -1.f, 0);
    Keyframe grab1(0.15f);
    glTranslatef(0, 1.f, 0);
    Keyframe grab2(0.3f);

    glTranslatef(0, 0, 2.f);
    Keyframe grab3(0.5f);

    glTranslatef(0, -3.8f, 0);
    Keyframe grab4(0.65f);
    glTranslatef(0, 3.8f, 0);
    Keyframe grab5(0.8f);
    glPopMatrix();

    anim.add(grab0);
    anim.add(grab1);
    anim.add(grab2);
    anim.add(grab3);
    anim.add(grab4);
    anim.add(grab5);
    anim.add(grab6);
  }

  //
  // Appareil de saisie des cubes du tapis du haut
  //
  static void buildGrab2Anim()
  {
    Anim & anim = list[grab2];
    anim.setAnim(7, 2000, 0);

    glPushMatrix();
    glLoadIdentity();
    Keyframe grab0(0);
    Keyframe grab6(1.f);
    glTranslatef(0, -1.f, 0);
    Keyframe grab1(0.15f);
    glTranslatef(0, 1.f, 0);
    Keyframe grab2(0.2f);

    glTranslatef(0, 0, 2.f);
    Keyframe grab3(0.35f);

    glTranslatef(0, -10.8f, 0);
    Keyframe grab4(0.65f);
    glTranslatef(0, 10.8f, 0);
    Keyframe grab5(0.9f);
    glPopMatrix();

    anim.add(grab0);
    anim.add(grab1);
    anim.add(grab2);
    anim.add(grab3);
    anim.add(grab4);
    anim.add(grab5);
    anim.add(grab6);
  }

  //
  // Appareil de saisie des cubes validés
  //
  static void buildGrabValidatedAnim()
  {
    Anim & anim = list[grabValidated];
    anim.setAnim(5, Factory::validatorCycleDuration / 2, 0);

    glPushMatrix();
    glLoadIdentity();
    anim.add(Keyframe(0));
    anim.add(Keyframe(0.1f));

    glTranslatef(0, 5.f, 0);
    anim.add(Keyframe(0.5f));
    anim.add(Keyframe(0.8f));

    glLoadIdentity();
    anim.add(Keyframe(1.f));
    glPopMatrix();
  }

  //
  // Cubes saisis sur le tapis du bas
  //
  static void buildGrabbed1Anim()
  {
    Anim & anim = list[grabbed1];
    anim.setAnim(6, 24000, 0);

    glPushMatrix();
    glLoadIdentity();
    glTranslatef(0, -1.f, 0);
    Keyframe grab0(0);

    glTranslatef(0, 1.f, 0);
    Keyframe grab1(0.15f / 12.f);

    glTranslatef(0, 0, 2.f);
    Keyframe grab2(0.35f / 12.f);

    glTranslatef(0, -3.8f, 0);
    Keyframe grab3(0.5f / 12.f);

    // Partie de l'anim où le cube est invisible (retour du grabber)
    Keyframe grab4(0.500001f / 12.f, 0);
    Keyframe grab5(1.f, 0);
    glPopMatrix();

    anim.add(grab0);
    anim.add(grab1);
    anim.add(grab2);
    anim.add(grab3);
    anim.add(grab4);
    anim.add(grab5);
  }

  //
  // Cubes saisis sur le tapis du haut
  //
  static void buildGrabbed2Anim()
  {
    Anim & anim = list[grabbed2];
    anim.setAnim(6, 24000, 0);

    glPushMatrix();
    glLoadIdentity();
    glTranslatef(0, -1.f, 0);
    Keyframe grab0(0);

    glTranslatef(0, 1.f, 0);
    Keyframe grab1(0.05f / 12.f);

    glTranslatef(0, 0, 2.f);
    Keyframe grab2(0.2f / 12.f);

    glTranslatef(0, -10.8f, 0);
    Keyframe grab3(0.5f / 12.f);

    // Partie de l'anim où le cube est invisible (retour du grabber)
    Keyframe grab4(0.500001f / 12.f, 0);
    Keyframe grab5(1.f, 0);
    glPopMatrix();

    anim.add(grab0);
    anim.add(grab1);
    anim.add(grab2);
    anim.add(grab3);
    anim.add(grab4);
    anim.add(grab5);
  }

  //
  // Cube validé saisi
  //
  static void buildGrabbedValidatedAnim()
  {
    Anim & anim = list[grabbedValidated];
    anim.setAnim(4, Factory::validatorCycleDuration / 2, 0);

    glPushMatrix();
    glLoadIdentity();
    anim.add(Keyframe(0));
    anim.add(Keyframe(0.05f));

    glTranslatef(0, 5.f, 0);
    anim.add(Keyframe(0.45f));

    anim.add(Keyframe(1.f));
    glPopMatrix();
  }

  //
  // Cubes sur les tapis pour le test
  //
  static void buildBeforeTestCubesAnim()
  {
    Anim & anim = list[beforeTest];
    anim.setAnim(4, Factory::onTestTreadmillDuration, 0);

    glPushMatrix();
    glLoadIdentity();
    anim.add(Keyframe(0));
    glTranslatef(0, 0, 48.f);
    anim.add(Keyframe(0.9f));
    glTranslatef(0, 0, 4.f);
    anim.add(Keyframe(0.94f));
    anim.add(Keyframe(1.f));
    glPopMatrix();
  }

  //
  // Validateurs dans l'usine
  //
  static void buildCubeValidatorCycle()
  {
    const char data[] =
      {
	  0, -2,  0,  0, // Left start
	 26, -2,  0,  0,
	 28, -1,  0,  0, // left
	 30, -1,  0,  0,
	 32, -1,  0, -1,
	 34,  0,  0, -1, // back
	 36,  0,  0, -1,
	 38,  0, -1, -1,
	 40,  0, -1,  0, // bottom
	 42,  0, -1,  0,
	 44,  1, -1,  0,
	 46,  1,  0,  0, // right
	 48,  1,  0,  0,
	 50,  2,  0,  0, // right end
	 76,  2,  0,  0, // right start
	 78,  1,  0,  0, // right
	 80,  1,  0,  0,
	 82,  1,  0,  1,
	 84,  0,  0,  1, // front
	 86,  0,  0,  1,
	 88,  0,  1,  1,
	 90,  0,  1,  0, // top
	 92,  0,  1,  0,
	 94, -1,  1,  0,
	 96, -1,  0,  0, // left
	 98, -1,  0,  0,
	100, -2,  0,  0  // left end
      };

    const unsigned int animLength = sizeof(data) / (4 * sizeof(char));

    Anim & anim = list[validatorCycle];
    anim.setAnim(animLength, Factory::validatorCycleDuration, 0);

    glPushMatrix();
    for (unsigned int i = 0; i < animLength; ++i)
    {
      glLoadIdentity();
      const unsigned int index = 4 * i;
      glTranslatef(Factory::validatorSize * 0.5f * data[index + 1],
		   Factory::validatorSize * 0.5f * data[index + 2],
		   Factory::validatorSize * 0.5f * data[index + 3]);
      const char t = data[index];
      const float textureFade = (t < 26 || t >= 98 ||
				 t >= 48 && t < 76 ? 1.f : 0);
      anim.add(Keyframe(0.01f * t, 1.f, false, textureFade));
    }
    glPopMatrix();
  }

  //
  // Cube qui tombe sur le tapis pour la fosse
  //
  static void buildGoToPitAnim()
  {
    Anim & anim = list[goToPit];
    anim.setAnim(3, 2000, 0);

    glPushMatrix();
    glLoadIdentity();
    glTranslatef(0, 0, 52.f);
    anim.add(Keyframe(0));
    glTranslatef(0, -9.2f, 0);
    glRotatef(10.f, 0, 1.f, 0);
    anim.add(Keyframe(0.5f));
    glLoadIdentity();
    glTranslatef(-1.f, -9.2f, 52.f);
    glRotatef(15.f, 0, 1.f, 0);
    anim.add(Keyframe(1.f));
    glPopMatrix();
  }

  //
  // Objets sur le tapis pour la fosse
  //
  static void buildPitObjectAnim()
  {
    Anim & anim = list[pitObject];
    anim.setAnim(7, Factory::onPitTreadmillDuration, 0);

    glPushMatrix();
    glLoadIdentity();
    glTranslatef(30.f, 0, 0);
    anim.add(Keyframe(0));
    glPopMatrix();

    // Chute de 5m : 1.01s, arrondi à 1s ici
    for (unsigned int i = 0; i <= 5; ++i)
    {
      const float t = i * 0.2f;

      glPushMatrix();
      glLoadIdentity();
      glTranslatef(-0.5f * i, 0.1f - 10.f * t * t, 0);
      glRotatef(20.f * i, 0, 0, 1.f);
      glRotatef(2.f * i, 1.f, 0, 0);
      anim.add(Keyframe(0.96f + 0.04f * t));
      glPopMatrix();
    }
  }

  //
  // Parties du canon Zeus
  //
  static void buildZeusAnims()
  {
    //
    // FIXME : mettre les constantes dans Zeus
    //
    const unsigned short periods[11] = {1000, 1000, 600, 600, 400, 50,
					250, 550, 700, 1050, 1200};
    const unsigned char sizes[11] = {6, 6, 8, 6, 4, 2,
				     16, 22, 28, 34, 40};

    for (unsigned int i = 0; i < 11; ++i)
    {
      const unsigned char size = sizes[i];
      buildSquareCycle(id(zeusHeart + i), periods[i] * 2 * size, float(size), float(size), false);
    }
  }

  //
  // Élément d'éclair
  //
  static void buildLightningAnim()
  {
    //
    // FIXME : récupérer les constantes depuis Zeus
    //
    const date forwardDuration = 200;
    const date backwardDuration = 500;

    Anim & forward = list[lightningForward];
    Anim & backward = list[lightningBackward];
    forward.setAnim(4, forwardDuration);
    backward.setAnim(4, backwardDuration);

    glPushMatrix();
    glLoadIdentity();
    const Keyframe growStart(0);
    const Keyframe growOver(0.1f, 2.f);
    const Keyframe growMiddle(0.2f);
    const Keyframe growEnd(1.f, 0);
    glPopMatrix();

    forward.add(growStart);
    forward.add(growOver);
    forward.add(growMiddle);
    forward.add(growEnd);

    forward.createCache();

    backward.add(growStart);
    backward.add(growOver);
    backward.add(growMiddle);
    backward.add(growEnd);

    backward.createCache();
  }

  //
  // Sphère qui réduit (noyée dans les cubes)
  //
  static void buildSphereDieAnim()
  {
    //
    // FIXME : récupérer la constante depuis Sphere
    //
    const date duration = 10000;

    Anim & anim = list[sphereDie];
    anim.setAnim(2, duration);

    glPushMatrix();
    glLoadIdentity();
    anim.add(Keyframe(0));
    anim.add(Keyframe(1.f, 0));
    glPopMatrix();
  }

  //
  // Animation de la chorégraphie : caisse qui passe
  //
  static void buildDance1Anim()
  {
    Anim & anim = list[dance1];
    anim.setAnim(2, Factory::dance1AnimDuration, 0);

    glPushMatrix();
    glLoadIdentity();
    glTranslatef(-20.f, 0, 0);
    anim.add(Keyframe(0));
    glLoadIdentity();
    glTranslatef(20.f, 0, 0);
    anim.add(Keyframe(1.f));
    glPopMatrix();
  }

  static void buildDance2Anim()
  {
    Anim & anim = list[dance2];
    anim.setAnim(4, Factory::dance2AnimDuration, 0);

    glPushMatrix();
    glLoadIdentity();
    glTranslatef(0, 0, 30.f);
    anim.add(Keyframe(0));

    glLoadIdentity();
    anim.add(Keyframe(0.4f));

    glRotatef(90.f, 0, 1.f, 0);
    anim.add(Keyframe(0.6f));

    glTranslatef(0, 0, -30.f);
    anim.add(Keyframe(1.));
    glPopMatrix();
  }

  //
  // Panache de fumée de l'usine
  //
  static void buildSmokeAnim()
  {
    Anim & anim = list[smoke];
    anim.setAnim(21, 8000, 0);

    glPushMatrix();
    for (unsigned int i = 0; i <= 20; ++i)
    {
      const float t = i * 0.05f;
      glLoadIdentity();
      glTranslatef(40.f * t * t, 20.f * t, 0);
      glRotatef(100.f * t, 1.f, 0, 0);
      glRotatef( 60.f * t, 0, 0, 1.f);
      const float size = 1.f + 6.f * t + 2.f * t * t;
      anim.add(Keyframe(t, size));
    }
    glPopMatrix();
  }

  //
  // FIXME : fonction pour pointer vers un point ; ranger quelque part
  //
  static void doRotation(float* pos, float* old)
  {
    const float difx = old[0] - pos[0];
    const float dify = old[1] - pos[1];
    const float difz = old[2] - pos[2];

    glRotatef(RAD_TO_DEG * msys_atan2f(difx, difz), 0.f, 1.f, 0.f);
    glRotatef(RAD_TO_DEG * -msys_sinf(dify), 1.f, 0.f, 0.f);
  }

  //
  // Wagon du train et cubes transportés
  //
  static void buildWagonAnim()
  {
    //
    // FIXME : récupérer les constantes depuis City
    // De plus il faut que rails ait été initialisé
    //
    const int skip = 5;
    const float wagon_width = 0.25f;
    const float cube_width = 0.1f;
    const unsigned int keyframes = 170;

    Anim & wagonAnim = list[wagon];
    wagonAnim.setAnim(keyframes, 50000);

    Anim & cube1Anim = list[wagonCube1];
    Anim & cube2Anim = list[wagonCube2];
    Anim & cube3Anim = list[wagonCube3];
    Anim & cube4Anim = list[wagonCube4];
    cube1Anim.setAnim(keyframes, 50000);
    cube2Anim.setAnim(keyframes, 50000);
    cube3Anim.setAnim(keyframes, 50000);
    cube4Anim.setAnim(keyframes, 50000);

    glPushMatrix();
    glLoadIdentity();

    float old[3];
    spline(rails_data, rails_points, 3, 0.f, old);
    int count = 0;
    for (float k = 0.f; k < 170.f; k += 0.1f)
    {
      float pos[3];
      spline(rails_data, rails_points, 3, k, pos);

      if (count > 0 && railsDist(pos, old) <= 5.f)
        continue;

      const float date = (float)count / 48.f; // 48 correspond au dernier count utilisé
      count++;

      glPushMatrix();

      glTranslatef(pos[0] * 0.5f, pos[1] * 0.5f, pos[2] * 0.5f);
      doRotation(pos, old);
      wagonAnim.add(Keyframe(date, 1.f, true));

      glPushMatrix();
      glTranslatef(-wagon_width/4.f, 0.f, -wagon_width/4.f);
      cube1Anim.add(Keyframe(date, 1.f, true));
      glPopMatrix();

      glPushMatrix();
      glTranslatef(wagon_width/4.f, 0.f, -wagon_width/4.f);
      cube2Anim.add(Keyframe(date, 1.f, true));
      glPopMatrix();

      glPushMatrix();
      glTranslatef(-wagon_width/4.f, 0.f, wagon_width/4.f);
      cube3Anim.add(Keyframe(date, 1.f, true));
      glPopMatrix();

      glPushMatrix();
      glTranslatef(wagon_width/4.f, 0.f, wagon_width/4.f);
      cube4Anim.add(Keyframe(date, 1.f, true));
      glPopMatrix();

      glPopMatrix();

      old[0] = pos[0];
      old[1] = pos[1];
      old[2] = pos[2];
    }
  }

  //
  // Voitures en ville
  //
  static void buildCarAnim()
  {
    const date carDuration = 25000;
    const int height = 60;

    Anim & anim = list[car];
    Anim & anim2 = list[car2]; // voitures dans l'autre sens
    anim.setAnim(2, carDuration, 0);
    anim2.setAnim(2, carDuration, 0);

    glPushMatrix();
    glLoadIdentity();
    anim.add(Keyframe(0));
    anim2.add(Keyframe(0));
    glTranslatef(0, 0, (float)height/2);
    anim.add(Keyframe(1.f));
    glTranslatef(0, 0, - (float)height);
    anim2.add(Keyframe(1.f));
    glPopMatrix();
  }

  // Rails
  static void buildRailsAnim()
  {
    const float height = 30.f;
    Anim & anim = list[rails];
    anim.setAnim(6, 3000);

    glPushMatrix();
    glLoadIdentity();
    anim.add(Keyframe(0.f));
    glTranslatef(0.f, -height, 0.f);
    anim.add(Keyframe(0.8f));

    glTranslatef(0.f, -0.01f * height, 0.f);
    anim.add(Keyframe(0.85f));

    glTranslatef(0.f, -0.005f * height, 0.f);
    anim.add(Keyframe(0.9f));

    glTranslatef(0.f, 0.005f * height, 0.f);
    anim.add(Keyframe(0.95f));

    glTranslatef(0.f, 0.01f * height, 0.f);
    anim.add(Keyframe(1.f));
    glPopMatrix();
  }

  static void buildGrowingBuilding()
  {
    const float height = 2.f;
	const int steps = 8;
    Anim & anim = list[growingBuilding];
    anim.setAnim(steps + 1, 4000);

    glPushMatrix();
    glLoadIdentity();
    glTranslatef(0.f, -height, 0.f);
	float t = 0.f;
    anim.add(Keyframe(t));
	for (int i = 0; i < steps; i++)
	{
	  glTranslatef(0.f, height / steps, 0.f);
	  t += i % 2 == 0 ? 5.f : 1.f;
	  AnimAddUnsafe(anim, Keyframe(t));
	}
	anim.normalize();
    glPopMatrix();
  }

  static void buildTetris()
  {
    const float height = 30.f;
    Anim & anim = list[tetris];
    anim.setAnim(2, 5000);

    glPushMatrix();
    glLoadIdentity();
    anim.add(Keyframe(0.f));
    glTranslatef(0.f, -height, 0.f);
    anim.add(Keyframe(1.f));
    glPopMatrix();
  }

  static void makeCubeWalk(Anim & anim, int steps, int time, float vx, float vz)
  {
    const float sqrt2 = 1.42f;
    const float vshift = (sqrt2 - 1.f) * 0.5f;
    anim.setAnim(steps * 3 + 2, time);

    glPushMatrix();
    float t = 0.f;

    for (int i = 0; i < 2 * steps + 1; i++)
    {
      glTranslatef(vx, i % 2 == 0 ? -vshift : vshift, vz);
      glPushMatrix();
      glRotatef((float)i * 45.f, vz, 0.f, vx);
      AnimAddUnsafe(anim, Keyframe(t));
      if (i % 2 == 0) AnimAddUnsafe(anim, Keyframe(t += 0.8f));
      glPopMatrix();
      t += i % 2 == 0 ? 1.f : 0.3f;
    }
    glPopMatrix();
    anim.normalize();
  }

  static void buildCityWalk1()
  {
    Anim & anim = list[cityWalk1];
    const int steps = 50;
    makeCubeWalk(anim, steps, 25000, 0.f, 1.f);
  }

  static void buildForestWalk()
  {
    Anim & anim = list[forestWalk];
    const int steps = 26;
    glTranslatef(0, 0.22f, 0);
    makeCubeWalk(anim, steps, 10400, 0.f, 1.f);
  }

  static void buildForestJumpingCube()
  {
    Anim & anim = list[forestJumpingCube];
    anim.setAnim(3, 1200);
    const float height = 2.f;

    glPushMatrix();
    glLoadIdentity();
    glTranslatef(0.f, -height, 0.f);
    anim.add(Keyframe(0.f, 1.f, true));

    glTranslatef(0.f, 2.f*height, 2.f);
    anim.add(Keyframe(0.5f, 1.f, true));

    glTranslatef(0.f, -height, 1.f);
    anim.add(Keyframe(1.f, 1.f, true));

    glPopMatrix();
  }

  //
  // Raquettes et balle du pong
  //

  //
  // FIXME : se débarrasser de ces constantes pour l'anim
  //

#define PONG_START_DATE	pongStartDate
#define PONG_END_DATE	pongEndDate

// Dimensions de l'"ecran de jeu" du pong (raquettes exclues)
#define PONG_HEIGHT 0.5f // taille d'une case de la ville
#define PONG_WIDTH 0.25f // espace entre les 2 voies d'une avenue
// Case de la ville ou la scene commence
#define XSQUARE 30
#define YSQUARE 46
// Vitesse de deplacement de la scene dans la ville (cases/ms)
#define SPEED 0.002f
// Date a partir de laquelle la scene arrete de se deplacer
#define STOP_DATE 7000
// Dimensions des elements
#define PADDLE_SIZE 0.05f
#define BALL_SIZE 0.05f

// Position du coin superieur gauche de l'ecran de jeu
#define PONG_XPOS ((XSQUARE)/2 - 0.1f)
#define PONG_YPOS 0.05f
#define PONG_ZPOS ((YSQUARE)/2 - 0.25f)

#define PONG_REAL_WIDTH (PONG_WIDTH - BALL_SIZE)


  struct PaddlePos
  {
    float pos;  // 0 = en bas de l'ecran, 1 = en haut
    unsigned duration;
  };
  struct BallPos
  {
    float xPos;
    float yPos;
    unsigned duration;
  };

  static void buildPongAnims()
  {
    const PaddlePos leftPaddlePos[] = {
      {0.50f,    0},
      {0.60f, 1000},
      {0.25f,  750},
      {0.40f,  250},
      {0.60f, 1000},
      {0.30f, 1000},
      {0.15f,  500},
      {0.50f, 1500},
      {0.65f,  500},
      {0.30f, 1500},
      {0.20f,  500},
      {0.50f, 1500},
      {0.70f,  500},
      {0.20f, 1250},
      {0.35f,  250},
      {0.65f, 1000},
    };

    const PaddlePos rightPaddlePos[] = {
      {0.50f,    0},
      {0.20f, 1000},
      {0.15f,  250},
      {0.80f, 1750},
      {0.90f,  500},
      {0.50f, 1500},
      {0.40f,  500},
      {0.40f, 1000},
      {0.70f,  500},
      {0.80f,  250},
      {0.80f, 1500},
      {0.70f,  250},
      {0.60f,  250},
      {0.60f,  750},
      {0.70f,  250},
      {0.30f,  750},
      {0.50f, 2000},
    };

    const BallPos ballPos[] = {
      {0.00f, 0.50f,    0},
      {1.00f, 0.20f, 1000},
      {0.00f, 0.40f, 1000},
      {1.00f, 0.80f, 1000},
      {0.00f, 0.30f, 1000},
      {1.00f, 0.50f, 1000},
      {0.00f, 0.50f, 1000},
      {1.00f, 0.70f, 1000},
      {0.70f, 0.90f,  250},
      {0.00f, 0.30f,  750},
      {0.30f, 0.10f,  250},
      {1.00f, 0.70f,  750},
      {0.00f, 0.50f, 1000},
      {1.00f, 0.30f, 1000},
      {0.67f, 0.10f,  250},
      {0.00f, 0.50f,  750},
      {-1.0f, 1.10f, 1000},
    };

    const unsigned int leftSteps = sizeof(leftPaddlePos) / sizeof(PaddlePos);
    const unsigned int rightSteps = sizeof(rightPaddlePos) / sizeof(PaddlePos);
    const unsigned int ballSteps = sizeof(ballPos) / sizeof(BallPos);

#if DEBUG
    date length = 0;
    for (int i = 0; i < leftSteps; ++i)
      length += leftPaddlePos[i].duration;
    assert(length == PONG_END_DATE - PONG_START_DATE);

    length = 0;
    for (int i = 0; i < rightSteps; ++i)
      length += rightPaddlePos[i].duration;
    assert(length == PONG_END_DATE - PONG_START_DATE);

    length = 0;
    for (int i = 0; i < ballSteps; ++i)
      length += ballPos[i].duration;
    assert(length == PONG_END_DATE - PONG_START_DATE);
#endif

    Anim & leftAnim = list[pongLeftPaddle];
    leftAnim.setAnim(leftSteps, PONG_END_DATE - PONG_START_DATE);

    Anim & rightAnim = list[pongRightPaddle];
    rightAnim.setAnim(rightSteps, PONG_END_DATE - PONG_START_DATE);

    Anim & ballAnim = list[pongBall];
    ballAnim.setAnim(ballSteps, PONG_END_DATE - PONG_START_DATE);


    glPushMatrix();
    unsigned t = 0;
    for (unsigned i = 0; i < leftSteps; ++i)
    {
      t += leftPaddlePos[i].duration;
      glLoadIdentity();
      // 0.5: taille d'une case de la ville
      const float movePos = SPEED * 0.5f * min(t, STOP_DATE);
      glTranslatef(PONG_XPOS - PADDLE_SIZE,
                   PONG_YPOS,
                   PONG_ZPOS + leftPaddlePos[i].pos * PONG_HEIGHT + movePos);
      leftAnim.add(Keyframe(t * 1.f / (PONG_END_DATE - PONG_START_DATE)));
    }

    t = 0;
    for (unsigned i = 0; i < rightSteps; ++i)
    {
      t += rightPaddlePos[i].duration;
      glLoadIdentity();
      // 0.5: taille d'une case de la ville
      const float movePos = SPEED * 0.5f * min(t, STOP_DATE);
      glTranslatef(PONG_XPOS + PONG_REAL_WIDTH + PADDLE_SIZE,
                   PONG_YPOS,
                   PONG_ZPOS + rightPaddlePos[i].pos * PONG_HEIGHT + movePos);
      rightAnim.add(Keyframe(t * 1.f / (PONG_END_DATE - PONG_START_DATE)));
    }

    t = 0;
    for (unsigned i = 0; i < ballSteps; ++i)
    {
      t += ballPos[i].duration;
      glLoadIdentity();
      const float movePos = SPEED * 0.5f * min(t, STOP_DATE);
      glTranslatef(PONG_XPOS + ballPos[i].xPos * PONG_REAL_WIDTH,
                   PONG_YPOS,
                   PONG_ZPOS + ballPos[i].yPos * PONG_HEIGHT + movePos);
      ballAnim.add(Keyframe(t * 1.f / (PONG_END_DATE - PONG_START_DATE)));
    }
    glPopMatrix();
  }

  //
  // Starfield
  //
  static void buildStarfieldAnims()
  {
    Anim & growAnim = list[conwayGrow];
    Anim & fromTopAnim = list[conwayFromTop];
    Anim & fromLeftAnim = list[conwayFromLeft];
    Anim & fromRightAnim = list[conwayFromRight];
    Anim & fromBottomAnim = list[conwayFromBottom];
    Anim & dieAnim = list[conwayDie];

    growAnim.setAnim(2, Conway::stepTime),
    fromTopAnim.setAnim(2, Conway::stepTime),
    fromLeftAnim.setAnim(2, Conway::stepTime),
    fromRightAnim.setAnim(2, Conway::stepTime),
    fromBottomAnim.setAnim(2, Conway::stepTime),
    dieAnim.setAnim(3, Conway::lifeTime),

    glPushMatrix();
    {
      glLoadIdentity();

      growAnim.add(Keyframe(0, 0));
      growAnim.add(Keyframe(1.f, 1.f));

      glPushMatrix();
      dieAnim.add(Keyframe(0));
      glTranslatef(0, 100.f, 0);
      dieAnim.add(Keyframe(0.5f));
      glTranslatef(0, 100.f, 0);
      dieAnim.add(Keyframe(1.f, 0));
      glPopMatrix();

      glPushMatrix();
      glTranslatef(0, 0, -1.f);
      fromTopAnim.add(Keyframe(0));
      glPopMatrix();

      glPushMatrix();
      glTranslatef(-1.f, 0, 0);
      fromLeftAnim.add(Keyframe(0));
      glPopMatrix();

      glPushMatrix();
      glTranslatef(1.f, 0, 0);
      fromRightAnim.add(Keyframe(0));
      glPopMatrix();

      glPushMatrix();
      glTranslatef(0, 0, 1.f);
      fromBottomAnim.add(Keyframe(0));
      glPopMatrix();

      fromTopAnim.add(Keyframe(1.f));
      fromLeftAnim.add(Keyframe(1.f));
      fromRightAnim.add(Keyframe(1.f));
      fromBottomAnim.add(Keyframe(1.f));

      growAnim.createCache();
      fromTopAnim.createCache();
      fromLeftAnim.createCache();
      fromRightAnim.createCache();
      fromBottomAnim.createCache();
      dieAnim.createCache();
    }
    glPopMatrix();
  }

  //
  // Cube dans le starfield
  //
  static void buildStarfieldCubeAnim()
  {
    //
    // FIXME : mettre ces constantes ailleurs
    //
    const int numCycles = 20;
    const date timeArrivalInGrid = 20000;//15000;
    const date timeAfterGrid = 0; //5000;
    const float y0 = 200;
    const float v = -y0 / timeArrivalInGrid;

    Anim & anim = list[conwayCube];
    anim.setAnim(numCycles + 1, timeArrivalInGrid + timeAfterGrid);

    const float invCycles = 1.f / numCycles;
    const float invTime = 1.f / (timeArrivalInGrid + timeAfterGrid);
    glPushMatrix();
    for (int i = 0; i <= numCycles; i++)
    {
      const float t = i * (timeArrivalInGrid + timeAfterGrid) * invCycles;
      const float position = v * t + y0;
      glLoadIdentity();
      glTranslatef(0, position, 0);
      glRotatef(i * 90.f, 0, 1.f, 0);
      glRotatef(i * 60.f, 0, 0, 1.f);
      anim.add(Keyframe(t * invTime));
    }
    glPopMatrix();
  }

  //
  // Cube sortant du starfield et arrivant dans la forêt
  //
  static void buildArrivingForestCubeAnim()
  {
    Anim & anim = list[arrivingForestCube];
    anim.setAnim(7, Forest::cubeStopDuration);

    glPushMatrix();
    for (unsigned int i = 0; i < 6; ++i)
    {
      const float step = 0.2f * i;
      const float x = step - 1.f;
      const float t = 0.3f * (1.f - x * x);
      const float height = 3.f * step;
      glLoadIdentity();
      glTranslatef(0, height, 0);
      glRotatef(180.f, 1.f, 0, 0);
      glRotatef(-90.f, 0, 1.f, 0);
      glRotatef(180.f * step, 0, 1.f, 0);
      glRotatef(120.f - 120.f * step, 0, 0, 1.f);
      anim.add(Keyframe(t));
    }
    glLoadIdentity();
    anim.add(Keyframe(1.f));
    glPopMatrix();
  }

  //
  // Cube qui se dirige vers le monstre (transition)
  //
  static void buildRampTransitionAnim()
  {
    Anim & anim = list[rampTransition];
    anim.setAnim(6, 2000);

    glPushMatrix();
    glLoadIdentity();
    glTranslatef(PONG_XPOS - 1.00f * PONG_REAL_WIDTH,
		 PONG_YPOS,
		 PONG_ZPOS + 1.10f * PONG_HEIGHT);
    anim.add(Keyframe(0.f));
    glPopMatrix();

    glPushMatrix();
    glLoadIdentity();
    glTranslatef(22.f, -15.f, -17.f);
    anim.add(Keyframe(0.70f, 1.f));
    anim.add(Keyframe(0.76f, 1.f));
    glTranslatef(0.f, +2.f, 0.f);
    anim.add(Keyframe(0.84f));
    glTranslatef(0.f, -2.f, 0.f);
    anim.add(Keyframe(0.92f));
    glRotatef(142.f, 0.f, 1.f, 0.f);
    anim.add(Keyframe(1.f, 1.f, true));
    glPopMatrix();
  }

  //
  // Le noeud du monstre qui grossit
  //
  static void buildRampNodeAnim()
  {
    Anim & anim = list[rampNode];
    anim.setAnim(3, 8000);

    glPushMatrix();
    glLoadIdentity();
    glTranslatef(0, 4.f, 0);
    anim.add(Keyframe(0, 1.f));
    glTranslatef(0, -4.f, 0);
    anim.add(Keyframe(0.5f, 1.f));
    glTranslatef(0, -4.f, 0);
    anim.add(Keyframe(1.f, 0));
    glPopMatrix();
  }

  //
  // Cube qui échappe au monstre
  //
  static void buildRampCubeAnim()
  {
    //
    // FIXME : récupérer les constantes depuis ramp
    //
    static const char* l_x1 = "X[++G]>[-^F]&F<G+[^--H]H";
    static const char* l_x2 = "X>&F<G+H";
    static const char* l_f  = "FX";
    static const char* l_g = "-GX";
    static const char* l_h  = "H+G";

    static const int iter = 7;
    static const float angle = 18.f;
    static const float reduction = 0.9f;
    static const float forward = 1.2f;
    static const unsigned char nb_cubes = 1;

    static const unsigned int cubeJourney = 36000;

    RenderList rampList(10000);

    {
      LSystem::System * l =
	new LSystem::System(rampList, rampEndDate, rampStartDate,
			    Shader::color, none, Texture::none,
			    angle, reduction, forward, nb_cubes,
			    2, 0);
      LSystem::state expended = l->iterate(l_x2, l_f, l_g, l_h, iter);
      l->addStateToList(expended);
      msys_mallocFree(expended);
      delete l;
    }

    const int skip = 8;
    const date lastDate = rampList.last().birthDate();

    Anim & anim = list[rampCube];
    anim.setAnim(1 + lastDate / skip, cubeJourney);

    glPushMatrix();
    glLoadIdentity();
    glTranslatef(22.f, -15.f, -17.f);
    anim.add(Keyframe(0.f, 1.f, true));
    glPopMatrix();


    float lastAngle1 = 0;
    float last = 0.f;
    for (int i = 2 * skip; i < rampList.size - 4 * skip; i += skip)
    {
      glPushMatrix();
      const float *mtx = rampList.elt[i].viewMatrix();
      glLoadMatrixf(mtx);
      last += 1.f + sinf((float)i / 100.f) * 0.1f;
      AnimAddUnsafe(anim, Keyframe(last, 1.f, true));
      glPopMatrix();
    }

    // Faire passer le cube derriere la camera
    glPushMatrix();
    glTranslatef(28.f, -28.f, -24.f);
    AnimAddUnsafe(anim, Keyframe((last += 8.f), 1.f, true));
    glPopMatrix();
    anim.normalize();
  }

  //
  // Cube d'arbre qui grossit
  //
  static void buildGrowingTreeAnim()
  {
    Anim & growAnim = list[growingTree];
    growAnim.setAnim(2, 1000);

    glPushMatrix();
    glLoadIdentity();
    growAnim.add(Keyframe(0.f, 0.f, true)); // FIXME : true nécessaire ?
    growAnim.add(Keyframe(1.f, 1.f, true));
    glPopMatrix();
  }

  //
  // Feuille qui bouge au vent
  //
  static void buildLeafAnim()
  {
    //
    // FIXME : tableau d'anims :-/
    //
    Anim & anim = list[leaf];

    // animation pour les noeuds de profondeur 3 a 13
    /*
    for (int i = 3; i < 14; i++)
    {
      animLeaf[i] = new Anim::Anim(3, 3000, 0);
    */
    anim/*[i]*/.setAnim(3, 3000, 0);
    int i = 13;
      const float amplitude = (float)(i-3.f) / 50.f;
      glPushMatrix();
      glLoadIdentity();
      anim/*[i]*/.add(Keyframe(0.f, 1.f));
      glTranslatef(amplitude, 0.f, 0.f);
      anim/*[i]*/.add(Keyframe(0.7f, 1.f));
      glTranslatef(-amplitude, 0.f, 0.f);
      anim/*[i]*/.add(Keyframe(1.f, 1.f));
      glPopMatrix();
      /*
    }
      */
  }

  //
  // Le noeud de la rope qui grossit
  //
  static void buildRopeNodeAnim()
  {
    Anim & anim = list[ropeNode];
    anim.setAnim(2, 1000);

    glPushMatrix();
    glScalef(0, 0, 0); // FIXME : tester l'effet de bord, qui peut être joli
    anim.add(Keyframe(0, 0));//, true)); // FIXME : la spline fait une différence ?
    glLoadIdentity();
    anim.add(Keyframe(1.f, 1.f));//, true));
    glPopMatrix();

  }

  //
  // Le cube qui suit la rope
  //
  static void buildRopeCubeAnim()
  {
    //
    // FIXME : récupérer les constantes depuis rope
    //
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

    static const unsigned int cubeJourney = 30000;

    RenderList ropeList(10000);

    {
      LSystem::System * l =
	new LSystem::System(ropeList, elevationEndDate, elevationStartDate,
			    Shader::color, none, Texture::none,
			    angle, reduction, forward, nb_cubes,
			    2, 0);
      LSystem::state expended = l->iterate(l_x2, l_f, l_g2, l_h, iter);
      l->addStateToList(expended);
      msys_mallocFree(expended);
      delete l;
    }

    const int skip = 4;
    const date lastDate = ropeList.last().birthDate();

    Anim & anim = list[ropeCube];
    anim.setAnim(1 + lastDate / skip, cubeJourney);

    float lastAngle1 = 0;
    for (int i = 0; i < ropeList.size; i += skip)
    {
      glPushMatrix();
      const float *mtx = ropeList.elt[i].viewMatrix();
      glLoadMatrixf(mtx);
      glRotatef(90.f, 0.f, 0.f, 1.f);
      const float angle1 = i * 2.f;
      const float dist = 2.f;
      glTranslatef(dist * msys_sinf(angle1), dist * msys_cosf(angle1), 0.f);
      glRotatef((float)i * 10.f, 0, 0, 1.f);
      anim.add(Keyframe((float)i / (ropeList.size - 1), 1.f, true));
      glPopMatrix();
    }
  }

  //
  // FIXME : déplacer cette constante
  //
  #define ARRIVAL_DURATION 8000

  static void buildJoinScreenAnim()
  {
    Anim & anim = list[joinScreen];
    anim.setAnim(22, ARRIVAL_DURATION, 0);

    glPushMatrix();
    glLoadIdentity();

    // Montée en douceur
    for (unsigned int i = 0; i <= 20; ++i)
    {
      const float t = i * 0.05f;

      glPushMatrix();
      glLoadIdentity();
      glTranslatef(0, -5.f * (1.f - t) * (1.f - t), 0);
      anim.add(Keyframe(0.7f * t));
      glPopMatrix();

      ++i;
    }

    // État arrêté à la fin
    anim.add(Keyframe(1.f));
    glPopMatrix();
  }
}
