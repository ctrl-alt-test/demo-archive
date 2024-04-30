#pragma once

#include "engine/runtime/RuntimeCompiler.hh"
#include "engine/mesh/IMeshBuilder.hh"

namespace mesh
{
	struct Mesh;
	struct Pool;

	float StairsPlatform(mesh::Mesh& dest, mesh::Pool& pool,
						 int steps, float stepHeight, float stepWidth,
						 float width, float length);
}

#ifdef ENABLE_RUNTIME_COMPILATION
struct Stairs : public TInterface<runtime::IID_IMeshBuilder, mesh::IMeshBuilder>
{
	void				Build(mesh::Mesh& dest, mesh::Pool& pool);

	static ObjectId		objectId;
	static Stairs*		instance;
	static void			BuildMesh(mesh::Mesh& dest, mesh::Pool& pool);
};
#else // !ENABLE_RUNTIME_COMPILATION
void					Stairs(mesh::Mesh& dest, mesh::Pool& pool);
#endif // !ENABLE_RUNTIME_COMPILATION
