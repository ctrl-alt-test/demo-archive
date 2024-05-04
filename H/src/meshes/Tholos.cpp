#include "Tholos.hh"

#include "GreekColumn.hh"
#include "engine/algebra/Functions.hh"
#include "engine/algebra/Interpolation.hxx"
#include "engine/algebra/Matrix.hxx"
#include "engine/algebra/Spline.hh"
#include "engine/algebra/Utils.hh"
#include "engine/algebra/Vector2.hxx"
#include "engine/container/Utils.hh"
#include "engine/mesh/Cube.hh"
#include "engine/mesh/Mesh.hh"
#include "engine/mesh/MeshPool.hh"
#include "engine/mesh/Revolution.hh"
#include "engine/mesh/Utils.hh"
#include "Artemision.hh"

#define ENABLE_THOLOS_MESH 1

#ifdef ENABLE_RUNTIME_COMPILATION
#include "RuntimeCompiledCPlusPlus/RuntimeObjectSystem/ObjectInterfacePerModule.h"

REGISTERCLASS(Tholos);
ObjectId Tholos::objectId;
Tholos* Tholos::instance = NULL;
void Tholos::BuildMesh(mesh::Mesh& dest, mesh::Pool& pool)
{
	return instance->Build(dest, pool);
}
#endif

using namespace Algebra;
using namespace mesh;

#if ENABLE_THOLOS_MESH

static void tholos(Mesh& dest, Pool& pool)
{
	// Inspired from http://www.penhook.org/tholos_at_epidaurus.htm

	float baseCurve[] = {
		0, 0, 60,
		0, 30, 60,
		0, 30, 57,
		0, 60, 57,
		0, 60, 54,
		0, 90, 54,
		0, 90, 0,
	};
	initSplineData(baseCurve, ARRAY_LEN(baseCurve), 3);
	Timeline::Variable v(baseCurve, 3, ARRAY_LEN(baseCurve) / 3, Timeline::modeSpline);
	Mesh& base = pool.GetTempMesh();
	Revolution(NULL, NULL, NULL, &v).GenerateMesh(base, 6, 36);
	base.Scale(1.f / 7.f, 1.f / 70.f, 1.f / 7.f);
	base.sharpenEdges();
	dest.add(base);

	float roof[] = {
		0, 120, 0,
		0, 70, 40,
		0, 0, 45,
		0, 0, 50,
		0, 10, 60,
		0, 60, 60,
		0, 80, 55,
		0, 95, 55,
		0, 120, 54,
		0, 210, 30,
		0, 250, 0,
	};
	initSplineData(roof, ARRAY_LEN(roof), 3);
	Timeline::Variable v2(roof, 3, ARRAY_LEN(roof) / 3, Timeline::modeSpline);
	Mesh& top = pool.GetTempMesh();
	Revolution(NULL, NULL, NULL, &v2).GenerateMesh(top, 21, 32);
	top.Scale(9.25f / 75.f, 0.85f / 75.f, 9.25f / 75.f);

	dest.add(top, matrix4::translation(0.f, 6.3f, 0.f));

	dest.ComputeNormals();
	GenerateAxisAlignedTextureCoordinates(dest, 0.25f);

	Mesh& hiPolyColumn = pool.GetTempMesh();
	mesh::IonicColumn(hiPolyColumn, pool, 12, 10, 6);
	hiPolyColumn.Scale(5.f);
	hiPolyColumn.ComputeNormals();

	for (int i = 0; i < 26; i++)
	{
		float angle = PI * 2.f * i / 26.f;
		float x = 6.5f * msys_cosf(angle);
		float z = 6.5f * msys_sinf(angle);
		dest.add(hiPolyColumn, matrix4::translation(x, 1.28f, z).rotate(-angle, vector3f::uy));
	}
}

#endif // ENABLE_THOLOS_MESH

#ifdef ENABLE_RUNTIME_COMPILATION
void Tholos::Build(Mesh& dest, Pool& pool)
#else // !ENABLE_RUNTIME_COMPILATION
void Tholos(Mesh& dest, Pool& pool)
#endif // !ENABLE_RUNTIME_COMPILATION
{
#if ENABLE_THOLOS_MESH
	tholos(dest, pool);
	dest.ComputeTangents();
#endif // ENABLE_THOLOS_MESH
}
