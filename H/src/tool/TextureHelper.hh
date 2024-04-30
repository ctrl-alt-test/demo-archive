#ifndef TEXTURE_HELPER_HH
#define TEXTURE_HELPER_HH

#include "gfx/ResourceID.hh"
#include "gfx/TextureFormat.hh"

namespace Gfx
{
	class IGraphicLayer;
}

namespace texture
{
	class Texture;
}

namespace Tool
{
	class TextureHelper
	{
	public:
		explicit TextureHelper(Gfx::IGraphicLayer* gfxLayer);

		Gfx::TextureID CreateRenderTexture(int width, int height, Gfx::TextureFormat::Enum textureFormat);

	private:
		Gfx::IGraphicLayer*	m_gfxLayer;
	};
}

#endif // TEXTURE_HELPER_HH
