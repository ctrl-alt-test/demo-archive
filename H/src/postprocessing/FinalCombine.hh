#ifndef FINAL_COMBINE_HH
#define FINAL_COMBINE_HH

#include "engine/algebra/Vector3.hh"
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
	class FinalCombine
	{
	public:
		explicit FinalCombine(Gfx::IGraphicLayer* gfxLayer):
			m_gfxLayer(gfxLayer)
		{
		}

		void Init(int bufferWidth, int bufferHeight,
				  const Gfx::Geometry& quad,
				  Tool::ShaderLoader* shaderLoader);

		// It's not very clean to have the shader implementation
		// details leak up to the class method, but it's either that
		// or an array of anonymous textures...
		void Apply(long currentTime,
				   const Gfx::TextureID& inputImage,
				   const Gfx::TextureID& gaussianBlurredImage,
				   const Gfx::TextureID& hStreakBlurredImage,
				   const Gfx::TextureID& vStreakBlurredImage,
				   const Algebra::vector3f& bloom,
				   const Algebra::vector3f& streak,
				   const Algebra::vector3f& lift,
				   const Algebra::vector3f& gamma,
				   const Algebra::vector3f& gain,
				   float exposure,
				   float vignetting,
				   float saturation);

		const Gfx::TextureID ResultTexture() const
		{
			return m_FBTextures[0];
		}

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

#endif // FINAL_COMBINE_HH
