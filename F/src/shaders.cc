//
// Tous les shaders utilisés
//

#include "shaders.hh"

#include "sys/msys.h"
#include "sys/msys_forbidden.h"
#include <GL/gl.h>
#include "sys/glext.h"

#include "array.hh"
#include "intro.hh"
#include "files.hh"
#include "interpolation.hh"
#include "minify.hh"
#include "textureunit.hh"

#define SHADER_DIRECTORY "shaders/"

namespace Shader
{
  Program * list = NULL;
  State state;

  //
  // Cette liste est en partie dupliquée dans :
  // - shaderid.hh
  // - shaders.cc (plus loin)
  // - shaders.hh
  //
  static const char* uniformNames[] =
    {
      // Constant sur la durée de la démo
      "cMap",          // Couleur du matériau (albedo)
      "dMap",          // Deuxième couleur (vitesse, MRT, etc.)
      "nMap",          // Micro relief (bump)
      "mMap",          // Nature du matériau (spéculaire, etc.)
//       "aMap",          // Ambient map
      "sMap",          // Shadow map

      // Changeant à chaque frame
      "texFade",       // coefficient de fondu entre deux textures
      "trans",         // coefficient pour la transition par texture
      "cameraMatrix",  // matrice de la caméra

      "glowness",      // Importance du glow
      "focus",         // Distance de focus
      "skip",          // Gros hack pour le bokeh
      "fade",          // coefficient de fondu
      "fadeLuminance", // couleur du fondu
      "zNear",         // near clipping
      "zFar",          // far clipping
      "invResolution", // inverse screen resolution
      "center",        // screen center
      "time",          // intro.now

      // Spécifique à chaque objet
      "id",            // id
      "shininess",     // Exposant dans le calcul du spéculaire
      "oldModelView",  // Matrice modelview précédente
      "textStep",
    };

  static Array<GLint> listUniforms;

#if !STATIC_SHADERS

# define IFGEN(x) x

  char * LoadSource(const char * filename);

#else

# define IFGEN(x)

  // Liste des shaders généré en DEBUG, pour la RELEASE
  const char *shader_src[] =
    {
# include "shaders_gen.cc"
      NULL
    };

#endif // !STATIC_SHADERS

  static void setUniform1i(uniforms u, int x);

  inline
  static void _getUniformLocations()
  {
    OGL_ERROR_CHECK("Shader::_getUniformLocations, début");
    DBG("Récupération des adresses des uniform");

    // verif de l'enum uniforms
    const int numberOfUniformNames = sizeof(uniformNames) / sizeof(char*);
    assert(numberOfUniformNames == numberOfUniforms);
    assert(list != NULL);

    IFDBG(if (0 == listUniforms.max_size))
    {
      listUniforms.init(numberOfUniforms * numberOfShaders);
    }
    listUniforms.empty();
    for (int j = 0; j < numberOfUniforms; ++j)
      for (int i = 0; i < numberOfShaders; ++i)
 	if (list[i].id() != 0)
	{
	  listUniforms.add(oglGetUniformLocation(list[i].id(), uniformNames[j]));
	  OGL_ERROR_CHECK("Shader::_getUniformLocations, glGetUniformLocation(shader id %d (%d/%d), \"%s\" (%d/%d))",
			  list[i].id(), i, numberOfShaders,
			  uniformNames[j], j, numberOfUniforms);
	}
	else
	{
	  listUniforms.add(-1);
	}

    // Envoi des valeurs d'initialisation
    state.fade = 1.f;
    state.fadeLuminance = 1.f;
    state.trans = 0.f;

    // Une fois pour toutes
    setUniform1i(albedoMap,   Texture::albedo);
    setUniform1i(albedo2Map,  Texture::albedo2);
    setUniform1i(normalMap,   Texture::normal);
    setUniform1i(materialMap, Texture::material);
//     setUniform1i(ambientMap,  Texture::ambient);
    setUniform1i(shadowMap,   Texture::shadow);

    OGL_ERROR_CHECK("Shader::loadShaders, après le _getUniformLocations");
  }

  static void setUniform1i(uniforms u, int x)
  {
    assert(listUniforms.size != 0);
    const GLint* li = &listUniforms[u * numberOfShaders];
    for (int i = 0; i < numberOfShaders; i++)
      if (li[i] >= 0)
      {
        list[i].use();
        oglUniform1i(li[i], x);
      }
  }

  void setUniform1i(id shaderId, uniforms u, int x)
  {
    assert(list[shaderId].isInUse());

    const GLint address = listUniforms[u * numberOfShaders + shaderId];
    if (address >= 0)
      oglUniform1i(address, x);
  }

  void setUniform1f(id shaderId, uniforms u, float x)
  {
    assert(list[shaderId].isInUse());

    const GLint address = listUniforms[u * numberOfShaders + shaderId];
    if (address >= 0)
      oglUniform1f(address, x);
  }

  void setUniform2f(id shaderId, uniforms u, float x, float y)
  {
    assert(list[shaderId].isInUse());

    const GLint address = listUniforms[u * numberOfShaders + shaderId];
    if (address >= 0)
      oglUniform2f(address, x, y);
  }

  void setUniform2fv(id shaderId, uniforms u, float * v)
  {
    assert(list[shaderId].isInUse());

    const GLint address = listUniforms[u * numberOfShaders + shaderId];
    if (address >= 0)
      oglUniform2fv(address, 1, v);
  }

  void setUniform3f(id shaderId, uniforms u, float x, float y, float z)
  {
    assert(list[shaderId].isInUse());

    const GLint address = listUniforms[u * numberOfShaders + shaderId];
    if (address >= 0)
      oglUniform3f(address, x, y, z);
  }

  void setUniform3fv(id shaderId, uniforms u, float * v)
  {
    assert(list[shaderId].isInUse());

    const GLint address = listUniforms[u * numberOfShaders + shaderId];
    if (address >= 0)
      oglUniform3fv(address, 1, v);
  }

  void setUniformM4fv(id shaderId, uniforms u, const float m[16])
  {
    assert(list[shaderId].isInUse());

    const GLint address = listUniforms[u * numberOfShaders + shaderId];
    if (address >= 0)
      oglUniformMatrix4fv(address, 1, 0, m);
  }

#define SET_UNIFORM(t, x) setUniform##t(shaderId, ##x, state.##x)

  void setState(GLuint programId)
  {
    OGL_ERROR_CHECK("Shader::setState, début");

    assert(listUniforms.size != 0);

    int i = 0;
    while (list[i].id() != programId)
    {
      ++i;
      assert(i < numberOfShaders);
    }
    id shaderId = static_cast<id>(i);

    assert(list[shaderId].isInUse());

    //
    // Cette liste est en partie dupliquée dans :
    // - shaderid.hh
    // - shaders.cc (au début)
    // - shaders.hh
    //
    SET_UNIFORM(1f,   textureFade);
    SET_UNIFORM(1f,   trans);
    SET_UNIFORM(M4fv, cameraMatrix);

    SET_UNIFORM(1f,   glowness);
    SET_UNIFORM(1f,   focus);
    SET_UNIFORM(1f,   skip);
    SET_UNIFORM(1f,   fade);
    SET_UNIFORM(1f,   fadeLuminance);
    SET_UNIFORM(1f,   zNear);
    SET_UNIFORM(1f,   zFar);
    SET_UNIFORM(2fv,  invResolution);
    SET_UNIFORM(2fv,  center);

    SET_UNIFORM(1i,  time);

    OGL_ERROR_CHECK("Shader::setState");
  }

  GLuint loadShader(GLenum type, const char * src)
  {
    assert(src != NULL);

    const GLuint shader = oglCreateShader(type);
    assert(shader != 0);

    oglShaderSource(shader, 1, (const GLchar**)&src, NULL);
    oglCompileShader(shader);

#if DEBUG

    GLint wentFine = GL_TRUE;
    oglGetShaderiv(shader, GL_COMPILE_STATUS, &wentFine);

    GLsizei logsize = 0;
    oglGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logsize);

    if (logsize != 0)
    {
      char * log = (char*)msys_mallocAlloc(logsize + 1);
      oglGetShaderInfoLog(shader, logsize, &logsize, log);
      // ----------------------------------
      // Parsing en dur du message d'erreur
      // En l'absence d'erreur :
      // Sur NVidia il n'y a pas de message
      // Sur ATI ça dit "...was successfully..." (avec ou sans \n, selon)
      // Sur Intel ça dit "No errors."
      if (wentFine != GL_TRUE ||
	  strlen(log) > 0 &&
	  strcmp(log, "No errors.") != 0 &&
	  strcmp(log, "Vertex shader was successfully compiled to run on hardware.") != 0 &&
	  strcmp(log, "Vertex shader was successfully compiled to run on hardware.\n") != 0 &&
	  strcmp(log, "Fragment shader was successfully compiled to run on hardware.") != 0)
      {
	DBG("[%s] %s", (wentFine == GL_TRUE ? "OK" : "KO"), log);
      }
      msys_mallocFree(log);
    }

    if(wentFine != GL_TRUE)
    {
      oglDeleteShader(shader);
    }

#endif // DEBUG

    return shader;
  }

  enum shaderObjectId
    {
      stdlibVS,
      stdlibFS,
      lightFunctionsFS,
      blur,
      postEffects,

      minimalVS,
//       minimalFS,

      createVSMVS,
      createVSMFS,

      bumpVS,
      bumpFS,
      lousianneVS,
      syncTestFS,
      floorFS,
      lightSourceFS,
      anisotropicFS,
      stringVS,
      stringFS,
      xyloFS,
      drumFS,

      getAlbedo,

//       ambientEnvMap,

      makeBump,

//       getBlurIntensityDOF,
//       godrays,
      fxaa,

      postVS,
      noPostFS,
//       holgaFS,
//       tvFS,
      hBlurPassFS,
      vBlurPassFS,
      bokehFstPassFS,
      bokehSndPassFS,
      finalPassFS,
      antialiasFS,
      lensOrbsVS,
      lensOrbsFS,

      textVS,
      textFS,

#if !STATIC_SHADERS
      debugNonLinearFS,
      debugNormalsVS,
      debugNormalsFS,
      debugLightVS,
      debugLightFS,
      debugWhiteLightFS,
//       debugZBufferFS,
#endif // DEBUG
    };

  struct shaderObjectSource
  {
    shaderObjectId	id;
    unsigned int	shader;
    GLenum		type; // Vertex, Fragment, etc.
IFGEN(const char *	filename;)
  };

  struct shaderDependency
  {
    id			programId;
    shaderObjectId	objectId;
  };


#define GL_VS GL_VERTEX_SHADER
#define GL_FS GL_FRAGMENT_SHADER

    static shaderObjectSource shaderObjects[] =
      {
	// Shader,       glid, type,       fichier source

	// Libs
	{stdlibVS,         0, GL_VS, IFGEN("stdlib.vert")},
	{stdlibFS,         0, GL_FS, IFGEN("stdlib.frag")},
	{lightFunctionsFS, 0, GL_FS, IFGEN("lightFunctions.frag")},
	{blur,		   0, GL_FS, IFGEN("blur.frag")},
	{postEffects,	   0, GL_FS, IFGEN("postEffects.frag")},

	// Shaders de rendu
	{minimalVS,        0, GL_VS, IFGEN("minimal.vert")},
// 	{minimalFS,        0, GL_FS, IFGEN("minimal.frag")},

	{createVSMVS,      0, GL_VS, IFGEN("createVSM.vert")},
	{createVSMFS,      0, GL_FS, IFGEN("createVSM.frag")},

	{bumpVS,           0, GL_VS, IFGEN("bump.vert")},
	{bumpFS,           0, GL_FS, IFGEN("bump.frag")},
	{lousianneVS,      0, GL_VS, IFGEN("lousianne.vert")},
	{syncTestFS,       0, GL_FS, IFGEN("syncTest.frag")},
	{lightSourceFS,    0, GL_FS, IFGEN("lightSource.frag")},
	{anisotropicFS,	   0, GL_FS, IFGEN("anisotropic.frag")},
        {floorFS,          0, GL_FS, IFGEN("floor.frag")},

	{stringVS,	   0, GL_VS, IFGEN("string.vert")},
	{stringFS,	   0, GL_FS, IFGEN("string.frag")},
	{xyloFS,	   0, GL_FS, IFGEN("xylo.frag")},
	{drumFS,	   0, GL_FS, IFGEN("drum.frag")},

	{getAlbedo,        0, GL_FS, IFGEN("getAlbedo.frag")},

// 	{ambientEnvMap,    0, GL_FS, IFGEN("getAmbient_envmap.frag")},

	{makeBump,         0, GL_FS, IFGEN("make_bump.frag")},

	// Shaders de post processing
// 	{getDepth,         0, GL_FS, IFGEN("getDepth.frag")},
// 	{getBlurIntensityDOF,0,GL_FS,IFGEN("getBlurIntensity_DOF.frag")},
// 	{godrays,          0, GL_FS, IFGEN("godrays.frag")},
	{fxaa,		   0, GL_FS, IFGEN("fxaa.frag")},

	{postVS,           0, GL_VS, IFGEN("postProcessing.vert")},
	{noPostFS,         0, GL_FS, IFGEN("noPostProcessing.frag")},
// 	{holgaFS,          0, GL_FS, IFGEN("holga.frag")},
// 	{tvFS,        	   0, GL_FS, IFGEN("television.frag")},
	{hBlurPassFS,      0, GL_FS, IFGEN("horizontalBlurPass.frag")},
	{vBlurPassFS,      0, GL_FS, IFGEN("verticalBlurPass.frag")},
	{bokehFstPassFS,   0, GL_FS, IFGEN("bokehFirstPass.frag")},
	{bokehSndPassFS,   0, GL_FS, IFGEN("bokehSecondPass.frag")},
	{finalPassFS,      0, GL_FS, IFGEN("finalPass.frag")},
	{antialiasFS,      0, GL_FS, IFGEN("antialiasPass.frag")},
	{lensOrbsVS,       0, GL_VS, IFGEN("lensOrbs.vert")},
	{lensOrbsFS,       0, GL_FS, IFGEN("lensOrbs.frag")},

	{textVS,  	   0, GL_VS, IFGEN("text.vert")},
	{textFS,  	   0, GL_FS, IFGEN("text.frag")},

#if !STATIC_SHADERS
	{debugNonLinearFS, 0, GL_FS, IFGEN("debugNonLinear.frag")},
	{debugNormalsVS,   0, GL_VS, IFGEN("debugNormals.vert")},
	{debugNormalsFS,   0, GL_FS, IFGEN("debugNormals.frag")},
	{debugLightVS,     0, GL_VS, IFGEN("debugLight.vert")},
	{debugLightFS,     0, GL_FS, IFGEN("debugLight.frag")},
	{debugWhiteLightFS,0, GL_FS, IFGEN("debugWhiteLight.frag")},
// 	{debugZBufferFS,   0, GL_FS, IFGEN("debugZBuffer.frag")},
#endif // !STATIC_SHADERS
      };
    const unsigned int numberOfObjects = sizeof(shaderObjects) / sizeof(shaderObjectSource);

    static const shaderDependency dependencies[] =
      {
	// Shader    ,  composants
	{createVSM,	createVSMVS},
	{createVSM,	createVSMFS},

	{parallax,	bumpVS},
	{parallax,	stdlibVS},
	{parallax,	stdlibFS},
	{parallax,	lightFunctionsFS},
	{parallax,	getAlbedo},
// 	{parallax,	ambientEnvMap},
	{parallax,	makeBump},
	{parallax,	bumpFS},

	{anisotropic,	bumpVS},
	{anisotropic,	stdlibVS},
	{anisotropic,	stdlibFS},
	{anisotropic,	lightFunctionsFS},
	{anisotropic,	getAlbedo},
	{anisotropic,	anisotropicFS},

	{string,	stringVS},
	{string,	stdlibVS},
	{string,	stdlibFS},
	{string,	lightFunctionsFS},
	{string,	getAlbedo},
	{string,	stringFS},

	{xylophone,	bumpVS},
	{xylophone,	stdlibVS},
	{xylophone,	stdlibFS},
	{xylophone,	lightFunctionsFS},
	{xylophone,	makeBump},
	{xylophone,	xyloFS},

	{drum,		bumpVS},
	{drum,		stdlibVS},
	{drum,		stdlibFS},
	{drum,		lightFunctionsFS},
	{drum,		makeBump},
	{drum,		drumFS},

        {lousianne,	lousianneVS},
	{lousianne,	stdlibVS},
	{lousianne,	stdlibFS},
	{lousianne,	lightFunctionsFS},
	{lousianne,	getAlbedo},
	{lousianne,	makeBump},
	{lousianne,	bumpFS},

        {syncTest,	bumpVS},
	{syncTest,	stdlibVS},
	{syncTest,	stdlibFS},
	{syncTest,	lightFunctionsFS},
	{syncTest,	getAlbedo},
// 	{syncTest,	ambientEnvMap},
	{syncTest,	makeBump},
	{syncTest,	syncTestFS},

	{floor,	        bumpVS},
	{floor,	        stdlibVS},
	{floor,	        stdlibFS},
	{floor,	        lightFunctionsFS},
	{floor,	        getAlbedo},
	{floor,	        floorFS},
	{floor,	        bumpFS},

        {lightSource,	minimalVS},
	{lightSource,	stdlibVS},
	{lightSource,	stdlibFS},
	{lightSource,	getAlbedo},
	{lightSource,	lightSourceFS},

	{noPost,	postVS},
	{noPost,	noPostFS},

	/*
	{holga,		postVS},
	{holga,		postEffects},
	{holga,		holgaFS},
	*/

	/*
	{television,	postVS},
	{television,	postEffects},
	{television,	tvFS},
	*/

	{finalPass,	postVS},
	{finalPass,	blur},
	{finalPass,	postEffects},
// 	{finalPass,	godrays},
	{finalPass,	finalPassFS},

	{antialias,	postVS},
	{antialias,	fxaa},
	{antialias,	antialiasFS},

	{lensOrbs,	lensOrbsVS},
	{lensOrbs,	lensOrbsFS},

	{hBlurPass,	postVS},
	{hBlurPass,	blur},
	{hBlurPass,	hBlurPassFS},

	{vBlurPass,	postVS},
	{vBlurPass,	blur},
	{vBlurPass,	vBlurPassFS},

	{bokehFstPass,	postVS},
	{bokehFstPass,	bokehFstPassFS},

	{bokehSndPass,	postVS},
	{bokehSndPass,	bokehSndPassFS},

	{text,  	textVS},
	{text,		stdlibVS},
	{text,		stdlibFS},
	{text,  	textFS},

#if !STATIC_SHADERS
	{debugNonLinear,postVS},
	{debugNonLinear,debugNonLinearFS},

	{debugNormals,	debugNormalsVS},
	{debugNormals,	stdlibVS},
	{debugNormals,	stdlibFS},
	{debugNormals,	debugNormalsFS},

	{debugLight,	debugLightVS},
	{debugLight,	stdlibVS},
	{debugLight,	stdlibFS},
	{debugLight,	lightFunctionsFS},
	{debugLight,	debugLightFS},

	{debugWhiteLight, debugLightVS},
	{debugWhiteLight, stdlibVS},
	{debugWhiteLight, stdlibFS},
	{debugWhiteLight, lightFunctionsFS},
	{debugWhiteLight, getAlbedo},
	{debugWhiteLight, debugWhiteLightFS},

// 	{debugZBuffer,	bumpVS},
// 	{debugZBuffer,	getDepth},
// 	{debugZBuffer,	debugZBufferFS},
#endif // !STATIC_SHADERS
      };
    const unsigned int numberOfDependencies = sizeof(dependencies) / sizeof(shaderDependency);


  // callback lors de la modification d'un fichier
#if !STATIC_SHADERS
  static void linkProgram(int i);
  static void attachObjectToProgram(int dep);

  void reloadShaders(const char *file)
  {
    START_TIME_EVAL;
    // on récupère le nom du fichier (sans le chemin)
    file += strlen(SHADER_DIRECTORY);

    // On récupère le shader
    int i;
    for (i = 0; i < numberOfObjects; i++)
      if (strcmp(shaderObjects[i].filename, file) == 0)
        break;

    if (i == numberOfObjects) // fichier non trouvé
    {
      assert(false);
      return;
    }
    shaderObjectSource & object = shaderObjects[i];

    // On le recompile
    char * src = LoadSource(object.filename);
    assert(src != NULL);
    oglDeleteShader(object.shader);
    object.shader = loadShader(object.type, src);

    // On relinke les dépendances
    for (int i = 0; i < numberOfDependencies; i++)
    {
      const shaderDependency & dep = dependencies[i];
      if (dep.objectId == object.id)
      {
        list[dep.programId].kill();
        // on réattache les objets du programme
        for (int j = 0; j < numberOfDependencies; j++)
        {
          if (dependencies[j].programId == dep.programId)
            attachObjectToProgram(j);
        }
        linkProgram(dep.programId);
      }
    }
    _getUniformLocations();
    END_TIME_EVAL("Shaders compilation");
  }

  static void compileDumpAndWatchAllObjects()
  {
    Minifier minifier;

    // Shader du chargement
    {
      char * src = LoadSource("progress.frag");
      minifier.addFile(src, "progress.frag");
      msys_mallocFree(src);
      src = LoadSource("progress.vert");
      minifier.addFile(src, "progress.vert");
      msys_mallocFree(src);
    }

    for (unsigned int i = 0; i < numberOfObjects; ++i)
    {
      shaderObjectSource & object = shaderObjects[i];

      char * src = LoadSource(object.filename);
      assert(src != NULL);
      object.shader = loadShader(object.type, src);
      assert(object.shader != 0);

      // On n'exporte que les shaders qui seront en release
      if (object.id < debugNonLinearFS)
      {
        minifier.addFile(src, object.filename);
      }
      msys_mallocFree(src);

      if (!intro.initDone)
      {
	// Surveille la modification du fichier
	char name[100];
	strcpy(name, SHADER_DIRECTORY);
	strcat(name, object.filename);
	Files::onFileChange(name, reloadShaders);

	// On ne veut pas compter les shaders de debug dans le chargement
	if (object.id < debugNonLinearFS)
	{
	  Loading::update();
	}
      }
    }
  }

#else

  static void compileObject(int i)
  {
    shaderObjectSource & object = shaderObjects[i];
    DBG("Compilation de %d", object.id);
    object.shader = loadShader(object.type, shader_src[i+2]);

    Loading::update();
  }

#endif // !STATIC_SHADERS

  static void attachObjectToProgram(int dep)
  {
    const shaderDependency & dependency = dependencies[dep];
    GLuint shader = 0;
    for (unsigned int j = 0; j < numberOfObjects; ++j)
      if (shaderObjects[j].id == dependency.objectId)
      {
        shader = shaderObjects[j].shader;
        list[dependency.programId].attachShader(shader);
        break;
      }
    assert(shader != 0);
  }

  static void linkProgram(int i)
  {
    DBG("Link du shader : %d", i);
    if (list[i].id() != 0)
    {
	list[i].link();
    }
#if !STATIC_SHADERS
    // On ne veut pas compter les shaders de debug dans le chargement
    if (i < debugNonLinear)
#endif
    {
	Loading::update();
    }
  }

  void loadShaders()
  {
    START_TIME_EVAL;
    DBG("loadShaders...");

    //
    // Première étape : on compile tous les objets
    //

#if !STATIC_SHADERS
    compileDumpAndWatchAllObjects();
#else
    for (unsigned int i = 0; i < numberOfObjects; ++i)
      compileObject(i);
#endif // STATIC_SHADERS
    OGL_ERROR_CHECK("Shader::loadShaders, après les compilations");

    //
    // Deuxième étape : on ajoute les objets aux programmes
    //
    list = new Program[numberOfShaders];
    for (unsigned int i = 0; i < numberOfDependencies; ++i)
      attachObjectToProgram(i);
    OGL_ERROR_CHECK("Shader::loadShaders, après les attach");

    //
    // Troisième étape : on linke les programmes
    //
    for (unsigned int i = 0; i < numberOfShaders; ++i)
      linkProgram(i);
    OGL_ERROR_CHECK("Shader::loadShaders, après les link");

    //
    // Quatrième étape : on peut supprimer les objets
    //  (en debug, on peut les garder de côté)
    //
#if STATIC_SHADERS
    for (unsigned int i = 0; i < numberOfObjects; ++i)
    {
      shaderObjectSource & object = shaderObjects[i];
      oglDeleteShader(object.shader);
      object.shader = 0;
    }
    OGL_ERROR_CHECK("Shader::loadShaders, après les suppressions");
#endif

    //
    // Touche finale : on peut alors récupérer les uniforms
    //
    _getUniformLocations();


    Loading::update();

    END_TIME_EVAL("Shaders compilation");
  }

#if !STATIC_SHADERS

  char * LoadSource(const char * filename)
  {
    char name[100];
    strcpy(name, SHADER_DIRECTORY);
    strcat(name, filename);

    DBG("Compilation de : %s", filename);

    FILE * fp = fopen(name, "r");
    assert(fp != NULL);

    fseek(fp, 0, SEEK_END);
    int size = ftell(fp);
    rewind(fp);

    char * src = (char*)msys_mallocAlloc(size + 1);

    for(int i = 0; i < size; i++)
    {
      const int c = fgetc(fp);
      if (c < 0) {
        src[i] = '\0';
	break;
      }
      src[i] = (char) c;
    }
    fclose(fp);
    return src;
  }
#endif // STATIC_SHADERS

}
