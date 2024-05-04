#include "Cobbles.hh"

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
REGISTERCLASS(Cobbles);
Cobbles* Cobbles::instance = NULL;

void Cobbles::BuildPipeCaller(texture::Texture& dest)
{
	instance->BuildPipe(dest);
}
#endif // ENABLE_RUNTIME_COMPILATION

const int Cobbles::s_layerSizes[] = { 3 + 1, 3 + 1, 3 + 1, 3 + 1 };
const int Cobbles::s_numberOfLayers = ARRAY_LEN(Cobbles::s_layerSizes);
const float Cobbles::s_parallaxScale = 0.01f;
const int* Cobbles_layerSizes = Cobbles::s_layerSizes;

#ifdef ENABLE_RUNTIME_COMPILATION
void Cobbles::BuildPipe(texture::Texture& dest)
#else // !ENABLE_RUNTIME_COMPILATION
void Cobbles_BuildPipe(texture::Texture& dest)
#endif // !ENABLE_RUNTIME_COMPILATION
{
	struct local
	{
		static void cobbles(void* out_pixelData, const texture::InputPixelInfo& in)
		{
			const vector3f col1 = { 0.4f, 0.4f, 0.4f };
			const vector3f col2 = { 0.25f, 0.23f, 0.24f };

			Noise::VoronoiInfo voronoi = Noise::WorleyNoise::value(in.uv * 32.f, 32.f, 0.5f, 1.f);
			float cellHash = voronoi.GetHash();

			float fbm = Noise::fBmWorley(in.uv + cellHash, 40.f, 4, 2.f, 1.f / 0.35f, -0.5f);
			float shape = clamp(1.1f - (voronoi.f2 - voronoi.f1), 0.f, 1.f);
			shape *= shape;
			shape *= shape;
			shape *= shape;
			shape = mix(mix(1.f - shape, fbm, 0.75f), cellHash, 0.3f);

			float perlin = Noise::PerlinNoise::value(in.uv * 128.f, 128.f) * 0.5f + 0.5f;

			float v = mix(voronoi.edge, perlin, 0.15f);
			float isCobble = smoothStep(0.f, 0.2f, v);

			float jointHeight = mix(0.f, 0.2f, perlin);
			float h = mix(jointHeight, shape, isCobble);

			vector3f cobbleCol = mix(col1, col2, mix(cellHash * cellHash, perlin, 0.2f));
			vector3f col = cobbleCol;

			const Noise::VoronoiInfo voronoiHF = Noise::WorleyNoise::value(in.uv * 300.f, 300.f, 1.f, 1.f);

			pixelFormat result;
			result.height = h;
			result.diffuseColor = col;
			result.specularColor = vec3(0.04f);
			result.opacity = 1.f;
			result.roughness = mix(1.f, mix(0.8f, 0.95f, voronoiHF.GetHash()), isCobble);
			*(static_cast<pixelFormat*>(out_pixelData)) = result;
		}
	};

	dest.ApplyFunction(local::cobbles);

	const size_t heightOffset = (float*)&(((pixelFormat*)NULL)->height) - (float*)NULL;
	const int normalOffset = (float*)&(((pixelFormat*)NULL)->normal) - (float*)NULL;
	texture::GenerateNormalMap(dest, heightOffset, normalOffset, Cobbles::s_parallaxScale);
}
