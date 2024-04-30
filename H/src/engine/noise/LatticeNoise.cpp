#include "LatticeNoise.hh"

#include "Hash.hh"
#include "engine/algebra/Functions.hh"
#include "engine/core/msys_temp.hh"

using namespace Noise;

float LatticeNoise::value(float p, float period)
{
	const int x0_periodic = msys_ifloorf(Algebra::mod(p, period));
	const int h = Hash::get8(x0_periodic);

	return h * (2.0f / Hash::mask) - 1.0f;
}

float LatticeNoise::value(const Algebra::vector2f& p, float period)
{
	const int x0_periodic = msys_ifloorf(Algebra::mod(p.x, period));
	const int y0_periodic = msys_ifloorf(Algebra::mod(p.y, period));
	const int h = Hash::get8(x0_periodic, y0_periodic);

	return h * (2.0f / Hash::mask) - 1.0f;
}

float LatticeNoise::value(const Algebra::vector3f& p, float period)
{
	const int x0_periodic = msys_ifloorf(Algebra::mod(p.x, period));
	const int y0_periodic = msys_ifloorf(Algebra::mod(p.y, period));
	const int z0_periodic = msys_ifloorf(Algebra::mod(p.z, period));
	const int h = Hash::get8(x0_periodic, y0_periodic, z0_periodic);

	return h * (2.0f / Hash::mask) - 1.0f;
}

float LatticeNoise::value(const Algebra::vector4f& p, float period)
{
	const int x0_periodic = msys_ifloorf(Algebra::mod(p.x, period));
	const int y0_periodic = msys_ifloorf(Algebra::mod(p.y, period));
	const int z0_periodic = msys_ifloorf(Algebra::mod(p.z, period));
	const int w0_periodic = msys_ifloorf(Algebra::mod(p.w, period));
	const int h = Hash::get8(x0_periodic, y0_periodic, z0_periodic, w0_periodic);

	return h * (2.0f / Hash::mask) - 1.0f;
}
