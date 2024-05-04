#ifndef VERTEX_DATA_PNT_HH
#define VERTEX_DATA_PNT_HH

#include "gfx/VertexAttribute.hh"
#include "MeshHelper.hh"

namespace mesh
{
	struct vertex;
}

namespace Tool
{
	/// <summary>
	/// Vertex attributes:
	/// 3d position, normal and texture coordinates.
	/// </summary>
	struct VertexDataPNT
	{
		float x;
		float y;
		float z;

		float nx;
		float ny;
		float nz;

		float u;
		float v;

		static const VertexFormatDescriptor s_descriptor;
	};
}

#endif // VERTEX_DATA_PNT_HH
