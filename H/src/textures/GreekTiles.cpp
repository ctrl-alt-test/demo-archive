#include "GreekTiles.hh"

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

#ifdef ENABLE_RUNTIME_COMPILATION
REGISTERCLASS(GreekTiles);
GreekTiles* GreekTiles::instance = NULL;

void GreekTiles::BuildPipeCaller(texture::Texture& dest)
{
	instance->BuildPipe(dest);
}
#endif // ENABLE_RUNTIME_COMPILATION

const int GreekTiles::s_layerSizes[] = { 3 + 1, 3 + 1, 3 + 1 };
const int GreekTiles::s_numberOfLayers = ARRAY_LEN(GreekTiles::s_layerSizes);
const float GreekTiles::s_parallaxScale = 0.01f;

void GreekTiles::BuildPipe(texture::Texture& dest)
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

			vector2f uv = in.uv;

			vector3f jointColor = mix(vec3(0.01f), vec3(0.10f), Algebra::pow(rand.fgen(), 3.f));

			float jMask;

			// 1st crown
			vector2f aUV = float(NUM_A_TILES) * uv;
			float aTileID = Noise::LatticeNoise::value(aUV, float(NUM_A_TILES)) * 0.5f + 0.5f;
			vector3f aColor = mix(vec3(0.13f, 0.28f, 0.26f), vec3(0.55f, 0.65f, 0.65f), aTileID);

			float ajThick = 0.08f;
			vector2f jaMask = smoothStep(ajThick - 0.02f, ajThick + 0.02f, abs(2.f * fract(aUV + 0.5f) - 1.f));
			vector2f abMask2 =
				mul(smoothStep(1.0f, 1.04f, float(NUM_A_TILES) * uv),
					smoothStep(1.0f, 1.04f, float(NUM_A_TILES) - float(NUM_A_TILES) * uv));
			float abMask = abMask2.x * abMask2.y;


			result.diffuseColor = aColor;
			vector2f aTileUV = (fract(aUV) - 0.5f*ajThick) / (1.f - ajThick);
			vector2f d = abs(aTileUV * 2.f - 1.f);
			float aHeight = 1.f - 0.1f * (Noise::PerlinNoise::value(uv * 16.f, 16.f) * 0.5f - 0.5f);
			aHeight -= Algebra::pow(1.f - clamp(10.f * msys_min(1.f - d.x, 1.f - d.y), 0.f, 1.f), 3.f);
			result.height = aHeight;
			jMask = jaMask.x * jaMask.y;
			uv = (float(NUM_A_TILES) * uv - 1.f)/(float(NUM_A_TILES) - 2.f);

			//*
			// 2n crown
			vector2f bUV = float(NUM_B_TILES) * (uv + vec2(-uv.y, uv.x) + 1.f);
			vector3f bColor = mix(vec3(0.81f, 0.53f, 0.36f), vec3(1.0f, 0.81f, 0.66f),
				Noise::LatticeNoise::value(bUV, float(NUM_B_TILES)) * 0.5f + 0.5f);
			float bjMask = 0.12f;
			vector2f jbMask = smoothStep(bjMask - 0.04f, bjMask + 0.04f, abs(2.f * fract(bUV + 0.5f) - 1.f));
			vector2f bcMask2 =
				mul(smoothStep(0.95f, 1.0f, float(NUM_B_TILES) * uv),
					smoothStep(0.95f, 1.0f, float(NUM_B_TILES) - float(NUM_B_TILES) * uv));
			float bcMask = bcMask2.x * bcMask2.y;

			uv = (float(NUM_B_TILES) * uv - 1.f)/(float(NUM_B_TILES) - 2.f);

			result.diffuseColor = mix(result.diffuseColor, bColor, abMask);
			d = abs(fract(bUV) * 2.f - 1.f);
			float bHeight = clamp(2.5f * msys_min(1.f - d.x, 1.f - d.y), 0.f, 1.f);
			result.height = mix(result.height, bHeight, abMask);

			jMask = mix(jMask, jbMask.x * jbMask.y, abMask);

			//*
			// Inner
			vector2f cUV = float(NUM_C_TILES) * (uv + vec2(-uv.y, uv.x) + 1.f);
			vector3f cColor = mix(vec3(0.29f, 0.43f, 0.41f), vec3(0.83f, 0.85f, 0.85f),
				Noise::LatticeNoise::value(cUV, float(NUM_C_TILES)) * 0.5f + 0.5f);
			float cjThick = 0.05f;
			vector2f jcMask =
				mul(smoothStep(cjThick - 0.01f, cjThick + 0.01f, abs(2.f * fract(cUV + 0.5f) - 1.f)),
					mul(smoothStep(0.995f, 0.99f, uv),
						smoothStep(0.995f, 0.99f, 1.f - uv)));

			result.diffuseColor = mix(result.diffuseColor, cColor, bcMask);
			//vector2f cTileUV = (fract(cUV) - 0.5f*cjThick) / (1.f - ajThick);
			d = abs(fract(cUV) * 2.f - 1.f);
			float cHeight = 1.f - 0.1f * (Noise::PerlinNoise::value(uv * 16.f, 16.f) * 0.5f - 0.5f);
			cHeight -= Algebra::pow(1.f - clamp(4.f * msys_min(1.f - d.x, 1.f - d.y), 0.f, 1.f), 3.f);
			result.height = mix(result.height, cHeight, bcMask);
			jMask = mix(jMask, jcMask.x * jcMask.y, bcMask);
			//*/

			result.diffuseColor = mix(jointColor, result.diffuseColor, jMask);
			result.roughness = mix(0.9f, 0.05f, jMask);
			result.roughness = clamp(result.roughness + rand.fgen() * 0.4f, 0.f, 1.f);
			result.height = mix(0.3f, result.height, jMask);

			vector2f center = { 0.5f, 0.5f };
			float r = norm(center - uv);
			float ring = smoothStep(0.35f, 0.348f, r) * smoothStep(0.250f, 0.252f, r);
			if (ring != 0.f)
			{
				result.diffuseColor = mix(result.diffuseColor, vec3(0.f), ring);

				// Red > 1 for gold; it's not a mistake.
				vector3f goldColor = { 1.2f, 0.766f, 0.336f };
				vector3f copperColor = { 0.955f, 0.637f, 0.538f };
				vector3f ringColor = (r > 0.3f ? goldColor : copperColor);
				result.specularColor = mix(result.specularColor, ringColor, ring);
				float fbm = 0.5f * Noise::PerlinNoise::value(uv * 8.f, 8.f) +
					0.25f * Noise::PerlinNoise::value(uv * 16.f, 16.f) +
					0.125f * Noise::PerlinNoise::value(uv * 32.f, 32.f) +
					0.0625f * Noise::PerlinNoise::value(uv * 64.f, 64.f);
				result.roughness = mix(result.roughness, mix(0.8f, 0.95f, fbm * 0.5f + 0.5f), ring);
				result.height = mix(result.height, mix(0.9f, 1.f, Algebra::abs(Noise::PerlinNoise::value(uv * 6.f, 6.f))), 0.99f * ring);
				float groove = mix(0.98f, 1.0f, smoothStep(0.001f, 0.0015f, Algebra::abs(r - 0.3f)));
				result.height *= groove;
			}

			*(static_cast<pixelFormat*>(out_pixelData)) = result;
		}
	};

	dest.ApplyFunction(local::Build);

	const size_t heightOffset = (float*)&(((pixelFormat*)NULL)->height) - (float*)NULL;
	const int normalOffset = (float*)&(((pixelFormat*)NULL)->normal) - (float*)NULL;
	texture::GenerateNormalMap(dest, heightOffset, normalOffset, s_parallaxScale);
}
