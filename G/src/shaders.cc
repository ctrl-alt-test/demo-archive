//
// Tous les shaders utilisés
//

#include "shaders.hh"

#include "array.hxx"
#include "intro.hh"
#include "files.hh"
#include "interpolation.hh"
#include "minify.hh"
#include "shaderprogram.hxx"
#include "textureunit.hh"

#include "sys/msys_forbidden.h"
#include "sys/msys_glext.h"
#include "sys/msys_malloc.h"

#define SHADER_DIRECTORY "shaders/"

namespace Shader
{
  Program * list = NULL;

  ShaderUniform uniforms[numberOfUniforms];
  GLint uniformLocations[numberOfUniforms * numberOfShaders];

#define SHADER_ID(x) #x
#if DEBUG
  const char * Names[] = {
    "none",
#include "shaderid.def"
  };

  compilResult compilationResults[ARRAY_LEN(Names)];
  compilResult linkResults[numberOfShaders];
#endif
#undef SHADER_ID


  struct uniformDef
  {
    uniformId			indexCheck;
    ShaderUniform::dataType	dataType;
    const char *		name;
#if DEBUG
    float			min;
    float			max;
#endif // DEBUG
  };

#define NO_MAX 1048576 // 2^20
#define NO_MIN -NO_MAX
#define COL_MIN 0.f
#define COL_MAX 1.f

  //
  // Ce tableau doit respecter l'ordre des uniformId
  //
  const uniformDef uniformDefinitions[] = {
    // Texture units
    { tex0           , ShaderUniform::vec1i, "tex0"          DBGARGS( NO_MIN , NO_MAX  )},
    { tex1           , ShaderUniform::vec1i, "tex1"          DBGARGS( NO_MIN , NO_MAX  )},
    { tex2           , ShaderUniform::vec1i, "tex2"          DBGARGS( NO_MIN , NO_MAX  )},
    { tex3           , ShaderUniform::vec1i, "tex3"          DBGARGS( NO_MIN , NO_MAX  )},
    { tex4           , ShaderUniform::vec1i, "tex4"          DBGARGS( NO_MIN , NO_MAX  )},
    { tex5           , ShaderUniform::vec1i, "tex5"          DBGARGS( NO_MIN , NO_MAX  )},
    { tex6           , ShaderUniform::vec1i, "tex6"          DBGARGS( NO_MIN , NO_MAX  )},
    { tex7           , ShaderUniform::vec1i, "tex7"          DBGARGS( NO_MIN , NO_MAX  )},

    // Light parameters
    { ambient        , ShaderUniform::vec4, "ambient"        DBGARGS( COL_MIN, COL_MAX )},
    { l0Color        , ShaderUniform::vec4, "lightCol[0]"    DBGARGS( COL_MIN, COL_MAX )},
    { l0Position     , ShaderUniform::vec4, "lightPos[0]"    DBGARGS( NO_MIN , NO_MAX  )},
    { l0Attenuation  , ShaderUniform::vec1, "lightAtt[0]"    DBGARGS( 0.f    , NO_MAX  )},
    { l1Color        , ShaderUniform::vec4, "lightCol[1]"    DBGARGS( COL_MIN, COL_MAX )},
    { l1Position     , ShaderUniform::vec4, "lightPos[1]"    DBGARGS( NO_MIN , NO_MAX  )},
    { l1Attenuation  , ShaderUniform::vec1, "lightAtt[1]"    DBGARGS( 0.f    , NO_MAX  )},

    // Wireframe
    { retroEdgeThick , ShaderUniform::vec2, "retroEdgeThick" DBGARGS( 0.f    , NO_MAX  )},
    { retroFaceColor , ShaderUniform::vec4, "retroFaceColor" DBGARGS( COL_MIN, COL_MAX )},
    { retroEdgeColor , ShaderUniform::vec4, "retroEdgeColor" DBGARGS( COL_MIN, COL_MAX )},
    { retroParams    , ShaderUniform::vec4, "retroParams"    DBGARGS( 0.f    , 1.f     )},

    // Lens effects
    { lensFishEye    , ShaderUniform::vec1, "lensFishEye"    DBGARGS( NO_MIN , NO_MAX  )}, // FIXME: limites ?
    { lensGlowPower  , ShaderUniform::vec1, "lensGlowPow"    DBGARGS( .00001f, NO_MAX  )}, // p>0
    { lensStreakPower, ShaderUniform::vec2, "lensStreakPow"  DBGARGS( .00001f, NO_MAX  )}, // p>0
    { lensOrbsSize   , ShaderUniform::vec4, "lensOrbsSize"   DBGARGS( 0.f    , NO_MAX  )},
    { lensOrbsColor  , ShaderUniform::vec4, "lensOrbsCol"    DBGARGS( COL_MIN, COL_MAX )},
    { lensFlareColor , ShaderUniform::vec4, "lensFlareCol"   DBGARGS( COL_MIN, COL_MAX )},
    { lensGlowColor  , ShaderUniform::vec4, "lensGlowCol"    DBGARGS( COL_MIN, COL_MAX )},
    { lensStreakColor, ShaderUniform::vec4, "lensStreakCol"  DBGARGS( COL_MIN, COL_MAX )},
    { focus          , ShaderUniform::vec1, "focus"          DBGARGS( 0.f    , 1.f     )},
    { skip           , ShaderUniform::vec1, "skip"           DBGARGS( 0.f    , NO_MAX  )},

    // Other post processing params
    { vignetting     , ShaderUniform::vec2, "vignetting"     DBGARGS( 0.f    , NO_MAX  )},
    { grain          , ShaderUniform::vec3, "grainParams"    DBGARGS( 0.f    , NO_MAX  )},
    { fade           , ShaderUniform::vec1, "fade"           DBGARGS( 0.f    , 1.f     )},
    { fadeLuminance  , ShaderUniform::vec1, "fadeLuminance"  DBGARGS( 0.f    , 1.f     )},

    { resolution     , ShaderUniform::vec2, "resolution"     DBGARGS( 1.f    , NO_MAX  )},
    { invResolution  , ShaderUniform::vec2, "invResolution"  DBGARGS( .00001f, 1.f     )},
    { center         , ShaderUniform::vec2, "center"         DBGARGS( NO_MIN , NO_MAX  )},
    { pass           , ShaderUniform::vec1i, "pass"          DBGARGS( NO_MIN , NO_MAX  )},

    // Camera and screen
    { cameraMatrix   , ShaderUniform::mat4, "cameraMatrix"   DBGARGS( NO_MIN , NO_MAX  )},
    { zNear          , ShaderUniform::vec1, "zNear"          DBGARGS( .00001f, NO_MAX  )}, // p>0
    { zFar           , ShaderUniform::vec1, "zFar"           DBGARGS( .00001f, NO_MAX  )}, // p>0

    { time           , ShaderUniform::vec1i, "time"          DBGARGS( NO_MIN , NO_MAX  )},

    // G specific: Y position of the camera
    { yPos           , ShaderUniform::vec1, "yPos"           DBGARGS( NO_MIN , NO_MAX  )},
    { oldYPos        , ShaderUniform::vec1, "oldYPos"        DBGARGS( NO_MIN , NO_MAX  )},

    // Legacy parameters (exposed through OpenGL functions for now)
    { skyColor       , ShaderUniform::vec4, "skyCol"         DBGARGS( COL_MIN, COL_MAX )},
    { fogColor       , ShaderUniform::vec4, "fogCol"         DBGARGS( COL_MIN, COL_MAX )},
    { fogDensity     , ShaderUniform::vec1, "fogDensity"     DBGARGS( 0.f    , 2.f     )},

    { objectId       , ShaderUniform::vec1i, "id"            DBGARGS( NO_MIN , NO_MAX  )},
    { birthDate      , ShaderUniform::vec1i, "birthDate"     DBGARGS( NO_MIN , NO_MAX  )},
    { shininess      , ShaderUniform::vec1, "shininess"      DBGARGS( .00001f, NO_MAX  )}, // p>0
    { trans          , ShaderUniform::vec1, "trans"          DBGARGS( 0.f    , NO_MAX  )},
    { oldModelView   , ShaderUniform::mat4, "oldModelView"   DBGARGS( NO_MIN , NO_MAX  )},
    { textStep       , ShaderUniform::vec1, "textStep"       DBGARGS( 0.f    , 1.f     )},
  };


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

  inline
  static void _getUniformLocations(id shaderId)
  {
    const Program& program = list[shaderId];
    int uniformComponentCost = 0;

    for (int i = 0; i < numberOfUniforms; ++i)
    {
      if (program.glId() != 0)
      {
	uniformLocations[numberOfUniforms * shaderId + i] = oglGetUniformLocation(program.glId(), uniforms[i].name());
	OGL_ERROR_CHECK("Shader::_getUniformLocations, glGetUniformLocation(shader id %d (%d/%d), \"%s\" (%d/%d))",
			program.glId(), shaderId, numberOfShaders,
			uniforms[i].name(), i, numberOfUniforms);

#if DEBUG
	switch (uniforms[i].type())
	{
	case ShaderUniform::vec1:
	case ShaderUniform::vec1i:
	  uniformComponentCost += 1; break;
	case ShaderUniform::vec2:
	case ShaderUniform::vec2i:
	  uniformComponentCost += 2; break;
	case ShaderUniform::vec3:
	case ShaderUniform::vec3i:
	  uniformComponentCost += 3; break;
	case ShaderUniform::vec4:
	case ShaderUniform::vec4i:
	case ShaderUniform::mat2:
	  uniformComponentCost += 4; break;
	case ShaderUniform::mat3:
	  uniformComponentCost += 9; break;
	case ShaderUniform::mat4:
	  uniformComponentCost += 16; break;
	default:
	  assert(false);
	}
#endif
      }
      else
      {
	uniformLocations[numberOfUniforms * shaderId + i] = -1;
      }
    }
    DBG("Uniforms cost: %d components", uniformComponentCost);
  }

  void setState(id shaderId)
  {
    OGL_ERROR_CHECK("Shader::setState, début");

    assert(list[shaderId].isInUse());
    GLint address;

    for (int i = 0; i < numberOfUniforms; ++i)
    {
      ShaderUniform& uniform = uniforms[i];
      address = uniformLocations[numberOfUniforms * shaderId + uniform.id()];
      if (address >= 0)
      {
	uniform.send(address DBGARG(shaderId));
      }
    }

#define CHECK_UNIFORM_VALUES 0
#if CHECK_UNIFORM_VALUES
    for (int i = 0; i < numberOfUniforms; ++i)
    {
      ShaderUniform& uniform = uniforms[i];
      address = uniformLocations[numberOfUniforms * shaderId + uniform.id()];
      if (address >= 0)
      {
	ShaderUniform test(uniform.id(), uniforms[i].type(), uniforms[i].name() DBGARGS(NO_MIN, NO_MAX));
	test.get(address, list[shaderId].glId());
	for (int j = 0; j < 16; ++j)
	{
	  assert(test.valuei()[j] == uniform.valuei()[j]);
	}
      }
    }
#endif

    OGL_ERROR_CHECK("Shader::setState");
  }

#if DEBUG
  void reportCompilationStatus()
  {
    compilResult worst = ok;
    for (int i = 0; i < ARRAY_LEN(compilationResults); ++i)
      if (compilationResults[i] > worst)
	worst = compilationResults[i];

    for (int i = 0; i < ARRAY_LEN(linkResults); ++i)
      if (linkResults[i] > worst)
	worst = linkResults[i];

    intro.debug.shaderStatus = worst;
  }
#endif

  GLuint loadShader(GLenum type, const char * src DBGARG(compilResult* result))
  {
    assert(src != NULL);

    const GLuint shader = oglCreateShader(type);
    assert(shader != 0);

    oglShaderSource(shader, 1, (const GLchar**)&src, NULL);
    oglCompileShader(shader);

#if DEBUG

    *result = ok;
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
	*result = (wentFine == GL_TRUE ? compilWarn : compilError);
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

//       createVSMVS,
      createVSMFS,

      bumpVS,
      bumpFS,
      text3DVS,
      particlesVS,
      particlesFS,
      retroFS,
      danceFloorFS,
      seagulTestVS,
      heightmapVS,
      hillsDisplacementVS,
      poppingVS,
      growTestVS,
      artDecoTestVS,
      seaDisplacementVS,
      floatingBoatVS,
      lightSourceFS,
      anisotropicFS,

      getAlbedo,

//       ambientEnvMap,

//       getBlurIntensityDOF,
      godraysFS,
      fxaa,

      attractorFS,

      postVS,
      noPostFS,
      hBlurPassFS,
      vBlurPassFS,
      hStreakPassFS,
      vStreakPassFS,
      bokehFstPassFS,
      bokehSndPassFS,
      finalPassFS,
      skyVS,
      skyFS,
      titleScreenFS,
      titleScreenGlowFS,
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
	{blur,		   0, GL_FS, IFGEN("screenspace/blur.frag")},
	{postEffects,	   0, GL_FS, IFGEN("screenspace/postEffects.frag")},

	// Shaders de rendu
	{minimalVS,        0, GL_VS, IFGEN("minimal.vert")},
// 	{minimalFS,        0, GL_FS, IFGEN("minimal.frag")},

// 	{createVSMVS,      0, GL_VS, IFGEN("createVSM.vert")},
	{createVSMFS,      0, GL_FS, IFGEN("createVSM.frag")},

	{bumpVS,           0, GL_VS, IFGEN("bump.vert")},
	{bumpFS,           0, GL_FS, IFGEN("bump.frag")},
	{text3DVS,         0, GL_VS, IFGEN("text3D.vert")},
	{particlesVS,      0, GL_VS, IFGEN("particles.vert")},
	{particlesFS,      0, GL_FS, IFGEN("particles.frag")},
	{retroFS,          0, GL_FS, IFGEN("retro.frag")},
	{danceFloorFS,     0, GL_FS, IFGEN("danceFloor.frag")},
	{seagulTestVS,     0, GL_VS, IFGEN("seagulTest.vert")},
	{heightmapVS,      0, GL_VS, IFGEN("heightmap.vert")},
	{hillsDisplacementVS,0,GL_VS,IFGEN("hillsDisplacement.vert")},
	{poppingVS,        0, GL_VS, IFGEN("popping.vert")},
	{growTestVS,       0, GL_VS, IFGEN("growTest.vert")},
	{artDecoTestVS,    0, GL_VS, IFGEN("artDecoTest.vert")},
	{seaDisplacementVS,0, GL_VS, IFGEN("seaDisplacement.vert")},
	{floatingBoatVS,   0, GL_VS, IFGEN("floatingBoat.vert")},
	{lightSourceFS,    0, GL_FS, IFGEN("lightSource.frag")},
	{anisotropicFS,	   0, GL_FS, IFGEN("anisotropic.frag")},

	{getAlbedo,        0, GL_FS, IFGEN("getAlbedo.frag")},

// 	{ambientEnvMap,    0, GL_FS, IFGEN("getAmbient_envmap.frag")},

	// Shader de particules
	{attractorFS,      0, GL_FS, IFGEN("attractor.frag")},

	// Shaders de post processing
	{godraysFS,        0, GL_FS, IFGEN("godrays.frag")},
	{fxaa,		   0, GL_FS, IFGEN("screenspace/fxaa.frag")},

	{postVS,           0, GL_VS, IFGEN("screenspace/postProcessing.vert")},
	{noPostFS,         0, GL_FS, IFGEN("screenspace/noPostProcessing.frag")},
	{hBlurPassFS,      0, GL_FS, IFGEN("screenspace/horizontalBlurPass.frag")},
	{vBlurPassFS,      0, GL_FS, IFGEN("screenspace/verticalBlurPass.frag")},
	{hStreakPassFS,    0, GL_FS, IFGEN("screenspace/horizontalStreakPass.frag")},
	{vStreakPassFS,    0, GL_FS, IFGEN("screenspace/verticalStreakPass.frag")},
	{bokehFstPassFS,   0, GL_FS, IFGEN("screenspace/bokehFirstPass.frag")},
	{bokehSndPassFS,   0, GL_FS, IFGEN("screenspace/bokehSecondPass.frag")},
	{skyVS,            0, GL_VS, IFGEN("screenspace/sky.vert")},
	{skyFS,            0, GL_FS, IFGEN("screenspace/sky.frag")},
	{titleScreenFS,    0, GL_FS, IFGEN("screenspace/titleScreen.frag")},
	{titleScreenGlowFS,0, GL_FS, IFGEN("screenspace/titleScreenGlow.frag")},
	{finalPassFS,      0, GL_FS, IFGEN("screenspace/finalPass.frag")},
	{antialiasFS,      0, GL_FS, IFGEN("screenspace/antialiasPass.frag")},
	{lensOrbsVS,       0, GL_VS, IFGEN("screenspace/lensOrbs.vert")},
	{lensOrbsFS,       0, GL_FS, IFGEN("screenspace/lensOrbs.frag")},

	{textVS,  	   0, GL_VS, IFGEN("text.vert")},
	{textFS,  	   0, GL_FS, IFGEN("text.frag")},

#if !STATIC_SHADERS
	{debugNonLinearFS, 0, GL_FS, IFGEN("debug/debugNonLinear.frag")},
	{debugNormalsVS,   0, GL_VS, IFGEN("debug/debugNormals.vert")},
	{debugNormalsFS,   0, GL_FS, IFGEN("debug/debugNormals.frag")},
	{debugLightVS,     0, GL_VS, IFGEN("debug/debugLight.vert")},
	{debugLightFS,     0, GL_FS, IFGEN("debug/debugLight.frag")},
	{debugWhiteLightFS,0, GL_FS, IFGEN("debug/debugWhiteLight.frag")},
// 	{debugZBufferFS,   0, GL_FS, IFGEN("debug/debugZBuffer.frag")},
#endif // !STATIC_SHADERS
      };
    const unsigned int numberOfObjects = sizeof(shaderObjects) / sizeof(shaderObjectSource);

    static const shaderDependency dependencies[] =
      {
	// Shader    ,  composants
	{staticGeomVSM,	bumpVS},
	{staticGeomVSM,	stdlibVS},
	{staticGeomVSM,	createVSMFS},

	{parallax,	bumpVS},
	{parallax,	stdlibVS},
	{parallax,	stdlibFS},
	{parallax,	lightFunctionsFS},
	{parallax,	getAlbedo},
// 	{parallax,	ambientEnvMap},
	{parallax,	bumpFS},

	{anisotropic,	bumpVS},
	{anisotropic,	stdlibVS},
	{anisotropic,	stdlibFS},
	{anisotropic,	lightFunctionsFS},
	{anisotropic,	getAlbedo},
	{anisotropic,	anisotropicFS},

	{retro,		bumpVS},
	{retro,		stdlibVS},
	{retro,		stdlibFS},
	{retro,		lightFunctionsFS},
	{retro,		getAlbedo},
// 	{retro,		ambientEnvMap},
	{retro,		retroFS},

	{danceFloor,	bumpVS},
	{danceFloor,	stdlibVS},
	{danceFloor,	stdlibFS},
	{danceFloor,	lightFunctionsFS},
	{danceFloor,	getAlbedo},
	{danceFloor,	danceFloorFS},

	{hills, 	heightmapVS},
	{hills, 	hillsDisplacementVS},
	{hills, 	stdlibVS},
	{hills, 	stdlibFS},
	{hills, 	lightFunctionsFS},
	{hills, 	getAlbedo},
	{hills, 	retroFS},

	{hillsVSM, 	heightmapVS},
	{hillsVSM, 	hillsDisplacementVS},
	{hillsVSM,	stdlibVS},
	{hillsVSM,	createVSMFS},

        {popping,	poppingVS},
	{popping,	stdlibVS},
	{popping,	stdlibFS},
	{popping,	lightFunctionsFS},
	{popping,	getAlbedo},
	{popping,	retroFS},

	{poppingVSM,	poppingVS},
	{poppingVSM,	stdlibVS},
	{poppingVSM,	createVSMFS},

	{seagulTest,	seagulTestVS},
	{seagulTest,	stdlibVS},
	{seagulTest,	stdlibFS},
	{seagulTest,	lightFunctionsFS},
	{seagulTest,	getAlbedo},
	{seagulTest,	retroFS},

	{seagulTestVSM,	seagulTestVS},
	{seagulTestVSM,	stdlibVS},
	{seagulTestVSM,	createVSMFS},

	{growTest,	growTestVS},
	{growTest,	stdlibVS},
	{growTest,	stdlibFS},
	{growTest,	lightFunctionsFS},
	{growTest,	getAlbedo},
	{growTest,	retroFS},

	{growTestVSM,	growTestVS},
	{growTestVSM,	stdlibVS},
	{growTestVSM,	createVSMFS},

	{artDecoTest,	artDecoTestVS},
	{artDecoTest,	stdlibVS},
	{artDecoTest,	stdlibFS},
	{artDecoTest,	lightFunctionsFS},
	{artDecoTest,	getAlbedo},
	{artDecoTest,	retroFS},

	{artDecoTestVSM,artDecoTestVS},
	{artDecoTestVSM,stdlibVS},
	{artDecoTestVSM,createVSMFS},

	{text3D,	text3DVS},
	{text3D,	stdlibVS},
	{text3D,	stdlibFS},
	{text3D,	lightFunctionsFS},
	{text3D,	getAlbedo},
	{text3D,	lightSourceFS},

	{text3DVSM,	text3DVS},
	{text3DVSM,	stdlibVS},
	{text3DVSM,	createVSMFS},

	{particles,	particlesVS},
	{particles,	stdlibVS},
	{particles,	stdlibFS},
	{particles,	particlesFS},

	{sea,		heightmapVS},
	{sea,		seaDisplacementVS},
	{sea,		stdlibVS},
	{sea,		stdlibFS},
	{sea,		lightFunctionsFS},
	{sea,		getAlbedo},
	{sea,		retroFS},

	{seaVSM,	heightmapVS},
	{seaVSM,	seaDisplacementVS},
	{seaVSM,	stdlibVS},
	{seaVSM,	createVSMFS},

	{floating,	floatingBoatVS},
	{floating,	stdlibVS},
	{floating,	stdlibFS},
	{floating,	lightFunctionsFS},
	{floating,	getAlbedo},
	{floating,	retroFS},

	{floatingVSM,	floatingBoatVS},
	{floatingVSM,	stdlibVS},
	{floatingVSM,	createVSMFS},

	{lightSource,	minimalVS},
	{lightSource,	stdlibVS},
	{lightSource,	stdlibFS},
	{lightSource,	getAlbedo},
	{lightSource,	lightSourceFS},

	{attractor,	postVS},
	{attractor,	attractorFS},

	{noPost,	postVS},
	{noPost,	noPostFS},

	{finalPass,	postVS},
	{finalPass,	blur},
	{finalPass,	postEffects},
	{finalPass,	godraysFS},
	{finalPass,	finalPassFS},

	{sky,		skyVS},
	{sky,		skyFS},

	{titleScreen,	postVS},
	{titleScreen,	titleScreenFS},

	{titleScreenGlow,postVS},
	{titleScreenGlow,titleScreenGlowFS},

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

	{hStreakPass,	postVS},
	{hStreakPass,	blur},
	{hStreakPass,	hStreakPassFS},

	{vStreakPass,	postVS},
	{vStreakPass,	blur},
	{vStreakPass,	vStreakPassFS},

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
// 	{debugZBuffer,	stdLibFS},
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
    object.shader = loadShader(object.type, src DBGARG(&compilationResults[i]));

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
	_getUniformLocations(dep.programId);
	OGL_ERROR_CHECK("Shader::loadShaders, link %d", dep.programId);
      }
    }

#if DEBUG
    reportCompilationStatus();
#endif

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
      object.shader = loadShader(object.type, src DBGARG(&compilationResults[i]));
      assert(object.shader != 0);

      // On n'exporte que les shaders qui seront en release
      if (object.id < debugNonLinearFS)
      {
        minifier.addFile(src, object.filename);
      }
      msys_mallocFree(src);

      // On ne veut pas compter les shaders de debug dans le chargement
      if (!intro.initDone && object.id < debugNonLinearFS)
      {
	Loading::update();
      }
    }

    // Surveille la modification du fichier
    // On sépare compilation et monitoring, pour avoir un log plus lisible
    if (!intro.initDone)
    {
      for (unsigned int i = 0; i < numberOfObjects; ++i)
      {
	char name[100];
	strcpy(name, SHADER_DIRECTORY);
	strcat(name, shaderObjects[i].filename);
	Files::onFileChange(name, reloadShaders);
      }
    }
  }

#else

  static void compileObject(int i)
  {
    shaderObjectSource & object = shaderObjects[i];
    DBG("Compiling %d", object.id);
    object.shader = loadShader(object.type, shader_src[i+2] DBGARG(compilationResults[i]));

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
    DBG("Linking %s", Names[i]);
    if (list[i].glId() != 0)
    {
      list[i].setId((id)i);
#if DEBUG
      list[i].link(&linkResults[i]);
#else
      list[i].link();
#endif
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
    // Initialisation des uniforms
    //
    for (int i = 0; i < numberOfUniforms; ++i)
    {
      const uniformDef& def = uniformDefinitions[i];
      assert(def.indexCheck == i);

      uniforms[i] = ShaderUniform(i, def.dataType, def.name DBGARGS(def.min, def.max));
    }

    // Une fois pour toutes
    uniforms[tex0].set(0);
    uniforms[tex1].set(1);
    uniforms[tex2].set(2);
    uniforms[tex3].set(3);
    uniforms[tex4].set(4);
    uniforms[tex5].set(5);
    uniforms[tex6].set(6);
    uniforms[tex7].set(7);


    //
    // Première étape : on compile tous les objets
    //
#if DEBUG
    for (int i = 0; i < numberOfObjects; ++i)
      compilationResults[i] = notReady;
    for (int i = 0; i < numberOfShaders; ++i)
      linkResults[i] = notReady;
#endif

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
    {
      attachObjectToProgram(i);
      OGL_ERROR_CHECK("Shader::loadShaders, attach %d", i);
    }

    //
    // Troisième étape : on linke les programmes
    //
#if DEBUG
    int max = 0;
    glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS, &max); DBG("Max uniform for vertex shader: %d components", max);
    glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS, &max); DBG("Max uniform for fragment shader: %d components", max);
#endif
    for (unsigned int i = 0; i < numberOfShaders; ++i)
    {
      linkProgram(i);
      _getUniformLocations((id)i);
      OGL_ERROR_CHECK("Shader::loadShaders, link %d", i);
    }

#if DEBUG
    reportCompilationStatus();
#endif

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

    Loading::update();

    END_TIME_EVAL("Shaders compilation");
  }

#if !STATIC_SHADERS

  char * LoadSource(const char * filename)
  {
    char name[100];
    strcpy(name, SHADER_DIRECTORY);
    strcat(name, filename);

    DBG("Compiling %s", filename);

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
