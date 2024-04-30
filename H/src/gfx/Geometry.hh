#ifndef GEOMETRY_HH
#define GEOMETRY_HH

#include "ResourceID.hh"

// FIXME: This kind of definition is not ideal. We probably want to
// make it a project setting.
#if DEBUG
#define GFX_ENABLE_VERTEX_BUFFER_OFFSET	1
#else // !DEBUG
#define GFX_ENABLE_VERTEX_BUFFER_OFFSET	0
#endif // !DEBUG

namespace Gfx
{
	/// <summary>
	/// Geometry instances for a drawcall. Consists in a vertex buffer,
	/// a number of vertex indices to use and a number of instances to
	/// draw.
	/// The name "Geometry" is the same as in "geometry instancing".
	/// </summary>
	struct Geometry
	{
		VertexBufferID	vertexBuffer;
		int				numberOfIndices;
		int				numberOfInstances;
#if GFX_ENABLE_VERTEX_BUFFER_OFFSET
		int				firstIndexOffset; // FIXME: find a good name.
#endif // GFX_ENABLE_VERTEX_BUFFER_OFFSET
	};
}

#endif // GEOMETRY_HH
