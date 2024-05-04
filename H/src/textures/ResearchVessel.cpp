#include "ResearchVessel.hh"

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
#include "engine/noise/Utils.hh"
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

static float generateMetalPlatesPatterns(const vector2f& uv)
{
	vector2f bumpGrid = clamp((fract(uv) * 2.f - 1.f) * 1.1f, -1.f, 1.f);
	float plates = 1.f - (1.f - bumpGrid.x * bumpGrid.x) * (1.f - bumpGrid.y * bumpGrid.y);
	return plates;
}

//
// Cracks in the paint that have been repainted over.
//
static float generatePaintCracks(const vector2f& uv, float cracksScale, float crackLineScale, float crackLineAmplitude)
{
	float fineDetail = Noise::fBmPerlin(uv * 8.f, 8.f, 3, 2.f, 1.25f);

	Noise::VoronoiInfo voronoiSmall = Noise::WorleyNoise::value(uv * (crackLineScale * cracksScale), crackLineScale * cracksScale, 1.f, 1.f);
	float perturbationX = voronoiSmall.GetHash() * 2.f - 1.f;
	float perturbationY = Noise::Hash::get32f(voronoiSmall.id) * 2.f - 1.f;
	const vector2f perturbation = { perturbationX, perturbationY };

	Noise::VoronoiInfo voronoi = Noise::WorleyNoise::value(cracksScale * uv + crackLineAmplitude * perturbation, cracksScale, 1.f, 1.f);
	float cracks = voronoi.GetHash();
	float cracksLowLayer = float(cracks > 0.1f);
	float cracksMidLayer = float(cracks > 0.4f);

	return (cracksLowLayer + cracksMidLayer + fineDetail) / 3.f;
}

static float generateWeldPatterns(const vector2f& uv, float p)
{
	// Weld patterns.
	float weldGrid = Algebra::abs(fract(p)) * 2.f - 1.f;
	float weld = interpolate(0.95f, 1.f, weldGrid);
	weld = 1.f - (1.f - weld) * (1.f - weld);

	// Deformation due to plates welded together.
	float perlinOffset = Noise::Hash::get32f(Algebra::floor(p));
	float perlin = Noise::PerlinNoise::value(vec2(uv.x, uv.y + perlinOffset) * 2.f, 2.f) * 0.5f + 0.5f;
	weld = msys_max(perlin, weld);

	return weld;
}

// ---8<---------------------------------------------------------------

static void HullTex(void* out_pixelData, const texture::InputPixelInfo& in)
{
	float plates = generateMetalPlatesPatterns(mul(vec2(13.f, 9.f), in.uv));
	float weld = generateWeldPatterns(in.uv, 6.f * in.uv.y + 0.2f);
	float paintCracks = generatePaintCracks(in.uv, 20.f, 2.f, 0.3f);

	// Paint color.
	vector3f colorAboveWaterLine = { 0.8f, 0.1f, 0.05f };
	vector3f colorBelowWaterLine = { 0.01f, 0.01f, 0.01f };
	vector3f colorWaterLineMarks = { 1.f, 1.f, 1.f };
	float waterLineMarks =
		interpolate(0.9f, 0.91f, Algebra::abs(fract(in.uv.y * 40.f) * 2.f - 1.f)) *
		interpolate(0.011f, 0.01f, Algebra::abs(in.uv.x - 0.8f)) *
		(interpolate(0.8f, 0.81f, in.uv.y) +
		interpolate(0.06f, 0.05f, in.uv.y));
	waterLineMarks = msys_max(waterLineMarks, float(in.uv.y < 0.01f));
	vector3f paintColor = mix((in.uv.y < 0.5f) ? colorAboveWaterLine : colorBelowWaterLine, colorWaterLineMarks, waterLineMarks);

	pixelFormat result;
	result.diffuseColor = paintColor;
	result.specularColor = vec3(0.04f);
	result.opacity = 1.f;
	result.height = 0.5f * weld + 0.4f * plates + 0.1f * paintCracks;
	result.roughness = 0.55f;

	*(static_cast<pixelFormat*>(out_pixelData)) = result;
}

#ifdef ENABLE_RUNTIME_COMPILATION
REGISTERCLASS(RVHullTex);
RVHullTex* RVHullTex::instance = NULL;

void RVHullTex::BuildPipeCaller(texture::Texture& dest)
{
	instance->BuildPipe(dest);
}
#endif // ENABLE_RUNTIME_COMPILATION

const int RVHullTex::s_layerSizes[] = { 3 + 1, 3 + 1, 3 + 1 };
const int RVHullTex::s_numberOfLayers = ARRAY_LEN(RVHullTex::s_layerSizes);
const float RVHullTex::s_parallaxScale = 0.0025f;
const int* RVHullTex_layerSizes = RVHullTex::s_layerSizes;

#ifdef ENABLE_RUNTIME_COMPILATION
void RVHullTex::BuildPipe(texture::Texture& dest)
#else // !ENABLE_RUNTIME_COMPILATION
void RVHullTex_BuildPipe(texture::Texture& dest)
#endif // !ENABLE_RUNTIME_COMPILATION
{
	dest.ApplyFunction(HullTex);

	const size_t heightOffset = (float*)&(((pixelFormat*)NULL)->height) - (float*)NULL;
	const int normalOffset = (float*)&(((pixelFormat*)NULL)->normal) - (float*)NULL;
	texture::GenerateNormalMap(dest, heightOffset, normalOffset, RVHullTex::s_parallaxScale);
}

// ---8<---------------------------------------------------------------

#ifdef ENABLE_RUNTIME_COMPILATION
REGISTERCLASS(SubmersibleBodyTex);
SubmersibleBodyTex* SubmersibleBodyTex::instance = NULL;

void SubmersibleBodyTex::BuildPipeCaller(texture::Texture& dest)
{
	instance->BuildPipe(dest);
}
#endif // ENABLE_RUNTIME_COMPILATION

const int SubmersibleBodyTex::s_layerSizes[] = { 3 + 1, 3 + 1, 3 + 1 };
const int SubmersibleBodyTex::s_numberOfLayers = ARRAY_LEN(RVHullTex::s_layerSizes);
const float SubmersibleBodyTex::s_parallaxScale = 0.01f;
const int* SubmersibleBodyTex_layerSizes = SubmersibleBodyTex::s_layerSizes;

#ifdef ENABLE_RUNTIME_COMPILATION
void SubmersibleBodyTex::BuildPipe(texture::Texture& dest)
#else // !ENABLE_RUNTIME_COMPILATION
void SubmersibleBodyTex_BuildPipe(texture::Texture& dest)
#endif // !ENABLE_RUNTIME_COMPILATION
{
	struct local
	{
		static void Build(void* out_pixelData, const texture::InputPixelInfo& in)
		{
			float plates = generateMetalPlatesPatterns(mul(vec2(5.f, 7.f), in.uv));
			float weld = generateWeldPatterns(in.uv, 4.f * in.uv.y + 0.2f);

			float pattern = 3.f - in.uv.x + 0.6f * fract(in.uv.y + 0.5f);
			float isBluePaint = smoothStep(0.07f, 0.065f, 1.f - 2.f * Algebra::abs(in.uv.y - 0.5f));
			float paintCracks = generatePaintCracks(in.uv, 30.f, 5.f, 0.1f);
			isBluePaint *= msys_max(smoothStep(0.86f, 0.85f, Algebra::abs(fract(pattern) * 2.f - 1.f)), smoothStep(0.32f, 0.34f, Algebra::abs(fract(40.f * pattern) * 2.f - 1.f)));

			vector3f bluePaintColor = { 0.f, 0.015f, 0.25f };
			vector3f paintColor = { 0.9f, 0.9f, 0.05f };
			paintColor = mix(paintColor, bluePaintColor, isBluePaint);

			pixelFormat result;
			result.diffuseColor = paintColor;
			result.specularColor = vec3(0.04f);
			result.opacity = 1.f;
			result.height = 0.5f * weld + 0.4f * plates + paintCracks * 0.1f;
			result.roughness = mix(0.6f, 0.5f, isBluePaint);

			*(static_cast<pixelFormat*>(out_pixelData)) = result;
		}
	};

	dest.ApplyFunction(local::Build);

	const size_t heightOffset = (float*)&(((pixelFormat*)NULL)->height) - (float*)NULL;
	const int normalOffset = (float*)&(((pixelFormat*)NULL)->normal) - (float*)NULL;
	texture::GenerateNormalMap(dest, heightOffset, normalOffset, SubmersibleBodyTex::s_parallaxScale);
}

// ---8<---------------------------------------------------------------

#ifdef ENABLE_RUNTIME_COMPILATION
REGISTERCLASS(SubmersibleSpotTex);
SubmersibleSpotTex* SubmersibleSpotTex::instance = NULL;

void SubmersibleSpotTex::BuildPipeCaller(texture::Texture& dest)
{
	instance->BuildPipe(dest);
}
#endif // ENABLE_RUNTIME_COMPILATION

const int SubmersibleSpotTex::s_layerSizes[] = { 3 + 1, 3 + 1, 3 + 1, 3 + 1 };
const int SubmersibleSpotTex::s_numberOfLayers = ARRAY_LEN(SubmersibleSpotTex::s_layerSizes);
const float SubmersibleSpotTex::s_parallaxScale = 0.f;
const int* SubmersibleSpotTex_layerSizes = SubmersibleSpotTex::s_layerSizes;

#ifdef ENABLE_RUNTIME_COMPILATION
void SubmersibleSpotTex::BuildPipe(texture::Texture& dest)
#else // !ENABLE_RUNTIME_COMPILATION
void SubmersibleSpotTex_BuildPipe(texture::Texture& dest)
#endif // !ENABLE_RUNTIME_COMPILATION
{
	struct local
	{
		static void Build(void* out_pixelData, const texture::InputPixelInfo& in)
		{
			float isReflector = smoothStep(0.85f, 0.86f, in.uv.y);
			vector3f paintColor = { 0.9f, 0.9f, 0.05f };

			glowingPixelFormat result;
			result.diffuseColor = mix(paintColor, vector3f::zero, isReflector);
			result.specularColor = mix(vec3(0.04f), vec3(0.7f), isReflector);
			result.opacity = 1.f;
			result.height = 0.f;
			result.roughness = mix(1.f, 0.8f, isReflector);
			result.emissiveColor = vec3(isReflector);
			result.emissiveInvIntensity = 1.f;

			*(static_cast<glowingPixelFormat*>(out_pixelData)) = result;
		}
	};

	dest.ApplyFunction(local::Build);

	const size_t heightOffset = (float*)&(((glowingPixelFormat*)NULL)->height) - (float*)NULL;
	const int normalOffset = (float*)&(((glowingPixelFormat*)NULL)->normal) - (float*)NULL;
	texture::GenerateNormalMap(dest, heightOffset, normalOffset, SubmersibleBodyTex::s_parallaxScale);
}

// ---8<---------------------------------------------------------------

#ifdef ENABLE_RUNTIME_COMPILATION
REGISTERCLASS(RVBridgeWindowTex);
RVBridgeWindowTex* RVBridgeWindowTex::instance = NULL;

void RVBridgeWindowTex::BuildPipeCaller(texture::Texture& dest)
{
	instance->BuildPipe(dest);
}
#endif // ENABLE_RUNTIME_COMPILATION

const int RVBridgeWindowTex::s_layerSizes[] = { 3 + 1, 3 + 1, 3 + 1 };
const int RVBridgeWindowTex::s_numberOfLayers = ARRAY_LEN(RVBridgeWindowTex::s_layerSizes);
const float RVBridgeWindowTex::s_parallaxScale = 0.01f;
const int* RVBridgeWindowTex_layerSizes = RVBridgeWindowTex::s_layerSizes;

#ifdef ENABLE_RUNTIME_COMPILATION
void RVBridgeWindowTex::BuildPipe(texture::Texture& dest)
#else // !ENABLE_RUNTIME_COMPILATION
void RVBridgeWindowTex_BuildPipe(texture::Texture& dest)
#endif // !ENABLE_RUNTIME_COMPILATION
{
	struct local
	{
		static void Build(void* out_pixelData, const texture::InputPixelInfo& in)
		{
			// float usedOnFloor = interpolate(0.332f, 0.334f, uv.x) * interpolate(0.668f, 0.665f, uv.x);
			// float wallOnly = 1.f - usedOnFloor;

			vector2f grid = fract(mul(vec2(6.f, 4.f), in.uv));
			grid.y *= 6.f/4.f;

			// Portholes.
			vector3f bridgePaintColor = { 0.01f, 0.01f, 0.01f };
			float bridgeOnly = interpolate(0.87f, 0.871f, in.uv.y);

			vector3f windowDiffuseColor = { 0.03f, 0.03f, 0.03f};
			float windowWidth = (in.uv.y < 0.8f ? 0.18f : 0.42f);
			float windowHeight = 0.26f;
			float windowCornerRadius = (in.uv.y < 0.8f ? 0.1f : 0.05f);
			float window = norm(max(abs(grid - vec2(0.5f, 1.08f)) - vec2(windowWidth - windowCornerRadius, windowHeight - windowCornerRadius), 0.f)) - windowCornerRadius;
			float windowGlass = interpolate(0.01f, 0.f, window);// * wallOnly;

			float plates = generateMetalPlatesPatterns(mul(vec2(6.f, 4.f), in.uv));
			float paintCracks = generatePaintCracks(in.uv, 50.f, 5.f, 0.2f);

			pixelFormat result;
			result.diffuseColor = mix(mix(vec3(0.8f), bridgePaintColor, bridgeOnly), windowDiffuseColor, windowGlass);
			result.specularColor = vec3(0.04f);
			result.opacity = 1.f;
			result.height = mix(0.f, 0.1f * plates + mix(0.25f, 0.26f, paintCracks), interpolate(0.02f, 0.025f, window));
			result.roughness = mix(0.6f, 0.2f, windowGlass);

			*(static_cast<pixelFormat*>(out_pixelData)) = result;
		}
	};

	dest.ApplyFunction(local::Build);

	const size_t heightOffset = (float*)&(((pixelFormat*)NULL)->height) - (float*)NULL;
	const int normalOffset = (float*)&(((pixelFormat*)NULL)->normal) - (float*)NULL;
	texture::GenerateNormalMap(dest, heightOffset, normalOffset, RVBridgeWindowTex::s_parallaxScale);
}

// ---8<---------------------------------------------------------------

#ifdef ENABLE_RUNTIME_COMPILATION
REGISTERCLASS(RVBridgeNoWindowTex);
RVBridgeNoWindowTex* RVBridgeNoWindowTex::instance = NULL;

void RVBridgeNoWindowTex::BuildPipeCaller(texture::Texture& dest)
{
	instance->BuildPipe(dest);
}
#endif // ENABLE_RUNTIME_COMPILATION

const int RVBridgeNoWindowTex::s_layerSizes[] = { 3 + 1, 3 + 1, 3 + 1 };
const int RVBridgeNoWindowTex::s_numberOfLayers = ARRAY_LEN(RVBridgeWindowTex::s_layerSizes);
const float RVBridgeNoWindowTex::s_parallaxScale = 0.02f;
const int* RVBridgeNoWindowTex_layerSizes = RVBridgeNoWindowTex::s_layerSizes;

#ifdef ENABLE_RUNTIME_COMPILATION
void RVBridgeNoWindowTex::BuildPipe(texture::Texture& dest)
#else // !ENABLE_RUNTIME_COMPILATION
void RVBridgeNoWindowTex_BuildPipe(texture::Texture& dest)
#endif // !ENABLE_RUNTIME_COMPILATION
{
	struct local
	{
		static void Build(void* out_pixelData, const texture::InputPixelInfo& in)
		{
			// Deformation of the hull revealing the underlying structure.
			vector2f bumpGrid = clamp((fract(mul(vec2(6.f, 4.f), in.uv)) * 2.f - 1.f) * 1.1f, -1.f, 1.f);
			float bump = 1.f - (1.f - bumpGrid.x * bumpGrid.x) * (1.f - bumpGrid.y * bumpGrid.y);

			float paintCracks = generatePaintCracks(in.uv, 50.f, 5.f, 0.2f);

			pixelFormat result;
			result.diffuseColor = vec3(0.8f);
			result.specularColor = vec3(0.04f);
			result.opacity = 1.f;
			result.height = 0.1f * bump + mix(0.25f, 0.26f, paintCracks);
			result.roughness = 0.6f;

			*(static_cast<pixelFormat*>(out_pixelData)) = result;
		}
	};

	dest.ApplyFunction(local::Build);

	const size_t heightOffset = (float*)&(((pixelFormat*)NULL)->height) - (float*)NULL;
	const int normalOffset = (float*)&(((pixelFormat*)NULL)->normal) - (float*)NULL;
	texture::GenerateNormalMap(dest, heightOffset, normalOffset, RVBridgeNoWindowTex::s_parallaxScale);
}

// ---8<---------------------------------------------------------------
