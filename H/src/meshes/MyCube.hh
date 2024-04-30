#ifndef MY_CUBE_HH
#define MY_CUBE_HH

#include "engine/mesh/IMeshBuilder.hh"
#include "engine/runtime/RuntimeCompiler.hh"

namespace mesh
{
	struct Mesh;
	struct Pool;
}

#ifdef ENABLE_RUNTIME_COMPILATION
struct MyCube : public TInterface<runtime::IID_IMeshBuilder, mesh::IMeshBuilder>
{
	void	Build(mesh::Mesh& dest, mesh::Pool& pool);

	static ObjectId objectId;
	static MyCube* instance;
	static void BuildMesh(mesh::Mesh& dest, mesh::Pool& pool);
};
#else // !ENABLE_RUNTIME_COMPILATION
void MyCube(mesh::Mesh& dest, mesh::Pool& pool);
#endif // !ENABLE_RUNTIME_COMPILATION

#endif // MY_CUBE_MESH_HH
