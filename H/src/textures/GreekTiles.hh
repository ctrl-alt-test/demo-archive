#ifndef GREEK_TILES_HH
#define GREEK_TILES_HH

#include "engine/algebra/Vector2.hh"
#include "engine/runtime/RuntimeCompiler.hh"
#include "engine/texture/ITextureBuilder.hh"

namespace texture
{
	class Texture;
}

#ifdef ENABLE_RUNTIME_COMPILATION
struct GreekTiles : public TInterface<runtime::IID_ITextureBuilder, texture::ITextureBuilder>
#else // !ENABLE_RUNTIME_COMPILATION
struct GreekTiles
#endif // !ENABLE_RUNTIME_COMPILATION
{
	static const int	s_layerSizes[];
	static const int	s_numberOfLayers;
	static const float	s_parallaxScale;

#ifdef ENABLE_RUNTIME_COMPILATION
	static GreekTiles*	instance;
	static void			BuildPipeCaller(texture::Texture& dest);
	void				BuildPipe(texture::Texture& dest);
#else // !ENABLE_RUNTIME_COMPILATION
	static	void		BuildPipe(texture::Texture& dest);
#endif // !ENABLE_RUNTIME_COMPILATION
};

#endif // GREEK_TILES_HH
