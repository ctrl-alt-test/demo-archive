#include "Artemision.hh"

#include "GreekColumn.hh"
#include "Stairs.hh"
#include "engine/algebra/Functions.hh"
#include "engine/algebra/Interpolation.hxx"
#include "engine/algebra/Matrix.hxx"
#include "engine/algebra/Spline.hh"
#include "engine/algebra/Utils.hh"
#include "engine/algebra/Vector2.hxx"
#include "engine/algebra/Vector3.hxx"
#include "engine/container/Utils.hh"
#include "engine/mesh/Cube.hh"
#include "engine/mesh/Mesh.hh"
#include "engine/mesh/MeshPool.hh"
#include "engine/mesh/Revolution.hh"
#include "engine/mesh/Utils.hh"

#define ENABLE_ARTEMISSION_MESH 1

// TODO - SIZE: Factorize the roof with Parthenon.

#ifdef ENABLE_RUNTIME_COMPILATION
#include "RuntimeCompiledCPlusPlus/RuntimeObjectSystem/ObjectInterfacePerModule.h"

REGISTERCLASS(Artemision);
ObjectId Artemision::objectId;
Artemision* Artemision::instance = NULL;
void Artemision::BuildMesh(mesh::Mesh& dest, mesh::Pool& pool)
{
	return instance->Build(dest, pool);
}

REGISTERCLASS(ArtemisionColumns);
ObjectId ArtemisionColumns::objectId;
ArtemisionColumns* ArtemisionColumns::instance = NULL;
void ArtemisionColumns::BuildMesh(mesh::Mesh& dest, mesh::Pool& pool)
{
	return instance->Build(dest, pool);
}

REGISTERCLASS(ArtemisionFloor);
ObjectId ArtemisionFloor::objectId;
ArtemisionFloor* ArtemisionFloor::instance = NULL;
void ArtemisionFloor::BuildMesh(mesh::Mesh& dest, mesh::Pool& pool)
{
	return instance->Build(dest, pool);
}

REGISTERCLASS(ArtemisionGreetingsFloor);
ObjectId ArtemisionGreetingsFloor::objectId;
ArtemisionGreetingsFloor* ArtemisionGreetingsFloor::instance = NULL;
void ArtemisionGreetingsFloor::BuildMesh(mesh::Mesh& dest, mesh::Pool& pool)
{
	return instance->Build(dest, pool);
}
#endif

using namespace Algebra;
using namespace mesh;

// ---8<---------------------------------------------------------------

#if DLL_COMPILATION

#define IONIC_ORDER_DIAMETER_TO_HEIGHT 8.5f
#define IONIC_ORDER_DIAMETER_TO_TOP_DIAMETER 0.85f
#define IONIC_ORDER_NUMBER_OF_FLUTES 24

static
void mesh::IonicColumn(Mesh& dest, Pool& pool, int axialResolution, int baseResolution, int /*capitaResolution*/)
{
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
}

static
void mesh::RandomizeIonicColumnTextureOffset(Mesh& dest, Noise::Rand& rand)
{
	float uvOffset = rand.igen(IONIC_ORDER_NUMBER_OF_FLUTES) * (1.f / IONIC_ORDER_NUMBER_OF_FLUTES);
	for (int i = 0; i < dest.vertices.size; ++i)
	{
		dest.vertices[i].u += uvOffset;
	}
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

#if ENABLE_ARTEMISSION_MESH

// The temple of Artemis is described in detail by John Turtle Wood, in
// his book "Discoveries at Ephesus, including the site and remains of
// the great temple of Diana", which reports his work that led to the
// discovery of its remains.
// https://archive.org/stream/discoveriesatep00woodgoog
//
// The floor plan:
// https://upload.wikimedia.org/wikipedia/commons/8/85/Artemistempleplan.jpg
//
// Description of the columns, page 265:
//     " The columns of the peristyle were, as Pliny has
//     described them, one hundred in number, twenty-seven of
//     which were the gifts of kings. They were 6 feet 1/2 inch
//     in diameter at the base ; and adopting the proportion
//     given by Vitruvius for the improved Ionic order, that is
//     8.5 diameters in height independent of the base upon
//     which they were raised, they would be 55 feet 8.75 inches
//     high, including the base. "

// The report by J. T. Wood uses English imperial units.
#define FOOT_TO_METER 0.3048f
#define INCH_TO_METER 0.0254f
#define NUM_STEPS 10

static void roofWalls(Mesh& dest, Pool& pool, float baseHeight)
{
	const float x0 = (239.f * FOOT_TO_METER + 4.5f * INCH_TO_METER) / 2.f -
		(19.f * FOOT_TO_METER + 4.0f * INCH_TO_METER +
		20.f * FOOT_TO_METER + 4.5f * INCH_TO_METER +
		23.f * FOOT_TO_METER + 6.0f * INCH_TO_METER +
		(28.f * FOOT_TO_METER + 8.5f * INCH_TO_METER) / 2.f);
	const float x1 = (239.f * FOOT_TO_METER + 4.5f * INCH_TO_METER) / 2.f +
		(19.f * FOOT_TO_METER + 4.0f * INCH_TO_METER +
		20.f * FOOT_TO_METER + 4.5f * INCH_TO_METER +
		23.f * FOOT_TO_METER + 6.0f * INCH_TO_METER +
		(28.f * FOOT_TO_METER + 8.5f * INCH_TO_METER) / 2.f);
	const float y0 = (418.f * FOOT_TO_METER + 1.5f * INCH_TO_METER) / 2.f -
		(19.f * FOOT_TO_METER + 4.0f * INCH_TO_METER +
		19.f * FOOT_TO_METER + 4.0f * INCH_TO_METER +
		(17.f * FOOT_TO_METER + 1.5f * INCH_TO_METER) * 7.5f);
	const float y1 = (418.f * FOOT_TO_METER + 1.5f * INCH_TO_METER) / 2.f +
		(19.f * FOOT_TO_METER + 4.0f * INCH_TO_METER +
		19.f * FOOT_TO_METER + 4.0f * INCH_TO_METER +
		(17.f * FOOT_TO_METER + 1.5f * INCH_TO_METER) * 7.5f);

	const float beamWidth = 4.f;     //
	const float beamHeight = 2.f;    // Made up values.
	const float sndBeamWidth = 1.5f; //
	const float sndBeamHeight = 3.f; //

	Mesh& tmp = pool.GetTempMesh();
	cube(tmp).Translate(-0.5f, 0.f, 0.f).Scale(beamWidth, beamHeight, y1 - y0 - beamWidth);
	dest.add(tmp, matrix4::translation(x0, baseHeight, y0 + beamWidth / 2.f));
	dest.add(tmp, matrix4::translation(x1, baseHeight, y0 + beamWidth / 2.f));

	tmp.Clear();
	cube(tmp).Translate(-0.5f, 0.f, 0.f).Scale(sndBeamWidth, sndBeamHeight, y1 - y0 - sndBeamWidth);
	dest.add(tmp, matrix4::translation(x0, baseHeight + beamHeight, y0 + sndBeamWidth / 2.f));
	dest.add(tmp, matrix4::translation(x1, baseHeight + beamHeight, y0 + sndBeamWidth / 2.f));

	tmp.Clear();
	cube(tmp).Translate(0.f, 0.f, -0.5f).Scale(x1 - x0 + beamWidth, beamHeight, beamWidth);
	dest.add(tmp, matrix4::translation(x0 - beamWidth / 2.f, baseHeight, y0));
	dest.add(tmp, matrix4::translation(x0 - beamWidth / 2.f, baseHeight, y1));

	tmp.Clear();
	cube(tmp).Translate(0.f, 0.f, -0.5f).Scale(x1 - x0 + sndBeamWidth, sndBeamHeight, sndBeamWidth);
	dest.add(tmp, matrix4::translation(x0 - sndBeamWidth / 2.f, baseHeight + beamHeight, y0));
	dest.add(tmp, matrix4::translation(x0 - sndBeamWidth / 2.f, baseHeight + beamHeight, y1));
}

// Mini-columns for the frieze (below the entablature).
static void roofFrieze(Mesh& dest, Pool& pool, float baseHeight)
{
	const float x0 = 12.f;
	const float y0 = 11.5f;
	const float beamHeight = 2.f;    // Made up values.
	const float width = 49.f;
	const int columns = 16;

	const float height = 3.f;    // Made up values.
	Mesh& tmp = pool.GetTempMesh();
	cube(tmp).Translate(-0.5f, 0.f, -0.5f).Scale(1.f, height, 1.f);
	for (int i = 0; i < columns; i++)
	{
		dest.add(tmp, matrix4::translation(x0 + i * width / (columns-1), baseHeight + beamHeight, y0));
	}
}

static void roof(Mesh& dest, Pool& pool, float baseHeight)
{
	// Dimensions of the roof are approximative
	// Function copied from Parthenon.cpp
	const float width = 51.f;
	const float topShift = 0.2f;
	const float maxRoofHeight = 8.f;
	const float minRoofHeight = 0.5f;

	Mesh& tmp = pool.GetTempMesh();
	cube(tmp)
		.Scale(0.5f * width + topShift, minRoofHeight, 1.f)
		.Translate(-topShift, 0.f, 0.f);

	int f = 4;
	tmp.Extrude(f);
	tmp.transformFace(f, matrix4::translation(0.5f * width + topShift, 0.f, 0.f));
	tmp.vertices[5].p.y = maxRoofHeight;
	tmp.vertices[6].p.y = maxRoofHeight;

	f = 16;
	tmp.Extrude(f); tmp.transformFace(f, matrix4::scaling(0.67f));
	tmp.Extrude(f); tmp.transformFace(f, matrix4::translation(0.f, 0.f, 0.00625f));
	f = 24;
	tmp.Extrude(f); tmp.transformFace(f, matrix4::scaling(0.67f));
	tmp.Extrude(f); tmp.transformFace(f, matrix4::translation(0.f, 0.f, 0.00625f));

	tmp.sharpenEdges();

	dest.add(tmp, matrix4::translation(11.f, baseHeight, 11.f - topShift)
		.scale(1.f, 1.f, 35.5f + topShift));

	dest.add(tmp, matrix4::translation(11.f, baseHeight, 72.f)
		.scale(1.f, 1.f, 44.5f + topShift));
}

static void outerColumns(Mesh& dest, Pool& pool, float baseHeight, bool trueForColumns)
{
	static const float columnSpacingW[] = {
		19.f * FOOT_TO_METER + 4.0f * INCH_TO_METER,
		20.f * FOOT_TO_METER + 4.5f * INCH_TO_METER,
		23.f * FOOT_TO_METER + 6.0f * INCH_TO_METER,
		28.f * FOOT_TO_METER + 8.5f * INCH_TO_METER,
		23.f * FOOT_TO_METER + 6.0f * INCH_TO_METER,
		20.f * FOOT_TO_METER + 4.5f * INCH_TO_METER,
		19.f * FOOT_TO_METER + 4.0f * INCH_TO_METER,
	};
	static const float columnSpacingH[] = {
		19.f * FOOT_TO_METER + 4.0f * INCH_TO_METER,
		19.f * FOOT_TO_METER + 4.0f * INCH_TO_METER,

		17.f * FOOT_TO_METER + 1.5f * INCH_TO_METER,
		17.f * FOOT_TO_METER + 1.5f * INCH_TO_METER,
		17.f * FOOT_TO_METER + 1.5f * INCH_TO_METER,
		17.f * FOOT_TO_METER + 1.5f * INCH_TO_METER,
		17.f * FOOT_TO_METER + 1.5f * INCH_TO_METER,

		17.f * FOOT_TO_METER + 1.5f * INCH_TO_METER,
		17.f * FOOT_TO_METER + 1.5f * INCH_TO_METER,
		17.f * FOOT_TO_METER + 1.5f * INCH_TO_METER,
		17.f * FOOT_TO_METER + 1.5f * INCH_TO_METER,
		17.f * FOOT_TO_METER + 1.5f * INCH_TO_METER,

		17.f * FOOT_TO_METER + 1.5f * INCH_TO_METER,
		17.f * FOOT_TO_METER + 1.5f * INCH_TO_METER,
		17.f * FOOT_TO_METER + 1.5f * INCH_TO_METER,
		17.f * FOOT_TO_METER + 1.5f * INCH_TO_METER,
		17.f * FOOT_TO_METER + 1.5f * INCH_TO_METER,

		19.f * FOOT_TO_METER + 4.0f * INCH_TO_METER,
		19.f * FOOT_TO_METER + 4.0f * INCH_TO_METER,
	};

	const float columnHeight = 55.f * FOOT_TO_METER + 8.75f * INCH_TO_METER;

	const float beamWidth = 2.f;   //
	const float beamHeight = 5.f;  // Made up values.
	const float beamLength = 14.f; //
	Mesh& tmp = pool.GetTempMesh();

	Mesh& loPolyColumn = pool.GetTempMesh();
	Mesh& hiPolyColumn = pool.GetTempMesh();
	if (trueForColumns)
	{
		mesh::IonicColumn(loPolyColumn, pool, 12, 10, 6);
		loPolyColumn.Scale(columnHeight);
		loPolyColumn.ComputeNormals();

		mesh::IonicColumn(hiPolyColumn, pool, 24, 18, 10);
		hiPolyColumn.Scale(columnHeight);
		hiPolyColumn.ComputeNormals();
	}

	const float x0 = (239.f * FOOT_TO_METER + 4.5f * INCH_TO_METER) / 2.f -
		(19.f * FOOT_TO_METER + 4.0f * INCH_TO_METER +
		20.f * FOOT_TO_METER + 4.5f * INCH_TO_METER +
		23.f * FOOT_TO_METER + 6.0f * INCH_TO_METER +
		(28.f * FOOT_TO_METER + 8.5f * INCH_TO_METER) / 2.f);
	const float x1 = (239.f * FOOT_TO_METER + 4.5f * INCH_TO_METER) / 2.f +
		(19.f * FOOT_TO_METER + 4.0f * INCH_TO_METER +
		20.f * FOOT_TO_METER + 4.5f * INCH_TO_METER +
		23.f * FOOT_TO_METER + 6.0f * INCH_TO_METER +
		(28.f * FOOT_TO_METER + 8.5f * INCH_TO_METER) / 2.f);
	const float y0 = (418.f * FOOT_TO_METER + 1.5f * INCH_TO_METER) / 2.f -
		(19.f * FOOT_TO_METER + 4.0f * INCH_TO_METER +
		19.f * FOOT_TO_METER + 4.0f * INCH_TO_METER +
		(17.f * FOOT_TO_METER + 1.5f * INCH_TO_METER) * 7.5f);
	const float y1 = (418.f * FOOT_TO_METER + 1.5f * INCH_TO_METER) / 2.f +
		(19.f * FOOT_TO_METER + 4.0f * INCH_TO_METER +
		19.f * FOOT_TO_METER + 4.0f * INCH_TO_METER +
		(17.f * FOOT_TO_METER + 1.5f * INCH_TO_METER) * 7.5f);

	Noise::Rand rand(0);
	float y = y0;
	for (size_t j = 0; j <= ARRAY_LEN(columnSpacingH); ++j)
	{
		// Columns
		if (trueForColumns)
		{
			float x = x0;
			for (size_t i = 0; i <= ARRAY_LEN(columnSpacingW); ++i)
			{
				if ((i < 2) || (i > ARRAY_LEN(columnSpacingW) - 2) ||
					(j < 2) || (j > ARRAY_LEN(columnSpacingH) - 2) ||
					((i == 3 || i == 4) && (j == 2 || j == ARRAY_LEN(columnSpacingH) - 2)))
				{
					vector3f pos = { x, baseHeight, y };

					Mesh& column = (j < 3 ? hiPolyColumn : loPolyColumn);
					mesh::RandomizeIonicColumnTextureOffset(column, rand);
					dest.add(column, matrix4::translation(pos));
				}
				x += columnSpacingW[i];
			}
		}
		else
		{
			// Beams
			if (j > 0 && j < ARRAY_LEN(columnSpacingH))
			{
				tmp.Clear();
				sharpCube(tmp).Scale(beamLength, beamHeight, beamWidth);
				dest.add(tmp, matrix4::translation(x0 - 1.f, baseHeight + columnHeight, y - beamWidth / 2.f));
				dest.add(tmp, matrix4::translation(x1 - beamLength + 1.f, baseHeight + columnHeight, y - beamWidth / 2.f));
			}
		}

		y += columnSpacingH[j];
	}

	if (!trueForColumns)
	{
		tmp.Clear();
		sharpCube(tmp).Translate(-0.5f, 0.f, 0.f).Scale(beamWidth, beamHeight, beamLength);
		dest.add(tmp, matrix4::translation((x0 + x1 + 28.f * FOOT_TO_METER + 8.5f * INCH_TO_METER) / 2.f, baseHeight + columnHeight, y0 - 0.5f));
		dest.add(tmp, matrix4::translation((x0 + x1 - 28.f * FOOT_TO_METER + 8.5f * INCH_TO_METER) / 2.f, baseHeight + columnHeight, y0 - 0.5f));
		dest.add(tmp, matrix4::translation((x0 + x1 + 28.f * FOOT_TO_METER + 8.5f * INCH_TO_METER) / 2.f, baseHeight + columnHeight, y1 - beamLength + 1.f));
		dest.add(tmp, matrix4::translation((x0 + x1 - 28.f * FOOT_TO_METER + 8.5f * INCH_TO_METER) / 2.f, baseHeight + columnHeight, y1 - beamLength + 1.f));
	}
}

static void innerColumns(Mesh& dest, Pool& pool, float baseHeight)
{
	// Center
	const float x = 36.5f;
	const float y = 64.f;

	// All made up values.
	const float wallHeight = 25.f;
	const float columnHeight = wallHeight / 2.f * 0.85f;
	const float beamHeight = wallHeight / 2.f * 0.15f;

	Mesh& column = pool.GetTempMesh();
	mesh::IonicColumn(column, pool, 24, 18, 10);
	column.Scale(columnHeight);
	column.ComputeNormals();

	// I have already tried using shorts or delta-encoding those values:
	// it doesn't work.
	const vector2f positions[] = {
		// Vestibule
		{4.f, -27.f},
		{4.f, -23.f},
		// Cella
		{6.f, -13.5f},
		{6.f, -9.f},
		{6.f, -4.5f},
		{6.f, 0.f},
		{6.f, 4.5f},
		// Opisthomodos
		{4.f, 20.8f},
		{4.f, 24.8f},
	};

	Noise::Rand rand(0);
	for (size_t i = 0; i < ARRAY_LEN(positions); ++i)
	{
		const vector2f& position = positions[i];
		// SIZE-CODING:
		// Writing in this obfuscated way saves ~89 bytes on the
		// compressed binary, compared to the naive version after.
#if 1
		for (int j = 0; j < 4; ++j)
		{
			mesh::RandomizeIonicColumnTextureOffset(column, rand);
			const float px = x + position.x * ((j&1) * 2 - 1);
			const float h = baseHeight + (columnHeight + beamHeight) * ((j&2)/2);
			const float py = y + position.y;
			dest.add(column, matrix4::translation(px, h, py));
		}
#else
		mesh::RandomizeIonicColumnTextureOffset(column, rand);
		dest.add(column, matrix4::translation(x + position.x, baseHeight, y + position.y));
		mesh::RandomizeIonicColumnTextureOffset(column, rand);
		dest.add(column, matrix4::translation(x - position.x, baseHeight, y + position.y));

		mesh::RandomizeIonicColumnTextureOffset(column, rand);
		dest.add(column, matrix4::translation(x + position.x, baseHeight + columnHeight + beamHeight, y + position.y));
		mesh::RandomizeIonicColumnTextureOffset(column, rand);
		dest.add(column, matrix4::translation(x - position.x, baseHeight + columnHeight + beamHeight, y + position.y));
#endif
	}
}

// TODO - SIZE: innerWalls costs ~170 bytes. Surely there must be less
// expensive way to write it.
static void innerWalls(Mesh& dest, Pool& pool, float baseHeight)
{
	// Center.
	const float x = 36.5f;
	const float y = 64.f;

	// Dimensions of the inner walls are approximative.
	Mesh& wall = pool.GetTempMesh();
	float wallW = 2.f; // wall width
	const float wallHeight = 25.f;
	const float cellaWidth = 21.f;
	const float entranceWidth = 6.f;
	const float entranceHeight = wallHeight / 2.f * 0.8f; // Just a little bit lower than the beam (at 0.85).

	// Two long walls.
	cube(wall).Translate(-0.5f, 0.f, -0.5f).Scale(wallW, wallHeight, 81.f);
	dest.add(wall, matrix4::translation(x - 0.5f * (cellaWidth + wallW), baseHeight, y));
	dest.add(wall, matrix4::translation(x + 0.5f * (cellaWidth + wallW), baseHeight, y));

	// Separations.
	wall.Clear();
	Mesh& tmp = pool.GetTempMesh();
	cube(tmp).Translate(0.f, 0.f, -0.5f).Scale(0.5f * (cellaWidth - entranceWidth), wallHeight, wallW);
	wall.add(tmp, matrix4::translation(-0.5f * cellaWidth, 0.f, 0.f));
	wall.add(tmp, matrix4::translation(0.5f * entranceWidth, 0.f, 0.f));
	tmp.Clear();
	cube(tmp).Translate(-0.5f, -0.5f, -0.5f).Scale(2.f, 2.f, 0.5f * wallW);
	wall.add(tmp, matrix4::translation(0.5f * entranceWidth, entranceHeight, 0.f).rotate(PI / 4.f, vector3f::uz));
	wall.add(tmp, matrix4::translation(-0.5f * entranceWidth, entranceHeight, 0.f).rotate(PI / 4.f, vector3f::uz));
	tmp.Clear();
	cube(tmp).Translate(-0.5f, 0.f, -0.5f).Scale(entranceWidth, wallHeight - entranceHeight, wallW);
	wall.add(tmp, matrix4::translation(0.f, entranceHeight, 0.f));

	// Vestibule.
	dest.add(wall, matrix4::translation(x, baseHeight, y - 31.f));
	dest.add(wall, matrix4::translation(x, baseHeight, y - 18.f));
	// Opisthomodos.
	dest.add(wall, matrix4::translation(x, baseHeight, y + 29.f));

	// Altar.
	wall.Clear();
	cube(wall).Translate(-0.5f, 0.f, -0.5f).Scale(14.f, wallHeight, wallW);
	dest.add(wall, matrix4::translation(x, baseHeight, y + 17.f));

	wall.Clear();
	cube(wall).Translate(-0.5f, 0.f, -0.5f).Scale(1.f, wallHeight, 7.f);
	dest.add(wall, matrix4::translation(x - 6.f, baseHeight, y + 12.5f));
	dest.add(wall, matrix4::translation(x + 6.f, baseHeight, y + 12.5f));

	wall.Clear();
	cube(wall).Translate(-0.5f, 0.f, -0.5f).Scale(1.5f, wallHeight, 1.5f);
	dest.add(wall, matrix4::translation(x - 6.f, baseHeight, y+ 9.f));
	dest.add(wall, matrix4::translation(x + 6.f, baseHeight, y+ 9.f));

	// Columns beams.
	const float columnHeight = wallHeight / 2.f * 0.85f;
	const float beamHeight = wallHeight / 2.f * 0.15f;
	const float beamWidth = 2.f;
	const float cellaBeamLength = 25.25f;
	const float vestibuleBeamLength = 11.f;
	const float optithodomosBeamLength = 10.f;

	Mesh& beam = pool.GetTempMesh();
	cube(beam).Translate(-0.5f, 0.f, 0.f).Scale(beamWidth, beamHeight, cellaBeamLength);
	dest.add(beam, matrix4::translation(x + 6.f, baseHeight + columnHeight, y - 17.f));
	dest.add(beam, matrix4::translation(x - 6.f, baseHeight + columnHeight, y - 17.f));
	dest.add(beam, matrix4::translation(x + 6.f, baseHeight + 2.f * columnHeight + beamHeight, y - 17.f));
	dest.add(beam, matrix4::translation(x - 6.f, baseHeight + 2.f * columnHeight + beamHeight, y - 17.f));

	beam.Clear();
	cube(beam).Translate(-0.5f, 0.f, 0.f).Scale(beamWidth, beamHeight, vestibuleBeamLength);
	dest.add(beam, matrix4::translation(x + 4.f, baseHeight + columnHeight, y - 30.f));
	dest.add(beam, matrix4::translation(x - 4.f, baseHeight + columnHeight, y - 30.f));
	dest.add(beam, matrix4::translation(x + 4.f, baseHeight + 2.f * columnHeight + beamHeight, y - 30.f));
	dest.add(beam, matrix4::translation(x - 4.f, baseHeight + 2.f * columnHeight + beamHeight, y - 30.f));

	beam.Clear();
	cube(beam).Translate(-0.5f, 0.f, 0.f).Scale(beamWidth, beamHeight, optithodomosBeamLength);
	dest.add(beam, matrix4::translation(x + 4.f, baseHeight + columnHeight, y + 18.f));
	dest.add(beam, matrix4::translation(x - 4.f, baseHeight + columnHeight, y + 18.f));
	dest.add(beam, matrix4::translation(x + 4.f, baseHeight + 2.f * columnHeight + beamHeight, y + 18.f));
	dest.add(beam, matrix4::translation(x - 4.f, baseHeight + 2.f * columnHeight + beamHeight, y + 18.f));
}

static void altar(Mesh& dest, Pool& pool, float baseHeight)
{
	float baseCurve[] = {
		0, 0, 100,
		0, 14, 100,
		0, 15, 100,
		0, 16, 90,
		0, 17, 90,
		0, 19, 90,
		0, 20, 90,
		0, 21, 80,
		0, 22, 80,

		0, 82, 80,
		0, 83, 80,
		0, 85, 90,
		0, 86, 90,
		0, 96, 90,
		0, 97, 90,
		0, 98, 100,
		0, 99, 100,
		0, 100, 99,
		0, 100, 90,
		0, 100, 1,
	};

	struct expandBase
	{
		static vertex f(int, int, const vertex& inputVertex)
		{
			vertex result = inputVertex;
			result.p.x += sign(result.p.x) * 2.5f;
			result.p.z += sign(result.p.z) * 3.2f;
			return result;
		}
	};

	// Position of the throne.
	const float x = 36.5f;
	const float y = 71.f;

	initSplineData(baseCurve, ARRAY_LEN(baseCurve), 3);
	Timeline::Variable v(baseCurve, 3, ARRAY_LEN(baseCurve) / 3, Timeline::modeSpline);
	Mesh& base = pool.GetTempMesh();
	Revolution(NULL, NULL, NULL, &v).GenerateMesh(base, (ARRAY_LEN(baseCurve)/3-1), -8);
	base.Scale(1.f / 100.f, 3.5f / 100.f, 1.f / 100.f);
	base.Rotate(0.5f * 360.f / 8.f * DEG_TO_RAD, 0.f, 1.f, 0.f);
	base.ApplyFunction(expandBase::f);
	dest.add(base, matrix4::translation(x, baseHeight, y));

	// Back.
	Mesh& part = pool.GetTempMesh();
	cube(part).Translate(-0.5f, 0.f, -0.5f).Scale(6.f, 13.5f, 1.f);
	part.Extrude(12); part.transformFace(12, matrix4::translation(0.f, 1.5f, 0.f).scale(0.01f, 1.f, 1.f));
	dest.add(part, matrix4::translation(x, baseHeight + 3.5f, y+3.2f));

	// Armrests.
	part.Clear();
	cube(part).Translate(-0.5f, 0.f, -0.5f).Scale(1.f, 5.8f, 4.8f);
	dest.add(part, matrix4::translation(x-2.5f, baseHeight + 3.5f, y+0.3f));
	dest.add(part, matrix4::translation(x+2.5f, baseHeight + 3.5f, y+0.3f));

	// Seat.
	part.Clear();
	cube(part).Translate(-0.5f, 0.f, -0.5f).Scale(4.f, 2.8f, 4.f);
	dest.add(part, matrix4::translation(x, baseHeight + 3.5f, y+0.7f));
}

#endif // ENABLE_ARTEMISSION_MESH

#ifdef ENABLE_RUNTIME_COMPILATION
void Artemision::Build(Mesh& dest, Pool& pool)
#else // !ENABLE_RUNTIME_COMPILATION
void Artemision(Mesh& dest, Pool& pool)
#endif // !ENABLE_RUNTIME_COMPILATION
{
#if ENABLE_ARTEMISSION_MESH
	const float floorHeight = NUM_STEPS * 0.4f;

	// Crepidoma
	mesh::StairsPlatform(dest, pool, NUM_STEPS,
		0.4f, 0.6f, // Made up values
		239.f * FOOT_TO_METER + 4.5f * INCH_TO_METER,
		418.f * FOOT_TO_METER + 1.5f * INCH_TO_METER);

	innerWalls(dest, pool, floorHeight);
	altar(dest, pool, floorHeight);

	float roofWallHeight = floorHeight + 55.f * FOOT_TO_METER + 8.75f * INCH_TO_METER;
	roofWalls(dest, pool, roofWallHeight);
	//roofFrieze(dest, pool, roofWallHeight);

	float roofHeight = roofWallHeight + 5.f;
	roof(dest, pool, roofHeight);
	dest.sharpenEdges();

	outerColumns(dest, pool, floorHeight, false);

	dest.ComputeNormals();
	GenerateAxisAlignedTextureCoordinates(dest, 0.1f);
	dest.ComputeTangents();
#endif
}

#ifdef ENABLE_RUNTIME_COMPILATION
void ArtemisionColumns::Build(Mesh& dest, Pool& pool)
#else // !ENABLE_RUNTIME_COMPILATION
void ArtemisionColumns(Mesh& dest, Pool& pool)
#endif // !ENABLE_RUNTIME_COMPILATION
{
#if ENABLE_ARTEMISSION_MESH
	const float floorHeight = NUM_STEPS * 0.4f;
	outerColumns(dest, pool, floorHeight, true);
	innerColumns(dest, pool, floorHeight);
	dest.ComputeTangents();
#endif
}

#ifdef ENABLE_RUNTIME_COMPILATION
void ArtemisionFloor::Build(Mesh& dest, Pool& /*pool*/)
#else // !ENABLE_RUNTIME_COMPILATION
void ArtemisionFloor(Mesh& dest, Pool& /*pool*/)
#endif // !ENABLE_RUNTIME_COMPILATION
{
#if ENABLE_ARTEMISSION_MESH
	const float floorHeight = NUM_STEPS * 0.4f;
	sharpCube(dest, (mesh::CubeFaces)(mesh::face_all & ~mesh::face_bottom))
		.Translate(0.f, 0.f, -0.5f)
		.Scale(21.f, 0.06f, 33.f);

	dest.ComputeNormals();
	GenerateAxisAlignedTextureCoordinates(dest, 1.f / 8.4f);

	dest.Translate(36.5f - 10.5f, floorHeight, 63.5f);
	dest.ComputeTangents();
#endif
}

#ifdef ENABLE_RUNTIME_COMPILATION
void ArtemisionGreetingsFloor::Build(Mesh& dest, Pool& /*pool*/)
#else // !ENABLE_RUNTIME_COMPILATION
void ArtemisionGreetingsFloor(Mesh& dest, Pool& /*pool*/)
#endif // !ENABLE_RUNTIME_COMPILATION
{
#if ENABLE_ARTEMISSION_MESH
	struct local
	{
		static vector3f discHFunc(float t, float)
		{
			const vector3f result = { 0.f, interpolate(0.f, 0.125f, t), 0.f };
			return result;
		}

		static float discRFunc(float t, float)
		{
			return (1.f - t);
		}

		static vertex f(int, int, const vertex& inputVertex)
		{
			vertex result = inputVertex;
			result.u = result.p.x * 0.5f + 0.5f;
			result.v = result.p.z * 0.5f + 0.5f;
			result.p.x *= 3.8f;
			result.p.y *= 0.05f;
			result.p.z *= 3.8f;
			return result;
		}
	};

	Revolution(local::discHFunc, local::discRFunc).GenerateMesh(dest, 6, 48);
	dest.ApplyFunction(local::f);

	dest.ComputeNormals();
	dest.ComputeTangents();
#endif
}
