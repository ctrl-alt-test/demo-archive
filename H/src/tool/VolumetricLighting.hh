#ifndef VOLUMETRIC_LIGHTING_HH
#define VOLUMETRIC_LIGHTING_HH

#include "engine/algebra/Vector3.hh"
#include "gfx/Geometry.hh"
#include "gfx/ShadingParameters.hh"
#include "gfx/TextureFormat.hh"
#include "postprocessing/BilateralGaussianBlur.hh"

namespace Core
{
	struct Camera;
	struct Light;
}

namespace Gfx
{
	class IGraphicLayer;
	struct DrawArea;
}

namespace Tool
{
	class ShaderHelper;
	class TextureHelper;
	class TextureLoader;
	struct FixedLightRig;
	struct ParticipatingMedium;

	class VolumetricLighting
	{
	public:
		explicit VolumetricLighting(Gfx::IGraphicLayer* gfxLayer);

		void Init(int renderWidth, int renderHeight,
				  int marchingSamples,
				  const Gfx::Geometry& quad,
				  Tool::ShaderLoader* shaderLoader,
				  Tool::TextureLoader* textureLoader,
				  Tool::TextureHelper* textureHelper);
		void Draw(long currentTime,
				  const Gfx::DrawArea& drawArea,
				  const Gfx::TextureID& depthMap,
				  const Core::Camera& camera,
				  const Tool::FixedLightRig& lightRig,
				  float zNear,
				  float zFar,
				  const ParticipatingMedium& medium,
				  float bilateralBlurSharpness,
				  int blurPasses);

	private:
		Gfx::IGraphicLayer*		m_gfxLayer;
		int						m_bufferWidth;
		int						m_bufferHeight;
		int						m_marchingSamples;
		Gfx::TextureID*			m_randomTexture;

		Gfx::Geometry			m_geometry;
		Gfx::ShadingParameters	m_mainShading;
		Gfx::ShadingParameters	m_combineShading;

		Gfx::TextureID			m_FBTexture;
		Gfx::FrameBufferID		m_FB;

		PostProcessing::BilateralGaussianBlur m_blur;
	};
}

#endif // VOLUMETRIC_LIGHTING_HH
