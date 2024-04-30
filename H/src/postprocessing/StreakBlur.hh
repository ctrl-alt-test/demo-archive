#ifndef STREAK_BLUR_HH
#define STREAK_BLUR_HH

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
	class StreakBlur
	{
	public:
		explicit StreakBlur(Gfx::IGraphicLayer* gfxLayer):
			m_gfxLayer(gfxLayer)
		{
		}

		void Init(int bufferWidth, int bufferHeight,
				  const Gfx::Geometry& quad,
				  Tool::ShaderLoader* shaderLoader);
		void Apply(const Gfx::TextureID& inputImage,
				   const Algebra::vector3f& horizontalStreakPower,
				   const Algebra::vector3f& verticalStreakPower);

		const Gfx::TextureID HorizontalResultTexture() const
		{
			return m_hFBTextures[0];
		}

		const Gfx::TextureID VerticalResultTexture() const
		{
			return m_vFBTextures[0];
		}

	private:
		void SetShaderParameters(int pass,
								 const Gfx::TextureID& texture0,
								 const Gfx::TextureID& texture1,
								 const Algebra::vector3f& streakPower,
								 float directionX, float directionY);

		Gfx::IGraphicLayer*		m_gfxLayer;
		int						m_bufferWidth;
		int						m_bufferHeight;
		int						m_passes;

		Gfx::Geometry			m_geometry;
		Gfx::ShadingParameters	m_shading;

		Gfx::TextureID			m_hFBTextures[2];
		Gfx::TextureID			m_vFBTextures[2];
		Gfx::TextureID			m_tmpFBTextures[2];
		Gfx::FrameBufferID		m_hFB;
		Gfx::FrameBufferID		m_vFB;
		Gfx::FrameBufferID		m_tmpFB;
	};
}

#endif // STREAK_BLUR_HH
