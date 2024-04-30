//
// Shader program
//

#include "shaderprogram.hh"

#include "array.hxx"
#include "mesh/vertex.hh"
#include "shaderid.hh"
#include "shaders.hh"

#include "sys/msys_glext.h"
#include "sys/msys_libc.h"
#include "sys/msys_malloc.h"

//
// void glCompileShader(GLuint shader)
//
// GLuint glCreateProgram(void)
// void glDeleteProgram(GLuint program)
//
// void glCompileShader(GLuint shader)
//
// void glAttachShader(GLuint program, GLuint shader)
// void glDetachShader(GLuint program, GLuint shader)
//
// void glLinkProgram(GLuintprogram)
//
// void glUseProgram(GLuint program)
//

namespace Shader
{
  static unsigned int currentProgramId = 0;


  Program::Program():
    _id(numberOfShaders),
    _gl_id(0),
    _shaders(16)
  {
  }

  Program::~Program()
  {
    if (_gl_id != 0)
      oglDeleteProgram(_gl_id);
  }

  void Program::kill()
  {
    _shaders.size = 0;
    oglDeleteProgram(_gl_id);
    _gl_id = 0;
    unUse();
  }

  bool Program::isInUse() const
  {
    return currentProgramId == _id;
  }

  void Program::use() const
  {
    OGL_ERROR_CHECK("Shader::Program::use(), début");
    if (!isInUse())
    {
      currentProgramId = _id;
      oglUseProgram(_gl_id);
      OGL_ERROR_CHECK("Shader::Program::use(), id=%d, glid=%d", _id, _gl_id);
    }
    setState(_id);
  }

  void Program::unUse()
  {
    currentProgramId = 0;
    oglUseProgram(0);
  }

  void Program::detachShader(unsigned int shader)
  {
    _shaders.remove(_shaders.find(shader));
    oglDetachShader(_gl_id, shader);
  }

  void Program::attachShader(unsigned int shader)
  {
    _shaders.add(shader);

    if (0 == _gl_id)
    {
      _gl_id = oglCreateProgram();
    }
    assert(_gl_id != 0);
    oglAttachShader(_gl_id, shader);
  }

  static void setAttributeLocations(unsigned int id)
  {
    //
    // Cette liste est dupliquée dans :
    // - vertex.hh
    //
    static const char* attributeNames[] =
      {
#if VERTEX_ATTR_TANGENT
	"aTan",  // Tangente
#endif
#if VERTEX_ATTR_COLOR
	"aCol",  // Couleur
#endif
#if VERTEX_ATTR_BARY_COORD
	"aBary", // Coordonnées barycentriques
#endif
#if VERTEX_ATTR_ID
	"aId",    // Identifiant
#endif
	NULL, // Le tableau reste valide si on désactive tout
      };
    assert(ARRAY_LEN(attributeNames) == vertex::numberOfAttributes + 1);

//     int nvidia_workaround = 3;
    for (int i = 0; i < vertex::numberOfAttributes; ++i)
    {
      oglBindAttribLocation(id, vertex::attributeLocation((vertex::attribute)i), attributeNames[i]);
      OGL_ERROR_CHECK("Shader::Program::_bindAttributes, %d: %s", i, attributeNames[i]);
    }
  }

#if DEBUG

  void Program::link(compilResult* result)
  {
    setAttributeLocations(_gl_id);
    oglLinkProgram(_gl_id);

    *result = ok;
    GLint wentFine = GL_TRUE;
    oglGetProgramiv(_gl_id, GL_LINK_STATUS, &wentFine);

    GLsizei logsize = 0;
    oglGetProgramiv(_gl_id, GL_INFO_LOG_LENGTH, &logsize);

    if (logsize != 0)
    {
      char * log = (char*) msys_mallocAlloc(logsize + 1);
      oglGetProgramInfoLog(_gl_id, logsize, &logsize, log);
      // ----------------------------------
      // Parsing en dur du message d'erreur
      // En l'absence d'erreur :
      // Sur NVidia il n'y a pas de message
      // Sur ATI ça dit "...linked, ..."
      // Sur Intel ça dit "No errors."
      if (wentFine != GL_TRUE ||
	  strlen(log) > 0 &&
	  strcmp(log, "No errors.") != 0 &&
	  strcmp(log, "Fragment shader(s) linked, vertex shader(s) linked. \n ") != 0 &&
	  strcmp(log, "Fragment shader(s) linked, vertex shader(s) linked. \n") != 0)
      {
	*result = (wentFine == GL_TRUE ? linkWarn : linkError);
	DBG("[%s] %s", (wentFine == GL_TRUE ? "OK" : "KO"), log);
      }
      msys_mallocFree(log);
    }

    if (wentFine != GL_TRUE)
      kill();
  }

#else

  void Program::link()
  {
    setAttributeLocations(_gl_id);
    oglLinkProgram(_gl_id);
  }

#endif // DEBUG

}
