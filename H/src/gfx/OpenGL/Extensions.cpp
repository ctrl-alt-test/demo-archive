// CR: change le commentaire ? :)
//--------------------------------------------------------------------------//
// iq . 2003/2008 . code for 64 kb intros by RGBA                           //
//--------------------------------------------------------------------------//

#include "Extensions.hh"
#include "engine/core/Debug.hh" 
// FIXME: ideally Gfx should not have dependency over Engine: provide
// the log/error function from outside.
#include "gfx/RasterTests.hh"
#include <cstring>
#include <cassert>

//--- d a t a ---------------------------------------------------------------

#define UNUSED_GL_EXTENSION "\x0"

//
// FIXME: I am not quite certain this is the proper way to do.
// From what I've seen on Internet, it should be possible to directly
// request a given OpenGL version, in which case there is no need to
// check for extensions support since it would be all in core.
//
static const char* extensionNames = {
	"GL_ARB_framebuffer_object\x0"
	"GL_ARB_fragment_shader\x0"
	"GL_ARB_instanced_arrays\x0"
	"GL_ARB_multitexture\x0"
	"GL_ARB_shader_objects\x0"
	"GL_ARB_vertex_buffer_object\x0"
	"GL_ARB_vertex_shader\x0"
	"GL_EXT_texture_filter_anisotropic\x0"
};

static const char* functionNames = {
	// vbo-ibo
	"glBindBuffer\x0"					// GL_ARB_vertex_buffer_object
	"glBufferData\x0"					// GL_ARB_vertex_buffer_object
	UNUSED_GL_EXTENSION // "glBufferSubData\x0"	// GL_ARB_vertex_buffer_object
	"glDeleteBuffers\x0"				// GL_ARB_vertex_buffer_object
	"glGenBuffers\x0"					// GL_ARB_vertex_buffer_object
	"glDrawArraysInstanced\x0"		// GL_ARB_instanced_arrays
	"glDrawElementsInstanced\x0"		// GL_ARB_instanced_arrays

	// Vertex attributes
	"glBindAttribLocation\x0"			// GL_ARB_vertex_shader
	"glDisableVertexAttribArray\x0"	// GL_ARB_vertex_shader
	"glEnableVertexAttribArray\x0"	// GL_ARB_vertex_shader
	"glVertexAttrib1f\x0"				// GL_ARB_vertex_shader
	"glVertexAttrib3f\x0"				// GL_ARB_vertex_shader
	"glVertexAttrib4f\x0"				// GL_ARB_vertex_shader
	"glVertexAttribPointer\x0"		// GL_ARB_vertex_shader

	// Textures
	"glActiveTexture\x0"					// GL_ARB_multitexture
	UNUSED_GL_EXTENSION // "glClientActiveTexture\x0"	// GL_ARB_multitexture
	"glGenerateMipmap\x0"
	UNUSED_GL_EXTENSION // "glMultiTexCoord4fv\x0"		// GL_ARB_multitexture
	UNUSED_GL_EXTENSION // "glTexImage3D\x0"

	// shader
	"glAttachShader\x0"
	"glCompileShader\x0"				// GL_ARB_shader_objects
	"glCreateProgram\x0"
	"glCreateShader\x0"
	"glDeleteProgram\x0"
	"glDeleteShader\x0"
	"glDetachShader\x0"
	"glGetProgramInfoLog\x0"
	"glGetProgramiv\x0"
	"glGetShaderInfoLog\x0"
	"glGetShaderiv\x0"
	UNUSED_GL_EXTENSION // "glIsShader\x0"
	"glLinkProgram\x0"				// GL_ARB_shader_objects
	"glShaderSource\x0"				// GL_ARB_shader_objects
	"glUseProgram\x0"

	// Uniforms
	"glGetUniformLocation\x0"			// GL_ARB_shader_objects
	"glUniform1fv\x0"					// GL_ARB_shader_objects
	"glUniform2fv\x0"					// GL_ARB_shader_objects
	"glUniform3fv\x0"					// GL_ARB_shader_objects
	"glUniform4fv\x0"					// GL_ARB_shader_objects
	"glUniform1iv\x0"					// GL_ARB_shader_objects
	"glUniform2iv\x0"					// GL_ARB_shader_objects
	"glUniform3iv\x0"					// GL_ARB_shader_objects
	"glUniform4iv\x0"					// GL_ARB_shader_objects
	"glUniformMatrix4fv\x0"			// GL_ARB_shader_objects

	// Render buffers
	"glBindFramebuffer\x0"					// GL_ARB_framebuffer_object
	UNUSED_GL_EXTENSION // "glBindRenderbuffer\x0"			// GL_ARB_framebuffer_object
	"glCheckFramebufferStatus\x0"				// GL_ARB_framebuffer_object
	"glDeleteFramebuffers\x0"					// GL_ARB_framebuffer_object
	UNUSED_GL_EXTENSION // "glDeleteRenderbuffers\x0"		// GL_ARB_framebuffer_object
	"glDrawBuffers\x0"
	UNUSED_GL_EXTENSION // "glFramebufferRenderbuffer\x0"	// GL_ARB_framebuffer_object
	UNUSED_GL_EXTENSION // "glFramebufferTexture1D\x0"		// GL_ARB_framebuffer_object
	"glFramebufferTexture2D\x0"				// GL_ARB_framebuffer_object
	UNUSED_GL_EXTENSION // "glFramebufferTexture3D\x0"		// GL_ARB_framebuffer_object
	"glGenFramebuffers\x0"					// GL_ARB_framebuffer_object
	UNUSED_GL_EXTENSION // "glRenderbufferStorage\x0"		// GL_ARB_framebuffer_object

	// Other
	UNUSED_GL_EXTENSION // "glLoadTransposeMatrixf\x0"
	"glBlendEquationSeparate\x0"
	"glBlendFuncSeparate\x0"
#if GFX_ENABLE_STENCIL_TESTING
	"glStencilFuncSeparate\x0"
	"glStencilOpSeparate\x0"
#else // !GFX_ENABLE_STENCIL_TESTING
	UNUSED_GL_EXTENSION
	UNUSED_GL_EXTENSION
#endif // !GFX_ENABLE_STENCIL_TESTING
};

void* Gfx::opengl_functions[NUM_FUNCTIONS];

//--- c o d e ---------------------------------------------------------------

bool Gfx::InitializeOpenGLExtensions()
{
	bool success = true;

	PFNGLGETSTRINGIPROC glGetStringi = NULL;
#ifdef _WIN32
	glGetStringi = (PFNGLGETSTRINGIPROC)wglGetProcAddress("glGetStringi");
#elif LINUX
	glGetStringi = (PFNGLGETSTRINGIPROC)glXGetProcAddress((const unsigned char*)"glGetStringi");
#endif // _WIN32 / LINUX
	if (glGetStringi == NULL)
	{
		LOG_ERROR("Binding of OpenGL function glGetStringi failed.");
		return false;
	}

	int numberOfExtensions = 0;
	glGetIntegerv(GL_NUM_EXTENSIONS, &numberOfExtensions);

	for (const char* extensionName = extensionNames; *extensionName != '\0'; extensionName += 1 + strlen(extensionName))
	{
		bool found = false;
		for (int i = 0; i < numberOfExtensions; ++i)
		{
			if (strcmp(extensionName, (const char*)glGetStringi(GL_EXTENSIONS, i)) == 0)
			{
				LOG_INFO("Found extension %s.", extensionName);
				found = true;
				break;
			}
		}
		if (!found)
		{
			LOG_ERROR("Extension %s is not available.", extensionName);
			success = false;
		}
	}

	const char * functionName = functionNames;
	for (int i = 0; i < NUM_FUNCTIONS; ++i)
	{
		if (*functionName == 0)
		{
			functionName += 1;
			continue;
		}

#ifdef _WIN32
		opengl_functions[i] = wglGetProcAddress(functionName);
#elif LINUX
		opengl_functions[i] = (void*)glXGetProcAddressARB((const GLubyte*)functionName);
#else
		NOT_IMPLEMENTED;
#endif // _WIN32 / LINUX

		if (!opengl_functions[i])
		{
			LOG_ERROR("Binding of OpenGL function %s failed.", functionName);
			success = false;
		}

		functionName += 1 + strlen(functionName);
	}

	return success;
}
