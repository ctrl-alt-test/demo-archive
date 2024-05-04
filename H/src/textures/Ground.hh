#pragma once

#include "engine/runtime/RuntimeCompiler.hh"
#include "engine/texture/ITextureBuilder.hh"

namespace texture
{
	class Texture;
}

#ifdef ENABLE_RUNTIME_COMPILATION
struct Ground : public TInterface<runtime::IID_ITextureBuilder, texture::ITextureBuilder>
#else // !ENABLE_RUNTIME_COMPILATION
struct Ground
#endif // !ENABLE_RUNTIME_COMPILATION
{
	static const int		s_layerSizes[];
	static const int		s_numberOfLayers;
	static const float		s_parallaxScale;

#ifdef ENABLE_RUNTIME_COMPILATION
	static Ground*			instance;
	static void				BuildPipeCaller(texture::Texture& dest);
	void					BuildPipe(texture::Texture& dest);
#endif // ENABLE_RUNTIME_COMPILATION
};
#ifndef ENABLE_RUNTIME_COMPILATION
void						Ground_BuildPipe(texture::Texture& dest);
#endif // !ENABLE_RUNTIME_COMPILATION
