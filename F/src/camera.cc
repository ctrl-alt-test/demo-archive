//
// L'animation de la caméra
//

#include "camera.hh"

#include "sys/msys.h"
#include "sys/msys_forbidden.h"
#include "sys/msys_debug.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "tweakval.h"

#include "anim.hh"
#include "basicTypes.hh"
#include "interpolation.hh"
#include "intro.hh"
#include "light.hh"
#include "quaternion.hh"
#include "shaders.hh"
#include "timing.hh"
#include "vector.hh"


// Pour la représentation de la caméra
#if DEBUG

#include "array.hh"
#include "renderlist.hh"

#endif // DEBUG


namespace Camera
{
  Camera interpolate(const Camera & c1,
		     const Camera & c2,
		     float weight)
  {
    assert(weight >= 0 && weight <= 1.f);

    Camera c;
    c.t = interpolate(c1.t, c2.t, weight);
    c.fov = mix(c1.fov, c2.fov, weight);
    c.focus = mix(c1.focus, c2.focus, weight);

    return c;
  }

  Camera interpolate(const Camera & c1,
		     const Camera & c2,
		     const Camera & c3,
		     const Camera & c4,
		     float weights[4])
  {
    Camera c;
    c.t = interpolate(c1.t, c2.t, c3.t, c4.t, weights);
    c.fov = (c1.fov * weights[0] +
	     c2.fov * weights[1] +
	     c3.fov * weights[2] +
	     c4.fov * weights[3]);
    c.focus = (c1.focus * weights[0] +
	       c2.focus * weights[1] +
	       c3.focus * weights[2] +
	       c4.focus * weights[3]);

    return c;
  }

  struct KeyframeSource
  {
    date dt;		// 4 octets
    unsigned char fov;	// 1
    char  qx;
    char  qy;
    char  qz;
    char  qw;		// 4
    float px;
    float py;
    float pz;		// 12
    unsigned char focus;	// 1
  };

  //
  // Sources de données
  //
  static const KeyframeSource cameraFrames[] = {
#include "../data/camera/factory.txt"
// autres fichiers de cameras
  };


  //
  // Durée totale d'une liste de frames
  //
  /*
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
  */

  //
  // Transformation compréhensible par Anim à partir d'un KeyframeSource
  //
  static Camera cameraFromSource(const KeyframeSource & f)
  {
    quaternion q(f.qx, f.qy, f.qz, f.qw);
    normalize(q);
    const vector3f v(f.px, f.py, f.pz);
    const Transformation t(q, v);

    const Camera camera = {t, float(f.fov), float(f.focus) / 255.f};
    return camera;
  }

  //
  // Remplit un Anim déjà alloué à partir d'un tableau de KeyframeSource
  //
  static void addFramesToAnim(Anim::Anim<Camera> & anim,
			      const KeyframeSource * keyframes,
			      unsigned int numberOfFrames)
  {
    assert(keyframes != NULL);

    date keyDate = 0;

    for (unsigned int i = 0; i < numberOfFrames; ++i)
    {
      const KeyframeSource & f = keyframes[i];
      keyDate += f.dt;
      anim.add(keyDate, cameraFromSource(f));
    }
  }

  Anim::Anim<Camera> * newAnimation()
  {
    START_TIME_EVAL;

    const unsigned numberOfFrames = sizeof(cameraFrames) / sizeof(KeyframeSource);

    Anim::Anim<Camera> * anim = new Anim::Anim<Camera>(numberOfFrames, 0);
    addFramesToAnim(*anim, cameraFrames, numberOfFrames);

    DBG("Duration of whole camera is %d (expected %d)", anim->duration(), demoEndDate);

    END_TIME_EVAL("Camera animation setup");

    return anim;
  }


#if DEBUG

  //
  // FIXME: A simplifier/factoriser/supprimer
  // Créée l'anim de caméra à partir des données d'entrée
  //
  static Anim::Anim<Camera> * makeAnimation(const KeyframeSource *keyframes,
					    unsigned numberOfFrames, date start)
  {
    Anim::Anim<Camera> * cameraAnim = new Anim::Anim<Camera>(1 + numberOfFrames, 0);
    date keyDate = start;
    for (unsigned int i = 0; i < numberOfFrames; ++i)
    {
      const KeyframeSource & f = keyframes[i];
      const Camera frame = cameraFromSource(f);
      keyDate += f.dt;
      if (i == 0)
        cameraAnim->add(0, frame);
      cameraAnim->add(keyDate, frame);
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
      const int result = sscanf(buffer, " { %d , %u , %d , %d , %d , %d , %ff , %ff , %ff , %u } , // %u",
				&k.dt, &k.fov,
				&k.qx, &k.qy, &k.qz, &k.qw,
				&k.px, &k.py, &k.pz,
				&k.focus, &checksum);
      if (result == 11)
      {
	//DBG("Read frame: {----, %u,   %d, %d, %d, %d,   %ff, %ff, %ff,   %d}, // %u",
	//    k.fov,
	//    k.qx, k.qy, k.qz, k.qw,
	//    k.px, k.py, k.pz,
	//    k.focus, checksum);
	++numberOfFrames;

	/*
	{
	  quaternion q(k.qx, k.qy, k.qz, k.qw);
	  normalize(q);
	  const compressedQuaternion cq = compressQuaternion(q);
	  quaternion q2 = cq.q();

	  DBG(" {%d, %u,  %d, %d, %d, %d,  %ff, %ff, %ff, %d}, // %u",
	      k.dt, k.fov,
	      cq.x, cq.y, cq.z, cq.w,
	      k.px, k.py, k.pz,
	      k.focus, checksum);

// 	  const float error = 100.f * dist(q, q2);
// 	  DBG("Erreur: %.2f%% \t {%d, %d, %d, %d}\t-> {%.2f, %.2f, %.2f, %.2f}\t{%.2f, %.2f, %.2f, %.2f}",
// 	      100.f * dist(q, q2),
// 	      cq.x, cq.y, cq.z, cq.w,
// 	      q2.x, q2.y, q2.z, q2.w,
// 	      q.x, q.y, q.z, q.w);
	}
	*/

      }
      else
      {
	const unsigned int last = strlen(buffer) - 1;
	if (buffer[last] == '\n')
	  buffer[last] = '\0';
	//DBG("%s", buffer);
      }
    }
    fclose(fd);

    return numberOfFrames;
  }


  //
  // Rechargement à la volée de toutes
  //
  Anim::Anim<Camera> * reloadAnimation()
  {
    const char * filenames[] = {
      "data/camera/factory.txt",
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

    Anim::Anim<Camera> * anim = new Anim::Anim<Camera>(numberOfFrames, 0);
    addFramesToAnim(*anim, cameraFrames, numberOfFrames);

    msys_mallocFree(cameraFrames);

    return anim;
  }

  /*
  Array<VBO::vertex> * vertices = NULL;

  void generateMesh()
  {
    assert(NULL == vertices);

    const unsigned int facesPerCylinder = 10;
    const unsigned int supposedNumberOfVertices = facesPerCylinder * 3 * 12 + 24;
    vertices = new Array<VBO::vertex>(supposedNumberOfVertices);

    float y1 = -0.5f;
    float l1 = 0;
    unsigned int index = 0;
    for (unsigned int j = 1; j <= RESOLUTION_LAT; ++j)
    {
      const float lat = PI * (float(j) / RESOLUTION_LAT - 0.5f);
      const float y2 = 0.5f * msys_sinf(lat);
      const float l2 = 0.5f * msys_cosf(lat);
      float lon1 = 0;
      for (unsigned int i = 1; i <= RESOLUTION_LON; ++i)
      {
	const float lon2 = (2.f * PI * i) / RESOLUTION_LON;
	const float x11 = l1 * msys_cosf(lon1);
	const float x12 = l1 * msys_cosf(lon2);
	const float x21 = l2 * msys_cosf(lon1);
	const float x22 = l2 * msys_cosf(lon2);

	const float z11 = l1 * msys_sinf(lon1);
	const float z12 = l1 * msys_sinf(lon2);
	const float z21 = l2 * msys_sinf(lon1);
	const float z22 = l2 * msys_sinf(lon2);

	vertices[index++] = sphereVertex(x21, y2, z21);
	vertices[index++] = sphereVertex(x22, y2, z22);
	vertices[index++] = sphereVertex(x12, y1, z12);
	vertices[index++] = sphereVertex(x11, y1, z11);
	lon1 = lon2;
      }
      y1 = y2;
      l1 = l2;
    }
  }
  */

  void drawRepresentation(const Camera & camera)
  {
    glPushMatrix();

    matrix4 matrix = computeMatrix(camera.t);

    glMultMatrixf(matrix.m);
    glScalef(10.f, 10.f, 10.f);
    RenderList::renderObject(VBO::singleCube, Shader::parallax, Texture::albedoTest);
    glPopMatrix();
  }

#endif // DEBUG


  static void placeCamera(const matrix4 & cameraPosition,
			  float aspectRatio, float fov, float focus,
			  float zNear, float zFar)
  {
    Shader::state.focus = focus;
    Shader::state.zNear = zNear;
    Shader::state.zFar = zFar;

    // Activer ou non le test de profondeur pour le bokeh :
    //
    // skip à 0 :
    //  - objets flous devant objets nets : OK :D
    //  - objets nets devant objets flous : KO :(
    //
    // skip à 1 :
    //  - objets flous devant objets nets : OK :D
    //  - objets nets devant objets flous : KO :(
    //
    // Dans la majorité des cas on veut 0, pour les plans qui posent
    // problème, faire une exception
    Shader::state.skip = 0;

    // Projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, aspectRatio, zNear, zFar); // FIXME : vrai frustrum

    // Position
    setToModelView(cameraPosition);

    msys_memcpy(Shader::state.cameraMatrix, cameraPosition.m,
		sizeof(Shader::state.cameraMatrix));
  }

  void placeLightCamera()
  {
    float fov = 120.f;
    float zNear = _TV(23.f);
    float zFar = _TV(170.f);

    // Ombres dynamiques à la main, comme des pros :)
    if (intro.now.story >= 44000 && intro.now.story < 52000)
    {
      // Gros plan sur la boite à musique
      fov = _TV(30.f);
      zNear = _TV(49.f);
      zFar = _TV(62.f);
    }

#if DEBUG
    const float * position = (intro.manualLight ? intro.lightPosition : Light::light0.position);
#else
    const float * position = Light::light0.position;
#endif // DEBUG

    // FIXME : placement de la lumière en dur, mais à l'avenir on
    //         voudra que ce soit dynamique, et on voudra passer
    //         par cet appel
    // placeCamera(lightCameraPosition, 1.f, fov, 0, zNear, zFar);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, 1.f, zNear, zFar);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glRotatef(90.f, 1.f, 0, 0);
    glRotatef(10.f, 0, 0, 1.f);
    glTranslatef(-position[0], -position[1], -position[2]);
  }

  void placeCamera()
  {
    intro.zNear = _TV(3.f); // Idéalement, déterminer dynamiquement
    intro.zFar = _TV(700.f);

    const Camera camera = intro.cameraAnim->get(intro.now.camera);

#if DEBUG
    if (intro.manualCamera)
    {
      placeCamera(intro.cameraPosition,
		  intro.aspectRatioVP, float(intro.fov), intro.focus,
		  intro.zNear, intro.zFar);
    }
    else
#endif // DEBUG
    {
      matrix4 cameraPosition = computeInvMatrix(camera.t);

      placeCamera(cameraPosition,
		  intro.aspectRatioVP, camera.fov, camera.focus,
		  intro.zNear, intro.zFar);
    }
  }

  //
  // Modifie la camera pour qu'elle regarde le point pos
  //
  void lookAt(float * m, const vector3f & pos)
  {
    vector3f z(m[12] - pos.x, m[13] - pos.y, m[14] - pos.z);
    vector3f y(m[4], m[5], m[6]);
    normalize(z);
    vector3f x = y ^ z;
    y = z ^ x;
    m[0] = x.x; m[1] = x.y; m[ 2] = x.z;
    m[4] = y.x; m[5] = y.y; m[ 6] = y.z;
    m[8] = z.x; m[9] = z.y; m[10] = z.z;
  }

  void orthoProj(double width, double height)
  {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width, 0, height);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
  }
}
