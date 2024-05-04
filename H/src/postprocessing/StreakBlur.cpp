#include "StreakBlur.hh"

#include "engine/algebra/Functions.hh"
#include "engine/algebra/Matrix.hh"
#include "engine/container/Utils.hh"
#include "gfx/IGraphicLayerImplementations.hh"
#include "gfx/Uniform.hxx"
#include "tool/ShaderHelper.hh"
#include "tool/ShaderLoader.hh"
#include "tool/TextureHelper.hh"
#include <cassert>
#include <cstring>

using namespace Gfx;
using namespace PostProcessing;

void StreakBlur::Init(int bufferWidth, int bufferHeight,
					  const Geometry& quad,
					  Tool::ShaderLoader* shaderLoader)
{
	m_bufferWidth = bufferWidth;
	m_bufferHeight = bufferHeight;
	m_geometry = quad;

	LOAD_SHADER(*shaderLoader, &m_shading.shader, streakBlur, "assets/shaders/screenspace/blitShader.vs", "assets/shaders/screenspace/streakBlurPass.fs");

	// Make sure there are enough passes for a streak over the entire width.
	m_passes = 1;
	while (Algebra::pow(4.f, float(m_passes)) < float(m_bufferWidth))
	{
		++m_passes;
	}

	// Frame buffers
	Tool::TextureHelper textureHelper(m_gfxLayer);
	m_hFBTextures[0] = textureHelper.CreateRenderTexture(m_bufferWidth, m_bufferHeight, TextureFormat::R11G11B10f);
	m_hFBTextures[1] = textureHelper.CreateRenderTexture(m_bufferWidth, m_bufferHeight, TextureFormat::R11G11B10f);
	m_hFB = m_gfxLayer->CreateFrameBuffer(m_hFBTextures, ARRAY_LEN(m_hFBTextures));

	m_vFBTextures[0] = textureHelper.CreateRenderTexture(m_bufferWidth, m_bufferHeight, TextureFormat::R11G11B10f);
	m_vFBTextures[1] = textureHelper.CreateRenderTexture(m_bufferWidth, m_bufferHeight, TextureFormat::R11G11B10f);
	m_vFB = m_gfxLayer->CreateFrameBuffer(m_vFBTextures, ARRAY_LEN(m_vFBTextures));

	m_tmpFBTextures[0] = textureHelper.CreateRenderTexture(m_bufferWidth, m_bufferHeight, TextureFormat::R11G11B10f);
	m_tmpFBTextures[1] = textureHelper.CreateRenderTexture(m_bufferWidth, m_bufferHeight, TextureFormat::R11G11B10f);
	m_tmpFB = m_gfxLayer->CreateFrameBuffer(m_tmpFBTextures, ARRAY_LEN(m_tmpFBTextures));
}

void StreakBlur::SetShaderParameters(int pass,
									 const TextureID& texture0,
									 const TextureID& texture1,
									 const Algebra::vector3f& streakPower,
									 float directionX, float directionY)
{
	m_shading.uniforms.empty();
	Tool::ShaderHelper::ExposeFullScreenBlitTransform(m_shading.uniforms);
	m_shading.uniforms.add(Uniform::Int1(U_PASS, pass));
	m_shading.uniforms.add(Uniform::Sampler1(U_TEXTURE0, texture0));
	m_shading.uniforms.add(Uniform::Sampler1(U_TEXTURE1, texture1));
	m_shading.uniforms.add(Uniform::Float2(U_DIRECTION, directionX, directionY));
	m_shading.uniforms.add(Uniform::Float3(U_STREAKPOWER, streakPower.x, streakPower.y, streakPower.z));
}

void StreakBlur::Apply(const TextureID& inputImage,
					   const Algebra::vector3f& horizontalStreakPower,
					   const Algebra::vector3f& verticalStreakPower)
{
	DrawArea target = { { -1 }, { 0, 0, m_bufferWidth, m_bufferHeight } };

	// Horizontal passes
	for (int i = 0; i < m_passes; i += 2)
	{
		SetShaderParameters(i,
			(i == 0 ? inputImage : m_hFBTextures[0]),
			(i == 0 ? inputImage : m_hFBTextures[1]),
			horizontalStreakPower,
			1.0f / m_bufferWidth, 0.f);
		target.frameBuffer = m_tmpFB;
		m_gfxLayer->Draw(target, RasterTests::NoDepthTest, m_geometry, m_shading);

		SetShaderParameters((i + 2 >= m_passes ? -(i + 1) : i + 1),
			m_tmpFBTextures[0],
			m_tmpFBTextures[1],
			horizontalStreakPower,
			1.0f / m_bufferWidth, 0.f);
		target.frameBuffer = m_hFB;
		m_gfxLayer->Draw(target, RasterTests::NoDepthTest, m_geometry, m_shading);
	}

	// Vertical passes
	for (int i = 0; i < m_passes; i += 2)
	{
		SetShaderParameters(i,
			(i == 0 ? inputImage : m_vFBTextures[0]),
			(i == 0 ? inputImage : m_vFBTextures[1]),
			verticalStreakPower,
			0.f, 1.0f / m_bufferHeight);
		target.frameBuffer = m_tmpFB;
		m_gfxLayer->Draw(target, RasterTests::NoDepthTest, m_geometry, m_shading);

		SetShaderParameters((i + 2 >= m_passes ? -(i + 1) : i + 1),
			m_tmpFBTextures[0],
			m_tmpFBTextures[1],
			verticalStreakPower,
			0.f, 1.0f / m_bufferHeight);
		target.frameBuffer = m_vFB;
		m_gfxLayer->Draw(target, RasterTests::NoDepthTest, m_geometry, m_shading);
	}
}
