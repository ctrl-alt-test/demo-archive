#include "Marble.hh"

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
#include "engine/noise/SimplexNoise.hh"
#include "engine/noise/ValueNoise.hh"
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

Algebra::vector3f texture::whiteMarble(const Algebra::vector2f& uv)
{
	int octaves = 6;
	float lacunarity = 2.f;
	float invGain = 2.f;
	float turbulence = 5.f;
	vector2f p = { Noise::fBmPerlin(uv, 1.f, octaves, lacunarity, invGain),                Noise::fBmPerlin(uv + vec2(3.1f, 5.7f), 1.f, octaves, lacunarity, invGain) };
	vector2f q = { Noise::fBmPerlin(uv + turbulence*p, 1.f, octaves, lacunarity, invGain), Noise::fBmPerlin(uv + turbulence*p + vec2(5.7f, 7.3f), 1.f, octaves, lacunarity, invGain) };
	vector2f r = { Noise::fBmPerlin(uv + turbulence*q, 1.f, octaves, lacunarity, invGain), Noise::fBmPerlin(uv + turbulence*q + vec2(7.3f, 11.7f), 1.f, octaves, lacunarity, invGain) };

	vector3f color = { 0.8f, 0.8f, 0.8f };

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

Algebra::vector3f texture::redMarble(const Algebra::vector2f& uv)
{
	int octaves = 6;
	float lacunarity = 2.f;
	float invGain = 2.f;
	float turbulence = 5.f;
	vector2f p = { Noise::fBmPerlin(uv, 1.f, octaves, lacunarity, invGain),                Noise::fBmPerlin(uv + vec2(3.1f, 5.7f), 1.f, octaves, lacunarity, invGain) };
	vector2f q = { Noise::fBmPerlin(uv + turbulence*p, 1.f, octaves, lacunarity, invGain), Noise::fBmPerlin(uv + turbulence*p + vec2(5.7f, 7.3f), 1.f, octaves, lacunarity, invGain) };
	vector2f r = { Noise::fBmPerlin(uv + turbulence*q, 1.f, octaves, lacunarity, invGain), Noise::fBmPerlin(uv + turbulence*q + vec2(7.3f, 11.7f), 1.f, octaves, lacunarity, invGain) };

	vector3f lightPink = { 0.652f, 0.364f, 0.272f };
	vector3f darkRed = { 0.038f, 0.001f, 0.012f };
	vector3f mediumRed = { 0.145f, 0.041f, 0.046f };

	vector3f color = { 0.117f, 0.037f, 0.043f };

	float lighterPatterns = smoothStep(0.f, 1.f, 2.f * norm(q));
	lighterPatterns *= lighterPatterns;
	color = mix(color, mediumRed, lighterPatterns);

	float darkPatterns = interpolate(0.f, 0.6f, 2.f * r.x);
	darkPatterns *= darkPatterns*darkPatterns;
	color = mix(color, darkRed, darkPatterns);

	float clearPatterns = interpolate(0.5f, 0.825f, 2.f * norm(r));
	clearPatterns *= clearPatterns*clearPatterns;
	color = mix(color, lightPink, clearPatterns);

	return color;
}

static
float DamagedMarbleBlocksBump(const vector2f& uv, const texture::tilePattern& blocks)
{
	float fbm1 = interpolate(0.1f, 1.f,
		Noise::fBmPerlin(uv * 2.f + blocks.id, 2.f, 2, 2.f, 1.f));

	const vector2f p = blocks.uv - vec2(0.5f);
	float blockBump = dot(p, p);
	blockBump = 0.15f - 1.6f * blockBump * blockBump * mix(msys_sqrtf(fbm1), 1.f, 0.2f);
	return blockBump;
}

// Typical 64k over factorization.
// The only reason for this function is to reduce binary size due to
// similar code. But from a design stand point, it doesn't make sense.
static
pixelFormat MakeItMarble(const Algebra::vector2f& uv, float h, float marbleOffset, bool isWhite, float isOrichalcum)
{
	const vector3f marbleColor = (isWhite ?
		texture::whiteMarble(mul(uv, vec2(1.f, 1.5f)) + marbleOffset) :
		texture::redMarble(mul(uv, vec2(1.f, 1.5f)) + marbleOffset));

	const Noise::VoronoiInfo roughnessVoronoi = Noise::WorleyNoise::value(uv * 300.f, 100.f, 1.f, 1.f);

	const vector3f orichalcumColor = { 1.f, 0.3f, 0.2f };
	const vector3f defaultSpecular = { 0.04f, 0.04f, 0.04f };

	pixelFormat result;
	result.height = h;
	result.diffuseColor = mix(marbleColor, vector3f::zero, isOrichalcum);
	result.specularColor = mix(defaultSpecular, orichalcumColor, isOrichalcum);
	result.opacity = 1.f;
	result.roughness = mix(0.6f, 0.8f, roughnessVoronoi.GetHash()) * mix(1.f, 0.8f, isOrichalcum);
	return result;
}

texture::tilePattern texture::interleavedTiles(vector2f p, int columns, int rows, float offset)
{
	vector2f scale = { (float)columns, (float)rows };
	int row = msys_ifloorf(p.y * scale.y);
	float xOffset = 0.25f * offset * row;//(row % 2);
	p.x += xOffset;
	int column = msys_ifloorf(p.x * scale.x);
	p = mul(p, scale);
	vector2f tile = floor(p);

	p = fract(p);
	vector2f p2 = 1.f - abs(p * 2.f - 1.f);
	p2.x /= scale.x;
	p2.y /= scale.y;
	float d = msys_min(p2.x, p2.y);
	float id = Noise::Hash::get32f(tile);
	texture::tilePattern result = { d, id, row, column, p };
	return result;
}

static float Plane(const vector2f& p, const vector2f& n, float t0)
{
	return dot(p, normalized(n)) - t0;
}

static float Box(const vector2f& p, const vector2f& size)
{
	vector2f u = abs(p) - size;
	return msys_max(u.x, u.y);
}

static float Spike(const vector2f& p)
{
	return msys_max(msys_max(
		Plane(p , vec2(1.f, 0.f), 0.f),
		Plane(p , vec2(-1.f, -1.f), -0.06f)),
		Plane(p , vec2(-5.f, 1.f), 0.09f));
}

static float ConspiracyLogo(const vector2f& c)
{
	float halfSqrt2 = 1.41421356f / 2.f;
	vector2f c2 = vec2(c.y, -c.x);
	vector2f c3 = halfSqrt2 * vec2(c.x + c.y, -c.x + c.y);
	vector2f c4 = vec2(-c3.y, c3.x);

	float d = norm(c) - 0.05f;
	d = msys_min(d, msys_min(Spike(c), Spike(-c)));
	d = msys_min(d, msys_min(Spike(c2), Spike(-c2)));
	d = msys_min(d, msys_min(Spike(c3), Spike(-c3)));
	d = msys_min(d, msys_min(Spike(c4), Spike(-c4)));
	return d;
}

static float StillLogo(vector2f p)
{
	const float h = 0.15f;

	vector2f pIII = p;
	pIII.x = (fract((1.f + pIII.x) * 5.f + 0.5f) - 0.5f) / 5.f;
	float d = Box(pIII, vec2(0.33f / 5.f, h));

	d = msys_max(d, -Box(p + vec2(3.f / 5.f, 0.f), vec2(0.1f, h * 1.2f)));
	d = msys_max(d, -Box(p + vec2(2.f / 5.f, h * -0.7f), vec2(0.1f, h * 0.5f)));
	d = msys_max(d, -Box(p + vec2(0.f, h * -0.4f), vec2(0.1f, h * 0.2f)));
	d = msys_min(d, Box(p + vec2(1.f / 5.f, 0.f), vec2(1.f / 5.f, h * 0.2f)));
	d = msys_min(d, Box(p + vec2(2.5f / 5.f, h * 0.8f), vec2(0.5f / 5.f, h * 0.2f)));

	d = msys_max(d, Box(p + vec2(1.f / 10.f, 0.), vec2(0.6f, 0.2f)));
	return d;
}

static float StillStill(vector2f p)
{
	p = fract(p);
	float d = StillLogo(1.6f * (p - vec2(0.81f, 0.5f)));
	d = msys_min(d, StillLogo(1.6f * (1.f - p - vec2(0.81f, 0.5f))));
	return d;
}

static float MercuryLogo(vector2f p)
{
	float d1 = (Algebra::abs(fract(p.y * 4.f + 1.5f) * 2.f - 1.f) - 0.6f) / 6.f;
	d1 = msys_max(d1, Box(p, vec2(0.4f)));

	float d2 = (Algebra::abs(fract(p.x * 3.5f + 1.5f) * 2.f - 1.f) - 0.6f) / 6.f;
	p.y = mix(-0.1f - p.y, p.y, smoothStep(0.149f, 0.151f, Algebra::abs(p.x)));
	d2 = msys_max(d2, -Box(p + vec2(0.f, 0.3f), vec2(0.5f, 0.25f)));

	float d = msys_min(d1, d2);
	d = msys_max(d, Box(p, vec2(0.35f, 0.45f)));
	return d;
}

// ---8<---------------------------------------------------------------

#ifdef ENABLE_RUNTIME_COMPILATION
REGISTERCLASS(FineWhiteMarbleWall);
FineWhiteMarbleWall* FineWhiteMarbleWall::instance = NULL;

void FineWhiteMarbleWall::BuildPipeCaller(texture::Texture& dest)
{
	instance->BuildPipe(dest);
}
#endif // ENABLE_RUNTIME_COMPILATION

const int FineWhiteMarbleWall::s_layerSizes[] = { 3 + 1, 3 + 1, 3 + 1 };
const int FineWhiteMarbleWall::s_numberOfLayers = ARRAY_LEN(FineWhiteMarbleWall::s_layerSizes);
const float FineWhiteMarbleWall::s_parallaxScale = 0.02f;
const int* FineWhiteMarbleWall_layerSizes = FineWhiteMarbleWall::s_layerSizes;

#ifdef ENABLE_RUNTIME_COMPILATION
void FineWhiteMarbleWall::BuildPipe(texture::Texture& dest)
#else // !ENABLE_RUNTIME_COMPILATION
void FineWhiteMarbleWall_BuildPipe(texture::Texture& dest)
#endif // !ENABLE_RUNTIME_COMPILATION
{
	struct local
	{
		static void Build(void* out_pixelData, const texture::InputPixelInfo& in)
		{
			texture::tilePattern blocks = texture::interleavedTiles(in.uv, 8, 11, 0.125f);
			float blockBump = DamagedMarbleBlocksBump(in.uv, blocks);

			float grout = smoothStep(0.002f, 0.004f, blocks.d);
			*(static_cast<pixelFormat*>(out_pixelData)) = MakeItMarble(in.uv, mix(0.f, 0.2f*blockBump, grout), blocks.id, true, 0.f);
		}
	};

	dest.ApplyFunction(local::Build);

	const size_t heightOffset = (float*)&(((pixelFormat*)NULL)->height) - (float*)NULL;
	const int normalOffset = (float*)&(((pixelFormat*)NULL)->normal) - (float*)NULL;
	texture::GenerateNormalMap(dest, heightOffset, normalOffset, FineWhiteMarbleWall::s_parallaxScale);
}

// ---8<---------------------------------------------------------------

#ifdef ENABLE_RUNTIME_COMPILATION
REGISTERCLASS(FineRedAndWhiteMarbleFloor);
FineRedAndWhiteMarbleFloor* FineRedAndWhiteMarbleFloor::instance = NULL;

void FineRedAndWhiteMarbleFloor::BuildPipeCaller(texture::Texture& dest)
{
	instance->BuildPipe(dest);
}
#endif // ENABLE_RUNTIME_COMPILATION

const int FineRedAndWhiteMarbleFloor::s_layerSizes[] = { 3 + 1, 3 + 1, 3 + 1 };
const int FineRedAndWhiteMarbleFloor::s_numberOfLayers = ARRAY_LEN(FineRedAndWhiteMarbleFloor::s_layerSizes);
const float FineRedAndWhiteMarbleFloor::s_parallaxScale = 0.02f;
const int* FineRedAndWhiteMarbleFloor_layerSizes = FineRedAndWhiteMarbleFloor::s_layerSizes;

#ifdef ENABLE_RUNTIME_COMPILATION
void FineRedAndWhiteMarbleFloor::BuildPipe(texture::Texture& dest)
#else // !ENABLE_RUNTIME_COMPILATION
void FineRedAndWhiteMarbleFloor_BuildPipe(texture::Texture& dest)
#endif // !ENABLE_RUNTIME_COMPILATION
{
	struct local
	{
		static void Build(void* out_pixelData, const texture::InputPixelInfo& in)
		{
			texture::tilePattern blocks = texture::interleavedTiles(in.uv, 4, 4, 0.f);
			float blockBump = DamagedMarbleBlocksBump(in.uv, blocks);

			bool isWhite = (blocks.row % 2 == blocks.column % 2);
			float grout = smoothStep(0.003f, 0.f, blocks.d);
			float h = 0.8f * blockBump;

			h = mix(h, 0.f, grout);

			*(static_cast<pixelFormat*>(out_pixelData)) = MakeItMarble(in.uv, h, blocks.id, isWhite, 0.f);
		}
	};

	dest.ApplyFunction(local::Build);

	const size_t heightOffset = (float*)&(((pixelFormat*)NULL)->height) - (float*)NULL;
	const int normalOffset = (float*)&(((pixelFormat*)NULL)->normal) - (float*)NULL;
	texture::GenerateNormalMap(dest, heightOffset, normalOffset, FineRedAndWhiteMarbleFloor::s_parallaxScale);
}

// ---8<---------------------------------------------------------------

#ifdef ENABLE_RUNTIME_COMPILATION
REGISTERCLASS(GreetingsMarbleFloor);
GreetingsMarbleFloor* GreetingsMarbleFloor::instance = NULL;

void GreetingsMarbleFloor::BuildPipeCaller(texture::Texture& dest)
{
	instance->BuildPipe(dest);
}
#endif // ENABLE_RUNTIME_COMPILATION

const int GreetingsMarbleFloor::s_layerSizes[] = { 3 + 1, 3 + 1, 3 + 1 };
const int GreetingsMarbleFloor::s_numberOfLayers = ARRAY_LEN(GreetingsMarbleFloor::s_layerSizes);
const float GreetingsMarbleFloor::s_parallaxScale = 0.02f;
const int* GreetingsMarbleFloor_layerSizes = GreetingsMarbleFloor::s_layerSizes;

#ifdef ENABLE_RUNTIME_COMPILATION
void GreetingsMarbleFloor::BuildPipe(texture::Texture& dest)
#else // !ENABLE_RUNTIME_COMPILATION
void GreetingsMarbleFloor_BuildPipe(texture::Texture& dest)
#endif // !ENABLE_RUNTIME_COMPILATION
{
	struct local
	{
		static void Build(void* out_pixelData, const texture::InputPixelInfo& in)
		{
			const vector2f circle = in.uv * 2.f - vec2(1.f, 1.f);
			float r = norm(circle);
			float theta = msys_atan2f(circle.x, circle.y) + PI ;
			float r1 = 0.83f;
			float r2 = 1.f;
			vector2f uvCircle = { 24.f * 0.5f * theta / PI, interpolate(r1, r2, r) };

			// Conspiracy greeting.
			float conspiracy = ConspiracyLogo(0.6f * (circle));
			float conspiracyOutline = smoothStep(0.005f, 0.003f, Algebra::abs(conspiracy));

			// Still greeting.
			vector2f uvStill = { uvCircle.x, 0.075f + 0.6f * uvCircle.y };
			float still = StillStill(uvStill);

			// Mercury greeting.
			vector2f uvMercury = { -0.1f + 2.f * uvCircle.y, Algebra::abs(fract(uvCircle.x) * 2.f - 1.f) * 1.1f - 0.1f };
			float mercury = MercuryLogo(uvMercury - 0.5f);

			bool isWhite = (conspiracy > 0.f) && (r < r1);
			float orichalcum = conspiracyOutline + interpolate(0.0025f, -0.0025f, msys_min(still, mercury));
			float isOrichalcum = interpolate(0.5f, 1.f, orichalcum);
			float grout = smoothStep(0.003f, 0.f, Algebra::abs(r - r2)) + smoothStep(0.003f, 0.f, Algebra::abs(r - r1));

			float h = mix(0.2f, 0.4f, interpolate(r2, r1, r));
			h = mix(h, 0.f, grout);
			h -= 0.02f * interpolate(0.f, 0.5f, orichalcum);

			*(static_cast<pixelFormat*>(out_pixelData)) = MakeItMarble(in.uv, h, 0.f, isWhite, isOrichalcum);
		}
	};

	dest.ApplyFunction(local::Build);

	const size_t heightOffset = (float*)&(((pixelFormat*)NULL)->height) - (float*)NULL;
	const int normalOffset = (float*)&(((pixelFormat*)NULL)->normal) - (float*)NULL;
	texture::GenerateNormalMap(dest, heightOffset, normalOffset, FineRedAndWhiteMarbleFloor::s_parallaxScale);
}

// ---8<---------------------------------------------------------------

#ifdef ENABLE_RUNTIME_COMPILATION
REGISTERCLASS(Gold);
Gold* Gold::instance = NULL;

void Gold::BuildPipeCaller(texture::Texture& dest)
{
	instance->BuildPipe(dest);
}
#endif // ENABLE_RUNTIME_COMPILATION

const int Gold::s_layerSizes[] = { 3 + 1, 3 + 1, 3 + 1 };
const int Gold::s_numberOfLayers = ARRAY_LEN(FineWhiteMarbleWall::s_layerSizes);
const float Gold::s_parallaxScale = 0.02f;
const int* Gold_layerSizes = FineWhiteMarbleWall::s_layerSizes;

#ifdef ENABLE_RUNTIME_COMPILATION
void Gold::BuildPipe(texture::Texture& dest)
#else // !ENABLE_RUNTIME_COMPILATION
void Gold_BuildPipe(texture::Texture& dest)
#endif // !ENABLE_RUNTIME_COMPILATION
{
	struct local
	{
		static void Build(void* out_pixelData, const texture::InputPixelInfo& in)
		{
			vector3f goldColor = { 1.f, 0.766f, 0.2f };

			const Noise::VoronoiInfo voronoi = Noise::WorleyNoise::value(in.uv * 400.f, 400.f, 1.f, 1.f);

			pixelFormat result;
			result.height = 0.f;
			result.diffuseColor = vec3(0);
			result.specularColor = goldColor;
			result.opacity = 1.f;
			result.roughness = mix(1.f, 0.8f, voronoi.GetHash());
			*(static_cast<pixelFormat*>(out_pixelData)) = result;
		}
	};

	dest.ApplyFunction(local::Build);

	const size_t heightOffset = (float*)&(((pixelFormat*)NULL)->height) - (float*)NULL;
	const int normalOffset = (float*)&(((pixelFormat*)NULL)->normal) - (float*)NULL;
	texture::GenerateNormalMap(dest, heightOffset, normalOffset, Gold::s_parallaxScale);
}
