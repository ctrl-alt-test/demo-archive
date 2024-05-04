//
// This is not part of the engine because it will generate code
// specific to each demo (include of generated list of meshes).
//

#ifndef MESH_LOADER_HH
#define MESH_LOADER_HH

#include "engine/algebra/AABB3.hh"
#include "engine/mesh/MeshPool.hh"
#include "engine/render/MeshGeometry.hh"
#include "engine/runtime/RuntimeCompiler.hh"
#include "tool/IncludeUtils.hh"
#include "tool/mesh/MeshHelper.hh"

#if DEBUG
# define MESH_EDIT 1
#else // !DEBUG
# define MESH_EDIT 0
#endif // !DEBUG

namespace Gfx
{
	class IGraphicLayer;
}

namespace Tool
{
/// <summary>
/// LOAD_MESH stores in dest the Gfx::Geometry of the mesh defined by
/// the builder and vertex format passed as parameters. The vertex
/// format is assumed to be defined in the Tool namespace.
/// In Debug it builds the mesh;
/// in release it only gets the mesh from a list of meshes assumed to
/// have been built already.
/// </summary>
#define LOAD_MESH(loader, dest, name, vertexFormat) LOAD_MESH_IMPLEM(loader, dest, name, vertexFormat)

	struct MeshBuilderExport
	{
		const char*					builderName;
		const char*					vertexFormatName;
		const char*					loadLocation;
	};

	struct RecompilableMeshBuilder
	{
		void						(*builder)(mesh::Mesh& dest, mesh::Pool& pool);
		Tool::VertexFormatDescriptor vertexFormatDescriptor;

#if MESH_EDIT
		MeshBuilderExport			exportInfo;

#ifdef ENABLE_RUNTIME_COMPILATION
		runtime::CompilationData	compilationData;
#endif // !ENABLE_RUNTIME_COMPILATION
#endif // MESH_EDIT
	};

#if MESH_EDIT
# define LOAD_MESH_IMPLEM(loader, dest, name, vertexFormat)		do { Tool::RecompilableMeshBuilder builder = RECOMPILABLE_MESH(name, vertexFormat); (dest) = (loader).Load(builder); } while (false)
#else // !MESH_EDIT
# include CONCATENATE_PATH(PROJECT_DIRECTORY,exported_meshNames.hh)
# define LOAD_MESH_IMPLEM(loader, dest, name, vertexFormat)		do { (dest) = (loader).GetMesh(::name##_##vertexFormat); } while (false)
#endif // !MESH_EDIT

#if MESH_EDIT
#ifdef ENABLE_RUNTIME_COMPILATION
#define RECOMPILABLE_MESH(name, vertexFormat)	{ name::BuildMesh, Tool::vertexFormat::s_descriptor, { QUOTEME(name), QUOTEME(vertexFormat), LOCATION_IN_SOURCE }, { QUOTEME(name), name::s_interfaceID, (void**)&name::instance, ObjectId(), false } }
#else
#define RECOMPILABLE_MESH(name, vertexFormat)	{ name, ::Tool::vertexFormat::s_descriptor, { QUOTEME(name), QUOTEME(vertexFormat), LOCATION_IN_SOURCE } }
#endif
#else // !MESH_EDIT
#define RECOMPILABLE_MESH(name, vertexFormat)	{ name, ::Tool::vertexFormat::s_descriptor }
#endif // !MESH_EDIT

#ifdef ENABLE_RUNTIME_COMPILATION
	class MeshLoader : public IObjectFactoryListener
#else // !ENABLE_RUNTIME_COMPILATION
	class MeshLoader
#endif // !ENABLE_RUNTIME_COMPILATION
	{
	public:
		MeshLoader(Gfx::IGraphicLayer* gfxLayer,
				   runtime::RuntimeCompiler* runtimeCompiler);
		~MeshLoader();

#if MESH_EDIT
		Render::MeshGeometry* Load(RecompilableMeshBuilder& builder);
#else // !MESH_EDIT

		/// <summary>
		/// Loads and compiles all the meshes.
		/// </summary>
		void LoadMeshes();

		/// <summary>
		/// Loads and compiles the meshes, one at a time.
		/// </summary>
		///
		/// <returns>True if there are meshes left to compile, false
		/// otherwise.</returns>
		bool LoadMeshesIncrementally(int i);

		Render::MeshGeometry* GetMesh(const MeshName& name) const;
#endif // !MESH_EDIT

#ifdef ENABLE_RUNTIME_COMPILATION
		void OnConstructorsAdded();
		void Rebuild();
#endif // ENABLE_RUNTIME_COMPILATION

	private:
		static void BuildMesh(Gfx::IGraphicLayer* gfxLayer,
							  mesh::Pool& meshPool,
							  RecompilableMeshBuilder& meshBuilder,
							  Render::MeshGeometry& meshGeometry);

		Gfx::IGraphicLayer*			m_gfxLayer;
		mesh::Pool					m_meshPool;

#if MESH_EDIT
#ifdef ENABLE_RUNTIME_COMPILATION
		runtime::RuntimeCompiler*	m_runtimeCompiler;
#endif // ENABLE_RUNTIME_COMPILATION

		Container::Array<RecompilableMeshBuilder>	m_meshBuilders;
		Container::Array<Render::MeshGeometry>		m_meshGeometries;
#endif // MESH_EDIT
	};
}

#endif // MESH_LOADER_HH
