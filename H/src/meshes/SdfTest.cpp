#include "SdfTest.hh"

#include "engine/algebra/Interpolation.hxx"
#include "engine/algebra/Matrix.hxx"
#include "engine/algebra/Spline.hh"
#include "engine/algebra/Utils.hh"
#include "engine/algebra/Vector2.hxx"
#include "engine/container/Utils.hh"
#include "engine/mesh/Cube.hh"
#include "engine/mesh/Marching.hh"
#include "engine/mesh/Mesh.hh"
#include "engine/mesh/MeshPool.hh"
#include "engine/mesh/Revolution.hh"
#include "tool/mesh/VertexDataPNT.hh"

#ifdef ENABLE_RUNTIME_COMPILATION
#include "RuntimeCompiledCPlusPlus/RuntimeObjectSystem/ObjectInterfacePerModule.h"

REGISTERCLASS(SdfTest);
ObjectId SdfTest::objectId;
SdfTest* SdfTest::instance = NULL;
void SdfTest::BuildMesh(mesh::Mesh& dest, mesh::Pool& pool)
{
	return instance->Build(dest, pool);
}
#endif

using namespace Algebra;
using namespace mesh;

// The map of the temple of Artemis, by J T Wood, is in imperial units.
// https://upload.wikimedia.org/wikipedia/commons/8/85/Artemistempleplan.jpg
#define FOOT_TO_METER 0.3048f
#define INCH_TO_METER 0.0254f


// http://iquilezles.org/www/articles/smin/smin.htm
static float smin(float a, float b, float k)
{
	float h = Algebra::clamp(0.5f + 0.5f * (b - a) / k, 0.0f, 1.0f);
	return mix(b, a, h) - k * h * (1.0f - h);
}

// http://iquilezles.org/www/articles/distfunctions/distfunctions.htm
static float sdSphere(Algebra::vector3f p, float radius)
{
	return Algebra::norm(p) - radius;
}

static float sdTorus(Algebra::vector3f p, Algebra::vector2f t)
{
	Algebra::vector2f xz = vec2(p.x, p.z);
	Algebra::vector2f q = vec2(Algebra::norm(xz) - t.x, p.y);
	return Algebra::norm(q) - t.y;
}

static float sdf(float x, float y, float z)
{
	// proof of concept
	Algebra::vector3f p = { x, y, z };
	Algebra::vector3f c0 = { 0.f, 3.f, 0.f };
	Algebra::vector3f c1 = { 3.f, 3.f, 0.f };
	return msys_max(sdTorus(p - c0, vec2(2.f, 1.f)), -sdSphere(p - c1, 1.f));
}


#ifdef ENABLE_RUNTIME_COMPILATION
void SdfTest::Build(Mesh& dest, Pool& /*pool*/)
#else // !ENABLE_RUNTIME_COMPILATION
void SdfTest(Mesh& dest, Pool& /*pool*/)
#endif // !ENABLE_RUNTIME_COMPILATION
{
	Marching(sdf).GenerateMesh(dest, -10.f, -10.f, -10.f, 10.f, 10.f, 10.f, 30, 30, 30);
}
