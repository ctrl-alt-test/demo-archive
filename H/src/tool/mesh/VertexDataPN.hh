#ifndef VERTEX_DATA_PN_HH
#define VERTEX_DATA_PN_HH

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
	/// 3d position and normal.
	/// </summary>
	struct VertexDataPN
	{
		float x;
		float y;
		float z;

		float nx;
		float ny;
		float nz;

		static const VertexFormatDescriptor s_descriptor;
	};
}

#endif // VERTEX_DATA_PN_HH
