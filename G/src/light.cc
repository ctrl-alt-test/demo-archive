//
// L'animation de la lumière
//

#include "light.hh"

#include "algebra/matrix.hxx"
#include "algebra/vector3.hxx"
#include "algebra/vector4.hxx"
#include "intro.hh"
#include "renderlist.hh"
#include "shaders.hh"
#include "textureunit.hh"
#include "timing.hh"
#include "tweakval.hh"

#include "sys/msys_forbidden.h"


// Pour la représentation de la lumière
#if DEBUG

#include "material.hh"
#include "textureid.hh"

#endif // DEBUG


namespace Light
{
//  float ambientColor[4];
  float light0Position[4];
  float light1Position[4];

  matrix4 lightProjectorMatrix;

  struct KeyframeSource
  {
    date dt;		// 4 octets
    float x;
    float y;
    float z;
    float w;
  };

  //
  // Sources de données
  //
#if DEBUG

  static const KeyframeSource originalLight0Frames[] = {
#include "../data/light/light0.txt"
  };
  static KeyframeSource * loadedLight0Frames = NULL;
  static unsigned int numberOfLoadedLight0Frames = 0;


  static const KeyframeSource originalLight1Frames[] = {
#include "../data/light/light1.txt"
  };
  static KeyframeSource * loadedLight1Frames = NULL;
  static unsigned int numberOfLoadedLight1Frames = 0;

#else

  static const KeyframeSource light0Frames[] = {
#include "../data/light/light0.txt"
  };

  static const KeyframeSource light1Frames[] = {
#include "../data/light/light1.txt"
  };

#endif // DEBUG

  //
  // Récupère la position de la lumière, sans interpolation ni rien (à modifier si besoin)
  //
  void _getLightPosition(date cameraDate,
			 const KeyframeSource * lightFrames, unsigned int numberOfFrames,
			 float * position)
  {
    date keyframeDate = 0;
    float x = 0;
    float y = 0;
    float z = 0;
    float w = 1.f; // Si on ne lit rien, on veut une position valide (0, 0, 0, 0) n'a pas de sens.
    for (unsigned int i = 0; i < numberOfFrames; ++i)
    {
      const KeyframeSource & keyframe = lightFrames[i];
      keyframeDate += keyframe.dt;
      if (keyframeDate > cameraDate)
	break;
      x = keyframe.x;
      y = keyframe.y;
      z = keyframe.z;
      w = keyframe.w;
    }
    position[0] = x;
    position[1] = y;
    position[2] = z;
    position[3] = w;
  }

#if DEBUG

  void _drawRepresentation(const float * position)
  {
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    if (position[3] == 0)
    {
      const vector3f pos(position);

      const float r = norm(pos);
      const float theta = msys_min(PI, msys_max(0.f, msys_acosf(pos.y / r)));
      const float phi = msys_atan2f(pos.z, pos.x);
      glRotatef(270.f - phi * 180.f / PI, 0, 1.f, 0);
      glRotatef(90.f - theta * 180.f / PI, 1.f, 0, 0);
      glTranslatef(0, 0, -r);
      glScalef(2.f, 2.f, 20.f);
    }
    else
    {
      glTranslatef(position[0], position[1], position[2]);
      glScalef(5.f, 5.f, 5.f);
    }
    RenderList::renderObject(VBO_(singleCube), Material::Element(Shader::lightSource, Shader::staticGeomVSM, 1, Texture::lightBulb));
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

  void updateLightPositions(date cameraDate)
  {
    // On utilise la date de la caméra et non la date de l'histoire,
    // pour pouvoir tricher avec des lumières qui dépendent du plan

#if DEBUG
    const KeyframeSource * light0Frames = (loadedLight0Frames != NULL ? loadedLight0Frames : originalLight0Frames);
    const KeyframeSource * light1Frames = (loadedLight1Frames != NULL ? loadedLight1Frames : originalLight1Frames);
    const unsigned int numberOfLight0Frames = (loadedLight0Frames != NULL ? numberOfLoadedLight0Frames : ARRAY_LEN(originalLight0Frames));
    const unsigned int numberOfLight1Frames = (loadedLight1Frames != NULL ? numberOfLoadedLight1Frames : ARRAY_LEN(originalLight1Frames));
#else
    const unsigned int numberOfLight0Frames = ARRAY_LEN(light0Frames);
    const unsigned int numberOfLight1Frames = ARRAY_LEN(light1Frames);
#endif // DEBUG

    _getLightPosition(cameraDate, light0Frames, numberOfLight0Frames, light0Position);
    _getLightPosition(cameraDate, light1Frames, numberOfLight1Frames, light1Position);
  }

  // void placeLights(const matrix4 & transform)
  void placeLights(const matrix4 & cameraMatrix)
  {
      vector4f l0pos(light0Position);
      vector4f l1pos(light1Position);

#if DEBUG
    if (intro.manualLight)
    {
      // Ce bout de code ne va marcher que pour une lumière, vu qu'on
      // ne peut contrôler qu'une lumière
      l0pos = intro.lightPosition;
    }
#endif

    if (l0pos.w == 0.f) normalize(l0pos);
    if (l1pos.w == 0.f) normalize(l1pos);

    // l0pos et l1pos sont en espace monde.
    // On les veut en espace caméra.
    // FIXME: voir si ça pourrait optimiser le vertex shader de les
    // avoir en espace objet
    vector4f l0posCam = cameraMatrix * vector4f(l0pos);
    vector4f l1posCam = cameraMatrix * vector4f(l1pos);

    Shader::uniforms[Shader::l0Position].set(l0posCam);
    Shader::uniforms[Shader::l1Position].set(l1posCam);
  }

  void placeProjector(const matrix4 & objectMatrix)
  {
    setToTextureMatrix(lightProjectorMatrix * objectMatrix, Texture::shadow);
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
    oglActiveTexture(GL_TEXTURE0 + Texture::shadow);

    glLoadMatrixf(bias);
    glMultMatrixf(projection.m);
    glMultMatrixf(orientation.m);

    glGetFloatv(GL_TEXTURE_MATRIX, lightProjectorMatrix.m);

    glMatrixMode(GL_MODELVIEW);
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
      const int result = sscanf(buffer, " { %d , %ff , %ff , %ff , %ff } ,",
				&k.dt, &k.x, &k.y, &k.z, &k.w);
      if (result == 5)
      {
	//DBG("Read frame: {----, %ff, %ff, %ff, %ff},", k.x, k.y, k.z, k.w);
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
    const char * l0filename = "data/light/light0.txt";
    const char * l1filename = "data/light/light1.txt";

    if (loadedLight0Frames != NULL) msys_mallocFree(loadedLight0Frames);
    if (loadedLight1Frames != NULL) msys_mallocFree(loadedLight1Frames);

    // Attention à la taille des données !
    loadedLight0Frames = (KeyframeSource*) msys_mallocAlloc(10000 * sizeof(KeyframeSource));
    loadedLight1Frames = (KeyframeSource*) msys_mallocAlloc(10000 * sizeof(KeyframeSource));
    numberOfLoadedLight0Frames = loadFrames(loadedLight0Frames, l0filename);
    numberOfLoadedLight1Frames = loadFrames(loadedLight1Frames, l1filename);
  }

#endif // DEBUG

}
