#include "Gate.hh"

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

#define ENABLE_GATE_MESH 1

#ifdef ENABLE_RUNTIME_COMPILATION
#include "RuntimeCompiledCPlusPlus/RuntimeObjectSystem/ObjectInterfacePerModule.h"

REGISTERCLASS(Gate);
ObjectId Gate::objectId;
Gate* Gate::instance = NULL;
void Gate::BuildMesh(mesh::Mesh& dest, mesh::Pool& pool)
{
	return instance->Build(dest, pool);
}
#endif

using namespace Algebra;
using namespace mesh;

#ifdef ENABLE_RUNTIME_COMPILATION
void Gate::Build(Mesh& dest, Pool& /* pool */)
#else // !ENABLE_RUNTIME_COMPILATION
void Gate(Mesh& dest, Pool& /* pool */)
#endif // !ENABLE_RUNTIME_COMPILATION
{
#if ENABLE_GATE_MESH
	cube(dest).Translate(-0.5f, 0, 2.f).Scale(2.5f, 0.5f, 1.f);
	int f = 12;
	for (int i = 0; i < 12; i++)
	{
		dest.Extrude(f);
		float x = i < 6 ? 0.75f : 1.25f;
		dest.transformFace(f,
			matrix4::translation(0, 0.375f, 0)
			.scale(x, 1, 1.f + i/32.f));
	}
	for (int i = 0; i < dest.vertices.size; i++)
	{
		if (dest.vertices[i].p.z > 3.f)
			dest.vertices[i].p.z = 3.f;
	}
	dest.add(dest, matrix4::rotation(PI, 0, 1, 0));
	dest.sharpenEdges();

	dest.ComputeNormals();
	GenerateAxisAlignedTextureCoordinates(dest, 0.5f);
	dest.ComputeTangents();
#endif // ENABLE_GATE_MESH
}
