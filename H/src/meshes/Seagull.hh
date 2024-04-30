#pragma once

#include "engine/runtime/RuntimeCompiler.hh"
#include "engine/mesh/IMeshBuilder.hh"

namespace mesh
{
	struct Mesh;
	struct Pool;
}

#ifdef ENABLE_RUNTIME_COMPILATION
struct Seagull : public TInterface<runtime::IID_IMeshBuilder, mesh::IMeshBuilder>
{
	void			Build(mesh::Mesh& dest, mesh::Pool& pool);

	static ObjectId	objectId;
	static Seagull*	instance;
	static void		BuildMesh(mesh::Mesh& dest, mesh::Pool& pool);
};
#else // !ENABLE_RUNTIME_COMPILATION
void				Seagull(mesh::Mesh& dest, mesh::Pool& pool);
#endif // !ENABLE_RUNTIME_COMPILATION
