#ifndef TONE_MAPPING_HH
#define TONE_MAPPING_HH

#include "gfx/Geometry.hh"
#include "gfx/ShadingParameters.hh"

namespace Gfx
{
	class IGraphicLayer;
}

namespace Tool
{
	class ShaderLoader;
}

namespace PostProcessing
{
	class ToneMapping
	{
	public:
		ToneMapping(Gfx::IGraphicLayer* gfxLayer);

		void Init(int bufferWidth, int bufferHeight,
				  const Gfx::Geometry& quad,
				  Tool::ShaderLoader* shaderLoader);

		void Apply(const Gfx::TextureID& inputImage);

		const Gfx::TextureID ResultTexture() const;

	private:
		Gfx::IGraphicLayer*		m_gfxLayer;
		int						m_bufferWidth;
		int						m_bufferHeight;

		Gfx::Geometry			m_geometry;
		Gfx::ShadingParameters	m_shading;

		Gfx::TextureID			m_FBTextures[1];
		Gfx::FrameBufferID		m_FB;
	};
}

#endif // TONE_MAPPING_HH
