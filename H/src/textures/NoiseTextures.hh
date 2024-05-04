#ifndef NOISE_TEXTURES_HH
#define NOISE_TEXTURES_HH

#include "engine/algebra/Vector2.hh"
#include "engine/runtime/RuntimeCompiler.hh"
#include "engine/texture/ITextureBuilder.hh"

namespace texture
{
	struct InputPixelInfo;
}

void LatticeNoise(void* out_pixelData, const texture::InputPixelInfo& in);
void ValueNoise(void* out_pixelData, const texture::InputPixelInfo& in);
void FbmPerlinNoise(void* out_pixelData, const texture::InputPixelInfo& in);
void SimplexValueNoise(void* out_pixelData, const texture::InputPixelInfo& in);
void SimplexNoise(void* out_pixelData, const texture::InputPixelInfo& in);
void SphereNormalMap(void* out_pixelData, const texture::InputPixelInfo& in);

#endif // NOISE_TEXTURES_HH
