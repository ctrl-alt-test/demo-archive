#ifndef FIXED_LIGHT_TOOL_BOX_HH
#define FIXED_LIGHT_TOOL_BOX_HH

#include "engine/algebra/Vector3.hh"
#include "engine/container/Utils.hh"
#include "engine/core/Camera.hh"
#include "engine/core/Light.hh"
#include "engine/core/msys_temp.hh"
#include "gfx/DrawArea.hh"
#include "gfx/ResourceID.hh"
#include "postprocessing/GaussianBlur.hh"
#include "tool/ShaderHelper.hh"
#include <cassert>

namespace Tool
{
	class RenderList;
	struct FixedLightRig;

	/// <summary>
	/// A trivial POD structure to define the rendering objects
	/// necessary for a fixed number of shadow casting lights.
	/// </summary>
	struct FixedLightToolBox
	{
		int								shadowResolution;

		Gfx::TextureID					VSMTempTexture;
		Gfx::TextureID					shadowVarianceTextures[NUM_LIGHTS];
		Gfx::TextureID					shadowDepthTextures[NUM_LIGHTS];
		Gfx::FrameBufferID				shadowFBs[NUM_LIGHTS];
		PostProcessing::GaussianBlur	VSMgaussianBlurs[NUM_LIGHTS];

		void			Init(Gfx::IGraphicLayer* gfxLayer,
							 Tool::ShaderLoader* shaderLoader,
							 const Gfx::Geometry& quad,
							 int shadowTextureWidth);

		void			RenderShadowMaps(RenderList& renderList,
										 long currentStoryTime,
										 FixedLightRig& lightRig,
										 const float* zNears,
										 const float* zFars,
										 int blurPasses);
	};

	struct FixedLightRig
	{
		Algebra::vector3f				ambientLight;
		Core::Light						lights[NUM_LIGHTS];
		Core::Camera					lightCameras[NUM_LIGHTS];
		Gfx::TextureID*					shadowMaps;

		void SetLighting(const Core::RigidTransform* lightTransforms,
						 const float* lightCones,
						 const int* lightTypes,
						 const Algebra::vector3f* lightColors);
	};
}

#endif // FIXED_LIGHT_TOOL_BOX_HH
