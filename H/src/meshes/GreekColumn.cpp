#include "GreekColumn.hh"

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

// TODO - SIZE: this file has the highest cost of all meshes. What can be factorized between these functions?

#define ENABLE_GREEK_COLUMN_MESHES 1

#ifdef ENABLE_RUNTIME_COMPILATION
#include "RuntimeCompiledCPlusPlus/RuntimeObjectSystem/ObjectInterfacePerModule.h"

REGISTERCLASS(GreekColumn);
ObjectId GreekColumn::objectId;
GreekColumn* GreekColumn::instance = NULL;
void GreekColumn::BuildMesh(mesh::Mesh& dest, mesh::Pool& pool)
{
	return instance->Build(dest, pool);
}
#endif

using namespace Algebra;
using namespace mesh;

// ---8<---------------------------------------------------------------

//
// This creates a column of dimensions 1x1x2m, with the origin at the
// ground on the axis of symmetry. Adjust size as needed by scaling the
// result.
//
void mesh::CubicColumn(Mesh& dest, Pool& /* pool */)
{
#if ENABLE_GREEK_COLUMN_MESHES
	cube(dest, (mesh::CubeFaces)(mesh::face_all & ~mesh::face_bottom))
		.Translate(-0.5f, 0, -0.5f);

	const float transcale[] = {
		0.15f, 0.8f,
		0.f, 1.1f,
		0.5f, 1.f,
		0.f, 0.7f,
		0.1f, 1.f,
		0.25f, 0.1f,
	};
	for (size_t i = 0; i < ARRAY_LEN(transcale); i += 2)
	{
		int f = 8;
		dest.Extrude(f);
		dest.transformFace(f,
			matrix4::translation(0.f, transcale[i], 0.f)
			.scale(transcale[i + 1], 1.f, transcale[i + 1]));
	}

	dest.sharpenEdges();
#endif // ENABLE_GREEK_COLUMN_MESHES
}

// ---8<---------------------------------------------------------------

void mesh::DoricColumn(Mesh& dest, Pool& pool, int axialResolution, int capitaResolution)
{
#if ENABLE_GREEK_COLUMN_MESHES
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
#endif // ENABLE_GREEK_COLUMN_MESHES
}

// ---8<---------------------------------------------------------------

#define IONIC_ORDER_DIAMETER_TO_HEIGHT 8.5f
#define IONIC_ORDER_DIAMETER_TO_TOP_DIAMETER 0.85f
#define IONIC_ORDER_NUMBER_OF_FLUTES 24

void mesh::IonicColumn(Mesh& dest, Pool& pool, int axialResolution, int baseResolution, int /*capitaResolution*/)
{
#if ENABLE_GREEK_COLUMN_MESHES
	const float standHeight = 0.025f;
	const float baseHeight = 60.f / 50.f * 0.5f / IONIC_ORDER_DIAMETER_TO_HEIGHT;
	const float trunkHeight = 1.f - standHeight - 2.f * baseHeight;

	struct plinthShape
	{
		static vertex f(int, int, const vertex& inputVertex)
		{
			const float roundNess = 0.05f;
			vertex result = inputVertex;
			result.p.x = mix(sign(result.p.x) * 1.f  , result.p.x, roundNess);
			result.p.y = mix((sign(result.p.y) + 1.f) * 0.1f, result.p.y + 1.f, roundNess);
			result.p.z = mix(sign(result.p.z) * 1.f  , result.p.z, roundNess);
			return result;
		}
	};
	Mesh& plinth = pool.GetTempMesh();
	Revolution(sphereHFunc, sphereRFunc).GenerateMesh(plinth, 5, 8);
	plinth.Rotate(PI / 8.f, 0.f, 1., 0.f);
	plinth.ApplyFunction(plinthShape::f);
	plinth.Scale(0.09f);

	float baseCurve[] = {
		0, 0, 60,
		0, 6, 65,
		0, 14, 65,
		0, 20, 60,

		0, 20, 57,
		0, 22, 57,
		0, 28, 52,
		0, 35, 51,
		0, 38, 53,
		0, 40, 53,

		0, 40, 50,
		0, 46, 55,
		0, 54, 55,
		0, 60, 50,
		0, 60, 45,
	};
	initSplineData(baseCurve, ARRAY_LEN(baseCurve), 3);
	Timeline::Variable v(baseCurve, 3, ARRAY_LEN(baseCurve) / 3, Timeline::modeSpline);
	Mesh& base = pool.GetTempMesh();
	Revolution(NULL, NULL, NULL, &v).GenerateMesh(base, baseResolution, axialResolution);
	base.Scale(1.f / 50.f);
	base.Scale(0.5f / IONIC_ORDER_DIAMETER_TO_HEIGHT);

	Mesh& shaft = pool.GetTempMesh();
	Revolution(pipeHFunc, pipeRFunc).GenerateMesh(shaft, 1, axialResolution);
	for (int i = 0; i < shaft.vertices.size; ++i)
	{
		vertex& v = shaft.vertices[i];
		v.p.x *= mix(1.f, IONIC_ORDER_DIAMETER_TO_TOP_DIAMETER, v.p.y);
		v.p.z *= mix(1.f, IONIC_ORDER_DIAMETER_TO_TOP_DIAMETER, v.p.y);
	}
	shaft.Scale(0.5f / IONIC_ORDER_DIAMETER_TO_HEIGHT, trunkHeight, 0.5f / IONIC_ORDER_DIAMETER_TO_HEIGHT);

	dest.add(plinth);
	dest.add(base, matrix4::translation(0.f, standHeight, 0.f));
	dest.add(shaft, matrix4::translation(0.f, standHeight + baseHeight, 0.f));
	dest.add(base, matrix4::translation(0.f, 1.f, 0.f).rotate(PI, vector3f::ux));
	for (int i = 0; i < dest.vertices.size; ++i)
	{
		vertex& vertex = dest.vertices[i];

		// Wrap the texture around the axis, for better resolution.
		vertex.u *= 2.f;

		// Rescale texture coordinate so that:
		// - the base has more resolution to use.
		// - the shaft and capital match.
		vertex.v = 0.05f + 0.95f * vertex.p.y;
		if (vertex.p.y < 0.1f)
		{
			vertex.v = 2.f * vertex.p.y;
		}
		if (vertex.p.y < standHeight * 1.1f)
		{
			// Leave a gap in UV so the texture pattern of the base
			// doesn't leak on the plinth.
			vertex.v = 1.5f * vertex.p.y;
		}
	}
#endif // ENABLE_GREEK_COLUMN_MESHES
}

void mesh::RandomizeIonicColumnTextureOffset(Mesh& dest, Noise::Rand& rand)
{
#if ENABLE_GREEK_COLUMN_MESHES
	float uvOffset = rand.igen(IONIC_ORDER_NUMBER_OF_FLUTES) * (1.f / IONIC_ORDER_NUMBER_OF_FLUTES);
	for (int i = 0; i < dest.vertices.size; ++i)
	{
		dest.vertices[i].u += uvOffset;
	}
#endif // ENABLE_GREEK_COLUMN_MESHES
}

// ---8<---------------------------------------------------------------

#ifdef ENABLE_RUNTIME_COMPILATION
void GreekColumn::Build(Mesh& dest, Pool& pool)
#else // !ENABLE_RUNTIME_COMPILATION
void GreekColumn(Mesh& dest, Pool& pool)
#endif // !ENABLE_RUNTIME_COMPILATION
{
#if ENABLE_GREEK_COLUMN_MESHES
	float columnCurve[] = {
		0, 1, 0,
		0, 0, 60,
		0, 15, 50,
		0, 20, 50,
		0, 22, 42,
		0, 35, 40,
		0, 48, 42,
		0, 48, 45,
		0, 55, 42,
		0, 60, 42,
		0, 60, 0,
	};
	initSplineData(columnCurve, ARRAY_LEN(columnCurve), 3);
	Timeline::Variable v(columnCurve, 3, ARRAY_LEN(columnCurve) / 3, Timeline::modeSpline);

	mesh::Mesh& part = pool.GetTempMesh();
	mesh::Revolution(NULL, NULL, NULL, &v).GenerateMesh(part, 20, 24);
	part.Scale(0.0251f);

	dest.add(part);
	dest.add(part, matrix4::translation(0.f, 12.f, 0.f).rotate(PI, vector3f::ux));

	part.Clear();
	Revolution(pipeHFunc, pipeRFunc).GenerateMesh(part, 2, -18);
	dest.add(part, matrix4::scaling(1.f, 12.f, 1.f));

	dest.ComputeNormals();
	GenerateAxisAlignedTextureCoordinates(dest, 0.1f);
	dest.ComputeTangents();
#endif // ENABLE_GREEK_COLUMN_MESHES
}
