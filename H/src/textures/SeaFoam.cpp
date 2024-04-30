#include "SeaFoam.hh"
#include "engine/algebra/Interpolation.hxx"
#include "engine/algebra/Vector2.hxx"
#include "engine/algebra/Vector3.hxx"
#include "engine/algebra/Vector4.hh"
#include "engine/container/Utils.hh"
#include "engine/noise/PerlinNoise.hh"
#include "engine/noise/WorleyNoise.hh"
#include "engine/noise/Utils.hh"
#include "engine/texture/Texture.hh"
#include "engine/texture/Utils.hh"

using namespace Algebra;

struct flowPixelFormat
{
	Algebra::vector2f	flow;
	float				height;
	float				pattern;
};

#ifdef ENABLE_RUNTIME_COMPILATION
#include "RuntimeCompiledCPlusPlus/RuntimeObjectSystem/ObjectInterfacePerModule.h"

REGISTERCLASS(SeaFoam);
SeaFoam* SeaFoam::instance = NULL;
void SeaFoam::BuildPipeCaller(texture::Texture& dest)
{
	instance->BuildPipe(dest);
}
#endif // ENABLE_RUNTIME_COMPILATION

const int SeaFoam::s_layerSizes[] = { 2 + 1 + 1, };
const int SeaFoam::s_numberOfLayers = ARRAY_LEN(SeaFoam::s_layerSizes);
const float SeaFoam::s_parallaxScale = 0.f;
const int* SeaFoam_layerSizes = SeaFoam::s_layerSizes;

#ifdef ENABLE_RUNTIME_COMPILATION
void SeaFoam::BuildPipe(texture::Texture& dest)
#else // !ENABLE_RUNTIME_COMPILATION
void SeaFoam_BuildPipe(texture::Texture& dest)
#endif // !ENABLE_RUNTIME_COMPILATION
{
	struct local
	{
		static void Build(void* out_pixelData, const texture::InputPixelInfo& in)
		{
			flowPixelFormat result;

			int octaves = 5;
			float lacunarity = 3.f;
			float invGain = 8.f;
			result.height = Noise::fBmPerlin(in.uv, 1.f, octaves, lacunarity, invGain) * 0.5f + 0.5f;
			result.pattern = 1.f - Noise::fBmWorley(in.uv, 5.f, 3, 2.f, 1.8f, 1.f);

			*(static_cast<flowPixelFormat*>(out_pixelData)) = result;
		}
	};

	dest.ApplyFunction(local::Build);

	const int heightOffset = (float*)&(((flowPixelFormat*)NULL)->height) - (float*)NULL;
	const int flowOffset = (float*)&(((flowPixelFormat*)NULL)->flow) - (float*)NULL;
	texture::GenerateFlowMap(dest, heightOffset, flowOffset, 64.f);
}
