#include "Fence.hh"

#include "GreekColumn.hh"
#include "Trident.hh"
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
#include "engine/mesh/Utils.hh"
#include "engine/noise/Rand.hh"

#define ENABLE_FENCE_MESH 1

#ifdef ENABLE_RUNTIME_COMPILATION
#include "RuntimeCompiledCPlusPlus/RuntimeObjectSystem/ObjectInterfacePerModule.h"

REGISTERCLASS(Fence);
ObjectId Fence::objectId;
Fence* Fence::instance = NULL;
void Fence::BuildMesh(mesh::Mesh& dest, mesh::Pool& pool)
{
	return instance->Build(dest, pool);
}

REGISTERCLASS(InsideFence);
ObjectId InsideFence::objectId;
InsideFence* InsideFence::instance = NULL;
void InsideFence::BuildMesh(mesh::Mesh& dest, mesh::Pool& pool)
{
	return instance->Build(dest, pool);
}
#endif

using namespace Algebra;
using namespace mesh;

#if ENABLE_FENCE_MESH

static void fencePart(Mesh& res, Pool& pool,
					  float columnDistance, float columnHeight, float columnWidth,
					  float barLength, float spikeLength,
					  bool last)
{
	// One spike every 25cm. There will be one spike less that spikeCount,
	// to leave room for the columns.
	float spikeDistance = 0.25f;
	int spikeCount = msys_ifloorf((columnDistance - columnWidth) * 4);

	Mesh& tmp = pool.GetTempMesh();
	cube(tmp).Translate(-0.5f, 0.f, -0.5f).Scale(0.07f);

	// Fence is 1.9m tall.
	mesh::IronSpike(tmp, 12, barLength, spikeLength); // Defined in Trident.cpp
	for (int i = 0; i < spikeCount; i++)
	{
		float x = (columnDistance + spikeDistance + (2 * i - spikeCount) * spikeDistance) / 2.f;
		res.add(tmp, matrix4::translation(x, 0, 0));
	}

	tmp.Clear();
	mesh::CubicColumn(tmp, pool);
	tmp.Scale(columnWidth, 0.5f * columnHeight, columnWidth);
	res.add(tmp);
	if (last) res.add(tmp, matrix4::translation(columnDistance, 0, 0));

	tmp.Clear();
	sharpCube(tmp).Translate(0,0,-0.5).Scale(columnDistance, 0.15f, 0.8f * columnWidth);
	res.add(tmp);

	// The origin is the bottom of the column.
	// The wall is below 0, so its apparent height can be adjusted just
	// by moving the mesh.
	tmp.Clear();
	sharpCube(tmp).Translate(0, -1.f, -0.5f).Scale(columnDistance + columnWidth, 0.45f * columnHeight, columnWidth);
	res.add(tmp, matrix4::translation(-0.5f * columnWidth, 0, 0));
}

#endif // ENABLE_FENCE_MESH

#ifdef ENABLE_RUNTIME_COMPILATION
void Fence::Build(Mesh& dest, Pool& pool)
#else // !ENABLE_RUNTIME_COMPILATION
void Fence(Mesh& dest, Pool& pool)
#endif // !ENABLE_RUNTIME_COMPILATION
{
#if ENABLE_FENCE_MESH
	for (int i = 0; i < 4; i++)
	{
		float columnDistance = 6.f;
		float columnHeight = 2.f;
		dest.Translate(columnDistance, 0, 0);
		fencePart(dest, pool, columnDistance, columnHeight, columnHeight * 0.3f, 1.6f, 0.3f, (i == 0));
	}

	Noise::Rand rnd;
	AddNoise(dest, rnd, 0.000125f);
	dest.ComputeNormals();
	GenerateAxisAlignedTextureCoordinates(dest, 0.05f);
	dest.ComputeTangents();
#endif // ENABLE_FENCE_MESH
}

#ifdef ENABLE_RUNTIME_COMPILATION
void InsideFence::Build(Mesh& dest, Pool& pool)
#else // !ENABLE_RUNTIME_COMPILATION
void InsideFence(Mesh& dest, Pool& pool)
#endif // !ENABLE_RUNTIME_COMPILATION
{
#if ENABLE_FENCE_MESH
	fencePart(dest, pool, 8.f, 0.6f, 0.4f, 1., 0.05f, true);

	dest.ComputeNormals();
	GenerateAxisAlignedTextureCoordinates(dest, 0.05f);
	dest.ComputeTangents();
#endif // ENABLE_FENCE_MESH
}
