#include "MaterialStudy.hh"

#include "engine/algebra/Functions.hh"
#include "engine/algebra/Interpolation.hxx"
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

struct glowingPixelFormat
{
	Algebra::vector3f	diffuseColor;
	float				opacity;
	Algebra::vector3f	specularColor;
	float				roughness;
	Algebra::vector3f	normal;
	float				height;
	Algebra::vector3f	emissiveColor;
	float				emissiveInvIntensity;
};

// ---8<---------------------------------------------------------------

#ifdef ENABLE_RUNTIME_COMPILATION
REGISTERCLASS(CanteenTiles);
CanteenTiles* CanteenTiles::instance = NULL;

void CanteenTiles::BuildPipeCaller(texture::Texture& dest)
{
	instance->BuildPipe(dest);
}
#endif // ENABLE_RUNTIME_COMPILATION

const int CanteenTiles::s_layerSizes[] = { 3 + 1, 3 + 1, 3 + 1 };
const int CanteenTiles::s_numberOfLayers = ARRAY_LEN(CanteenTiles::s_layerSizes);
const float CanteenTiles::s_parallaxScale = 0.02f;

void CanteenTiles::BuildPipe(texture::Texture& dest)
{
	struct local
	{
		static void Build(void* out_pixelData, const texture::InputPixelInfo& in)
		{
#define NUM_A_TILES 12
#define NUM_B_TILES 10
#define NUM_C_TILES 4

			pixelFormat result;
			result.opacity = 1.f;
			result.specularColor = vec3(0.02f);

			Noise::Rand rand(Noise::Hash::get8(in.i, in.j));

			vector3f greyTile = vec3(0.3f, 0.3f, 0.3f);
			vector3f whiteTile = vec3(0.65f, 0.65f, 0.65f);
			vector3f blueTile = vec3(0.19f, 0.32f, 0.58f);
			vector3f groutColor = mix(vec3(0.1f, 0.11f, 0.1f), vec3(0.1f), Algebra::pow(rand.fgen(), 3.f));
			float pattern = smoothStep(0.0f, 0.3f, mix(Noise::PerlinNoise::value(in.uv * 50.f, 50.f), Noise::PerlinNoise::value(in.uv * 300.f, 300.f), 0.75f) + 0.5f * rand.sfgen());
			vector3f patternTile = mix(whiteTile, greyTile, pattern);

			float jMask;

			vector2f aUV = float(NUM_A_TILES) * in.uv;
			float aTileID = Noise::LatticeNoise::value(aUV, float(NUM_A_TILES)) * 0.5f + 0.5f;
			vector3f aColor = patternTile;
			if (aTileID < 0.15f) aColor = whiteTile;
			if (aTileID > 0.85f) aColor = blueTile;

			float ajThick = 0.05f;
			vector2f jaMask = smoothStep(ajThick - 0.002f, ajThick + 0.002f, abs(2.f * fract(aUV + 0.5f) - 1.f));
			//vector2f abMask2 =
			//	mul(smoothStep(1.0f, 1.04f, float(NUM_A_TILES) * uv),
			//		smoothStep(1.0f, 1.04f, float(NUM_A_TILES) - float(NUM_A_TILES) * uv));
			//float abMask = abMask2.x * abMask2.y;

			result.diffuseColor = aColor;
			vector2f aTileUV = (fract(aUV) - 0.5f*ajThick) / (1.f - ajThick);
			vector2f d = abs(aTileUV * 2.f - 1.f);
			float aHeight = 1.f - 0.1f * (Noise::PerlinNoise::value(in.uv * 12.f, 12.f) * 0.5f - 0.5f);
			aHeight -= Algebra::pow(1.f - clamp(10000.f * msys_min(1.f - d.x, 1.f - d.y), 0.f, 1.f), 2.f);
			result.height = aHeight;
			jMask = jaMask.x * jaMask.y;

			result.diffuseColor = mix(groutColor, result.diffuseColor, jMask);
			result.roughness = mix(0.9f, 0.4f, jMask);
			result.roughness = clamp(result.roughness + rand.fgen() * 0.2f, 0.f, 1.f);
			result.height = mix(0.5f, result.height, jMask);

			*(static_cast<pixelFormat*>(out_pixelData)) = result;
		}
	};

	dest.ApplyFunction(local::Build);

	const size_t heightOffset = (float*)&(((pixelFormat*)NULL)->height) - (float*)NULL;
	const int normalOffset = (float*)&(((pixelFormat*)NULL)->normal) - (float*)NULL;
	texture::GenerateNormalMap(dest, heightOffset, normalOffset, s_parallaxScale);
}

// ---8<---------------------------------------------------------------

#ifdef ENABLE_RUNTIME_COMPILATION
REGISTERCLASS(SandAndPebble);
SandAndPebble* SandAndPebble::instance = NULL;

void SandAndPebble::BuildPipeCaller(texture::Texture& dest)
{
	instance->BuildPipe(dest);
}
#endif // ENABLE_RUNTIME_COMPILATION

const int SandAndPebble::s_layerSizes[] = { 3 + 1, 3 + 1, 3 + 1 };
const int SandAndPebble::s_numberOfLayers = ARRAY_LEN(SandAndPebble::s_layerSizes);
const float SandAndPebble::s_parallaxScale = 0.02f;

void SandAndPebble::BuildPipe(texture::Texture& dest)
{
	struct local
	{
		static void VoronoiStones(void* out_pixelData, const texture::InputPixelInfo& in)
		{
			Noise::VoronoiInfo voronoi = Noise::WorleyNoise::value(in.uv * 10.f, 10.f, 1.f, 1.f);
			Noise::Rand rand(Noise::Hash::get8(in.i, in.j));

			const vector3f redSand = vec3(0.21f, 0.05f, 0.02f);
			const vector3f orangeSand = vec3(0.62f, 0.33f, 0.11f);
			const vector3f whiteSand = vec3(0.67f, 0.65f, 0.54f);

			float sandType = Noise::PerlinNoise::value(in.uv * 256.f, 256.f) * 0.5f + 0.5f;
			sandType *= sandType * sandType;
			const vector3f sandTint = mix(orangeSand, redSand, sandType);
			const vector3f sandColor = mix(sandTint, whiteSand, rand.fgen());
			const vector3f gravelColor = mix(vec3(0.1f), vec3(0.5f), voronoi.GetHash());

			// FIXME - SIZE: replace with call to fBmPerlin.
			float fbm = 0.5f  * Noise::PerlinNoise::value(in.uv * 16.f, 16.f) +
				0.2500f * Noise::PerlinNoise::value(in.uv * 32.f, 32.f) +
				0.1250f * Noise::PerlinNoise::value(in.uv * 64.f, 64.f) +
				0.0625f * Noise::PerlinNoise::value(in.uv * 128.f, 128.f);

			float wetSand = 0.5f + fbm*fbm*fbm;
			float gravel = voronoi.f2 - voronoi.f1;
			float isGravel = smoothStep(-0.05f, 0.05f, gravel - wetSand);
			float isGravelBorder = smoothStep(-0.4f, 0.4f, gravel - wetSand) * (1.f - isGravel);

			pixelFormat result;
			result.height = msys_max(wetSand, gravel);

			float wetArea = smoothStep(-0.1f, 0.3f, mix(Noise::PerlinNoise::value(in.uv * 3.f, 3.f), Noise::PerlinNoise::value(in.uv * 8.f, 8.f), 0.2f));
			float sandRoughness = mix(mix(0.7f, 0.8f, rand.fgen()), 0.f, wetArea);
			sandRoughness = mix(sandRoughness, 0.1f, smoothStep(0.5f, 0.4f, result.height));
			result.height -= 0.2f * isGravelBorder * isGravelBorder;
			result.height = mix(result.height, 0.4f, (1.f - isGravel) * wetArea);
			result.height = (result.height - 0.4f) / 0.6f;

			float gravelRoughness = 0.8f;

			result.diffuseColor = mix(sandColor, gravelColor, isGravel);
			result.specularColor = vec3(mix(0.04f, 0.02f, isGravel));
			result.opacity = 1.f;
			result.roughness = mix(sandRoughness, gravelRoughness, isGravel);

			*(static_cast<pixelFormat*>(out_pixelData)) = result;
		}
	};

	dest.ApplyFunction(local::VoronoiStones);

	const size_t heightOffset = (float*)&(((pixelFormat*)NULL)->height) - (float*)NULL;
	const int normalOffset = (float*)&(((pixelFormat*)NULL)->normal) - (float*)NULL;
	texture::GenerateNormalMap(dest, heightOffset, normalOffset, s_parallaxScale);
}

// ---8<---------------------------------------------------------------

#ifdef ENABLE_RUNTIME_COMPILATION
REGISTERCLASS(ScifiMetalPlate);
ScifiMetalPlate* ScifiMetalPlate::instance = NULL;

void ScifiMetalPlate::BuildPipeCaller(texture::Texture& dest)
{
	instance->BuildPipe(dest);
}
#endif // ENABLE_RUNTIME_COMPILATION

const int ScifiMetalPlate::s_layerSizes[] = { 3 + 1, 3 + 1, 3 + 1, 3 + 1 };
const int ScifiMetalPlate::s_numberOfLayers = ARRAY_LEN(ScifiMetalPlate::s_layerSizes);
const float ScifiMetalPlate::s_parallaxScale = 0.02f;

void ScifiMetalPlate::BuildPipe(texture::Texture& dest)
{
	struct local
	{
		static void Build(void* out_pixelData, const texture::InputPixelInfo& in)
		{
			Noise::VoronoiInfo voronoi1 = Noise::WorleyNoise::value(in.uv * 4.5f, 4.5f, 0.5f, 2.f);
			vector2f uvOffset = abs(vec2((voronoi1.id % 1000) / 1000.f, ((voronoi1.id / 1000) % 1000) / 1000.f));
			Noise::VoronoiInfo voronoi2 = Noise::WorleyNoise::value((in.uv + uvOffset) * 10.f, 10.f, 1.f, 0.f);
			glowingPixelFormat result;

			vector3f titaniumSpecularColor = { 0.542f, 0.497f, 0.449f };
			float fbm1 = mix(Noise::PerlinNoise::value(in.uv * 02.f, 02.f), mix(Noise::PerlinNoise::value(in.uv, 004.f), Noise::PerlinNoise::value(in.uv * 008.f, 008.f), 0.50f), 0.50f);
			float fbm2 = mix(Noise::PerlinNoise::value(in.uv * 64.f, 64.f), mix(Noise::PerlinNoise::value(in.uv, 128.f), Noise::PerlinNoise::value(in.uv * 256.f, 256.f), 0.75f), 0.75f);
			float rust = 0.f;//smoothStep(0.2f, 0.6f, mix(mix(fbm1, fbm2, 0.4f), voronoi1.f1, 0.6f));

			float shape = msys_min(
				mix(Algebra::pow(smoothStep(0.02f, 0.04f, voronoi1.f2 - voronoi1.f1), 0.2f),
				smoothStep(0.33f, 0.335f, voronoi1.f2 - voronoi1.f1), 0.1f),
				mix(0.9f, 1.f, smoothStep(0.15f, 0.16f, voronoi2.f2 - voronoi2.f1)));
			bool isMetalPlate = (shape > 0.f);
			bool isStripe = (Noise::LatticeNoise::value(in.uv * 8.f, 8.f) > 0.7f);
			float stripePattern = smoothStep(0.f, 0.01f, Algebra::abs((20.f * (in.uv.x + in.uv.y) - Algebra::floor(20.f * (in.uv.x + in.uv.y))) * 4.f - 2.f) - 1.f);
			vector3f stripeColor = mix(vec3(0.05f), vec3(0.9f, 0.8f, 0.0f), stripePattern);

			vector2f lightPattern = abs(fract(120.f * in.uv) * 2.f - 1.f);
			float lightIntensity = mix(20.f, 1.f, clamp((lightPattern.x * lightPattern.x + lightPattern.y * lightPattern.y), 0.f, 1.f));

			result.diffuseColor = (isMetalPlate ? (isStripe ? stripeColor : mix(vec3(0.01f), vec3(0.12f, 0.04f, 0.01f), rust)) : vec3(0.05f, 0.05f, 0.05f));
			result.opacity = 1.f;
			result.specularColor = mix(vec3(0.02f), titaniumSpecularColor, (isMetalPlate ? (isStripe ? 0.f : 1.f - rust) : 0.f));
			result.roughness = (isMetalPlate ? (isStripe ? 0.45f : mix(0.7f, 0.9f, smoothStep(0.5f, 0.6f, rust))) : 0.5f);
			result.height = (isMetalPlate ? 0.25f * shape + 0.2f * fbm1 + 0.005f * fbm2 : 0.1f * fbm1);
			result.emissiveColor = (isMetalPlate ? vector3f::zero : vec3(1.f, 0.02f, 0.08f) );
			result.emissiveInvIntensity = 1.f / lightIntensity;
			*(glowingPixelFormat*)out_pixelData = result;
		}
	};
	dest.ApplyFunction(local::Build);

	const size_t heightOffset = (float*)&(((glowingPixelFormat*)NULL)->height) - (float*)NULL;
	const int normalOffset = (float*)&(((glowingPixelFormat*)NULL)->normal) - (float*)NULL;
	texture::GenerateNormalMap(dest, heightOffset, normalOffset, s_parallaxScale);
}

// ---8<---------------------------------------------------------------

#ifdef ENABLE_RUNTIME_COMPILATION
REGISTERCLASS(Orichalcum);
Orichalcum* Orichalcum::instance = NULL;

void Orichalcum::BuildPipeCaller(texture::Texture& dest)
{
	instance->BuildPipe(dest);
}
#endif // ENABLE_RUNTIME_COMPILATION

const int Orichalcum::s_layerSizes[] = { 3 + 1, 3 + 1, 3 + 1, 3 + 1 };
const int Orichalcum::s_numberOfLayers = ARRAY_LEN(Orichalcum::s_layerSizes);
const float Orichalcum::s_parallaxScale = 0.02f;

void Orichalcum::BuildPipe(texture::Texture& dest)
{
	struct pattern
	{
		float		d;
		float		id;
		vector2f	uv;
	};

	struct local
	{
		static pattern interleavedTiles(vector2f p, int columns, int rows, float offset)
		{
			vector2f scale = { (float)columns, (float)rows };
			int row = msys_ifloorf(p.y * scale.y);
			float xOffset = offset * (row % 2);
			p.x += xOffset;
			p = mul(p, scale);
			vector2f tile = floor(p);

			p = fract(p);
			vector2f p2 = 1.f - abs(p * 2.f - 1.f);
			p2.x /= scale.x;
			p2.y /= scale.y;
			float d = msys_min(p2.x, p2.y);
			float id = Noise::Hash::get32f(tile);
			pattern result = { d, id, p };
			return result;
		}

		static float displaceTiles(const pattern tiles, float shape, float extrudeOrTilt, float intensity)
		{
			float tilt = (tiles.id - 0.5f) * (mix(tiles.uv.x, tiles.uv.y, 0.4f) - 0.5f);
			float extrude = (Noise::Hash::get32(tiles.id) % 1000) / 1000.f;
			return mix(shape, mix(extrude, tilt, extrudeOrTilt), intensity);
		}

		static void Build(void* out_pixelData, const texture::InputPixelInfo& in)
		{
			glowingPixelFormat result;

			pattern tiles1 = interleavedTiles(in.uv, 5, 12, 0.25f);
			pattern p = tiles1;

			vector2f pyramid = Algebra::abs(tiles1.uv * 2.f - 1.f);
			bool isOrichalcum = (pyramid.x < 0.8f && pyramid.y < 0.5f);

			// FIXME - SIZE: replace with calls to fBmPerlin.
			float fbm1 = mix(Noise::PerlinNoise::value(in.uv * 02.f, 02.f), mix(Noise::PerlinNoise::value(in.uv * 004.f, 004.f), Noise::PerlinNoise::value(in.uv * 008.f, 008.f), 0.50f), 0.50f);
			float fbm2 = mix(Noise::PerlinNoise::value(in.uv * 64.f, 64.f), mix(Noise::PerlinNoise::value(in.uv * 128.f, 128.f), Noise::PerlinNoise::value(in.uv * 256.f, 256.f), 0.75f), 0.75f);
			Noise::VoronoiInfo voronoi1 = Noise::WorleyNoise::value(in.uv * 15.f, 15.f, 1.f, 1.f);
			//Noise::VoronoiInfo voronoi2 = Noise::WorleyNoise::value(in.uv * 30.f, 30.f, 1.f, 1.f);
			//Noise::VoronoiInfo voronoi3 = Noise::WorleyNoise::value(in.uv * 60.f, 60.f, 1.f, 1.f);
			float worleyNoise = mix(voronoi1.f1, mix(voronoi1.f2, voronoi1.f3, 0.5f), 0.5f);

			// FIXME: faire le grout d'un cote, avec la meme valeur que la shape
			// et faire le sloppy grout de l'autre cote.
			float grout = mix(0.1f, 0.13f, fbm2);
			float brickSurface = mix(worleyNoise, fbm1, 0.5f);
			float brickVolume = Algebra::pow(clamp(50.f * tiles1.d - 0.1f, 0.f, 1.f), 0.15f);
			float shape = mix(brickVolume, brickSurface, 1.f);
			//float sloppyGrout = smoothStep(0.f, 0.25f, fbm1);
			float isGrout = 0.f;//mix(smoothStep(0.01f, 0.0f, tiles1.d), smoothStep(0.025f, 0.01f, tiles1.d), sloppyGrout);

			result.diffuseColor = 0.25f*mix(mix(vec3(0.25f, 0.25f, 0.27f), vec3(0.3f, 0.28f, 0.26f), p.id), vec3(0.3f, 0.3f, 0.3f), clamp(20.f * isGrout, 0.f, 1.f));
			result.specularColor = vec3(0.04f);
			result.roughness = mix(mix(0.8f, 0.1f, fbm2*fbm2), 0.9f, isGrout);
			result.height = 0.9f * displaceTiles(tiles1, shape, 0.8f, 0.75f) + grout * isGrout;
			result.emissiveColor = vector3f::zero;
			result.emissiveInvIntensity = 1.f;

			if (isOrichalcum)
			{
				vector3f orichalcumColor = vec3(1.f, 0.3f, 0.2f);

				result.diffuseColor = vector3f::zero;
				result.specularColor = orichalcumColor;
				result.roughness = 0.7f;
				result.height += 0.1f;

				float borders = smoothStep(msys_max(pyramid.x * 2.f - 1.f, pyramid.y));
				float heatIntensity = mix(1.f, 0.7f, borders) * clamp(mix(fbm1, fbm2, 0.2f) * 0.5f + (in.uv.y * 2.f - 1.f), 0.f, 1.f);
				heatIntensity *= heatIntensity;
				vector3f blackBody = vec3(1.f, 0.047f, 0.0022f) * expf(heatIntensity * 4.f - 1.f);

				result.emissiveColor = clamp(heatIntensity, 0.f, 1.f) * blackBody;
				result.emissiveInvIntensity = 1.f / (heatIntensity == 0.f ? 1.f : 20.f * heatIntensity);
			}

			result.opacity = 1.f;
			*(glowingPixelFormat*)out_pixelData = result;
		}
	};
	dest.ApplyFunction(local::Build);

	const size_t heightOffset = (float*)&(((glowingPixelFormat*)NULL)->height) - (float*)NULL;
	const int normalOffset = (float*)&(((glowingPixelFormat*)NULL)->normal) - (float*)NULL;
	texture::GenerateNormalMap(dest, heightOffset, normalOffset, s_parallaxScale);
}

// ---8<---------------------------------------------------------------

#ifdef ENABLE_RUNTIME_COMPILATION
REGISTERCLASS(Bricks);
Bricks* Bricks::instance = NULL;

void Bricks::BuildPipeCaller(texture::Texture& dest)
{
	instance->BuildPipe(dest);
}
#endif // ENABLE_RUNTIME_COMPILATION

const int Bricks::s_layerSizes[] = { 3 + 1, 3 + 1, 3 + 1 };
const int Bricks::s_numberOfLayers = ARRAY_LEN(Bricks::s_layerSizes);
const float Bricks::s_parallaxScale = 0.02f;

void Bricks::BuildPipe(texture::Texture& dest)
{
	struct pattern
	{
		float		d;
		float		id;
		vector2f	uv;
	};

	struct local
	{
		static pattern interleavedTiles(vector2f p, int columns, int rows, float offset)
		{
			vector2f scale = { (float)columns, (float)rows };
			int row = msys_ifloorf(p.y * scale.y);
			float xOffset = offset * (row % 2);
			p.x += xOffset;
			p = mul(p, scale);
			vector2f tile = floor(p);

			p = fract(p);
			vector2f p2 = 1.f - abs(p * 2.f - 1.f);
			p2.x /= scale.x;
			p2.y /= scale.y;
			float d = msys_min(p2.x, p2.y);
			float id = Noise::Hash::get32f(tile);
			pattern result = { d, id, p };
			return result;
		}

		static float pairedTiles(const vector2f uv, int columns, int rows, int pairs)
		{
			int column = msys_ifloorf(columns * uv.x);
			int row = msys_ifloorf(rows * uv.y);
			vector2f uv2 = fract(mul(vec2((float)columns, (float)rows), uv));
			if (row % 2 == column % 2)
			{
				uv2 = fract(vec2(pairs * uv2.x, uv2.y));
			}
			else
			{
				uv2 = fract(vec2(uv2.x, pairs * uv2.y));
			}
			float d = msys_min(1.f - Algebra::abs(uv2.x * 2.f - 1.f), 1.f - Algebra::abs(uv2.y * 2.f - 1.f));
			return d;
		}

		static float octogonalTiles(const vector2f uv, int tiles)
		{
			vector2f uv1 = fract((float)tiles * uv);
			float d1 = msys_min(1.f - Algebra::abs(uv1.x * 2.f - 1.f), 1.f - Algebra::abs(uv1.y * 2.f - 1.f));

			vector2f uv45 = 0.5f + (float)tiles * vec2(uv.x + uv.y, 1.f + uv.x - uv.y);
			int column = msys_ifloorf(uv45.y);
			int row = msys_ifloorf(uv45.x);
			vector2f uv2 = fract(uv45) * ((float)(tiles%2 + column % 2 == row % 2) * 2.f - 1.f);
			float d2 = msys_min(1.f - Algebra::abs(uv2.x * 2.f - 1.f), 1.f - Algebra::abs(uv2.y * 2.f - 1.f)) - 0.5f;

			float isCorner = (float)(d2 > 0.f);
			float d = mix(msys_min(d1, -d2), d2, isCorner);
			return d;
		}

		static float displaceTiles(const pattern tiles, float shape, float extrudeOrTilt, float intensity)
		{
			float tilt = (tiles.id - 0.5f) * (mix(tiles.uv.x, tiles.uv.y, 0.4f) - 0.5f);
			float extrude = (Noise::Hash::get32(tiles.id) % 1000) / 1000.f;
			return mix(shape, mix(extrude, tilt, extrudeOrTilt), intensity);
		}

		static void Build(void* out_pixelData, const texture::InputPixelInfo& in)
		{
			pixelFormat result;

			pattern tiles1 = interleavedTiles(in.uv, 5, 12, 0.25f);
			//float d2 = pairedTiles(uv, 5, 5, 4);
			//float d3 = octogonalTiles(uv, 5);
			pattern p = tiles1;

			// FIXME - SIZE: replace with call to fBmPerlin.
			float fbm1 = mix(Noise::PerlinNoise::value(in.uv * 02.f, 02.f), mix(Noise::PerlinNoise::value(in.uv * 004.f, 004.f), Noise::PerlinNoise::value(in.uv * 008.f, 008.f), 0.50f), 0.50f);
			float fbm2 = mix(Noise::PerlinNoise::value(in.uv * 64.f, 64.f), mix(Noise::PerlinNoise::value(in.uv * 128.f, 128.f), Noise::PerlinNoise::value(in.uv * 256.f, 256.f), 0.75f), 0.75f);
			Noise::VoronoiInfo voronoi1 = Noise::WorleyNoise::value(in.uv * 15.f, 15.f, 1.f, 1.f);
			//Noise::VoronoiInfo voronoi2 = Noise::WorleyNoise::value(in.uv * 30.f, 30.f, 1.f, 1.f);
			//Noise::VoronoiInfo voronoi3 = Noise::WorleyNoise::value(in.uv * 60.f, 60.f, 1.f, 1.f);
			float worleyNoise = mix(voronoi1.f1, mix(voronoi1.f2, voronoi1.f3, 0.5f), 0.5f);

			// FIXME: faire le grout d'un cote, avec la meme valeur que la shape
			// et faire le sloppy grout de l'autre cote.
			float grout = mix(0.1f, 0.13f, fbm2);
			float brickSurface = mix(worleyNoise, fbm1, 0.5f);
			float brickVolume = Algebra::pow(clamp(50.f * tiles1.d - 0.1f, 0.f, 1.f), 0.15f);
			float shape = mix(brickVolume, brickSurface, 1.f);
			//float sloppyGrout = smoothStep(0.f, 0.25f, fbm1);
			float isGrout = 0.f;//mix(smoothStep(0.01f, 0.0f, tiles1.d), smoothStep(0.025f, 0.01f, tiles1.d), sloppyGrout);
			float h = 0.9f * displaceTiles(tiles1, shape, 0.8f, 0.75f) + grout * isGrout;


			result.diffuseColor = mix(mix(vec3(0.25f, 0.03f, 0.02f), vec3(0.3f, 0.15f, 0.08f), p.id), vec3(0.3f, 0.3f, 0.3f), clamp(20.f * isGrout, 0.f, 1.f));
			result.opacity = 1.f;
			result.specularColor = vec3(0.02f);
			result.roughness = mix(mix(0.8f, 0.1f, fbm2*fbm2), 0.9f, isGrout);
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
REGISTERCLASS(GraniteCobbleStone);
GraniteCobbleStone* GraniteCobbleStone::instance = NULL;

void GraniteCobbleStone::BuildPipeCaller(texture::Texture& dest)
{
	instance->BuildPipe(dest);
}
#endif // ENABLE_RUNTIME_COMPILATION

const int GraniteCobbleStone::s_layerSizes[] = { 3 + 1, 3 + 1, 3 + 1 };
const int GraniteCobbleStone::s_numberOfLayers = ARRAY_LEN(GraniteCobbleStone::s_layerSizes);
const float GraniteCobbleStone::s_parallaxScale = 0.02f;

void GraniteCobbleStone::BuildPipe(texture::Texture& dest)
{
	struct pattern
	{
		float		d;
		float		id;
		vector2f	uv;
	};

	struct local
	{
		static pattern interleavedTiles(vector2f p, int columns, int rows, float offset)
		{
			vector2f scale = { (float)columns, (float)rows };
			int row = msys_ifloorf(p.y * scale.y);
			float xOffset = offset * (row % 2);
			p.x += xOffset;
			p = mul(p, scale);
			vector2f tile = floor(p);

			p = fract(p);
			vector2f p2 = 1.f - abs(p * 2.f - 1.f);
			p2.x /= scale.x;
			p2.y /= scale.y;
			float d = msys_min(p2.x, p2.y);
			float id = Noise::Hash::get32f(tile);
			pattern result = { d, id, p };
			return result;
		}

		static float pairedTiles(const vector2f uv, int columns, int rows, int pairs)
		{
			int column = msys_ifloorf(columns * uv.x);
			int row = msys_ifloorf(rows * uv.y);
			vector2f uv2 = fract(mul(vec2((float)columns, (float)rows), uv));
			if (row % 2 == column % 2)
			{
				uv2 = fract(vec2(pairs * uv2.x, uv2.y));
			}
			else
			{
				uv2 = fract(vec2(uv2.x, pairs * uv2.y));
			}
			float d = msys_min(1.f - Algebra::abs(uv2.x * 2.f - 1.f), 1.f - Algebra::abs(uv2.y * 2.f - 1.f));
			return d;
		}

		static float octogonalTiles(const vector2f uv, int tiles)
		{
			vector2f uv1 = fract((float)tiles * uv);
			float d1 = msys_min(1.f - Algebra::abs(uv1.x * 2.f - 1.f), 1.f - Algebra::abs(uv1.y * 2.f - 1.f));

			vector2f uv45 = 0.5f + (float)tiles * vec2(uv.x + uv.y, 1.f + uv.x - uv.y);
			int column = msys_ifloorf(uv45.y);
			int row = msys_ifloorf(uv45.x);
			vector2f uv2 = fract(uv45) * ((float)(tiles%2 + column % 2 == row % 2) * 2.f - 1.f);
			float d2 = msys_min(1.f - Algebra::abs(uv2.x * 2.f - 1.f), 1.f - Algebra::abs(uv2.y * 2.f - 1.f)) - 0.5f;

			float isCorner = (float)(d2 > 0.f);
			float d = mix(msys_min(d1, -d2), d2, isCorner);
			return d;
		}

		static float displaceTiles(const pattern tiles, float shape, float extrudeOrTilt, float intensity)
		{
			float tilt = (tiles.id - 0.5f) * (mix(tiles.uv.x, tiles.uv.y, 0.4f) - 0.5f);
			float extrude = (Noise::Hash::get32(tiles.id) % 1000) / 1000.f;
			return mix(shape, mix(extrude, tilt, extrudeOrTilt), intensity);
		}

		static void Build(void* out_pixelData, const texture::InputPixelInfo& in)
		{
			float uvHash = Noise::Hash::get32f(in.uv);

			// Height.
			float h = 0.f;
			float sum = 0.f;
			float factor = 3.f;
			float frequency = 8.f;
			for (int k = 0; k < 4; ++k)
			{
				// The 2.4 for norm parameter gives interesting lines
				// going from the center to the outside.
				Noise::VoronoiInfo voronoi = Noise::WorleyNoise::value(in.uv * frequency, frequency, 1.f, 2.4f);
				float intensity = mix(0.25f, 1.f, float(voronoi.id)/float(0xffffffff));
				float detail = clamp(1.f * (voronoi.f2 - voronoi.f1), 0.f, 1.f);
				h = factor * h + detail * intensity;
				sum = factor * sum + 1.f;
				frequency *= 2.f;
			}
			h /= sum;
			float grout = mix(0.08f, 0.1f, uvHash);
			float isGrout = interpolate(-0.01f, 0.01f, grout - h);
			h = msys_max(h, grout);

			vector3f color = vec3(mix(0.2f, 0.3f, uvHash));
			float roughness = 1.f;
			float specular = 0.02f;

			// Stone color.
			if (isGrout < 1.f)
			{
				Noise::VoronoiInfo facetsVoronoi = Noise::WorleyNoise::value(in.uv * 300.f, 300.f, 1.f, 2.4f);
				float facetHash = float(facetsVoronoi.id)/float(0xffffffff);
				float shinyFacets = interpolate(0.96f, 0.997f, facetHash);
				float roughFacets = 0.3f * facetHash;

				float blackFlakes = interpolate(0.f, 0.1f, facetHash);
				float whiteFlakes = interpolate(0.1f, 0.5f, facetHash);
				float colorFlakes = interpolate(0.5f, 1.f, facetHash);
				vector3f stoneColor = (facetHash < 0.1f) ?
					mix(vec3(0.01f), vec3(0.4f), blackFlakes) :
					((facetHash < 0.5f) ?
					mix(vec3(0.6f), vec3(0.4f), whiteFlakes) :
					mix(vec3(0.4f, 0.32f, 0.3f), vec3(0.6f), colorFlakes));
				color = mix(stoneColor, color, isGrout);

				float stoneRoughness = mix(1.f, 0.4f, msys_max(roughFacets, shinyFacets));
				roughness = mix(stoneRoughness, roughness, isGrout);

				// Mica has an IoR of 1.56~1.61 -> F0 = 0.048~0.054.
				// Feldspar has an IoR of 1.52~1.53 -> 0.043.
				// Quartz has an IoR of 1.48 -> 0.037
				float stoneSpecular = mix(0.037f, 0.054f, facetHash);
				specular = mix(stoneSpecular, specular, isGrout);
			}

			pixelFormat result;
			result.diffuseColor = color;
			result.opacity = 1.f;
			result.specularColor = vec3(specular);
			result.roughness = roughness;
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
