//
// Tous les shaders utilisés
//

#include "shaders.hh"

#include "sys/msys.h"
#include <GL/gl.h>
#include "sys/glext.h"

#include "array.hh"
#include "interpolation.hh"

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
      "cMap",          // color map
      "c2Map",         // 2nd color map
      "nMap",          // normal map
      "sMap",          // specular map
      "aMap",          // ambient map
      "dMap",          // dynamic map

      // Changeant à chaque frame
      "texFade",       // coefficient de fondu entre deux textures
      "trans",         // coefficient pour la transition par texture
      "cameraMatrix",  // matrice de la caméra

      "glowness",      // Importance du glow
      "trail1Color",
      "trail2Color",
      "trail3Color",
      "dof",           // profondeur de champ
      "fade",          // coefficient de fondu
      "fadeLuminance", // couleur du fondu
      "zNear",         // near clipping
      "zFar",          // far clipping
      "screenCX",      // screen center
      "screenCY",
      "time",          // intro.now

      // Spécifique à chaque objet
      "id",            // id
      "oldModelView",  // Matrice modelview précédente
      "textStep",
    };

  static Array<GLint> listUniforms;

#if DEBUG

# define IFGEN(x) x

  char * LoadSource(const char * filename);
  void generate_cc(char * data, const char * filename, FILE * out);

#else

# define IFGEN(x)

  // Liste des shaders généré en DEBUG, pour la RELEASE
  static const char *shader_src[] =
    {
# include "shaders_gen.cc"
      NULL
    };

#endif

  static void setUniform1i(uniforms u, int x);

  static void init_uniforms()
  {
    OGL_ERROR_CHECK("Shader::init_uniforms, début");
    DBG("Récupération des adresses des uniform");

    // verif de l'enum uniforms
    const int numberOfUniformNames = sizeof(uniformNames) / sizeof(char*);
    assert(numberOfUniformNames == numberOfUniforms);
    assert(list != NULL);

    if (0 == listUniforms.max_size)
    {
      listUniforms.init(numberOfUniforms * numberOfShaders);
    }
    listUniforms.empty();
    for (int j = 0; j < numberOfUniforms; ++j)
      for (int i = 0; i < numberOfShaders; ++i)
 	if (list[i].id() != 0)
	{
	  listUniforms.add(oglGetUniformLocation(list[i].id(), uniformNames[j]));
	  OGL_ERROR_CHECK("Shader::init_uniforms, glGetUniformLocation(shader id %d (%d/%d), \"%s\" (%d/%d))",
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
    setUniform1i(colorMap,    0);
    setUniform1i(sndColorMap, 1);
    setUniform1i(normalMap,   2);
    setUniform1i(specularMap, 3);
    setUniform1i(ambientMap,  4);
    setUniform1i(dynamicMap,  5);
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

  void setUniform4fv(id shaderId, uniforms u, const float m[16])
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
    SET_UNIFORM(1f,  textureFade);
    SET_UNIFORM(1f,  trans);
    SET_UNIFORM(4fv, cameraMatrix);

    SET_UNIFORM(1f,  glowness);
    SET_UNIFORM(3fv, trail1Color);
    SET_UNIFORM(3fv, trail2Color);
    SET_UNIFORM(3fv, trail3Color);
    SET_UNIFORM(1f,  dof);
    SET_UNIFORM(1f,  fade);
    SET_UNIFORM(1f,  fadeLuminance);
    SET_UNIFORM(1f,  zNear);
    SET_UNIFORM(1f,  zFar);
    SET_UNIFORM(1f,  screenCX);
    SET_UNIFORM(1f,  screenCY);

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

#ifdef DEBUG

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

#endif // !DEBUG

    return shader;
  }

  typedef enum
    {
      minimalVS,
      minimalFS,

      bumpVS,
      bumpFS,
      groundFS,

//       albedoWhite,
//       albedoColor,
//       albedoTex0,
//       albedoColorTex0,
//       albedoColorTex0Tex1,
      albedoTripleLight,

      ambientEnvMap,

      makeBump,
      computeTBN,
      texParallax,
      texRegular,
      getSpeed,
      getSpeedColor,

      normalPhong,
      normalBump,

      getDepth,
//       getBlurIntensityDOF,
//       pixelize,
//       dealign,
//       TVdealign,
//       dispersion,
//       radialBlur,
//       axialBlur,
      gaussianBlur,
      motionBlur,
      combineGlow,
      addRegularNoise,
//       addTVNoise,
      addVignetting,
      filter,
      godrays,

      postVS,
      noPostFS,
//       holgaFS,
//       tvFS,
      hBlurPassFS,
      vBlurPassFS,
      finalPassFS,

// #if DEBUG
//       debugZFS,
// #endif

      textVS,
      textFS,
    } shaderObjectId;

  typedef struct
  {
    shaderObjectId	id;
    unsigned int	shader;
    GLenum		type; // Vertex, Fragment, etc.
IFGEN(const char *	filename;)
  } shaderObjectSource;

  typedef struct
  {
    id			programId;
    shaderObjectId	objectId;
  } shaderDependency;


#define GL_VS GL_VERTEX_SHADER
#define GL_FS GL_FRAGMENT_SHADER

  void loadShaders(Loading::ProgressDelegate * pd, int low, int high)
  {
    START_TIME_EVAL;

    shaderObjectSource shaderObjects[] =
      {
	// Shader,       glid, type,       fichier source

	// Shaders de rendu
	{minimalVS,        0, GL_VS, IFGEN("minimal.vert")},
	{minimalFS,        0, GL_FS, IFGEN("minimal.frag")},

	{bumpVS,           0, GL_VS, IFGEN("bump.vert")},
	{bumpFS,           0, GL_FS, IFGEN("bump.frag")},
        {groundFS,         0, GL_FS, IFGEN("ground.frag")},

// 	{albedoWhite,      0, GL_FS, IFGEN("getAlbedo_white.frag")},
// 	{albedoColor,      0, GL_FS, IFGEN("getAlbedo_vColor_only.frag")},
// 	{albedoTex0,       0, GL_FS, IFGEN("getAlbedo_tex0_only.frag")},
// 	{albedoColorTex0,  0, GL_FS, IFGEN("getAlbedo_vColor_modulated_tex0.frag")},
// 	{albedoColorTex0Tex1,0,GL_FS,IFGEN("getAlbedo_vColor_tex0_fade_tex1.frag")},
	{albedoTripleLight,0, GL_FS, IFGEN("getAlbedo_triple_light.frag")},

	{ambientEnvMap,    0, GL_FS, IFGEN("getAmbient_envmap.frag")},

	{computeTBN,       0, GL_VS, IFGEN("computeTBN.vert")},
	{texRegular,       0, GL_FS, IFGEN("getTexCoord_regular.frag")},
	{texParallax,      0, GL_FS, IFGEN("getTexCoord_parallax_mapped.frag")},

	{getSpeed,         0, GL_VS, IFGEN("getSpeed.vert")},
	{getSpeedColor,    0, GL_FS, IFGEN("getSpeedColor.frag")},

	{makeBump,         0, GL_FS, IFGEN("make_bump.frag")},
	{normalPhong,      0, GL_FS, IFGEN("getNormal_phong.frag")},
	{normalBump,       0, GL_FS, IFGEN("getNormal_normal_mapped.frag")},

	// Shaders de post processing
	{getDepth,         0, GL_FS, IFGEN("getDepth.frag")},
// 	{getBlurIntensityDOF,0,GL_FS,IFGEN("getBlurIntensity_DOF.frag")},
// 	{pixelize,         0, GL_FS, IFGEN("pixelize.frag")},
// 	{dealign,          0, GL_FS, IFGEN("dealign.frag")},
// 	{TVdealign,        0, GL_FS, IFGEN("TVdealign.frag")},
// 	{dispersion,       0, GL_FS, IFGEN("dispersion.frag")},
// 	{radialBlur,       0, GL_FS, IFGEN("radialBlur.frag")},
// 	{axialBlur,        0, GL_FS, IFGEN("axialBlur.frag")},
	{gaussianBlur,     0, GL_FS, IFGEN("gaussianBlur.frag")},
	{motionBlur,       0, GL_FS, IFGEN("motionBlur.frag")},
	{combineGlow,      0, GL_FS, IFGEN("combineGlow.frag")},
	{addRegularNoise,  0, GL_FS, IFGEN("addRegularNoise.frag")},
// 	{addTVNoise,       0, GL_FS, IFGEN("addTVNoise.frag")},
	{addVignetting,    0, GL_FS, IFGEN("addVignetting.frag")},
	{filter,           0, GL_FS, IFGEN("filter.frag")},
	{godrays,          0, GL_FS, IFGEN("godrays.frag")},

	{postVS,           0, GL_VS, IFGEN("postProcessing.vert")},
	{noPostFS,         0, GL_FS, IFGEN("noPostProcessing.frag")},
// 	{holgaFS,          0, GL_FS, IFGEN("holga.frag")},
// 	{tvFS,        	   0, GL_FS, IFGEN("television.frag")},
	{hBlurPassFS,      0, GL_FS, IFGEN("horizontalBlurPass.frag")},
	{vBlurPassFS,      0, GL_FS, IFGEN("verticalBlurPass.frag")},
	{finalPassFS,      0, GL_FS, IFGEN("finalPass.frag")},

// #if DEBUG
// 	{debugZFS,         0, GL_FS, IFGEN("debugZBuffer.frag")},
// #endif

	{textVS,  	   0, GL_VS, IFGEN("text.vert")},
	{textFS,  	   0, GL_FS, IFGEN("text.frag")},
      };
    const unsigned int numberOfObjects = sizeof(shaderObjects) / sizeof(shaderObjectSource);

    shaderDependency dependencies[] =
      {
	// Shader    ,  composants
// 	{default,	minimalVS},
// 	{default,	albedoColorTex0},
// 	{default,	texRegular},
// 	{default,	normalPhong},
// 	{default,	getSpeed},      // A virer
// 	{default,	getSpeedColor}, // A virer
// 	{default,	minimalFS},

	{parallax,	bumpVS},
	{parallax,	computeTBN},
	{parallax,	getSpeed},
	{parallax,	getSpeedColor},
	{parallax,	albedoTripleLight},
	{parallax,	ambientEnvMap},
	{parallax,	texParallax},
	{parallax,	normalBump},
	{parallax,	makeBump},
	{parallax,	bumpFS},

	{ground,	bumpVS},
	{ground,	computeTBN},
	{ground,	getSpeed},
	{ground,	getSpeedColor},
	{ground,	albedoTripleLight},
	{ground,	ambientEnvMap},
	{ground,	texParallax},
	{ground,	normalBump},
	{ground,	makeBump},
	{ground,	groundFS},

	{noPost,	postVS},
	{noPost,	noPostFS},

	/*
	{holga,		postVS},
	{holga,		dispersion},
	{holga,		addVignetting},
	{holga,		holgaFS},
	*/

	/*
	{television,	postVS},
	{television,	dealign},
	{television,	TVdealign},
	{television,	addTVNoise},
	{television,	addVignetting},
	{television,	tvFS},
	*/

	{finalPass,	postVS},
	{finalPass,	combineGlow},
	{finalPass,	filter},
	{finalPass,	motionBlur},
	{finalPass,	addRegularNoise},
	{finalPass,	addVignetting},
	{finalPass,	godrays},
	{finalPass,	finalPassFS},

	{hBlurPass,	postVS},
	{hBlurPass,	gaussianBlur},
	{hBlurPass,	hBlurPassFS},

	{vBlurPass,	postVS},
	{vBlurPass,	gaussianBlur},
	{vBlurPass,	vBlurPassFS},

/*
#if DEBUG
	{debugZ,	postVS},
	{debugZ,	getDepth},
	{debugZ,	debugZFS},
#endif
*/

	{text,  	textVS},
	{text,		texRegular},
	{text,  	textFS},
      };
    const unsigned int numberOfDependencies = sizeof(dependencies) / sizeof(shaderDependency);


    int currentLd = low;

    DBG("loadShaders...");

    //
    // Première étape : on compile tous les objets
    //

#if DEBUG

    FILE * out = fopen("src/shaders_gen.cc", "w");
//     FILE * out = fopen("src/shaders_gen.cc", "a"); // Append à cause de loadFirstShader
    for (unsigned int i = 0; i < numberOfObjects; ++i)
    {
      shaderObjectSource & object = shaderObjects[i];

      char * src = LoadSource(object.filename);
      assert(src != NULL);
      object.shader = loadShader(object.type, src);
      assert(object.shader != 0);

      generate_cc(src, object.filename, out);
      msys_mallocFree(src);

      if (pd != NULL)
      {
	const int currentLd = low + (i * (high - low)) / (numberOfObjects + numberOfShaders);
	pd->func( pd->obj, currentLd);
      }
    }
    fclose(out);

#else

    for (unsigned int i = 0; i < numberOfObjects; ++i)
    {
      shaderObjectSource & object = shaderObjects[i];
      object.shader = loadShader(object.type, shader_src[i]);

      if (pd != NULL)
      {
	const int currentLd = low + (i * (high - low)) / (numberOfObjects + numberOfShaders);
	pd->func( pd->obj, currentLd);
      }
    }

#endif // DEBUG

    OGL_ERROR_CHECK("Shader::loadShaders, après les compilations");

    //
    // Deuxième étape : on ajoute les objets aux programmes
    //
    list = new Program[numberOfShaders];

    for (unsigned int i = 0; i < numberOfDependencies; ++i)
    {
      const shaderDependency & dependency = dependencies[i];
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
    OGL_ERROR_CHECK("Shader::loadShaders, après les attach");

    //
    // Troisième étape : on linke les programmes
    //
    for (unsigned int i = 0; i < numberOfShaders; ++i)
    {
      DBG("Link du shader : %d", i);
      if (list[i].id() != 0)
      {
	list[i].link();
      }

      if (pd != NULL)
      {
	const int currentLd = low + ((numberOfObjects + i) * (high - low)) / (numberOfObjects + numberOfShaders);
	pd->func( pd->obj, currentLd);
      }
    }
    OGL_ERROR_CHECK("Shader::loadShaders, après les link");

    //
    // Quatrième étape : on peut supprimer les objets
    //
    for (unsigned int i = 0; i < numberOfObjects; ++i)
    {
      shaderObjectSource & object = shaderObjects[i];
      oglDeleteShader(object.shader);
      object.shader = 0;
    }
    OGL_ERROR_CHECK("Shader::loadShaders, après les suppressions");

    //
    // Touche finale : on peut alors récupérer les uniforms
    //
    init_uniforms();
    OGL_ERROR_CHECK("Shader::loadShaders, après le init_uniforms");

#ifdef DEBUG

    if (pd)
    {
      pd->func( pd->obj, high ); // TEST
    }

#else

    pd->func( pd->obj, high ); // TEST

#endif // DEBUG

    END_TIME_EVAL("Shaders compilation");
  }

#ifdef DEBUG

  char * LoadSource(const char * filename)
  {
    char name[100];
    strcpy(name, "shaders/");
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

  bool search(char c, char* str)
  {
    for (char *ptr = str; *ptr; ptr++)
      if (*ptr == c) return true;
    return false;
  }

  // Compresse et ecrit le shader dans un fichier .cc
  void generate_cc(char * data, const char * filename, FILE * out)
  {
    assert(out != NULL);
    if (data == NULL)
    {
      fprintf(out, "NULL,\n");
      return;
    }
    fprintf(out, "// %s\n", filename);

    bool comment = false;  // simple //
    bool commentm = false; // multiligne /* */
    bool squeeze = true;  // squeeze les espaces
    bool macro = false;
    char * ptr2 = data;
    for (char * ptr = data; *ptr; ptr++) {
      if (*ptr == '/' && ptr[1] == '/')
	comment = true;
      if (*ptr == '/' && ptr[1] == '*')
	commentm = true;
      if (*ptr == '#')
	macro = true;

      if (search(*ptr, "\r\n")) comment = false;
      if (!macro && squeeze && search(*ptr, " \t\r\n")) continue;
      if (search(*ptr, "\r\n")) macro = false;

      squeeze = commentm || comment || search(*ptr, "+-*/=;(){}[],|&<> \t\r\n");

      if (squeeze && ptr2[-1] == ' ' && search(*ptr, "+-*/=;(){},|&<>"))
	ptr2--;

      if (!comment && !commentm) {
	if (*ptr == '\r' || *ptr == '\n') { // echappe le saut de ligne
	  *ptr2++ = '\\';
	  *ptr2++ = 'n';
	}
	else *ptr2++ = *ptr;
      }

      if (commentm && *ptr == '/' && ptr[-1] == '*')
	commentm = false;
    }
    *ptr2 = '\0';
    fprintf(out, "\"%s\",\n", data);
  }

#endif // DEBUG

}
