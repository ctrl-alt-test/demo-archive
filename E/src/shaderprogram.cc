//
// Shader program
//

#include "shaderprogram.hh"

#include "sys/msys.h"
#include "sys/msys_debug.h"
#include <GL/gl.h>
#include "sys/glext.h"

#include "shaderid.hh"
#include "shaders.hh"

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
    _id(0),
    _shaders(16)
  {
  }

  Program::~Program()
  {
    if (_id != 0)
      oglDeleteProgram(_id);
  }

  bool Program::isInUse() const
  {
    return currentProgramId == _id;
  }

  void Program::use() const
  {
    if (!isInUse())
    {
      currentProgramId = _id;
      oglUseProgram(_id);
      setState(_id);
    }
  }

  void Program::unUse()
  {
    currentProgramId = 0;
    oglUseProgram(0);
  }

  void Program::detachShader(unsigned int shader)
  {
    _shaders.remove(_shaders.find(shader));
    oglDetachShader(_id, shader);
  }

  void Program::attachShader(unsigned int shader)
  {
    _shaders.add(shader);

    if (0 == _id)
    {
      _id = oglCreateProgram();
    }
    assert(_id != 0);
    oglAttachShader(_id, shader);
  }

  void Program::link()
  {
    oglLinkProgram(_id);

#ifdef DEBUG

    GLint wentFine = GL_TRUE;
    oglGetProgramiv(_id, GL_LINK_STATUS, &wentFine);

    GLsizei logsize = 0;
    oglGetProgramiv(_id, GL_INFO_LOG_LENGTH, &logsize);

    if (logsize != 0)
    {
      char * log = (char*) msys_mallocAlloc(logsize + 1);
      oglGetProgramInfoLog(_id, logsize, &logsize, log);
      // ----------------------------------
      // Parsing en dur du message d'erreur
      // En l'absence d'erreur :
      // Sur NVidia il n'y a pas de message
      // Sur ATI ça dit "...linked, ..."
      // Sur Intel ça dit "No errors."
      if (wentFine != GL_TRUE ||
	  strlen(log) > 0 &&
	  strcmp(log, "No errors.") != 0 &&
	  strcmp(log, "Fragment shader(s) linked, vertex shader(s) linked. \n ") != 0)
      {
	DBG("[%s] %s", (wentFine == GL_TRUE ? "OK" : "KO"), log);
      }
      msys_mallocFree(log);
    }

#endif // !DEBUG

  }
}
