#ifndef DEBUG_TEXTURES_HH
#define DEBUG_TEXTURES_HH

#if DEBUG

#include "engine/runtime/RuntimeCompiler.hh"
#include "engine/texture/ITextureBuilder.hh"

namespace texture
{
	class Texture;
}

#ifdef ENABLE_RUNTIME_COMPILATION
struct DebugParallaxMapping : public TInterface<runtime::IID_ITextureBuilder, texture::ITextureBuilder>
#else // !ENABLE_RUNTIME_COMPILATION
struct DebugParallaxMapping
#endif // !ENABLE_RUNTIME_COMPILATION
{
	static const int		s_layerSizes[];
	static const int		s_numberOfLayers;
	static const float		s_parallaxScale;

#ifdef ENABLE_RUNTIME_COMPILATION
	static DebugParallaxMapping* instance;
	static void				BuildPipeCaller(texture::Texture& dest);
	void					BuildPipe(texture::Texture& dest);
#else // !ENABLE_RUNTIME_COMPILATION
	static	void			BuildPipe(texture::Texture& dest);
#endif // !ENABLE_RUNTIME_COMPILATION
};

#ifdef ENABLE_RUNTIME_COMPILATION
struct DebugNoise : public TInterface<runtime::IID_ITextureBuilder, texture::ITextureBuilder>
#else // !ENABLE_RUNTIME_COMPILATION
struct DebugNoise
#endif // !ENABLE_RUNTIME_COMPILATION
{
	static const int		s_layerSizes[];
	static const int		s_numberOfLayers;

#ifdef ENABLE_RUNTIME_COMPILATION
	static DebugNoise*		instance;
	static void				BuildCaller(void* out_pixelData, const void* inputPixelData,
										int width, int height, int i, int j, const Algebra::vector2f& coord);
	void					Build(void* out_pixelData, const void* inputPixelData,
								  int width, int height, int i, int j, const Algebra::vector2f& coord);
#else // !ENABLE_RUNTIME_COMPILATION
	static void				Build(void* out_pixelData, const void* inputPixelData,
								  int width, int height, int i, int j, const Algebra::vector2f& coord);
#endif // !ENABLE_RUNTIME_COMPILATION
};

#endif // DEBUG

#endif // DEBUG_TEXTURES_HH
