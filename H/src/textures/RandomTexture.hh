#pragma once

#include "engine/runtime/RuntimeCompiler.hh"
#include "engine/texture/ITextureBuilder.hh"

namespace texture
{
	class Texture;
	struct InputPixelInfo;
}

#ifdef ENABLE_RUNTIME_COMPILATION
struct RandomTexture : public TInterface<runtime::IID_ITextureBuilder, texture::ITextureBuilder>
#else // !ENABLE_RUNTIME_COMPILATION
struct RandomTexture
#endif // !ENABLE_RUNTIME_COMPILATION
{
	static const int		s_layerSizes[];
	static const int		s_numberOfLayers;

#ifdef ENABLE_RUNTIME_COMPILATION
	static RandomTexture*	instance;
	static void				BuildCaller(void* out_pixelData, const texture::InputPixelInfo& in);
	void					Build(void* out_pixelData, const texture::InputPixelInfo& in);
#endif // ENABLE_RUNTIME_COMPILATION
};
#ifndef ENABLE_RUNTIME_COMPILATION
void						RandomTexture_Builder(void* out_pixelData, const texture::InputPixelInfo& in);
#endif // !ENABLE_RUNTIME_COMPILATION
