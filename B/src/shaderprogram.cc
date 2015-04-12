//
// Shader program
//

#include "shaderprogram.hh"

#include "sys/msys.h"
#include "sys/msys_debug.h"
#include <GL/gl.h>
#include "sys/glext.h"

//
// void glCompileShader(GLuint shader)
//
// GLuint glCreateProgram(void)
// void glDeleteProgram(GLuint program)
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
    _vertexShader(0),
    _fragmentShader(0)
  {
  }

  Program::~Program()
  {
    if (_id != 0)
      oglDeleteProgram(_id);
  }

  void Program::use() const
  {
    if (currentProgramId != _id)
    {
      currentProgramId = _id;
      oglUseProgram(_id);
    }
  }

  void Program::unUse()
  {
    currentProgramId = 0;
    oglUseProgram(0);
  }

  void Program::detachVertexShader()
  {
    if (_vertexShader != 0)
    {
      oglDetachShader(_id, _vertexShader);
      _vertexShader = 0;
    }
  }

  void Program::attachVertexShader(unsigned int vertexShader)
  {
    detachVertexShader();
    _vertexShader = vertexShader;
    oglAttachShader(_id, vertexShader);
  }

  void Program::detachFragmentShader()
  {
    if (_fragmentShader != 0)
    {
      oglDetachShader(_id, _fragmentShader);
      _fragmentShader = 0;
    }
  }

  void Program::attachFragmentShader(unsigned int fragmentShader)
  {
    detachFragmentShader();
    _fragmentShader = fragmentShader;
    oglAttachShader(_id, fragmentShader);
  }

  GLuint LoadShader(GLenum type, const char *src)
  {
    assert(src != NULL);

    const GLuint shader = oglCreateShader(type);
    assert(shader != 0);

    oglShaderSource(shader, 1, (const GLchar**)&src, NULL);
    oglCompileShader(shader);

#ifdef DEBUG

    GLint wentFine = GL_TRUE;
    oglGetShaderiv(shader, GL_COMPILE_STATUS, &wentFine);
    if(wentFine != GL_TRUE)
    {
      GLsizei logsize = 0;
      oglGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logsize);

      char * log = (char*)msys_mallocAlloc(logsize + 1);
      oglGetShaderInfoLog(shader, logsize, &logsize, log);
      DBG("Erreur de compilation : %s", log);
      msys_mallocFree(log);
      oglDeleteShader(shader);

      return 0;
    }

#endif // !DEBUG

    return shader;
  }

  void Program::LoadShaders(const char * vertexSrc, const char * fragmentSrc)
  {
    DBG("Chargement des shaders");
    _id = 0;
    if (NULL == vertexSrc && NULL == fragmentSrc)
    {
      return;
    }

    _id = oglCreateProgram();
    assert(_id != 0);

    GLuint vertexShader = 0;
    if (vertexSrc != NULL)
    {
      DBG("Vertex shader");
      vertexShader = LoadShader(GL_VERTEX_SHADER, vertexSrc);
      attachVertexShader(vertexShader);
    }
    GLuint fragmentShader = 0;
    if (fragmentSrc != NULL)
    {
      DBG("Fragment shader");
      fragmentShader = LoadShader(GL_FRAGMENT_SHADER, fragmentSrc);
      attachFragmentShader(fragmentShader);
    }

    oglLinkProgram(_id);

#ifdef DEBUG

    GLint wentFine = GL_TRUE;
    oglGetProgramiv(_id, GL_LINK_STATUS, &wentFine);
    if (wentFine != GL_TRUE)
    {
      GLsizei logsize = 0;
      oglGetProgramiv(_id, GL_INFO_LOG_LENGTH, &logsize);

      char * log = (char*) msys_mallocAlloc(logsize + 1);
      oglGetProgramInfoLog(_id, logsize, &logsize, log);

      DBG("Erreur de link : %s", log);

      msys_mallocFree(log);
    }

#endif // !DEBUG

    if (vertexSrc != NULL)
    {
      oglDeleteShader(vertexShader);
    }
    if (fragmentSrc != NULL)
    {
      oglDeleteShader(fragmentShader);
    }
  }
}
