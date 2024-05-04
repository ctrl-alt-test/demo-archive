#include "FinalCombine.hh"

#include "engine/algebra/Matrix.hh"
#include "engine/container/Utils.hh"
#include "gfx/IGraphicLayerImplementations.hh"
#include "tool/ShaderHelper.hh"
#include "tool/ShaderLoader.hh"
#include "tool/TextureHelper.hh"
#include <cassert>
#include <cstring>

using namespace Gfx;
using namespace PostProcessing;

void FinalCombine::Init(int bufferWidth, int bufferHeight,
						const Geometry& quad,
						Tool::ShaderLoader* shaderLoader)
{
	m_bufferWidth = bufferWidth;
	m_bufferHeight = bufferHeight;
	m_geometry = quad;

	LOAD_SHADER(*shaderLoader, &m_shading.shader, finalCombine,
		"assets/shaders/screenspace/blitShader.vs",
		"assets/shaders/screenspace/finalCombinePass.fs");

	// Frame buffer
	m_FBTextures[0] = Tool::TextureHelper(m_gfxLayer).CreateRenderTexture(m_bufferWidth, m_bufferHeight, TextureFormat::RGBA8);
	m_FB = m_gfxLayer->CreateFrameBuffer(m_FBTextures, ARRAY_LEN(m_FBTextures));
}

void FinalCombine::Apply(long currentTime,
						 const TextureID& inputImage,
						 const TextureID& gaussianBlurredImage,
						 const TextureID& hStreakBlurredImage,
						 const TextureID& vStreakBlurredImage,
						 const Algebra::vector3f& bloom,
						 const Algebra::vector3f& streak,
						 const Algebra::vector3f& lift,
						 const Algebra::vector3f& gamma,
						 const Algebra::vector3f& gain,
						 float exposure,
						 float vignetting,
						 float saturation)
{
	DrawArea target = { m_FB, { 0, 0, m_bufferWidth, m_bufferHeight } };

	m_shading.uniforms.empty();
	Tool::ShaderHelper::ExposeFullScreenBlitTransform(m_shading.uniforms);
	Tool::ShaderHelper::ExposeResolutionInfo(m_shading.uniforms, m_bufferWidth, m_bufferHeight);
	Tool::ShaderHelper::ExposeTimeInfo(m_shading.uniforms, currentTime);
	m_shading.uniforms.add(Uniform::Sampler1(U_TEXTURE0, inputImage));
	m_shading.uniforms.add(Uniform::Sampler1(U_TEXTURE1, gaussianBlurredImage));
	m_shading.uniforms.add(Uniform::Sampler1(U_TEXTURE2, hStreakBlurredImage));
	m_shading.uniforms.add(Uniform::Sampler1(U_TEXTURE3, vStreakBlurredImage));

	m_shading.uniforms.add(Uniform::Float1(U_EXPOSURE, exposure));
	m_shading.uniforms.add(Uniform::Float1(U_VIGNETTING, vignetting));
	m_shading.uniforms.add(Uniform::Float1(U_SATURATION, saturation));

	m_shading.uniforms.add(Uniform::Float3(U_BLOOMCOLOR, bloom.x, bloom.y, bloom.z));
	m_shading.uniforms.add(Uniform::Float3(U_STREAKCOLOR, streak.x, streak.y, streak.z));

	m_shading.uniforms.add(Uniform::Float3(U_LIFT, lift.x, lift.y, lift.z));
	m_shading.uniforms.add(Uniform::Float3(U_GAMMA, gamma.x, gamma.y, gamma.z));
	m_shading.uniforms.add(Uniform::Float3(U_GAIN, gain.x, gain.y, gain.z));

	m_gfxLayer->Draw(target, RasterTests::NoDepthTest, m_geometry, m_shading);
}
