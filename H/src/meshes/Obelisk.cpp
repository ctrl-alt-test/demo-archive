#include "Obelisk.hh"

#include "engine/algebra/Functions.hh"
#include "engine/algebra/Interpolation.hxx"
#include "engine/algebra/Matrix.hxx"
#include "engine/algebra/Spline.hh"
#include "engine/algebra/Utils.hh"
#include "engine/algebra/Vector2.hxx"
#include "engine/container/Utils.hh"
#include "engine/mesh/Cube.hh"
#include "engine/mesh/Mesh.hh"
#include "engine/mesh/MeshPool.hh"
#include "engine/mesh/Revolution.hh"
#include "engine/mesh/Utils.hh"

#define ENABLE_OBELISK_MESH 1

#ifdef ENABLE_RUNTIME_COMPILATION
#include "RuntimeCompiledCPlusPlus/RuntimeObjectSystem/ObjectInterfacePerModule.h"

REGISTERCLASS(Obelisk);
ObjectId Obelisk::objectId;
Obelisk* Obelisk::instance = NULL;
void Obelisk::BuildMesh(mesh::Mesh& dest, mesh::Pool& pool)
{
	return instance->Build(dest, pool);
}
#endif

using namespace Algebra;
using namespace mesh;

#ifdef ENABLE_RUNTIME_COMPILATION
void Obelisk::Build(Mesh& dest, Pool& /*pool*/)
#else // !ENABLE_RUNTIME_COMPILATION
void Obelisk(Mesh& dest, Pool& /*pool*/)
#endif // !ENABLE_RUNTIME_COMPILATION
{
#if ENABLE_OBELISK_MESH
	// Inspired by https://en.wikipedia.org/wiki/Luxor_Obelisk
	cube(dest, (mesh::CubeFaces)(mesh::face_all & ~mesh::face_bottom))
		.Translate(-0.5f, 0, -0.5f)
		.Scale(4.f, 1.f, 4.f);

	const float transcale[] = {
		0.5f, 0.8f,
		0.2f, 1.1f,
		2.f, 1.f,
		0.25f, 0.91f,
		0.05f, 1.f,
		0.125f, 1.1f,
		0.125f, 1.f,
		0.125f, 0.95f,
		17.f, 0.65f,
		2.f, 0.0625f,
	};
	for (size_t i = 0; i < ARRAY_LEN(transcale); i += 2)
	{
		int f = 8;
		dest.Extrude(f);
		dest.transformFace(f,
			matrix4::translation(0.f, transcale[i], 0.f)
			.scale(transcale[i + 1], 1.f, transcale[i + 1]));
	}
	dest.sharpenEdges();
	dest.ComputeNormals();
	GenerateAxisAlignedTextureCoordinates(dest, 0.15f);
	dest.ComputeTangents();
#endif // ENABLE_OBELISK_MESH
}
