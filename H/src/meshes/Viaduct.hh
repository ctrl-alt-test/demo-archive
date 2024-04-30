#ifndef VIADUCT_HH
#define VIADUCT_HH

#include "engine/runtime/RuntimeCompiler.hh"
#include "engine/mesh/IMeshBuilder.hh"

namespace mesh
{
	struct Mesh;
	struct Pool;
}

#ifdef ENABLE_RUNTIME_COMPILATION
struct Viaduct : public TInterface<runtime::IID_IMeshBuilder, mesh::IMeshBuilder>
{
	void			Build(mesh::Mesh& dest, mesh::Pool& pool);

	static ObjectId	objectId;
	static Viaduct*	instance;
	static void		BuildMesh(mesh::Mesh& dest, mesh::Pool& pool);
};
#else // !ENABLE_RUNTIME_COMPILATION
void				Viaduct(mesh::Mesh& dest, mesh::Pool& pool);
#endif // !ENABLE_RUNTIME_COMPILATION

#endif // VIADUCT_HH
