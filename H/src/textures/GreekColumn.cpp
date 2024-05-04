#include "GreekColumn.hh"

#include "Marble.hh"
#include "engine/algebra/Functions.hh"
#include "engine/algebra/Interpolation.hxx"
#include "engine/algebra/Utils.hh"
#include "engine/algebra/Vector2.hxx"
#include "engine/algebra/Vector3.hxx"
#include "engine/algebra/Vector4.hxx"
#include "engine/container/Utils.hh"
#include "engine/core/msys_temp.hh"
#include "engine/noise/Hash.hh"
#include "engine/noise/PerlinNoise.hh"
#include "engine/noise/WorleyNoise.hh"
#include "engine/noise/Utils.hh"
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

// ---8<---------------------------------------------------------------

#if DLL_COMPILATION
Algebra::vector3f texture::whiteMarble(const Algebra::vector2f& uv)
{
	int octaves = 6;
	float lacunarity = 2.f;
	float invGain = 2.f;
	float turbulence = 5.f;
	vector2f p = { Noise::fBmPerlin(uv, 1.f, octaves, lacunarity, invGain),                Noise::fBmPerlin(uv + vec2(3.1f, 5.7f), 1.f, octaves, lacunarity, invGain) };
	vector2f q = { Noise::fBmPerlin(uv + turbulence*p, 1.f, octaves, lacunarity, invGain), Noise::fBmPerlin(uv + turbulence*p + vec2(5.7f, 7.3f), 1.f, octaves, lacunarity, invGain) };
	vector2f r = { Noise::fBmPerlin(uv + turbulence*q, 1.f, octaves, lacunarity, invGain), Noise::fBmPerlin(uv + turbulence*q + vec2(7.3f, 11.7f), 1.f, octaves, lacunarity, invGain) };

	vector3f color = vec3(0.8f);

	float bluePatterns = clamp(2.f*norm(r), 0.f, 1.f);
	bluePatterns *= bluePatterns*bluePatterns;
	color = mix(color, vec3(0.02f, 0.1f, 0.4f), bluePatterns);

	float creamPatterns = clamp(3.f * norm(q), 0.f, 1.f);//norm(q);
	creamPatterns *= creamPatterns;
	color = mix(color, vec3(0.75f, 0.7f, 0.6f), creamPatterns);

	float whitePatterns = clamp(3.f * Algebra::abs(q.x), 0.f, 1.f);//norm(q);
	whitePatterns = interpolate(0.5f, 0.9f, whitePatterns);
	color = mix(color, vec3(0.85f), whitePatterns);

	return color;

	// Colors for a black marble with green bits:
	//0.005f + 0.5f * pow(mix(vec3(0.7f, 0.8f, 0.75f), vec3(0.8f, 0.74f, 0.7f), r.y), 200.f*clamp(r.x, 0.f, 1.f));
}
#endif // DLL_COMPILATION

// ---8<---------------------------------------------------------------

#ifdef ENABLE_RUNTIME_COMPILATION
REGISTERCLASS(DoricColumn);
DoricColumn* DoricColumn::instance = NULL;

void DoricColumn::BuildPipeCaller(texture::Texture& dest)
{
	instance->BuildPipe(dest);
}
#endif // ENABLE_RUNTIME_COMPILATION

const int DoricColumn::s_layerSizes[] = { 3 + 1, 3 + 1, 3 + 1 };
const int DoricColumn::s_numberOfLayers = ARRAY_LEN(DoricColumn::s_layerSizes);
const float DoricColumn::s_parallaxScale = 0.02f;
const int* DoricColumn_layerSizes = DoricColumn::s_layerSizes;

#define NUMBER_OF_DORIC_FLUTES 20

#ifdef ENABLE_RUNTIME_COMPILATION
void DoricColumn::BuildPipe(texture::Texture& dest)
#else // !ENABLE_RUNTIME_COMPILATION
void DoricColumn_BuildPipe(texture::Texture& dest)
#endif // !ENABLE_RUNTIME_COMPILATION
{
	struct local
	{
		static void Build(void* out_pixelData, const texture::InputPixelInfo& in)
		{
			pixelFormat result;

			vector3f color = { 0.6f, 0.6f, 0.55f };

			float fluttings = clamp(9.5f/8.25f * Algebra::abs(fract(NUMBER_OF_DORIC_FLUTES * in.uv.x) * 2.f - 1.f), 0.f, 1.f);
			fluttings = 1.f - msys_sqrtf(1.f - fluttings * fluttings);

			float base = 1.0f;
			float capital = mix(1.f, 0.9f, (fract(200.f * in.uv.y) * 2.f - 1.f) * float(in.uv.y >= 0.97f && in.uv.y <= 0.99f));

			float h = mix(base, mix(fluttings, capital, smoothStep(0.94f, 0.96f, in.uv.y)), smoothStep(0.01f, 0.02f, in.uv.y));

			result.diffuseColor = color;
			result.opacity = 1.f;
			result.specularColor = vec3(0.04f);
			result.roughness = 0.9f;
			result.height = h;
			*(pixelFormat*)out_pixelData = result;
		}
	};
	dest.ApplyFunction(local::Build);

	const size_t heightOffset = (float*)&(((pixelFormat*)NULL)->height) - (float*)NULL;
	const int normalOffset = (float*)&(((pixelFormat*)NULL)->normal) - (float*)NULL;
	texture::GenerateNormalMap(dest, heightOffset, normalOffset, DoricColumn::s_parallaxScale);
}

// ---8<---------------------------------------------------------------

#ifdef ENABLE_RUNTIME_COMPILATION
REGISTERCLASS(IonicColumn);
IonicColumn* IonicColumn::instance = NULL;

void IonicColumn::BuildPipeCaller(texture::Texture& dest)
{
	instance->BuildPipe(dest);
}
#endif // ENABLE_RUNTIME_COMPILATION

const int IonicColumn::s_layerSizes[] = { 3 + 1, 3 + 1, 3 + 1 };
const int IonicColumn::s_numberOfLayers = ARRAY_LEN(IonicColumn::s_layerSizes);
const float IonicColumn::s_parallaxScale = 0.02f;
const int* IonicColumn_layerSizes = IonicColumn::s_layerSizes;

#define NUMBER_OF_IONIC_FLUTES 24

#ifdef ENABLE_RUNTIME_COMPILATION
void IonicColumn::BuildPipe(texture::Texture& dest)
#else // !ENABLE_RUNTIME_COMPILATION
void IonicColumn_BuildPipe(texture::Texture& dest)
#endif // !ENABLE_RUNTIME_COMPILATION
{
	struct local
	{
		static void Build(void* out_pixelData, const texture::InputPixelInfo& in)
		{
			// Warning about UV coordinates:
			// Because the texture is meant for columns that are very
			// tall, we lack precision vertically. To keep precision at
			// the base, the texture is stretched differently at the
			// base than the rest of the shaft.
			// That is done in IonicColumn, in GreekColumn.cpp.
			//
			// So here we rescale the uv before doing anything else.
			vector2f uv;

			// 0.5 factor because the texture is wrapped twice.
			uv.x = 0.5f * in.uv.x;

			// Stretched to give more precision to the base.
			uv.y = (in.uv.y < 0.2f ? 0.5f * in.uv.y : mix(0.1f, 1.f, interpolate(0.2f, 1.f, in.uv.y)));

			vector3f orichalcumColor = { 1.f, 0.3f, 0.2f };

			pixelFormat result;

			float fluttings = clamp(9.5f/8.25f * Algebra::abs(fract(NUMBER_OF_IONIC_FLUTES * uv.x) * 2.f - 1.f), 0.f, 1.f);
			fluttings = 1.f - msys_sqrtf(1.f - fluttings * fluttings);

			float base = clamp(9.5f/8.25f * Algebra::abs(fract(4.f * interpolate(0.05f, 0.07f, uv.y)) * 2.f - 1.f), 0.f, 1.f);
			base = smoothStep(0.51f, 0.49f, base);
			base = mix(0.5f, 0.5f + 0.5f * clamp(1.f + msys_sinf(60.f * PI * (uv.x + uv.y)), 0.f, 1.f), base);

			float wave = 0.003f * msys_sinf(uv.x * 2.f * PI * NUMBER_OF_IONIC_FLUTES * 0.5f);
			base += smoothStep(0.037f, 0.036f, uv.y + wave) *
				smoothStep(0.025f, 0.026f, uv.y);

			float noise1 = Noise::PerlinNoise::value(mul(vec2(30.f, 15.f), uv), 10.f);
			float noise2 = Noise::PerlinNoise::value(mul(vec2(80.f, 60.f), uv), 40.f);
			noise1 = mix(noise1, noise2, 0.25f);
			noise1 = clamp(20.f * noise1 * noise1, 0.f, 1.f);
			noise2 = clamp(10.f * noise2 * noise2, 0.f, 1.f);
			float capital = mix(0.5f, 1.f, mix(noise1, noise2, 0.25f));
			capital = mix(capital, 1.f, smoothStep(0.95f, 0.92f, uv.y));
			capital = mix(capital, 1.f, smoothStep(0.975f, 0.98f, uv.y));

			float h = mix(fluttings, capital, smoothStep(0.9f, 0.95f, uv.y));
			h = mix(0.25f * base, h, smoothStep(0.095f, 0.11f, uv.y));

			float blocks = 9.f;
			float blockDistance = 1.f - Algebra::abs(fract(blocks * uv.y) * 2.f - 1.f);
			float blockId = Noise::Hash::get32f(Algebra::floor(blocks * uv.y) / blocks);
			h = mix(h, interpolate(0.f, 0.015f, blockDistance), 0.05f);

			vector3f marbleColor = texture::whiteMarble(mul(uv, vec2(1.f, 1.5f)) + blockId);
			const Noise::VoronoiInfo roughnessVoronoi = Noise::WorleyNoise::value(uv * 200.f, 100.f, 1.f, 1.f);

			float isOrichalcum = smoothStep(0.5f, 0.9f, base);
			if (uv.y > 0.096f && uv.y < 0.975f)
			{
				const Noise::VoronoiInfo patternVoronoi = Noise::WorleyNoise::value(mul(uv, 5.f * vec2(1.f, 2.8f)), 5.f, 1.f, 1.f);
				float pattern = smoothStep(0.75f, 0.85f, Algebra::abs(fract(7.f * patternVoronoi.f4) * 2.f - 1.f));
				isOrichalcum = interpolate(0.5f, 1.f, pattern);
				h += 0.05f * interpolate(0.f, 0.5f, pattern);
			}

			result.diffuseColor = mix(marbleColor, vector3f::zero, isOrichalcum);
			result.opacity = 1.f;
			result.specularColor = mix(vec3(0.04f), orichalcumColor, isOrichalcum);
			result.roughness = mix(0.6f, 0.8f, roughnessVoronoi.GetHash()) * mix(1.f, 0.8f, isOrichalcum);
			result.height = h;
			*(pixelFormat*)out_pixelData = result;
		}
	};
	dest.ApplyFunction(local::Build);

	const size_t heightOffset = (float*)&(((pixelFormat*)NULL)->height) - (float*)NULL;
	const int normalOffset = (float*)&(((pixelFormat*)NULL)->normal) - (float*)NULL;
	texture::GenerateNormalMap(dest, heightOffset, normalOffset, IonicColumn::s_parallaxScale);
}

// ---8<---------------------------------------------------------------
