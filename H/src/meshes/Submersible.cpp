#include "Submersible.hh"

#include "engine/algebra/Functions.hh"
#include "engine/algebra/Interpolation.hxx"
#include "engine/algebra/Matrix.hxx"
#include "engine/algebra/Utils.hh"
#include "engine/container/Utils.hh"
#include "engine/core/RigidTransform.hxx"
#include "engine/mesh/Cube.hh"
#include "engine/mesh/Mesh.hh"
#include "engine/mesh/MeshPool.hh"
#include "engine/mesh/Revolution.hh"
#include "engine/mesh/Utils.hh"
#include "H/TheSubmersible.hh"

#define ENABLE_SUBMERSIBLE_MESH 1

#ifdef ENABLE_RUNTIME_COMPILATION
#include "RuntimeCompiledCPlusPlus/RuntimeObjectSystem/ObjectInterfacePerModule.h"

REGISTERCLASS(Submersible);
ObjectId Submersible::objectId;
Submersible* Submersible::instance = NULL;
void Submersible::BuildMesh(mesh::Mesh& dest, mesh::Pool& pool)
{
	return instance->Build(dest, pool);
}

REGISTERCLASS(SubmersibleFrame);
ObjectId SubmersibleFrame::objectId;
SubmersibleFrame* SubmersibleFrame::instance = NULL;
void SubmersibleFrame::BuildMesh(mesh::Mesh& dest, mesh::Pool& pool)
{
	return instance->Build(dest, pool);
}

REGISTERCLASS(SubmersibleSpots);
ObjectId SubmersibleSpots::objectId;
SubmersibleSpots* SubmersibleSpots::instance = NULL;
void SubmersibleSpots::BuildMesh(mesh::Mesh& dest, mesh::Pool& pool)
{
	return instance->Build(dest, pool);
}
#endif

using namespace Algebra;
using namespace mesh;

#ifdef ENABLE_RUNTIME_COMPILATION
void Submersible::Build(Mesh& dest, Pool& pool)
#else // !ENABLE_RUNTIME_COMPILATION
void Submersible(Mesh& dest, Pool& pool)
#endif // !ENABLE_RUNTIME_COMPILATION
{
#if ENABLE_SUBMERSIBLE_MESH
	cube(dest).Translate(-0.5f, -0.5f, -0.5f).Scale(2.8f, 3.f, 3.5f);

	// Up rear.
	dest.Extrude(16); dest.transformFace(16, matrix4::translation(0.f, 0.5f, -0.5f).scale(0.8f, 0.25f, 1.f));

	// Top front
	dest.transformFace(12, matrix4::translation(0.f, 0.f, 0.75f).scale(1.f, 0.f, 1.5f));

	// Sphere cover.
	dest.Extrude(20); dest.transformFace(20, matrix4::translation(0.f, 0.f, 0.f).scale(0.9f, 0.7f, 0.7f));
	dest.Extrude(20); dest.transformFace(20, matrix4::translation(0.f, 0.f, -1.5f).scale(0.75f));

	// Curved bottom.
	dest.transformFace(8, matrix4::translation(0.f, 0.f, 0.f).scale(0.9f, 1.f, 1.f));
	dest.Extrude(8); dest.transformFace(8, matrix4::translation(0.f, 0.5f, 0.5f).scale(0.5f, 0.f, 0.75f));

	//*/
	dest.smooth();
	dest.smooth();
	dest.smooth();
	/*/
	dest.sharpenEdges();
	//*/
	GenerateAxisAlignedTextureCoordinates(dest, 0.25f);

	struct local
	{
		static vertex scaleTexture(int, int, const vertex& inputVertex)
		{
			vertex result = inputVertex;
			result.u *= 2.f;
			result.v = mix(0.1f, 0.9f, result.v);
			return result;
		}

		static vertex expand(int, int, const vertex& inputVertex)
		{
			vertex result = inputVertex;
			result.p.z = (Algebra::pow(Algebra::abs(2.f * result.p.z), 0.4f) / 2.f + 1.4f) * (result.p.z > 0.f ? 1.f : -1.f);
			result.v = 0.45f + result.p.z * 0.2f;
			return result;
		}
	};

	// View port.
	Mesh& part = pool.GetTempMesh();
	Revolution(sphereHFunc, sphereRFunc).GenerateMesh(part, 24, 36);
	part.ApplyFunction(local::scaleTexture);
	dest.add(part, matrix4::translation(0.f, -0.2f, 2.6f).rotate(PI / 2.f, vector3f::uy).scale(1.2f));
	part.Clear();

	// Ballast.
	Revolution(sphereHFunc, sphereRFunc).GenerateMesh(part, 17, 20);
	part.Rotate(PI / 2.f, 1.f, 0.f, 0.f).Scale(0.4f);
	part.ApplyFunction(local::expand);
	dest.add(part, matrix4::translation(1.3f, -1.f, 0.4f).rotate(PI, vector3f::uy));
	dest.add(part, matrix4::translation(-1.3f, -1.f, 0.4f));
	part.Clear();

	// Hatch.
	float hatchCurve[] = {
		0, -0.15f, 0.45f,
		0, 0.f, 0.45f,
		0, 0.04f, 0.45f,
		0, 0.05f, 0.38f,
		0, 0.075f, 0.34f,
		0, 0.10f, 0.0f,
	};
	initSplineData(hatchCurve, ARRAY_LEN(hatchCurve), 3);
	Timeline::Variable v(hatchCurve, 3, ARRAY_LEN(hatchCurve) / 3, Timeline::modeSpline);
	Revolution(NULL, NULL, NULL, &v).GenerateMesh(part, (ARRAY_LEN(hatchCurve)*2)/3, 20);
	part.ApplyFunction(local::scaleTexture);
	dest.add(part, matrix4::translation(0.f, 1.38f, 1.f));

	// Using the hatch shape as a decoration.
	dest.add(part, matrix4::translation(-1.23f, -0.35f, -0.5f).rotate(PI / 2.f, vector3f::uz).scale(0.4f));
	dest.add(part, matrix4::translation(-1.23f, -0.35f, -0.f).rotate(PI / 2.f, vector3f::uz).scale(0.4f));

	dest.ComputeNormals();
	dest.ComputeTangents();
#endif // ENABLE_SUBMERSIBLE_MESH
}

#if ENABLE_SUBMERSIBLE_MESH

// SIZE: I have tried to use the symmetry to reduce size, but it was
// counter productive.

static float s_topFrameSplineData[] = {
	0, 0.01f, 0.2f, -2.4f,
	0, -1.2f, 0.2f, -2.2f,
	0, -1.5f, 0.2f, -1.5f,
	0, -1.5f, 0.2f, 0.5f,
	0, -1.5f, 0.2f, 1.5f,
	0, -1.5f, 0.5f, 2.f,
	0, -1.5f, 0.5f, 3.f,
	0, -0.75f, 0.5f, 3.8f,
	0, 0.75f, 0.5f, 3.8f,
	0, 1.5f, 0.5f, 3.f,
	0, 1.5f, 0.5f, 2.f,
	0, 1.5f, 0.2f, 1.5f,
	0, 1.5f, 0.2f, 0.5f,
	0, 1.5f, 0.2f, -1.5f,
	0, 1.2f, 0.2f, -2.2f,
	0, -0.01f, 0.2f, -2.4f,
};

static float s_middleFrameSplineData[] = {
	0, -1.5f, -0.4f, 2.5f,
	0, -1.5f, -0.4f, 1.f,
	0, -1.5f, -0.4f, -1.5f,
	0, -1.2f, -0.4f, -2.2f,
	0, 0.0f, -0.4f, -2.4f,
	0, 1.2f, -0.4f, -2.2f,
	0, 1.5f, -0.4f, -1.5f,
	0, 1.5f, -0.4f, 1.f,
	0, 1.5f, -0.4f, 2.5f,
};

// HACK: for some reason, the original mesh was generated inside out.
// So instead I wrote it upside down (y inverted), and rotated it once
// generated.
static float s_bottomFrameSplineData[] = {
	0,  1.5f, -0.5f, 2.5f,
	0,  1.5f, -0.0f, 2.5f,
	0,  1.5f, 0.4f, 2.5f,
	0,  1.4f, 0.7f, 2.8f,
	0,  1.0f, 1.2f, 3.4f,
	0,  0.0f, 1.3f, 3.5f,
	0, -1.0f, 1.2f, 3.4f,
	0, -1.4f, 0.7f, 2.8f,
	0, -1.5f, 0.4f, 2.5f,
	0, -1.5f, -0.0f, 2.5f,
	0, -1.5f, -0.5f, 2.5f,
};

static float s_verticalFrameSplineData[] = {
	0,  1.35f, -0.7f, 0.f,
	0,  1.5f, -0.4f, 0.f,
	0,  1.5f,  0.0f, 0.f,
	0,  1.5f,  0.3f, 0.f,
	0,  1.2f,  0.5f, 0.f,
};

#endif // ENABLE_SUBMERSIBLE_MESH

#ifdef ENABLE_RUNTIME_COMPILATION
void SubmersibleFrame::Build(Mesh& dest, Pool& pool)
#else // !ENABLE_RUNTIME_COMPILATION
void SubmersibleFrame(Mesh& dest, Pool& pool)
#endif // !ENABLE_RUNTIME_COMPILATION
{
#if ENABLE_SUBMERSIBLE_MESH
	initSplineData(s_topFrameSplineData, ARRAY_LEN(s_topFrameSplineData), 4);
	initSplineData(s_middleFrameSplineData, ARRAY_LEN(s_middleFrameSplineData), 4);
	initSplineData(s_bottomFrameSplineData, ARRAY_LEN(s_bottomFrameSplineData), 4);
	initSplineData(s_verticalFrameSplineData, ARRAY_LEN(s_verticalFrameSplineData), 4);

	struct local
	{
		static vector3f topFrameHFunc(float t, float)
		{
			vector3f result;
			spline(s_topFrameSplineData, ARRAY_LEN(s_topFrameSplineData) / 4, 3, t, &result.x);
			return result;
		}

		static vector3f middleFrameHFunc(float t, float)
		{
			vector3f result;
			spline(s_middleFrameSplineData, ARRAY_LEN(s_middleFrameSplineData) / 4, 3, t, &result.x);
			return result;
		}

		static vector3f bottomFrameHFunc(float t, float)
		{
			vector3f result;
			spline(s_bottomFrameSplineData, ARRAY_LEN(s_bottomFrameSplineData) / 4, 3, t, &result.x);
			return result;
		}

		static vector3f verticalFrameHFunc(float t, float)
		{
			vector3f result;
			spline(s_verticalFrameSplineData, ARRAY_LEN(s_verticalFrameSplineData) / 4, 3, t, &result.x);
			return result;
		}

		static float frameRFunc(float /* t */, float /* thetaLevel */)
		{
			return 0.06f;
		}
	};

	Mesh& part = pool.GetTempMesh();
	mesh::Revolution(local::topFrameHFunc, local::frameRFunc).GenerateMesh(part, ARRAY_LEN(s_topFrameSplineData), 8);
	dest.add(part);

	part.Clear();
	mesh::Revolution(local::middleFrameHFunc, local::frameRFunc).GenerateMesh(part, ARRAY_LEN(s_middleFrameSplineData), 8);
	dest.add(part);

	part.Clear();
	mesh::Revolution(local::bottomFrameHFunc, local::frameRFunc).GenerateMesh(part, ARRAY_LEN(s_bottomFrameSplineData), 8);
	// HACK: for some reason, the original mesh was generated inside
	// out. So instead I wrote it upside down (y inverted), and here
	// it is rotated.
	dest.add(part, matrix4::rotation(PI, 0.f, 0.f, 1.f));

	part.Clear();
	mesh::Revolution(local::verticalFrameHFunc, local::frameRFunc).GenerateMesh(part, ARRAY_LEN(s_verticalFrameSplineData)/4-1, 8);
	dest.add(part, matrix4::translation(0.f, 0.f, 1.3f));
	dest.add(part, matrix4::translation(0.f, 0.f, -0.5f));
	dest.add(part, matrix4::translation(0.f, 0.f, 1.3f).rotate(PI, vector3f::uy));
	dest.add(part, matrix4::translation(0.f, 0.f, -0.5f).rotate(PI, vector3f::uy));

	dest.ComputeNormals();
	dest.ComputeTangents();
#endif // ENABLE_SUBMERSIBLE_MESH
}

#ifdef ENABLE_RUNTIME_COMPILATION
void SubmersibleSpots::Build(Mesh& dest, Pool& pool)
#else // !ENABLE_RUNTIME_COMPILATION
void SubmersibleSpots(Mesh& dest, Pool& pool)
#endif // !ENABLE_RUNTIME_COMPILATION
{
#if ENABLE_SUBMERSIBLE_MESH
	float spotCurve[] = {
		0, -0.25f, 0.f,
		0, -0.25f, 0.02f,
		0, -0.20f, 0.10f,
		0, -0.05f, 0.11f,
		0, -0.04f, 0.12f,
		0, 0.f, 0.12f,
		0, 0.f, 0.f,
	};
	initSplineData(spotCurve, ARRAY_LEN(spotCurve), 3);
	Timeline::Variable v(spotCurve, 3, ARRAY_LEN(spotCurve) / 3, Timeline::modeSpline);

	Mesh& part = pool.GetTempMesh();
	Revolution(NULL, NULL, NULL, &v).GenerateMesh(part, (ARRAY_LEN(spotCurve)*2)/3, 12);
	part.Translate(0.f, 0.35f, 0.f).Rotate(-PI / 2.f, 1.f, 0.f, 0.f);

	Core::RigidTransform spotLightTransforms[3];
	TheSubmersible::GetLights(nullptr, nullptr, spotLightTransforms);

	for (size_t i = 0; i < ARRAY_LEN(spotLightTransforms); ++i)
	{
		dest.add(part, ComputeMatrix(spotLightTransforms[i]));
	}

	dest.ComputeNormals();
	dest.ComputeTangents();
#endif // ENABLE_SUBMERSIBLE_MESH
}
