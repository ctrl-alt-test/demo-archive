#include "RuinsFloor.hh"

#include "engine/algebra/Interpolation.hxx"
#include "engine/algebra/Matrix.hxx"
#include "engine/algebra/Vector2.hxx"
#include "engine/mesh/HeightMap.hh"
#include "engine/mesh/Mesh.hh"
#include "engine/mesh/MeshPool.hh"
#include "engine/mesh/Utils.hh"
#include "engine/noise/Utils.hh"

#define ENABLE_RUINS_FLOOR_MESH 1

// TODO - SIZE: Can this be factorized with CityFloor?

#ifdef ENABLE_RUNTIME_COMPILATION
#include "RuntimeCompiledCPlusPlus/RuntimeObjectSystem/ObjectInterfacePerModule.h"

REGISTERCLASS(RuinsFloor);
ObjectId RuinsFloor::objectId;
RuinsFloor* RuinsFloor::instance = NULL;
void RuinsFloor::BuildMesh(mesh::Mesh& dest, mesh::Pool& pool)
{
	return instance->Build(dest, pool);
}
#endif

using namespace Algebra;
using namespace mesh;

#ifdef ENABLE_RUNTIME_COMPILATION
void RuinsFloor::Build(Mesh& dest, Pool& /*pool*/)
#else // !ENABLE_RUNTIME_COMPILATION
void RuinsFloor(Mesh& dest, Pool& /*pool*/)
#endif // !ENABLE_RUNTIME_COMPILATION
{
#if ENABLE_RUINS_FLOOR_MESH
	struct local
	{
		// Very similar to UnderwaterFloor.
		static float h(float x, float z)
		{
			const vector2f p = { x, z };
			float h = Noise::fBmWorley(p, 4.f, 4, 2.0f, 1.f / 0.4f, -0.5f);
			float isWild = mix(0.08f, 1.f, smoothStep(0.03f, 0.1f, Algebra::abs(0.5f * h + x - 0.5f)));
			return isWild * h;
		}

		static vertex scaleTexture(int, int, const vertex& inputVertex)
		{
			vertex result = inputVertex;
			result.u *= 40.f;
			result.v *= 40.f;
			return result;
		}
	};

	mesh::Heightmap(local::h).GenerateMesh(dest, 0.2f, 0.2f, 0.8f, 0.8f, 250, 250);
	dest.Scale(1000.f, 200.f, 1000.f).Translate(-400.f, 0.f, -350.f);
	dest.ApplyFunction(local::scaleTexture);
	
	dest.ComputeNormals();
	dest.ComputeTangents();
#endif // ENABLE_RUINS_FLOOR_MESH
}
