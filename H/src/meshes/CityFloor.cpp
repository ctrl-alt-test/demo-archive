#include "CityFloor.hh"

#include "engine/algebra/Interpolation.hxx"
#include "engine/algebra/Matrix.hxx"
#include "engine/algebra/Vector2.hxx"
#include "engine/mesh/HeightMap.hh"
#include "engine/mesh/Mesh.hh"
#include "engine/mesh/MeshPool.hh"
#include "engine/mesh/Utils.hh"
#include "engine/noise/WorleyNoise.hh"
#include "engine/noise/Utils.hh"

#define ENABLE_CITY_FLOOR_MESH 1

#ifdef ENABLE_RUNTIME_COMPILATION
#include "RuntimeCompiledCPlusPlus/RuntimeObjectSystem/ObjectInterfacePerModule.h"

REGISTERCLASS(CityFloor);
ObjectId CityFloor::objectId;
CityFloor* CityFloor::instance = NULL;
void CityFloor::BuildMesh(mesh::Mesh& dest, mesh::Pool& pool)
{
	return instance->Build(dest, pool);
}

REGISTERCLASS(IslandFloor);
ObjectId IslandFloor::objectId;
IslandFloor* IslandFloor::instance = NULL;
void IslandFloor::BuildMesh(mesh::Mesh& dest, mesh::Pool& pool)
{
	return instance->Build(dest, pool);
}
#endif

using namespace Algebra;
using namespace mesh;

static void makeFloor(Mesh& dest, bool isIsland)
{
#if ENABLE_CITY_FLOOR_MESH
	struct local
	{
		static float hGen(float x, float z, bool isIsland)
		{
			const vector2f p = { x, z };
			float d = dist(p, vec2(0.5f, 0.5f));
			float h = Noise::fBmWorley(p, 4.f, 4, 2.f, 1.f / 0.35f, -0.5f);
			float detail = Noise::fBmWorley(p, 20.f, 4, 2.f, 1.f / 0.45f, -0.5f);
			Noise::Rand rnd((int)(x + z * 11.f));
			Noise::VoronoiInfo voronoi = Noise::WorleyNoise::value(10.f * p, 3.f, 1.f, 2.5f);

			float notQuiteRound = d + 0.08f * voronoi.f3;

			float ret = 0;
			// Background mountains
			float island = isIsland ? -1.f : 1.f;
			ret += island * smoothStep(0.35f, 0.44f, notQuiteRound) * h * 0.75f;

			// Central hill
			ret += 0.19f * (smoothStep(0.13f, 0.07f, d+voronoi.f1*0.03f) +
				1.5f *
				detail *
				smoothStep(0.15f, 0.13f, d+voronoi.f1*0.03f) *
				smoothStep(0.10f, 0.13f, notQuiteRound));

			// Temple hill (to ensure the temple can be safely built).
			 float d2 = dist(p, vec2(0.5f, 0.51f));
			ret = mix(ret, 0.25f,
				smoothStep(0.08f, 0.05f, d2));

			// Moat.
			ret += smoothStep(0.45f, 0.55f, notQuiteRound) * 0.2f;
			ret -= smoothStep(0.43f, 0.45f, notQuiteRound) * 0.1f;
			ret -= smoothStep(0.38f, 0.41f, notQuiteRound) * 0.13f;
			ret -= smoothStep(0.37f, 0.38f, notQuiteRound) * 0.07f;

			return ret;
		}

		static vertex scaleTexture(int, int, const vertex& inputVertex)
		{
			vertex result = inputVertex;
			result.u *= 80.f;
			result.v *= 80.f;
			return result;
		}

		static float hIsland(float x, float z)
		{
			return hGen(x, z, true);
		}

		static float hCity(float x, float z)
		{
			return hGen(x, z, false);
		}
	};

	mesh::Heightmap(isIsland ? local::hIsland : local::hCity).GenerateMesh(dest, 0.f, 0.f, 1.f, 1.f, 200, 200);
	dest.Scale(1500.f, 250.f, 1500.f).Translate(0.f, 0.f, 0.f);
	dest.ApplyFunction(local::scaleTexture);

	dest.ComputeNormals();
	dest.ComputeTangents();
#endif // ENABLE_CITY_FLOOR_MESH
}

#ifdef ENABLE_RUNTIME_COMPILATION
void CityFloor::Build(Mesh& dest, Pool& /*pool*/)
#else // !ENABLE_RUNTIME_COMPILATION
void CityFloor(Mesh& dest, Pool& /*pool*/)
#endif // !ENABLE_RUNTIME_COMPILATION
{
	makeFloor(dest, false);
}

#ifdef ENABLE_RUNTIME_COMPILATION
void IslandFloor::Build(Mesh& dest, Pool& /*pool*/)
#else // !ENABLE_RUNTIME_COMPILATION
void IslandFloor(Mesh& dest, Pool& /*pool*/)
#endif // !ENABLE_RUNTIME_COMPILATION
{
	makeFloor(dest, true);
}
