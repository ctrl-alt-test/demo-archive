#ifndef FXAA_HH
#define FXAA_HH

#include "gfx/Geometry.hh"
#include "gfx/ShadingParameters.hh"

namespace Gfx
{
	class IGraphicLayer;
}

namespace Tool
{
	class ShaderLoader;
	class TextureLoader;
}

namespace PostProcessing
{
	class FXAA
	{
	public:
		explicit FXAA(Gfx::IGraphicLayer* gfxLayer):
			m_gfxLayer(gfxLayer)
		{
		}

		void Init(int bufferWidth, int bufferHeight,
				  const Gfx::Geometry& quad,
				  Tool::ShaderLoader* shaderLoader,
				  Tool::TextureLoader* textureLoader);
		void Apply(long currentTime,
				   const Gfx::TextureID& inputImage);

		const Gfx::TextureID ResultTexture() const
		{
			return m_FBTextures[0];
		}

	private:
		Gfx::IGraphicLayer*		m_gfxLayer;
		int						m_bufferWidth;
		int						m_bufferHeight;
		Gfx::TextureID*			m_randomTexture;

		Gfx::Geometry			m_geometry;
		Gfx::ShadingParameters	m_shading;

		Gfx::TextureID			m_FBTextures[1];
		Gfx::FrameBufferID		m_FB;
	};
}

#endif // FXAA_HH
