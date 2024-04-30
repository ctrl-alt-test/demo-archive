#ifndef SHARK_HH
#define SHARK_HH

#include "engine/runtime/RuntimeCompiler.hh"
#include "engine/mesh/IMeshBuilder.hh"

namespace mesh
{
	struct Mesh;
	struct Pool;
}

#ifdef ENABLE_RUNTIME_COMPILATION
struct Shark : public TInterface<runtime::IID_IMeshBuilder, mesh::IMeshBuilder>
{
	void			Build(mesh::Mesh& dest, mesh::Pool& pool);

	static ObjectId	objectId;
	static Shark*	instance;
	static void		BuildMesh(mesh::Mesh& dest, mesh::Pool& pool);
};
#else // !ENABLE_RUNTIME_COMPILATION
void				Shark(mesh::Mesh& dest, mesh::Pool& pool);
#endif // !ENABLE_RUNTIME_COMPILATION

#endif // SHARK_HH
