#include "Rocks.hh"

#include "engine/algebra/Matrix.hxx"
#include "engine/mesh/Cube.hh"
#include "engine/mesh/Mesh.hh"
#include "engine/mesh/MeshPool.hh"
#include "engine/mesh/Utils.hh"

#define ENABLE_ROCKS_MESH 1

#ifdef ENABLE_RUNTIME_COMPILATION
#include "RuntimeCompiledCPlusPlus/RuntimeObjectSystem/ObjectInterfacePerModule.h"

REGISTERCLASS(Rocks);
ObjectId Rocks::objectId;
Rocks* Rocks::instance = NULL;
void Rocks::BuildMesh(mesh::Mesh& dest, mesh::Pool& pool)
{
	return instance->Build(dest, pool);
}
#endif

using namespace Algebra;
using namespace mesh;

#ifdef ENABLE_RUNTIME_COMPILATION
void Rocks::Build(Mesh& dest, Pool& pool)
#else // !ENABLE_RUNTIME_COMPILATION
void Rocks(Mesh& dest, Pool& pool)
#endif // !ENABLE_RUNTIME_COMPILATION
{
#if ENABLE_ROCKS_MESH
	float size = 5.f;
	Noise::Rand rand;
	Mesh& tmp = pool.GetTempMesh();
	cube(tmp).Scale(size, size, size);
	for (int i = 0; i < 10; i++)
	{
		dest.add(tmp, matrix4::translation(rand.fgen()*10.f*size, 0.f, i*size));
	}
	dest.smooth();
	AddNoise(dest, rand, size * 0.2f);
	dest.smooth();
	AddNoise(dest, rand, size * 0.06f);
	dest.smooth();
	AddNoise(dest, rand, size * 0.018f);

	dest.ComputeNormals();
	GenerateAxisAlignedTextureCoordinates(dest, 0.5f);
	dest.ComputeTangents();
#endif // ENABLE_ROCKS_MESH
}
