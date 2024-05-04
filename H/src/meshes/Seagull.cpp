#include "Seagull.hh"

#include "engine/algebra/Interpolation.hxx"
#include "engine/algebra/Matrix.hxx"
#include "engine/algebra/Utils.hh"
#include "engine/algebra/Vector2.hxx"
#include "engine/container/Utils.hh"
#include "engine/mesh/Cube.hh"
#include "engine/mesh/Mesh.hh"
#include "engine/mesh/MeshPool.hh"
#include "engine/mesh/Utils.hh"
#include "engine/noise/PerlinNoise.hh"

#define ENABLE_SEAGULL_MESH 1

#ifdef ENABLE_RUNTIME_COMPILATION
#include "RuntimeCompiledCPlusPlus/RuntimeObjectSystem/ObjectInterfacePerModule.h"

REGISTERCLASS(Seagull);
ObjectId Seagull::objectId;
Seagull* Seagull::instance = NULL;
void Seagull::BuildMesh(mesh::Mesh& dest, mesh::Pool& pool)
{
	return instance->Build(dest, pool);
}
#endif

using namespace Algebra;
using namespace mesh;

#if ENABLE_SEAGULL_MESH
static void extrudeScale(Mesh& dest, int f, const float* values, int size, float sign)
{
	for (int i = 0; i < size; i += 4)
	{
		dest.Extrude(f);
		dest.transformFace(f,
			matrix4::translation(0.f, values[i], sign * values[i + 1])
			.scale(values[i + 2], values[i + 3], 1.f));
	}
}
#endif // ENABLE_SEAGULL_MESH

#ifdef ENABLE_RUNTIME_COMPILATION
void Seagull::Build(Mesh& dest, Pool& pool)
#else // !ENABLE_RUNTIME_COMPILATION
void Seagull(Mesh& dest, Pool& pool)
#endif // !ENABLE_RUNTIME_COMPILATION
{
#if ENABLE_SEAGULL_MESH
	Mesh& tmp = pool.GetTempMesh();
	cube(tmp).Scale(0.09f, 0.03f, 0.02f);

	const float wing[] = {
		0.03f, 0.25f, 1.25f, 0.5f,
		-0.01f, 0.13f, 0.7f, 0.8f,
		-0.01f, 0.08f, 0.5, 0.8f,
		0.005f, 0.03f, 0.1f, 0.75f,
	};
	extrudeScale(tmp, 20, wing, ARRAY_LEN(wing), +1.f);
	dest.add(tmp, matrix4::rotation(0.5f * PI, 0.f, 1.f, 0.f).translate(-0.17f, 0.01f, 0.01f));
	dest.add(tmp, matrix4::rotation(1.5f * PI, 0.f, 1.f, 0.f).translate(0.08f, 0.01f, 0.01f));

	tmp.Clear();
	cube(tmp)
		.Translate(-0.5f, 0, -0.5f)
		.Scale(0.01f);
	const float body[] = {
		0.f, 0.03f, 5.f, 5.f,
		0.f, 0.02f, 0.7f, 0.75f,
		0.f, 0.04f, 2.8f, 2.f,
		0.f, 0.06f, 1.f, 1.1f,
		0.f, 0.1f, 0.55f, 0.3f,
		0.f, 0.03f, 0.8f, 0.6f,
		0.f, 0.08f, 3.f, 0.5f,
		0.f, 0.02f, 0.7f, 0.5f,
	};
	extrudeScale(tmp, 20, body, ARRAY_LEN(body), +1.f);
	dest.add(tmp);
	dest.Scale(6.f);

	dest.smooth();
	dest.ComputeNormals();
	GenerateAxisAlignedTextureCoordinates(dest, 1.f);
	dest.ComputeTangents();
#endif // ENABLE_SEAGULL_MESH
}
