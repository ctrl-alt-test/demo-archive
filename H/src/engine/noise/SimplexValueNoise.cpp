#include "SimplexValueNoise.hh"

#include "Hash.hh"
#include "engine/algebra/Functions.hh"
#include "engine/algebra/Vector2.hxx"
#include "engine/algebra/Vector3.hxx"
#include "engine/core/msys_temp.hh"

using namespace Algebra;
using namespace Noise;

const float squaresToTriangles = (3.0f - msys_sqrtf(3.0f)) / 6.0f;
const float trianglesToSquares = (msys_sqrtf(3.0f) - 1.0f) / 2.0f;

const int xaValues3D[] = { 0, 0, 0, 0, 0, 0, 1, 1 };
const int yaValues3D[] = { 0, 1, 0, 1, 0, 0, 0, 0 };
const int zaValues3D[] = { 1, 0, 1, 0, 1, 1, 0, 0 };
const int xbValues3D[] = { 0, 0, 0, 1, 1, 1, 1, 1 };
const int ybValues3D[] = { 1, 1, 1, 1, 0, 0, 0, 1 };
const int zbValues3D[] = { 1, 1, 1, 0, 1, 1, 1, 0 };

float simplexValue3DPart(vector3f p, int x0, int y0, int z0)
{
	const float unskew = (x0 + y0 + z0) * (1.0f / 6.0f);
	const vector3f p0 = { float(x0), float(y0), float(z0) };
	const vector3f dp = p - p0 + unskew;
	const float f = msys_max(0.0f, 0.5f - dot(dp, dp));

	const int h = Hash::get8(x0, y0, z0);
	return f * f * f * h;
}

float SimplexValueNoise::value(float p, float period)
{
	const int x0 = msys_ifloorf(p);
	const int x1 = x0 + 1;
	const float dx0 = p - x0;
	const float dx1 = p - x1;
	const float f0 = 1.0f - dx0 * dx0;
	const float f1 = 1.0f - dx1 * dx1;

	const int x0_periodic = msys_ifloorf(Algebra::mod(p, period));
	const int x1_periodic = msys_ifloorf(Algebra::mod(x0_periodic + 1.f, period));
	const int h0 = Hash::get8(x0_periodic);
	const int h1 = Hash::get8(x1_periodic);

	const float sample = (f0 * f0 * f0 * h0) + (f1 * f1 * f1 * h1);

	return sample * (2.0f / Hash::mask) - 1.0f;
}

float SimplexValueNoise::value(const vector2f& p)
{
	const vector2f sp = p + (p.x + p.y) * trianglesToSquares;
	const int x0 = msys_ifloorf(sp.x);
	const int y0 = msys_ifloorf(sp.y);
	const int x1 = x0 + 1;
	const int y1 = y0 + 1;

	const int xa = (sp.x - x0 >= sp.y - y0 ? x1 : x0);
	const int ya = (sp.x - x0 >= sp.y - y0 ? y0 : y1);

	const int h0 = Hash::get8(x0, y0);
	const int h1 = Hash::get8(x1, y1);
	const int ha = Hash::get8(xa, ya);

	const vector2f p0 = { float(x0), float(y0) };
	const vector2f dp0 = p - p0 + (x0 + y0) * squaresToTriangles;
	const float f0 = msys_max(0.0f, 0.5f - dot(dp0, dp0));

	const vector2f p1 = { float(x1), float(y1) };
	const vector2f dp1 = p - p1 + (x1 + y1) * squaresToTriangles;
	const float f1 = msys_max(0.0f, 0.5f - dot(dp1, dp1));

	const vector2f pa = { float(xa), float(ya) };
	const vector2f dpa = p - pa + (xa + ya) * squaresToTriangles;
	const float fa = msys_max(0.0f, 0.5f - dot(dpa, dpa));

	const float sample = (f0 * f0 * f0 * h0) + (f1 * f1 * f1 * h1) + (fa * fa * fa * ha);

	return sample * (8.0f * 2.0f / Hash::mask) - 1.0f;
}

float SimplexValueNoise::value(const vector3f& p)
{
	const vector3f sp = p + (p.x + p.y + p.z) * (1.0f / 3.0f);
	const int x0 = msys_ifloorf(sp.x);
	const int y0 = msys_ifloorf(sp.y);
	const int z0 = msys_ifloorf(sp.z);
	const int x1 = x0 + 1;
	const int y1 = y0 + 1;
	const int z1 = z0 + 1;

	const float x = sp.x - x0;
	const float y = sp.y - y0;
	const float z = sp.z - z0;
	const int selector = (x >= y ? 4 : 0) + (x >= z ? 2 : 0) + (y >= z ? 1 : 0);

	const int xa = x0 + (x1 - x0) * xaValues3D[selector];
	const int ya = y0 + (y1 - y0) * yaValues3D[selector];
	const int za = z0 + (z1 - z0) * zaValues3D[selector];
	const int xb = x0 + (x1 - x0) * xbValues3D[selector];
	const int yb = y0 + (y1 - y0) * ybValues3D[selector];
	const int zb = z0 + (z1 - z0) * zbValues3D[selector];

	const float sample = (simplexValue3DPart(p, x0, y0, z0) +
						  simplexValue3DPart(p, x1, y1, z1) +
						  simplexValue3DPart(p, xa, ya, za) +
						  simplexValue3DPart(p, xb, yb, zb));

	return sample * (8.0f * 2.0f / Hash::mask) - 1.0f;
}
