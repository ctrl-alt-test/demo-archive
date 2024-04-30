#include "ToneMapping.hh"

#include "engine/algebra/Matrix.hh"
#include "engine/container/Array.hxx"
#include "engine/container/Utils.hh"
#include "gfx/IGraphicLayerImplementations.hh"
#include "tool/ShaderHelper.hh"
#include "tool/ShaderLoader.hh"
#include "tool/TextureHelper.hh"
#include <cassert>
#include <cstring>

using namespace Gfx;
using namespace PostProcessing;

ToneMapping::ToneMapping(IGraphicLayer* gfxLayer):
	m_gfxLayer(gfxLayer)
{
}

void ToneMapping::Init(int bufferWidth, int bufferHeight,
					   const Geometry& quad,
					   Tool::ShaderLoader* shaderLoader)
{
	m_bufferWidth = bufferWidth;
	m_bufferHeight = bufferHeight;
	m_geometry = quad;

	LOAD_SHADER(*shaderLoader, &m_shading.shader, toneMapping,
		"assets/shaders/screenspace/blitShader.vs",
		"assets/shaders/screenspace/toneMappingPass.fs");

	Tool::ShaderHelper::ExposeFullScreenBlitTransform(m_shading.uniforms);
	m_shading.uniforms.add(Uniform::Sampler1(U_TEXTURE0, TextureID::InvalidID));

	// Frame buffer
	m_FBTextures[0] = Tool::TextureHelper(m_gfxLayer).CreateRenderTexture(m_bufferWidth, m_bufferHeight, TextureFormat::RGBA8);
	m_FB = m_gfxLayer->CreateFrameBuffer(m_FBTextures, ARRAY_LEN(m_FBTextures));
}

void ToneMapping::Apply(const TextureID& inputImage)
{
	DrawArea target = { m_FB, { 0, 0, m_bufferWidth, m_bufferHeight } };

	assert(strcmp(m_shading.uniforms[2].name, U_TEXTURE0) == 0);
	m_shading.uniforms[2].id = inputImage;

	m_gfxLayer->Draw(target, RasterTests::NoDepthTest, m_geometry, m_shading);
}

const TextureID ToneMapping::ResultTexture() const
{
	return m_FBTextures[0];
}
