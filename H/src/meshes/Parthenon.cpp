#include "Parthenon.hh"

#include "GreekColumn.hh"
#include "Stairs.hh"
#include "engine/algebra/Interpolation.hxx"
#include "engine/algebra/Matrix.hxx"
#include "engine/algebra/Utils.hh"
#include "engine/container/Utils.hh"
#include "engine/mesh/Cube.hh"
#include "engine/mesh/Mesh.hh"
#include "engine/mesh/MeshPool.hh"
#include "engine/mesh/Revolution.hh"
#include "engine/mesh/Utils.hh"

#define ENABLE_PARTHENON_MESH 1

#ifdef ENABLE_RUNTIME_COMPILATION
#include "RuntimeCompiledCPlusPlus/RuntimeObjectSystem/ObjectInterfacePerModule.h"

REGISTERCLASS(Parthenon);
ObjectId Parthenon::objectId;
Parthenon* Parthenon::instance = NULL;
void Parthenon::BuildMesh(mesh::Mesh& dest, mesh::Pool& pool)
{
	return instance->Build(dest, pool);
}

REGISTERCLASS(ParthenonColumns);
ObjectId ParthenonColumns::objectId;
ParthenonColumns* ParthenonColumns::instance = NULL;
void ParthenonColumns::BuildMesh(mesh::Mesh& dest, mesh::Pool& pool)
{
	return instance->Build(dest, pool);
}
#endif

using namespace Algebra;
using namespace mesh;

// ---8<---------------------------------------------------------------

#if DLL_COMPILATION

static
void mesh::DoricColumn(Mesh& dest, Pool& pool, int axialResolution, int capitaResolution)
{
	const float topDiameter = 1.4f;
	const float capitaDiameter = topDiameter + 2.f * 0.252f;
	const float capitaHeight = 0.276f + 0.06f;
	const float columnHeight = 10.443f; // real size for outside columns
	const float plinthHeight = 0.351f + 0.08f; // Manually tweaked
	const float shaftHeight = columnHeight - plinthHeight;

	struct shaftShape
	{
		static float rFunc(float t, float)
		{
			const float bottomDiameter = 1.923f; // real size
			const float topDiameter = 1.4f;
			return mix(bottomDiameter, topDiameter, t) / 2.f;
		}
	};
	Mesh& shaft = pool.GetTempMesh();
	Revolution(pipeHFunc, shaftShape::rFunc).GenerateMesh(shaft, 1, axialResolution);

	float capitaCurve[] = {
		0, 0, 80,
		0, 40, 89,
		0, 85, 100,
		0, 100, 98,
	};
	initSplineData(capitaCurve, ARRAY_LEN(capitaCurve), 3);
	Timeline::Variable v(capitaCurve, 3, ARRAY_LEN(capitaCurve) / 3, Timeline::modeSpline);
	Mesh& capita = pool.GetTempMesh();
	Revolution(NULL, NULL, NULL, &v).GenerateMesh(capita, capitaResolution, axialResolution);
	// Rescale texture coordinate so the shaft and capital match.
	for (int i = 0; i < capita.vertices.size; ++i)
	{
		vertex& vertex = capita.vertices[i];
		vertex.v = mix(1.f - capitaHeight / shaftHeight, 1.f, vertex.p.y / 100.f);
	}
	capita.Scale(capitaDiameter / 200.f, capitaHeight / 100.f, capitaDiameter / 200.f);

	// The plinth is made with a tiny sphere, extended along the three
	// axes, to get a rounded bloc. I didn't manage to get the exact
	// size under control, hence the manual height tweaking.
	struct plinthShape
	{
		static vertex f(int, int, const vertex& inputVertex)
		{
			const float columnHeight = 10.443f; // real size for outside columns
			const float plinthWidth = 1.758f;
			const float plinthHeight = 0.351f;
			const float shaftHeight = columnHeight - plinthHeight;
			const float roundNess = 0.05f;
			vertex result = inputVertex;
			result.p.x = mix(sign(result.p.x) * plinthWidth / 2.f, result.p.x, roundNess);
			result.p.y = mix((sign(result.p.y) + 1.f) * plinthHeight / 2.f, result.p.y + 1.f, roundNess);
			result.p.z = mix(sign(result.p.z) * plinthWidth / 2.f, result.p.z, roundNess);
			result.v = shaftHeight / columnHeight + result.p.y / columnHeight;
			return result;
		}
	};
	Mesh& plinth = pool.GetTempMesh();
	Revolution(sphereHFunc, sphereRFunc).GenerateMesh(plinth, 5, 8);
	plinth.Rotate(PI / 8.f, 0.f, 1., 0.f);
	plinth.ApplyFunction(plinthShape::f);

	dest.add(shaft, matrix4::scaling(1.f, shaftHeight, 1.f));
	dest.add(capita, matrix4::translation(0.f, shaftHeight - capitaHeight, 0.f));
	dest.add(plinth, matrix4::translation(0.f, shaftHeight, 0.f));
}

static
float mesh::StairsPlatform(Mesh& dest, Pool& pool,
						   int steps, float stepHeight, float stepWidth,
						   float width, float length)
{
	Mesh& tmp = pool.GetTempMesh();

	// The steps are created from the top to the bottom on purpose, to
	// reduce overdraw. On my machine this notably improved performance
	// (from 30 to 60fps).
	for (int i = steps - 1; i >= 0; --i)
	{
		float x = stepWidth * i;
		float y = stepHeight * i;
		float w = width - 2.f * x;
		float l = length - 2.f * x;
		tmp.Clear();
		sharpCube(tmp, (mesh::CubeFaces)(mesh::face_all & ~mesh::face_bottom))
			.Scale(w, stepHeight, l).Translate(x, y, x);
		dest.add(tmp);
	}

	return steps * stepHeight;
}

#endif // DLL_COMPILATION

// ---8<---------------------------------------------------------------

// Based on http://athang1504.blogspot.de/2011/01/parthenon.html
static const float width = 30.9f; // real size = 30.96m | 30.91m.
static const float length = 69.6f; // real size = 69.62 | 69.56m.
static const float stepHeight = 0.512f; // real size: 0.512m, 0.512m and 0.552m.
static const float stepWidth = 0.7f;  // real size.
static const float innerStepHeight = 0.35f; // real size: 0.315m and 0.399m.
static const float innerStepWidth = 0.42f;  // real size: 0.422m.

#if ENABLE_PARTHENON_MESH

static void roof(Mesh& dest, Pool& pool)
{
	const float height = 1.362f; // real size

	Mesh& tmp = pool.GetTempMesh();
	sharpCube(tmp).Scale(width, height, length);
	dest.add(tmp, matrix4::translation(0.f, 0.f, 0.f));

	const float middleShift = -0.5f;
	tmp.Clear();
	sharpCube(tmp).Scale(width + middleShift, height, length + middleShift);
	dest.add(tmp, matrix4::translation(-middleShift*0.5f, height, -middleShift*0.5f));

	const float topShift = 0.5f;
	const float maxRoofHeight = 4.995f; // real size
	const float minRoofHeight = 0.5f;
	tmp.Clear();
	cube(tmp).Scale(0.5f, minRoofHeight, 1.f);

	int f = 4;
	tmp.Extrude(f);
	tmp.transformFace(f, matrix4::translation(0.5f, 0.f, 0.f));
	tmp.vertices[5].p.y += maxRoofHeight - minRoofHeight;
	tmp.vertices[6].p.y += maxRoofHeight - minRoofHeight;

	f = 20;
	tmp.Extrude(f); tmp.transformFace(f, matrix4::scaling(0.75f));
	tmp.Extrude(f); tmp.transformFace(f, matrix4::translation(0.f, 0.f, -0.00625f));
	f = 32;
	tmp.Extrude(f); tmp.transformFace(f, matrix4::scaling(0.75f));
	tmp.Extrude(f); tmp.transformFace(f, matrix4::translation(0.f, 0.f, -0.00625f));

	tmp.Scale(width+topShift, 1.f, length+topShift);
	tmp.sharpenEdges();
	dest.add(tmp, matrix4::translation(-topShift*0.5f, height * 2.f, -topShift*0.5f));
}

static void outerColumns(Mesh& dest, Pool& pool, float baseHeight)
{
	float firstColumnSpacing = 4.71f - 1.923f / 2.f; // Varies between 4.677m and 4.734m.
	float columnSpacing = 4.3f; // Varies between 4.284m and 4.311m.

	const int cols = 8;
	const int rows = 17;

	Mesh& column = pool.GetTempMesh();
	mesh::DoricColumn(column, pool, 20, 5);
	column.ComputeNormals();

	const float x0 = 2.5f;
	const float y0 = 2.5f;
	float y = y0;
	for (int j = 0; j < rows; ++j)
	{
		float x = x0;
		for (int i = 0; i < cols; ++i)
		{
			if (i == 0 || i == cols - 1 ||
				j == 0 || j == rows - 1)
			{
				dest.add(column, matrix4::translation(x, baseHeight, y));
			}
			x += (i == 0 || i == cols - 2) ? firstColumnSpacing : columnSpacing;
		}
		y += (j == 0 || j == rows - 2) ? firstColumnSpacing : columnSpacing;
	}
}

#endif // ENABLE_PARTHENON_MESH

#ifdef ENABLE_RUNTIME_COMPILATION
void Parthenon::Build(Mesh& dest, Pool& pool)
#else // !ENABLE_RUNTIME_COMPILATION
void Parthenon(Mesh& dest, Pool& pool)
#endif // !ENABLE_RUNTIME_COMPILATION
{
#if ENABLE_PARTHENON_MESH
	const float columnHeight = 10.443f; // real size for outside columns

	// Crepidoma
	float floorHeight = mesh::StairsPlatform(dest, pool, 3,
		stepHeight, stepWidth,
		width + 3.f, length + 3.f);

	Mesh& tmp = pool.GetTempMesh();
	float innerStairsShift = 14.f;
	float innerStairsWidth = width - innerStairsShift + 3.f;
	float innerStairsLength = length - innerStairsShift + 3.f;
	float cellaSairsHeight = mesh::StairsPlatform(tmp, pool, 2,
		innerStepHeight, innerStepWidth,
		innerStairsWidth, innerStairsLength);
	dest.add(tmp, matrix4::translation(innerStairsShift*0.5f, floorHeight, innerStairsShift*0.5f));

	tmp.Clear();
	sharpCube(tmp, (mesh::CubeFaces)(mesh::face_all & ~mesh::face_bottom & ~mesh::face_top))
		.Scale(innerStairsWidth - 1.2f, columnHeight - cellaSairsHeight, innerStairsLength - 8.f);
	dest.add(tmp, matrix4::translation((innerStairsShift + 1.2f)*0.5f, floorHeight + cellaSairsHeight, (innerStairsShift + 8.f)*0.5f));

	tmp.Clear();
	roof(tmp, pool);
	dest.add(tmp, matrix4::translation(1.5f, floorHeight + columnHeight, 1.5f));

	dest.ComputeNormals();
	GenerateAxisAlignedTextureCoordinates(dest, 0.1f);

	// Add the columns after texture generation, to avoid overwriting
	// their texture coordinates.
	//outerColumns(dest, pool, floorHeight);

	dest.ComputeTangents();
#endif // ENABLE_PARTHENON_MESH
}

#ifdef ENABLE_RUNTIME_COMPILATION
void ParthenonColumns::Build(Mesh& dest, Pool& pool)
#else // !ENABLE_RUNTIME_COMPILATION
void ParthenonColumns(Mesh& dest, Pool& pool)
#endif // !ENABLE_RUNTIME_COMPILATION
{
#if ENABLE_PARTHENON_MESH
	// Crepidoma
	float floorHeight = 3 * stepHeight;

	outerColumns(dest, pool, floorHeight);

	dest.ComputeTangents();
#endif // ENABLE_PARTHENON_MESH
}
