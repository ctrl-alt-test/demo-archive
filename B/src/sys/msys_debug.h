//--------------------------------------------------------------------------//
// iq . 2003/2008 . code for 64 kb intros by RGBA                           //
//--------------------------------------------------------------------------//

#ifndef _MSYS_DEBUG_H_
#define _MSYS_DEBUG_H_

#ifdef DEBUG

# define DBG(format,...) do { msys_debugPrintf(format, __VA_ARGS__); msys_debugPrintf("\n"); } while (0)
#  define OGL_ERROR_CHECK(format,...) OpenGLErrorCheck(format, __VA_ARGS__)

int  msys_debugInit( void );
void msys_debugEnd( void );
void msys_debugPrintf( char *format,... );
void msys_debugCheckfor( bool expression, char *format,... );
void msys_debugHaltProgram( char *str );
void OpenGLErrorCheck(char * format,...);

#include <assert.h>

#else

# ifdef DEBUG_RELEASE

#  define assert(_Expression) (void)( (!!(_Expression)) || (dr_assert(#_Expression, __FILE__, __LINE__) ))
#  define DBG(format,...)
#  define OGL_ERROR_CHECK(format,...)

void OpenGLErrorCheck(char * format,...);
bool dr_assert(char *expr, char *file, int line);

# else // pas d'assert ni de log en release

#  define assert(_Expression)
#  define DBG(format,...)
#  define OGL_ERROR_CHECK(format,...)

# endif

#endif

#endif
