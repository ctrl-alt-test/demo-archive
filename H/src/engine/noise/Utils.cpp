#include "Utils.hh"

#include "PerlinNoise.hh"
#include "WorleyNoise.hh"
#include "engine/algebra/Interpolation.hxx"
#include "engine/algebra/Vector2.hxx"

using namespace Noise;

float Noise::fBmPerlin(const Algebra::vector2f& p, float frequency, int octaves, float lacunarity, float invGain)
{
	float h = 0.f;
	float sum = 0.00001f;
	for (int i = 0; i < octaves; ++i)
	{
		h = invGain * h + Noise::PerlinNoise::value(p * frequency, frequency);
		sum = invGain * sum + 1.f;
		frequency *= lacunarity;
	}
	return h / sum;
}

float Noise::fBmWorley(const Algebra::vector2f& p, float frequency, int octaves, float lacunarity, float invGain, float minPeak)
{
	float h = 0.f;
	float sum = 0.00001f;
	for (int i = 0; i < octaves; ++i)
	{
		Noise::VoronoiInfo voronoi = Noise::WorleyNoise::value(p * frequency, frequency, 1.f, 2.5f);
		float hash = voronoi.GetHash();
		h = h * invGain + (voronoi.f2 - voronoi.f1) * Algebra::mix(minPeak, 1.f, hash);
		sum = sum * invGain + 1.f;
		frequency *= lacunarity;
	}
	return h / sum;
}
