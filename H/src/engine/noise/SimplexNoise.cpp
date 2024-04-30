#include "SimplexNoise.hh"

#include "Gradient.hh"
#include "engine/algebra/Functions.hh"
#include "engine/algebra/Vector2.hxx"
#include "engine/algebra/Vector3.hxx"
#include "engine/core/msys_temp.hh"

using namespace Algebra;
using namespace Noise;

const float squaresToTriangles = (3.0f - msys_sqrtf(3.0f)) / 6.0f;
const float trianglesToSquares = (msys_sqrtf(3.0f) - 1.0f) / 2.0f;

const float simplexScale2D = 2916.0f * msys_sqrtf(2.0f) / 125.0f;
const float simplexScale3D = 8192.0f * msys_sqrtf(3.0f) / 375.0f;

const int xaValues3D[] = { 0, 0, 0, 0, 0, 0, 1, 1 };
const int yaValues3D[] = { 0, 1, 0, 1, 0, 0, 0, 0 };
const int zaValues3D[] = { 1, 0, 1, 0, 1, 1, 0, 0 };
const int xbValues3D[] = { 0, 0, 0, 1, 1, 1, 1, 1 };
const int ybValues3D[] = { 1, 1, 1, 1, 0, 0, 0, 1 };
const int zbValues3D[] = { 1, 1, 1, 0, 1, 1, 1, 0 };

float simplex2DPart(vector2f p, int x0, int y0)
{
	const float unskew = (x0 + y0) * squaresToTriangles;
	const vector2f p0 = { float(x0), float(y0) };
	const vector2f dp = p - p0 + unskew;
	const float f = msys_max(0.0f, 0.5f - dot(dp, dp));

	const vector2f g = Gradient::get(x0, y0);
	return f * f * f * dot(g, dp);
}

float simplex3DPart(vector3f p, int x0, int y0, int z0)
{
	const float unskew = (x0 + y0 + z0) * (1.0f / 6.0f);
	const vector3f p0 = { float(x0), float(y0), float(z0) };
	const vector3f dp = p - p0 + unskew;
	const float f = msys_max(0.0f, 0.5f - dot(dp, dp));

	const vector3f g = Gradient::getInSimplex(x0, y0, z0);
	return f * f * f * dot(g, dp);
}

float SimplexNoise::value(float p, float period)
{
	const int x0 = msys_ifloorf(p);
	const int x1 = x0 + 1;
	const float dp0 = p - x0;
	const float dp1 = p - x1;
	const float f0 = 1.0f - dp0 * dp0;
	const float f1 = 1.0f - dp1 * dp1;

	const int x0_periodic = msys_ifloorf(Algebra::mod(p, period));
	const int x1_periodic = msys_ifloorf(Algebra::mod(x0_periodic + 1.f, period));
	const float g0 = Gradient::get(x0_periodic);
	const float g1 = Gradient::get(x1_periodic);

	const float sample = (f0 * f0 * f0 * g0 * dp0) + (f1 * f1 * f1 * g1 * dp1);

	return sample * (64.0f / 27.0f);
}

float SimplexNoise::value(const vector2f& p)
{
	const vector2f sp = p + (p.x + p.y) * trianglesToSquares;
	const int x0 = msys_ifloorf(sp.x);
	const int y0 = msys_ifloorf(sp.y);
	const int x1 = x0 + 1;
	const int y1 = y0 + 1;

	const int xa = (sp.x - x0 >= sp.y - y0 ? x1 : x0);
	const int ya = (sp.x - x0 >= sp.y - y0 ? y0 : y1);

	const float sample = (simplex2DPart(p, x0, y0) +
						  simplex2DPart(p, x1, y1) +
						  simplex2DPart(p, xa, ya));

	return sample * simplexScale2D;
}

float SimplexNoise::value(const vector3f& p)
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

	const float sample = (simplex3DPart(p, x0, y0, z0) +
						  simplex3DPart(p, x1, y1, z1) +
						  simplex3DPart(p, xa, ya, za) +
						  simplex3DPart(p, xb, yb, zb));

	return sample * simplexScale3D;
}
