#include "SeaSurface.hh"

#include "engine/container/Utils.hh"
#include "engine/algebra/Interpolation.hxx"
#include "engine/algebra/Matrix.hxx"
#include "engine/algebra/Utils.hh"
#include "engine/mesh/Cube.hh"
#include "engine/mesh/HeightMap.hh"
#include "engine/mesh/Mesh.hh"
#include "engine/mesh/MeshPool.hh"
#include "engine/mesh/Revolution.hh"
#include "engine/mesh/Utils.hh"
#include "tool/mesh/VertexDataPNT.hh"

#define ENABLE_SEA_SURFACE_MESH 1
#define ENABLE_LARGE_SEA_PATCH 0

#ifdef ENABLE_RUNTIME_COMPILATION
#include "RuntimeCompiledCPlusPlus/RuntimeObjectSystem/ObjectInterfacePerModule.h"

REGISTERCLASS(SeaSurface);
ObjectId SeaSurface::objectId;
SeaSurface* SeaSurface::instance = NULL;
void SeaSurface::BuildMesh(mesh::Mesh& dest, mesh::Pool& pool)
{
	return instance->Build(dest, pool);
}

REGISTERCLASS(TinySeaPatch);
ObjectId TinySeaPatch::objectId;
TinySeaPatch* TinySeaPatch::instance = NULL;
void TinySeaPatch::BuildMesh(mesh::Mesh& dest, mesh::Pool& pool)
{
	return instance->Build(dest, pool);
}

#if ENABLE_LARGE_SEA_PATCH
REGISTERCLASS(LargeSeaPatch);
ObjectId LargeSeaPatch::objectId;
LargeSeaPatch* LargeSeaPatch::instance = NULL;
void LargeSeaPatch::BuildMesh(mesh::Mesh& dest, mesh::Pool& pool)
{
	return instance->Build(dest, pool);
}
#endif // ENABLE_LARGE_SEA_PATCH
#endif

using namespace Algebra;
using namespace mesh;

#ifdef ENABLE_RUNTIME_COMPILATION
void SeaSurface::Build(Mesh& dest, Pool& /*pool*/)
#else // !ENABLE_RUNTIME_COMPILATION
void SeaSurface(Mesh& dest, Pool& /*pool*/)
#endif // !ENABLE_RUNTIME_COMPILATION
{
#if ENABLE_SEA_SURFACE_MESH
	struct local
	{
		static vector3f discHFunc(float /*t*/, float)
		{
			return vector3f::zero;
		}

		static float exponentialDiscRFunc(float t, float)
		{
			return 512.f * t*t*t;
		}

		static vertex texCoord(int /*vertexId*/, int /*faceId*/, const vertex& inputVertex)
		{
			vertex result = inputVertex;
			result.u = result.p.x;
			result.v = -result.p.z;
			return result;
		}
	};

	Revolution(local::discHFunc, local::exponentialDiscRFunc).GenerateMesh(dest, 256, 192);
	dest.ApplyFunction(local::texCoord);
#endif // ENABLE_SEA_SURFACE_MESH
}

#ifdef ENABLE_RUNTIME_COMPILATION
void TinySeaPatch::Build(Mesh& dest, Pool& /*pool*/)
#else // !ENABLE_RUNTIME_COMPILATION
void TinySeaPatch(Mesh& dest, Pool& /*pool*/)
#endif // !ENABLE_RUNTIME_COMPILATION
{
#if ENABLE_SEA_SURFACE_MESH
	struct local
	{
		static vector3f discHFunc(float /*t*/, float)
		{
			return vector3f::zero;
		}

		static float discRFunc(float t, float)
		{
			return mix(1.f / 64.f, 8.f, t);
		}
	};

	Revolution(local::discHFunc, local::discRFunc).GenerateMesh(dest, 8, 36);
	//dest.ApplyFunction(local::texCoord);
#endif // ENABLE_SEA_SURFACE_MESH
}

#if ENABLE_LARGE_SEA_PATCH
#ifdef ENABLE_RUNTIME_COMPILATION
void LargeSeaPatch::Build(Mesh& dest, Pool& /*pool*/)
#else // !ENABLE_RUNTIME_COMPILATION
void LargeSeaPatch(Mesh& dest, Pool& /*pool*/)
#endif // !ENABLE_RUNTIME_COMPILATION
{
#if ENABLE_SEA_SURFACE_MESH
	struct local
	{
		static float h(float /*x*/, float /*z*/)
		{
			return 0.f;
		}
	};

	mesh::Heightmap(local::h).GenerateMesh(dest, -800.f, -800.f, 800.f, 800.f, 100, 100);
	dest.ComputeNormals();
	GenerateAxisAlignedTextureCoordinates(dest, 1.f);
	dest.ComputeTangents();
#endif // ENABLE_SEA_SURFACE_MESH
}
#endif // ENABLE_LARGE_SEA_PATCH
