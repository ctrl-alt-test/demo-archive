//
// L'animation de la caméra
//

#include "camera.hh"

#include "algebra/matrix.hxx"
#include "algebra/quaternion.hxx"
#include "algebra/vector3.hxx"
#include "algebra/vector4.hxx"
#include "anim.hxx"
#include "array.hxx"
#include "basicTypes.hh"
#include "demo.hh"
#include "interpolation.hh"
#include "intro.hh"
#include "light.hh"
#include "shaders.hh"
#include "timing.hh"
#include "tweakval.hh"
#include "variable.hh"

#include "sys/msys.h"
#include "sys/msys_forbidden.h"
#include <GL/glu.h>

// Pour la représentation de la caméra
#if DEBUG

#include "material.hh"
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

    const unsigned numberOfFrames = Demo::cameraFramesLength;

    Anim::Anim<Camera> * anim = new Anim::Anim<Camera>(numberOfFrames, 0);
    addFramesToAnim(*anim, Demo::cameraFrames, numberOfFrames);

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
    DBG("Duration of whole camera is %d (expected %d)", anim->duration(), demoEndDate);
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
    RenderList::renderObject(VBO_(singleCube), Material::Element(Shader::lightSource, Shader::staticGeomVSM, 1, Texture::albedoTest));
    glPopMatrix();
  }

#endif // DEBUG


  static void placeCamera(const matrix4 & cameraPosition,
			  float aspectRatio, float fov, float focus,
			  float zNear, float zFar)
  {
    Shader::uniforms[Shader::focus].set(focus);
    Shader::uniforms[Shader::zNear].set(zNear);
    Shader::uniforms[Shader::zFar].set(zFar);

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
    Shader::uniforms[Shader::skip].set(0.f);

    // Projection
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(fov, aspectRatio, zNear, zFar); // FIXME : vrai frustrum

    // Position
    setToModelView(cameraPosition);
    Shader::uniforms[Shader::cameraMatrix].set(cameraPosition);
  }

  void placeLightCamera()
  {
    float fov = _TV(120.f);
    float zNear = _TV(20.f);
    float zFar = _TV(200.f);

#if DEBUG
    const float * position = (intro.manualLight ? intro.lightPosition : Light::light0Position);
#else
    const float * position = Light::light0Position;
#endif // DEBUG


    if (position[3] == 0)
    {
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();

      // Ombres dynamiques à la main, toujours comme des pros :)
      const bool tunnelCase = (intro.now.story >= _TV(238000) &&
			       intro.now.story < _TV(254100));
      const bool cityCase = intro.now.story >= _TV(254100);

      float projSize = 500.f;
      float zProjSize = 500.f;
      if (tunnelCase)
      {
	projSize = _TV(80.f);
	zProjSize = _TV(100.f);
      } else if (cityCase)
      {
	projSize = _TV(300.f);
	zProjSize = _TV(1000.f);
      }
      glOrtho(-projSize, projSize, -projSize, projSize, -zProjSize, zProjSize);

      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();

      const vector3f pos(position);

      const float r = norm(pos);
      const float theta = msys_min(PI, msys_max(0.f, msys_acosf(pos.y / r)));
      const float phi = msys_atan2f(pos.z, pos.x);

      glRotatef(90.f - theta * 180.f / PI, 1.f, 0, 0);
      glRotatef(270.f + phi * 180.f / PI, 0, 1.f, 0);
      if (tunnelCase)
      {
	const Camera camera = intro.cameraAnim->get(intro.now.camera);
	glTranslatef(_TV(0.f), _TV(0.f), -camera.t.v.z + _TV(100.f));
      }
      else if (cityCase)
      {
	const Camera camera = intro.cameraAnim->get(intro.now.camera);
	vector3f p = -camera.t.v;

	const matrix4 matrix = computeMatrix(camera.t);
	const matrix4 invMatrix = computeInvMatrix(camera.t);
	vector4f at = _TV(1) ? matrix * vector4f::uz : invMatrix * vector4f::uz;

	vector3f groundAt = vector3f(at.x, 0.f, at.z);
	p += _TV(250.f) * groundAt;

	glTranslatef(p.x, 0.f, p.z + _TV(0.f));
      }
    }
    else
    {
      // FIXME : placement de la lumière en dur, mais à l'avenir on
      //         voudra que ce soit dynamique
      glMatrixMode(GL_PROJECTION);
      glLoadIdentity();
      gluPerspective(fov, 1.f, zNear, zFar);

      glMatrixMode(GL_MODELVIEW);
      glLoadIdentity();
      glRotatef(90.f, 1.f, 0, 0);
      glTranslatef(-position[0], -position[1], -position[2]);
    }
  }

  Camera getCamera(date cameraDate)
  {
    Camera camera = intro.cameraAnim->get(cameraDate);

    float amplitude[2];
    float frequency[2];
    VAR(cameraShakeAmplitude)->get((float)cameraDate, 2, amplitude);
    VAR(cameraShakeFrequency)->get((float)cameraDate, 2, frequency);
    vector3f shake(amplitude[0] * msys_sinf(PI * 0.002f * cameraDate * frequency[0]),
		   amplitude[1] * msys_cosf(PI * 0.002f * cameraDate * frequency[1]),
		   0);
    camera.t.v += rotate(conjugate(camera.t.q), shake);

    return camera;
  }

  void placeCamera()
  {
    intro.zNear = _TV(2.f); // Idéalement, déterminer dynamiquement
    intro.zFar = _TV(1000.f);

    const Camera camera = getCamera(intro.now.camera);

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

  void orthoProj(double width, double height)
  {
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, width, 0, height);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
  }
}
