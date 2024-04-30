#ifndef DOLPHIN_HH
#define DOLPHIN_HH

#include "engine/runtime/RuntimeCompiler.hh"
#include "engine/mesh/IMeshBuilder.hh"

namespace mesh
{
	struct Mesh;
	struct Pool;
}

#ifdef ENABLE_RUNTIME_COMPILATION
struct Dolphin : public TInterface<runtime::IID_IMeshBuilder, mesh::IMeshBuilder>
{
	void			Build(mesh::Mesh& dest, mesh::Pool& pool);

	static ObjectId	objectId;
	static Dolphin*	instance;
	static void		BuildMesh(mesh::Mesh& dest, mesh::Pool& pool);
};
#else // !ENABLE_RUNTIME_COMPILATION
void				Dolphin(mesh::Mesh& dest, mesh::Pool& pool);
#endif // !ENABLE_RUNTIME_COMPILATION

#endif // DOLPHIN_HH
