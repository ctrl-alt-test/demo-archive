#ifndef PARTHENON_HH
#define PARTHENON_HH

#include "engine/runtime/RuntimeCompiler.hh"
#include "engine/mesh/IMeshBuilder.hh"

namespace mesh
{
	struct Mesh;
	struct Pool;
}

#ifdef ENABLE_RUNTIME_COMPILATION
struct Parthenon : public TInterface<runtime::IID_IMeshBuilder, mesh::IMeshBuilder>
{
	void					Build(mesh::Mesh& dest, mesh::Pool& pool);

	static ObjectId			objectId;
	static Parthenon*		instance;
	static void				BuildMesh(mesh::Mesh& dest, mesh::Pool& pool);
};
#else // !ENABLE_RUNTIME_COMPILATION
void						Parthenon(mesh::Mesh& dest, mesh::Pool& pool);
#endif // !ENABLE_RUNTIME_COMPILATION

#ifdef ENABLE_RUNTIME_COMPILATION
struct ParthenonColumns : public TInterface<runtime::IID_IMeshBuilder, mesh::IMeshBuilder>
{
	void					Build(mesh::Mesh& dest, mesh::Pool& pool);

	static ObjectId			objectId;
	static ParthenonColumns*instance;
	static void				BuildMesh(mesh::Mesh& dest, mesh::Pool& pool);
};
#else // !ENABLE_RUNTIME_COMPILATION
void						ParthenonColumns(mesh::Mesh& dest, mesh::Pool& pool);
#endif // !ENABLE_RUNTIME_COMPILATION

#endif // GREEK_COLUMN_HH
