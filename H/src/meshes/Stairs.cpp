#include "Stairs.hh"

#include "GreekColumn.hh"
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
#include "engine/noise/PerlinNoise.hh"

#define ENABLE_STAIRS_MESH 1

#ifdef ENABLE_RUNTIME_COMPILATION
#include "RuntimeCompiledCPlusPlus/RuntimeObjectSystem/ObjectInterfacePerModule.h"

REGISTERCLASS(Stairs);
ObjectId Stairs::objectId;
Stairs* Stairs::instance = NULL;
void Stairs::BuildMesh(mesh::Mesh& dest, mesh::Pool& pool)
{
	return instance->Build(dest, pool);
}
#endif

using namespace Algebra;
using namespace mesh;

float mesh::StairsPlatform(Mesh& dest, Pool& pool,
						   int steps, float stepHeight, float stepWidth,
						   float width, float length)
{
#if ENABLE_STAIRS_MESH
	Mesh& tmp = pool.GetTempMesh();

	// The steps are created from the top to the bottom on purpose, to
	// reduce overdraw. On my machine this notably improved performance
	// (from 30 to 60fps).
	for (int i = steps - 1; i >= 0; --i)
	{
		float x = stepWidth * i;
		float y = stepHeight * i;
		float w = width - 2.f * x;
		float l = length - 2.f * x;
		tmp.Clear();
		sharpCube(tmp, (mesh::CubeFaces)(mesh::face_all & ~mesh::face_bottom))
			.Scale(w, stepHeight, l).Translate(x, y, x);
		dest.add(tmp);
	}
#endif // ENABLE_STAIRS_MESH

	return steps * stepHeight;
}

#if ENABLE_STAIRS_MESH

static const float width = 14.f;
static const int steps = 150;
// To get realistic dimensions, we check Wikipedia: https://en.wikipedia.org/wiki/Stairs
// and follow the UK law. For that, let's keep:
// stepRise in [0.155 .. 0.22] and treadDepth in [0.245 .. 0.260].
// We also suppose they had the same law in Atlantis. :)
static const float stepRise = 0.21f;
static const float treadDepth = 0.26f;
static const float landingSize = 8.f;

#define STAIRS_RESOLUTION 6

static void stairs(Mesh& dest, Pool& pool)
{
	Mesh& tmp = pool.GetTempMesh();
	sharpCube(tmp, (mesh::CubeFaces)(mesh::face_top | mesh::face_back))
		.Scale(width / STAIRS_RESOLUTION, stepRise, treadDepth);
	for (int i = 0; i < steps; ++i)
	{
		float x = treadDepth * i;
		float y = stepRise * i;

		for (int j = 0; j < STAIRS_RESOLUTION; ++j)
		{
			dest.add(tmp, matrix4::translation(j * width / STAIRS_RESOLUTION, y, x));
		}
	}
}

static void handRails(Mesh& dest, Pool& pool)
{
	float length = steps * treadDepth;

	Mesh& tmp = pool.GetTempMesh();
	cube(tmp, (mesh::CubeFaces)(mesh::face_all & ~mesh::face_bottom))
		.Translate(-1.f, 0, 0)
		.Scale(0.5f, 1.f, length);
	float height = stepRise * steps;
	tmp.vertices[6].p.y += height;
	tmp.vertices[2].p.y += height;
	tmp.sharpenEdges();

	dest.add(tmp, matrix4::translation(0, 0, 0));
	dest.add(tmp, matrix4::translation(width, 0, 0));
}

void landing(Mesh& dest, Pool& pool)
{
	float height = steps * stepRise;
	Mesh& tmp = pool.GetTempMesh();
	sharpCube(tmp, (mesh::CubeFaces)(mesh::face_all & ~mesh::face_bottom))
		.Translate(0.f, 0, 0.f)
		.Scale(width, height, landingSize + steps * treadDepth);
	dest.add(tmp, matrix4::translation(0, 0, steps * treadDepth));

	tmp.Clear();
	sharpCube(tmp, (mesh::CubeFaces)(mesh::face_all & ~mesh::face_bottom))
		.Translate(-1.f, 0, 0.f)
		.Scale(0.5, height + 1.f, landingSize + steps * treadDepth);
	dest.add(tmp, matrix4::translation(0, 0, steps * treadDepth));
	dest.add(tmp, matrix4::translation(width, 0, steps * treadDepth));
}

static vertex deformation(int, int, const vertex& inputVertex)
{
	vertex result = inputVertex;

	vector2f p1 = { inputVertex.p.x * 0.03f, inputVertex.p.y * 0.5f };
	vector2f p2 = { inputVertex.p.x * 0.1f, inputVertex.p.z * 1.f };
	float noiseY = Noise::PerlinNoise::value(p1, 100.f);
	float noiseZ = Noise::PerlinNoise::value(p2, 100.f);

	result.p.y += 0.2f * noiseY;
	result.p.z += 0.15f * noiseZ;

	return result;
}

#endif // ENABLE_STAIRS_MESH

#ifdef ENABLE_RUNTIME_COMPILATION
void Stairs::Build(Mesh& dest, Pool& pool)
#else // !ENABLE_RUNTIME_COMPILATION
void Stairs(Mesh& dest, Pool& pool)
#endif // !ENABLE_RUNTIME_COMPILATION
{
#if ENABLE_STAIRS_MESH
	stairs(dest, pool);

	handRails(dest, pool);
	dest.add(dest, matrix4::translation(0, steps * stepRise, steps * treadDepth + landingSize));

	Mesh& tmp = pool.GetTempMesh();
	mesh::CubicColumn(tmp, pool);
	dest.add(tmp, matrix4::translation(-0.25f, 0.f, 0.f));
	dest.add(tmp, matrix4::translation(width - 0.25f, 0.f, 0.f));

	landing(dest, pool);
	dest.Rotate(180 * DEG_TO_RAD, 0.f, 1.f, 0.f);

	dest.ApplyFunction(deformation);

	dest.ComputeNormals();
	GenerateAxisAlignedTextureCoordinates(dest, 0.5f);
	dest.ComputeTangents();
#endif // ENABLE_STAIRS_MESH
}
