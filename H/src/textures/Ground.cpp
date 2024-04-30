#include "Ground.hh"

#include "engine/algebra/Functions.hh"
#include "engine/algebra/Interpolation.hxx"
#include "engine/algebra/Utils.hh"
#include "engine/algebra/Vector2.hxx"
#include "engine/algebra/Vector3.hxx"
#include "engine/algebra/Vector4.hxx"
#include "engine/container/Utils.hh"
#include "engine/core/msys_temp.hh"
#include "engine/texture/Texture.hh"
#include "engine/texture/Utils.hh"
#include "engine/noise/Hash.hh"
#include "engine/noise/LatticeNoise.hh"
#include "engine/noise/PerlinNoise.hh"
#include "engine/noise/Rand.hh"
#include "engine/noise/WorleyNoise.hh"
#include "engine/noise/Utils.hh"

#ifdef ENABLE_RUNTIME_COMPILATION
#include "RuntimeCompiledCPlusPlus/RuntimeObjectSystem/ObjectInterfacePerModule.h"
#endif

using namespace Algebra;

struct pixelFormat
{
	Algebra::vector3f	diffuseColor;
	float				opacity;
	Algebra::vector3f	specularColor;
	float				roughness;
	Algebra::vector3f	normal;
	float				height;
};

// ---8<---------------------------------------------------------------

#ifdef ENABLE_RUNTIME_COMPILATION
REGISTERCLASS(Ground);
Ground* Ground::instance = NULL;

void Ground::BuildPipeCaller(texture::Texture& dest)
{
	instance->BuildPipe(dest);
}
#endif // ENABLE_RUNTIME_COMPILATION

const int Ground::s_layerSizes[] = { 3 + 1, 3 + 1, 3 + 1 };
const int Ground::s_numberOfLayers = ARRAY_LEN(Ground::s_layerSizes);
const float Ground::s_parallaxScale = 0.05f;
const int* Ground_layerSizes = Ground::s_layerSizes;

#ifdef ENABLE_RUNTIME_COMPILATION
void Ground::BuildPipe(texture::Texture& dest)
#else // !ENABLE_RUNTIME_COMPILATION
void Ground_BuildPipe(texture::Texture& dest)
#endif // !ENABLE_RUNTIME_COMPILATION
{
	struct local
	{
		static void ground(void* out_pixelData, const texture::InputPixelInfo& in)
		{
			const vector3f col1 = { 0.27f, 0.21f, 0.18f };
			const vector3f col2 = { 0.48f, 0.4f, 0.29f };
			const vector3f stoneCol = { 0.4f, 0.4f, 0.4f };

			Noise::VoronoiInfo voronoi = Noise::WorleyNoise::value(in.uv * 48.f, 48.f, 1.f, 1.f);
			float perlin = Noise::PerlinNoise::value(mul(in.uv, vec2(8.f, 16.f)), 8.f) * 0.5f + 0.5f;
			perlin = Algebra::pow(clamp(perlin, 0.f, 1.f), 1.5f);

			float x = mix(voronoi.f2, perlin, 0.6f);
			float h = mix(smoothStep(0.2f, 0.5f, voronoi.f2), perlin, 0.95f);
			h = mix(h, Noise::Hash::get32f(in.uv), 0.02f);

			float stones = smoothStep(0.25f, 0.1f, voronoi.f1+perlin);
			h += stones * 3.f * perlin;

			vector3f col = mix(col1, col2, x);
			col = mix(col, stoneCol, stones);

			pixelFormat result;
			result.height = h;
			result.diffuseColor = col;
			result.specularColor = vec3(0.04f);
			result.opacity = 1.f;
			result.roughness = 1.f - 0.6f * stones;
			*(static_cast<pixelFormat*>(out_pixelData)) = result;
		}
	};

	dest.ApplyFunction(local::ground);

	const size_t heightOffset = (float*)&(((pixelFormat*)NULL)->height) - (float*)NULL;
	const int normalOffset = (float*)&(((pixelFormat*)NULL)->normal) - (float*)NULL;
	texture::GenerateNormalMap(dest, heightOffset, normalOffset, Ground::s_parallaxScale);
}
