#ifndef PLAYGROUND_MESH_HH
#define PLAYGROUND_MESH_HH

#include "engine/mesh/IMeshBuilder.hh"
#include "engine/runtime/RuntimeCompiler.hh"

namespace mesh
{
	struct Mesh;
	struct Pool;
}

#ifdef ENABLE_RUNTIME_COMPILATION
struct PlaygroundMesh : public TInterface<runtime::IID_IMeshBuilder, mesh::IMeshBuilder>
{
	void			Build(mesh::Mesh& dest, mesh::Pool& pool);

	static ObjectId objectId;
	static PlaygroundMesh* instance;
	static void BuildMesh(mesh::Mesh& dest, mesh::Pool& pool);
};
#else // !ENABLE_RUNTIME_COMPILATION
void				PlaygroundMesh(mesh::Mesh& dest, mesh::Pool& pool);
#endif // !ENABLE_RUNTIME_COMPILATION

#endif // PLAYGROUND_MESH_HH
