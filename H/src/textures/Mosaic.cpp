#include "Mosaic.hh"

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
REGISTERCLASS(Mosaic);
Mosaic* Mosaic::instance = NULL;

void Mosaic::BuildPipeCaller(texture::Texture& dest)
{
	instance->BuildPipe(dest);
}
#endif // ENABLE_RUNTIME_COMPILATION

const int Mosaic::s_layerSizes[] = { 3 + 1, 3 + 1, 3 + 1 };
const int Mosaic::s_numberOfLayers = ARRAY_LEN(Mosaic::s_layerSizes);
const float Mosaic::s_parallaxScale = 0.02f;

void Mosaic::BuildPipe(texture::Texture& dest)
{
	struct local
	{
		static float circle(vector2f uv, vector2f center, float size)
		{
			return smoothStep(size, size*0.8f, norm(uv - center));
		}

		static float rectangle(vector2f uv, vector2f topLeft, vector2f bottomRight)
		{
			float r = smoothStep(topLeft.x-0.01f, topLeft.x, uv.x) -
				smoothStep(topLeft.y-0.01f, topLeft.y, uv.y) -
				smoothStep(bottomRight.x-0.01f, bottomRight.x, uv.x) -
				smoothStep(bottomRight.y, bottomRight.y-0.01f, uv.y);
			return clamp(r);
		}

		struct PictureColor
		{
			bool		isWritten;
			vector3f	rgb;
			float		orichalcum;
		};

		static PictureColor sun(const vector2f& uv)
		{
			vector3f white = { 1.f, 0.883f, 0.297f };
			vector3f yellow = { 1.f, 0.797f, 0.044f };
			vector3f orange = { 1.f, 0.542f, 0.045f };

			vector2f ray = vec2(0.5f, 0.83f) - uv;
			float r = norm(ray);
			float theta = msys_atan2f(ray.y, ray.x) + PI;

			float radialDistance = Algebra::abs((fract(21.f * theta / (2.f * PI) + 0.04f/r * smoothStep(0.f, 1.f, Algebra::abs(fract(10.f * r) * 2.f - 1.f))) * 2.f - 1.f));
			float radial = smoothStep(0.45f, 0.55f, radialDistance);
			float sun = smoothStep(0.06f, 0.05f, r);

			PictureColor result;
			result.isWritten = true;
			result.rgb = mix(orange, mix(white, yellow, clamp(3.f * r, 0.f, 1.f)), clamp(radial + sun));
			result.orichalcum = smoothStep(0.35f, 0.45f, radialDistance) * smoothStep(0.65f, 0.55f, radialDistance) * (1.f - sun);//smoothStep(0.12f, 0.125f, r);
			return result;
		}

		static PictureColor sea(const vector2f& uv, float lines, float period)
		{
			vector3f lightBlue = { 0.459f, 0.672f, 0.948f };
			vector3f blue = { 0.044f, 0.235f, 0.619f };
			vector3f darkBlue = { 0.001f, 0.080f, 0.335f };

			float wave = 0.02f * smoothStep(0.f, 1.f, Algebra::abs(fract(period * uv.x) * 2.f - 1.f));
			if (uv.y + wave > 1.f)
			{
				PictureColor discard = { false };
				return discard;
			}
			float waves = Algebra::abs(fract(lines * (uv.y + wave)) * 2.f - 1.f);

			vector3f blue1 = mix(darkBlue, blue, smoothStep(0.f, 1.f, uv.y));
			vector3f blue2 = mix(blue, lightBlue, uv.y);

			PictureColor result;
			result.isWritten = true;
			result.rgb = mix(blue1, blue2, mix(uv.y, smoothStep(0.f, 0.9f, waves), 0.7f));
			result.orichalcum = 0.f;
			return result;
		}

		static PictureColor starFish(const vector2f& uv, int arms, float rotation)
		{
			vector2f ray = vec2(0.5f, 0.5f) - uv;
			float r = norm(ray);
			float theta = msys_atan2f(ray.y, ray.x) + PI + rotation;

			float wave = 0.4f / (0.4f + Algebra::abs(msys_tanf(0.5f * arms * theta)));
			float shape = mix(0.25f, 1.f, smootherStep(wave)) - 2.f * r;

			PictureColor result;
			result.isWritten = (shape > 0.f);
			result.rgb = vec3(0.907f, 0.037f, 0.130f);
			result.orichalcum = 0.f;

			return result;
		}

		static PictureColor snailShell(const vector2f& uv)
		{
			vector3f white = { 1.f, 0.883f, 0.297f };
			vector3f orange = { 1.f, 0.542f, 0.045f };

			PictureColor result;

			vector2f ray = vec2(0.5f, 0.5f) - uv;
			float r = norm(ray);
			float theta = msys_atan2f(ray.y, ray.x) + PI;

			float exp_r = 5.f * Algebra::pow(0.0005f, r);
			float loop = 0.5f * theta/PI;
			float endRadius = 0.7f;
			float end = smoothStep(endRadius, endRadius + 0.005f, norm(vec2(0.715f + endRadius, 0.5f) - uv));
			float shade = fract(exp_r + loop);

			result.rgb = mix(orange*0.25f, white, shade*shade);
			result.isWritten = (smoothStep(0.98f, 1.f, exp_r + loop) * end > 0.f);
			result.orichalcum = smoothStep(0.f, 0.05f, shade) * smoothStep(0.2f, 0.15f, shade);
			return result;
		}

		static PictureColor bivalveShell(const vector2f& uv)
		{
			vector3f lightBlue = { 0.459f, 0.672f, 0.948f };
			vector3f darkBlue = { 0.001f, 0.080f, 0.335f };

			vector2f ray = vec2(0.5f, 0.5f) - uv;
			float r = norm(ray);
			float theta = msys_atan2f(ray.x, -ray.y) + PI;
			float a = 0.5f * theta / PI;
			float lines = 1.f - Algebra::abs(fract(15.f * a) * 2.f - 1.f);
			lines = 1.f - lines * lines;

			vector2f box = Algebra::abs(vec2(0.5f, 0.59f) - uv);
			float square = msys_max(box.x, box.y);

			PictureColor result;
			result.rgb = mix(darkBlue, lightBlue, mix(1.f, lines, smoothStep(0.05f, 0.4f, r)));
			result.isWritten = (smoothStep(0.09f, 0.08f, square) + smoothStep(0.4f, 0.39f, r - 0.03f * lines) * smoothStep(0.3f, 0.31f, a) * smoothStep(0.7f, 0.69f, a) > 0.f);
			result.orichalcum = 0.f;

			return result;
		}

		static PictureColor waveFrieze(const vector2f& uv)
		{
			vector3f lightBlue = { 0.459f, 0.672f, 0.948f };
			vector3f blue = { 0.044f, 0.235f, 0.619f };
			vector3f darkBlue = { 0.001f, 0.080f, 0.335f };

			vector2f p = fract(10.f * uv) - vec2(0.5f);
			float r = 2.f * norm(p);
			float theta = msys_atan2f(p.y, p.x);
			theta += 3.f * PI * clamp(1.f - r, 0.f, 1.f);
			p = vec2(r*msys_cosf(theta), r*msys_sinf(theta));

			PictureColor result;
			result.rgb = mix(darkBlue, lightBlue, smoothStep(-0.05f, 0.05f, p.y));
			result.isWritten = true;
			result.orichalcum = 0.f;
			return result;
		}
	
		static PictureColor picture(const vector2f& uv)
		{
			PictureColor result = sea(uv * 1.5f, 17.f, 5.f);
			if (!result.isWritten)
			{
				result = sun(uv);
			}
			else
			{
				if (uv.y < 0.33f)
				{
					Noise::Rand rand(Noise::Hash::get32(floor(6.f * uv)));
					vector2f randomOffset = { 0.15f * rand.sfgen(), 0.5f * rand.fgen() };
					vector2f smalluv = fract(6.f * uv) + randomOffset;
					PictureColor things = { false };
					if (rand.igen() % 4 > 0)
					{
						things = bivalveShell(smalluv);
					}
					if (things.isWritten)
					{
						result = things;
					}
				}

				if (uv.y < 0.4f)
				{
					Noise::Rand rand(Noise::Hash::get32(floor(5.f * uv)));
					vector2f randomOffset = { rand.fgen(), rand.fgen() };

					vector2f smalluv = 1.5f * fract(5.f * uv) - 0.5f * randomOffset;
					PictureColor things = { false };
					if (rand.igen() % 3 > 0)
					{
						things = starFish(smalluv, (rand.igen() % 2 == 0 ? 5 : 7), rand.fgen());
					}
					if (things.isWritten)
					{
						result = things;
					}
				}

				{
					Noise::Rand rand(Noise::Hash::get32(floor(3.f * uv)));
					vector2f randomOffset = { mix(-0.18f, 0.18f, rand.fgen()), mix(-0.25f, 0.08f, rand.fgen()) };
					vector2f smalluv = fract(3.f * uv) + randomOffset;
					PictureColor things = { false };
					if (rand.igen() % 3 == 0)
					{
						things = snailShell(smalluv);
					}
					if (things.isWritten)
					{
						result = things;
					}
				}
			}
			return result;
		}

		static void mosaic(void* out_pixelData, const texture::InputPixelInfo& in)
		{
			vector3f orichalcumColor = { 1.f, 0.3f, 0.2f };

#if 1
			const float tiles = 120.f;
			const Noise::VoronoiInfo voronoi = Noise::WorleyNoise::value(in.uv * tiles, tiles, 1.f, 1.f);
			const float hash = voronoi.GetHash();
			float isGlass = smoothStep(0.f, 0.15f, voronoi.edge);
			vector2f uv2 = voronoi.pos / tiles;
#else
			const float hash = 0.f;
			float isGlass = 1.f;
			vector2f uv2 = uv;
#endif

			PictureColor pictureColor = picture(uv2);
			pictureColor.orichalcum = picture(in.uv).orichalcum;

#if 1
			const vector3f tones = { 7.f, 10.f, 20.f };
			vector3f palette = div(floor(mul(pictureColor.rgb, tones)), tones);
#else
			vector3f palette = pictureColor.rgb;
#endif

#if 1
			float colorDamage = mix(hash, Noise::PerlinNoise::value(in.uv * 3.f, 3.f) * 0.5f + 0.5f, 0.25f);
			float luma = dot(palette, vec3(0.2126f, 0.7152f, 0.0722f));
			palette = mix(palette, vec3(luma), mix(0.2f, 0.8f, colorDamage));

			float physicalDamage = (Noise::PerlinNoise::value(uv2 * 2.f, 2.f) * 1.2f +
				Noise::PerlinNoise::value(uv2 * 4.f, 4.f) +
				Noise::PerlinNoise::value(uv2 * 8.f, 8.f)) / 3.2f;
			physicalDamage = smoothStep(0.25f, 0.28f, physicalDamage);
			palette = mix(palette, vec3(mix(0.32f, 0.42f, hash)), physicalDamage);
#else
			float physicalDamage = 0.f;
#endif

#if 1
			float surfaceDeformation = Noise::PerlinNoise::value(in.uv * 3.f, 3.f) * 0.5f + 0.5f;
#else
			float surfaceDeformation = 0.f;
#endif

			float tilesHeight = mix(0.1f + isGlass * 0.1f, 0.f, physicalDamage);
			float orichalcumHeight = mix(0.2f, 0.215f, Noise::Hash::get32f(in.uv));

			pixelFormat result;
			result.height = 0.7f * surfaceDeformation + mix(tilesHeight, orichalcumHeight, interpolate(0.f, 0.5f, pictureColor.orichalcum));
			result.diffuseColor = mix(palette, vector3f::zero, pictureColor.orichalcum);
			result.specularColor = mix(vec3(0.04f), orichalcumColor, interpolate(0.5f, 1.f, pictureColor.orichalcum));
			result.opacity = 1.f;
			result.roughness = mix(mix(mix(0.95f, 0.7f, hash*hash), 1.f, physicalDamage), 0.5f, pictureColor.orichalcum);
			*(static_cast<pixelFormat*>(out_pixelData)) = result;
		}
	};

	dest.ApplyFunction(local::mosaic);

	const size_t heightOffset = (float*)&(((pixelFormat*)NULL)->height) - (float*)NULL;
	const int normalOffset = (float*)&(((pixelFormat*)NULL)->normal) - (float*)NULL;
	texture::GenerateNormalMap(dest, heightOffset, normalOffset, s_parallaxScale);
}
