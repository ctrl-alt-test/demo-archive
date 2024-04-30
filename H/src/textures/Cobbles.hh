#pragma once

#include "engine/runtime/RuntimeCompiler.hh"
#include "engine/texture/ITextureBuilder.hh"

namespace texture
{
	class Texture;
}

#ifdef ENABLE_RUNTIME_COMPILATION
struct Cobbles : public TInterface<runtime::IID_ITextureBuilder, texture::ITextureBuilder>
#else // !ENABLE_RUNTIME_COMPILATION
struct Cobbles
#endif // !ENABLE_RUNTIME_COMPILATION
{
	static const int		s_layerSizes[];
	static const int		s_numberOfLayers;
	static const float		s_parallaxScale;

#ifdef ENABLE_RUNTIME_COMPILATION
	static Cobbles*			instance;
	static void				BuildPipeCaller(texture::Texture& dest);
	void					BuildPipe(texture::Texture& dest);
#endif // ENABLE_RUNTIME_COMPILATION
};
#ifndef ENABLE_RUNTIME_COMPILATION
void						Cobbles_BuildPipe(texture::Texture& dest);
#endif // !ENABLE_RUNTIME_COMPILATION
