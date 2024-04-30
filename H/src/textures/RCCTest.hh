#ifndef RCC_TEST_HH
#define RCC_TEST_HH

#include "engine/algebra/Vector2.hh"
#include "engine/runtime/RuntimeCompiler.hh"
#include "engine/texture/ITextureBuilder.hh"

namespace texture
{
	struct InputPixelInfo;
}

#ifdef ENABLE_RUNTIME_COMPILATION
struct SomeTexture : public TInterface<runtime::IID_ITextureBuilder, texture::ITextureBuilder>
#else // !ENABLE_RUNTIME_COMPILATION
struct SomeTexture
#endif // !ENABLE_RUNTIME_COMPILATION
{
	static const int	s_layerSizes[];
	static const int	s_numberOfLayers;
	static const float	s_parallaxScale;

#ifdef ENABLE_RUNTIME_COMPILATION
	static SomeTexture*	instance;
	static void			BuildCaller(void* out_pixelData, const texture::InputPixelInfo& in);

	void				Build(void* out_pixelData, const texture::InputPixelInfo& in);
#else // !ENABLE_RUNTIME_COMPILATION
	static
	void				Build(void* out_pixelData, const texture::InputPixelInfo& in);
#endif // !ENABLE_RUNTIME_COMPILATION
};

#endif // RCC_TEST_HH
