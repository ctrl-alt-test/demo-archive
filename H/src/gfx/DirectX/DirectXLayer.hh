#ifndef DIRECTX_LAYER_HH
#define DIRECTX_LAYER_HH

#ifdef _WIN32

#include "engine/container/Array.hh" // FIXME: remove dependency, ideally Gfx should not have dependency over Engine
#include "gfx/IGraphicLayer.hh"
#include <d3d9.h>

#if defined(GFX_DIRECTX_ONLY) || defined(GFX_MULTI_API)

namespace Gfx
{
	/// <summary>
	/// DirectX implementation of an IGraphicLayer (work in progress).
	/// </summary>
	class DirectXLayer
#ifdef GFX_MULTI_API
		: public IGraphicLayer
#endif // GFX_MULTI_API
	{
	public:
		DirectXLayer();

		void					CreateRenderingContext(const HWND& hWnd);
		void					DestroyRenderingContext();

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
		void					EndFrame();

	private:
		struct VertexBufferInfo
		{
			IDirect3DVertexBuffer9*	buffer;
			DWORD					fvf;
			int						stride;
			int						primitiveCount;
		};

		struct TextureInfo
		{
			IDirect3DTexture9*		buffer;
		};

	private:
		LPDIRECT3D9				m_d3d;
		LPDIRECT3DDEVICE9		m_device;

		Container::Array<VertexBufferInfo>	m_vertexBuffers;
		Container::Array<TextureInfo>		m_textures;
	};
}

#endif // defined(GFX_DIRECTX_ONLY) || defined(GFX_MULTI_API)

#endif // _WIN32

#endif // DIRECTX_LAYER_HH
