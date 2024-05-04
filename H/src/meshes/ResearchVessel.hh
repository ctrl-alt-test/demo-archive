#ifndef RESEARCH_VESSEL_HH
#define RESEARCH_VESSEL_HH

#include "engine/runtime/RuntimeCompiler.hh"
#include "engine/mesh/IMeshBuilder.hh"

namespace mesh
{
	struct Mesh;
	struct Pool;
}

#ifdef ENABLE_RUNTIME_COMPILATION
struct ResearchVesselHull : public TInterface<runtime::IID_IMeshBuilder, mesh::IMeshBuilder>
{
	void							Build(mesh::Mesh& dest, mesh::Pool& pool);

	static ObjectId					objectId;
	static ResearchVesselHull*		instance;
	static void						BuildMesh(mesh::Mesh& dest, mesh::Pool& pool);
};
#else // !ENABLE_RUNTIME_COMPILATION
void								ResearchVesselHull(mesh::Mesh& dest, mesh::Pool& pool);
#endif // !ENABLE_RUNTIME_COMPILATION

#ifdef ENABLE_RUNTIME_COMPILATION
struct ResearchVesselBridgeWindowPart : public TInterface<runtime::IID_IMeshBuilder, mesh::IMeshBuilder>
{
	void							Build(mesh::Mesh& dest, mesh::Pool& pool);

	static ObjectId					objectId;
	static ResearchVesselBridgeWindowPart*	instance;
	static void						BuildMesh(mesh::Mesh& dest, mesh::Pool& pool);
};
#else // !ENABLE_RUNTIME_COMPILATION
void								ResearchVesselBridgeWindowPart(mesh::Mesh& dest, mesh::Pool& pool);
#endif // !ENABLE_RUNTIME_COMPILATION

#ifdef ENABLE_RUNTIME_COMPILATION
struct ResearchVesselBridgeNoWindowPart : public TInterface<runtime::IID_IMeshBuilder, mesh::IMeshBuilder>
{
	void							Build(mesh::Mesh& dest, mesh::Pool& pool);

	static ObjectId					objectId;
	static ResearchVesselBridgeNoWindowPart*	instance;
	static void						BuildMesh(mesh::Mesh& dest, mesh::Pool& pool);
};
#else // !ENABLE_RUNTIME_COMPILATION
void								ResearchVesselBridgeNoWindowPart(mesh::Mesh& dest, mesh::Pool& pool);
#endif // !ENABLE_RUNTIME_COMPILATION

#ifdef ENABLE_RUNTIME_COMPILATION
struct ResearchVesselCrane : public TInterface<runtime::IID_IMeshBuilder, mesh::IMeshBuilder>
{
	void							Build(mesh::Mesh& dest, mesh::Pool& pool);

	static ObjectId					objectId;
	static ResearchVesselCrane*		instance;
	static void						BuildMesh(mesh::Mesh& dest, mesh::Pool& pool);
};
#else // !ENABLE_RUNTIME_COMPILATION
void								ResearchVesselCrane(mesh::Mesh& dest, mesh::Pool& pool);
#endif // !ENABLE_RUNTIME_COMPILATION

#endif // RESEARCH_VESSEL_HH
