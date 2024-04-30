#include "OldStone.hh"

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

// ---8<---------------------------------------------------------------

#if DLL_COMPILATION
namespace texture
{
	tilePattern interleavedTiles(vector2f p, int columns, int rows, float offset)
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
}
#endif // DLL_COMPILATION

// ---8<---------------------------------------------------------------

#ifdef ENABLE_RUNTIME_COMPILATION
REGISTERCLASS(OldStone);
OldStone* OldStone::instance = NULL;

void OldStone::BuildPipeCaller(texture::Texture& dest)
{
	instance->BuildPipe(dest);
}
#endif // ENABLE_RUNTIME_COMPILATION

const int OldStone::s_layerSizes[] = { 3 + 1, 3 + 1, 3 + 1 };
const int OldStone::s_numberOfLayers = ARRAY_LEN(OldStone::s_layerSizes);
const float OldStone::s_parallaxScale = 0.025f;
const int* OldStone_layerSizes = OldStone::s_layerSizes;

#ifdef ENABLE_RUNTIME_COMPILATION
void OldStone::BuildPipe(texture::Texture& dest)
#else // !ENABLE_RUNTIME_COMPILATION
void OldStone_BuildPipe(texture::Texture& dest)
#endif // !ENABLE_RUNTIME_COMPILATION
{
	struct local
	{
		// Inspired by the Pont du Gard:
		// http://theredlist.com/media/database/architecture/history/architecture-romaine/pont-du-gard/005_pont-du-gard_theredlist.jpg
		static void stones(void* out_pixelData, const texture::InputPixelInfo& in)
		{
			// Step 1: Make some interleaved tiles.
			//texture::tilePattern stone = texture::interleavedTiles(in.uv, 1, 1, 0.f);
			//texture::tilePattern stone = texture::interleavedTiles(in.uv, 8, 8, 0.f);
			//texture::tilePattern stone = texture::interleavedTiles(in.uv, 10, 24, 0.f);
			texture::tilePattern stone = texture::interleavedTiles(in.uv, 10, 24, 0.15f);

			// Shape
			// ---8<---------------------------------------------------
			vector2f center = abs(stone.uv * 2.f - 1.f);
			float d = msys_max(center.x, center.y);
			float height = d;


			// Step 2: Basic shape of the bricks.
			//*
			float perfectShape = mix(
				smoothStep(1.1f, 0.9f, d),
				smoothStep(0.825f, 0.8f, d), 0.15f);
			height = perfectShape;
			// */


			// Step 3: Move bricks up or down, and add grain detail.
			//*
			float grain = Noise::fBmPerlin(in.uv, 16.f, 4, 3.f, 1.25f);
			float loose = Noise::Hash::get32f(stone.id);
			float slope = (stone.id - 0.5f) * stone.uv.x + 0.5f;
			float originalShape = mix(
				mix(perfectShape, grain, 0.15f),
				mix(loose, slope, 0.5f), 0.6f);
			height = originalShape;
			// */


			// Step 4: Add damage.
			//*
			float holes = 1.f;
			if (stone.id < 0.6f)
			{
				//vector2f holeuv = in.uv;
				vector2f holeuv = vec2(stone.id, Noise::Hash::get32f(stone.id)) + mul(stone.uv, vec2(0.1f, 0.15f));
				float noise = Noise::fBmPerlin(holeuv, 2.f, 5, 3.f, 1.8f);
				holes = smoothStep(-0.5f, 0.2f, noise);
				holes = holes * holes * holes * holes;

				//height = noise;
				//height = holes;
				height = originalShape * mix(0.25f, 1.f, holes);
			}
			// */


			// Step 5: Add some color.
			/*
			vector3f color = vec3(height);
			/*/
			const vector3f orange = { 0.75f, 0.49f, 0.22f };
			const vector3f white = { 0.83f, 0.73f, 0.54f };
			//vector3f color = vec3(stone.id);
			//vector3f color = vec3(Noise::PerlinNoise::value(in.uv * 3.f, 3.f));
			//vector3f color = vec3(mix(stone.id, Noise::PerlinNoise::value(in.uv * 3.f, 3.f), 0.4f));
			vector3f color = mix(orange, white, mix(stone.id, Noise::PerlinNoise::value(in.uv * 3.f, 3.f), 0.4f));
			// */


			// Step 7: Add stains.
			//*
			const vector3f stainColor = { 0.04f, 0.08f, 0.09f };
			float stains = smoothStep(0.1f, -0.435f, Noise::fBmPerlin(mul(in.uv, vec2(2.f, 1.f)), 2.f, 5, 4.f, 1.4f));
			color = mix(color, stainColor, stains);
			// */


			// Step 8: Darken the cracks.
			//*
			float cracks = interpolate(0.4f, 0.1f, height);
			color *= mix(1.f, 0.2f, cracks);
			// */


			pixelFormat result;
			result.height = height;
			result.diffuseColor = color;
			result.specularColor = vec3(0.04f);
			result.opacity = 1.f;

			// Step 6: Roughness based on the damage.
			/*
			result.roughness = 0.25f;
			/*/
			result.roughness = mix(0.9f, 1.f, interpolate(0.25f, 0.f, holes));
			// */

			*(static_cast<pixelFormat*>(out_pixelData)) = result;
		}
	};

	dest.ApplyFunction(local::stones);

	const size_t heightOffset = (float*)&(((pixelFormat*)NULL)->height) - (float*)NULL;
	const int normalOffset = (float*)&(((pixelFormat*)NULL)->normal) - (float*)NULL;
	texture::GenerateNormalMap(dest, heightOffset, normalOffset, OldStone::s_parallaxScale);
}
