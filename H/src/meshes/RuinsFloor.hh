#pragma once

#include "engine/mesh/IMeshBuilder.hh"
#include "engine/runtime/RuntimeCompiler.hh"

namespace mesh
{
	struct Mesh;
	struct Pool;
}

#ifdef ENABLE_RUNTIME_COMPILATION
struct RuinsFloor : public TInterface<runtime::IID_IMeshBuilder, mesh::IMeshBuilder>
{
	void					Build(mesh::Mesh& dest, mesh::Pool& pool);

	static ObjectId			objectId;
	static RuinsFloor*		instance;
	static void				BuildMesh(mesh::Mesh& dest, mesh::Pool& pool);
};
#else // !ENABLE_RUNTIME_COMPILATION
void RuinsFloor(mesh::Mesh& dest, mesh::Pool& pool);
#endif // !ENABLE_RUNTIME_COMPILATION
