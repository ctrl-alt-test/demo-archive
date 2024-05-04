#include "DebugTextures.hh"

#include "engine/algebra/Functions.hh"
#include "engine/algebra/Interpolation.hxx"
#include "engine/algebra/Vector2.hxx"
#include "engine/algebra/Vector3.hxx"
#include "engine/algebra/Vector4.hxx"
#include "engine/container/Utils.hh"
#include "engine/core/msys_temp.hh"
#include "engine/noise/Hash.hh"
#include "engine/noise/LatticeNoise.hh"
#include "engine/noise/PerlinNoise.hh"
#include "engine/noise/SimplexNoise.hh"
#include "engine/noise/SimplexValueNoise.hh"
#include "engine/noise/ValueNoise.hh"
#include "engine/noise/WorleyNoise.hh"
#include "engine/texture/Texture.hh"
#include "engine/texture/Utils.hh"

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

struct thinPixelFormat
{
	Algebra::vector3f	color;
	float				opacity;
};

// ---8<---------------------------------------------------------------

#ifdef ENABLE_RUNTIME_COMPILATION
REGISTERCLASS(DebugParallaxMapping);
DebugParallaxMapping* DebugParallaxMapping::instance = NULL;

void DebugParallaxMapping::BuildPipeCaller(texture::Texture& dest)
{
	instance->BuildPipe(dest);
}
#endif // ENABLE_RUNTIME_COMPILATION

const int DebugParallaxMapping::s_layerSizes[] = { 3 + 1, 3 + 1, 3 + 1 };
const int DebugParallaxMapping::s_numberOfLayers = ARRAY_LEN(DebugParallaxMapping::s_layerSizes);
const float DebugParallaxMapping::s_parallaxScale = 0.02f;

void DebugParallaxMapping::BuildPipe(texture::Texture& dest)
{
	struct local
	{
		static void Build(void* out_pixelData, const texture::InputPixelInfo& in)
		{
			pixelFormat result;

			vector2f grid1 = smoothStep(1.f - 0.0625f, 1.f - 0.063f, abs(fract(in.uv * (in.width/64.f)) * 2.f - 1.f));
			vector2f grid2 = smoothStep(1.f - 0.25f, 1.f - 0.251f, abs(fract(in.uv * (in.width/8.f)) * 2.f - 1.f));
			vector2f grid = mul(grid1, 0.5f + 0.5f * grid2);

			vector3f c1 = { 1.00f, 0.10f, 0.01f }; c1 = pow(c1, 2.2f);
			vector3f c2 = { 1.00f, 0.51f, 0.01f }; c2 = pow(c2, 2.2f);
			vector3f c3 = { 0.04f, 0.78f, 0.98f }; c3 = pow(c3, 2.2f);
			vector3f c4 = { 0.01f, 0.99f, 0.22f }; c4 = pow(c4, 2.2f);

			vector2f colorMap = fract(floor(in.uv * (in.width / 32.f)) * (2.f * 32.f / in.width));
			vector3f color = mix(mix(c1, c2, colorMap.x), mix(c3, c4, colorMap.x), colorMap.y);
			color = mix(color, vec3(1.f), 0.5f * float(in.uv.x > 0.5f == in.uv.y > 0.5f));
			color = mix(color, vec3(0.f), 0.5f * float(fract(8.f * in.uv.x) > 0.5f == fract(8.f * in.uv.y) > 0.5f));

			vector2f p = 2.f * abs(fract(4.f * in.uv) - 0.5f);
			float h = 0.f;

			if (in.uv.x > 0.5f && in.uv.y > 0.5f)
			{
				float cones = clamp(1.f - 2.f * norm(p), 0.f, 1.f);
				h = cones;
			}
			else if (in.uv.x > 0.5f && in.uv.y <= 0.5f)
			{
				float hemispheres = msys_sqrtf(1.f - clamp(4.f * (p.x * p.x + p.y * p.y), 0.f, 1.f));
				h = hemispheres;
			}
			else if (in.uv.x <= 0.5f && in.uv.y > 0.5f)
			{
				float pyramids = clamp(1.f - 2.f * msys_max(p.x, p.y), 0.f, 1.f);
				h = pyramids;
			}
			else
			{
				float hemicubes = float(1.f - 2.f * msys_max(p.x, p.y) > 0.f);
				h = hemicubes;
			}

			result.diffuseColor = grid.x * grid.y * color;
			result.opacity = 1.f;
			result.specularColor = vec3(0.02f);
			result.roughness = mix(0.2f, 0.6f, float(fract(in.uv.x * 16.f) > 0.5f == fract(in.uv.y * 16.f) > 0.5f));
			result.height = h;
			*(pixelFormat*)out_pixelData = result;
		}
	};
	dest.ApplyFunction(local::Build);

	const size_t heightOffset = (float*)&(((pixelFormat*)NULL)->height) - (float*)NULL;
	const int normalOffset = (float*)&(((pixelFormat*)NULL)->normal) - (float*)NULL;
	texture::GenerateNormalMap(dest, heightOffset, normalOffset, s_parallaxScale);
}

// ---8<---------------------------------------------------------------

#ifdef ENABLE_RUNTIME_COMPILATION
REGISTERCLASS(DebugNoise);
DebugNoise* DebugNoise::instance = NULL;
void DebugNoise::BuildCaller(void* out_pixelData, const void* inputPixelData,
							int width, int height, int i, int j, const Algebra::vector2f& coord)
{
	return instance->Build(out_pixelData, inputPixelData, width, height, i, j, coord);
}
#endif // ENABLE_RUNTIME_COMPILATION

const int DebugNoise::s_layerSizes[] = { 4 };
const int DebugNoise::s_numberOfLayers = ARRAY_LEN(DebugNoise::s_layerSizes);

void DebugNoise::Build(void* out_pixelData, const void*,
					  int width, int height, int i, int j, const Algebra::vector2f& coord)
{
	const vector2f uv = { coord.x / (float)width, coord.y / (float)height };

	thinPixelFormat result;
	result.color = vec3(0.5f);

	float band = 0.f;
	float bandWidth = 0.125f;

	// Hash.
	if (uv.y >= band && uv.y < band + bandWidth)
	{
		float y = interpolate(band, band + bandWidth, uv.y);

		unsigned int hash = (y < 0.5f ?
			Noise::Hash::get32(uv.x) :
			Noise::Hash::get32(uv));
		float uvHash = float(hash) / float(0xffffffff);
		result.color = vec3(uvHash);
	}
	band += bandWidth;

	// Lattice noise.
	if (uv.y >= band && uv.y < band + bandWidth)
	{
		int n = (interpolate(band, band + bandWidth, uv.y) < 0.5f ? 1 : 5);
		float x = 0.f;
		float sum = 0.f;
		float frequency = 20.f;
		for (int k = 0; k < n; ++k)
		{
			x = 2.f * x + Noise::LatticeNoise::value(uv * frequency, frequency / 2.f);
			sum = 2.f * sum + 1.f;
			frequency *= 2.f;
		}
		x /= sum;
		result.color = Algebra::abs(x) * (x > 0.f ? vec3(0.f, 0.7f, 0.2f) : vec3(1.f, 0.f, 0.1f));
	}
	band += bandWidth;

	// Value noise.
	if (uv.y >= band && uv.y < band + bandWidth)
	{
		int n = (interpolate(band, band + bandWidth, uv.y) < 0.5f ? 1 : 5);
		float x = 0.f;
		float sum = 0.f;
		float frequency = 20.f;
		for (int k = 0; k < n; ++k)
		{
			x = 2.f * x + Noise::ValueNoise::value(uv * frequency, frequency / 2.f);
			sum = 2.f * sum + 1.f;
			frequency *= 2.f;
		}
		x /= sum;
		result.color = Algebra::abs(x) * (x > 0.f ? vec3(0.f, 0.7f, 0.2f) : vec3(1.f, 0.f, 0.1f));
	}
	band += bandWidth;

	// Perlin noise.
	if (uv.y >= band && uv.y < band + bandWidth)
	{
		int n = (interpolate(band, band + bandWidth, uv.y) < 0.5f ? 1 : 5);
		float x = 0.f;
		float sum = 0.f;
		float frequency = 20.f;
		for (int k = 0; k < n; ++k)
		{
			x = 2.f * x + Noise::PerlinNoise::value(uv * frequency, frequency / 2.f);
			sum = 2.f * sum + 1.f;
			frequency *= 2.f;
		}
		x /= sum;
		result.color = Algebra::abs(x) * (x > 0.f ? vec3(0.f, 0.7f, 0.2f) : vec3(1.f, 0.f, 0.1f));
	}
	band += bandWidth;

	// Simplex value noise.
	if (uv.y >= band && uv.y < band + bandWidth)
	{
		int n = (interpolate(band, band + bandWidth, uv.y) < 0.5f ? 1 : 5);
		float x = 0.f;
		float sum = 0.f;
		float frequency = 20.f;
		for (int k = 0; k < n; ++k)
		{
			x = 2.f * x + Noise::SimplexValueNoise::value(uv * frequency);
			sum = 2.f * sum + 1.f;
			frequency *= 2.f;
		}
		x /= sum;
		result.color = Algebra::abs(x) * (x > 0.f ? vec3(0.f, 0.7f, 0.2f) : vec3(1.f, 0.f, 0.1f));
	}
	band += bandWidth;

	// Simplex noise.
	if (uv.y >= band && uv.y < band + bandWidth)
	{
		int n = (interpolate(band, band + bandWidth, uv.y) < 0.5f ? 1 : 5);
		float x = 0.f;
		float sum = 0.f;
		float frequency = 20.f;
		for (int k = 0; k < n; ++k)
		{
			x = 2.f * x + Noise::SimplexNoise::value(uv * frequency);
			sum = 2.f * sum + 1.f;
			frequency *= 2.f;
		}
		x /= sum;
		result.color = Algebra::abs(x) * (x > 0.f ? vec3(0.f, 0.7f, 0.2f) : vec3(1.f, 0.f, 0.1f));
	}
	band += bandWidth;

	// Voronoi.
	if (uv.y >= band && uv.y < band + bandWidth)
	{
		float y = interpolate(band, band + bandWidth, uv.y);

		Noise::VoronoiInfo voronoi = Noise::WorleyNoise::value(uv * 20.f, 10.f, 1.f, 2.4f);
		float cellId = voronoi.GetHash();

		result.color = (voronoi.f2 - voronoi.f1) * mix(vec3(1.f, 0.55f, 0.f), vec3(0.f, 0.45f, 1.f), cellId);
	}
	band += bandWidth;
	
	result.opacity = 1.f;
	*(thinPixelFormat*)out_pixelData = result;
}

// ---8<---------------------------------------------------------------
