#include "Marching.hh"

#include "engine/algebra/Interpolation.hxx"
#include "engine/algebra/Vector3.hxx"
#include "engine/container/HashTable.hxx"
#include "engine/container/Utils.hh"

using namespace Algebra;
using namespace mesh;

struct SDFVertex
{
	int					i;
	int					j;
	int					k;
	Algebra::vector3f	p;
	float				d;
};

// Used as the key for the cache.
struct edge
{
	int	i0;
	int	j0;
	int	k0;
	int	i1;
	int	j1;
	int	k1;

	bool operator == (const edge& rhs) const
	{
		return (
			i0 == rhs.i0 &&
			j0 == rhs.j0 &&
			k0 == rhs.k0 &&
			i1 == rhs.i1 &&
			j1 == rhs.j1 &&
			k1 == rhs.k1);
	}
};

static
void CreatePoly(Mesh& dest,
				Container::HashTable<edge, int>& cache,
				SDFFunction sdf, const Algebra::vector3f& dp,
				const SDFVertex** edgesToSplit)
{
	for (int i = 0; i < 4; ++i)
	{
		SDFVertex v1 = *edgesToSplit[2 * i];
		SDFVertex v2 = *edgesToSplit[2 * i + 1];

		// Order v1 and v2 to make sure both edges
		// (v1, v2) and (v2, v1) will generate the
		// same hash for the cache.
		if (v2.k < v1.k ||
			(v2.k == v1.k && (v2.j < v1.j ||
							  (v2.j == v1.j && v2.i < v1.i))))
		{
			v1 = *edgesToSplit[2 * i + 1];
			v2 = *edgesToSplit[2 * i];
		}
		// Corner case: if the point is at the end of the edge, it has
		// to be shared with other edges. So again, we make sure those
		// points will generate the same hash for the cache.
		if (v1.d == 0.f)
		{
			v2.i = v1.i;
			v2.j = v1.j;
			v2.k = v1.k;
		}
		if (v2.d == 0.f)
		{
			v1.i = v2.i;
			v1.j = v2.j;
			v1.k = v2.k;
		}
		edge e =
		{
			v1.i, v1.j, v1.k,
			v2.i, v2.j, v2.k
		};

		int index = dest.vertices.size;
		int* cached = cache[e];
		if (cached != NULL)
		{
			index = *cached;
		}
		else
		{
			assert((v1.d > 0) != (v2.d > 0));

			// Estimate the point by interpolating between the
			// tetrahedron edge vertices.
			const vector3f p = mix(v1.p, v2.p, v1.d / (v1.d - v2.d));

			// Estimate local gradient to get normal.
			float d0 = sdf(p.x, p.y, p.z);
			float dx = sdf(p.x + dp.x, p.y       , p.z       ) - d0;
			float dy = sdf(p.x       , p.y + dp.y, p.z       ) - d0;
			float dz = sdf(p.x       , p.y       , p.z + dp.z) - d0;
			const vector3f gradient = { dx / dp.x, dy / dp.y, dz / dp.z };
			vector3f n = { 0.f, 0.f, 0.f };
			float length = norm(gradient);
			if (length > 0.f)
			{
				n = gradient / length;
			}

			const vertex vertex = { {p.x, p.y, p.z}, {n.x, n.y, n.z}, { 0, 0, 0 }, 0, 0, 0, };
			dest.vertices.add(vertex);
			cache.add(e, index);
		}
		dest.quads.add(index);
	}
}

static
void PolygonizeTetrahedron(Mesh& dest,
						   Container::HashTable<edge, int>& cache,
						   SDFFunction sdf, const Algebra::vector3f& dp,
						   const SDFVertex& a,
						   const SDFVertex& b,
						   const SDFVertex& c,
						   const SDFVertex& d)
{
	// This is how we consider the tetrahedron:
	//
	//     C
	//    /:\
	//   / : \
	//  A-----B
	//   \ : /
	//    \:/
	//     D

	const SDFVertex* orientations[] =
	{
		&a, &b, &c, &d,
		&b, &c, &a, &d,
		&c, &d, &a, &b,
		&d, &a, &c, &b,
		// The following two orientations are only for the quad case.
		&a, &c, &d, &b,
		&b, &d, &c, &a,
	};
	const SDFVertex* edgesToSplit[8];
	const SDFVertex** list = edgesToSplit;

	bool createPoly = false;
	for (int i = 0; i < 6; ++i)
	{
		const SDFVertex* vertex1 = orientations[4 * i];
		const SDFVertex* vertex2 = orientations[4 * i + 1];
		const SDFVertex* vertex3 = orientations[4 * i + 2];
		const SDFVertex* vertex4 = orientations[4 * i + 3];

		// Test for quad:
		if (vertex1->d > 0 && vertex2->d > 0 &&
			vertex3->d <= 0 && vertex4->d <= 0)
		{
			*(list++) = vertex1;
			*(list++) = vertex3;
			*(list++) = vertex1;
			*(list++) = vertex4;
			*(list++) = vertex2;
			*(list++) = vertex4;
			*(list++) = vertex2;
			*(list++) = vertex3;
			createPoly = true;
			break;
		}

		// Test for triangle front facing:
		else if (i < 4 &&
				 vertex1->d > 0 &&
				 vertex2->d <= 0 && vertex3->d <= 0 && vertex4->d <= 0)
		{
			*(list++) = vertex1;
			*(list++) = vertex2;
			*(list++) = vertex1;
			*(list++) = vertex3;
			*(list++) = vertex1;
			*(list++) = vertex4;
			*(list++) = vertex1; // Repeat last vertex.
			*(list++) = vertex4;
			createPoly = true;
			break;
		}

		// Test for triangle back facing:
		else if (i < 4 &&
				 vertex1->d <= 0 &&
				 vertex2->d > 0 && vertex3->d > 0 && vertex4->d > 0)
		{
			*(list++) = vertex1;
			*(list++) = vertex4;
			*(list++) = vertex1;
			*(list++) = vertex3;
			*(list++) = vertex1;
			*(list++) = vertex2;
			*(list++) = vertex1; // Repeat last vertex.
			*(list++) = vertex2;
			createPoly = true;
			break;
		}
	}

	if (createPoly)
	{
		CreatePoly(dest, cache, sdf, dp, edgesToSplit);
	}
}

static
void PolygonizeVoxel(Mesh& dest,
					 Container::HashTable<edge, int>& cache,
					 SDFFunction sdf, const Algebra::vector3f& dp,
					 int i0, int j0, int k0,
					 int i1, int j1, int k1,
					 float x0, float y0, float z0,
					 float x1, float y1, float z1)
{
	// Using the wikipedia page illustration ordering.
	//
	//   ^ y        3----2
	//   |         /:   /|
	//   |        0----1 |
	//   +---> z  | 7..|.6
	//  /         |'   |/
	// x          4----5

	// SIZE-CODING:
	// Writing in this obfuscated way saves ~68 bytes on the compressed
	// binary, compared to the naive version after.
#if 1
	SDFVertex vertices[8];
	for (unsigned int i = 0; i < ARRAY_LEN(vertices); ++i)
	{
		const bool chooseX1 = (i%4 < 2);
		const bool chooseY1 = (i < 4);
		const bool chooseZ1 = ((i+1)%4 > 1);

		SDFVertex& vertex = vertices[i];
		vertex.i = chooseX1 ? i1 : i0;
		vertex.j = chooseY1 ? j1 : j0;
		vertex.k = chooseZ1 ? k1 : k0;
		vertex.p.x = chooseX1 ? x1 : x0;
		vertex.p.y = chooseY1 ? y1 : y0;
		vertex.p.z = chooseZ1 ? z1 : z0;
		vertex.d = sdf(vertex.p.x, vertex.p.y, vertex.p.z);
	}
#else
	const SDFVertex vertices[] = {
		{ i1, j1, k0, { x1, y1, z0 }, sdf(x1, y1, z0) },
		{ i1, j1, k1, { x1, y1, z1 }, sdf(x1, y1, z1) },
		{ i0, j1, k1, { x0, y1, z1 }, sdf(x0, y1, z1) },
		{ i0, j1, k0, { x0, y1, z0 }, sdf(x0, y1, z0) },
		{ i1, j0, k0, { x1, y0, z0 }, sdf(x1, y0, z0) },
		{ i1, j0, k1, { x1, y0, z1 }, sdf(x1, y0, z1) },
		{ i0, j0, k1, { x0, y0, z1 }, sdf(x0, y0, z1) },
		{ i0, j0, k0, { x0, y0, z0 }, sdf(x0, y0, z0) },
	};
#endif

	PolygonizeTetrahedron(dest, cache, sdf, dp, vertices[2], vertices[7], vertices[1], vertices[3]);
	PolygonizeTetrahedron(dest, cache, sdf, dp, vertices[0], vertices[7], vertices[3], vertices[1]);
	PolygonizeTetrahedron(dest, cache, sdf, dp, vertices[6], vertices[1], vertices[2], vertices[7]);
	PolygonizeTetrahedron(dest, cache, sdf, dp, vertices[0], vertices[1], vertices[4], vertices[7]);
	PolygonizeTetrahedron(dest, cache, sdf, dp, vertices[1], vertices[4], vertices[7], vertices[5]);
	PolygonizeTetrahedron(dest, cache, sdf, dp, vertices[1], vertices[7], vertices[6], vertices[5]);
}

void Marching::GenerateMesh(Mesh& dest,
							float x0, float y0, float z0,
							float x1, float y1, float z1,
							int xFaces, int yFaces, int zFaces)
{
	// A cache to reuse index for same vertices.
	const int expectedVertices = xFaces * yFaces * zFaces * 8 - 1;
	Container::HashTable<edge, int> cache(expectedVertices);

	// dp used for sampling nearby values and estimate local gradient.
	const Algebra::vector3f dp = {
		0.25f * (x1 - x0) / float(xFaces),
		0.25f * (y1 - y0) / float(yFaces),
		0.25f * (z1 - z0) / float(zFaces),
	};

	int k0 = 0;
	float zk0 = z0;
	for (int k = 1; k <= zFaces; ++k)
	{
		float zk = mix(z0, z1, float(k) / float(zFaces));

		int j0 = 0;
		float yj0 = y0;
		for (int j = 1; j <= yFaces; ++j)
		{
			float yj = mix(y0, y1, float(j) / float(yFaces));

			int i0 = 0;
			float xi0 = x0;
			for (int i = 1; i <= xFaces; ++i)
			{
				float xi = mix(x0, x1, float(i) / float(xFaces));

				PolygonizeVoxel(dest, cache, m_sdf, dp,
					i0, j0, k0, i, j, k,
					xi0, yj0, zk0, xi, yj, zk);

				i0 = i;
				xi0 = xi;
			}
			j0 = j;
			yj0 = yj;
		}
		k0 = k;
		zk0 = zk;
	}
}
