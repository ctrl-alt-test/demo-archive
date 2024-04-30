#include "Road.hh"

#include "engine/container/Utils.hh"
#include "engine/algebra/Matrix.hxx"
#include "engine/algebra/Utils.hh"
#include "engine/mesh/Cube.hh"
#include "engine/mesh/Mesh.hh"
#include "engine/mesh/MeshPool.hh"
#include "engine/mesh/Revolution.hh"
#include "engine/mesh/RevolutionFunctions.hh"
#include "engine/mesh/Utils.hh"

#define ENABLE_ROAD_MESHES 1

#ifdef ENABLE_RUNTIME_COMPILATION
#include "RuntimeCompiledCPlusPlus/RuntimeObjectSystem/ObjectInterfacePerModule.h"

REGISTERCLASS(Road);
ObjectId Road::objectId;
Road* Road::instance = NULL;
void Road::BuildMesh(mesh::Mesh& dest, mesh::Pool& pool)
{
	return instance->Build(dest, pool);
}

REGISTERCLASS(CityRoad);
ObjectId CityRoad::objectId;
CityRoad* CityRoad::instance = NULL;
void CityRoad::BuildMesh(mesh::Mesh& dest, mesh::Pool& pool)
{
	return instance->Build(dest, pool);
}
#endif

using namespace Algebra;
using namespace mesh;

#if ENABLE_ROAD_MESHES

#endif // ENABLE_ROAD_MESHES

#ifdef ENABLE_RUNTIME_COMPILATION
void CityRoad::Build(Mesh& dest, Pool& /* pool */)
#else // !ENABLE_RUNTIME_COMPILATION
void CityRoad(Mesh& dest, Pool& /* pool */)
#endif // !ENABLE_RUNTIME_COMPILATION
{
#if ENABLE_ROAD_MESHES

	Revolution(mesh::wheelHFunc, mesh::wheelRFunc, NULL, NULL).GenerateMesh(dest, 3, 20);
	dest.Scale(440, 0.25f, 440);

	dest.ComputeNormals();
	GenerateAxisAlignedTextureCoordinates(dest, 0.08f);
	dest.ComputeTangents();
#endif // ENABLE_ROAD_MESHES
}

#ifdef ENABLE_RUNTIME_COMPILATION
void Road::Build(Mesh& dest, Pool& /* pool */)
#else // !ENABLE_RUNTIME_COMPILATION
void Road(Mesh& dest, Pool& /* pool */)
#endif // !ENABLE_RUNTIME_COMPILATION
{
#if ENABLE_ROAD_MESHES
	dest.Clear();
	cube(dest).Scale(10, 1, 200);
	dest.sharpenEdges();

	Noise::Rand rand;
	AddNoise(dest, rand, 0.01f);
	dest.ComputeNormals();
	GenerateAxisAlignedTextureCoordinates(dest, 0.03f);
	dest.ComputeTangents();
#endif // ENABLE_ROAD_MESHES
}