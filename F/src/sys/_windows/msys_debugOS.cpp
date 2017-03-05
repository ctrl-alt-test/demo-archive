//--------------------------------------------------------------------------//
// iq . 2003/2008 . code for 64 kb intros by RGBA                           //
//--------------------------------------------------------------------------//

#ifdef DEBUG

#include <windows.h>
#include <stdio.h>
#include <stdarg.h>

#include "../msys.h"
#include "../msys_debug.h"
#include <GL/gl.h>
#include "../glext.h"

static FILE *fp;

int msys_debugInit( void )
{
    fp = fopen( "debug.txt", "wt" );
    if( !fp )
        return( 0 );

    fprintf( fp, "debug file\n" );
    fprintf( fp, "-------------------------\n" );
    fflush( fp );

    return( 1 );
}

void msys_debugEnd( void )
{
    fprintf( fp, "-------------------------\n" );
    fflush( fp );
    fclose( fp );
}

void msys_debugPrintf( char *format, ... )
{
  va_list arglist;
  va_start( arglist, format );
  vfprintf( fp, format, arglist );
  fflush( fp );
  va_end( arglist );
}


void msys_debugCheckfor( bool expression, char *format, ... )
{
  char    str[1024];

  if( !expression )
  {
    va_list arglist;
    va_start( arglist, format );
    vsprintf( str, format, arglist );
    va_end( arglist );
    msys_debugHaltProgram( str );
  }
}

void msys_debugHaltProgram( char *str )
{
    MessageBox( 0, str, "error", MB_OK );
    DebugBreak();
}

void OpenGLErrorCheck(char *format,...)
{
  GLenum error = glGetError();
  while (GL_NO_ERROR != error)
  {
    switch (error)
    {
    case GL_INVALID_ENUM:      msys_debugPrintf("GL_INVALID_ENUM      : "); break;
    case GL_INVALID_VALUE:     msys_debugPrintf("GL_INVALID_VALUE     : "); break;
    case GL_INVALID_OPERATION: msys_debugPrintf("GL_INVALID_OPERATION : "); break;
    case GL_STACK_OVERFLOW:    msys_debugPrintf("GL_STACK_OVERFLOW    : "); break;
    case GL_STACK_UNDERFLOW:   msys_debugPrintf("GL_STACK_UNDERFLOW   : "); break;
    case GL_OUT_OF_MEMORY:     msys_debugPrintf("GL_OUT_OF_MEMORY     : "); break;
    case GL_TABLE_TOO_LARGE:   msys_debugPrintf("GL_TABLE_TOO_LARGE   : "); break;
    default:                   msys_debugPrintf("Erreur OGL non gérée (%d) : ", error);
    };

    va_list arglist;
    va_start(arglist, format);
    vfprintf(fp, format, arglist);
    fflush(fp);
    va_end(arglist);

    msys_debugPrintf("\n");
    error = glGetError();
  }
}

void OpenGLFBOErrorCheck(char *format,...)
{
  GLenum error = oglCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
  if (GL_FRAMEBUFFER_COMPLETE_EXT == error)
    return;

  switch (error)
  {
  case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT:         msys_debugPrintf("GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT_EXT         : "); break;
  case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT: msys_debugPrintf("GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT_EXT : "); break;
  case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:         msys_debugPrintf("GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT         : "); break;
  case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:            msys_debugPrintf("GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT            : "); break;
  case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT:        msys_debugPrintf("GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER_EXT        : "); break;
  case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT:        msys_debugPrintf("GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER_EXT        : "); break;
  case GL_FRAMEBUFFER_UNSUPPORTED_EXT:                   msys_debugPrintf("GL_FRAMEBUFFER_UNSUPPORTED_EXT                   : "); break;
  default:                                               msys_debugPrintf("Erreur de FBO non gérée (%d) : ", error);
  };

  va_list arglist;
  va_start(arglist, format);
  vfprintf(fp, format, arglist);
  fflush(fp);
  va_end(arglist);

  msys_debugPrintf("\n");
}

#endif

#ifdef DEBUG_RELEASE

#include "../msys.h"
#include "../msys_debug.h"
#include <GL/gl.h>

void OpenGLErrorAssert()
{
  GLenum error = glGetError();
  assert(error != GL_NO_ERROR);
}

void OpenGLFBOErrorAssert()
{
  GLenum error = oglCheckFramebufferStatusEXT(GL_FRAMEBUFFER_EXT);
  assert(error != GL_FRAMEBUFFER_COMPLETE_EXT == error);
}

bool dr_assert(char *expr, char *file, int line)
{
  // METTRE UN BREAKPOINT ICI lors du mode debug-release.
  int i = 42;
  return i == 42;
}

#endif
