#ifndef SEA_SURFACE_HH
#define SEA_SURFACE_HH

#include "engine/runtime/RuntimeCompiler.hh"
#include "engine/mesh/IMeshBuilder.hh"

namespace mesh
{
	struct Mesh;
	struct Pool;
}

#ifdef ENABLE_RUNTIME_COMPILATION
struct SeaSurface : public TInterface<runtime::IID_IMeshBuilder, mesh::IMeshBuilder>
{
	void				Build(mesh::Mesh& dest, mesh::Pool& pool);

	static ObjectId		objectId;
	static SeaSurface*	instance;
	static void			BuildMesh(mesh::Mesh& dest, mesh::Pool& pool);
};
#else // !ENABLE_RUNTIME_COMPILATION
void					SeaSurface(mesh::Mesh& dest, mesh::Pool& pool);
#endif // !ENABLE_RUNTIME_COMPILATION

#ifdef ENABLE_RUNTIME_COMPILATION
struct TinySeaPatch : public TInterface<runtime::IID_IMeshBuilder, mesh::IMeshBuilder>
{
	void					Build(mesh::Mesh& dest, mesh::Pool& pool);

	static ObjectId			objectId;
	static TinySeaPatch*	instance;
	static void				BuildMesh(mesh::Mesh& dest, mesh::Pool& pool);
};
#else // !ENABLE_RUNTIME_COMPILATION
void						TinySeaPatch(mesh::Mesh& dest, mesh::Pool& pool);
#endif // !ENABLE_RUNTIME_COMPILATION

#ifdef ENABLE_RUNTIME_COMPILATION
struct LargeSeaPatch : public TInterface<runtime::IID_IMeshBuilder, mesh::IMeshBuilder>
{
	void					Build(mesh::Mesh& dest, mesh::Pool& pool);

	static ObjectId			objectId;
	static LargeSeaPatch*	instance;
	static void				BuildMesh(mesh::Mesh& dest, mesh::Pool& pool);
};
#else // !ENABLE_RUNTIME_COMPILATION
void						LargeSeaPatch(mesh::Mesh& dest, mesh::Pool& pool);
#endif // !ENABLE_RUNTIME_COMPILATION

#endif // SEA_SURFACE_HH
