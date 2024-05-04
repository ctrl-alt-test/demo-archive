#pragma once

#include "engine/runtime/RuntimeCompiler.hh"
#include "engine/mesh/IMeshBuilder.hh"

namespace mesh
{
	struct Mesh;
	struct Pool;
}

#ifdef ENABLE_RUNTIME_COMPILATION
struct Lightning : public TInterface<runtime::IID_IMeshBuilder, mesh::IMeshBuilder>
{
	void				Build(mesh::Mesh& dest, mesh::Pool& pool);

	static ObjectId		objectId;
	static Lightning*	instance;
	static void			BuildMesh(mesh::Mesh& dest, mesh::Pool& pool);
};
#else // !ENABLE_RUNTIME_COMPILATION
void					Lightning(mesh::Mesh& dest, mesh::Pool& pool);
#endif // !ENABLE_RUNTIME_COMPILATION

#ifdef ENABLE_RUNTIME_COMPILATION
struct SingleLightning : public TInterface<runtime::IID_IMeshBuilder, mesh::IMeshBuilder>
{
	void				Build(mesh::Mesh& dest, mesh::Pool& pool);

	static ObjectId		objectId;
	static SingleLightning*	instance;
	static void			BuildMesh(mesh::Mesh& dest, mesh::Pool& pool);
};
#else // !ENABLE_RUNTIME_COMPILATION
void					SingleLightning(mesh::Mesh& dest, mesh::Pool& pool);
#endif // !ENABLE_RUNTIME_COMPILATION
