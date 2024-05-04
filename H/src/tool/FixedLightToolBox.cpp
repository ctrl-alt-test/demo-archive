#include "FixedLightToolBox.hh"

#include "engine/algebra/Vector3.hxx"
#include "engine/algebra/Utils.hh"
#include "engine/container/Utils.hh"
#include "gfx/IGraphicLayerImplementations.hh"
#include "tool/ParticipatingMedium.hh"
#include "tool/RenderList.hh"
#include "tool/TextureHelper.hh"

using namespace Tool;

void FixedLightToolBox::Init(Gfx::IGraphicLayer* gfxLayer,
							 Tool::ShaderLoader* shaderLoader,
							 const Gfx::Geometry& quad,
							 int _shadowResolution)
{
	shadowResolution = _shadowResolution;
	Tool::TextureHelper textureHelper(gfxLayer);

	VSMTempTexture = textureHelper.CreateRenderTexture(shadowResolution, shadowResolution, Gfx::TextureFormat::RG32f);
	for (int i = 0; i < NUM_LIGHTS; ++i)
	{
		shadowVarianceTextures[i] = textureHelper.CreateRenderTexture(shadowResolution, shadowResolution, Gfx::TextureFormat::RG32f);
		shadowDepthTextures[i] = textureHelper.CreateRenderTexture(shadowResolution, shadowResolution, Gfx::TextureFormat::Depth);
		const Gfx::TextureID shadowTextures[] = { shadowVarianceTextures[i], shadowDepthTextures[i] };
		shadowFBs[i] = gfxLayer->CreateFrameBuffer(shadowTextures, ARRAY_LEN(shadowTextures));
		VSMgaussianBlurs[i].Init(gfxLayer, shadowResolution, shadowResolution, quad, shaderLoader, Gfx::TextureFormat::RG32f, shadowVarianceTextures[i], VSMTempTexture);
	}
}

void FixedLightToolBox::RenderShadowMaps(RenderList& renderList,
										 long currentStoryTime,
										 FixedLightRig& lightRig,
										 const float* zNears,
										 const float* zFars,
										 int blurPasses)
{
	for (int i = 0; i < NUM_LIGHTS; ++i)
	{
		lightRig.lightCameras[i] = lightRig.lights[i].CreateLightCamera(zNears[i], zFars[i]);

		const Gfx::DrawArea shadowFB = { shadowFBs[i], { 0, 0, shadowResolution, shadowResolution } };
		renderList.gfxLayer->Clear(shadowFB.frameBuffer, 1.f, 1.f, 0.f, true);

		if (lightRig.lights[i].color != Algebra::vector3f::zero &&
			lightRig.lights[i].coneAngle < PI)
		{
			Tool::ParticipatingMedium dummyMedium;
			Gfx::RasterTests lightDepthTest(Gfx::FaceCulling::None, Gfx::DepthFunction::Less, true);
			renderList.Render(currentStoryTime, shadowFB, lightDepthTest, lightRig.lightCameras[i], nullptr, lightRig, dummyMedium, true);
			VSMgaussianBlurs[i].Apply(shadowVarianceTextures[i], blurPasses, blurPasses);
		}
	}
}

void FixedLightRig::SetLighting(const Core::RigidTransform* lightTransforms,
								const float* lightCones,
								const int* lightTypes,
								const Algebra::vector3f* lightColors)
{
	for (unsigned int i = 0; i < ARRAY_LEN(lights); ++i)
	{
		Core::Light& light = lights[i];
		light.transform = lightTransforms[i];
		light.coneAngle = lightCones[i];
		light.color = lightColors[i];
		light.type = lightTypes[i];

		if (light.color != Algebra::vector3f::zero)
		{
			// Assume perfect spot light, that emits all the light in the spot direction.
			// The smaller the angle, the stronger the beam.
			const float cosAngle = msys_cosf(light.coneAngle / 2.f);
			assert(cosAngle < 1.f);
			light.color *= 2.f / (1.f - cosAngle);
		}
	}
}
