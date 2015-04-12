//--------------------------------------------------------------------------//
// iq . 2003/2008 . code for 64 kb intros by RGBA                           //
//--------------------------------------------------------------------------//

#ifdef WINDOWS
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#endif

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

static char *funciones = {
    // multitexture
    "glActiveTextureARB\x0"
    "glClientActiveTextureARB\x0"
    "glMultiTexCoord4fvARB\x0"
    // programs
    "glDeleteProgramsARB\x0"
    "glBindProgramARB\x0"
    "glProgramStringARB\x0"
    "glProgramLocalParameter4fvARB\x0"
    "glProgramEnvParameter4fvARB\x0"
    // textures 3d
    "glTexImage3D\x0"
    // vbo-ibo
    "glGenBuffersARB\x0"
    "glBindBufferARB\x0"
    "glBufferDataARB\x0"
    "glBufferSubDataARB\x0"
    "glDeleteBuffersARB\x0"

    // shader
    "glCreateProgram\x0"
    "glCreateShader\x0"
    "glShaderSource\x0"
    "glCompileShader\x0"
    "glAttachShader\x0"
    "glDetachShader\x0"
    "glLinkProgram\x0"
    "glUseProgram\x0"
    "glUniform4fv\x0"
    "glUniform1i\x0"
    "glUniformMatrix4fv\x0"
    "glGetUniformLocationARB\x0"
    "glGetObjectParameterivARB\x0"
    "glGetInfoLogARB\x0"

    "glLoadTransposeMatrixf\x0"

    //"glIsRenderbufferEXT\x0"
    "glBindRenderbufferEXT\x0"
    "glDeleteRenderbuffersEXT\x0"
    //"glGenRenderbuffersEXT\x0"
    "glRenderbufferStorageEXT\x0"
    //"glGetRenderbufferParameterivEXT\x0"
    //"glIsFramebufferEXT\x0"
    "glBindFramebufferEXT\x0"
    "glDeleteFramebuffersEXT\x0"
    //"glGenFramebuffersEXT\x0"
    "glCheckFramebufferStatusEXT\x0"
    "glFramebufferTexture1DEXT\x0"
    "glFramebufferTexture2DEXT\x0"
    "glFramebufferTexture3DEXT\x0"
    "glFramebufferRenderbufferEXT\x0"
    //"glGetFramebufferAttachmentParameterivEXT\x0"
    "glGenerateMipmapEXT\x0"

    // added by Ctrl-Alt-Test
    "glGetShaderiv\x0"
    "glGetProgramiv\x0"
    "glGetShaderInfoLog\x0"
    "glGetProgramInfoLog\x0"
    "glDeleteShader\x0"
    "glDeleteProgram\x0"
    "glIsShader\x0"
    "glUniform3f\x0"
    "glVertexAttribPointer\x0"
    "glEnableVertexAttribArray\x0"
    "glGetAttribLocation\x0"
    "glGenFramebuffersEXT\x0"
    "glUniform1f\x0"
};

void *msys_oglfunc[NUMFUNCIONES];

//--- c o d e ---------------------------------------------------------------

int msys_glextInit( void )
{
    char *str = funciones;
    for( int i=0; i<NUMFUNCIONES; i++ )
        {
        #ifdef WINDOWS
        msys_oglfunc[i] = wglGetProcAddress( str );
        #endif
        #ifdef LINUX
        msys_oglfunc[i] = glXGetProcAddress( (const unsigned char *)str );
        #endif
        #ifdef IRIX
        msys_oglfunc[i] = glXGetProcAddress( (const unsigned char *)str );
        #endif

        str += 1+strlen( str );

        if( !msys_oglfunc[i] )
			return( 0 );
        }


    return( 1 );
}
