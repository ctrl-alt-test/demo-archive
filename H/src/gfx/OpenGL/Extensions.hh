//--------------------------------------------------------------------------//
// iq . 2003/2008 . code for 64 kb intros by RGBA                           //
//--------------------------------------------------------------------------//

#ifndef OPENGL_EXTENSIONS_HH
#define OPENGL_EXTENSIONS_HH

#ifdef _WIN32
#include <windows.h>
#elif LINUX
#include <GL/glx.h>
// glx.h includes X11/X.h, which defines things like None and Always
// This conflicts with our code.
#undef None
#undef Always
#endif // _WIN32

#define NUM_FUNCTIONS (7+7+5+15+10+12+5)

namespace Gfx
{
	extern void*	opengl_functions[NUM_FUNCTIONS];

	/// <summary>
	/// Loads the OpenGL extension functions.
	/// </summary>
	///
	/// <returns>True if all functions are successfully bound, false
	/// otherwise.</returns>
	bool			InitializeOpenGLExtensions();
}

// vbo-ibo (7)
#define glBindBuffer                  ((PFNGLBINDBUFFERPROC)              opengl_functions[0])
#define glBufferData                  ((PFNGLBUFFERDATAPROC)              opengl_functions[1])
#define glBufferSubData               ((PFNGLBUFFERSUBDATAPROC)           opengl_functions[2])
#define glDeleteBuffers               ((PFNGLDELETEBUFFERSPROC)           opengl_functions[3])
#define glGenBuffers                  ((PFNGLGENBUFFERSPROC)              opengl_functions[4])
#define glDrawArraysInstanced         ((PFNGLDRAWARRAYSINSTANCEDPROC)     opengl_functions[5])
#define glDrawElementsInstanced       ((PFNGLDRAWELEMENTSINSTANCEDPROC)   opengl_functions[6])

// Vertex attributes (7)
#define glBindAttribLocation          ((PFNGLBINDATTRIBLOCATIONPROC)      opengl_functions[7])
#define glDisableVertexAttribArray    ((PFNGLDISABLEVERTEXATTRIBARRAYPROC)opengl_functions[8])
#define glEnableVertexAttribArray     ((PFNGLENABLEVERTEXATTRIBARRAYPROC) opengl_functions[9])
#define glVertexAttrib1f              ((PFNGLVERTEXATTRIB1FPROC)          opengl_functions[10])
#define glVertexAttrib3f              ((PFNGLVERTEXATTRIB3FPROC)          opengl_functions[11])
#define glVertexAttrib4f              ((PFNGLVERTEXATTRIB4FPROC)          opengl_functions[12])
#define glVertexAttribPointer         ((PFNGLVERTEXATTRIBPOINTERPROC)     opengl_functions[13])

// Textures (5)
#define glActiveTexture               ((PFNGLACTIVETEXTUREPROC)           opengl_functions[14])
#define glClientActiveTexture         ((PFNGLCLIENTACTIVETEXTUREPROC)     opengl_functions[15])
#define glGenerateMipmap              ((PFNGLGENERATEMIPMAPPROC)          opengl_functions[16])
#define glMultiTexCoord4fv            ((PFNGLMULTITEXCOORD4FVPROC)        opengl_functions[17])
#define glTexImage3D                  ((PFNGLTEXIMAGE3DPROC)              opengl_functions[18])

// Shaders (15)
#define glAttachShader                ((PFNGLATTACHSHADERPROC)            opengl_functions[19])
#define glCompileShader               ((PFNGLCOMPILESHADERPROC)           opengl_functions[20])
#define glCreateProgram               ((PFNGLCREATEPROGRAMPROC)           opengl_functions[21])
#define glCreateShader                ((PFNGLCREATESHADERPROC)            opengl_functions[22])
#define glDeleteProgram               ((PFNGLDELETEPROGRAMPROC)           opengl_functions[23])
#define glDeleteShader                ((PFNGLDELETESHADERPROC)            opengl_functions[24])
#define glDetachShader                ((PFNGLDETACHSHADERPROC)            opengl_functions[25])
#define glGetProgramInfoLog           ((PFNGLGETPROGRAMINFOLOGPROC)       opengl_functions[26])
#define glGetProgramiv                ((PFNGLGETPROGRAMIVPROC)            opengl_functions[27])
#define glGetShaderInfoLog            ((PFNGLGETSHADERINFOLOGPROC)        opengl_functions[28])
#define glGetShaderiv                 ((PFNGLGETSHADERIVPROC)             opengl_functions[29])
#define glIsShader                    ((PFNGLISSHADERPROC)                opengl_functions[30])
#define glLinkProgram                 ((PFNGLLINKPROGRAMPROC)             opengl_functions[31])
#define glShaderSource                ((PFNGLSHADERSOURCEPROC)            opengl_functions[32])
#define glUseProgram                  ((PFNGLUSEPROGRAMPROC)              opengl_functions[33])

// Uniforms (10)
#define glGetUniformLocation          ((PFNGLGETUNIFORMLOCATIONPROC)      opengl_functions[34])
#define glUniform1fv                  ((PFNGLUNIFORM1FVPROC)              opengl_functions[35])
#define glUniform2fv                  ((PFNGLUNIFORM2FVPROC)              opengl_functions[36])
#define glUniform3fv                  ((PFNGLUNIFORM3FVPROC)              opengl_functions[37])
#define glUniform4fv                  ((PFNGLUNIFORM4FVPROC)              opengl_functions[38])
#define glUniform1iv                  ((PFNGLUNIFORM1IVPROC)              opengl_functions[39])
#define glUniform2iv                  ((PFNGLUNIFORM2IVPROC)              opengl_functions[40])
#define glUniform3iv                  ((PFNGLUNIFORM3IVPROC)              opengl_functions[41])
#define glUniform4iv                  ((PFNGLUNIFORM4IVPROC)              opengl_functions[42])
#define glUniformMatrix4fv            ((PFNGLUNIFORMMATRIX4FVPROC)        opengl_functions[43])

// Render buffers (12)
#define glBindFramebuffer             ((PFNGLBINDFRAMEBUFFERPROC)         opengl_functions[44])
#define glBindRenderbuffer            ((PFNGLBINDRENDERBUFFERPROC)        opengl_functions[45])
#define glCheckFramebufferStatus      ((PFNGLCHECKFRAMEBUFFERSTATUSPROC)  opengl_functions[46])
#define glDeleteFramebuffers          ((PFNGLDELETEFRAMEBUFFERSPROC)      opengl_functions[47])
#define glDeleteRenderbuffers         ((PFNGLDELETERENDERBUFFERSPROC)     opengl_functions[48])
#define glDrawBuffers                 ((PFNGLDRAWBUFFERSPROC)             opengl_functions[49])
#define glFramebufferRenderbuffer     ((PFNGLFRAMEBUFFERRENDERBUFFERPROC) opengl_functions[50])
#define glFramebufferTexture1D        ((PFNGLFRAMEBUFFERTEXTURE1DPROC)    opengl_functions[51])
#define glFramebufferTexture2D        ((PFNGLFRAMEBUFFERTEXTURE2DPROC)    opengl_functions[52])
#define glFramebufferTexture3D        ((PFNGLFRAMEBUFFERTEXTURE3DPROC)    opengl_functions[53])
#define glGenFramebuffers             ((PFNGLGENFRAMEBUFFERSPROC)         opengl_functions[54])
#define glRenderbufferStorage         ((PFNGLRENDERBUFFERSTORAGEPROC)     opengl_functions[55])

// Others (5)
#define glLoadTransposeMatrixf        ((PFNGLLOADTRANSPOSEMATRIXFPROC)    opengl_functions[56])
#define glBlendEquationSeparate       ((PFNGLBLENDEQUATIONSEPARATEPROC)   opengl_functions[57])
#define glBlendFuncSeparate           ((PFNGLBLENDFUNCSEPARATEPROC)       opengl_functions[58])
#define glStencilFuncSeparate         ((PFNGLSTENCILFUNCSEPARATEPROC)     opengl_functions[59])
#define glStencilOpSeparate           ((PFNGLSTENCILOPSEPARATEPROC)       opengl_functions[60])

#endif // OPENGL_EXTENSIONS_HH
