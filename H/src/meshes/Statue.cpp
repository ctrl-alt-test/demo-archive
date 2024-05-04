#include "Statue.hh"

#include "engine/algebra/Functions.hh"
#include "engine/algebra/Interpolation.hxx"
#include "engine/algebra/Matrix.hxx"
#include "engine/algebra/Spline.hh"
#include "engine/algebra/Utils.hh"
#include "engine/algebra/Vector2.hxx"
#include "engine/container/Utils.hh"
#include "engine/core/msys_temp.hh"
#include "engine/mesh/Cube.hh"
#include "engine/mesh/Marching.hh"
#include "engine/mesh/Mesh.hh"
#include "engine/mesh/MeshPool.hh"
#include "engine/mesh/Revolution.hh"
#include "engine/mesh/Utils.hh"
#include "engine/noise/Rand.hh"

#define ENABLE_STATUE_MESHES 1
#define ENABLE_STATUE_ARMY 0
#define ENABLE_STATUE_KILLING 0
#define ENABLE_UGLY_POSEIDON_MESHES 0


#ifdef ENABLE_RUNTIME_COMPILATION
#include "RuntimeCompiledCPlusPlus/RuntimeObjectSystem/ObjectInterfacePerModule.h"

REGISTERCLASS(Statue);
ObjectId Statue::objectId;
Statue* Statue::instance = NULL;
void Statue::BuildMesh(mesh::Mesh& dest, mesh::Pool& pool)
{
	return instance->Build(dest, pool);
}

#if ENABLE_STATUE_KILLING
REGISTERCLASS(StatueKilling);
ObjectId StatueKilling::objectId;
StatueKilling* StatueKilling::instance = NULL;
void StatueKilling::BuildMesh(mesh::Mesh& dest, mesh::Pool& pool)
{
	return instance->Build(dest, pool);
}
#endif // ENABLE_STATUE_KILLING

REGISTERCLASS(FountainStatues);
ObjectId FountainStatues::objectId;
FountainStatues* FountainStatues::instance = NULL;
void FountainStatues::BuildMesh(mesh::Mesh& dest, mesh::Pool& pool)
{
	return instance->Build(dest, pool);
}

#if ENABLE_UGLY_POSEIDON_MESHES
REGISTERCLASS(Poseidon);
ObjectId Poseidon::objectId;
Poseidon* Poseidon::instance = NULL;
void Poseidon::BuildMesh(mesh::Mesh& dest, mesh::Pool& pool)
{
	return instance->Build(dest, pool);
}
#endif

#if ENABLE_STATUE_ARMY
REGISTERCLASS(StatuesArmy);
ObjectId StatuesArmy::objectId;
StatuesArmy* StatuesArmy::instance = NULL;
void StatuesArmy::BuildMesh(mesh::Mesh& dest, mesh::Pool& pool)
{
	return instance->Build(dest, pool);
}
#endif // ENABLE_STATUE_ARMY
#endif

using namespace Algebra;
using namespace mesh;

static void move(Mesh& dest, int face, float d)
{
	vector3f& v1 = dest.vertices[dest.quads[face + 0]].p;
	vector3f& v2 = dest.vertices[dest.quads[face + 1]].p;
	vector3f& v3 = dest.vertices[dest.quads[face + 2]].p;
	vector3f normal = cross(v2 - v1, v3 - v1);
	normalize(normal);
	dest.transformFace(face, matrix4::translation(d * normal));
}

void turn(Mesh& dest, int f, int steps, float d, float angle, float x, float y, float z)
{
	angle /= steps;
	d /= steps;
	for (int i = 0; i < steps; i++)
	{
		dest.transformFace(f, matrix4::rotation(angle * DEG_TO_RAD, x, y, z));
		dest.Extrude(f);
		move(dest, f, d);
	}
}

#if ENABLE_STATUE_MESHES

void turn(Mesh& dest, int f, int steps, float d, matrix4 m)
{
	d /= steps;
	for (int i = 0; i < steps; i++)
	{
		dest.transformFace(f, m);
		dest.Extrude(f);
		move(dest, f, d);
	}
}

static void arm(Mesh& tmp, int face, float a1, float a2, float a3)
{
	int larm = face;
	tmp.Extrude(larm);

	// shoulder
	move(tmp, larm, 1.f);
	tmp.transformFace(larm, matrix4::scaling(1.f, 1.f, 0.75f));
	tmp.Extrude(larm);
	move(tmp, larm, 10.f);

	turn(tmp, larm, 1, 14.f,
		matrix4::rotation(a1 * DEG_TO_RAD, 0,1,0) *
		matrix4::rotation(a2 * DEG_TO_RAD, 0,0,1));

	// arm
	tmp.transformFace(larm, matrix4::scaling(0.6f));
	tmp.Extrude(larm);
	move(tmp, larm, 28.f);
	tmp.transformFace(larm, matrix4::scaling(1.125f));
	// elbow
	turn(tmp, larm, 4, 5.f, a3, 1, 0, 0);
	// forearm
	tmp.transformFace(larm, matrix4::scaling(0.75f));
	move(tmp, larm, 16.f);
	tmp.transformFace(larm, matrix4::scaling(0.9f));
	tmp.Extrude(larm);
	move(tmp, larm, 16.f);
	tmp.Extrude(larm);
	tmp.transformFace(larm, matrix4::scaling(1.5f));
	move(tmp, larm, 4.f);
}

static void leg(Mesh& tmp, int face, float a1, float a2, float a3)
{
	tmp.transformFace(face, matrix4::scaling(0.9f));
	turn(tmp, face, 4, 3.f, a1, 0, 0, 1);
	turn(tmp, face, 4, 3.f, a2, 1, 0, 0);
	move(tmp, face, 35.f);
	tmp.transformFace(face, matrix4::scaling(0.75f));

	//turn(tmp, face, 3, 5.f, 30, 1, 0, 0);
	move(tmp, face, 5.f);
	turn(tmp, face, 2, 5.f, a3, 1, 0, 0);
	tmp.transformFace(face, matrix4::scaling(0.8f));
	move(tmp, face, 30.f);
	turn(tmp, face, 3, 10.f, -90.f, 1, 0, 0);
	tmp.transformFace(face, matrix4::scaling(1.f, 0.5f, 1.f));
	tmp.Extrude(face);
	move(tmp, face, 20.f);
}

static void body(Mesh& tmp, int f, int& larm, int& rarm)
{
	tmp.transformFace(f, matrix4::translation(0.f, 6.f, 0)
		.scale(0.9f, 0.9f, 0.9f));
	tmp.Extrude(f);
	tmp.transformFace(f, matrix4::translation(0.f, 6.f, 0)
		.scale(0.8f, 0.8f, 0.8f));
	tmp.Extrude(f);
	tmp.transformFace(f, matrix4::translation(0.f, 25.f, 0)
		.scale(1.25f, 1.25f, 2.f));
	larm = tmp.quads.size;
	rarm = tmp.quads.size + 8;
	tmp.Extrude(f);
	tmp.transformFace(f, matrix4::translation(0.f, 20.f, 0)
		.scale(1.0f, 1.f, 0.5f));
	// shoulders
	tmp.Extrude(f);
	tmp.transformFace(f, matrix4::translation(0.f, 3.f, 0)
		.scale(0.4f, 0.4f, 0.8f));
	// neck
	tmp.Extrude(f);
	tmp.transformFace(f, matrix4::translation(0.f, 8.f, 0)
		.scale(0.8f, 0.8f, 0.8f));
	// head
	tmp.Extrude(f);
	tmp.transformFace(f, matrix4::translation(0.f, 6.f, 4.f)
		.scale(2.5f, 2.5f, 2.f));
	tmp.Extrude(f);
	tmp.transformFace(f, matrix4::translation(0.f, 18.f, 0)
		.scale(1.f, 1.f, 1.f));
	tmp.Extrude(f);
	tmp.transformFace(f, matrix4::translation(0.f, 8.f, 0)
		.scale(0.5f, 0.5f, 0.9f));
	tmp.Extrude(f);
	tmp.transformFace(f, matrix4::translation(0.f, 2.f, 0)
		.scale(0.5f, 0.5f, 0.5f));
}

static void spear(Mesh& dest)
{
	// A spear was around 2.00m - 2.75m, with a diameter of 2.5cm.
	cube(dest).Scale(2.5f, 230.f, 2.5f);
	int f = 12;
	dest.Extrude(f);
	dest.transformFace(f, matrix4::translation(0.f, 2.f, 0)
		.scale(4.f, 2.f, 2.f));
	dest.Extrude(f);
	dest.transformFace(f, matrix4::translation(0.f, 25.f, 0)
		.scale(0.01f, 0.01f, 0.1f));
}

static void statue(Mesh& dest, Mesh& tmp, Noise::Rand& rnd, float* args)
{
	// Unit is centimeter, we rescale at the end.
	float width = 40.f;
	tmp.Clear();
	cube(tmp).Scale(width, 5.f, 30.f);

	int larm;
	int rarm;
	body(tmp, 12, larm, rarm);

	arm(tmp, larm, args[0], args[1], args[2]);
	arm(tmp, rarm, -args[3], -args[4], args[5]);

	// split face (poor man's version)
	int lleg = 8;
	tmp.Extrude(lleg);
	int rleg = tmp.quads.size - 12;
	tmp.vertices[tmp.quads[lleg+1]].p.x -= width*0.5f;
	tmp.vertices[tmp.quads[lleg+2]].p.x -= width*0.5f;

	leg(tmp, lleg, -10, args[6], args[7]);
	leg(tmp, rleg, 5.f, args[8], args[9]);

	AddNoise(tmp, rnd, 1.f);
	tmp.smooth();

	dest.add(tmp, matrix4::scaling(0.01f, 0.01f, 0.01f));
}

static void randomStatue(Mesh& dest, Mesh& tmp, Noise::Rand& rnd)
{
	float args[] = {
		// right arm
		90, rnd.fgen(-45, 120), rnd.fgen(-90, 0),
		// left arm
		90, rnd.fgen(-45, 120), rnd.fgen(-90, 0),
		// right leg
		rnd.fgen(-45, 45), rnd.fgen(0, 90),
		// left leg
		rnd.fgen(-45, 45), rnd.fgen(0, 90),
	};
	statue(dest, tmp, rnd, args);
	dest.Rotate(rnd.fgen(-PI/4, PI/4), 0.f, 1.f, 0.f);
}

static void guardingStatue(Mesh& dest, Mesh& tmp, Noise::Rand& rnd)
{
	float args[] = {
		// right arm
		90, 45, -45,
		// left arm
		rnd.fgen(70.f, 90.f), rnd.fgen(70.f, 90.f), 0,
		// right leg
		rnd.fgenOnAverage(-20.f, 5.f), rnd.fgenOnAverage(20.f, 5.f),
		// left leg
		rnd.fgen(-8.f, 0.f), rnd.fgen(0.f, 8.f),
	};
	statue(dest, tmp, rnd, args);
	//dest.Rotate(rnd.fgen(-PI/4, PI/4), 0.f, 1.f, 0.f);
}

#endif // ENABLE_STATUE_MESHES

#ifdef ENABLE_RUNTIME_COMPILATION
void FountainStatues::Build(Mesh& dest, Pool& pool)
#else // !ENABLE_RUNTIME_COMPILATION
void FountainStatues(Mesh& dest, Pool& pool)
#endif // !ENABLE_RUNTIME_COMPILATION
{
#if ENABLE_STATUE_MESHES
	Noise::Rand rnd;
	Mesh& s1 = pool.GetTempMesh();
	Mesh& s2 = pool.GetTempMesh();

	for (int i = 0; i < 360; i += 16)
	{
		s1.Clear();
		randomStatue(s1, s2, rnd);
		float x = 4.25f * msys_cosf(i * DEG_TO_RAD);
		float y = 4.25f * msys_sinf(i * DEG_TO_RAD);
		float angle = - i * DEG_TO_RAD + PI/2;
		dest.add(s1, matrix4::translation(x, 0, y)
			.rotate(angle, vector3f::uy));
	}
	dest.Scale(0.16f);
	dest.ComputeNormals();
	GenerateAxisAlignedTextureCoordinates(dest, 0.5f);
	dest.ComputeTangents();
#endif // ENABLE_STATUE_MESHES
}

#if ENABLE_UGLY_POSEIDON_MESHES

#ifdef ENABLE_RUNTIME_COMPILATION
void Poseidon::Build(Mesh& dest, Pool& pool)
#else // !ENABLE_RUNTIME_COMPILATION
void Poseidon(Mesh& dest, Pool& pool)
#endif // !ENABLE_RUNTIME_COMPILATION
{
#if ENABLE_STATUE_MESHES
	Noise::Rand rnd;
	//Mesh& s1 = pool.GetTempMesh();
	Mesh& s2 = pool.GetTempMesh();

	float args[] = {
		// right arm
		60, 20, -45,
		// left arm
		90, 90, -25,
		// right leg
		-85, 70,
		// left leg
		-85, 90,
	};
	statue(dest, s2, rnd, args);
	dest.Scale(6.f);
	dest.Rotate(PI, 0.f, 1.f, 0.f);
	dest.ComputeNormals();
	GenerateAxisAlignedTextureCoordinates(dest, 0.1f);
	dest.ComputeTangents();
#endif // ENABLE_STATUE_MESHES
}
#endif // ENABLE_UGLY_POSEIDON_MESHES

#if ENABLE_STATUE_KILLING
#ifdef ENABLE_RUNTIME_COMPILATION
void StatueKilling::Build(Mesh& dest, Pool& pool)
#else // !ENABLE_RUNTIME_COMPILATION
void StatueKilling(Mesh& dest, Pool& pool)
#endif // !ENABLE_RUNTIME_COMPILATION
{
#if ENABLE_STATUE_MESHES
	Noise::Rand rnd;
	Mesh& s1 = pool.GetTempMesh();
	Mesh& s2 = pool.GetTempMesh();

	s1.Clear();
	float args[] = {
		90, -30, -50,  90, 70, -30,    5, 25, -30, 25
	};
	statue(s1, s2, rnd, args);
	s1.Rotate(15*DEG_TO_RAD, 1.f, 0.f, 0.f);
	dest.add(s1, matrix4::translation(0,0,-1.25));

	s1.Clear();
	float args2[] = {
		90, 50, -20,  90, 60, -40,   -30, 20, -15, 20
	};
	statue(s1, s2, rnd, args2);
	s1.Rotate(180*DEG_TO_RAD, 0.f, 1.f, 0.f);
	s1.Rotate(55*DEG_TO_RAD, 1.f, 0.f, 0.f);
	dest.add(s1, matrix4::translation(0.1f,-0.75f,0.75f));

	Mesh& sp = pool.GetTempMesh();
	spear(sp);
	sp.Scale(0.02f, 0.01f, 0.02f);
	sp.Rotate(135*DEG_TO_RAD, 1.f, 0.f, 0.f);
	dest.add(sp, matrix4::translation(-0.1f,1.4f,-1.f));

	dest.ComputeNormals();
	GenerateAxisAlignedTextureCoordinates(dest, 0.125f);
	dest.ComputeTangents();
#endif // ENABLE_STATUE_MESHES
}
#endif // ENABLE_STATUE_KILLING

#ifdef ENABLE_RUNTIME_COMPILATION
void Statue::Build(Mesh& dest, Pool& pool)
#else // !ENABLE_RUNTIME_COMPILATION
void Statue(Mesh& dest, Pool& pool)
#endif // !ENABLE_RUNTIME_COMPILATION
{
#if ENABLE_STATUE_MESHES
	Noise::Rand rnd;
	Mesh& s1 = pool.GetTempMesh();
	Mesh& s2 = pool.GetTempMesh();

	//s1.Clear();
	//s2.Clear();
	//float args3[] = {
	//	90, -40, -50,  90, 70, -30,    -15, 25, 5, 25
	//};
	//statue(s1, pool, rnd, args3);
	//s1.Rotate(-5*DEG_TO_RAD, 1, 0, 0);
	//dest.add(s1,
	//	matrix4::translation(0,0,-3.5f));
	//dest.add(sp, matrix4::translation(-0.1f,1.75f,-3.75f));

	//float args4[] = {
	//	90, -30, -50,  90, -40, -30,    -10, 100, 5, 100
	//};
	//statue(s2, pool, rnd, args4);
	//s2.Rotate(180*DEG_TO_RAD, 0, 1, 0);
	//dest.add(s2, matrix4::translation(0.1f,-0.5f,-1.f));

	//s1.Clear();
	//float args5[] = {
	//	90, 120, -60,  90, 70, -50,    -45, 45, -15, 45
	//};
	//statue(s1, pool, rnd, args5);
	//s1.Rotate(-100*DEG_TO_RAD, 0, 1, 0);
	//dest.add(s1, matrix4::translation(0.1f,-0.5f, 3.f));

	for (int i = -24; i < 24; i++)
	{
		s1.Clear();
		randomStatue(s1, s2, rnd);
		float angle = rnd.fgen(-PI/4, PI/4);
		if (i > -3 && i < 2) continue;
		if (i > 0) angle += PI;
		dest.add(s1, matrix4::translation(0, -0.25f, (float)i)
			.rotate(angle, vector3f::uy));
	}

	Mesh& sp = pool.GetTempMesh();
	spear(sp);
	sp.Scale(0.02f, 0.01f, 0.02f);
	sp.Rotate(125*DEG_TO_RAD, 1.f, 0.f, 0.f);
	//dest.add(sp, matrix4::translation(-0.1f,1.4f,-1.f));

	s1.Clear();
	float args3[] = {
		90, -40, -50,  90, 70, -30,    -15, 25, 5, 25
	};
	statue(s1, s2, rnd, args3);
	s1.Rotate(-5*DEG_TO_RAD, 1.f, 0.f, 0.f);
	dest.add(s1,
		matrix4::translation(0,0,-2.f));
	dest.add(sp, matrix4::translation(-0.1f,1.75f,-2.25f));

	s1.Clear();
	float args4[] = {
		90, -30, -50,  90, -40, -30,    -10, 100, 5, 100
	};
	statue(s1, s2, rnd, args4);
	s1.Rotate(180*DEG_TO_RAD, 0.f, 1.f, 0.f);
	dest.add(s1, matrix4::translation(0.1f, -0.5f, 0.5f));

	dest.ComputeNormals();
	GenerateAxisAlignedTextureCoordinates(dest, 0.125f);
	dest.ComputeTangents();
#endif // ENABLE_STATUE_MESHES
}

#if ENABLE_STATUE_ARMY
#ifdef ENABLE_RUNTIME_COMPILATION
void StatuesArmy::Build(Mesh& dest, Pool& pool)
#else // !ENABLE_RUNTIME_COMPILATION
void StatuesArmy(Mesh& dest, Pool& pool)
#endif // !ENABLE_RUNTIME_COMPILATION
{
#if ENABLE_STATUE_MESHES
	Mesh& s1 = pool.GetTempMesh();
	Mesh& s2 = pool.GetTempMesh();

	Mesh& sp = pool.GetTempMesh();
	spear(sp);
	sp.Scale(0.02f, 0.01f, 0.02f);

	for (int i = 0; i < 10; i += 1)
	for (int j = 0; j < 10; j += 1)
	{
		Noise::Rand rnd(i + 123 * j);
		s1.Clear();
		guardingStatue(s1, s2, rnd);
		float x = i * 1.5f + rnd.fgenOnAverage(0.f, 0.0625f);
		float z = j * 1.5f + rnd.fgenOnAverage(0.f, 0.0625f);
		float angle = - i * DEG_TO_RAD + PI/2;
		float size = rnd.fgen(1.f, 1.2f);
		dest.add(s1, matrix4::translation(x, size - 1.f, z)
			.rotate(angle, vector3f::uy)
			.scale(1.f, size, 1.f));
		dest.add(sp, matrix4::translation(x+0.8f,-0.4f,z+0.2f));
	}
	dest.ComputeNormals();
	GenerateAxisAlignedTextureCoordinates(dest, 0.5f);
	dest.ComputeTangents();
#endif // ENABLE_STATUE_MESHES
}
#endif // ENABLE_STATUE_ARMY
