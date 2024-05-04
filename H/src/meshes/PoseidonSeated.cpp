#include "PoseidonSeated.hh"

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
#include "tool/mesh/VertexDataPNT.hh"

#define ENABLE_POSEIDON_SEATED_MESH 1

#ifdef ENABLE_RUNTIME_COMPILATION
#include "RuntimeCompiledCPlusPlus/RuntimeObjectSystem/ObjectInterfacePerModule.h"

REGISTERCLASS(PoseidonSeated);
ObjectId PoseidonSeated::objectId;
PoseidonSeated* PoseidonSeated::instance = NULL;
void PoseidonSeated::BuildMesh(mesh::Mesh& dest, mesh::Pool& pool)
{
	return instance->Build(dest, pool);
}

REGISTERCLASS(PoseidonEyes);
ObjectId PoseidonEyes::objectId;
PoseidonEyes* PoseidonEyes::instance = NULL;
void PoseidonEyes::BuildMesh(mesh::Mesh& dest, mesh::Pool& pool)
{
	return instance->Build(dest, pool);
}
#endif


using namespace Algebra;
using namespace mesh;

#if ENABLE_POSEIDON_SEATED_MESH
static float segment(const vector3f& p,
					 const vector3f& a,
					 const vector3f& b,
					 float& h)
{
	const vector3f ap = p - a;
	const vector3f ab = b - a;
	h = clamp(dot(ap, ab) / dot(ab, ab), 0.f, 1.f);
	return norm(ap - ab * h);
}

static float limb(const vector3f& p,
				  const vector3f& a,
				  const vector3f& b,
				  float ra,
				  float rb,
				  float curvePower,
				  float curveHeight)
{
	float da = norm(a - p) - ra;
	float db = norm(b - p) - rb;
	float h;
	float dmid = segment(p, a, b, h);

	float bump = msys_sinf(2.f * PI * (Algebra::pow(h, curvePower) - 0.25f)) * 0.75f + 0.25f;
	dmid -= mix(ra, rb, h) * (1.f + curveHeight * bump);

	float d = msys_min(dmid, msys_min(da, db));
	return d;
}

static float PoseidonSDF(float x, float y, float z)
{
	Algebra::vector3f p = { x, y, z };

	// Radius and positions of all the joints.
	const float joints[] = {
		0.03f,		0.15f, 0.f, 0.36f,			// Left leg toe tip
		0.05f,		0.1f, 0.f,  0.23f,			//              ankle
		0.06f,		0.21f, 0.44f, 0.24f,		//              knee
		0.125f,		0.1f, 0.47f, -0.25f,		//              hip
		0.03f,		-0.18f, 0.f, 0.44f,			// Right leg toe tip
		0.05f,		-0.14f, 0.02f, 0.27f,		//               ankle
		0.06f,		-0.14f, 0.48f, 0.25f,		//               knee
		0.125f,		-0.1f, 0.47f, -0.23f,		//               hip
		0.01f,		0.36f, 0.72f, 0.2f,			// Left hand finger tip
		0.04f,		0.38f, 0.8f,  0.1f,			//           wrist
		0.045f,		0.38f, 0.8f, -0.18f,		//           elbow
		0.07f,		0.2f,  0.96f, -0.32f,		//           shoulder
		0.01f,		-0.38f, 1.08f, 0.35f,		// Right hand finger tip
		0.04f,		-0.38f, 1.08f, 0.3f,		//            wrist
		0.045f,		-0.33f, 0.99f,  0.f,		//            elbow
		0.07f,		-0.2f, 1.f, -0.27f,			//            shoulder
		0.15f,		0.f, 0.65f, -0.26f,			// Belly
		0.09f,		0.f, 1.03f, -0.32f,			// Neck
		0.08f,		0.f, 1.26f, -0.28f,			// Head
		0.077f,		0.f, 1.262f, -0.23f,		// Forehead
		0.075f,		0.f, 1.17f, -0.23f,			// Jaw
		0.025f,		0.f, 1.19f, -0.142f,		// Nose
	};

	// Bones described as paris of joints.
	const char links[] = {
		// Left leg.
		0, 1,	1, 2,	2, 3,
		// Right leg.
		4, 5,	5, 6,	6, 7,
		// Left arm.
		8, 9,	9, 10,	10, 11,
		// Right arm.
		12, 13,	13, 14,	14, 15,
		// Lower back.
		3, 16,	7, 16,
		// Upper back.
		11, 16,	15, 16,
		// Shoulders and neck.
		11, 17,	15, 17,	16, 17,
		// Head.
		17, 18,	18, 19,	19, 20, 19, 21,
	};

	// Shape of the flesh around the bones.
	const float linksCurvePower[] = {
		// Left leg.
		1.f, 1.8f, 0.8f,
		// Right leg.
		1.f, 1.6f, 0.9f,
		// Left arm.
		1.f, 2.2f, 0.8f,
		// Right arm.
		1.f, 2.2f, 0.8f,
		// Lower back.
		1.f, 1.f,
		// Upper back.
		2.5f, 1.8f,
		// Shoulders and neck.
		1.f, 1.f, 1.15f,
		// Head.
		0.9f, 0.8f, 0.6f, 0.8f,
	};

	// Thickness of the flesh around the bones.
	const float linksCurveHeight[] = {
		// Left leg.
		0.f, 0.35f, 0.25f,
		// Right leg.
		0.f, 0.3f, 0.2f,
		// Left arm.
		0.f, 0.25f, 0.2f,
		// Right arm.
		0.f, 0.25f, 0.2f,
		// Lower back.
		0.11f, 0.11f,
		// Upper back.
		0.075f, 0.1f,
		// Shoulders and neck.
		0.f, 0.f, 0.18f,
		// Head.
		-0.2f, 0.1f, -0.1f, 0.35f,
	};

	float d = 1e30f;
#if 0
	for (size_t i = 0; i < ARRAY_LEN(joints); i += 4)
	{
		float r = joints[i];
		vector3f v = { joints[i + 1], joints[i + 2], joints[i + 3] };
		d = msys_min(d, norm(p - v) - r);
	}
#else
	for (size_t i = 0; i < ARRAY_LEN(links); i += 2)
	{
		const float* joint1 = joints + 4 * links[i];
		float r1 = joint1[0];
		vector3f v1 = { joint1[1], joint1[2], joint1[3] };

		const float* joint2 = joints + 4 * links[i + 1];
		float r2 = joint2[0];
		vector3f v2 = { joint2[1], joint2[2], joint2[3] };

		d = msys_min(d, limb(p, v1, v2, r1, r2, linksCurvePower[i/2], linksCurveHeight[i/2]));
	}
#endif
	return d;
}
#endif // ENABLE_POSEIDON_SEATED_MESH

#ifdef ENABLE_RUNTIME_COMPILATION
void PoseidonSeated::Build(Mesh& dest, Pool& /*pool*/)
#else // !ENABLE_RUNTIME_COMPILATION
void PoseidonSeated(Mesh& dest, Pool& /*pool*/)
#endif // !ENABLE_RUNTIME_COMPILATION
{
#if ENABLE_POSEIDON_SEATED_MESH
	Marching(PoseidonSDF).GenerateMesh(dest, -0.5f, -0.1f, -0.5f, 0.5f, 1.4f, 0.5f, 30, 45, 25);
	dest.ComputeNormals();
	dest.Translate(0.f, -.5f, 0.f);
	dest.Scale(7.5f);

	GenerateAxisAlignedTextureCoordinates(dest, 0.1f);
	dest.ComputeTangents();
#endif // ENABLE_POSEIDON_SEATED_MESH
}


#ifdef ENABLE_RUNTIME_COMPILATION
void PoseidonEyes::Build(Mesh& dest, Pool& /*pool*/)
#else // !ENABLE_RUNTIME_COMPILATION
void PoseidonEyes(Mesh& dest, Pool& /*pool*/)
#endif // !ENABLE_RUNTIME_COMPILATION
{
	cube(dest).Scale(0.175f);
	dest.add(dest, matrix4::translation(0.5f, 0.f, 0.f));
	dest.smooth();
	dest.Scale(1.f, 0.5f, 0.5f);
	dest.ComputeNormals();
	GenerateAxisAlignedTextureCoordinates(dest, 0.1f);
	dest.ComputeTangents();
}
