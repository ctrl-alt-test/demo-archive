#include "ValueNoise.hh"

#include "Hash.hh"
#include "engine/algebra/Functions.hh"
#include "engine/algebra/Interpolation.hxx"
#include "engine/core/msys_temp.hh"

using namespace Algebra;
using namespace Noise;

float ValueNoise::value(float p, float period)
{
	const int x0_periodic = msys_ifloorf(Algebra::mod(p, period));
	const int x1_periodic = msys_ifloorf(Algebra::mod(x0_periodic + 1.f, period));
	const int h0 = Hash::get8(x0_periodic);
	const int h1 = Hash::get8(x1_periodic);

	const int x0 = msys_ifloorf(p);
	const float dx = smoothStep(p - x0);
	return mix(h0, h1, dx)
		* (2.0f / Hash::mask) - 1.0f;
}

float ValueNoise::value(const vector2f& p, float period)
{
	const int x0_periodic = msys_ifloorf(Algebra::mod(p.x, period));
	const int y0_periodic = msys_ifloorf(Algebra::mod(p.y, period));
	const int x1_periodic = msys_ifloorf(Algebra::mod(x0_periodic + 1.f, period));
	const int y1_periodic = msys_ifloorf(Algebra::mod(y0_periodic + 1.f, period));
	const int h00 = Hash::get8(x0_periodic, y0_periodic);
	const int h01 = Hash::get8(x1_periodic, y0_periodic);
	const int h10 = Hash::get8(x0_periodic, y1_periodic);
	const int h11 = Hash::get8(x1_periodic, y1_periodic);

	const int x0 = msys_ifloorf(p.x);
	const int y0 = msys_ifloorf(p.y);
	const float dx = smoothStep(p.x - x0);
	const float dy = smoothStep(p.y - y0);
	return mix(mix(h00,
				   h01, dx),
			   mix(h10,
				   h11, dx), dy)
		* (2.0f / Hash::mask) - 1.0f;
}

float ValueNoise::value(const vector3f& p, float period)
{
	const int x0_periodic = msys_ifloorf(Algebra::mod(p.x, period));
	const int y0_periodic = msys_ifloorf(Algebra::mod(p.y, period));
	const int z0_periodic = msys_ifloorf(Algebra::mod(p.z, period));
	const int x1_periodic = msys_ifloorf(Algebra::mod(x0_periodic + 1.f, period));
	const int y1_periodic = msys_ifloorf(Algebra::mod(y0_periodic + 1.f, period));
	const int z1_periodic = msys_ifloorf(Algebra::mod(z0_periodic + 1.f, period));
	const int h000 = Hash::get8(x0_periodic, y0_periodic, z0_periodic);
	const int h001 = Hash::get8(x1_periodic, y0_periodic, z0_periodic);
	const int h010 = Hash::get8(x0_periodic, y1_periodic, z0_periodic);
	const int h011 = Hash::get8(x1_periodic, y1_periodic, z0_periodic);
	const int h100 = Hash::get8(x0_periodic, y0_periodic, z1_periodic);
	const int h101 = Hash::get8(x1_periodic, y0_periodic, z1_periodic);
	const int h110 = Hash::get8(x0_periodic, y1_periodic, z1_periodic);
	const int h111 = Hash::get8(x1_periodic, y1_periodic, z1_periodic);

	const int x0 = msys_ifloorf(p.x);
	const int y0 = msys_ifloorf(p.y);
	const int z0 = msys_ifloorf(p.z);
	const float dx = smoothStep(p.x - x0);
	const float dy = smoothStep(p.y - y0);
	const float dz = smoothStep(p.z - z0);
	return mix(mix(mix(h000,
					   h001, dx),
				   mix(h010,
					   h011, dx), dy),
			   mix(mix(h100,
					   h101, dx),
				   mix(h110,
					   h111, dx), dy), dz)
		* (2.0f / Hash::mask) - 1.0f;
}

float ValueNoise::value(const vector4f& p, float period)
{
	const int x0_periodic = msys_ifloorf(Algebra::mod(p.x, period));
	const int y0_periodic = msys_ifloorf(Algebra::mod(p.y, period));
	const int z0_periodic = msys_ifloorf(Algebra::mod(p.z, period));
	const int w0_periodic = msys_ifloorf(Algebra::mod(p.w, period));
	const int x1_periodic = msys_ifloorf(Algebra::mod(x0_periodic + 1.f, period));
	const int y1_periodic = msys_ifloorf(Algebra::mod(y0_periodic + 1.f, period));
	const int z1_periodic = msys_ifloorf(Algebra::mod(z0_periodic + 1.f, period));
	const int w1_periodic = msys_ifloorf(Algebra::mod(w0_periodic + 1.f, period));
	const int h0000 = Hash::get8(x0_periodic, y0_periodic, z0_periodic, w0_periodic);
	const int h0001 = Hash::get8(x1_periodic, y0_periodic, z0_periodic, w0_periodic);
	const int h0010 = Hash::get8(x0_periodic, y1_periodic, z0_periodic, w0_periodic);
	const int h0011 = Hash::get8(x1_periodic, y1_periodic, z0_periodic, w0_periodic);
	const int h0100 = Hash::get8(x0_periodic, y0_periodic, z1_periodic, w0_periodic);
	const int h0101 = Hash::get8(x1_periodic, y0_periodic, z1_periodic, w0_periodic);
	const int h0110 = Hash::get8(x0_periodic, y1_periodic, z1_periodic, w0_periodic);
	const int h0111 = Hash::get8(x1_periodic, y1_periodic, z1_periodic, w0_periodic);
	const int h1000 = Hash::get8(x0_periodic, y0_periodic, z0_periodic, w1_periodic);
	const int h1001 = Hash::get8(x1_periodic, y0_periodic, z0_periodic, w1_periodic);
	const int h1010 = Hash::get8(x0_periodic, y1_periodic, z0_periodic, w1_periodic);
	const int h1011 = Hash::get8(x1_periodic, y1_periodic, z0_periodic, w1_periodic);
	const int h1100 = Hash::get8(x0_periodic, y0_periodic, z1_periodic, w1_periodic);
	const int h1101 = Hash::get8(x1_periodic, y0_periodic, z1_periodic, w1_periodic);
	const int h1110 = Hash::get8(x0_periodic, y1_periodic, z1_periodic, w1_periodic);
	const int h1111 = Hash::get8(x1_periodic, y1_periodic, z1_periodic, w1_periodic);

	const int x0 = msys_ifloorf(p.x);
	const int y0 = msys_ifloorf(p.y);
	const int z0 = msys_ifloorf(p.z);
	const int w0 = msys_ifloorf(p.w);
	const float dx = smoothStep(p.x - x0);
	const float dy = smoothStep(p.y - y0);
	const float dz = smoothStep(p.z - z0);
	const float dw = smoothStep(p.w - w0);
	return mix(mix(mix(mix(h0000,
						   h0001, dx),
					   mix(h0010,
						   h0011, dx), dy),
				   mix(mix(h0100,
						   h0101, dx),
					   mix(h0110,
						   h0111, dx), dy), dz),
			   mix(mix(mix(h1000,
						   h1001, dx),
					   mix(h1010,
						   h1011, dx), dy),
				   mix(mix(h1100,
						   h1101, dx),
					   mix(h1110,
						   h1111, dx), dy), dz), dw)
		* (2.0f / Hash::mask) - 1.0f;
}
