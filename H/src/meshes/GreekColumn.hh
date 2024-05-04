#ifndef GREEK_COLUMN_HH
#define GREEK_COLUMN_HH

#include "engine/runtime/RuntimeCompiler.hh"
#include "engine/mesh/IMeshBuilder.hh"

namespace Noise
{
	class Rand;
}

namespace mesh
{
	struct Mesh;
	struct Pool;

	void CubicColumn(Mesh& dest, Pool& pool);
	void DoricColumn(mesh::Mesh& dest, mesh::Pool& pool, int axialResolution, int capitaResolution);
	void IonicColumn(mesh::Mesh& dest, mesh::Pool& pool, int axialResolution, int baseResolution, int capitaResolution);
	void RandomizeIonicColumnTextureOffset(mesh::Mesh& dest, Noise::Rand& rand);
}

#ifdef ENABLE_RUNTIME_COMPILATION
struct GreekColumn : public TInterface<runtime::IID_IMeshBuilder, mesh::IMeshBuilder>
{
	void				Build(mesh::Mesh& dest, mesh::Pool& pool);

	static ObjectId		objectId;
	static GreekColumn*	instance;
	static void			BuildMesh(mesh::Mesh& dest, mesh::Pool& pool);
};
#else // !ENABLE_RUNTIME_COMPILATION
void				GreekColumn(mesh::Mesh& dest, mesh::Pool& pool);
#endif // !ENABLE_RUNTIME_COMPILATION

#endif // GREEK_COLUMN_HH
