#include "Dolphin.hh"

#include "engine/algebra/Matrix.hxx"
#include "engine/mesh/Cube.hh"
#include "engine/mesh/Mesh.hh"
#include "engine/mesh/MeshPool.hh"
#include "tool/mesh/VertexDataPNT.hh"

#ifdef ENABLE_RUNTIME_COMPILATION
#include "RuntimeCompiledCPlusPlus/RuntimeObjectSystem/ObjectInterfacePerModule.h"

REGISTERCLASS(Dolphin);
ObjectId Dolphin::objectId;
Dolphin* Dolphin::instance = NULL;
void Dolphin::BuildMesh(mesh::Mesh& dest, mesh::Pool& pool)
{
	return instance->Build(dest, pool);
}
#endif

using namespace Algebra;
using namespace mesh;

#ifdef ENABLE_RUNTIME_COMPILATION
void Dolphin::Build(Mesh& dest, Pool& pool)
#else // !ENABLE_RUNTIME_COMPILATION
void Dolphin(Mesh& dest, Pool& pool)
#endif // !ENABLE_RUNTIME_COMPILATION
{
	bool smooth = true;

	cube(dest).Translate(-0.5f, -0.5f, -0.5f).Scale(9.f, 11.f, 10.f);

	// Head
	dest.transformFace(0, matrix4::scaling(1.0f, 0.9f, 0.9f));
	dest.Extrude(0); dest.transformFace(0, matrix4::translation(-6.0f, -0.6f, 0) * matrix4::scaling(1.0f, 0.65f, 0.80f));
	dest.Extrude(0); dest.transformFace(0, matrix4::translation(-2.5f, -0.8f, 0) * matrix4::scaling(1.0f, 0.50f, 0.55f));
	dest.Extrude(0); dest.transformFace(0, matrix4::translation( 0.2f,  0.0f, 0) * matrix4::scaling(1.0f, 1.00f, 1.00f));
	dest.Extrude(0); dest.transformFace(0, matrix4::translation(-3.0f,  0.2f, 0) * matrix4::scaling(1.0f, 0.60f, 0.50f));
	dest.Extrude(0); dest.transformFace(0, matrix4::translation(-2.5f,  0.3f, 0) * matrix4::scaling(1.0f, 0.90f, 1.00f));

	// Tail
	dest.Extrude(4); dest.transformFace(4, matrix4::translation(12.0f,  0.0f, 0) * matrix4::scaling(1.0f, 0.85f, 0.82f));
	dest.Extrude(4); dest.transformFace(4, matrix4::translation( 5.5f,  0.1f, 0) * matrix4::scaling(1.0f, 0.70f, 0.50f));
	dest.Extrude(4); dest.transformFace(4, matrix4::translation( 7.0f, -1.5f, 0) * matrix4::scaling(1.0f, 0.70f, 0.45f));
	dest.Extrude(4); dest.transformFace(4, matrix4::translation( 5.0f, -2.0f, 0) * matrix4::scaling(1.0f, 0.60f, 0.50f));

	// Right flipper
	dest.Extrude(16); dest.transformFace(16, matrix4::translation(-2.0f, -3.0f,  0.5f) * matrix4::scaling(0.4f, 0.07f, 1));
	dest.Extrude(16); dest.transformFace(16, matrix4::translation( 0.8f, -0.5f, -0.5f) * matrix4::scaling(1.4f, 1.00f, 1));
	dest.Extrude(16); dest.transformFace(16, matrix4::translation( 0.2f, -1.5f, -3.0f) * matrix4::scaling(0.8f, 1.00f, 1));
	dest.Extrude(16); dest.transformFace(16, matrix4::translation( 2.5f, -2.5f, -3.0f) * matrix4::scaling(0.4f, 0.70f, 1));

	// Left flipper
	dest.Extrude(20); dest.transformFace(20, matrix4::translation(-2.0f, -3.0f, -0.5f) * matrix4::scaling(0.4f, 0.07f, 1));
	dest.Extrude(20); dest.transformFace(20, matrix4::translation( 0.8f, -0.5f,  0.5f) * matrix4::scaling(1.4f, 1.00f, 1));
	dest.Extrude(20); dest.transformFace(20, matrix4::translation( 0.2f, -1.5f,  3.0f) * matrix4::scaling(0.8f, 1.00f, 1));
	dest.Extrude(20); dest.transformFace(20, matrix4::translation( 2.5f, -2.5f,  3.0f) * matrix4::scaling(0.4f, 0.70f, 1));

	// Left fluke
	dest.Extrude(160); dest.transformFace(160, matrix4::translation(1.0f, -0.5f, 0.0f) * matrix4::scaling(1.0f, 0.15f, 1));
	dest.Extrude(160); dest.transformFace(160, matrix4::translation(0.5f, -0.5f, 4.0f) * matrix4::scaling(0.8f, 1.00f, 1));
	dest.Extrude(160); dest.transformFace(160, matrix4::translation(1.0f, -1.0f, 2.5f) * matrix4::scaling(0.7f, 1.00f, 1));
	dest.Extrude(160); dest.transformFace(160, matrix4::translation(1.5f, -0.5f, 1.0f) * matrix4::scaling(0.8f, 0.50f, 1));

	// Right fluke
	dest.Extrude(152); dest.transformFace(152, matrix4::translation(1.0f, -0.5f,  0.0f) * matrix4::scaling(1.0f, 0.15f, 1));
	dest.Extrude(152); dest.transformFace(152, matrix4::translation(0.5f, -0.5f, -4.0f) * matrix4::scaling(0.8f, 1.00f, 1));
	dest.Extrude(152); dest.transformFace(152, matrix4::translation(1.0f, -1.0f, -2.5f) * matrix4::scaling(0.7f, 1.00f, 1));
	dest.Extrude(152); dest.transformFace(152, matrix4::translation(1.5f, -0.5f, -1.0f) * matrix4::scaling(0.8f, 0.50f, 1));

	// Dorsal fin
	dest.Extrude(108); dest.transformFace(108, matrix4::translation(0.0f, -0.2f, 0) * matrix4::scaling(0.75f, 1.0f, 0.12f));
	dest.Extrude(108); dest.transformFace(108, matrix4::translation(0.0f,  0.8f, 0) * matrix4::scaling(0.75f, 0.5f, 0.90f));
	dest.Extrude(108); dest.transformFace(108, matrix4::translation(1.0f,  2.2f, 0) * matrix4::scaling(0.50f, 1.0f, 0.80f));
	dest.Extrude(108); dest.transformFace(108, matrix4::translation(3.5f,  2.5f, 0) * matrix4::scaling(0.80f, 1.0f, 0.80f));

	dest.Scale(0.5f, 0.5f, 0.5f);

	if (smooth)
	{
		Mesh& tmp = pool.GetTempMesh();
		//Mesh& tmp2 = getTempMesh();
		dest.smooth(tmp); dest.Clear(); tmp.smooth(dest);//tmp.smooth(tmp2); tmp2.smooth(dest);
	}
	else
	{
		dest.sharpenEdges();
	}

	dest.ComputeNormals();
}
