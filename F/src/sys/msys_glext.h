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

#define NUMFUNCIONES (3+5+1+5+18+1+12+12+1)

extern void *msys_oglfunc[NUMFUNCIONES];

// multitexture
#define oglActiveTextureARB            ((PFNGLACTIVETEXTUREARBPROC)msys_oglfunc[0])
#define oglClientActiveTextureARB      ((PFNGLCLIENTACTIVETEXTUREARBPROC)msys_oglfunc[1])
#define oglMultiTexCoord4fvARB         ((PFNGLMULTITEXCOORD4FVARBPROC)msys_oglfunc[2])

// programs
#define oglDeleteProgramsARB           ((PFNGLDELETEPROGRAMSARBPROC)msys_oglfunc[3])
#define oglBindProgramARB              ((PFNGLBINDPROGRAMARBPROC)msys_oglfunc[4])
#define oglProgramStringARB            ((PFNGLPROGRAMSTRINGARBPROC)msys_oglfunc[5])
#define oglProgramLocalParameter4fvARB ((PFNGLPROGRAMLOCALPARAMETER4FVARBPROC)msys_oglfunc[6])
#define oglProgramEnvParameter4fvARB   ((PFNGLPROGRAMENVPARAMETER4FVARBPROC)msys_oglfunc[7])

// textures 3d
#define oglTexImage3D                  ((PFNGLTEXIMAGE3DPROC)msys_oglfunc[8])

// vbo-ibo
#define oglGenBuffers                  ((PFNGLGENBUFFERSPROC)msys_oglfunc[9])
#define oglBindBuffer                  ((PFNGLBINDBUFFERARBPROC)msys_oglfunc[10])
#define oglBufferData                  ((PFNGLBUFFERDATAARBPROC)msys_oglfunc[11])
#define oglBufferSubData               ((PFNGLBUFFERSUBDATAARBPROC)msys_oglfunc[12])
#define oglDeleteBuffers               ((PFNGLDELETEBUFFERSARBPROC)msys_oglfunc[13])

// shaders
#define oglCreateProgram               ((PFNGLCREATEPROGRAMPROC)msys_oglfunc[14])
#define oglCreateShader                ((PFNGLCREATESHADERPROC)msys_oglfunc[15])
#define oglShaderSource                ((PFNGLSHADERSOURCEPROC)msys_oglfunc[16])
#define oglCompileShader               ((PFNGLCOMPILESHADERPROC)msys_oglfunc[17])
#define oglAttachShader                ((PFNGLATTACHSHADERPROC)msys_oglfunc[18])
#define oglDetachShader                ((PFNGLDETACHSHADERPROC)msys_oglfunc[19])
#define oglLinkProgram                 ((PFNGLLINKPROGRAMPROC)msys_oglfunc[20])
#define oglUseProgram                  ((PFNGLUSEPROGRAMPROC)msys_oglfunc[21])
#define oglUniform1i                   ((PFNGLUNIFORM1IPROC)msys_oglfunc[22])
#define oglUniform1f                   ((PFNGLUNIFORM1FPROC)msys_oglfunc[23])
#define oglUniform2f                   ((PFNGLUNIFORM2FPROC)msys_oglfunc[24])
#define oglUniform2fv                  ((PFNGLUNIFORM2FVPROC)msys_oglfunc[25])
#define oglUniform3f                   ((PFNGLUNIFORM3FPROC)msys_oglfunc[26])
#define oglUniform3fv                  ((PFNGLUNIFORM3FVPROC)msys_oglfunc[27])
#define oglUniformMatrix4fv            ((PFNGLUNIFORMMATRIX4FVPROC)msys_oglfunc[28])
#define oglGetUniformLocation          ((PFNGLGETUNIFORMLOCATIONARBPROC)msys_oglfunc[29])
#define oglGetObjectParameteriv        ((PFNGLGETOBJECTPARAMETERIVARBPROC)msys_oglfunc[30])
#define oglGetInfoLog                  ((PFNGLGETINFOLOGARBPROC)msys_oglfunc[31])

#define oglLoadTransposeMatrixf        ((PFNGLLOADTRANSPOSEMATRIXFPROC)msys_oglfunc[32])

// fbo
#define oglBindRenderbufferEXT         ((PFNGLBINDRENDERBUFFEREXTPROC)msys_oglfunc[33])
#define oglDeleteRenderbuffersEXT      ((PFNGLDELETERENDERBUFFERSEXTPROC)msys_oglfunc[34])
#define oglRenderbufferStorageEXT      ((PFNGLRENDERBUFFERSTORAGEEXTPROC)msys_oglfunc[35])
#define oglBindFramebufferEXT          ((PFNGLBINDFRAMEBUFFEREXTPROC)msys_oglfunc[36])
#define oglDeleteFramebuffersEXT       ((PFNGLDELETEFRAMEBUFFERSEXTPROC)msys_oglfunc[37])
#define oglCheckFramebufferStatusEXT   ((PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC)msys_oglfunc[38])
#define oglFramebufferTexture1DEXT     ((PFNGLFRAMEBUFFERTEXTURE1DEXTPROC)msys_oglfunc[39])
#define oglFramebufferTexture2DEXT     ((PFNGLFRAMEBUFFERTEXTURE2DEXTPROC)msys_oglfunc[40])
#define oglFramebufferTexture3DEXT     ((PFNGLFRAMEBUFFERTEXTURE3DEXTPROC)msys_oglfunc[41])
#define oglFramebufferRenderbufferEXT  ((PFNGLFRAMEBUFFERRENDERBUFFEREXTPROC)msys_oglfunc[42])
#define oglGenerateMipmapEXT           ((PFNGLGENERATEMIPMAPEXTPROC)msys_oglfunc[43])
#define oglDrawBuffers                 ((PFNGLDRAWBUFFERSPROC)msys_oglfunc[44])

// shaders for CAT
#define oglGetShaderiv                 ((PFNGLGETSHADERIVPROC)msys_oglfunc[45])
#define oglGetProgramiv                ((PFNGLGETPROGRAMIVPROC)msys_oglfunc[46])
#define oglGetShaderInfoLog            ((PFNGLGETSHADERINFOLOGPROC)msys_oglfunc[47])
#define oglGetProgramInfoLog           ((PFNGLGETPROGRAMINFOLOGPROC)msys_oglfunc[48])
#define oglDeleteShader                ((PFNGLDELETESHADERPROC)msys_oglfunc[49])
#define oglDeleteProgram               ((PFNGLDELETEPROGRAMPROC)msys_oglfunc[50])
#define oglIsShader                    ((PFNGLISSHADERPROC)msys_oglfunc[51])
#define oglVertexAttribPointer         ((PFNGLVERTEXATTRIBPOINTERPROC)msys_oglfunc[52])
#define oglEnableVertexAttribArray     ((PFNGLENABLEVERTEXATTRIBARRAYPROC)msys_oglfunc[53])
#define oglDisableVertexAttribArray    ((PFNGLDISABLEVERTEXATTRIBARRAYPROC)msys_oglfunc[54])
#define oglBindAttribLocation          ((PFNGLBINDATTRIBLOCATIONPROC)msys_oglfunc[55])
#define oglGenFramebuffersEXT          ((PFNGLGENFRAMEBUFFERSEXTPROC)msys_oglfunc[56])

// Attribs immediate mode
#define oglVertexAttrib3f              ((PFNGLVERTEXATTRIB3FPROC)msys_oglfunc[57])

// VBO
//#define oglGenBuffers                  ((PFNGLGENBUFFERSPROC)msys_oglfunc[58])

// init
int msys_glextInit( void );

#endif
