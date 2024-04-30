#include "VolumetricLighting.hh"

#include "engine/algebra/Matrix.hh"
#include "engine/container/Utils.hh"
#include "engine/core/Camera.hh"
#include "gfx/IGraphicLayerImplementations.hh"
#include "gfx/Uniform.hxx"
#include "textures/RandomTexture.hh"
#include "tool/FixedLightToolBox.hh"
#include "tool/ParticipatingMedium.hh"
#include "tool/ShaderHelper.hh"
#include "tool/ShaderLoader.hh"
#include "tool/TextureHelper.hh"
#include "tool/TextureLoader.hh"
#include "tweakval/tweakval.h"
#include <cassert>
#include <cstring>

using namespace Gfx;
using namespace Tool;

VolumetricLighting::VolumetricLighting(IGraphicLayer* gfxLayer):
	m_gfxLayer(gfxLayer)
{
}

void VolumetricLighting::Init(int bufferWidth, int bufferHeight,
							  int marchingSamples,
							  const Gfx::Geometry& quad,
							  Tool::ShaderLoader* shaderLoader,
							  Tool::TextureLoader* textureLoader,
							  Tool::TextureHelper* textureHelper)
{
	m_bufferWidth = bufferWidth;
	m_bufferHeight = bufferHeight;
	m_marchingSamples = marchingSamples;
	m_geometry = quad;

	LOAD_TEXTURE_SIMPLE(*textureLoader, m_randomTexture, RandomTexture, 256, 256);

	LOAD_SHADER(*shaderLoader, &m_mainShading.shader, volumetricLighting,
		"assets/shaders/screenspace/blitShader.vs",
		"assets/shaders/screenspace/volumetricLighting.fs");

	LOAD_SHADER(*shaderLoader, &m_combineShading.shader, combineVolumetricLighting,
		"assets/shaders/screenspace/blitShader.vs",
		"assets/shaders/screenspace/blitShader.fs");

	m_FBTexture = textureHelper->CreateRenderTexture(m_bufferWidth / 2, m_bufferHeight / 2, TextureFormat::R11G11B10f);
	m_FB = m_gfxLayer->CreateFrameBuffer(&m_FBTexture, 1);
	m_blur.Init(m_gfxLayer, m_bufferWidth / 2, m_bufferHeight / 2, quad, shaderLoader, TextureFormat::R11G11B10f, m_FBTexture);
}

void VolumetricLighting::Draw(long currentTime,
							  const Gfx::DrawArea& drawArea,
							  const Gfx::TextureID& depthMap,
							  const Core::Camera& camera,
							  const Tool::FixedLightRig& lightRig,
							  float zNear,
							  float zFar,
							  const ParticipatingMedium& medium,
							  float bilateralBlurSharpness,
							  int blurPasses)
{
	// Participating media
	{
		DrawArea target = { m_FB, { 0, 0, m_bufferWidth / 2, m_bufferHeight / 2 } };

		m_mainShading.uniforms.empty();
		Tool::ShaderHelper::ExposeTransformationInfoForFullScreen(m_mainShading.uniforms, camera);
		Tool::ShaderHelper::ExposeResolutionInfo(m_mainShading.uniforms, target.viewport.width, target.viewport.height);
		Tool::ShaderHelper::ExposeTimeInfo(m_mainShading.uniforms, currentTime);
		Tool::ShaderHelper::ExposeLightInfo(m_mainShading.uniforms, lightRig.ambientLight, lightRig.lights, lightRig.lightCameras, lightRig.shadowMaps, Algebra::matrix4::identity);
		Tool::ShaderHelper::ExposeFogInfo(m_mainShading.uniforms, medium.color, medium.density);
		m_mainShading.uniforms.add(Uniform::Sampler1(U_DEPTHMAP, depthMap));
		m_mainShading.uniforms.add(Uniform::Sampler1(U_RANDOMTEXTURE, m_randomTexture[0]));
		m_mainShading.uniforms.add(Uniform::Float1(U_FORWARDSCATTERINGSTRENGTH, medium.forwardScatteringStrength));
		m_mainShading.uniforms.add(Uniform::Float1(U_SCATTERINGINTENSITY, medium.scatteringIntensity));
		m_mainShading.uniforms.add(Uniform::Int1(U_MARCHINGSTEPS, m_marchingSamples));

		m_gfxLayer->Draw(target, RasterTests::NoDepthTest, m_geometry, m_mainShading);
		m_blur.Apply(m_FBTexture, depthMap, zNear, zFar, bilateralBlurSharpness, blurPasses);
	}

	// Combine back participating media into main render
	{
		m_combineShading.blendingMode = BlendingMode::Additive;
		m_combineShading.uniforms.empty();
		Tool::ShaderHelper::ExposeFullScreenBlitTransform(m_combineShading.uniforms);
		m_combineShading.uniforms.add(Uniform::Sampler1(U_TEXTURE0, m_FBTexture));

		m_gfxLayer->Draw(drawArea, RasterTests::NoDepthTest, m_geometry, m_combineShading);
	}
}
