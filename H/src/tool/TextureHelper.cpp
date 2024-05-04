#include "TextureHelper.hh"

#include "engine/texture/Texture.hxx"
#include "gfx/IGraphicLayerImplementations.hh"

using namespace Gfx;
using namespace Tool;

TextureHelper::TextureHelper(IGraphicLayer* gfxLayer):
	m_gfxLayer(gfxLayer)
{
}

TextureID TextureHelper::CreateRenderTexture(int width, int height, TextureFormat::Enum textureFormat)
{
	TextureSampling renderTextureSampling = {
		TextureFilter::Linear,
		TextureFilter::Linear,
		1.0f,
		TextureWrap::ClampToBorder,
		TextureWrap::ClampToBorder,
		TextureWrap::ClampToBorder,
	};
	return m_gfxLayer->LoadTexture(m_gfxLayer->CreateTexture(),
								   width, height,
								   TextureType::Texture2D, textureFormat,
								   0, -1, NULL, renderTextureSampling);
}
