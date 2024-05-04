#pragma once

#include "engine/runtime/RuntimeCompiler.hh"
#include "engine/mesh/IMeshBuilder.hh"

namespace mesh
{
	struct Mesh;
	struct Pool;
}

#ifdef ENABLE_RUNTIME_COMPILATION
struct PlantDensePatch : public TInterface<runtime::IID_IMeshBuilder, mesh::IMeshBuilder>
{
	void					Build(mesh::Mesh& dest, mesh::Pool& pool);

	static ObjectId			objectId;
	static PlantDensePatch*	instance;
	static void				BuildMesh(mesh::Mesh& dest, mesh::Pool& pool);
};
#else // !ENABLE_RUNTIME_COMPILATION
void						PlantDensePatch(mesh::Mesh& dest, mesh::Pool& pool);
#endif // !ENABLE_RUNTIME_COMPILATION

#ifdef ENABLE_RUNTIME_COMPILATION
struct PlantSparsePatch : public TInterface<runtime::IID_IMeshBuilder, mesh::IMeshBuilder>
{
	void					Build(mesh::Mesh& dest, mesh::Pool& pool);

	static ObjectId			objectId;
	static PlantSparsePatch*instance;
	static void				BuildMesh(mesh::Mesh& dest, mesh::Pool& pool);
};
#else // !ENABLE_RUNTIME_COMPILATION
void						PlantSparsePatch(mesh::Mesh& dest, mesh::Pool& pool);
#endif // !ENABLE_RUNTIME_COMPILATION
