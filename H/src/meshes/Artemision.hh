#ifndef ARTEMISION_HH
#define ARTEMISION_HH

#include "engine/runtime/RuntimeCompiler.hh"
#include "engine/mesh/IMeshBuilder.hh"

namespace mesh
{
	struct Mesh;
	struct Pool;
}

#ifdef ENABLE_RUNTIME_COMPILATION
struct Artemision : public TInterface<runtime::IID_IMeshBuilder, mesh::IMeshBuilder>
{
	void						Build(mesh::Mesh& dest, mesh::Pool& pool);

	static ObjectId				objectId;
	static Artemision*			instance;
	static void					BuildMesh(mesh::Mesh& dest, mesh::Pool& pool);
};
#else // !ENABLE_RUNTIME_COMPILATION
void							Artemision(mesh::Mesh& dest, mesh::Pool& pool);
#endif // !ENABLE_RUNTIME_COMPILATION

#ifdef ENABLE_RUNTIME_COMPILATION
struct ArtemisionColumns : public TInterface<runtime::IID_IMeshBuilder, mesh::IMeshBuilder>
{
	void						Build(mesh::Mesh& dest, mesh::Pool& pool);

	static ObjectId				objectId;
	static ArtemisionColumns*	instance;
	static void					BuildMesh(mesh::Mesh& dest, mesh::Pool& pool);
};
#else // !ENABLE_RUNTIME_COMPILATION
void							ArtemisionColumns(mesh::Mesh& dest, mesh::Pool& pool);
#endif // !ENABLE_RUNTIME_COMPILATION

#ifdef ENABLE_RUNTIME_COMPILATION
struct ArtemisionFloor : public TInterface<runtime::IID_IMeshBuilder, mesh::IMeshBuilder>
{
	void						Build(mesh::Mesh& dest, mesh::Pool& pool);

	static ObjectId				objectId;
	static ArtemisionFloor*		instance;
	static void					BuildMesh(mesh::Mesh& dest, mesh::Pool& pool);
};
#else // !ENABLE_RUNTIME_COMPILATION
void							ArtemisionFloor(mesh::Mesh& dest, mesh::Pool& pool);
#endif // !ENABLE_RUNTIME_COMPILATION

#ifdef ENABLE_RUNTIME_COMPILATION
struct ArtemisionGreetingsFloor : public TInterface<runtime::IID_IMeshBuilder, mesh::IMeshBuilder>
{
	void						Build(mesh::Mesh& dest, mesh::Pool& pool);

	static ObjectId				objectId;
	static ArtemisionGreetingsFloor* instance;
	static void					BuildMesh(mesh::Mesh& dest, mesh::Pool& pool);
};
#else // !ENABLE_RUNTIME_COMPILATION
void							ArtemisionGreetingsFloor(mesh::Mesh& dest, mesh::Pool& pool);
#endif // !ENABLE_RUNTIME_COMPILATION

#endif // ARTEMISION_HH
