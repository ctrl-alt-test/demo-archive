//
// L'animation de la caméra
//

#include "sys/msys.h"

#include "anim.hh"
#include "basicTypes.hh"
#include "keyframe.hh"
#include "quaternion.hh"
#include "timing.hh"
#include "vector.hh"
#include "sys/msys_forbidden.h"

namespace Camera
{
  typedef struct
  {
    date dt;		// 4 octets
    unsigned char fov;	// 1
    float qx;
    float qy;
    float qz;
    float qw;		// 16 (ou 4 si on passe en char)
    float px;
    float py;
    float pz;		// 12
    unsigned char dof;	// 1
  } KeyframeSource;

  //
  // Sources de données
  // FIXME : mettre en locale dans newAnimation ?
  //
  static const KeyframeSource cameraFrames[] = {
#include "../data/camera/factory.txt"
#include "../data/camera/city.txt"
#include "../data/camera/ramp.txt"
#include "../data/camera/starfield.txt"
#include "../data/camera/forest.txt"
#include "../data/camera/elevation.txt"
#include "../data/camera/screen.txt"
  };


  //
  // Durée totale d'une liste de frames
  //
  static date framesDuration(const KeyframeSource * keyframes,
			     unsigned int numberOfFrames)
  {
    assert(keyframes != NULL);

    date duration = 0;
    for (unsigned int i = 0; i < numberOfFrames; ++i)
    {
      duration += keyframes[i].dt;
    }
    return duration;
  }

  //
  // Transformation compréhensible par Anim à partir d'un KeyframeSource
  //
  static Transformation frameSourceToTransformation(const KeyframeSource & f)
  {
    quaternion q = {f.qx, f.qy, f.qz, f.qw};
    normalize(q);
    const vector3f v = {f.px, f.py, f.pz};
    const Transformation t = {float(f.fov), (float)f.dof/255.f, 0, q, v, true};
    return t;
  }

  //
  // Remplit un Anim déjà alloué à partir d'un tableau de KeyframeSource
  //
  static void addFramesToAnim(Anim::Anim * anim,
			      const KeyframeSource * keyframes,
			      unsigned int numberOfFrames,
			      date duration)
  {
    assert(anim != NULL);
    assert(keyframes != NULL);

    date keyDate = 0;

    for (unsigned int i = 0; i < numberOfFrames; ++i)
    {
      const KeyframeSource & f = keyframes[i];
      const Transformation t = frameSourceToTransformation(f);
      keyDate += f.dt;
      anim->add(Anim::Keyframe(float(keyDate) / duration, t));
    }
  }


  Anim::Anim * newAnimation()
  {
    const unsigned numberOfFrames = sizeof(cameraFrames) / sizeof(KeyframeSource);
    const date duration = framesDuration(cameraFrames, numberOfFrames);

    Anim::Anim * anim = new Anim::Anim(numberOfFrames, duration, 0);
    addFramesToAnim(anim, cameraFrames, numberOfFrames, duration);

    DBG("Duration of whole camera is %d (expected %d)", duration, demoEndDate);

    return anim;
  }

  /*
  #define CAMERA_GEN_FILE "src/camera_gen.cc"
  //
  // Enregistrement des positions de caméra dans un fichier .cc
  //
  void dumpCameraAnimation(const KeyframeSource * keyframes,
			   unsigned int numberOfFrames)
  {
    FILE * out = fopen(CAMERA_GEN_FILE, "w");
    fprintf(out, "// Fichier genere par camera.cc\n");

    if (NULL == out || NULL == keyframes) return;

    // FIXME : attention à la locale, les flottants ne fonctionneront
    //         pas en français !
    //
    // FIXME : on peut organiser les données différemment pour mieux
    //         compresser
    for (unsigned int i = 0; i < numberOfFrames; ++i)
    {
      const KeyframeSource & k = keyframes[i];
      fprintf(out, "{%u, %u,  %f, %f, %f, %f,  %f, %f, %f,  %u},\n",
	      k.dt, k.fov, k.qx, k.qy, k.qz, k.qw, k.px, k.py, k.pz, k.dof);
    }
  }
  */

#if DEBUG

  //
  // FIXME: A simplifier/factoriser/supprimer
  // Créée l'anim de caméra à partir des données d'entrée
  //
  static Anim::Anim * makeAnimation(const KeyframeSource *keyframes,
				    unsigned numberOfFrames, date start)
  {
    date duration = start;
    for (unsigned int i = 0; i < numberOfFrames; ++i)
    {
      duration += keyframes[i].dt;
    }
    Anim::Anim * cameraAnim = new Anim::Anim(1+numberOfFrames, duration, 0);
    date keyDate = start;
    for (unsigned int i = 0; i < numberOfFrames; ++i)
    {
      const KeyframeSource & f = keyframes[i];
      const Transformation t = frameSourceToTransformation(f);
      keyDate += f.dt;
      if (i == 0)
        cameraAnim->add(Anim::Keyframe(0.f, t));
      cameraAnim->add(Anim::Keyframe(float(keyDate) / duration, t));
    }
    return cameraAnim;
  }

  //
  // Chargement à la volée depuis un fichier C++ like
  //
  static unsigned int loadFrames(KeyframeSource * frames, const char * file)
  {
    DBG("Lecture des frames de camera depuis %s...\n", file);
    FILE * fd = fopen(file, "r");
    assert(fd != NULL);

    int numberOfFrames = 0;
    char buffer[2000];

    while (fgets(buffer, sizeof (buffer), fd) != NULL)
    {
      unsigned checksum = 0;
      KeyframeSource & k = frames[numberOfFrames];
      const int result = sscanf(buffer, " { %d , %u , %ff , %ff , %ff , %ff , %ff , %ff , %ff , %u } , // %u",
				&k.dt, &k.fov,
				&k.qx, &k.qy, &k.qz, &k.qw,
				&k.px, &k.py, &k.pz,
				&k.dof, &checksum);
      if (result == 11)
      {
	DBG("Read frame: {----, %u,   %ff, %ff, %ff, %ff,   %ff, %ff, %ff,   %d}, // %u",
	    k.fov,
	    k.qx, k.qy, k.qz, k.qw,
	    k.px, k.py, k.pz,
	    k.dof, checksum);
	++numberOfFrames;
      }
      else
      {
	const unsigned int last = strlen(buffer) - 1;
	if (buffer[last] == '\n')
	  buffer[last] = '\0';
	DBG("Nothing to read in: %s", buffer);
      }
    }
    fclose(fd);
    return numberOfFrames;
  }

  //
  // Rechargement à la volée de toutes
  //
  Anim::Anim * reloadAnimation()
  {
    const char * filenames[] = {
      "data/camera/factory.txt",
      "data/camera/city.txt",
      "data/camera/ramp.txt",
      "data/camera/starfield.txt",
      "data/camera/forest.txt",
      "data/camera/elevation.txt",
      "data/camera/screen.txt"
    };
    const unsigned int numberOfFiles = sizeof(filenames) / sizeof(char *);

    // Attention à la taille des données !
    KeyframeSource * cameraFrames = (KeyframeSource*) msys_mallocAlloc(10000 * sizeof(KeyframeSource));

    unsigned int numberOfFrames = 0;
    for (unsigned int i = 0; i < numberOfFiles; ++i)
    {
      numberOfFrames += loadFrames(cameraFrames + numberOfFrames,
				   filenames[i]);
    }

    const date duration = framesDuration(cameraFrames, numberOfFrames);

    Anim::Anim * anim = new Anim::Anim(numberOfFrames, duration, 0);
    addFramesToAnim(anim, cameraFrames, numberOfFrames, duration);

    msys_mallocFree(cameraFrames);

    return anim;
  }

#endif // DEBUG


  //
  // Modifie la camera pour qu'elle regarde le point pos
  //
  void lookAt(float * m, const vector3f & pos)
  {
    vector3f z = {m[12] - pos.x, m[13] - pos.y, m[14] - pos.z};
    vector3f y = {m[4], m[5], m[6]};
    normalize(z);
    vector3f x = cross(y, z);
    y = cross(z, x);
    m[0] = x.x; m[1] = x.y; m[ 2] = x.z;
    m[4] = y.x; m[5] = y.y; m[ 6] = y.z;
    m[8] = z.x; m[9] = z.y; m[10] = z.z;
  }
}
