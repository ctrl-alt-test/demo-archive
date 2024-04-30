#include "HeightMap.hh"

#include "Mesh.hh"
#include "engine/container/Array.hxx"

using namespace Algebra;
using namespace mesh;

void Heightmap::ComputePositions(Container::Array<vertex>& vertices,
								 float x0, float z0, float x1, float z1,
								 int xFaces, int zFaces)
{
	for (int j = 0; j <= zFaces; ++j)
	{
		const float z = z0 + (z1 - z0) * float(j) / zFaces;
		for (int i = 0; i <= xFaces; ++i)
		{
			const float x = x0 + (x1 - x0) * float(i) / xFaces;
			const float y = m_heightFunc(x, z);

			const vertex p = {
#if VERTEX_ATTR_POSITION
				{ x, y, z },
#endif
#if VERTEX_ATTR_NORMAL
				{ 0, 0, 0 }, // Normal will be computed later.
#endif
#if VERTEX_ATTR_TANGENT
				{ 0, 0, 0 }, 0.f, // Tangent will be computed later.
#endif
#if VERTEX_ATTR_COLOR
				1.f, 1.f, 1.f, // Color (deprecated).
#endif
#if VERTEX_ATTR_TEX_COORD
				float(i) / xFaces, 1.f - float(j) / zFaces, // Simple texture coordinates.
#endif
#if VERTEX_ATTR_ID
				0,
#endif
			};

			vertices.add(p);
		}
	}
}

void Heightmap::UpdateQuadsIndices(Mesh& mesh, int vertexStart,
								   int xFaces, int zFaces)
{
	for (int j = 0; j < zFaces; ++j)
	{
		for (int i = 0; i < xFaces; ++i)
		{
			int length = xFaces + 1;
			mesh.quads.add(vertexStart + i     + j     * length);
			mesh.quads.add(vertexStart + i     + (j+1) * length);
			mesh.quads.add(vertexStart + (i+1) + (j+1) * length);
			mesh.quads.add(vertexStart + (i+1) + j     * length);
		}
	}
}

void Heightmap::GenerateMesh(Mesh& mesh,
							 float x0, float z0, float x1, float z1,
							 int xFaces, int zFaces)
{
	assert(xFaces >= 1);
	assert(zFaces >= 1);

#if DEBUG
	assert(mesh.vertices.max_size - mesh.vertices.size >= 4 * xFaces * zFaces);
#endif

	int vertexStart = mesh.vertices.size;
	ComputePositions(mesh.vertices, x0, z0, x1, z1, xFaces, zFaces);
	UpdateQuadsIndices(mesh, vertexStart, xFaces, zFaces);
}
