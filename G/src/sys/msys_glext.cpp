//--------------------------------------------------------------------------//
// iq . 2003/2008 . code for 64 kb intros by RGBA                           //
//--------------------------------------------------------------------------//

#ifdef WINDOWS
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#endif

#include "msys.h"

#include <GL/gl.h>
#include "glext.h"

#ifdef WINDOWS
#endif

#ifdef LINUX
#include <GL/glx.h>
#endif

#ifdef IRIX
#include <GL/glx.h>
#endif

#include <string.h>

//--- d a t a ---------------------------------------------------------------
#include "msys_glext.h"

#define UNUSED "\x0"

static const char * function_names = {

  // Textures
  "glActiveTexture\x0"
  UNUSED // "glClientActiveTexture\x0"
  "glGenerateMipmap\x0"
  UNUSED // "glMultiTexCoord4fv\x0"
  UNUSED // "glTexImage3D\x0"

  // vbo-ibo
  "glBindBuffer\x0"
  "glBufferData\x0"
  UNUSED // "glBufferSubData\x0"
  "glDeleteBuffers\x0"
  "glGenBuffers\x0"

  // shader
  "glAttachShader\x0"
  "glCompileShader\x0"
  "glCreateProgram\x0"
  "glCreateShader\x0"
  "glDeleteProgram\x0"
  "glDeleteShader\x0"
  "glDetachShader\x0"
  "glGetProgramInfoLog\x0"
  "glGetProgramiv\x0"
  "glGetShaderInfoLog\x0"
  "glGetShaderiv\x0"
  UNUSED // "glIsShader\x0"
  "glLinkProgram\x0"
  "glShaderSource\x0"
  "glUseProgram\x0"

  // Uniforms
  "glGetUniformLocation\x0"
  "glUniform1f\x0"
  "glUniform1i\x0"
  UNUSED // "glUniform2f\x0"
  "glUniform2fv\x0"
  UNUSED // "glUniform3f\x0"
  "glUniform3fv\x0"
  "glUniform4fv\x0"
  "glUniformMatrix4fv\x0"

  // Vertex attributes
  "glBindAttribLocation\x0"
  "glDisableVertexAttribArray\x0"
  "glEnableVertexAttribArray\x0"
  "glVertexAttrib1f\x0"
  "glVertexAttrib3f\x0"
  "glVertexAttrib4f\x0"
  "glVertexAttribPointer\x0"

  // Render buffers
  "glBindFramebuffer\x0"
  UNUSED // "glBindRenderbuffer\x0"
  "glCheckFramebufferStatus\x0"
  "glDeleteFramebuffers\x0"
  UNUSED // "glDeleteRenderbuffers\x0"
  "glDrawBuffers\x0"
  UNUSED // "glFramebufferRenderbuffer\x0"
  UNUSED // "glFramebufferTexture1D\x0"
  "glFramebufferTexture2D\x0"
  UNUSED // "glFramebufferTexture3D\x0"
  "glGenFramebuffers\x0"
  UNUSED // "glRenderbufferStorage\x0"

  // Other
  UNUSED // "glLoadTransposeMatrixf\x0"
};

void *msys_oglfunc[NUM_FUNCTIONS];

//--- c o d e ---------------------------------------------------------------

const char * msys_glextInit( void )
{
  const char * str = function_names;
  for (int i = 0; i < NUM_FUNCTIONS; ++i)
  {
    if (*str == 0)
    {
      str += 1;
      continue;
    }

#ifdef WINDOWS
    msys_oglfunc[i] = wglGetProcAddress( str );
#endif
#ifdef LINUX
    msys_oglfunc[i] = glXGetProcAddress( (const unsigned char *)str );
#endif
#ifdef IRIX
    msys_oglfunc[i] = glXGetProcAddress( (const unsigned char *)str );
#endif

    if (!msys_oglfunc[i])
    {
      DBG("Binding of function [%s] failed!", str);
      return str;
    }

    str += 1+strlen( str );
  }

  return NULL;
}
