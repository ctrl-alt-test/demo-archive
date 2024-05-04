#pragma once

#include "engine/mesh/IMeshBuilder.hh"
#include "engine/runtime/RuntimeCompiler.hh"

namespace mesh
{
	struct Mesh;
	struct Pool;
}

#ifdef ENABLE_RUNTIME_COMPILATION
struct Rocks : public TInterface<runtime::IID_IMeshBuilder, mesh::IMeshBuilder>
{
	void	Build(mesh::Mesh& dest, mesh::Pool& pool);

	static ObjectId objectId;
	static Rocks* instance;
	static void BuildMesh(mesh::Mesh& dest, mesh::Pool& pool);
};
#else // !ENABLE_RUNTIME_COMPILATION
void Rocks(mesh::Mesh& dest, mesh::Pool& pool);
#endif // !ENABLE_RUNTIME_COMPILATION
