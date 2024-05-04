#include "RandomTexture.hh"

#include "engine/algebra/Vector2.hxx"
#include "engine/container/Utils.hh"
#include "engine/texture/Texture.hh"
#include "engine/noise/Hash.hh"
#include "engine/noise/PerlinNoise.hh"

#ifdef ENABLE_RUNTIME_COMPILATION
#include "RuntimeCompiledCPlusPlus/RuntimeObjectSystem/ObjectInterfacePerModule.h"
#endif

using namespace Algebra;

struct NoisePixelFormat
{
	float	whiteNoise1;
	float	whiteNoise2;
	float	perlinNoise;
	float	blueNoise;
};

#ifdef ENABLE_RUNTIME_COMPILATION
REGISTERCLASS(RandomTexture);
RandomTexture* RandomTexture::instance = NULL;

void RandomTexture::BuildCaller(void* out_pixelData, const texture::InputPixelInfo& in)
{
	return instance->Build(out_pixelData, in);
}
#endif // ENABLE_RUNTIME_COMPILATION

const int RandomTexture::s_layerSizes[] = { 4 };
const int RandomTexture::s_numberOfLayers = ARRAY_LEN(RandomTexture::s_layerSizes);
const int* RandomTexture_layerSizes = RandomTexture::s_layerSizes;

#ifdef ENABLE_RUNTIME_COMPILATION
void RandomTexture::Build(void* out_pixelData, const texture::InputPixelInfo& in)
#else // !ENABLE_RUNTIME_COMPILATION
void RandomTexture_Builder(void* out_pixelData, const texture::InputPixelInfo& in)
#endif // !ENABLE_RUNTIME_COMPILATION
{
	struct blob
	{
		int i;
		int j;
		int k;
	};
	blob seed = { in.i, in.j, 0 };

	// https://en.wikipedia.org/wiki/Central_limit_theorem
	// In probability theory, the central limit theorem (CLT)
	// establishes that, for the most commonly studied scenarios, when
	// independent random variables are added, their sum tends toward
	// a normal distribution.

	NoisePixelFormat result = { 0, 0, 0, 0 };
	for (int k = 0; k < 16; ++k)
	{
		// SIZE-CODING: The = for white and += for blue is not a
		// mistake. whiteNoise* only needs to be set once, but it
		// generates shorter code to put it inside the loop.

		seed.k = k;
		result.whiteNoise1 = Noise::Hash::get32f(seed);
		result.blueNoise += Noise::Hash::get32f(seed);

		seed.k = -k;
		result.whiteNoise2 = Noise::Hash::get32f(seed);
	}
	result.blueNoise /= 16.f;

	// The texture will be 256x256, so with f=8 we'll have a resolution
	// of 32 samples per period of Perlin noise.
	result.perlinNoise = Noise::PerlinNoise::value(8.f * in.uv, 8.f) * 0.5f + 0.5f;

	*(static_cast<NoisePixelFormat*>(out_pixelData)) = result;
}
