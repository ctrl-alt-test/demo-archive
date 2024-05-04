#ifndef I_GRAPHIC_LAYER_HH
#define I_GRAPHIC_LAYER_HH

#include "TextureFormat.hh"
#include "VertexAttribute.hh"
#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

namespace Gfx
{
	struct DrawArea;
	struct FrameBufferID;
	struct Geometry;
	struct RasterTests;
	struct ShaderID;
	struct ShadingParameters;
	struct TextureID;
	struct VertexAttribute;
	struct VertexBufferID;

#ifdef GFX_MULTI_API
	/// <summary>
	/// A minimalist graphics API abstraction.
	/// </summary>
	class IGraphicLayer
	{
	public:
		virtual ~IGraphicLayer();

		virtual void				CreateRenderingContext() = 0;
		virtual void				DestroyRenderingContext() = 0;

		/// <summary>
		/// Creates an uninitialized vertex buffer.
		/// </summary>
		virtual VertexBufferID		CreateVertexBuffer() = 0;
		/// <summary>
		/// Sets the vertex data of an existing vertex buffer.
		/// </summary>
		///
		/// <param name="vertexAttributes">List of vertex attributes
		///     included in the data.</param>
		/// <param name="numberOfAttributes">Number of vertex
		///     attributes of the list.</param>
		/// <param name="stride">Offset in bytes between one vertex
		///     and the next.</param>
		/// <param name="vertexDataSize">Size of the vertex data in
		///     bytes.</param>
		/// <param name="vertexData">Raw vertex data.</param>
		/// <param name="indexDataSize">Size of the index data in
		///     bytes. The vertex buffer is assumed to be non indexed
		///     if the value is 0.</param>
		/// <param name="indexData">Raw index data.</param>
		/// <param name="indexType">Type of the indices.</param>
		virtual const VertexBufferID&	LoadVertexBuffer(const VertexBufferID& id,
														 const VertexAttribute* vertexAttributes,
														 int numberOfAttributes, int stride,
														 int vertexDataSize, void* vertexData,
														 int indexDataSize, void* indexData,
														 VertexIndexType::Enum indexType) = 0;
		virtual void				DestroyVertexBuffer(const VertexBufferID& id) = 0;

		/// <summary>
		/// Creates an uninitialized texture.
		/// </summary>
		virtual TextureID			CreateTexture() = 0;
		/// <summary>
		/// Sets the texture data of an existing texture.
		/// </summary>
		///
		/// <param name="textureType">2D texture or cube map.</param>
		/// <param name="textureFormat">Packing format to use for data.</param>
		/// <param name="side">In case of a cube map, face of the cube from 0 to 5.</param>
		/// <param name="lodLevel">-1 to automatically generate mipmap; mipmap level otherwise.</param>
		virtual const TextureID&	LoadTexture(const TextureID& id,
												int width, int height,
												TextureType::Enum textureType,
												TextureFormat::Enum textureFormat,
												int side, int lodLevel,
												void* data,
												const TextureSampling& textureSampling) = 0;
		virtual void				DestroyTexture(const TextureID& id) = 0;

		/// <summary>
		/// Creates an uninitialized shader program.
		/// </summary>
		virtual ShaderID			CreateShader() = 0;
		/// <summary>
		/// Compiles and sets the code of an existing shader program.
		/// </summary>
		virtual const ShaderID&		LoadShader(const ShaderID& id,
											   const char* vertexShaderSource,
											   const char* vertexShaderSourceInfo,
											   const char* fragmentShaderSource,
											   const char* fragmentShaderSourceInfo) = 0;
		virtual void				DestroyShader(const ShaderID& id) = 0;

		virtual FrameBufferID		CreateFrameBuffer(const TextureID* textures,
													  int numberOfTextures) = 0;
		virtual void				DestroyFrameBuffer(const FrameBufferID& id) = 0;

		virtual void				Clear(const FrameBufferID& frameBuffer, float r, float g, float b, bool clearDepth) = 0;

		/// <summary>
		/// Draws one mesh or more, using the given parameters.
		/// </summary>
		virtual void				Draw(const DrawArea& drawArea,
										 const RasterTests& rasterTests,
										 const Geometry& geometry,
										 const ShadingParameters& shadingParameters) = 0;
		virtual void				EndFrame() = 0;
	};
#endif // GFX_MULTI_API

// FIXME: this should not be in this file. IGraphicsLayer should stay
// clear of any dependency over existing implementations.
#ifdef GFX_DIRECTX_ONLY
#define DirectXLayer IGraphicLayer
#endif // GFX_DIRECTX_ONLY

#ifdef GFX_OPENGL_ONLY
#define OpenGLLayer IGraphicLayer
#endif // GFX_OPENGL_ONLY
}

#endif // I_GRAPHIC_LAYER_HH
