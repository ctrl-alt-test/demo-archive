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
  if (GL_NO_ERROR == error)
	  return;

  va_list arglist;
  va_start( arglist, format );
  DBG(format, arglist);
  va_end( arglist );

  switch (error)
  {
//  case GL_NO_ERROR: DBG("GL_NO_ERROR"); break;
  case GL_INVALID_ENUM: DBG("GL_INVALID_ENUM"); break;
  case GL_INVALID_VALUE: DBG("GL_INVALID_VALUE"); break;
  case GL_INVALID_OPERATION: DBG("GL_INVALID_OPERATION"); break;
  case GL_STACK_OVERFLOW: DBG("GL_STACK_OVERFLOW"); break;
  case GL_STACK_UNDERFLOW: DBG("GL_STACK_UNDERFLOW"); break;
  case GL_OUT_OF_MEMORY: DBG("GL_OUT_OF_MEMORY"); break;
  case GL_TABLE_TOO_LARGE: DBG("GL_TABLE_TOO_LARGE"); break;
  default:
    DBG("Erreur OpenGL non gérée dans le switch.");
  };
}

#endif

#ifdef DEBUG_RELEASE

#include "../msys.h"
#include "../msys_debug.h"
#include <GL/gl.h>

void OpenGLErrorCheck(char *format,...)
{
  GLenum error = glGetError();
  assert(error != GL_NO_ERROR);
  return;
}

bool dr_assert(char *expr, char *file, int line)
{
  // METTRE UN BREAKPOINT ICI lors du mode debug-release.
  int i = 42;
	  return i == 42;
}

#endif
