#ifndef SDF_TEST_HH
#define SDF_TEST_HH

#include "engine/runtime/RuntimeCompiler.hh"
#include "engine/mesh/IMeshBuilder.hh"

namespace mesh
{
	struct Mesh;
	struct Pool;
}

#ifdef ENABLE_RUNTIME_COMPILATION
struct SdfTest : public TInterface<runtime::IID_IMeshBuilder, mesh::IMeshBuilder>
{
	void				Build(mesh::Mesh& dest, mesh::Pool& pool);

	static ObjectId		objectId;
	static SdfTest*		instance;
	static void			BuildMesh(mesh::Mesh& dest, mesh::Pool& pool);
};
#else // !ENABLE_RUNTIME_COMPILATION
void					SdfTest(mesh::Mesh& dest, mesh::Pool& pool);
#endif // !ENABLE_RUNTIME_COMPILATION

#endif // SDF_TEST_HH
