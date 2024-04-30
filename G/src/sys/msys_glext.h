//--------------------------------------------------------------------------//
// iq . 2003/2008 . code for 64 kb intros by RGBA                           //
//--------------------------------------------------------------------------//

#ifndef _MSYS_GLEXT_H_
#define _MSYS_GLEXT_H_

#ifdef WINDOWS
#define WIN32_LEAN_AND_MEAN
#define WIN32_EXTRA_LEAN
#include <windows.h>
#endif
#include <GL/gl.h>
#include "glext.h"

#define NUM_FUNCTIONS (5+5+15+9+7+12+1)

extern void *msys_oglfunc[NUM_FUNCTIONS];

// Textures (5)
#define oglActiveTexture               ((PFNGLACTIVETEXTUREPROC)           msys_oglfunc[0])
#define oglClientActiveTexture         ((PFNGLCLIENTACTIVETEXTUREPROC)     msys_oglfunc[1])
#define oglGenerateMipmap              ((PFNGLGENERATEMIPMAPPROC)          msys_oglfunc[2])
#define oglMultiTexCoord4fv            ((PFNGLMULTITEXCOORD4FVPROC)        msys_oglfunc[3])
#define oglTexImage3D                  ((PFNGLTEXIMAGE3DPROC)              msys_oglfunc[4])

// vbo-ibo (5)
#define oglBindBuffer                  ((PFNGLBINDBUFFERPROC)              msys_oglfunc[5])
#define oglBufferData                  ((PFNGLBUFFERDATAPROC)              msys_oglfunc[6])
#define oglBufferSubData               ((PFNGLBUFFERSUBDATAPROC)           msys_oglfunc[7])
#define oglDeleteBuffers               ((PFNGLDELETEBUFFERSPROC)           msys_oglfunc[8])
#define oglGenBuffers                  ((PFNGLGENBUFFERSPROC)              msys_oglfunc[9])

// Shaders (15)
#define oglAttachShader                ((PFNGLATTACHSHADERPROC)            msys_oglfunc[10])
#define oglCompileShader               ((PFNGLCOMPILESHADERPROC)           msys_oglfunc[11])
#define oglCreateProgram               ((PFNGLCREATEPROGRAMPROC)           msys_oglfunc[12])
#define oglCreateShader                ((PFNGLCREATESHADERPROC)            msys_oglfunc[13])
#define oglDeleteProgram               ((PFNGLDELETEPROGRAMPROC)           msys_oglfunc[14])
#define oglDeleteShader                ((PFNGLDELETESHADERPROC)            msys_oglfunc[15])
#define oglDetachShader                ((PFNGLDETACHSHADERPROC)            msys_oglfunc[16])
#define oglGetProgramInfoLog           ((PFNGLGETPROGRAMINFOLOGPROC)       msys_oglfunc[17])
#define oglGetProgramiv                ((PFNGLGETPROGRAMIVPROC)            msys_oglfunc[18])
#define oglGetShaderInfoLog            ((PFNGLGETSHADERINFOLOGPROC)        msys_oglfunc[19])
#define oglGetShaderiv                 ((PFNGLGETSHADERIVPROC)             msys_oglfunc[20])
#define oglIsShader                    ((PFNGLISSHADERPROC)                msys_oglfunc[21])
#define oglLinkProgram                 ((PFNGLLINKPROGRAMPROC)             msys_oglfunc[22])
#define oglShaderSource                ((PFNGLSHADERSOURCEPROC)            msys_oglfunc[23])
#define oglUseProgram                  ((PFNGLUSEPROGRAMPROC)              msys_oglfunc[24])

// Uniforms (9)
#define oglGetUniformLocation          ((PFNGLGETUNIFORMLOCATIONPROC)      msys_oglfunc[25])
#define oglUniform1f                   ((PFNGLUNIFORM1FPROC)               msys_oglfunc[26])
#define oglUniform1i                   ((PFNGLUNIFORM1IPROC)               msys_oglfunc[27])
#define oglUniform2f                   ((PFNGLUNIFORM2FPROC)               msys_oglfunc[28])
#define oglUniform2fv                  ((PFNGLUNIFORM2FVPROC)              msys_oglfunc[29])
#define oglUniform3f                   ((PFNGLUNIFORM3FPROC)               msys_oglfunc[30])
#define oglUniform3fv                  ((PFNGLUNIFORM3FVPROC)              msys_oglfunc[31])
#define oglUniform4fv                  ((PFNGLUNIFORM4FVPROC)              msys_oglfunc[32])
#define oglUniformMatrix4fv            ((PFNGLUNIFORMMATRIX4FVPROC)        msys_oglfunc[33])

// Vertex attributes (7)
#define oglBindAttribLocation          ((PFNGLBINDATTRIBLOCATIONPROC)      msys_oglfunc[34])
#define oglDisableVertexAttribArray    ((PFNGLDISABLEVERTEXATTRIBARRAYPROC)msys_oglfunc[35])
#define oglEnableVertexAttribArray     ((PFNGLENABLEVERTEXATTRIBARRAYPROC) msys_oglfunc[36])
#define oglVertexAttrib1f              ((PFNGLVERTEXATTRIB1FPROC)          msys_oglfunc[37])
#define oglVertexAttrib3f              ((PFNGLVERTEXATTRIB3FPROC)          msys_oglfunc[38])
#define oglVertexAttrib4f              ((PFNGLVERTEXATTRIB4FPROC)          msys_oglfunc[39])
#define oglVertexAttribPointer         ((PFNGLVERTEXATTRIBPOINTERPROC)     msys_oglfunc[40])

// Render buffers (12)
#define oglBindFramebuffer             ((PFNGLBINDFRAMEBUFFERPROC)         msys_oglfunc[41])
#define oglBindRenderbuffer            ((PFNGLBINDRENDERBUFFERPROC)        msys_oglfunc[42])
#define oglCheckFramebufferStatus      ((PFNGLCHECKFRAMEBUFFERSTATUSPROC)  msys_oglfunc[43])
#define oglDeleteFramebuffers          ((PFNGLDELETEFRAMEBUFFERSPROC)      msys_oglfunc[44])
#define oglDeleteRenderbuffers         ((PFNGLDELETERENDERBUFFERSPROC)     msys_oglfunc[45])
#define oglDrawBuffers                 ((PFNGLDRAWBUFFERSPROC)             msys_oglfunc[46])
#define oglFramebufferRenderbuffer     ((PFNGLFRAMEBUFFERRENDERBUFFERPROC) msys_oglfunc[47])
#define oglFramebufferTexture1D        ((PFNGLFRAMEBUFFERTEXTURE1DPROC)    msys_oglfunc[48])
#define oglFramebufferTexture2D        ((PFNGLFRAMEBUFFERTEXTURE2DPROC)    msys_oglfunc[49])
#define oglFramebufferTexture3D        ((PFNGLFRAMEBUFFERTEXTURE3DPROC)    msys_oglfunc[50])
#define oglGenFramebuffers             ((PFNGLGENFRAMEBUFFERSPROC)         msys_oglfunc[51])
#define oglRenderbufferStorage         ((PFNGLRENDERBUFFERSTORAGEPROC)     msys_oglfunc[52])

// Others (1)
#define oglLoadTransposeMatrixf        ((PFNGLLOADTRANSPOSEMATRIXFPROC)    msys_oglfunc[53])

// init
const char * msys_glextInit();

#endif
