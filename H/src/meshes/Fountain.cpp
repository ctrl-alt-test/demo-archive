#include "Fountain.hh"

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

#define ENABLE_FOUNTAIN_MESH 1

#ifdef ENABLE_RUNTIME_COMPILATION
#include "RuntimeCompiledCPlusPlus/RuntimeObjectSystem/ObjectInterfacePerModule.h"

REGISTERCLASS(Fountain);
ObjectId Fountain::objectId;
Fountain* Fountain::instance = NULL;
void Fountain::BuildMesh(mesh::Mesh& dest, mesh::Pool& pool)
{
	return instance->Build(dest, pool);
}
#endif

using namespace Algebra;
using namespace mesh;

#if ENABLE_FOUNTAIN_MESH

static void fountain(Mesh& dest, Pool& pool)
{
	// Inspired from https://en.wikipedia.org/wiki/File:Diaf%C3%A1ni_%E2%80%93_Fountain_of_Neptune_-_1.jpg

	static float baseCurve[] = {
		0, 0, 60,
		0, 5, 60,
		0, 6, 57,
		0, 9, 57,
		0, 12, 54,
		0, 30, 54,
		0, 34, 60,
		0, 38, 60,
		0, 38, 55,
		0, 37, 50,
		0, 10, 50,
		0, 10, 0,
	};
	initSplineData(baseCurve, ARRAY_LEN(baseCurve), 3);
	Timeline::Variable v(baseCurve, 3, ARRAY_LEN(baseCurve) / 3, Timeline::modeSpline);
	Mesh& base = pool.GetTempMesh();
	Revolution(NULL, NULL, NULL, &v).GenerateMesh(base, 30, -12);
	dest.add(base);

	static float insideColumn[] = {
		0, 0, 60,
		0, 20, 12,
		0, 42, 12,
		0, 42, 16,
		0, 47, 12,
		0, 47, 15,
		0, 49, 15,
		0, 49, 17,
		0, 61, 22,
		0, 63, 25,
		0, 66, 24,
		0, 72, 26,
		0, 72, 16,
		0, 85, 0,
	};
	initSplineData(insideColumn, ARRAY_LEN(insideColumn), 3);
	Timeline::Variable v2(insideColumn, 3, ARRAY_LEN(insideColumn) / 3, Timeline::modeSpline);
	Mesh& col = pool.GetTempMesh();
	Revolution(NULL, NULL, NULL, &v2).GenerateMesh(col, 40, 12);
	dest.add(col);
	dest.Scale(1.f / 30.f);
}

#endif // ENABLE_FOUNTAIN_MESH

#ifdef ENABLE_RUNTIME_COMPILATION
void Fountain::Build(Mesh& dest, Pool& pool)
#else // !ENABLE_RUNTIME_COMPILATION
void Fountain(Mesh& dest, Pool& pool)
#endif // !ENABLE_RUNTIME_COMPILATION
{
#if ENABLE_FOUNTAIN_MESH
	fountain(dest, pool);

	dest.ComputeNormals();
	dest.ComputeTangents();
#endif // ENABLE_FOUNTAIN_MESH
}
