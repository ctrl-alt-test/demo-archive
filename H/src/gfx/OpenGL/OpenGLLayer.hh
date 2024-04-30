#ifndef OPEN_GL_LAYER_HH
#define OPEN_GL_LAYER_HH

#include "engine/container/Array.hh"
// FIXME: ideally Gfx should not have dependency over Engine.
#include "gfx/BlendingMode.hh"
#include "gfx/DrawArea.hh"
#include "gfx/IGraphicLayer.hh"
#include "gfx/PolygonMode.hh"
#include "gfx/RasterTests.hh"
#include <GL/gl.h>

#define MAX_VERTEX_ATTRIBUTES 16
#define MAX_VERTEX_BUFFERS	256
#define MAX_TEXTURES		256
#define MAX_SHADERS			256
#define MAX_FRAME_BUFFERS	256
#define NUM_TEXTURE_UNITS	16

#if defined(GFX_OPENGL_ONLY) || defined(GFX_MULTI_API)

namespace Gfx
{
	struct BlendingMode;
	struct Uniform;

	/// <summary>
	/// OpenGL implementation of an IGraphicLayer.
	/// </summary>
	class OpenGLLayer
#ifdef GFX_MULTI_API
		: public IGraphicLayer
#endif // GFX_MULTI_API
	{
	public:
		void					CreateRenderingContext();
		void					DestroyRenderingContext() {}

		VertexBufferID			CreateVertexBuffer();
		const VertexBufferID&	LoadVertexBuffer(const VertexBufferID& id,
												 const VertexAttribute* vertexAttributes,
												 int numberOfAttributes, int stride,
												 int vertexDataSize, void* vertexData,
												 int indexDataSize, void* indexData,
												 VertexIndexType::Enum indexType);
		void					DestroyVertexBuffer(const VertexBufferID& id);

		TextureID				CreateTexture();
		const TextureID&		LoadTexture(const TextureID& id,
											int width, int height,
											TextureType::Enum textureType,
											TextureFormat::Enum textureFormat,
											int side, int lodLevel,
											void* data,
											const TextureSampling& textureSampling);
		void					DestroyTexture(const TextureID& id);
		ShaderID				CreateShader();
		const ShaderID&			LoadShader(const ShaderID& id,
										   const char* vertexShaderSource,
										   const char* vertexShaderSourceInfo,
										   const char* fragmentShaderSource,
										   const char* fragmentShaderSourceInfo);
		void					DestroyShader(const ShaderID& id);

		FrameBufferID			CreateFrameBuffer(const TextureID* textures,
												  int numberOfTextures);
		void					DestroyFrameBuffer(const FrameBufferID& id);

		void					Clear(const FrameBufferID& frameBuffer, float r, float g, float b, bool clearDepth);
		void					Draw(const DrawArea& drawArea,
									 const RasterTests& rasterTests,
									 const Geometry& geometry,
									 const ShadingParameters& shadingParameters);
		void					EndFrame() {}

	private:
		// These methods are private so from the outside the API looks stateless.
		void					SetRasterizerState(const Viewport& viewport,
												   const PolygonMode::Enum& polygonMode,
												   const RasterTests& rasterTests,
												   const BlendingMode& blendingMode);
		void					BindVertexBuffer(const VertexBufferID& id);
		void					BindTexture(const TextureID& id, int slot);
		void					BindShader(const ShaderID& id);
		void					BindUniforms(const Uniform* uniforms, int numberOfUniforms);
		void					BindFrameBuffer(const FrameBufferID& framebuffer);

	private:
		struct VBOInfo
		{
			const VertexAttribute* vertexAttributes;
			int		numberOfAttributes;
			int		stride;

			GLuint	vertexBuffer;
			GLuint	indexBuffer;
			GLenum	indexType;
			bool	indexed;
		};

		struct TextureInfo
		{
			GLuint	texture;
			int		width;
			int		height;
			GLenum	type;
			GLenum	format;
		};

		struct ShaderInfo
		{
			GLuint	program;
			GLuint	shaders[2];
		};

		struct FBOInfo
		{
			GLuint	frameBuffer;
			int		width;
			int		height;
		};

		Container::Array<VBOInfo>		m_vbos;
		Container::Array<TextureInfo>	m_textures;
		Container::Array<ShaderInfo>	m_shaders;
		Container::Array<FBOInfo>		m_fbos;

		Viewport					m_currentViewport;
		RasterTests					m_currentRasterTests;
		BlendingMode				m_currentBlendingMode;
		PolygonMode::Enum			m_currentPolygonMode;

		VertexBufferID				m_currentVbo;
		TextureID					m_currentTextures[NUM_TEXTURE_UNITS];
		ShaderID					m_currentShader;
		FrameBufferID				m_currentFrameBuffer;

		bool						m_enabledVertexAttributes[MAX_VERTEX_ATTRIBUTES];
	};
}

#endif // defined(GFX_OPENGL_ONLY) || defined(GFX_MULTI_API)

#endif // OPEN_GL_LAYER_HH
