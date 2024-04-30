#ifndef SUBMERSIBLE_HH
#define SUBMERSIBLE_HH

#include "engine/runtime/RuntimeCompiler.hh"
#include "engine/mesh/IMeshBuilder.hh"

namespace Core
{
	struct RigidTransform;
}

namespace mesh
{
	struct Mesh;
	struct Pool;
}

#ifdef ENABLE_RUNTIME_COMPILATION
struct Submersible : public TInterface<runtime::IID_IMeshBuilder, mesh::IMeshBuilder>
{
	void					Build(mesh::Mesh& dest, mesh::Pool& pool);

	static ObjectId			objectId;
	static Submersible*		instance;
	static void				BuildMesh(mesh::Mesh& dest, mesh::Pool& pool);
};
#else // !ENABLE_RUNTIME_COMPILATION
void						Submersible(mesh::Mesh& dest, mesh::Pool& pool);
#endif // !ENABLE_RUNTIME_COMPILATION

#ifdef ENABLE_RUNTIME_COMPILATION
struct SubmersibleFrame : public TInterface<runtime::IID_IMeshBuilder, mesh::IMeshBuilder>
{
	void					Build(mesh::Mesh& dest, mesh::Pool& pool);

	static ObjectId			objectId;
	static SubmersibleFrame*instance;
	static void				BuildMesh(mesh::Mesh& dest, mesh::Pool& pool);
};
#else // !ENABLE_RUNTIME_COMPILATION
void						SubmersibleFrame(mesh::Mesh& dest, mesh::Pool& pool);
#endif // !ENABLE_RUNTIME_COMPILATION

#ifdef ENABLE_RUNTIME_COMPILATION
struct SubmersibleSpots : public TInterface<runtime::IID_IMeshBuilder, mesh::IMeshBuilder>
{
	void					Build(mesh::Mesh& dest, mesh::Pool& pool);

	static ObjectId			objectId;
	static SubmersibleSpots*instance;
	static void				BuildMesh(mesh::Mesh& dest, mesh::Pool& pool);
};
#else // !ENABLE_RUNTIME_COMPILATION
void						SubmersibleSpots(mesh::Mesh& dest, mesh::Pool& pool);
#endif // !ENABLE_RUNTIME_COMPILATION

#endif // SUBMERSIBLE_HH
