#include "OpenGLLayer.hh"

#include "Extensions.hh"
#include "OpenGLTypeConversion.hh"
#include "engine/container/Array.hxx"
#include "engine/core/Debug.hh"
// FIXME: ideally Gfx should not have dependency over Engine.
#include "gfx/Geometry.hh"
#include "gfx/ShadingParameters.hh"
#include <cassert>
#include <stdexcept>
#include <cstring>
#include <GL/gl.h>
#include "glext.h"

#if defined(GFX_MULTI_API) || defined(GFX_OPENGL_ONLY)

#define MAX_MRT 3

// Defines whether after a shader compilation, we should try to get the
// log from the shader compiler.
#ifndef ENABLE_SHADER_COMPILATION_ERROR_CHECK
#define ENABLE_SHADER_COMPILATION_ERROR_CHECK 1
#endif

// Defines if, in case the log from the shader compiler is displayed,
// we should try to reformat it so the IDE can understand it.
#ifndef ENABLE_SHADER_COMPILER_MESSAGE_PARSING
#define ENABLE_SHADER_COMPILER_MESSAGE_PARSING 1
#endif

#if DEBUG
#define ENABLE_OPENGL_ERROR_CHECK
#endif // DEBUG

#ifdef ENABLE_OPENGL_ERROR_CHECK

#define GL_CHECK(exp) 													\
	do																	\
	{																	\
		exp;															\
		GLenum error = glGetError();									\
		if (error != GL_NO_ERROR)										\
		{																\
			LOG_ERROR("\n%s(%d): OpenGL %s error.", __FILE__, __LINE__, Gfx::getErrorCode(error));	\
			throw new std::runtime_error(Gfx::getErrorDescription(error));	\
		}																\
		break;															\
	} while (0)

#else // !ENABLE_OPENGL_ERROR_CHECK

#define GL_CHECK(exp) exp

#endif // !ENABLE_OPENGL_ERROR_CHECK

using namespace Gfx;

void OpenGLLayer::CreateRenderingContext()
{
	if (!InitializeOpenGLExtensions())
	{
		Core::TerminateOnFatalError("Could not load OpenGL extensions.");
	}

	m_vbos.init(MAX_VERTEX_BUFFERS);
	m_textures.init(MAX_TEXTURES);
	m_shaders.init(MAX_SHADERS);
	m_fbos.init(MAX_FRAME_BUFFERS);

	m_currentVbo = VertexBufferID::InvalidID;
	for (int i = 0; i < NUM_TEXTURE_UNITS; ++i)
	{
		m_currentTextures[i] = TextureID::InvalidID;
	}
	m_currentShader = ShaderID::InvalidID;

	for (int i = 0; i < MAX_VERTEX_ATTRIBUTES; ++i)
	{
		m_enabledVertexAttributes[i] = false;
	}

	m_currentFrameBuffer = FrameBufferID::InvalidID;
	m_currentViewport.x = -1; // Initializing to an invalid value
	m_currentViewport.y = -1;
	m_currentViewport.width = -1;
	m_currentViewport.height = -1;
	m_currentPolygonMode = PolygonMode::Filled;
}

void OpenGLLayer::SetRasterizerState(const Viewport& viewport,
									 const PolygonMode::Enum& polygonMode,
									 const RasterTests& rasterTests,
									 const BlendingMode& blendingMode)
{
	if (m_currentViewport != viewport)
	{
		glViewport(viewport.x, viewport.y, viewport.width, viewport.height);
		m_currentViewport = viewport;
	}

	if (m_currentPolygonMode != polygonMode)
	{
		GL_CHECK(glPolygonMode(GL_FRONT_AND_BACK, polygonMode));
		m_currentPolygonMode = polygonMode;
	}

	if (m_currentRasterTests != rasterTests)
	{
#if GFX_ENABLE_FACE_CULLING
		const bool enableFaceCulling =
			rasterTests.faceCulling != FaceCulling::None;
		if (enableFaceCulling)
		{
			GL_CHECK(glEnable(GL_CULL_FACE));
			GL_CHECK(glCullFace(rasterTests.faceCulling));
		}
		else
		{
			GL_CHECK(glDisable(GL_CULL_FACE));
		}
#endif // GFX_ENABLE_FACE_CULLING

#if GFX_ENABLE_SCISSOR_TESTING
		if (rasterTests.scissorTestEnabled)
		{
			GL_CHECK(glEnable(GL_SCISSOR_TEST));
			GL_CHECK(glScissor(rasterTests.scissorX, rasterTests.scissorY,
				rasterTests.scissorWidth, rasterTests.scissorHeight));
		}
		else
		{
			GL_CHECK(glDisable(GL_SCISSOR_TEST));
		}
#endif // GFX_ENABLE_SCISSOR_TESTING

#if GFX_ENABLE_STENCIL_TESTING
		const bool enableStencilTest =
			rasterTests.stencilFrontValue != StencilFunction::Always ||
			rasterTests.stencilFrontOpPass != StencilOperation::Keep;
		if (enableStencilTest)
		{
			GL_CHECK(glEnable(GL_STENCIL_TEST));

			GL_CHECK(glStencilFuncSeparate(GL_FRONT,
				rasterTests.stencilFrontTest,
				rasterTests.stencilFrontValue,
				rasterTests.stencilFrontMask));
			GL_CHECK(glStencilOpSeparate(GL_FRONT,
				rasterTests.stencilFrontOpStencilFail,
				rasterTests.stencilFrontOpDepthFail,
				rasterTests.stencilFrontOpPass));

			GL_CHECK(glStencilFuncSeparate(GL_BACK,
				rasterTests.stencilBackTest,
				rasterTests.stencilBackValue,
				rasterTests.stencilBackMask));
			GL_CHECK(glStencilOpSeparate(GL_BACK,
				rasterTests.stencilBackOpStencilFail,
				rasterTests.stencilBackOpDepthFail,
				rasterTests.stencilBackOpPass));
		}
		else
		{
			GL_CHECK(glDisable(GL_STENCIL_TEST));
		}
#endif // GFX_ENABLE_STENCIL_TESTING

#if GFX_ENABLE_DEPTH_TESTING
		const bool enableDepthTest =
			rasterTests.depthTest != DepthFunction::Always ||
			rasterTests.depthWrite == true;
		if (enableDepthTest)
		{
			GL_CHECK(glEnable(GL_DEPTH_TEST));
			GL_CHECK(glDepthFunc(rasterTests.depthTest));
			GL_CHECK(glDepthMask(rasterTests.depthWrite ? GL_TRUE : GL_FALSE));
		}
		else
		{
			GL_CHECK(glDisable(GL_DEPTH_TEST));
		}
#endif // GFX_ENABLE_DEPTH_TESTING

#if GFX_ENABLE_CLIPPING
		if (rasterTests.enableClipDistance)
		{
			GL_CHECK(glEnable(GL_CLIP_DISTANCE0));
		}
		else
		{
			GL_CHECK(glDisable(GL_CLIP_DISTANCE0));
		}
#endif // GFX_ENABLE_CLIPPING

		m_currentRasterTests = rasterTests;
	}

	if (m_currentBlendingMode != blendingMode)
	{
		const bool enableBlending =
			blendingMode.rgbEquation != BlendEquation::SrcPlusDst ||
			blendingMode.alphaEquation != BlendEquation::SrcPlusDst ||
			blendingMode.srcRGBFunction != BlendFunction::One ||
			blendingMode.srcAlphaFunction != BlendFunction::One ||
			blendingMode.dstRGBFunction != BlendFunction::Zero ||
			blendingMode.dstAlphaFunction != BlendFunction::Zero;

		if (enableBlending)
		{
			GL_CHECK(glEnable(GL_BLEND));
			GL_CHECK(glBlendFuncSeparate(blendingMode.srcRGBFunction,
										 blendingMode.dstRGBFunction,
										 blendingMode.srcAlphaFunction,
										 blendingMode.dstAlphaFunction));
			GL_CHECK(glBlendEquationSeparate(blendingMode.rgbEquation,
											 blendingMode.alphaEquation));
		}
		else
		{
			GL_CHECK(glDisable(GL_BLEND));
		}

		m_currentBlendingMode = blendingMode;
	}
}

VertexBufferID OpenGLLayer::CreateVertexBuffer()
{
	VBOInfo newVBO;
	newVBO.vertexAttributes = NULL;
	newVBO.numberOfAttributes = 0;
	GL_CHECK(glGenBuffers(1, &newVBO.vertexBuffer));
	GL_CHECK(glGenBuffers(1, &newVBO.indexBuffer));

	// Internal resource indexing
	m_vbos.add(newVBO);
	VertexBufferID id = { m_vbos.size - 1 };
	return id;
}

const VertexBufferID& OpenGLLayer::LoadVertexBuffer(const VertexBufferID& id,
													const VertexAttribute* vertexAttributes,
													int numberOfAttributes, int stride,
													int vertexDataSize, void* vertexData,
													int indexDataSize, void* indexData,
													VertexIndexType::Enum indexType)
{
	assert(m_vbos.size > id.index);
	VBOInfo& vertexBuffer = m_vbos[id.index];
	vertexBuffer.vertexAttributes = vertexAttributes;
	vertexBuffer.numberOfAttributes = numberOfAttributes;
	vertexBuffer.stride = stride;

	int vertexBufferToRestore = 0;
	GL_CHECK(glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &vertexBufferToRestore));
	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer.vertexBuffer)); // Yes, questionable naming. :(
	GL_CHECK(glBufferData(GL_ARRAY_BUFFER, vertexDataSize, vertexData, GL_STATIC_DRAW));
	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vertexBufferToRestore));

	vertexBuffer.indexType = indexType;
	vertexBuffer.indexed = false;
	if (indexDataSize != 0 && indexData != NULL)
	{
		int indexBufferToRestore = 0;
		GL_CHECK(glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &indexBufferToRestore));
		GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertexBuffer.indexBuffer));
		GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexDataSize, indexData, GL_STATIC_DRAW));
		GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferToRestore));
		vertexBuffer.indexed = true;
	}

	return id;
}

void OpenGLLayer::DestroyVertexBuffer(const VertexBufferID& id)
{
	assert(m_vbos.size > id.index);
	glDeleteBuffers(1, &m_vbos[id.index].vertexBuffer);
	glDeleteBuffers(1, &m_vbos[id.index].indexBuffer);
	m_vbos[id.index].vertexBuffer = 0;
	m_vbos[id.index].indexBuffer = 0;
}

void OpenGLLayer::BindVertexBuffer(const VertexBufferID& id)
{
	assert(m_vbos.size > id.index);
	const int vboIndex = id.index;

	if (m_currentVbo.index != vboIndex)
	{
		int firstAttributeToDisable = 0;

		if (vboIndex >= 0)
		{
			const VBOInfo& vboInfo = m_vbos[vboIndex];

			GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, vboInfo.vertexBuffer));
			GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, (vboInfo.indexed ? vboInfo.indexBuffer : 0)));

			unsigned long offset = 0;
			for (int i = 0; i < vboInfo.numberOfAttributes; ++i)
			{
				const VertexAttribute& attrib = vboInfo.vertexAttributes[i];

				const GLenum glenum = getVertexAttributeGLenum(attrib.type);
				const unsigned long size = attrib.num * getVertexAttributeSize(attrib.type);

				GL_CHECK(glEnableVertexAttribArray(i));
				GL_CHECK(glVertexAttribPointer(i, attrib.num, glenum, GL_FALSE, vboInfo.stride, (void*)((unsigned long)NULL + offset)));
				offset += size;

				m_enabledVertexAttributes[i] = true;
			}
			firstAttributeToDisable = vboInfo.numberOfAttributes;
		}
		else
		{
			GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, 0));
			GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));
		}

		for (int i = firstAttributeToDisable; i < MAX_VERTEX_ATTRIBUTES; ++i)
		{
			if (m_enabledVertexAttributes[i])
			{
				GL_CHECK(glDisableVertexAttribArray(i));
				m_enabledVertexAttributes[i] = false;
			}
		}

		m_currentVbo.index = vboIndex;
	}
}

TextureID OpenGLLayer::CreateTexture()
{
	TextureInfo newTexture;
	newTexture.width = 0;
	newTexture.height = 0;
	GL_CHECK(glGenTextures(1, &newTexture.texture));

	// Internal resource indexing
	m_textures.add(newTexture);
	TextureID id = { m_textures.size - 1 };
	return id;
}

const TextureID& OpenGLLayer::LoadTexture(const TextureID& id,
										  int width, int height,
										  TextureType::Enum textureType,
										  TextureFormat::Enum textureFormat,
										  int side, int lodLevel,
										  void* data,
										  const TextureSampling& textureSampling)
{
	assert(m_textures.size > id.index);
	TextureInfo& texture = m_textures[id.index];

	texture.width = width;
	texture.height = height;
	texture.type = textureType;
	const GLenum target = (texture.type == GL_TEXTURE_2D ? texture.type : GL_TEXTURE_CUBE_MAP_POSITIVE_X + side);
	const GLenum internalFormat = getTextureFormat_InternalFormatGLenum(textureFormat);
	const GLenum format = getTextureFormat_FormatGLenum(textureFormat);
	const GLenum type = getTextureFormat_TypeGLenum(textureFormat);
	texture.format = format;

	int textureToRestore = 0;
	GL_CHECK(glGetIntegerv((texture.type == GL_TEXTURE_2D ? GL_TEXTURE_BINDING_2D : GL_TEXTURE_BINDING_CUBE_MAP), &textureToRestore));
	GL_CHECK(glBindTexture(texture.type, texture.texture));

	if (data != NULL && texture.type == GL_TEXTURE_2D)
	{
		if (lodLevel < 0)
		{
			GL_CHECK(glTexImage2D(target, 0, internalFormat, width, height, 0, format, type, data));
			if (textureSampling.minifyingFilter >= TextureFilter::NearestMipmapLinear)
			{
				glGenerateMipmap(GL_TEXTURE_2D);
			}
		}
		else
		{
			GL_CHECK(glTexImage2D(target, lodLevel, internalFormat, width, height, 0, format, type, data));
		}

		assert(textureSampling.maxAnisotropy >= 1.f);
		const GLenum minFilter = textureSampling.minifyingFilter;
		const GLenum magFilter = textureSampling.magnifyingFilter;
		GL_CHECK(glTexParameteri(texture.type, GL_TEXTURE_MIN_FILTER, minFilter));
		GL_CHECK(glTexParameteri(texture.type, GL_TEXTURE_MAG_FILTER, magFilter));
		GL_CHECK(glTexParameterf(texture.type, GL_TEXTURE_MAX_ANISOTROPY_EXT, textureSampling.maxAnisotropy));
	}
	else
	{
		GL_CHECK(glTexImage2D(target, 0, internalFormat, width, height, 0, format, type, data));

		// FIXME: if we set mipmapping for render to texture, we need
		// to generate the mipmaps with glGenerateMipmap(GL_TEXTURE_2D).
		// I don't know yet what would be a clean way to do it, so
		// let's have no mipmap for now.
		GL_CHECK(glTexParameteri(texture.type, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
		GL_CHECK(glTexParameteri(texture.type, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
		GL_CHECK(glTexParameterf(texture.type, GL_TEXTURE_MAX_ANISOTROPY_EXT, 2));
	}

	const GLenum sWrap = textureSampling.sWrap;
	const GLenum tWrap = textureSampling.tWrap;
	if (texture.type == GL_TEXTURE_CUBE_MAP)
	{
		const GLenum rWrap = textureSampling.rWrap;
		GL_CHECK(glTexParameteri(texture.type, GL_TEXTURE_WRAP_R, rWrap));
	}
	GL_CHECK(glTexParameteri(texture.type, GL_TEXTURE_WRAP_S, sWrap));
	GL_CHECK(glTexParameteri(texture.type, GL_TEXTURE_WRAP_T, tWrap));

	GL_CHECK(glBindTexture(texture.type, textureToRestore));

	return id;
}

void OpenGLLayer::DestroyTexture(const TextureID& id)
{
	assert(m_textures.size > id.index);
	GL_CHECK(glDeleteTextures(1, &m_textures[id.index].texture));
	m_textures[id.index].texture = 0;
}

void OpenGLLayer::BindTexture(const TextureID& id, int slot)
{
	assert(m_textures.size > id.index);
	const int textureIndex = id.index;

	assert(slot >= 0 && slot < NUM_TEXTURE_UNITS);
	if (m_currentTextures[slot].index != textureIndex)
	{
		GL_CHECK(glActiveTexture(GL_TEXTURE0 + slot));

		if (textureIndex >= 0)
		{
			GL_CHECK(glBindTexture(m_textures[textureIndex].type, m_textures[textureIndex].texture));
		}
		else
		{
			GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
		}

		m_currentTextures[slot].index = textureIndex;
	}
}

#if ENABLE_SHADER_COMPILATION_ERROR_CHECK

// This function is a complete hack, and tries to recognize and
// partially reformat errors from the shader compiler, so Visual Studio
// is able to parse the first line. When it works Visual Studio can
// then show us directly where the error is.
//
// The formatting used by Visual Studio looks like:
// d:\ctrl-alt-test\h2o\src\gfx\opengl\opengllayer.cpp(411): error C2220: warning treated as error - no 'object' file generated
static void ReformatShaderCompilerMessage(char originalLog[], const char* srcInfo, size_t maxLen)
{
#if ENABLE_SHADER_COMPILER_MESSAGE_PARSING
	char buffer[2048];

	if (strncmp(originalLog, "0(", 2) == 0)
	{
		// It seems to be the NVidia message format used here:
		// 0(97) : error C0000: syntax error, unexpected '=', expecting "::" at token "="
		// The log starts lines with a '0', where the name of the
		// source file should be.
		strcpy(buffer, originalLog + 1);
		strcpy(originalLog, srcInfo);
		size_t len = strlen(buffer) + strlen(srcInfo);
		strncpy(originalLog + strlen(srcInfo), buffer, (len >= maxLen ? maxLen - 1 : len));
	}
	else if (strncmp(originalLog, "ERRO R: 0:", 9) == 0)
	{
		// It seems to be the NVidia message format used here:
		// ERROR: 0:97: 'fragme' : undeclared identifier
		strcpy(buffer, originalLog + 8);
		buffer[0] = '(';
		char* column = strstr(buffer, ":");
		if (column != nullptr && column - buffer < 6)
		{
			column[0] = ')';
			column[1] = ':';
		}
		strcpy(originalLog, srcInfo);
		size_t len = strlen(buffer) + strlen(srcInfo);
		strncpy(originalLog + strlen(srcInfo), buffer, (len >= maxLen ? maxLen - 1 : len));
	}
	else
	{
		LOG_DEBUG("Could not recognize the formatting used in the following message. Please consider implementing it.");
	}
#endif // ENABLE_SHADER_COMPILER_MESSAGE_PARSING
	LOG_RAW(originalLog);
}
#endif // ENABLE_SHADER_COMPILATION_ERROR_CHECK

GLuint CompileShader(GLenum shaderType, const char* src, const char* srcInfo)
{
	if (src == NULL)
	{
		return 0;
	}

	const GLuint shader = glCreateShader(shaderType);
	GL_CHECK(glShaderSource(shader, 1, (const GLchar**)&src, NULL));
	GL_CHECK(glCompileShader(shader));

#if ENABLE_SHADER_COMPILATION_ERROR_CHECK
	GLint wentFine = GL_TRUE;
	GLsizei logsize = 0;
	GL_CHECK(glGetShaderiv(shader, GL_COMPILE_STATUS, &wentFine));
	GL_CHECK(glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logsize));

	if (logsize != 0)
	{
		// Using a static array to avoid using new[].
		char log[2048];

		GL_CHECK(glGetShaderInfoLog(shader, sizeof(log) / sizeof(log[0]) - 1, &logsize, log));
		if (logsize != 0)
		{
			if (wentFine == GL_TRUE)
			{
				LOG_WARNING("Something might be wrong in %s:", srcInfo);
			}
			else
			{
				LOG_ERROR("Compilation of %s failed:", srcInfo);
			}
			ReformatShaderCompilerMessage(log, srcInfo, sizeof(log));
		}
	}

#if _HAS_EXCEPTIONS
	if (wentFine != GL_TRUE)
	{
		throw new std::exception("Shader compilation failed.");
	}
#endif // _HAS_EXCEPTIONS
#endif // ENABLE_SHADER_COMPILATION_ERROR_CHECK

	return shader;
}

GLuint CreateAndLinkProgram(const GLuint* shaders, int numShaders)
{
	const GLuint program = glCreateProgram();
	for (int i = 0; i < numShaders; ++i)
	{
		GL_CHECK(glAttachShader(program, shaders[i]));
	}
	GL_CHECK(glLinkProgram(program));

#if ENABLE_SHADER_COMPILATION_ERROR_CHECK
	GLint wentFine = GL_TRUE;
	GLsizei logsize = 0;
	GL_CHECK(glGetProgramiv(program, GL_LINK_STATUS, &wentFine));
	GL_CHECK(glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logsize));

	if (logsize != 0)
	{
		// Using a static array to avoid using new[].
		char log[2048];

		GL_CHECK(glGetProgramInfoLog(program, sizeof(log) / sizeof(log[0]) - 1, &logsize, log));
		if (logsize != 0)
		{
			if (wentFine == GL_TRUE)
			{
				LOG_WARNING(log);
			}
			else
			{
				LOG_ERROR(log);
			}
		}
	}

	for (int i = 0; i < numShaders; ++i)
	{
		GL_CHECK(glDetachShader(program, shaders[i]));
	}

#if _HAS_EXCEPTIONS
	if (wentFine != GL_TRUE)
	{
		throw new std::exception("Shader linking failed.");
	}
#endif // _HAS_EXCEPTIONS
#endif // ENABLE_SHADER_COMPILATION_ERROR_CHECK

	return program;
}

ShaderID OpenGLLayer::CreateShader()
{
	ShaderInfo newShader;
	newShader.shaders[0] = 0;
	newShader.shaders[1] = 0;
	newShader.program = 0;

	// Internal resource indexing
	m_shaders.add(newShader);
	ShaderID id = { m_shaders.size - 1 };
	return id;
}

const ShaderID& OpenGLLayer::LoadShader(const ShaderID& id,
										const char* vertexShaderSource,
										const char* vertexShaderSourceInfo,
										const char* fragmentShaderSource,
										const char* fragmentShaderSourceInfo)
{
	assert(m_shaders.size > id.index);
	ShaderInfo& shader = m_shaders[id.index];

	GL_CHECK(glDeleteProgram(shader.program)); // From the manual: "A value of 0 for program will be silently ignored."
	GL_CHECK(glDeleteShader(shader.shaders[0])); // From the manual: "A value of 0 for shader will be silently ignored."
	GL_CHECK(glDeleteShader(shader.shaders[1]));
	shader.program = 0;
	shader.shaders[0] = 0;
	shader.shaders[1] = 0;

	// Warning: the following functions can throw exceptions, which
	// means shader.program and shader.shaders[x] might not be set in
	// case of compilation error. That's why we set them to 0 first.
	shader.shaders[0] = CompileShader(GL_VERTEX_SHADER, vertexShaderSource, vertexShaderSourceInfo);
	shader.shaders[1] = CompileShader(GL_FRAGMENT_SHADER, fragmentShaderSource, fragmentShaderSourceInfo);
	shader.program = CreateAndLinkProgram(shader.shaders, 2);

	return id;
}

void OpenGLLayer::DestroyShader(const ShaderID& id)
{
	assert(m_shaders.size > id.index);
	ShaderInfo& shader = m_shaders[id.index];

	GL_CHECK(glDeleteProgram(shader.program));
	GL_CHECK(glDeleteShader(shader.shaders[0]));
	GL_CHECK(glDeleteShader(shader.shaders[1]));
	shader.program = 0;
	shader.shaders[0] = 0;
	shader.shaders[1] = 0;

	BindShader(ShaderID::InvalidID);
}

void OpenGLLayer::BindShader(const ShaderID& id)
{
	const int shaderIndex = id.index;

	if (m_currentShader.index != shaderIndex)
	{
		if (shaderIndex >= 0)
		{
			GL_CHECK(glUseProgram(m_shaders[shaderIndex].program));
		}
		else
		{
			GL_CHECK(glUseProgram(0));
		}

		m_currentShader.index = shaderIndex;
	}
}

void OpenGLLayer::BindUniforms(const Uniform* uniforms, int numberOfUniforms)
{
	GLuint program = 0;
	if (m_currentShader.index >= 0)
	{
		program = m_shaders[m_currentShader.index].program;
	}

	if (program != 0)
	{
		int textureSlot = 0;
		for (int i = 0; i < numberOfUniforms; ++i)
		{
			const Uniform& uniform = uniforms[i];
			GLint location = glGetUniformLocation(program, uniform.name);
			assert(uniform.size > 0 && uniform.size <= 16);

			if (uniform.type == UniformType::Float)
			{
				switch (uniform.size)
				{
				case 1: GL_CHECK(glUniform1fv(location, 1, uniform.fValue)); break;
				case 2: GL_CHECK(glUniform2fv(location, 1, uniform.fValue)); break;
				case 3: GL_CHECK(glUniform3fv(location, 1, uniform.fValue)); break;
				case 4: GL_CHECK(glUniform4fv(location, 1, uniform.fValue)); break;
// 				case 9: GL_CHECK(glUniformMatrix3fv(location, 1, GL_FALSE, uniform.fValue)); break;
				case 16:GL_CHECK(glUniformMatrix4fv(location, 1, GL_FALSE, uniform.fValue)); break;
				default:
					LOG_ERROR("Uniform size not supported: %s.", uniform.name);
					break;
				}
			}
			else if (uniform.type == UniformType::Int)
			{
				switch (uniform.size)
				{
				case 1: GL_CHECK(glUniform1iv(location, 1, uniform.iValue)); break;
				case 2: GL_CHECK(glUniform2iv(location, 1, uniform.iValue)); break;
				case 3: GL_CHECK(glUniform3iv(location, 1, uniform.iValue)); break;
				case 4: GL_CHECK(glUniform4iv(location, 1, uniform.iValue)); break;
				default:
					LOG_ERROR("Uniform size not supported: %s.", uniform.name);
					break;
				}
			}
			else
			{
				assert(uniform.size == 1);
				assert(textureSlot < NUM_TEXTURE_UNITS);
				GL_CHECK(glUniform1iv(location, 1, &textureSlot));
				BindTexture(uniform.id, textureSlot++);
			}
		}
	}
}

FrameBufferID OpenGLLayer::CreateFrameBuffer(const TextureID* textures, int numberOfTextures)
{
	assert(textures != NULL && numberOfTextures > 0);

	FBOInfo newFBO;
	newFBO.width = m_textures[textures[0].index].width;
	newFBO.height = m_textures[textures[0].index].height;

	int frameBufferToRestore = 0;
	GL_CHECK(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &frameBufferToRestore));

	GL_CHECK(glGenFramebuffers(1, &newFBO.frameBuffer));
	GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, newFBO.frameBuffer));
	m_currentFrameBuffer.index = m_fbos.size - 1;

	GLenum buffers[MAX_MRT];
	int numberOfBuffers = 0;

	for (int i = 0; i < numberOfTextures; ++i)
	{
		const TextureInfo& texture = m_textures[textures[i].index];
		assert(texture.width == newFBO.width && texture.height == newFBO.height);

		GLenum attachment = GL_COLOR_ATTACHMENT0 + i;
		if (texture.format == GL_STENCIL_INDEX)
		{
			attachment = GL_STENCIL_ATTACHMENT;
		}
		else if (texture.format == GL_DEPTH_COMPONENT)
		{
			attachment = GL_DEPTH_ATTACHMENT;
		}
		else if (texture.format == GL_DEPTH_STENCIL)
		{
			attachment = GL_DEPTH_STENCIL_ATTACHMENT;
		}
		else
		{
			assert(numberOfBuffers < MAX_MRT);
			buffers[numberOfBuffers++] = attachment;
		}
		GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, attachment,
										GL_TEXTURE_2D,				// FIXME: cube map support
										texture.texture, 0));
	}
	// According to the OpenGL 3.3 spec, the buffer selection state is per
	// frame buffer.
	glDrawBuffers(numberOfBuffers, buffers);

	assert(GL_FRAMEBUFFER_COMPLETE == glCheckFramebufferStatus(GL_FRAMEBUFFER));

	m_fbos.add(newFBO);

	FrameBufferID id = { m_fbos.size - 1 };
	return id;
}

void OpenGLLayer::DestroyFrameBuffer(const FrameBufferID& id)
{
	GL_CHECK(glDeleteFramebuffers(1, &m_fbos[id.index].frameBuffer));
	m_fbos[id.index].frameBuffer = 0;
}

void OpenGLLayer::BindFrameBuffer(const FrameBufferID& frameBuffer)
{
	const int frameBufferIndex = frameBuffer.index;

	if (m_currentFrameBuffer.index != frameBufferIndex)
	{
		if (frameBufferIndex >= 0)
		{
			GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, m_fbos[frameBufferIndex].frameBuffer));
		}
		else
		{
			GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
		}

		m_currentFrameBuffer.index = frameBufferIndex;
	}
}

void OpenGLLayer::Clear(const FrameBufferID& frameBuffer, float r, float g, float b, bool clearDepth)
{
	BindFrameBuffer(frameBuffer);

	GL_CHECK(glClearColor(r, g, b, 0.0f));

#if GFX_ENABLE_DEPTH_TESTING
	if (clearDepth)
	{
		m_currentRasterTests.depthWrite = true;
		GL_CHECK(glDepthMask(GL_TRUE));
		GL_CHECK(glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT));
	}
	else
#endif // GFX_ENABLE_DEPTH_TESTING
	{
		GL_CHECK(glClear(GL_COLOR_BUFFER_BIT));
	}
}

void OpenGLLayer::Draw(const DrawArea& drawArea,
					   const RasterTests& rasterTests,
					   const Geometry& geometry,
					   const ShadingParameters& shadingParameters)
{
	BindVertexBuffer(geometry.vertexBuffer);
	BindShader(shadingParameters.shader);
	BindUniforms(shadingParameters.uniforms.elt, shadingParameters.uniforms.size);
	BindFrameBuffer(drawArea.frameBuffer);

	SetRasterizerState(drawArea.viewport,
		shadingParameters.polygonMode,
		rasterTests,
		shadingParameters.blendingMode);

	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	if (geometry.vertexBuffer.index >= 0)
	{
#if GFX_ENABLE_VERTEX_BUFFER_OFFSET
		const int firstIndexOffset = geometry.firstIndexOffset;
#else // !GFX_ENABLE_VERTEX_BUFFER_OFFSET
		const int firstIndexOffset = 0;
#endif // !GFX_ENABLE_VERTEX_BUFFER_OFFSET

		const VBOInfo& vbo = m_vbos[geometry.vertexBuffer.index];
		if (geometry.numberOfInstances <= 1)
		{
			if (vbo.indexed)
			{
				GL_CHECK(glDrawElements(GL_TRIANGLES, geometry.numberOfIndices, vbo.indexType, (void*)firstIndexOffset));
			}
			else
			{
				GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, geometry.numberOfIndices));
			}
		}
		else
		{
			if (vbo.indexed)
			{
				GL_CHECK(glDrawElementsInstanced(GL_TRIANGLES, geometry.numberOfIndices, vbo.indexType, (void*)firstIndexOffset, geometry.numberOfInstances));
			}
			else
			{
				GL_CHECK(glDrawArraysInstanced(GL_TRIANGLES, 0, geometry.numberOfIndices, geometry.numberOfInstances));
			}
		}
	}
}

#endif // defined(GFX_MULTI_API) || defined(GFX_OPENGL_ONLY)
