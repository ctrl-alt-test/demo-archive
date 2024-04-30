#include "Shark.hh"

#include "engine/algebra/Matrix.hxx"
#include "engine/mesh/Cube.hh"
#include "engine/mesh/Mesh.hh"
#include "engine/mesh/MeshPool.hh"
#include "tool/mesh//VertexDataPNT.hh"

#ifdef ENABLE_RUNTIME_COMPILATION
#include "RuntimeCompiledCPlusPlus/RuntimeObjectSystem/ObjectInterfacePerModule.h"

REGISTERCLASS(Shark);
ObjectId Shark::objectId;
Shark* Shark::instance = NULL;
void Shark::BuildMesh(mesh::Mesh& dest, mesh::Pool& pool)
{
	return instance->Build(dest, pool);
}
#endif

using namespace Algebra;
using namespace mesh;

#ifdef ENABLE_RUNTIME_COMPILATION
void Shark::Build(Mesh& dest, Pool& pool)
#else // !ENABLE_RUNTIME_COMPILATION
void Shark(Mesh& dest, Pool& pool)
#endif // !ENABLE_RUNTIME_COMPILATION
{
	bool smooth = true;

	cube(dest).Translate(-0.5f, -0.5f, -0.5f).Scale(9.f, 11.f, 10.f);

	// Head
	dest.transformFace(0, matrix4::scaling(1., 0.85f, 0.9f));
	dest.Extrude(0); dest.transformFace(0, matrix4::translation(-6, 0.1f, 0) * matrix4::scaling(1., 0.65f, 0.85f));
	dest.Extrude(0); dest.transformFace(0, matrix4::translation(-2.5, 0.2f, 0) * matrix4::scaling(1., 0.6f, 0.8f));
	dest.Extrude(0); dest.transformFace(0, matrix4::translation(-2., 0.4f, 0) * matrix4::scaling(1., 0.1f, 0.5f));

	// Tail
	dest.Extrude(4); dest.transformFace(4, matrix4::translation(12, 0, 0) * matrix4::scaling(1., 0.9f, 0.9f));
	dest.Extrude(4); dest.transformFace(4, matrix4::translation(10, -0.2f, 0) * matrix4::scaling(1., 0.75f, 0.6f));
	dest.Extrude(4); dest.transformFace(4, matrix4::translation(9, -0.2f, 0) * matrix4::scaling(1., 0.45f, 0.3f));
	dest.Extrude(4); dest.transformFace(4, matrix4::translation(1, 0, 0) * matrix4::scaling(1, 1.2f, 0.5f));
	dest.Extrude(4); dest.transformFace(4, matrix4::translation(4, 0, 0) * matrix4::scaling(1, 0.05f, 0.8f));

	// Right fin
	dest.Extrude(16); dest.transformFace(16, matrix4::translation(0, -1.5f, 0.5f) * matrix4::scaling(0.4f, 0.07f, 1));
	dest.Extrude(16); dest.transformFace(16, matrix4::translation(0.8f, -0.5f, -0.5f) * matrix4::scaling(1.4f, 1, 1));
	dest.Extrude(16); dest.transformFace(16, matrix4::translation(0.6f, -1.5f, -4) * matrix4::scaling(0.7f, 1, 1));
	dest.Extrude(16); dest.transformFace(16, matrix4::translation(2.5f, -2.5f, -4) * matrix4::scaling(0.3f, 0.7f, 1));

	// Left fin
	dest.Extrude(20); dest.transformFace(20, matrix4::translation(0, -1.5f, -0.5f) * matrix4::scaling(0.4f, 0.07f, 1));
	dest.Extrude(20); dest.transformFace(20, matrix4::translation(0.8f, -0.5f, 0.5) * matrix4::scaling(1.4f, 1, 1));
	dest.Extrude(20); dest.transformFace(20, matrix4::translation(0.6f, -1.5f, 4) * matrix4::scaling(0.7f, 1, 1));
	dest.Extrude(20); dest.transformFace(20, matrix4::translation(2.5f, -2.5f, 4) * matrix4::scaling(0.3f, 0.7f, 1));

	// Bottom caudal fin
	dest.Extrude(148); dest.transformFace(148, matrix4::translation(3, -4, 0) * matrix4::scaling(0.7f, 0.1f, 0.2f));
	dest.Extrude(148); dest.transformFace(148, matrix4::translation(1.9f, -1, 0) * matrix4::scaling(0.2f, 0.1f, 0.2f));

	// Top caudal fin
	dest.Extrude(140); dest.transformFace(140, matrix4::translation(3, 4, 0) * matrix4::scaling(1, 0.1f, 0.2f));
	dest.Extrude(140); dest.transformFace(140, matrix4::translation(3.5f, 3, 0) * matrix4::scaling(0.6f, 0.6f, 1));
	dest.Extrude(140); dest.transformFace(140, matrix4::translation(3.5f, 4, 0) * matrix4::scaling(0.2f, 0.2f, 1));
	dest.Extrude(352); dest.transformFace(352, matrix4::translation(-0.5f, -1, 0) * matrix4::scaling(0.4f, 0.1f, 1));
	dest.Extrude(352); dest.transformFace(352, matrix4::translation(0.5f, -0.8f, 0) * matrix4::scaling(0.8f, -0.5f, 1));

	// First dorsal fin
	dest.Extrude(76); dest.transformFace(76, matrix4::translation(0, -0.2f, 0) * matrix4::scaling(0.8f, 1, 0.08f));
	dest.Extrude(76); dest.transformFace(76, matrix4::translation(-0.5f, 0.8f, 0) * matrix4::scaling(0.7f, 0.5f, 0.9f));
	dest.Extrude(76); dest.transformFace(76, matrix4::translation(1., 3, 0) * matrix4::scaling(0.6f, 1, 0.7f));
	dest.Extrude(76); dest.transformFace(76, matrix4::translation(1.2f, 2, 0) * matrix4::scaling(0.2f, 1, 0.7f));

	// Second dorsal fin
	dest.Extrude(108); dest.transformFace(108, matrix4::translation(0, 0, 0) * matrix4::scaling(0.5f, 0.5f, 0.25f));
	dest.Extrude(108); dest.transformFace(108, matrix4::translation(-0.8f, 1, 0) * matrix4::scaling(0.3f, 0.5f, 0.5f));
	dest.Extrude(108); dest.transformFace(108, matrix4::translation(0.7f, 1, 0) * matrix4::scaling(0.3f, 0.2f, 0.5f));

	// Anal fin
	dest.Extrude(116); dest.transformFace(116, matrix4::translation(0, 0, 0) * matrix4::scaling(0.7f, 0.7f, 0.25f));
	dest.Extrude(116); dest.transformFace(116, matrix4::translation(0, -0.5, 0) * matrix4::scaling(0.5f, 0.5f, 0.5f));
	dest.Extrude(116);
	dest.Extrude(116); dest.transformFace(116, matrix4::translation(1.5, -1, 0) * matrix4::scaling(1., 0, 0.5f));

	dest.Scale(0.5f, 0.5f, 0.5f);

	if (smooth)
	{
		Mesh& tmp = pool.GetTempMesh();
		//Mesh& tmp2 = pool.GetTempMesh();
		dest.smooth(tmp); dest.Clear(); tmp.smooth(dest);//tmp2); tmp2.smooth(res);
	}
	else
	{
		dest.sharpenEdges();
	}

	dest.ComputeNormals();
}
