#include "Utils.hh"

#include "engine/algebra/AABB3.hxx"
#include "engine/algebra/Functions.hh"
#include "engine/core/Debug.hh"
#include "engine/noise/Rand.hh"
#include "engine/mesh/Mesh.hh"
#include "engine/render/MeshGeometry.hh"
#include <cassert>
#if DEBUG
#include <cstdio>
#endif // DEBUG

using namespace mesh;

Mesh& mesh::AddNoise(Mesh& mesh, Noise::Rand& rand, float intensity)
{
	for (int i = 0; i < mesh.vertices.size; i++)
	{
		mesh.vertices[i].p.x += rand.sfgen() * intensity;
		mesh.vertices[i].p.y += rand.sfgen() * intensity;
		mesh.vertices[i].p.z += rand.sfgen() * intensity;
	}
	return mesh;
}

void mesh::GenerateAxisAlignedTextureCoordinates(Mesh& dest, float scale)
{
	for (int i = 0; i < dest.vertices.size; ++i)
	{
		vertex& vertex = dest.vertices[i];
		if (Algebra::abs(vertex.n.x) >= Algebra::abs(vertex.n.y) &&
			Algebra::abs(vertex.n.x) >= Algebra::abs(vertex.n.z))
		{
			if (vertex.n.x > 0.f)
			{
				vertex.u = scale * -vertex.p.z;
				vertex.v = scale * vertex.p.y;
			}
			else
			{
				vertex.u = scale * vertex.p.z;
				vertex.v = scale * vertex.p.y;
			}
		}
		else if (Algebra::abs(vertex.n.y) >= Algebra::abs(vertex.n.z) &&
				 Algebra::abs(vertex.n.y) >= Algebra::abs(vertex.n.x))
		{
			if (vertex.n.y > 0.f)
			{
				vertex.u = scale * vertex.p.x;
				vertex.v = scale * -vertex.p.z;
			}
			else
			{
				vertex.u = scale * vertex.p.x;
				vertex.v = scale * vertex.p.z;
			}
		}
		else
		{
			if (vertex.n.z > 0.f)
			{
				vertex.u = scale * vertex.p.x;
				vertex.v = scale * vertex.p.y;
			}
			else
			{
				vertex.u = scale * -vertex.p.x;
				vertex.v = scale * vertex.p.y;
			}
		}
	}
}

void mesh::ComputeBoundingVolume(const Mesh& mesh, Render::MeshGeometry* geometry)
{
	assert(mesh.vertices.size > 0);

#if ENABLE_SPHERE_CULLING
	float biggestSqrDistance = 0.f;
#endif // ENABLE_SPHERE_CULLING

#if ENABLE_AABB_CULLING
	Algebra::AABB3 volume = { mesh.vertices[0].p, mesh.vertices[0].p };
#endif // ENABLE_AABB_CULLING

	for (int i = 0; i < mesh.vertices.size; ++i)
	{
		const Algebra::vector3f& p = mesh.vertices[i].p;

#if ENABLE_SPHERE_CULLING
		float sqrDistance = Algebra::dot(p, p);
		if (sqrDistance > biggestSqrDistance)
		{
			biggestSqrDistance = sqrDistance;
		}
#endif // ENABLE_SPHERE_CULLING

#if ENABLE_AABB_CULLING
		volume.expand(p);
#endif // ENABLE_AABB_CULLING
	}

#if ENABLE_SPHERE_CULLING
	geometry->boundingSphereRadius = msys_sqrtf(biggestSqrDistance);
#endif // ENABLE_SPHERE_CULLING
#if ENABLE_AABB_CULLING
	geometry->aabb = volume;
#endif // ENABLE_AABB_CULLING
}

#if DEBUG
void mesh::ExportAsWavefront(const mesh::Mesh& mesh, const char* fileName)
{
	FILE* fp = fopen(fileName, "w");
	if (fp == NULL)
	{
		LOG_ERROR("Cannot open '%s', mesh won't be exported.", fileName);
	}
	else
	{
		fprintf(fp, "# Wavefront OBJ file generated\n# by %s\n# in %s@l.%d\n#\n", __FUNCTION__, __FILE__, __LINE__);
		fprintf(fp, "# %d vertices and %d indices\n\n", mesh.vertices.size, mesh.quads.size);

		fprintf(fp, "\n# %d vertices:\n", mesh.vertices.size);
		for (int i = 0; i < mesh.vertices.size; ++i)
		{
			const Algebra::vector3f& p = mesh.vertices[i].p;
			fprintf(fp, "v %f %f %f\n", p.x, p.y, p.z);
		}

		fprintf(fp, "\n# %d texture coordinates:\n", mesh.vertices.size);
		for (int i = 0; i < mesh.vertices.size; ++i)
		{
			fprintf(fp, "vt %f %f\n", mesh.vertices[i].u, mesh.vertices[i].v);
		}

		fprintf(fp, "\n# %d normals:\n", mesh.vertices.size);
		for (int i = 0; i < mesh.vertices.size; ++i)
		{
			const Algebra::vector3f& n = mesh.vertices[i].n;
			fprintf(fp, "vn %f %f %f\n", n.x, n.y, n.z);
		}

		fprintf(fp, "\n# %d faces:\n", mesh.quads.size / 4);
		for (int i = 0; i < mesh.quads.size; i += 4)
		{
			fprintf(fp, "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d\n",
				mesh.quads[i + 0] + 1, mesh.quads[i + 0] + 1, mesh.quads[i + 0] + 1,
				mesh.quads[i + 1] + 1, mesh.quads[i + 1] + 1, mesh.quads[i + 1] + 1,
				mesh.quads[i + 2] + 1, mesh.quads[i + 2] + 1, mesh.quads[i + 2] + 1,
				mesh.quads[i + 3] + 1, mesh.quads[i + 3] + 1, mesh.quads[i + 3] + 1);
		}

		fclose(fp);
	}

}
#endif // DEBUG
