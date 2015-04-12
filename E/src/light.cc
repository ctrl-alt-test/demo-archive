//
// L'animation de la lumière
//

#include "light.hh"

#include "factory.hh"
#include "timing.hh"
#include "sys/msys_forbidden.h"

namespace Light
{
  typedef struct
  {
    date dt;		// 4 octets
    float x;
    float y;
    float z;
    /*
    float w;
    */
  } KeyframeSource;

  //
  // Sources de données
  //
#if DEBUG

  static const KeyframeSource originalLightFrames[] = {
#include "../data/light/factory.txt"
  };

  static KeyframeSource * loadedLightFrames = NULL;
  static unsigned int numberOfLoadedFrames = 0;

#else

  static const KeyframeSource lightFrames[] = {
#include "../data/light/factory.txt"
  };

#endif // DEBUG

  //
  // setLight temporaire, sans interpolation ni rien
  //
  void setLight(date storyDate,
		date cameraDate,
                GLfloat * position)
  {
#if DEBUG

    const KeyframeSource * lightFrames = (loadedLightFrames != NULL ? loadedLightFrames : originalLightFrames);
    const unsigned int numberOfFrames = (loadedLightFrames != NULL ? numberOfLoadedFrames : sizeof(originalLightFrames) / sizeof(KeyframeSource));

#else

    const unsigned int numberOfFrames = sizeof(lightFrames) / sizeof(KeyframeSource);

#endif // DEBUG

    date keyframeDate = 0;
    float x = position[0];
    float y = position[1];
    float z = position[2];
    /*
    float w = position[3];
    */
    for (unsigned int i = 0; i < numberOfFrames; ++i)
    {
      const KeyframeSource & keyframe = lightFrames[i];
      keyframeDate += keyframe.dt;
      if (keyframeDate > cameraDate)
	break;
      x = keyframe.x;
      y = keyframe.y;
      z = keyframe.z;
      /*
      w = keyframe.w;
      */
    }
    position[0] = x;
    position[1] = y;
    position[2] = z;
    /*
    position[3] = w;
    */
  }

#if DEBUG

  //
  // Chargement à la volée depuis un fichier C++ like
  //
  static unsigned int loadFrames(KeyframeSource * frames, const char * file)
  {
    DBG("Lecture des frames de lumière depuis %s...\n", file);
    FILE * fd = fopen(file, "r");
    assert(fd != NULL);

    int numberOfFrames = 0;
    char buffer[2000];

    while (fgets(buffer, sizeof (buffer), fd) != NULL)
    {
      KeyframeSource & k = frames[numberOfFrames];
      /*
      const int result = sscanf(buffer, " { %d , %ff , %ff , %ff , %ff } ,",
				&k.dt, &k.x, &k.y, &k.z, &k.w);
      if (result == 5)
      {
	DBG("Read frame: {----, %ff, %ff, %ff, %ff},", k.x, k.y, k.z, k.w);
      */
      const int result = sscanf(buffer, " { %d , %ff , %ff , %ff } ,",
				&k.dt, &k.x, &k.y, &k.z);
      if (result == 4)
      {
	DBG("Read frame: {----, %ff, %ff, %ff},", k.x, k.y, k.z);
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
  void reloadFrames()
  {
    const char * filenames[] = {
      "data/light/factory.txt",
    };
    const unsigned int numberOfFiles = sizeof(filenames) / sizeof(char *);

    // Attention à la taille des données !
    if (loadedLightFrames != NULL)
    {
      msys_mallocFree(loadedLightFrames);
    }
    loadedLightFrames = (KeyframeSource*) msys_mallocAlloc(10000 * sizeof(KeyframeSource));

    numberOfLoadedFrames = 0;
    for (unsigned int i = 0; i < numberOfFiles; ++i)
    {
      numberOfLoadedFrames += loadFrames(loadedLightFrames + numberOfLoadedFrames,
					 filenames[i]);
    }
  }

#endif // DEBUG

}
