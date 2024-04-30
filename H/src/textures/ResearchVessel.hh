#ifndef RESEARCH_VESSEL_TEXTURES_HH
#define RESEARCH_VESSEL_TEXTURES_HH

#include "engine/algebra/Vector2.hh"
#include "engine/runtime/RuntimeCompiler.hh"
#include "engine/texture/ITextureBuilder.hh"

namespace texture
{
	class Texture;
}

#ifdef ENABLE_RUNTIME_COMPILATION
struct RVHullTex : public TInterface<runtime::IID_ITextureBuilder, texture::ITextureBuilder>
#else // !ENABLE_RUNTIME_COMPILATION
struct RVHullTex
#endif // !ENABLE_RUNTIME_COMPILATION
{
	static const int		s_layerSizes[];
	static const int		s_numberOfLayers;
	static const float		s_parallaxScale;

#ifdef ENABLE_RUNTIME_COMPILATION
	static RVHullTex*		instance;
	static void				BuildPipeCaller(texture::Texture& dest);
	void					BuildPipe(texture::Texture& dest);
#endif // ENABLE_RUNTIME_COMPILATION
};
#ifndef ENABLE_RUNTIME_COMPILATION
void						RVHullTex_BuildPipe(texture::Texture& dest);
#endif // !ENABLE_RUNTIME_COMPILATION

#ifdef ENABLE_RUNTIME_COMPILATION
struct SubmersibleBodyTex : public TInterface<runtime::IID_ITextureBuilder, texture::ITextureBuilder>
#else // !ENABLE_RUNTIME_COMPILATION
struct SubmersibleBodyTex
#endif // !ENABLE_RUNTIME_COMPILATION
{
	static const int		s_layerSizes[];
	static const int		s_numberOfLayers;
	static const float		s_parallaxScale;

#ifdef ENABLE_RUNTIME_COMPILATION
	static SubmersibleBodyTex* instance;
	static void				BuildPipeCaller(texture::Texture& dest);
	void					BuildPipe(texture::Texture& dest);
#endif // ENABLE_RUNTIME_COMPILATION
};
#ifndef ENABLE_RUNTIME_COMPILATION
void						SubmersibleBodyTex_BuildPipe(texture::Texture& dest);
#endif // !ENABLE_RUNTIME_COMPILATION


#ifdef ENABLE_RUNTIME_COMPILATION
struct SubmersibleSpotTex : public TInterface<runtime::IID_ITextureBuilder, texture::ITextureBuilder>
#else // !ENABLE_RUNTIME_COMPILATION
struct SubmersibleSpotTex
#endif // !ENABLE_RUNTIME_COMPILATION
{
	static const int		s_layerSizes[];
	static const int		s_numberOfLayers;
	static const float		s_parallaxScale;

#ifdef ENABLE_RUNTIME_COMPILATION
	static SubmersibleSpotTex* instance;
	static void				BuildPipeCaller(texture::Texture& dest);
	void					BuildPipe(texture::Texture& dest);
#endif // ENABLE_RUNTIME_COMPILATION
};
#ifndef ENABLE_RUNTIME_COMPILATION
void						SubmersibleSpotTex_BuildPipe(texture::Texture& dest);
#endif // !ENABLE_RUNTIME_COMPILATION

#ifdef ENABLE_RUNTIME_COMPILATION
struct RVBridgeWindowTex : public TInterface<runtime::IID_ITextureBuilder, texture::ITextureBuilder>
#else // !ENABLE_RUNTIME_COMPILATION
struct RVBridgeWindowTex
#endif // !ENABLE_RUNTIME_COMPILATION
{
	static const int		s_layerSizes[];
	static const int		s_numberOfLayers;
	static const float		s_parallaxScale;

#ifdef ENABLE_RUNTIME_COMPILATION
	static RVBridgeWindowTex*	instance;
	static void				BuildPipeCaller(texture::Texture& dest);
	void					BuildPipe(texture::Texture& dest);
#endif // ENABLE_RUNTIME_COMPILATION
};
#ifndef ENABLE_RUNTIME_COMPILATION
void						RVBridgeWindowTex_BuildPipe(texture::Texture& dest);
#endif // !ENABLE_RUNTIME_COMPILATION

#ifdef ENABLE_RUNTIME_COMPILATION
struct RVBridgeNoWindowTex : public TInterface<runtime::IID_ITextureBuilder, texture::ITextureBuilder>
#else // !ENABLE_RUNTIME_COMPILATION
struct RVBridgeNoWindowTex
#endif // !ENABLE_RUNTIME_COMPILATION
{
	static const int		s_layerSizes[];
	static const int		s_numberOfLayers;
	static const float		s_parallaxScale;

#ifdef ENABLE_RUNTIME_COMPILATION
	static RVBridgeNoWindowTex*	instance;
	static void				BuildPipeCaller(texture::Texture& dest);
	void					BuildPipe(texture::Texture& dest);
#endif // ENABLE_RUNTIME_COMPILATION
};
#ifndef ENABLE_RUNTIME_COMPILATION
void						RVBridgeNoWindowTex_BuildPipe(texture::Texture& dest);
#endif // !ENABLE_RUNTIME_COMPILATION

#endif // RESEARCH_VESSEL_TEXTURES_HH
