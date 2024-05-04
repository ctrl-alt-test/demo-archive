#include "Mesh.hh"

#include "engine/algebra/Interpolation.hxx"
#include "engine/algebra/Matrix.hxx"
#include "engine/algebra/Quaternion.hxx"
#include "engine/algebra/Vector3.hxx"
#include "engine/container/Array.hxx"
#include "engine/core/Debug.hh"
#include "engine/core/RigidTransform.hxx"
#include "engine/mesh/MeshPool.hh"
#include "engine/noise/Rand.hh"
#include <cstring>

using namespace Algebra;
using namespace mesh;

// =========================================================================
// Vertex operations

Mesh& Mesh::ApplyFunction(MeshFunction function)
{
	for (int i = 0; i < vertices.size; ++i)
	{
		vertices[i] = function(i, i / 4, vertices[i]);
	}
	return *this;
}

Mesh& Mesh::ApplyFunction(const int* vertexIds, int numberOfVertices, MeshFunction function)
{
	for (int j = 0; j < numberOfVertices; ++j)
	{
		const int i = vertexIds[j];
		vertices[i] = function(i, i / 4, vertices[i]);
	}
	return *this;
}

vertex& transformVertex(vertex& vert, const Core::RigidTransform& t)
{
#if VERTEX_ATTR_POSITION
	vert.p = t.v + Algebra::rotate(t.q, vert.p);
#endif

#if VERTEX_ATTR_NORMAL
	vert.n = Algebra::rotate(t.q, vert.n);
#endif

#if VERTEX_ATTR_TANGENT
	vert.t = Algebra::rotate(t.q, vert.t);
#endif

	return vert;
}

vertex& transformVertex(vertex& vert, const matrix4& m)
{
#if VERTEX_ATTR_POSITION
	applyMatrixToPoint(m, vert.p);
#endif

#if VERTEX_ATTR_NORMAL
	applyMatrixToDirection(m, vert.n);
#endif

#if VERTEX_ATTR_TANGENT
	applyMatrixToDirection(m, vert.t);
#endif

	return vert;
}

Mesh& Mesh::Transform(const Core::RigidTransform& t)
{
	for (int i = 0; i < vertices.size; ++i)
	{
		transformVertex(vertices[i], t);
	}
	return *this;
}

Mesh& Mesh::Transform(const matrix4& mat)
{
	for (int i = 0; i < vertices.size; ++i)
	{
		transformVertex(vertices[i], mat);
	}
	return *this;
}

Mesh& Mesh::Translate(float x, float y, float z)
{
	return Transform(Core::RigidTransform::translation(x, y, z));
}

Mesh& Mesh::Translate(const vector3f& t)
{
	return Transform(Core::RigidTransform::translation(t));
}

Mesh& Mesh::Rotate(float angle, float x, float y, float z)
{
	return Transform(Core::RigidTransform::rotation(angle, x, y, z));
}

Mesh& Mesh::Rotate(float angle, const vector3f& axis)
{
	return Transform(Core::RigidTransform::rotation(angle, axis));
}

Mesh& Mesh::Scale(float x, float y, float z)
{
	for (int i = 0; i < vertices.size; ++i)
	{
#if VERTEX_ATTR_POSITION
		vertices[i].p.x *= x;
		vertices[i].p.y *= y;
		vertices[i].p.z *= z;
#endif
	}
	return *this;
}

Mesh& Mesh::Scale(const vector3f& s)
{
	return Scale(s.x, s.y, s.z);
}

// =========================================================================
// Topology operations

void Mesh::Clear()
{
	vertices.empty();
	quads.empty();
}

// Add a vertex at the end of the mesh.
// Must be called 4*n times to ensure the quads array is legal.
void Mesh::add(const vertex& v)
{
	int pos = vertices.size;
	vertices.add(v);
	quads.add(pos);
}

void Mesh::addNoDup(const vertex& v, float epsilon)
{
	for (int i = 0; i < vertices.size; i++)
	{
		if (dist(vertices[i].p, v.p) < epsilon)
		{
			quads.add(i);
			return;
		}
	}
	add(v);
}

// Add vertices at the end of the mesh.
Mesh& Mesh::add(const Container::Array<vertex>& v)
{
	IFDBG(assert(vertices.max_size >= vertices.size + v.size));

	int size = v.size;
	assert(size % 4 == 0);
	for (int i = 0; i < size; i++)
	{
		add(v[i]);
	}

	return *this;
}

//
// Append a mesh
//
Mesh& Mesh::add(const Mesh& mesh)
{
	IFDBG(assert(vertices.max_size >= vertices.size + mesh.vertices.size));

	const int oldsize = vertices.size;
	const int size = mesh.vertices.size;
	for (int i = 0; i < size; ++i)
	{
		vertices.add(mesh.vertices[i]);
	}

	const int nb_quads = mesh.quads.size;
	for (int i = 0; i < nb_quads; ++i)
	{
		quads.add(mesh.quads[i] + oldsize);
	}

	return *this;
}

//
// Ajout à un tas de sommets d'un autre tas de sommets transformé
// par une matrice
//
Mesh& Mesh::add(const Mesh& mesh, const matrix4& transform)
{
	IFDBG(assert(vertices.max_size >= vertices.size + mesh.vertices.size));

	const int oldsize = vertices.size;
	const int size = mesh.vertices.size;
	for (int i = 0; i < size; ++i)
	{
		vertex vert = mesh.vertices[i];
		transformVertex(vert, transform);
		vertices.add(vert);
	}

	const int nb_quads = mesh.quads.size;
	for (int i = 0; i < nb_quads; ++i)
	{
		quads.add(mesh.quads[i] + oldsize);
	}

	return *this;
}

void Mesh::CleanUp()
{
	IFDBG(int oldSize = vertices.size);
	IFDBG(int expectedNewSize = 0);

	Container::Array<int> usedVertices(vertices.size);
	usedVertices.size = vertices.size;
	for (int i = 0; i < usedVertices.size; ++i)
	{
		usedVertices[i] = -1;
	}
	for (int i = 0; i < quads.size; ++i)
	{
		const int vertexIndex = quads[i];
#ifdef DEBUG
		if (usedVertices[vertexIndex] == -1)
		{
			++expectedNewSize;
		}
#endif // DEBUG
		usedVertices[vertexIndex] = vertexIndex;
	}

	for (int i = 0; i < usedVertices.size; ++i)
	{
		if (usedVertices[i] == -1)
		{
			while (i < usedVertices.size - 1 && usedVertices.last() == -1)
			{
				--usedVertices.size;
			}
			if (i < usedVertices.size - 1)
			{
				for (int j = 0; j < quads.size; ++j)
				{
					if (quads[j] == usedVertices.size - 1)
					{
						quads[j] = i;
					}
				}
				vertices[i] = vertices[usedVertices.size - 1];
			}
			--usedVertices.size;
		}
	}
	assert(usedVertices.size == expectedNewSize);
	vertices.size = usedVertices.size;

	shouldCleanUp = false;
	LOG_INFO("Cleaned up mesh: %d -> %d vertices.", oldSize, vertices.size);
}

Mesh& Mesh::Cut(SDFFunction sdf)
{
	// Quick and dirty implementation, because the proper one (that
	// handles the faces cut through) is a lot more complicated and I'm
	// too lazy to figure it now.

	// Counting backward because removeFace() is going to invalidate
	// indices.
	for (int i = quads.size - 4; i >= 0; i -= 4)
	{
#if VERTEX_ATTR_POSITION
		int culledVertices = 0;
		for (int j = 0; j < 4; ++j)
		{
			const Algebra::vector3f& p = vertices[quads[i + j]].p;
			const float d = sdf(p.x, p.y, p.z);
			if (d < 0.f)
			{
				++culledVertices;
			}
		}
		if (culledVertices == 4)
		{
			removeFace(i);
		}
#endif
	}
	return *this;
}

// =========================================================================
// Face operations

void Mesh::removeFace(int face)
{
	assert(face % 4 == 0);
	for (int i = 3; i >= 0; i--)
	{
		quads.remove(face + i);
	}
	shouldCleanUp = true;
}

vector3f Mesh::faceCenter(int face) const
{
	assert(face % 4 == 0);
	return (vertices[quads[face + 0]].p +
            vertices[quads[face + 1]].p +
            vertices[quads[face + 2]].p +
            vertices[quads[face + 3]].p) * 0.25f;
}

void Mesh::transformFace(int face, const matrix4& mat)
{
	assert(face % 4 == 0);
	vector3f center = faceCenter(face);
	matrix4 m = matrix4::translation(center);
	m *= mat;
	m.translate(-center);
	for (int i = 0; i < 4; ++i)
	{
		transformVertex(vertices[quads[face + i]], m);
	}
}

void Mesh::Extrude(int face, int* out_newFaces)
{
	assert(face % 4 == 0);

	// Starting index of the new vertices
	const int newv = vertices.size;

	// Duplicate the face vertices.
	for (int i = 0; i < 4; ++i)
	{
		vertices.add(vertices[quads[face + i]]);
	}

	// Create the side faces.
	for (int i1 = 0; i1 < 4; ++i1)
	{
		if (out_newFaces != NULL)
		{
			out_newFaces[i1] = quads.size;
		}

		int i2 = (i1 + 1) % 4;
		quads.add(quads[face + i1]);
		quads.add(quads[face + i2]);
		quads.add(newv + i2);
		quads.add(newv + i1);
	}

	// Update the indices of the extruded face.
	for (int i = 0; i < 4; ++i)
	{
		quads[face + i] = newv + i;
	}
}

void Mesh::CreaseEdge(int face1, int face2)
{
	assert(face1 % 4 == 0 && face2 % 4 == 0 && face1 != face2);
	int indicesToDuplicate[4];
	int found = 0;

	// Starting index of the new vertices.
	const int newv = vertices.size;

	// Find all common vertices between face1 and face2
	for (int i1 = 0; i1 < 4; ++i1)
	{
		for (int i2 = 0; i2 < 4; ++i2)
		{
			if (quads[face1 + i1] == quads[face2 + i2])
			{
				indicesToDuplicate[found++] = face2 + i2;
			}
		}
	}

	// Duplicate those vertices
	for (int i = 0; i < found; ++i)
	{
		vertices.add(vertices[quads[indicesToDuplicate[i]]]);
		quads[indicesToDuplicate[i]] = newv + i;
	}

	// FIXME: also fix the indices for all the other faces.
}

void Mesh::SmoothEdge(int face1, int face2)
{
	assert(face1 % 4 == 0 && face2 % 4 == 0 && face1 != face2);

	int toDel[2] = { -1, -1 };
	int replaceWith[2] = { -1, -1 };
	float minSqrDistances[2] = { 65536.f, 65536.f };

	// Find the best candidates for the edge to smooth.
	for (int i1 = 0; i1 < 4; ++i1)
	{
		for (int i2 = 0; i2 < 4; ++i2)
		{
			vector3f delta = vertices[quads[face2 + i2]].p - vertices[quads[face1 + i1]].p;
			float sqrDistance = dot(delta, delta);
			if (sqrDistance < minSqrDistances[0])
			{
				toDel[1]           = toDel[0];
				replaceWith[1]     = replaceWith[0];
				minSqrDistances[1] = minSqrDistances[0];

				toDel[0]           = face1 + i1;
				replaceWith[0]     = face2 + i2;
				minSqrDistances[0] = sqrDistance;
			}
			else if(sqrDistance < minSqrDistances[1])
			{
				toDel[1]           = face1 + i1;
				replaceWith[1]     = face2 + i2;
				minSqrDistances[1] = sqrDistance;
			}
		}
	}

#if DEBUG
	// Sanity check: make sure the vertices we picked are at least closer
	// from each other than the vertices of a same face.
	vector3f a = vertices[quads[face1    ]].p - vertices[quads[face1 + 2]].p;
	vector3f b = vertices[quads[face1 + 1]].p - vertices[quads[face1 + 3]].p;
	vector3f c = vertices[quads[face2    ]].p - vertices[quads[face2 + 2]].p;
	vector3f d = vertices[quads[face2 + 1]].p - vertices[quads[face2 + 3]].p;
	float minFaceDiagonal = msys_min(msys_min(dot(a, a), dot(b,b)), msys_min(dot(c, c), dot(d, d)));
	assert(minSqrDistances[0] < 0.25f * minFaceDiagonal && minSqrDistances[1] < 0.25f * minFaceDiagonal);
#endif // DEBUG

	// Replace the vertices.
	for (int i = 0; i < quads.size; ++i)
	{
		if (quads[i] == toDel[0])
		{
			quads[i] = replaceWith[0];
		}
		else if (quads[i] == toDel[1])
		{
			quads[i] = replaceWith[1];
		}
	}
	shouldCleanUp = true;
}

void Mesh::sharpenEdges()
{
	int vsize = vertices.size;
	Container::Array<vertex> old(vsize);
	old.size = vsize;
	for (int i = 0; i < vsize; i++)
	{
		old[i] = vertices[i];
	}
	vertices.empty();
	int qsize = quads.size;
	for (int i = 0; i < qsize; i++)
	{
		vertices.add(old[quads[i]]);
		quads[i] = i;
	}
}

// Merge vertices when the distance is smaller than epsilon.
// Warning: O(N^2) - will be slow on big meshes.
void Mesh::unsharpenEdges(float epsilon)
{
	int oldSize = vertices.size;
	vertices.size = 0;
	quads.size = 0;
	for (int i = 0; i < oldSize; i++)
	{
		addNoDup(vertices.elt[i], epsilon);
	}
}

inline
float distancePointLine(const vector3f& p, const vector3f& a, const vector3f& b)
{
	const vector3f PA = a - p;
	const vector3f AB = b - a;
	float dotABAB = dot(AB, AB);

	if (dotABAB == 0) return norm(PA);

	const vector3f PH = PA - AB * dot(PA, AB) / dotABAB;
	return norm(PH);
}

static vertex interpolate3(const vertex& v1, const vertex& v2, const vertex& v3, float x, float y)
{
	assert(x + y <= 1.f); // sinon, ça sort du triangle (v1, v2, v3)

	vertex v = v1;
	// FIXME: couleurs et normales
#if VERTEX_ATTR_POSITION
	v.p = v1.p + (v2.p - v1.p) * x + (v3.p - v1.p) * y;
#endif

#if VERTEX_ATTR_TEX_COORD
	v.u = v1.u + (v2.u - v1.u) * x + (v3.u - v1.u) * y;
	v.v = v1.v + (v2.v - v1.v) * x + (v3.v - v1.v) * y;
#endif

	return v;
}

static vertex interpolate4(const vertex* vertices, float x, float y)
{
	if (x + y < 1.f)
	{
		return interpolate3(vertices[0], vertices[1], vertices[3], x, y);
	}
	else
	{
		return interpolate3(vertices[2], vertices[1], vertices[3], 1.f - y, 1.f - x);
	}
}

// Split a face in x*y faces
void Mesh::splitFace(const vertex* oldVertices, int x, int y)
{
	assert(x > 0 && y > 0);

	for (int i = 0; i < x; i++)
	{
		for (int j = 0; j < y; j++)
		{
			add(interpolate4(oldVertices, (float) i / x,     (float) j / y));
			add(interpolate4(oldVertices, (float) (i+1) / x, (float) j / y));
			add(interpolate4(oldVertices, (float) (i+1) / x, (float) (j+1) / y));
			add(interpolate4(oldVertices, (float) i / x,     (float) (j+1) / y));
		}
	}
}

void Mesh::splitAllFaces(int x)
{
	Mesh originalMesh(vertices.size);
	originalMesh.add(*this);

	Clear();
	for (int i = 0; i < originalMesh.vertices.size; i += 4)
	{
		splitFace(&originalMesh.vertices[i], x, x);
	}
}

void Mesh::splitAllFacesDownToMaxSide(float maxSide)
{
#if VERTEX_ATTR_POSITION
	Mesh originalMesh(vertices.size);
	originalMesh.add(*this);

	Clear();
	for (int i = 0; i < originalMesh.vertices.size; i += 4)
	{
		const vector3f AB = originalMesh.vertices[i + 1].p - originalMesh.vertices[i    ].p;
		const vector3f BC = originalMesh.vertices[i + 2].p - originalMesh.vertices[i + 1].p;
		const vector3f CD = originalMesh.vertices[i + 3].p - originalMesh.vertices[i + 2].p;
		const vector3f DA = originalMesh.vertices[i    ].p - originalMesh.vertices[i + 3].p;

		int xSplit = 1 + msys_ifloorf(msys_max(norm(AB), norm(CD)) / maxSide);
		int ySplit = 1 + msys_ifloorf(msys_max(norm(BC), norm(DA)) / maxSide);

		splitFace(&originalMesh.vertices[i], xSplit, ySplit);
	}
#endif
}

// ========================================================================
// Normales

// Adapted from IQ's code.
// http://www.iquilezles.org/www/articles/normals/normals.htm
// http://www.iquilezles.org/www/material/breakpoint2007/bp2007.pdf
void Mesh::ComputeNormals()
{
#if VERTEX_ATTR_POSITION && VERTEX_ATTR_NORMAL
	for (int i = 0; i < vertices.size; i++)
	{
		vertices[i].n = vector3f::zero;
	}

	for (int i = 0; i < quads.size; i += 4)
	{
		for (int j = 0; j < 4; j++)
		{
			int index1 = quads[i + j];
			int index2 = quads[i + ((j + 1) % 4)];
			vector3f tmp = cross(vertices[index1].p, vertices[index2].p);
			for (int k = 0; k < 4; ++k)
			{
				vertices[quads[i + k]].n += tmp;
			}
		}
	}

	for (int i = 0; i < vertices.size; i++)
	{
		float length = norm(vertices[i].n);
		if (length > 0.f)
		{
			vertices[i].n /= length;
		}
	}
#endif // VERTEX_ATTR_POSITION && VERTEX_ATTR_NORMAL
}

// Adapted from a much referenced article, now gone:
// http://www.terathon.com/code/tangent.html
// https://web.archive.org/web/20160409104130/http://www.terathon.com/code/tangent.html
void Mesh::ComputeTangents()
{
#if VERTEX_ATTR_POSITION && VERTEX_ATTR_NORMAL && VERTEX_ATTR_TEX_COORD

	// FIXME: it would be more cache friendly to have one array:
	//        { t0, b0, t1, b1, ... , tn, bn }
	//        instead of two arrays:
	//        { t0, t1, ... , tn } and { b0. b1. ... , bn }.
	vector3f* tan1 = (vector3f*)malloc(sizeof(vector3f) * vertices.size * 2);
	vector3f* tan2 = tan1 + vertices.size;
	memset(tan1, 0, sizeof(vector3f) * vertices.size * 2);

	for (int i = 0; i < quads.size; i += 4)
	{
		const int vertexIndex0 = quads[i    ];
		const int vertexIndex1 = quads[i + 1];
		const int vertexIndex2 = quads[i + 2];
		const int vertexIndex3 = quads[i + 3];
		const vertex& vertex0 = vertices[vertexIndex0];
		const vertex& vertex1 = vertices[vertexIndex1];
		const vertex& vertex2 = vertices[vertexIndex2];
		const vertex& vertex3 = vertices[vertexIndex3];

		// v3 +----+ v2
		//    |\  /|
		//    | \/ |
		//    | /\ |
		//    |/  \|
		// v0 +----+ v1

		vector3f q1 = vertex2.p - vertex0.p;
		float s1 = vertex2.u - vertex0.u;
		float t1 = vertex2.v - vertex0.v;
		vector3f q2 = vertex3.p - vertex1.p;
		float s2 = vertex3.u - vertex1.u;
		float t2 = vertex3.v - vertex1.v;

		float r = 1.0f / (s1 * t2 - s2 * t1);
		vector3f sdir = (t2 * q1 - t1 * q2) * r;
		vector3f tdir = (s1 * q2 - s2 * q1) * r;

		tan1[vertexIndex0] += sdir;
		tan1[vertexIndex1] += sdir;
		tan1[vertexIndex2] += sdir;
		tan1[vertexIndex3] += sdir;

		tan2[vertexIndex0] += tdir;
		tan2[vertexIndex1] += tdir;
		tan2[vertexIndex2] += tdir;
		tan2[vertexIndex3] += tdir;
	}
	
	for (int i = 0; i < vertices.size; ++i)
	{
		const vector3f& n = vertices[i].n;
		const vector3f& t = tan1[i];

		// Gram-Schmidt orthogonalize.
		vector3f tangent = t - n * dot(n, t);
		float l = norm(tangent);
		if (l > 0.f)
		{
			tangent /= l;
		}

		vertices[i].t = tangent;

		// Calculate handedness.
		vertices[i].tw = (dot(cross(n, t), tan2[i]) < 0.0f) ? -1.0f : 1.0f;
	}

	free(tan1);
#endif // VERTEX_ATTR_POSITION && VERTEX_ATTR_NORMAL && VERTEX_ATTR_TEX_COORD
}

// =========================================================================

/*
void Mesh::generateCubicTorus(float outerSize, float innerSize, float height)
{
	const float width = 0.5f * (outerSize - innerSize);
	const float position = 0.5f * (innerSize + width);
	assert(width > 0.f);
	Mesh paveA(Cube::numberOfVertices);
	Mesh paveB(Cube::numberOfVertices);
	Pave(width, height, outerSize).generateMesh(paveA);
	Pave(width, height, innerSize).generateMesh(paveB);
	paveB.rotate(DEG_TO_RAD * 90.f, 0, 1.f, 0);

	add(paveA, matrix4::translation(vector3f(position, 0.f, 0.f)));
	add(paveA, matrix4::translation(vector3f(-position, 0.f, 0.f)));
	add(paveB, matrix4::translation(vector3f(0.f, 0.f, position)));
	add(paveB, matrix4::translation(vector3f(0.f, 0.f, -position)));
}

// Génère un cube de 1 de côté, troué. Le trou va de (x1, y1) à (x2, y2)
//
//  +-------+
//  |       |
//  |  +--+ |
//  |  +--+ |
//  +-------+
void Mesh::generateCustomCubicTorus(float x1, float y1, float x2, float y2)
{
	assert(x1 < x2);
	assert(y1 < y2);
	assert(x1 < 1.f && x2 < 1.f && y1 < 1.f && y2 < 1.f);

	// génère un cube unitaire centré en 0,5
	Mesh& cube = mesh::getTempMesh();
	Pave(1.f, 1.f, 1.f).generateMesh(cube);
	cube.translate(0.5f, 0.5f, 0.5f);

	// pavé gauche
	add(cube, matrix4::scaling(vector3f(x1, 1.f, 1.f)));

	// pavé bas
	add(cube, matrix4::translation(vector3f(x1, 0.f, 0.f)).scale(vector3f(x2 - x1, y1, 1.f)));

	// pavé haut
	add(cube, matrix4::translation(vector3f(x1, y2, 0.f)).scale(vector3f(x2 - x1, 1.f - y2, 1.f)));

	// pavé droite
	add(cube, matrix4::translation(vector3f(x2, 0, 0.f)).scale(vector3f(1.f - x2, 1.f, 1.f)));
}

// FIXME: Attention à la globale : ce n'est pas thread safe !
float gThicknessRatio;
static vector3f couronneHFunc(float, float) { return vector3f(0, 0, 0); }
static float couronneRFunc(float t, float) { return (1.f - gThicknessRatio + gThicknessRatio * t); }

// cylindre creu avec une épaisseur
// thickness_ratio est l'épaisseur relative (entre 0 et 1)
void Mesh::generateTorusCylinder(float height, float diameter,
								 float thicknessRatio, int thetaFaces)
{
	gThicknessRatio = thicknessRatio;
	Revolution outPipe(pipeHFunc, pipeRFunc);
	Revolution inPipe(inPipeHFunc, pipeRFunc);
	Revolution couronne(couronneHFunc, couronneRFunc);

	Mesh& bellOut = getTempMesh();
	Mesh& bellIn = getTempMesh();
	Mesh& bellCouronne = getTempMesh();

	outPipe.generateMesh(bellOut, height, 0.5f * diameter, 1, thetaFaces);
	inPipe.generateMesh(bellIn, height, 0.5f * diameter * (1.f - thicknessRatio), 1, thetaFaces);
	couronne.generateMesh(bellCouronne, 1.f, 0.5f * diameter, 1, thetaFaces);

	add(bellOut);
	add(bellIn);
	add(bellCouronne);
	add(bellCouronne, matrix4::translation(vector3f(0, height, 0)).rotate(DEG_TO_RAD * 180.f, vector3f::ux));
}
*/

// Catmull-Clark
// Based on implementation by IQ from
//   http://www.iquilezles.org/www/material/breakpoint2007/bp2007.pdf
// Additional background on the algorithm:
//   http://www.rorydriscoll.com/2008/08/01/catmull-clark-subdivision-the-basics/

// Look for the edge [a-b] in the edges array and increase the counter.
// edges is a hash table mapping from an edge ID to a number of occurrences.
// Add the edge in the hash map if it didn't exist.
// Return the edge ID.
static int getEdgeID(int a, int b, int *edges, int& nedges)
{
	// Unique id for the edge.
	int n = (a > b) ? (a << 16) | b : (b << 16) | a;
	int i;
    for(i = 0; edges[2*i + 0] != n && i < nedges; i++)
		;
	edges[2*i + 0] = n;  // Store the edge id.
	edges[2*i + 1]++;    // Increase counter of occurrences
	// When i==nedge, it means we just added a new edge.
	// So update nedges.
	nedges += (i == nedges);
	return i;
}

void Mesh::smooth()
{
	Mesh originalMesh(quads.size);
	originalMesh.vertices.copyFrom(vertices);
	originalMesh.quads.copyFrom(quads);
	Clear();
	originalMesh.smooth(*this);
}

void Mesh::smooth(Mesh& dst) const
{
	static const float vpw[4] = {9.0f, 3.0f, 1.0f, 3.0f};
	static const float epw[4] = {3.0f, 3.0f, 1.0f, 1.0f};

	const int MAXEDGES = 5000;
	const int MAXVERTS = 5000;
	int* buffer = (int*)malloc((MAXEDGES + MAXVERTS) * sizeof(int));
	memset(buffer, 0, (MAXEDGES + MAXVERTS) * sizeof(int)); // clear edges and valences

	int eid[4], abcd[4];
	int* edges = buffer;
	int* face_valences = &buffer[MAXEDGES];  // Number of faces that contain a vertex
	int nedges = 0;
	int off_vp = quads.size/4;
	int off_ep = quads.size/4 + vertices.size;

	// We don't know yet how many vertices we'll have, so we oversize the array.
	dst.vertices.size = MAXVERTS;
	memset(dst.vertices.elt, 0, dst.vertices.size * sizeof(dst.vertices[0]));

	for (int j = 0; j < quads.size; j += 4)
	{
		for (int k = 0; k < 4; k++)
		{
			for(int i = 0; i < 4; i++)
			{
				abcd[i] = quads[j + (i+k) % 4]; // get the 4 vertices
			}
			eid[k] = getEdgeID(abcd[0], abcd[1], edges, nedges); // create edges
			face_valences[abcd[0]]++; // update face-valence
			dst.vertices[j/4].p += vertices[abcd[0]].p * 0.25f; // increment face point

			for (int i = 0; i < 4; i++)
			{
				// increment vertex point
				dst.vertices[off_vp + abcd[0]].p += vertices[abcd[i]].p * vpw[i];
				// increment edge point
				dst.vertices[off_ep + eid[k]].p +=  vertices[abcd[i]].p * epw[i];
			}
		}
		for (int k = 0; k < 4; k++) // make child faces
		{
			dst.quads.add(j/4);
			dst.quads.add(off_ep + eid[(3+k) & 3]);
			dst.quads.add(off_vp + quads[j + k]);
			dst.quads.add(off_ep + eid[(0+k) & 3]);
		}
	}

	// The vertex position is a weighted average. In the loop above, we added
	// values. Now we only need to divide.

	// We added vpw coefficients (total 16) for each adjacent face (face valence).
	for (int j = 0; j < vertices.size; j++)
	{
		dst.vertices[off_vp + j].p *= 0.0625f / (float)(face_valences[j]);
	}
	// We added epw coefficients (total 8) for each adjacent edge (edge valence).
	for (int j = 0; j < nedges; j++)
	{
		dst.vertices[off_ep + j].p *= 0.1250f / (float)(edges[j*2+1]);
	}
	free(buffer);

	dst.vertices.size = off_ep + nedges;
}
