#ifndef MATERIAL_STUDY_HH
#define MATERIAL_STUDY_HH

#include "engine/runtime/RuntimeCompiler.hh"
#include "engine/texture/ITextureBuilder.hh"

namespace texture
{
	class Texture;
}

#ifdef ENABLE_RUNTIME_COMPILATION
struct CanteenTiles : public TInterface<runtime::IID_ITextureBuilder, texture::ITextureBuilder>
#else // !ENABLE_RUNTIME_COMPILATION
struct CanteenTiles
#endif // !ENABLE_RUNTIME_COMPILATION
{
	static const int		s_layerSizes[];
	static const int		s_numberOfLayers;
	static const float		s_parallaxScale;

#ifdef ENABLE_RUNTIME_COMPILATION
	static CanteenTiles*	instance;
	static void				BuildPipeCaller(texture::Texture& dest);
	void					BuildPipe(texture::Texture& dest);
#else // !ENABLE_RUNTIME_COMPILATION
	static	void			BuildPipe(texture::Texture& dest);
#endif // !ENABLE_RUNTIME_COMPILATION
};

#ifdef ENABLE_RUNTIME_COMPILATION
struct SandAndPebble : public TInterface<runtime::IID_ITextureBuilder, texture::ITextureBuilder>
#else // !ENABLE_RUNTIME_COMPILATION
struct SandAndPebble
#endif // !ENABLE_RUNTIME_COMPILATION
{
	static const int		s_layerSizes[];
	static const int		s_numberOfLayers;
	static const float		s_parallaxScale;

#ifdef ENABLE_RUNTIME_COMPILATION
	static SandAndPebble*	instance;
	static void				BuildPipeCaller(texture::Texture& dest);
	void					BuildPipe(texture::Texture& dest);
#else // !ENABLE_RUNTIME_COMPILATION
	static	void			BuildPipe(texture::Texture& dest);
#endif // !ENABLE_RUNTIME_COMPILATION
};

#ifdef ENABLE_RUNTIME_COMPILATION
struct ScifiMetalPlate : public TInterface<runtime::IID_ITextureBuilder, texture::ITextureBuilder>
#else // !ENABLE_RUNTIME_COMPILATION
struct ScifiMetalPlate
#endif // !ENABLE_RUNTIME_COMPILATION
{
	static const int		s_layerSizes[];
	static const int		s_numberOfLayers;
	static const float		s_parallaxScale;

#ifdef ENABLE_RUNTIME_COMPILATION
	static ScifiMetalPlate*	instance;
	static void				BuildPipeCaller(texture::Texture& dest);
	void					BuildPipe(texture::Texture& dest);
#else // !ENABLE_RUNTIME_COMPILATION
	static	void			BuildPipe(texture::Texture& dest);
#endif // !ENABLE_RUNTIME_COMPILATION
};

#ifdef ENABLE_RUNTIME_COMPILATION
struct Orichalcum : public TInterface<runtime::IID_ITextureBuilder, texture::ITextureBuilder>
#else // !ENABLE_RUNTIME_COMPILATION
struct Orichalcum
#endif // !ENABLE_RUNTIME_COMPILATION
{
	static const int		s_layerSizes[];
	static const int		s_numberOfLayers;
	static const float		s_parallaxScale;

#ifdef ENABLE_RUNTIME_COMPILATION
	static Orichalcum*		instance;
	static void				BuildPipeCaller(texture::Texture& dest);
	void					BuildPipe(texture::Texture& dest);
#else // !ENABLE_RUNTIME_COMPILATION
	static	void			BuildPipe(texture::Texture& dest);
#endif // !ENABLE_RUNTIME_COMPILATION
};

#ifdef ENABLE_RUNTIME_COMPILATION
struct Bricks : public TInterface<runtime::IID_ITextureBuilder, texture::ITextureBuilder>
#else // !ENABLE_RUNTIME_COMPILATION
struct Bricks
#endif // !ENABLE_RUNTIME_COMPILATION
{
	static const int		s_layerSizes[];
	static const int		s_numberOfLayers;
	static const float		s_parallaxScale;

#ifdef ENABLE_RUNTIME_COMPILATION
	static Bricks*			instance;
	static void				BuildPipeCaller(texture::Texture& dest);
	void					BuildPipe(texture::Texture& dest);
#else // !ENABLE_RUNTIME_COMPILATION
	static	void			BuildPipe(texture::Texture& dest);
#endif // !ENABLE_RUNTIME_COMPILATION
};

#ifdef ENABLE_RUNTIME_COMPILATION
struct GraniteCobbleStone : public TInterface<runtime::IID_ITextureBuilder, texture::ITextureBuilder>
#else // !ENABLE_RUNTIME_COMPILATION
struct GraniteCobbleStone
#endif // !ENABLE_RUNTIME_COMPILATION
{
	static const int		s_layerSizes[];
	static const int		s_numberOfLayers;
	static const float		s_parallaxScale;

#ifdef ENABLE_RUNTIME_COMPILATION
	static GraniteCobbleStone*	instance;
	static void				BuildPipeCaller(texture::Texture& dest);
	void					BuildPipe(texture::Texture& dest);
#else // !ENABLE_RUNTIME_COMPILATION
	static	void			BuildPipe(texture::Texture& dest);
#endif // !ENABLE_RUNTIME_COMPILATION
};

#endif // MATERIAL_STUDY_HH
