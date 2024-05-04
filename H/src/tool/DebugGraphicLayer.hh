#ifndef DEBUG_GRAPHIC_LAYER_HH
#define DEBUG_GRAPHIC_LAYER_HH

#if DEBUG

#include "gfx/Geometry.hh"
#include "gfx/IGraphicLayer.hh"
#include "gfx/ShadingParameters.hh"
#include <vector>

namespace Tool
{
	struct DebugControls;

	/// <summary>
	/// A debug implementation of an IGraphicLayer. It uses an actual
	/// implementation and redirects all calls to add debug
	/// functionality.
	/// </summary>
	class DebugGraphicLayer : public Gfx::IGraphicLayer
	{
	public:
		DebugGraphicLayer(Gfx::IGraphicLayer& gfxLayer,
						  const DebugControls* debugControls);

		virtual void					CreateRenderingContext();
		virtual void					DestroyRenderingContext();

		virtual Gfx::VertexBufferID		CreateVertexBuffer();
		virtual const Gfx::VertexBufferID&	LoadVertexBuffer(const Gfx::VertexBufferID& id,
															 const Gfx::VertexAttribute* vertexAttributes,
															 int numberOfAttributes, int stride,
															 int vertexDataSize, void* vertexData,
															 int indexDataSize, void* indexData,
															 Gfx::VertexIndexType::Enum indexType);
		virtual void					DestroyVertexBuffer(const Gfx::VertexBufferID& id);

		virtual Gfx::TextureID			CreateTexture();
		virtual const Gfx::TextureID&	LoadTexture(const Gfx::TextureID& id,
													int width, int height,
													Gfx::TextureType::Enum textureType,
													Gfx::TextureFormat::Enum textureFormat,
													int side, int lodLevel,
													void* data,
													const Gfx::TextureSampling& textureSampling);
		virtual void					DestroyTexture(const Gfx::TextureID& id);

		virtual Gfx::ShaderID			CreateShader();
		virtual const Gfx::ShaderID&	LoadShader(const Gfx::ShaderID& id,
												   const char* vertexShaderSource,
												   const char* vertexShaderSourceInfo,
												   const char* fragmentShaderSource,
												   const char* fragmentShaderSourceInfo);
		virtual void					DestroyShader(const Gfx::ShaderID& id);

		virtual Gfx::FrameBufferID		CreateFrameBuffer(const Gfx::TextureID* textures,
														  int numberOfTextures);
		virtual void					DestroyFrameBuffer(const Gfx::FrameBufferID& id);

		virtual void					Clear(const Gfx::FrameBufferID& frameBuffer, float r, float g, float b, bool clearDepth);

		virtual void					Draw(const Gfx::DrawArea& drawArea,
											 const Gfx::RasterTests& rasterTests,
											 const Gfx::Geometry& geometry,
											 const Gfx::ShadingParameters& shadingParameters);
		virtual void					EndFrame();

		void							InitBlitDisplay();
		void							ResetDrawCallsCount() { m_drawCallsCount = 0; }
		int								DrawCallsCount() const { return m_drawCallsCount; }

	private:
		void						SetupCustomShadingForDrawCall(const Gfx::ShadingParameters& originalShading, int numberOfIndices);
		void						DisplayFrameBuffer(const Gfx::FrameBufferID& frameBuffer);

		Gfx::IGraphicLayer&			m_gfxLayer;
		const DebugControls*		m_debugControls;
		int							m_drawCallsCount;

		Gfx::Geometry				m_quadGeometry;
		Gfx::ShaderID				m_colorBufferShader;
		Gfx::ShaderID				m_depthBufferShader;
		Gfx::ShadingParameters		m_shading;
		Gfx::ShadingParameters		m_customDrawCallShading1;
		Gfx::ShadingParameters		m_customDrawCallShading2;

		struct VertexBufferInfo
		{
			Gfx::VertexBufferID		vertexBuffer;
			int						stride;
			int						vertexDataSize;
			int						indexDataSize;
		};
		std::vector<VertexBufferInfo>	m_vertexBuffersInfo;

		struct ShaderInfo
		{
			Gfx::ShaderID			shader;
			Gfx::ShaderID			whiteShader;
			Gfx::ShaderID			unlitWhiteShader;
			Gfx::ShaderID			highlightShader;
			Gfx::ShaderID			showTexCoordShader;
		};
		std::vector<ShaderInfo>		m_shadersInfo;

		struct TextureInfo
		{
			Gfx::TextureID			texture;
			int						width;
			int						height;
			Gfx::TextureType::Enum	type;
			Gfx::TextureFormat::Enum format;
		};
		std::vector<TextureInfo>	m_texturesInfo;

		struct FrameBufferInfo
		{
			Gfx::FrameBufferID			frameBuffer;
			std::vector<Gfx::TextureID>	textures;
		};
		std::vector<FrameBufferInfo>	m_frameBuffersInfo;
	};
}

#endif // DEBUG

#endif // !DEBUG_GRAPHIC_LAYER_HH
