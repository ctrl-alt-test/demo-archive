#include "Lightning.hh"

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

#define ENABLE_LIGHTNING_MESH 1

#ifdef ENABLE_RUNTIME_COMPILATION
#include "RuntimeCompiledCPlusPlus/RuntimeObjectSystem/ObjectInterfacePerModule.h"

REGISTERCLASS(Lightning);
ObjectId Lightning::objectId;
Lightning* Lightning::instance = NULL;
void Lightning::BuildMesh(mesh::Mesh& dest, mesh::Pool& pool)
{
	return instance->Build(dest, pool);
}

REGISTERCLASS(SingleLightning);
ObjectId SingleLightning::objectId;
SingleLightning* SingleLightning::instance = NULL;
void SingleLightning::BuildMesh(mesh::Mesh& dest, mesh::Pool& pool)
{
	return instance->Build(dest, pool);
}
#endif

using namespace Algebra;
using namespace mesh;

#if ENABLE_LIGHTNING_MESH

void turn(Mesh& dest, int f, int steps, float d, float angle, float x, float y, float z);

static void branch(Mesh& dest, Mesh& tmp, int f, int n, Noise::Rand& rnd, float d)
{
	while (n > 0)
	{
		float angle = rnd.sfgen() * 5.f;
		turn(dest, f, 2, d, angle, 0, 0, 1);
		if (rnd.boolean(0.026f)) {
			tmp.Clear();
			cube(tmp).Scale(n / 100.f);
			const vector3f& p = dest.vertices[dest.quads[f]].p;
			int f2 = dest.quads.size + 12;
			dest.add(tmp, matrix4::translation(p)
				.rotate(rnd.fgenOnAverage(0, 10.f)*DEG_TO_RAD, vector3f::ux));
			branch(dest, tmp, f2, n - 1, rnd, d);
		}
		n--;
	}
}
#endif // ENABLE_LIGHTNING_MESH

#ifdef ENABLE_RUNTIME_COMPILATION
void Lightning::Build(Mesh& dest, Pool& pool)
#else // !ENABLE_RUNTIME_COMPILATION
void Lightning(Mesh& dest, Pool& pool)
#endif // !ENABLE_RUNTIME_COMPILATION
{
#if ENABLE_LIGHTNING_MESH
	Mesh& tmp = pool.GetTempMesh();
	cube(dest);
	tmp.Translate(0, 0.5f, 0);
	Noise::Rand rnd;
	branch(dest, tmp, 12, 100, rnd, 2.f);
	dest.ComputeNormals();
	dest.ComputeTangents();
#endif // ENABLE_THOLOS_MESH
}

#ifdef ENABLE_RUNTIME_COMPILATION
void SingleLightning::Build(Mesh& dest, Pool& pool)
#else // !ENABLE_RUNTIME_COMPILATION
void SingleLightning(Mesh& dest, Pool& pool)
#endif // !ENABLE_RUNTIME_COMPILATION
{
#if ENABLE_LIGHTNING_MESH
	Mesh& tmp = pool.GetTempMesh();
	cube(dest);
	tmp.Translate(0, 0.5f, 0);
	Noise::Rand rnd;
	branch(dest, tmp, 12, 200, rnd, 20.f);
	dest.ComputeNormals();
	dest.ComputeTangents();
#endif // ENABLE_THOLOS_MESH
}
