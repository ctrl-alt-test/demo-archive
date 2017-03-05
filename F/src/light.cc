//
// L'animation de la lumière
//

#include "light.hh"

#include "sys/msys_forbidden.h"

#include "intro.hh"
#include "matrix.hh"
#include "renderlist.hh"
#include "textureunit.hh"
#include "timing.hh"
#include "tweakval.h"

namespace Light
{
  Light light0;
  Light light1;
  /*
  Light light2;
  */
  matrix4 lightProjectorMatrix;

  struct KeyframeSource
  {
    date dt;		// 4 octets
    float x;
    float y;
    float z;
    /*
    float w;
    */
  };

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
                Light & light)
  {
    static const Light defaultLight = {
      { 0.f, 0.f, 0.f, 1.f }, // Position, OSEF
      { 0.f, 0.f, 0.f, 0.f }, // Ambient
      { _TV(1.00f), _TV(0.88f), _TV(0.75f), 0.f }, // Diffuse
      { 1.f, 1.f, 1.f, 0.f }, // Specular
      _TV(0.002f), // Attenuation
    };
    light = defaultLight;

#if DEBUG

    const KeyframeSource * lightFrames = (loadedLightFrames != NULL ? loadedLightFrames : originalLightFrames);
    const unsigned int numberOfFrames = (loadedLightFrames != NULL ? numberOfLoadedFrames : sizeof(originalLightFrames) / sizeof(KeyframeSource));

#else

    const unsigned int numberOfFrames = sizeof(lightFrames) / sizeof(KeyframeSource);

#endif // DEBUG

    date keyframeDate = 0;
    float x = light.position[0];
    float y = light.position[1];
    float z = light.position[2];
    /*
    float w = light.position[3];
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
    light.position[0] = x;
    light.position[1] = y;
    light.position[2] = z;
    /*
    light.position[3] = w;
    */
  }

  //
  // Cas particulier en dur, pour la lumière de la fenêtre
  //
  void setSecondLight(Light & light)
  {
    static const Light defaultLight = {
      { _TV(-20.f), _TV(140.f), _TV(173.f), 1.f }, // Position
      { _TV(0.23f), _TV(0.36f), _TV(0.51f), 0.f }, // Ambient
      { _TV(1.00f), _TV(0.63f), _TV(0.37f), 0.f }, // Diffuse
      { 1.f, 1.f, 1.f, 0.f }, // Specular
      _TV(0.001f), // Attenuation
    };
    light = defaultLight;
  }

#if DEBUG

  void _drawRepresentation(const float * position)
  {
    glPushMatrix();
    glTranslatef(position[0], position[1], position[2]);
    glScalef(5.f, 5.f, 5.f);
    RenderList::renderObject(VBO::singleCube, Shader::lightSource, Texture::albedoTest);
    glPopMatrix();
  }

  void drawRepresentations()
  {
    if (intro.manualLight)
    {
      _drawRepresentation(intro.lightPosition);
//     _drawRepresentation(light1.position);
//     _drawRepresentation(light2.position);
    }
  }

#endif // DEBUG

  void _applyLightParams(GLenum lightId, const Light & light)
  {
    glLightfv(lightId, GL_AMBIENT, light.ambientColor);
    glLightfv(lightId, GL_DIFFUSE, light.diffuseColor);
    glLightfv(lightId, GL_SPECULAR, light.specularColor);
    glLightf(lightId, GL_LINEAR_ATTENUATION, light.attenuation);

    // Inutile vu qu'on ne lit pas l'information dans le shader
    glEnable(lightId);
  }

  void _placeLight(GLenum lightId, const Light & light,
		   const matrix4 & invTransform)
  {
    float lightPos[4];

#if DEBUG

    if (GL_LIGHT0 == lightId && intro.manualLight)
    {
      // Ce bout de code ne va marcher que pour une lumière, vu qu'on
      // ne peut contrôler qu'une lumière
      lightPos[0] = intro.lightPosition[0];
      lightPos[1] = intro.lightPosition[1];
      lightPos[2] = intro.lightPosition[2];
      lightPos[3] = intro.lightPosition[3];
    }
    else
    {
      lightPos[0] = light.position[0];
      lightPos[1] = light.position[1];
      lightPos[2] = light.position[2];
      lightPos[3] = light.position[3];
    }

#else

    lightPos[0] = light.position[0];
    lightPos[1] = light.position[1];
    lightPos[2] = light.position[2];
    lightPos[3] = light.position[3];

#endif

    applyMatrixToPoint(invTransform, *((vector3f*)lightPos));
    glLightfv(lightId, GL_POSITION, lightPos);
  }

  void applyLightsParams()
  {
    glEnable(GL_LIGHTING);
    glLightModeli(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

    _applyLightParams(GL_LIGHT0, light0);
    _applyLightParams(GL_LIGHT1, light1);
//     _applyLightParams(GL_LIGHT2, light2);
  }

  void setProjectorMatrix()
  {
    // Moving from unit cube [-1,1] to [0,1]
    const float bias[16] = {
      0.5f, 0.0f, 0.0f, 0.0f,
      0.0f, 0.5f, 0.0f, 0.0f,
      0.0f, 0.0f, 0.5f, 0.0f,
      0.5f, 0.5f, 0.5f, 1.0f};

    // Récupérer la projection actuelle
    matrix4 projection;
    matrix4 orientation;
    getFromProjection(projection);
    getFromModelView(orientation);

    // L'appliquer à la texwture
    glMatrixMode(GL_TEXTURE);
    oglActiveTextureARB(GL_TEXTURE0 + Texture::shadow);

    glLoadMatrixf(bias);
    glMultMatrixf(projection.m);
    glMultMatrixf(orientation.m);

    glGetFloatv(GL_TEXTURE_MATRIX, lightProjectorMatrix.m);

    glMatrixMode(GL_MODELVIEW);
  }

  void placeLights(const matrix4 & transform)
  {
    matrix4 invTransform = transform;
    invMatrix(invTransform);

    _placeLight(GL_LIGHT0, light0, invTransform);
    _placeLight(GL_LIGHT1, light1, invTransform);
//     _placeLight(GL_LIGHT2, light2, invTransform);

    setToTextureMatrix(lightProjectorMatrix * transform, Texture::shadow);
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
	//DBG("Read frame: {----, %ff, %ff, %ff},", k.x, k.y, k.z);
	++numberOfFrames;
      }
      else
      {
	const unsigned int last = strlen(buffer) - 1;
	if (buffer[last] == '\n')
	  buffer[last] = '\0';
	//DBG("Nothing to read in: %s", buffer);
      }
    }
    fclose(fd);
    return numberOfFrames;
  }

  //
  // Rechargement à la volée de toutes
  //
  void reloadFrames(const char*)
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
