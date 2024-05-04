#include "FXAA.hh"

#include "engine/algebra/Matrix.hh"
#include "engine/container/Array.hxx"
#include "engine/container/Utils.hh"
#include "gfx/IGraphicLayerImplementations.hh"
#include "gfx/Uniform.hxx"
#include "textures/RandomTexture.hh"
#include "tool/ShaderHelper.hh"
#include "tool/ShaderLoader.hh"
#include "tool/TextureHelper.hh"
#include "tool/TextureLoader.hh"
#include <cassert>
#include <cstring>

using namespace Gfx;
using namespace PostProcessing;
using namespace Tool;

void FXAA::Init(int bufferWidth, int bufferHeight,
				const Geometry& quad,
				ShaderLoader* shaderLoader,
				TextureLoader* textureLoader)
{
	m_bufferWidth = bufferWidth;
	m_bufferHeight = bufferHeight;
	m_geometry = quad;

	LOAD_TEXTURE_SIMPLE(*textureLoader, m_randomTexture, RandomTexture, 256, 256);

	LOAD_SHADER(*shaderLoader, &m_shading.shader, fxaa,
		"assets/shaders/screenspace/blitShader.vs",
		"assets/shaders/screenspace/antialiasPass.fs");

	// Frame buffer
	m_FBTextures[0] = TextureHelper(m_gfxLayer).CreateRenderTexture(m_bufferWidth, m_bufferHeight, TextureFormat::RGBA8);
	m_FB = m_gfxLayer->CreateFrameBuffer(m_FBTextures, ARRAY_LEN(m_FBTextures));
}

void FXAA::Apply(long currentTime,
				 const TextureID& inputImage)
{
	DrawArea target = { m_FB, { 0, 0, m_bufferWidth, m_bufferHeight } };

	m_shading.uniforms.empty();
	ShaderHelper::ExposeFullScreenBlitTransform(m_shading.uniforms);
	ShaderHelper::ExposeResolutionInfo(m_shading.uniforms, m_bufferWidth, m_bufferHeight);
	ShaderHelper::ExposeTimeInfo(m_shading.uniforms, currentTime);
	m_shading.uniforms.add(Uniform::Sampler1(U_RANDOMTEXTURE, m_randomTexture[0]));
	m_shading.uniforms.add(Uniform::Sampler1(U_TEXTURE0, inputImage));

	m_gfxLayer->Draw(target, RasterTests::NoDepthTest, m_geometry, m_shading);
}
