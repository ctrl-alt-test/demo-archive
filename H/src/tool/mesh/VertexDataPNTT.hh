#ifndef VERTEX_DATA_PNTT_HH
#define VERTEX_DATA_PNTT_HH

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
	/// 3d position, normal, tangent and texture coordinates.
	/// </summary>
	struct VertexDataPNTT
	{
		float x;
		float y;
		float z;

		float nx;
		float ny;
		float nz;

		float tx;
		float ty;
		float tz;
		float tw;

		float u;
		float v;

		static const VertexFormatDescriptor s_descriptor;
	};
}

#endif // VERTEX_DATA_PNTT_HH
