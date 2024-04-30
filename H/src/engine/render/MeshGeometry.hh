#ifndef MESH_GEOMETRY_HH
#define MESH_GEOMETRY_HH

#include "engine/algebra/AABB3.hh"
#include "gfx/Geometry.hh"

// Set to 1 to get rough frustum culling using a bounding sphere.
#define ENABLE_SPHERE_CULLING	0

// Set to 1 to get finer frustum culling using an axis aligned bounding box.
#define ENABLE_AABB_CULLING		1

namespace Render
{
	struct MeshGeometry
	{
		Gfx::Geometry	geometry;

#if ENABLE_AABB_CULLING
		Algebra::AABB3	aabb;
#endif // ENABLE_AABB_CULLING

#if ENABLE_SPHERE_CULLING
		float			boundingSphereRadius;
#endif // ENABLE_SPHERE_CULLING
	};
}

#endif // MESH_GEOMETRY_HH
