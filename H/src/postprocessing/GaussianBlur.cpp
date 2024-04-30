#include "GaussianBlur.hh"

#include "engine/algebra/Matrix.hh"
#include "engine/container/Utils.hh"
#include "gfx/IGraphicLayerImplementations.hh"
#include "gfx/Uniform.hxx"
#include "tool/ShaderLoader.hh"
#include "tool/ShaderHelper.hh"
#include "tool/TextureHelper.hh"
#include <cassert>
#include <cstring>

using namespace Gfx;
using namespace PostProcessing;

void GaussianBlur::Init(IGraphicLayer* gfxLayer,
						int bufferWidth, int bufferHeight,
						const Geometry& quad,
						Tool::ShaderLoader* shaderLoader,
						TextureFormat::Enum textureFormat,
						const TextureID& destTexture,
						const TextureID& tempTexture)
{
	m_gfxLayer = gfxLayer;
	m_bufferWidth = bufferWidth;
	m_bufferHeight = bufferHeight;
	m_geometry = quad;

	LOAD_SHADER(*shaderLoader, &m_shading.shader, gaussianBlur,
		"assets/shaders/screenspace/blitShader.vs",
		"assets/shaders/screenspace/gaussianBlurPass.fs");

	// Frame buffers
	Tool::TextureHelper textureHelper(m_gfxLayer);
	m_FBTextures[0] = (destTexture != TextureID::InvalidID ? destTexture :
		textureHelper.CreateRenderTexture(m_bufferWidth, m_bufferHeight, textureFormat));
	m_FB = m_gfxLayer->CreateFrameBuffer(m_FBTextures, ARRAY_LEN(m_FBTextures));

	m_tmpFBTextures[0] = (tempTexture != TextureID::InvalidID ? tempTexture :
		textureHelper.CreateRenderTexture(m_bufferWidth, m_bufferHeight, textureFormat));
	m_tmpFB = m_gfxLayer->CreateFrameBuffer(m_tmpFBTextures, ARRAY_LEN(m_tmpFBTextures));
}

void GaussianBlur::SetShaderParameters(int pass,
									   const TextureID& texture,
									   float directionX, float directionY)
{
	m_shading.uniforms.empty();
	Tool::ShaderHelper::ExposeFullScreenBlitTransform(m_shading.uniforms);
	m_shading.uniforms.add(Uniform::Int1(U_PASS, pass));
	m_shading.uniforms.add(Uniform::Sampler1(U_TEXTURE0, texture));
	m_shading.uniforms.add(Uniform::Float2(U_DIRECTION, directionX, directionY));
}

void GaussianBlur::Apply(const TextureID& inputImage, int horizontalPasses, int verticalPasses)
{
	DrawArea target = { { -1 }, { 0, 0, m_bufferWidth, m_bufferHeight } };

	// HACK: this does max(horizontalPasses, verticalPasses) passes,
	// with a scale of 0 if necessary, because it was a simpler code
	// modification than to handle odd/even cases for the ping pong.
	for (int i = 0; i < horizontalPasses || i < verticalPasses; ++i)
	{
		// Horizontal pass
		SetShaderParameters(i,
			(i == 0 ? inputImage : m_FBTextures[0]),
			int(i < horizontalPasses) * (1.0f + i) / m_bufferWidth, 0.f);
		target.frameBuffer = m_tmpFB;
		m_gfxLayer->Draw(target, RasterTests::NoDepthTest, m_geometry, m_shading);

		// Vertical pass
		SetShaderParameters(i,
			m_tmpFBTextures[0],
			0.f, int(i < verticalPasses) * (1.0f + i) / m_bufferHeight);
		target.frameBuffer = m_FB;
		m_gfxLayer->Draw(target, RasterTests::NoDepthTest, m_geometry, m_shading);
	}
}
