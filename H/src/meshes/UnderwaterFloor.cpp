#include "UnderwaterFloor.hh"

#include "engine/algebra/Interpolation.hxx"
#include "engine/algebra/Matrix.hxx"
#include "engine/algebra/Vector2.hxx"
#include "engine/mesh/HeightMap.hh"
#include "engine/mesh/Mesh.hh"
#include "engine/mesh/MeshPool.hh"
#include "engine/mesh/Utils.hh"
#include "engine/noise/Utils.hh"

#ifdef ENABLE_RUNTIME_COMPILATION
#include "RuntimeCompiledCPlusPlus/RuntimeObjectSystem/ObjectInterfacePerModule.h"

REGISTERCLASS(UnderwaterFloor);
ObjectId UnderwaterFloor::objectId;
UnderwaterFloor* UnderwaterFloor::instance = NULL;
void UnderwaterFloor::BuildMesh(mesh::Mesh& dest, mesh::Pool& pool)
{
	return instance->Build(dest, pool);
}
#endif

using namespace Algebra;
using namespace mesh;

#ifdef ENABLE_RUNTIME_COMPILATION
void UnderwaterFloor::Build(Mesh& dest, Pool& /*pool*/)
#else // !ENABLE_RUNTIME_COMPILATION
void UnderwaterFloor(Mesh& dest, Pool& /*pool*/)
#endif // !ENABLE_RUNTIME_COMPILATION
{
	struct local
	{
		static float h(float x, float z)
		{
			float h = Noise::fBmWorley(vec2(x, z), 4.f, 4, 2.f, 1.f / 0.35f, -0.5f);
			float isWild = smoothStep(0.15f, 0.2f, norm(vec2(x - 0.5f, z - 0.5f)));
			return mix(0.1f, 1.f, isWild) * h;
		}

		static vertex scaleTexture(int, int, const vertex& inputVertex)
		{
			vertex result = inputVertex;
			result.u *= 20.f;
			result.v *= 20.f;
			return result;
		}
	};

	mesh::Heightmap(local::h).GenerateMesh(dest, 0.f, 0.f, 1.f, 1.f, 200, 200);
	dest.Scale(1000.f, 200.f, 1000.f).Translate(-400.f, 0.f, -350.f);
	dest.ApplyFunction(local::scaleTexture);

	dest.ComputeNormals();
}
