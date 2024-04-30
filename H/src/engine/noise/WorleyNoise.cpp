#include "WorleyNoise.hh"

#include "Hash.hh"
#include "Rand.hh"
#include "engine/algebra/Functions.hh"
#include "engine/algebra/Interpolation.hxx"
#include "engine/algebra/Vector2.hxx"
#include "engine/core/msys_temp.hh"

using namespace Algebra;
using namespace Noise;

VoronoiInfo WorleyNoise::value(const vector2f& p, float period, float randomness, float norm)
{
	vector2f p0 = floor(p);
	VoronoiInfo result = { 1e30f, 1e30f, 1e30f, 1e30f, 0, vector2f::zero, 1e30f};

	for (int j = -1; j <= 1; ++j)
	{
		for (int i = -1; i <= 1; ++i)
		{
			vector2f p_i = { float(i), float(j) };
			p_i += p0;

			vector2f p_i_periodic = Algebra::mod(p_i, period);
			if (p_i_periodic.x < 0.f)
			{
				p_i_periodic.x += period;
			}
			if (p_i_periodic.y < 0.f)
			{
				p_i_periodic.y += period;
			}
			unsigned int hash = Hash::get32(p_i_periodic);
			Rand rand(hash);
			vector2f center = { 0.5f, 0.5f };
			vector2f randomOrigin = { rand.fgen(), rand.fgen() };
			vector2f cellOrigin = p_i + mix(center, randomOrigin, randomness);

			vector2f delta = abs(cellOrigin - p);
			float dMahattan = delta.x + delta.y;
			float dEuclid = Algebra::norm(delta);
			float dChebychev = msys_max(delta.x, delta.y);

			float d = 0.f;
			if (norm <= 1.f)
			{
				d = mix(dMahattan, dEuclid, norm);
			}
			else if (norm <= 2.f)
			{
				d = mix(dEuclid, dChebychev, norm - 1.f);
			}
			else if (norm <= 3.f)
			{
				d = mix(dChebychev, dMahattan, norm - 2.f);
			}

			if (d < result.f1)
			{
				result.f4 = result.f3;
				result.f3 = result.f2;
				result.f2 = result.f1;
				result.f1 = d;
				result.id = hash;
				result.pos = cellOrigin;
			}
			else if (d < result.f2)
			{
				result.f4 = result.f3;
				result.f3 = result.f2;
				result.f2 = d;
			}
			else if (d < result.f3)
			{
				result.f4 = result.f3;
				result.f3 = d;
			}
			else if (d < result.f4)
			{
				result.f4 = d;
			}
		}
	}

	// Compute edges
	// Based on https://www.shadertoy.com/view/llG3zy
	// (Faster Voronoi Edge Distance - Tomasz Dobrowolski)
	for (int j = -1; j <= 1; ++j)
	{
		for (int i = -1; i <= 1; ++i)
		{
			vector2f p_i = { float(i), float(j) };
			p_i += p0;

			vector2f p_i_periodic = Algebra::mod(p_i, period);
			if (p_i_periodic.x < 0.f)
			{
				p_i_periodic.x += period;
			}
			if (p_i_periodic.y < 0.f)
			{
				p_i_periodic.y += period;
			}
			unsigned int hash = Hash::get32(p_i_periodic);
			Rand rand(hash);
			vector2f center = { 0.5f, 0.5f };
			vector2f randomOrigin = { rand.fgen(), rand.fgen() };

			vector2f cellOrigin = p_i + mix(center, randomOrigin, randomness);
			vector2f r = cellOrigin - p;
			vector2f mr = result.pos - p;
			if (Algebra::norm(mr - r) > 0.001f)
			{
				float d = Algebra::dot(0.5f*(mr + r), normalized(r - mr));
				result.edge = msys_min(result.edge, d);
			}
		}
	}
	return result;
}
