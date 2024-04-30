#include "CommonPostProcessing.hh"

#include "engine/container/Array.hxx"
#include "gfx/IGraphicLayerImplementations.hh"
#include "tool/ShaderLoader.hh"
#include "tool/ShaderHelper.hh"

using namespace Gfx;
using namespace PostProcessing;

void CommonPostProcessing::Init(IGraphicLayer* gfxLayer,
								const Geometry& quad,
								Tool::ShaderLoader* shaderLoader,
								Tool::TextureLoader* textureLoader,
								int renderWidth, int renderHeight,
								int windowWidth, int windowHeight)
{
	m_windowWidth = windowWidth;
	m_windowHeight = windowHeight;
	m_geometry = quad;

	m_gaussianBlur.Init(gfxLayer, renderWidth / 2, renderHeight / 2, quad, shaderLoader, Gfx::TextureFormat::R11G11B10f);
	m_streakBlur.Init(renderWidth / 2, renderHeight / 2, quad, shaderLoader);
	m_finalCombine.Init(renderWidth, renderHeight, quad, shaderLoader);
	m_fxaa.Init(renderWidth, renderHeight, quad, shaderLoader, textureLoader);

	LOAD_SHADER(*shaderLoader, &m_blitShader.shader, blit,
		"assets/shaders/screenspace/blitShader.vs",
		"assets/shaders/screenspace/blitShader.fs");
}

void CommonPostProcessing::Apply(long currentTime,
								 const TextureID& texture,
								 const Algebra::vector3f& bloomFilter,
								 const Algebra::vector3f& horizontalStreakPower,
								 const Algebra::vector3f& verticalStreakPower,
								 const Algebra::vector3f& streakFilter,
								 const Algebra::vector3f& lift,
								 const Algebra::vector3f& gamma,
								 const Algebra::vector3f& gain,
								 float exposure,
								 float vignetting,
								 float saturation)
{
	m_gaussianBlur.Apply(texture, 4, 4);
	m_streakBlur.Apply(texture, horizontalStreakPower, verticalStreakPower);
	m_finalCombine.Apply(currentTime,
						 texture,
						 m_gaussianBlur.ResultTexture(),
						 m_streakBlur.HorizontalResultTexture(),
						 m_streakBlur.VerticalResultTexture(),
						 bloomFilter, streakFilter,
						 lift, gamma, gain,
						 exposure,
						 vignetting,
						 saturation);
	m_fxaa.Apply(currentTime,
				 m_finalCombine.ResultTexture());

	DrawArea directToScreen = { { -1 }, { 0, 0, m_windowWidth, m_windowHeight } };

	m_blitShader.uniforms.empty();
	Tool::ShaderHelper::ExposeFullScreenBlitTransform(m_blitShader.uniforms);
	m_blitShader.uniforms.add(Uniform::Sampler1(U_TEXTURE0, m_fxaa.ResultTexture()));

	m_gfxLayer->Draw(directToScreen, RasterTests::NoDepthTest, m_geometry, m_blitShader);
}
