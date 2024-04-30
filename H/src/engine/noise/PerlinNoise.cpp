#include "PerlinNoise.hh"

#include "Gradient.hh"
#include "engine/algebra/Functions.hh"
#include "engine/algebra/Interpolation.hxx"
#include "engine/algebra/Vector2.hxx"
#include "engine/algebra/Vector3.hxx"
#include "engine/algebra/Vector4.hxx"
#include "engine/core/msys_temp.hh"

// Improved Perlin noise.
// Original Perlin noise implementation can be found at:
// http://mrl.nyu.edu/~perlin/doc/oscar.html#noise

using namespace Algebra;
using namespace Noise;

const float sqrt2 = msys_sqrtf(2.f);

float PerlinNoise::value(float p, float period)
{
	const int x0 = msys_ifloorf(p);
	const int x1 = x0 + 1;
	const float dx0 = p - x0;
	const float dx1 = p - x1;

	const int x0_periodic = msys_ifloorf(Algebra::mod(p, period));
	const int x1_periodic = msys_ifloorf(Algebra::mod(x0_periodic + 1.f, period));
	const float b0 = Gradient::get(x0_periodic) * dx0;
	const float b1 = Gradient::get(x1_periodic) * dx1;

	const float dx = smootherStep(dx0);
	return mix(b0,
			   b1, dx) * 2.f;
}

float PerlinNoise::value(const vector2f& p, float period)
{
	const int x0 = msys_ifloorf(p.x);
	const int y0 = msys_ifloorf(p.y);
	const int x1 = x0 + 1;
	const int y1 = y0 + 1;
	const float dx0 = p.x - x0;
	const float dx1 = p.x - x1;
	const float dy0 = p.y - y0;
	const float dy1 = p.y - y1;
	const vector2f v00 = { dx0, dy0 };
	const vector2f v01 = { dx0, dy1 };
	const vector2f v10 = { dx1, dy0 };
	const vector2f v11 = { dx1, dy1 };

	const int x0_periodic = msys_ifloorf(Algebra::mod(p.x, period));
	const int y0_periodic = msys_ifloorf(Algebra::mod(p.y, period));
	const int x1_periodic = msys_ifloorf(Algebra::mod(x0_periodic + 1.f, period));
	const int y1_periodic = msys_ifloorf(Algebra::mod(y0_periodic + 1.f, period));
	const float b00 = dot(Gradient::get(x0_periodic, y0_periodic), v00);
	const float b01 = dot(Gradient::get(x0_periodic, y1_periodic), v01);
	const float b10 = dot(Gradient::get(x1_periodic, y0_periodic), v10);
	const float b11 = dot(Gradient::get(x1_periodic, y1_periodic), v11);

	const float dx = smootherStep(dx0);
	const float dy = smootherStep(dy0);
	return mix(mix(b00,
				   b01, dy),
			   mix(b10,
				   b11, dy), dx) * sqrt2;
}

float PerlinNoise::value(const vector3f& p, float period)
{
	const int x0 = msys_ifloorf(p.x);
	const int y0 = msys_ifloorf(p.y);
	const int z0 = msys_ifloorf(p.z);
	const int x1 = x0 + 1;
	const int y1 = y0 + 1;
	const int z1 = z0 + 1;
	const float dx0 = p.x - x0;
	const float dx1 = p.x - x1;
	const float dy0 = p.y - y0;
	const float dy1 = p.y - y1;
	const float dz0 = p.z - z0;
	const float dz1 = p.z - z1;
	const vector3f v000 = { dx0, dy0, dz0 };
	const vector3f v001 = { dx0, dy0, dz1 };
	const vector3f v010 = { dx0, dy1, dz0 };
	const vector3f v011 = { dx0, dy1, dz1 };
	const vector3f v100 = { dx1, dy0, dz0 };
	const vector3f v101 = { dx1, dy0, dz1 };
	const vector3f v110 = { dx1, dy1, dz0 };
	const vector3f v111 = { dx1, dy1, dz1 };

	const int x0_periodic = msys_ifloorf(Algebra::mod(p.x, period));
	const int y0_periodic = msys_ifloorf(Algebra::mod(p.y, period));
	const int z0_periodic = msys_ifloorf(Algebra::mod(p.z, period));
	const int x1_periodic = msys_ifloorf(Algebra::mod(x0_periodic + 1.f, period));
	const int y1_periodic = msys_ifloorf(Algebra::mod(y0_periodic + 1.f, period));
	const int z1_periodic = msys_ifloorf(Algebra::mod(z0_periodic + 1.f, period));
	const float b000 = dot(Gradient::getInCube(x0_periodic, y0_periodic, z0_periodic), v000);
	const float b001 = dot(Gradient::getInCube(x0_periodic, y0_periodic, z1_periodic), v001);
	const float b010 = dot(Gradient::getInCube(x0_periodic, y1_periodic, z0_periodic), v010);
	const float b011 = dot(Gradient::getInCube(x0_periodic, y1_periodic, z1_periodic), v011);
	const float b100 = dot(Gradient::getInCube(x1_periodic, y0_periodic, z0_periodic), v100);
	const float b101 = dot(Gradient::getInCube(x1_periodic, y0_periodic, z1_periodic), v101);
	const float b110 = dot(Gradient::getInCube(x1_periodic, y1_periodic, z0_periodic), v110);
	const float b111 = dot(Gradient::getInCube(x1_periodic, y1_periodic, z1_periodic), v111);

	const float dx = smootherStep(dx0);
	const float dy = smootherStep(dy0);
	const float dz = smootherStep(dz0);
	return mix(mix(mix(b000,
					   b001, dz),
				   mix(b010,
					   b011, dz), dy),
			   mix(mix(b100,
					   b101, dz),
				   mix(b110,
					   b111, dz), dy), dx);
}

//
// Noise function, returning the Perlin Noise at a given point
//
float PerlinNoise::value(const vector4f& p, float period)
{
	// The unit hypercube containing the point
	const int x0 = msys_ifloorf(p.x);
	const int y0 = msys_ifloorf(p.y);
	const int z0 = msys_ifloorf(p.z);
	const int w0 = msys_ifloorf(p.w);
	const int x1 = x0 + 1;
	const int y1 = y0 + 1;
	const int z1 = z0 + 1;
	const int w1 = w0 + 1;
	const float dx0 = p.x - x0;
	const float dx1 = p.x - x1;
	const float dy0 = p.y - y0;
	const float dy1 = p.y - y1;
	const float dz0 = p.z - z0;
	const float dz1 = p.z - z1;
	const float dw0 = p.w - w0;
	const float dw1 = p.w - w1;
	const vector4f v0000 = { dx0, dy0, dz0, dw0 };
	const vector4f v0001 = { dx0, dy0, dz0, dw1 };
	const vector4f v0010 = { dx0, dy0, dz1, dw0 };
	const vector4f v0011 = { dx0, dy0, dz1, dw1 };
	const vector4f v0100 = { dx0, dy1, dz0, dw0 };
	const vector4f v0101 = { dx0, dy1, dz0, dw1 };
	const vector4f v0110 = { dx0, dy1, dz1, dw0 };
	const vector4f v0111 = { dx0, dy1, dz1, dw1 };
	const vector4f v1000 = { dx1, dy0, dz0, dw0 };
	const vector4f v1001 = { dx1, dy0, dz0, dw1 };
	const vector4f v1010 = { dx1, dy0, dz1, dw0 };
	const vector4f v1011 = { dx1, dy0, dz1, dw1 };
	const vector4f v1100 = { dx1, dy1, dz0, dw0 };
	const vector4f v1101 = { dx1, dy1, dz0, dw1 };
	const vector4f v1110 = { dx1, dy1, dz1, dw0 };
	const vector4f v1111 = { dx1, dy1, dz1, dw1 };

	const int x0_periodic = msys_ifloorf(Algebra::mod(p.x, period));
	const int y0_periodic = msys_ifloorf(Algebra::mod(p.y, period));
	const int z0_periodic = msys_ifloorf(Algebra::mod(p.z, period));
	const int w0_periodic = msys_ifloorf(Algebra::mod(p.w, period));
	const int x1_periodic = msys_ifloorf(Algebra::mod(x0_periodic + 1.f, period));
	const int y1_periodic = msys_ifloorf(Algebra::mod(y0_periodic + 1.f, period));
	const int z1_periodic = msys_ifloorf(Algebra::mod(z0_periodic + 1.f, period));
	const int w1_periodic = msys_ifloorf(Algebra::mod(w0_periodic + 1.f, period));

	// The dot products of the 16 gradients and vectors
	const float b0000 = dot(Gradient::get(x0_periodic, y0_periodic, z0_periodic, w0_periodic), v0000);
	const float b0001 = dot(Gradient::get(x0_periodic, y0_periodic, z0_periodic, w1_periodic), v0001);
	const float b0010 = dot(Gradient::get(x0_periodic, y0_periodic, z1_periodic, w0_periodic), v0010);
	const float b0011 = dot(Gradient::get(x0_periodic, y0_periodic, z1_periodic, w1_periodic), v0011);
	const float b0100 = dot(Gradient::get(x0_periodic, y1_periodic, z0_periodic, w0_periodic), v0100);
	const float b0101 = dot(Gradient::get(x0_periodic, y1_periodic, z0_periodic, w1_periodic), v0101);
	const float b0110 = dot(Gradient::get(x0_periodic, y1_periodic, z1_periodic, w0_periodic), v0110);
	const float b0111 = dot(Gradient::get(x0_periodic, y1_periodic, z1_periodic, w1_periodic), v0111);
	const float b1000 = dot(Gradient::get(x1_periodic, y0_periodic, z0_periodic, w0_periodic), v1000);
	const float b1001 = dot(Gradient::get(x1_periodic, y0_periodic, z0_periodic, w1_periodic), v1001);
	const float b1010 = dot(Gradient::get(x1_periodic, y0_periodic, z1_periodic, w0_periodic), v1010);
	const float b1011 = dot(Gradient::get(x1_periodic, y0_periodic, z1_periodic, w1_periodic), v1011);
	const float b1100 = dot(Gradient::get(x1_periodic, y1_periodic, z0_periodic, w0_periodic), v1100);
	const float b1101 = dot(Gradient::get(x1_periodic, y1_periodic, z0_periodic, w1_periodic), v1101);
	const float b1110 = dot(Gradient::get(x1_periodic, y1_periodic, z1_periodic, w0_periodic), v1110);
	const float b1111 = dot(Gradient::get(x1_periodic, y1_periodic, z1_periodic, w1_periodic), v1111);

	// Then the interpolations, down to the result
	const float dx = smootherStep(dx0);
	const float dy = smootherStep(dy0);
	const float dz = smootherStep(dz0);
	const float dw = smootherStep(dw0);
	return mix(mix(mix(mix(b0000,
						   b0001, dw),
					   mix(b0010,
						   b0011, dw), dz),
				   mix(mix(b0100,
						   b0101, dw),
					   mix(b0110,
						   b0111, dw), dz), dy),
			   mix(mix(mix(b1000,
						   b1001, dw),
					   mix(b1010,
						   b1011, dw), dz),
				   mix(mix(b1100,
						   b1101, dw),
					   mix(b1110,
						   b1111, dw), dz), dy), dx);
}
