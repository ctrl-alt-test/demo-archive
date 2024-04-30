//--------------------------------------------------------------------------//
// iq . 2003/2008 . code for 64 kb intros by RGBA                           //
//--------------------------------------------------------------------------//

#ifndef _MSYS_DEBUG_H_
#define _MSYS_DEBUG_H_

#ifdef DEBUG

# define DBG(format,...) do { msys_debugPrintf(format, __VA_ARGS__); msys_debugPrintf("\n"); } while (0)
# define IFDBG(exp) exp
# define DBGARG(exp) , exp
# define DBGARGS(arg, ...) , arg, ##__VA_ARGS__
# define DBG_IF_FLAG(flag) if (intro.debug.flag)
# define OGL_ERROR_CHECK(format,...) OpenGLErrorCheck(format, __VA_ARGS__)
# define OGL_FBO_ERROR_CHECK(format,...) OpenGLFBOErrorCheck(format, __VA_ARGS__)
# define START_TIME_EVAL const long dbg_startEvalTime = msys_timerGet();
# define END_TIME_EVAL(action)  msys_debugPrintf("%s done in %d ms\n", action, (int)(msys_timerGet() - dbg_startEvalTime));

int  msys_debugInit( void );
void msys_debugEnd( void );
void msys_debugPrintf( char *format,... );
void msys_debugCheckfor( bool expression, char *format,... );
void msys_debugHaltProgram( char *str );
void OpenGLErrorCheck(char * format,...);
void OpenGLFBOErrorCheck(char *format,...);

# include <assert.h>

#else

# ifdef DEBUG_RELEASE

#  define assert(_Expression) (void)( (!!(_Expression)) || (dr_assert(#_Expression, __FILE__, __LINE__) ))
#  define IFDBG(exp)
#  define DBGARG(exp)
#  define DBGARGS(arg, ...)
#  define DBG_IF_FLAG(flag)
#  define DBG(format,...)
#  define OGL_ERROR_CHECK(format,...) OpenGLErrorAssert()
#  define OGL_FBO_ERROR_CHECK(format,...) OpenGLFBOErrorAssert()
#  define START_TIME_EVAL
#  define END_TIME_EVAL(action)

bool dr_assert(char *expr, char *file, int line);
void OpenGLErrorAssert();
void OpenGLFBOErrorAssert();

# else // pas d'assert ni de log en release

#  define assert(_Expression)

#  define IFDBG(exp)
#  define DBGARG(exp)
#  define DBGARGS(arg, ...)
#  define DBG_IF_FLAG(flag)
#  define DBG(format,...)
#  define OGL_ERROR_CHECK(format,...)
#  define OGL_FBO_ERROR_CHECK(format,...)
#  define START_TIME_EVAL
#  define END_TIME_EVAL(action)

# endif

#endif // DEBUG

#endif // _MSYS_DEBUG_H_
