#include "Houses.hh"

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

#define ENABLE_HOUSES_MESH 1

#ifdef ENABLE_RUNTIME_COMPILATION
#include "RuntimeCompiledCPlusPlus/RuntimeObjectSystem/ObjectInterfacePerModule.h"

REGISTERCLASS(Houses);
ObjectId Houses::objectId;
Houses* Houses::instance = NULL;
void Houses::BuildMesh(mesh::Mesh& dest, mesh::Pool& pool)
{
	return instance->Build(dest, pool);
}
#endif

using namespace Algebra;
using namespace mesh;

#if ENABLE_HOUSES_MESH

void house(Mesh& dest, Mesh& tmp, Noise::Rand& rand, float sizex, float sizey, float sizez)
{
	if (rand.boolean(0.2f)) return;

	sizex *= rand.fgenOnAverage(1.f, 0.2f);
	sizey *= rand.fgenOnAverage(1.f, 0.2f);
	sizez *= rand.fgenOnAverage(1.f, 0.2f);

	tmp.Clear();
	cube(tmp).Scale(sizex, sizey, sizez);
	int f = 12;
	tmp.Extrude(f);
	if (rand.boolean(0.8f))
	{
		tmp.transformFace(f, matrix4::scaling(1.1f, 1.f, 1.1f));
		tmp.Extrude(f);
		tmp.transformFace(f, matrix4::translation(0, sizey * 2 / 3, 0));
		if (rand.boolean(0.8f))
		{
			tmp.transformFace(f, matrix4::scaling(0.75f, 1.f, 0.01f));
		}
		else
		{
			tmp.transformFace(f, matrix4::scaling(1.f, 1.f, 0.01f));
		}
	}
	else
	{
		tmp.Extrude(f);
		tmp.transformFace(f, matrix4::scaling(0.95f, 1.f, 0.95f));
		tmp.transformFace(f, matrix4::translation(0, -0.25f, 0));
	}
	dest.add(tmp);
}

void block(Mesh& dest, Mesh& tmp, Mesh& h, Noise::Rand& rand)
{
	house(dest, tmp, rand, 11, 3, 4);

	h.Clear();
	house(h, tmp, rand, 13, 3.5, 4);
	dest.add(h, matrix4::translation(10, 0, 0));

	h.Clear();
	house(h, tmp, rand, 8, 2, 4);
	dest.add(h, matrix4::translation(18, 0, 1).rotate(90.f*DEG_TO_RAD, vector3f::uy));

	h.Clear();
	house(h, tmp, rand, 6, 2, 5);
	dest.add(h, matrix4::translation(10, 0, 1).rotate(90.f*DEG_TO_RAD, vector3f::uy));

	h.Clear();
	house(h, tmp, rand, 5, 2, 3);
	dest.add(h, matrix4::translation(0, 0, 1).rotate(90.f*DEG_TO_RAD, vector3f::uy));

	h.Clear();
	house(h, tmp, rand, 13, 2.5, 4);
	dest.add(h, matrix4::translation(10, 0, -14));

	h.Clear();
	house(h, tmp, rand, 8, 3, 4);
	dest.add(h, matrix4::translation(0, 0, -10));
}

#endif // ENABLE_HOUSES_MESH

#ifdef ENABLE_RUNTIME_COMPILATION
void Houses::Build(Mesh& dest, Pool& pool)
#else // !ENABLE_RUNTIME_COMPILATION
void Houses(Mesh& dest, Pool& pool)
#endif // !ENABLE_RUNTIME_COMPILATION
{
#if ENABLE_HOUSES_MESH
	Noise::Rand rand;

	Mesh& h = pool.GetTempMesh();
	Mesh& tmp = pool.GetTempMesh();

	Mesh& b = pool.GetTempMesh();
	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			b.Clear();
			block(b, tmp, h, rand);
			float x = i * 25.f;
			float z = (j - 2) * 23.f * (0.9f + 0.005f * x);
			dest.add(b, matrix4::translation(x, 0, z)
				.rotate(rand.fgen(-3, 3)*DEG_TO_RAD, vector3f::uy));
		}
	}

	//h.Clear();
	//house(h, tmp, rand, 8, 2, 8);
	//dest.add(h, matrix4::translation(10, 0, -2));

	//Mesh& tmp = pool.GetTempMesh();

	//dest.add(base);
	//dest.add(roof);
	//dest.smooth();
	AddNoise(dest, rand, 0.25f);

	dest.sharpenEdges();
	dest.ComputeNormals();
	GenerateAxisAlignedTextureCoordinates(dest, 0.25f);
	dest.ComputeTangents();
#endif // ENABLE_HOUSES_MESH
}
