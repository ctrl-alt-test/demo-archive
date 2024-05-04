#include "MyCube.hh"

#include "engine/algebra/Matrix.hxx"
#include "engine/mesh/Cube.hh"
#include "engine/mesh/Mesh.hh"
#include "engine/mesh/MeshPool.hh"
#include "tool/mesh/VertexDataPNT.hh"

#ifdef ENABLE_RUNTIME_COMPILATION
#include "RuntimeCompiledCPlusPlus/RuntimeObjectSystem/ObjectInterfacePerModule.h"

REGISTERCLASS(MyCube);
ObjectId MyCube::objectId;
MyCube* MyCube::instance = NULL;
void MyCube::BuildMesh(mesh::Mesh& dest, mesh::Pool& pool)
{
	return instance->Build(dest, pool);
}
#endif

using namespace Algebra;
using namespace mesh;

#ifdef ENABLE_RUNTIME_COMPILATION
void MyCube::Build(Mesh& dest, Pool& pool)
#else // !ENABLE_RUNTIME_COMPILATION
void MyCube(Mesh& dest, Pool& pool)
#endif // !ENABLE_RUNTIME_COMPILATION
{
	bool smooth = true;

	dest.Clear();
	sharpCube(dest).Scale(5, 5, 5);
	dest.splitAllFaces(4);
	dest.unsharpenEdges(1.f);

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
