#include "ResearchVessel.hh"

#include "engine/algebra/Functions.hh"
#include "engine/algebra/Interpolation.hxx"
#include "engine/algebra/Matrix.hxx"
#include "engine/algebra/Utils.hh"
#include "engine/container/Utils.hh"
#include "engine/mesh/Cube.hh"
#include "engine/mesh/HeightMap.hh"
#include "engine/mesh/Mesh.hh"
#include "engine/mesh/MeshPool.hh"
#include "engine/mesh/Revolution.hh"
#include "engine/mesh/Utils.hh"

#define ENABLE_RESEARCH_VESSEL_MESH 1

// SIZE: Enable to remove details off bridge model and save 16 bytes.
// Those details are not visible from the point of view in the shot.
#define ENABLE_BRIDGE_SIZE_OPTIMS 1

// SIZE: Comment to remove the details off the rudder and save 17
// bytes.
#define ENABLE_RUDDER_DETAILS 1

// SIZE: Comment to remove the propeller and save 62 bytes.
#define ENABLE_PROPELLER 1

#ifdef ENABLE_RUNTIME_COMPILATION
#include "RuntimeCompiledCPlusPlus/RuntimeObjectSystem/ObjectInterfacePerModule.h"

REGISTERCLASS(ResearchVesselHull);
ObjectId ResearchVesselHull::objectId;
ResearchVesselHull* ResearchVesselHull::instance = NULL;
void ResearchVesselHull::BuildMesh(mesh::Mesh& dest, mesh::Pool& pool)
{
	return instance->Build(dest, pool);
}

REGISTERCLASS(ResearchVesselBridgeWindowPart);
ObjectId ResearchVesselBridgeWindowPart::objectId;
ResearchVesselBridgeWindowPart* ResearchVesselBridgeWindowPart::instance = NULL;
void ResearchVesselBridgeWindowPart::BuildMesh(mesh::Mesh& dest, mesh::Pool& pool)
{
	return instance->Build(dest, pool);
}

REGISTERCLASS(ResearchVesselBridgeNoWindowPart);
ObjectId ResearchVesselBridgeNoWindowPart::objectId;
ResearchVesselBridgeNoWindowPart* ResearchVesselBridgeNoWindowPart::instance = NULL;
void ResearchVesselBridgeNoWindowPart::BuildMesh(mesh::Mesh& dest, mesh::Pool& pool)
{
	return instance->Build(dest, pool);
}

REGISTERCLASS(ResearchVesselCrane);
ObjectId ResearchVesselCrane::objectId;
ResearchVesselCrane* ResearchVesselCrane::instance = NULL;
void ResearchVesselCrane::BuildMesh(mesh::Mesh& dest, mesh::Pool& pool)
{
	return instance->Build(dest, pool);
}
#endif

using namespace Algebra;
using namespace mesh;

const float width = 7.f;
const float height = 9.f;
const float length = 25.f;

#if ENABLE_RESEARCH_VESSEL_MESH

static void generateHullTextureCoordinates(Mesh& dest, float scale)
{
	for (int i = 0; i < dest.vertices.size; ++i)
	{
		vertex& vertex = dest.vertices[i];
		if (vertex.n.x >= 0.f)
		{
			vertex.u = scale * -vertex.p.z;
			vertex.v = scale * vertex.p.y;
		}
		else
		{
			vertex.u = scale * vertex.p.z;
			vertex.v = scale * vertex.p.y;
		}
	}
}

#endif // ENABLE_RESEARCH_VESSEL_MESH

#ifdef ENABLE_RUNTIME_COMPILATION
void ResearchVesselHull::Build(Mesh& dest, Pool& pool)
#else // !ENABLE_RUNTIME_COMPILATION
void ResearchVesselHull(Mesh& dest, Pool& pool)
#endif // !ENABLE_RUNTIME_COMPILATION
{
#if ENABLE_RESEARCH_VESSEL_MESH
	struct local
	{
		static float hullCull(float, float y, float)
		{
			return y+0.5f;
		}

		static float seaLevel(float, float)
		{
			return 0;
		}

		static float h(float x, float z)
		{
			float x1 = Algebra::abs(x);
			float z1 = Algebra::abs(1.35f*z-0.25f);
			float frontSlope = -3.f * z + 1.5f;
			float angleFront = 1.f - x1;
			float parabollaFront = 1.f - mix(x1*x1, x1*x1*x1, 0.8f);
			float front = mix(angleFront, parabollaFront, smoothStep(-0.6f, -1.3f, frontSlope))+frontSlope;
			// Adding a 0.001 contribution of x1 to x1^5 to avoid a
			// perfectly flat floor, which would produce degenerate
			// tangents.
			float middle = (1.f - mix(x1*x1*x1*x1*x1, x1, 0.001f)) * (1.f - z1*z1*z1/5.f);
			float back = (1.f - x1*x1) * (1.f - z1*z1*z1/5.f);

			float backPlane = -z*z+.7f;

			float shape = mix(middle, back, smoothStep(0.f, -1.f, z));
			shape = mix(shape, front, smoothStep(0.4f, 0.9f, z));

			return mix(shape, backPlane, smoothStep(-.92f, -1.1f, z));
		}

		static vertex squeeze(int, int, const vertex& inputVertex)
		{
			vertex result = inputVertex;
			float x = Algebra::abs(result.p.x);
			float z = Algebra::abs(result.p.z);
			result.p.x *= mix(1.f-z*z*z, 1.f, 0.6f);
			if (x < 0.03f && result.p.z < 0.f)
			{
				// Keep a slope to avoid degenerate tangents.
				float tinySlope = 1.f - 0.001f * z;
				result.p.y = tinySlope * mix(result.p.y, 1.f, smoothStep(-0.71f, -0.65f, result.p.z));
			}
			return result;
		}
	};

	mesh::Mesh& part = pool.GetTempMesh();

	mesh::Heightmap(local::h).GenerateMesh(part, 0.05f, -1.f, 1.2f, 1.f, 10, 40);
	part.ApplyFunction(local::squeeze);
	dest.add(part);

	part.Clear();
	mesh::Heightmap(local::h).GenerateMesh(part, -1.2f, -1.f, -0.05f, 1.f, 10, 40);
	part.ApplyFunction(local::squeeze);
	dest.add(part);

	part.Clear();
	mesh::Heightmap(local::h).GenerateMesh(part, -0.05f, -1.f, 0.05f, 1.f, 5, 40);
	part.ApplyFunction(local::squeeze);
	dest.add(part);

	dest.Cut(local::hullCull);

	dest.Transform(matrix4::translation(0.f, 0.3f, 0.f).rotate(PI, vector3f::ux));//.scale(width, height, length));

	part.Clear();
	sharpCube(part).Translate(-0.5f, -1.f, -0.25f).Scale(0.02f, 0.3f, 0.1f);
	dest.add(part, matrix4::translation(0.f, 0.f, 0.8f));

	mesh::Mesh& rudder = pool.GetTempMesh();
	part.Clear();
	cube(part).Translate(-0.5f, -1.f, -0.25f).Scale(0.02f, 0.4f, 0.1f);
	part.Extrude(16); part.transformFace(16, matrix4::translation(0.f, 0.f, -0.005f).scale(0.25f, 1.f, 1.f));
	part.transformFace(20, matrix4::translation(0.f, 0.f, -0.01f).scale(0.5f, 1.f, 1.f));
	part.Extrude(20); part.transformFace(20, matrix4::translation(0.f, 0.f, 0.005f).scale(0.5f, 1.f, 1.f));
	part.sharpenEdges();
	rudder.add(part);

#if ENABLE_RUDDER_DETAILS
	part.Clear();
	cube(part).Translate(-0.5f, -1.f, -0.25f).Scale(0.05f, 0.02f, 0.09f);
	part.smooth();
	rudder.add(part, matrix4::translation(0.f, -0.05f, 0.f));
	rudder.add(part, matrix4::translation(0.f, -0.15f, 0.f));
	rudder.add(part, matrix4::translation(0.f, -0.25f, 0.f));
	rudder.add(part, matrix4::translation(0.f, -0.35f, 0.f));
#endif // ENABLE_RUDDER_DETAILS
	rudder.Translate(0.f, -0.3f, 0.805f);
	dest.add(rudder);

	dest.Transform(matrix4::scaling(width, height, length));

#if ENABLE_PROPELLER
	part.Clear();
	cube(part).Translate(-0.5f, 0.f, -0.5f).Scale(0.4f, 0.5f, 0.02f);
	part.Extrude(20);
	part.transformFace(8, matrix4::scaling(0.1f, 1.f, 1.f));
	part.smooth();
	for (int i = 0; i < 4; ++i)
	{
		float angle = 1.f + float(i) * 0.5f * PI;
		dest.add(part, matrix4::translation(0.f, height * -0.53f, length * 0.72f)
			.rotate(angle, vector3f::uz)
			.rotate(0.25f, vector3f::uy)
			.scale(width * 0.4f));
	}
#endif // ENABLE_PROPELLER

#if 0 // water plane
	part.Clear();
	mesh::Heightmap(local::seaLevel).GenerateMesh(part, -1.f, -1.f, 1.f, 1.f, 4, 8);
	dest.add(part, matrix4::translation(0.f, 0.f, 0.f).scale(2.f * length, 1.f, 2.f * length));
#endif

	dest.ComputeNormals();

	// Quick and dirty generation of reasonable texture coordinates.
	// Between 0.7 and 0.75 makes the texture approximately the height of
	// the hull. The way the mesh is done, 0 is the floating line:
	// [0..0.5] is above water, [0.5..1] is under water.
	generateHullTextureCoordinates(dest, 0.075f);

	dest.ComputeTangents();
#endif // ENABLE_RESEARCH_VESSEL_MESH
}

#if ENABLE_RESEARCH_VESSEL_MESH

static void buildBridge(Mesh& dest, Pool& pool, bool windowPart)
{
	mesh::Mesh& part = pool.GetTempMesh();

	// Cabin
	sharpCube(part).Translate(-0.5f, -1.f, 0.f).Scale(1.7f, 0.7f, 0.6f);
	dest.add(part, matrix4::translation(0.f, 0.f, -0.15f));
	part.Clear();
	cube(part).Translate(-0.5f, -1.f, 0.15f).Scale(1.4f, 1.5f, 0.34f);
	part.Extrude(16); part.transformFace(16, matrix4::translation(0.f, 0.f, -0.025f).scale(.85f, 1.f, 1.f));
#if !ENABLE_BRIDGE_SIZE_OPTIMS
	part.Extrude(20); part.transformFace(20, matrix4::translation(0.f, 0.f, 0.025f).scale(.85f, 1.f, 1.f));
#endif // ENABLE_BRIDGE_SIZE_OPTIMS
	part.sharpenEdges();
	dest.add(part, matrix4::translation(0.f, 0.f, -0.2f));

	part.Clear();
	sharpCube(part).Translate(-0.5f, -1.f, 0.f).Scale(1.85f, 0.02f, 0.4f);
	dest.add(part, matrix4::translation(0.f, -0.7f, -0.15f));
	dest.add(part, matrix4::translation(0.f, -1.052f, -0.15f));
	dest.add(part, matrix4::translation(0.f, -1.40f, -0.15f));

	// Bridge
	part.Clear();
	cube(part).Translate(-0.5f, -1.f, 0.f).Scale(1.6f, 0.37f, 0.18f);
	part.transformFace(8, matrix4::translation(0.f, 0.f, 0.02f).scale(1.03f, 1.f, 1.2f));

	part.Extrude(4); part.transformFace(4, matrix4::translation(0.18f, 0.f, -0.02f).scale(1.f, 1.f, 0.7f).rotate(0.1f, vector3f::uz));
#if !ENABLE_BRIDGE_SIZE_OPTIMS
	part.Extrude(0); part.transformFace(0, matrix4::translation(-0.18f, 0.f, -0.02f).scale(1.f, 1.f, 0.7f).rotate(-0.1f, vector3f::uz));
	part.Extrude(8); part.transformFace(8, matrix4::translation(0.f, -0.08f, -0.04f).scale(.85f, 1.f, .45f));
#endif // !ENABLE_BRIDGE_SIZE_OPTIMS
	part.sharpenEdges();
	dest.add(part, matrix4::translation(0.f, -1.05f, 0.13f));

	dest.Transform(matrix4::translation(0.f, 0.3f, 0.f).rotate(PI, vector3f::ux).scale(width, height, length));

	dest.ComputeNormals();

	// Counting backward because removeFace() is going to invalidate
	// indices.
	for (int i = dest.quads.size - 4; i >= 0; i -= 4)
	{
		bool isHorizontal = true;
		for (int v = 0; v < 4; ++v)
		{
			const vertex& vertex = dest.vertices[dest.quads[i + v]];
			if (Algebra::abs(vertex.n.y) < 0.8f) // FIXME: with 0.6f it causes a crash in Mesh::CleanUp().
			{
				isHorizontal = false;
				break;
			}
		}
		bool isBackward = true;
		for (int v = 0; v < 4; ++v)
		{
			const vertex& vertex = dest.vertices[dest.quads[i + v]];
			if (vertex.n.z < 0.8f)
			{
				isBackward = false;
				break;
			}
		}
		if ((isHorizontal || isBackward) == windowPart)
		{
			dest.removeFace(i);
		}
	}

	// Quick and dirty generation of reasonable texture coordinates.
	// The 0.78 more or less makes the texture 4 floors tall.
	GenerateAxisAlignedTextureCoordinates(dest, 0.078f);

	dest.ComputeTangents();
}

#endif // ENABLE_RESEARCH_VESSEL_MESH

#ifdef ENABLE_RUNTIME_COMPILATION
void ResearchVesselBridgeWindowPart::Build(Mesh& dest, Pool& pool)
#else // !ENABLE_RUNTIME_COMPILATION
void ResearchVesselBridgeWindowPart(Mesh& dest, Pool& pool)
#endif // !ENABLE_RUNTIME_COMPILATION
{
#if ENABLE_RESEARCH_VESSEL_MESH
	buildBridge(dest, pool, true);
#endif // ENABLE_RESEARCH_VESSEL_MESH
}

#ifdef ENABLE_RUNTIME_COMPILATION
void ResearchVesselBridgeNoWindowPart::Build(Mesh& dest, Pool& pool)
#else // !ENABLE_RUNTIME_COMPILATION
void ResearchVesselBridgeNoWindowPart(Mesh& dest, Pool& pool)
#endif // !ENABLE_RUNTIME_COMPILATION
{
#if ENABLE_RESEARCH_VESSEL_MESH
	buildBridge(dest, pool, false);
#endif // ENABLE_RESEARCH_VESSEL_MESH
}

#ifdef ENABLE_RUNTIME_COMPILATION
void ResearchVesselCrane::Build(Mesh& dest, Pool& pool)
#else // !ENABLE_RUNTIME_COMPILATION
void ResearchVesselCrane(Mesh& dest, Pool& pool)
#endif // !ENABLE_RUNTIME_COMPILATION
{
#if ENABLE_RESEARCH_VESSEL_MESH
	mesh::Mesh& part = pool.GetTempMesh();

	// Base
	Revolution(pipeHFunc, pipeRFunc).GenerateMesh(part, 5, 20);
	dest.add(part, matrix4::scaling(.15f, .55f, .15f));
	dest.add(part, matrix4::scaling(.1f, 1.f, .1f));

	// Arm
	part.Clear();
	cube(part).Translate(-0.5f, -0.5f, -0.3f).Scale(0.1f, 0.1f, 0.35f);
	part.transformFace(20, matrix4::translation(0.f, 0.02f, 0.f).scale(1.f, 1.7f, 1.f));
	part.Extrude(16); part.transformFace(16, matrix4::translation(0.f, 0.f, -0.03f).scale(1.f, 0.3f, 1.f));
	part.Extrude(20); part.transformFace(20, matrix4::translation(0.f, -0.03f, 0.8f).scale(1.f, 0.6f, 1.f));
	part.Extrude(20); part.transformFace(20, matrix4::translation(0.f, 0.f, 0.05f).scale(.6f, 0.8f, 1.f));
	part.sharpenEdges();

	// Quick and dirty generation of reasonable texture coordinates.
	part.ComputeNormals();
	GenerateAxisAlignedTextureCoordinates(part, 1.f);

	dest.add(part, matrix4::translation(0.f, 1.f, 0.0f)
		.rotate(PI, vector3f::uy)
		.rotate(-0.2f, vector3f::ux));

	dest.Transform(matrix4::scaling(width)); // .translate(.6f, 0.f, 2.f));

	dest.ComputeNormals();
#endif // ENABLE_RESEARCH_VESSEL_MESH
}
