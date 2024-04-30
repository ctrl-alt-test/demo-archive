#ifndef COMMON_POST_PROCESSING_HH
#define COMMON_POST_PROCESSING_HH

#include "engine/algebra/Vector3.hh"
#include "postprocessing/FXAA.hh"
#include "postprocessing/FinalCombine.hh"
#include "postprocessing/GaussianBlur.hh"
#include "postprocessing/StreakBlur.hh"

namespace Core
{
	class Settings;
}

namespace Gfx
{
	class IGraphicLayer;
	struct Geometry;
	struct TextureID;
}

namespace Tool
{
	class ShaderLoader;
	class TextureLoader;
}

namespace PostProcessing
{
	class CommonPostProcessing
	{
	public:
		explicit CommonPostProcessing(Gfx::IGraphicLayer* gfxLayer):
			m_gfxLayer(gfxLayer),
			//m_gaussianBlur(gfxLayer),
			m_streakBlur(gfxLayer),
			m_finalCombine(gfxLayer),
			m_fxaa(gfxLayer)
		{
		}

		void Init(Gfx::IGraphicLayer* gfxLayer,
				  const Gfx::Geometry& quad,
				  Tool::ShaderLoader* shaderLoader,
				  Tool::TextureLoader* textureLoader,
				  int renderWidth, int renderHeight,
				  int displayWidth, int displayHeight);
		void Apply(long currentTime,
				   const Gfx::TextureID& texture,
				   const Algebra::vector3f& bloomFilter,
				   const Algebra::vector3f& horizontalStreakPower,
				   const Algebra::vector3f& verticalStreakPower,
				   const Algebra::vector3f& streakFilter,
				   const Algebra::vector3f& lift,
				   const Algebra::vector3f& gamma,
				   const Algebra::vector3f& gain,
				   float exposure,
				   float vignetting,
				   float saturation);

	private:
		Gfx::IGraphicLayer*		m_gfxLayer;
		int						m_windowWidth;
		int						m_windowHeight;

		GaussianBlur			m_gaussianBlur;
		StreakBlur				m_streakBlur;
		FinalCombine			m_finalCombine;
		FXAA					m_fxaa;

		Gfx::Geometry			m_geometry;
		Gfx::ShadingParameters	m_blitShader;
	};
}

#endif // COMMON_POST_PROCESSING_HH
