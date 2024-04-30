#ifndef BILATERAL_GAUSSIAN_BLUR_HH
#define BILATERAL_GAUSSIAN_BLUR_HH

#include "gfx/Geometry.hh"
#include "gfx/ShadingParameters.hh"
#include "gfx/TextureFormat.hh"

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
	class BilateralGaussianBlur
	{
	public:
		BilateralGaussianBlur():
			m_gfxLayer(nullptr),
			m_bufferWidth(0),
			m_bufferHeight(0)
		{}

		void Init(Gfx::IGraphicLayer* gfxLayer,
				  int bufferWidth, int bufferHeight,
				  const Gfx::Geometry& quad,
				  Tool::ShaderLoader* shaderLoader,
				  Gfx::TextureFormat::Enum textureFormat,
				  const Gfx::TextureID& destTexture = Gfx::TextureID::InvalidID,
				  const Gfx::TextureID& tempTexture = Gfx::TextureID::InvalidID);
		void Apply(const Gfx::TextureID& inputImage,
				   const Gfx::TextureID& depthMap,
				   float zNear, float zFar, float sharpness,
				   int passes);

		const Gfx::TextureID ResultTexture() const
		{
			return m_FBTextures[0];
		}

	private:
		void SetShaderParameters(int pass,
								 const Gfx::TextureID& texture,
								 const Gfx::TextureID& depthMap,
								 float directionX, float directionY,
								 float zNear, float zFar, float sharpness);

		Gfx::IGraphicLayer*		m_gfxLayer;
		int						m_bufferWidth;
		int						m_bufferHeight;

		Gfx::Geometry			m_geometry;
		Gfx::ShadingParameters	m_shading;

		Gfx::TextureID			m_FBTextures[1];
		Gfx::TextureID			m_tmpFBTextures[1];
		Gfx::FrameBufferID		m_FB;
		Gfx::FrameBufferID		m_tmpFB;
	};
}

#endif // BILATERAL_GAUSSIAN_BLUR_HH
