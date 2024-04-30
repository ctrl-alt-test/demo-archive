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
#include "engine/mesh/Revolution.hh"
#include "engine/noise/Rand.hh"

#define ENABLE_TRIDENT_MESH 1

#ifdef ENABLE_RUNTIME_COMPILATION
#include "RuntimeCompiledCPlusPlus/RuntimeObjectSystem/ObjectInterfacePerModule.h"

REGISTERCLASS(Trident);
ObjectId Trident::objectId;
Trident* Trident::instance = NULL;
void Trident::BuildMesh(mesh::Mesh& dest, mesh::Pool& pool)
{
	return instance->Build(dest, pool);
}

REGISTERCLASS(MiniTrident);
ObjectId MiniTrident::objectId;
MiniTrident* MiniTrident::instance = NULL;
void MiniTrident::BuildMesh(mesh::Mesh& dest, mesh::Pool& pool)
{
	return instance->Build(dest, pool);
}
#endif

using namespace Algebra;
using namespace mesh;

void mesh::IronSpike(Mesh & dest, int top, float length, float tip)
{
	const float transcale[] = {
		0.025f * length, 1.f, 1.f,
		0.05f * length, 1.f, 1.f,
		0.1f * length, 0.5f, 0.5f,
		0.4f * length, 1.f, 1.f,
		0.4f * length, 1.f, 1.f,

		0.1f * tip, 2.f, 1.f,
		0.05f * tip, 1.1f, 1.f,
		0.25f * tip, 1.f, 1.f,
		0.45f * tip, 0.4f, 0.25f,
		0.15f * tip, 0.1f, 0.1f,
	};
	for (size_t i = 0; i < ARRAY_LEN(transcale); i += 3)
	{
		dest.Extrude(top);
		dest.transformFace(top,
			matrix4::translation(0.f, transcale[i], 0.f)
			.scale(transcale[i + 1], 1.f, transcale[i + 2]));
	}
}

#if ENABLE_TRIDENT_MESH

static int branch(Mesh & dest, int start, int bottom_shift, float dir)
{
	dest.Extrude(start);

	// TODO - SIZE: replace with chars. Or factorize the whole transcale thing.
	const float transcale[] = {
		.5f, 0.f, .5f,
		5.f, 2.f, 1.f,
		5.f,-.5f, 1.f,
		0.f, 0.f, 1.f,
		5.f, 1.f, 2.f,
	};
	for (size_t i = 0; i < ARRAY_LEN(transcale); i += 3)
	{
		dest.Extrude(start);
		dest.transformFace(start,
			matrix4::translation(dir * transcale[i], transcale[i + 1], 0)
			.scale(1.f, 1.f, transcale[i + 2]));
	}

	int bottom = dest.quads.size - bottom_shift;
	dest.Extrude(bottom);
	dest.transformFace(bottom, matrix4::translation(0, -3, 0).scale(0.4f, 1, 0.6f));
	dest.Extrude(bottom);
	dest.transformFace(bottom, matrix4::translation(0, -1, 0).scale(0.1f, 1, 0.1f));
	return bottom;
}

static void trident(Mesh& dest, Pool& pool, float length)
{
	Mesh& face = pool.GetTempMesh();

	cube(face).Translate(-0.5f, 0.f, -0.5f).Scale(5.6f, 5.25f, 5.6f);

	// Middle spike.
	IronSpike(face, 12, 30, 17);

	// Left and right spikes.
	int spikei;
	spikei = branch(face, 0, 16, -1);
	IronSpike(face, spikei + 8, 24, 12);

	spikei = branch(face, 4, 4, 1);
	IronSpike(face, spikei - 8, 24, 12);

	// Stick.
	const float transcale[] = {
		-2.5f, 1.f,
		-0.25f, 1.f,
		-1.5f, 0.75f,
		-15.f, 1.5f,
		-0.25f, 1.f,
		-1.f, 0.8f,
		-0.25f, 0.75f,
		-length, 0.75f,
		-5.f, 0.5f
	};
	for (size_t i = 0; i < ARRAY_LEN(transcale); i += 2)
	{
		int f = 8; // cube bottom
		face.Extrude(f);
		face.transformFace(f,
			matrix4::translation(0, transcale[i], 0)
			.scale(transcale[i+1], 1.f, transcale[i+1]));
	}

	face.smooth();
	face.Scale(0.05f);

	// Counting backward because removeFace() is going to invalidate
	// indices.
	for (int i = face.quads.size - 4; i >= 0; i -= 4)
	{
		for (int j = 0; j < 4; ++j)
		{
			vertex& vertex = face.vertices[face.quads[i + j]];
			if (vertex.p.z < 0.f)
			{
				face.removeFace(i);
				break;
			}
			vertex.u = vertex.p.x;
			vertex.v = vertex.p.y;
		}
	}
	dest.add(face);
	dest.add(face, Algebra::matrix4::rotation(PI, 0.f, 1.f, 0.f));
	dest.ComputeNormals();
	dest.ComputeTangents();
}

#endif // ENABLE_TRIDENT_MESH

#ifdef ENABLE_RUNTIME_COMPILATION
void Trident::Build(Mesh& dest, Pool& pool)
#else // !ENABLE_RUNTIME_COMPILATION
void Trident(Mesh& dest, Pool& pool)
#endif // !ENABLE_RUNTIME_COMPILATION
{
#if ENABLE_TRIDENT_MESH
	trident(dest, pool, 180.f);
#endif // ENABLE_TRIDENT_MESH
}

#ifdef ENABLE_RUNTIME_COMPILATION
void MiniTrident::Build(Mesh& dest, Pool& pool)
#else // !ENABLE_RUNTIME_COMPILATION
void MiniTrident(Mesh& dest, Pool& pool)
#endif // !ENABLE_RUNTIME_COMPILATION
{
#if ENABLE_TRIDENT_MESH
	trident(dest, pool, 100.f);
#endif // ENABLE_TRIDENT_MESH
}
