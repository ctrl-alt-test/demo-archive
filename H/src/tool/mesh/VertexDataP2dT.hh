#ifndef VERTEX_DATA_P2DT_HH
#define VERTEX_DATA_P2DT_HH

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
	/// 2d position and texture coordinates.
	/// </summary>
	struct VertexDataP2dT
	{
		float x;
		float y;

		float u;
		float v;

		static const VertexFormatDescriptor s_descriptor;
	};
}

#endif // VERTEX_DATA_P2DT_HH
