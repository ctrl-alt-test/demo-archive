#ifndef MESH_UTILS_HH
#define MESH_UTILS_HH

#include <cstddef>

namespace Algebra
{
	struct AABB3;
}

namespace Noise
{
	class Rand;
}

namespace Render
{
	struct MeshGeometry;
}

namespace mesh
{
	struct Mesh;

	/// <summary>
	/// Modifies a given mesh by adding noise to the position of the
	/// vertices. The same mesh is returned for convenience.
	/// </summary>
	Mesh& AddNoise(Mesh& mesh, Noise::Rand& rand, float intensity);

	/// <summary>
	/// Computes the bounding volume of a mesh, and stores it in the
	/// given MeshGeometry.
	/// </summary>
	void ComputeBoundingVolume(const Mesh& mesh, Render::MeshGeometry* geometry);

	/// <summary>
	/// Generates texture coordinates by choosing the axis plane
	/// (x, y), (y, z) or (z, x) each face is the closest to.
	/// </summary>
	void GenerateAxisAlignedTextureCoordinates(mesh::Mesh& dest, float scale);

#if DEBUG
	/// <summary>
	/// Exports a generated mesh as a .obj (Wavefront format).
	/// </summary>
	void ExportAsWavefront(const mesh::Mesh& mesh, const char* fileName);
#endif // DEBUG
}

#endif // MESH_UTILS_HH
