#include "Plant.hh"

#include "engine/algebra/Functions.hh"
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
#include "engine/mesh/Utils.hh"

#define ENABLE_PLANT_MESH 1

#ifdef ENABLE_RUNTIME_COMPILATION
#include "RuntimeCompiledCPlusPlus/RuntimeObjectSystem/ObjectInterfacePerModule.h"

REGISTERCLASS(PlantDensePatch);
ObjectId PlantDensePatch::objectId;
PlantDensePatch* PlantDensePatch::instance = NULL;
void PlantDensePatch::BuildMesh(mesh::Mesh& dest, mesh::Pool& pool)
{
	return instance->Build(dest, pool);
}

REGISTERCLASS(PlantSparsePatch);
ObjectId PlantSparsePatch::objectId;
PlantSparsePatch* PlantSparsePatch::instance = NULL;
void PlantSparsePatch::BuildMesh(mesh::Mesh& dest, mesh::Pool& pool)
{
	return instance->Build(dest, pool);
}

#endif // ENABLE_RUNTIME_COMPILATION

using namespace Algebra;
using namespace mesh;

#if ENABLE_PLANT_MESH

void vegetal(Mesh& dest, Mesh& branch, Noise::Rand& rand, float size)
{
	int n = msys_ifloorf(size * 10.f);
	float yinc = size * 0.015f;

	for (int i = 0; i < n; i++)
	{
		matrix4 m = matrix4::scaling(1.f, 1.f, rand.fgen(0.25f, 1.f));
		m.translate(0, i*yinc, 0);
		m.rotate(rand.fgen(0, 2.f * 3.1415f), vector3f::uy);
		m.rotate(-rand.fgen(0.5f, 1.5f), vector3f::ux);
		dest.add(branch, m);
	}
	AddNoise(dest, rand, 0.02f);
}

void createPatch(Mesh& dest, Pool& pool, int numberOfPlants, float patchScale)
{
	Noise::Rand rand;
	Noise::Rand randPos;
	Mesh& branch = pool.GetTempMesh();

	cube(branch, (mesh::CubeFaces)(mesh::face_all & ~mesh::face_bottom)).Scale(0.1f, 0.15f, 0.5f);
	branch.Extrude(16);
	branch.transformFace(16, matrix4::translation(0.f, 0.f, 0.25f).scale(0.5f, 0.4f, 1));

	Mesh& singlePlant = pool.GetTempMesh();

	for (int i = 0; i < numberOfPlants; ++i)
	{
		vegetal(singlePlant, branch, rand, rand.fgen(0.25f, 1.f) * rand.fgen(1.f, 8.f));

		vector3f position = vector3f::zero;
		for (int k = 0; k < 10; ++k)
		{
			position.x += randPos.fgen(-1.f, 1.f) * patchScale;
			position.z += randPos.fgen(-1.f, 1.f) * patchScale;
		}

		singlePlant.Translate(position);
		dest.add(singlePlant);
		singlePlant.Clear();
	}

	dest.ComputeNormals();
	GenerateAxisAlignedTextureCoordinates(dest, 0.5f);
	dest.ComputeTangents();
}

#endif // ENABLE_PLANT_MESH

#ifdef ENABLE_RUNTIME_COMPILATION
void PlantDensePatch::Build(Mesh& dest, Pool& pool)
#else // !ENABLE_RUNTIME_COMPILATION
void PlantDensePatch(Mesh& dest, Pool& pool)
#endif // !ENABLE_RUNTIME_COMPILATION
{
#if ENABLE_PLANT_MESH
	createPatch(dest, pool, 100, 2.f);
#endif // ENABLE_PLANT_MESH
}

#ifdef ENABLE_RUNTIME_COMPILATION
void PlantSparsePatch::Build(Mesh& dest, Pool& pool)
#else // !ENABLE_RUNTIME_COMPILATION
void PlantSparsePatch(Mesh& dest, Pool& pool)
#endif // !ENABLE_RUNTIME_COMPILATION
{
#if ENABLE_PLANT_MESH
	createPatch(dest, pool, 50, 4.f);
#endif // ENABLE_PLANT_MESH
}
