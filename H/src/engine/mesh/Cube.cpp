//
// Définition du cube
//

#include "Cube.hh"

#include "Mesh.hh"
#include "Vertex.hh"
#include "engine/container/Array.hxx"
#include "engine/container/Utils.hh"
#include <cstring>

static const char s_vertices[] = {
	0, 0, 0,
	0, 0, 1,
	0, 1, 1,
	0, 1, 0,

	1, 0, 0,
	1, 1, 0,
	1, 1, 1,
	1, 0, 1,

	0, 0, 0,
	1, 0, 0,
	1, 0, 1,
	0, 0, 1,

	0, 1, 0,
	0, 1, 1,
	1, 1, 1,
	1, 1, 0,

	0, 0, 0,
	0, 1, 0,
	1, 1, 0,
	1, 0, 0,

	0, 0, 1,
	1, 0, 1,
	1, 1, 1,
	0, 1, 1,
};
static const unsigned int s_numberOfVertices = ARRAY_LEN(s_vertices) / 3;

static const char s_indices[] = {
	0, 1, 2, 3,
	4, 5, 6, 7,
	0, 4, 7, 1,
	3, 2, 6, 5,
	0, 3, 5, 4,
	1, 7, 6, 2,
};

namespace mesh
{
	Mesh& sharpCube(Mesh& dest, CubeFaces faces)
	{
		int k = 0;
		for (unsigned int i = 0; i < s_numberOfVertices; ++i)
		{
			if (((int)faces & (1 << (i / 4))) != 0)
			{
				vertex v;
				memset(&v, 0, sizeof(vertex));
				v.p.x = s_vertices[3 * i];
				v.p.y = s_vertices[3 * i + 1];
				v.p.z = s_vertices[3 * i + 2];
				dest.vertices.add(v);
				dest.quads.add(k);
				k++;
			}
		}
		return dest;
	}

	Mesh& cube(Mesh& dest, CubeFaces faces)
	{
		int first = dest.vertices.size;
		for (int i = 0; i < 8; i++)
		{
			vertex v;
			memset(&v, 0, sizeof(vertex));
			v.p.x = s_vertices[3 * i];
			v.p.y = s_vertices[3 * i + 1];
			v.p.z = s_vertices[3 * i + 2];
			dest.vertices.add(v);
		}

		for (unsigned int i = 0; i < ARRAY_LEN(s_indices); ++i)
		{
			if (((unsigned int)faces & (1 << (i / 4))) != 0)
			{
				dest.quads.add(first + s_indices[i]);
			}
		}
		return dest;
	}
}
