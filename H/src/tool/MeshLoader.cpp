#include "MeshLoader.hh"

#include "engine/container/Array.hxx"
#include "engine/container/Utils.hh"
#include "engine/core/Debug.hh"
#include "engine/core/FileIO.hh"
#include "engine/core/StringUtils.hh"
#include "engine/mesh/Mesh.hh"
#include "engine/mesh/MeshPool.hh"
#include "engine/mesh/Utils.hh"
#include "engine/noise/Hash.hh"
#include "engine/runtime/RuntimeCompiler.hh"
#include "gfx/IGraphicLayerImplementations.hh"
#include "tool/mesh/MeshHelper.hh"
#include "tool/mesh/VertexDataP2dT.hh"
#include "tool/mesh/VertexDataPNT.hh"
#include "tool/mesh/VertexDataPNTT.hh"

#if MESH_EDIT
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>
#endif

#define ENABLE_SAVE_MESHES_TO_DISK 0

using namespace Tool;

#define MAX_BUILT_MESHES 1024

namespace LoadingBar { void update(void* = nullptr); }

#if MESH_EDIT
void ExportMeshNames(const char* fileName, const char* macroName, const Container::Array<RecompilableMeshBuilder>& meshBuilders)
{
	FILE* fp = fopen(fileName, "w");
	if (fp == NULL)
	{
		LOG_ERROR("Cannot open '%s', meshes ids won't be exported.", fileName);
	}
	else
	{
		fprintf(fp, "// File generated\n// by %s\n// in %s@l.%d\n\n#ifndef %s\n#define %s\n\n", __FUNCTION__, __FILE__, __LINE__, macroName, macroName);
		fprintf(fp, "enum MeshName {\n");
		for (int i = 0; i < meshBuilders.size; ++i)
		{
			const RecompilableMeshBuilder& builder = meshBuilders[i];
			if (builder.exportInfo.builderName != nullptr &&
				builder.exportInfo.vertexFormatName != nullptr)
			{
				std::string name = builder.exportInfo.builderName +
					std::string("_") + builder.exportInfo.vertexFormatName;
				Core::ReplaceAll(name, " ", "");
				Core::ReplaceAll(name, "::", "_");
				fprintf(fp, "	%s,\n", name.c_str());
			}
		}
		fprintf(fp, "\n");

		fprintf(fp, "	NumberOfMeshes,\n");
		fprintf(fp, "};\n\n#endif // %s\n", macroName);
		fclose(fp);
	}
}

void ExportMeshBuilders(const char* fileName, const char* macroName, const Container::Array<RecompilableMeshBuilder>& meshBuilders)
{
	FILE* fp = fopen(fileName, "w");
	if (fp == NULL)
	{
		LOG_ERROR("Cannot open '%s', meshes definitions won't be exported.", fileName);
	}
	else
	{
		fprintf(fp, "// File generated\n// by %s\n// in %s@l.%d\n\n#ifndef %s\n#define %s\n\n", __FUNCTION__, __FILE__, __LINE__, macroName, macroName);

		fprintf(fp, "// Forward declarations:\n");
		std::set<std::string> vertexFormats;
		for (int i = 0; i < meshBuilders.size; ++i)
		{
			const RecompilableMeshBuilder& builder = meshBuilders[i];
			if (builder.exportInfo.builderName != nullptr &&
				builder.exportInfo.vertexFormatName != nullptr)
			{
				vertexFormats.insert(builder.exportInfo.vertexFormatName);
				fprintf(fp, "void %s(mesh::Mesh& dest, mesh::Pool& pool);\n",
					builder.exportInfo.builderName);
			}
		}
		fprintf(fp, "\n");

		for (std::set<std::string>::const_iterator it = vertexFormats.begin(); it != vertexFormats.end(); ++it)
		{
			// We assume the vertex format to be in the namespace Tool.
			fprintf(fp, "namespace Tool { struct %s; }\n", (*it).c_str());
		}
		fprintf(fp, "\n");

		fprintf(fp, "::Render::MeshGeometry meshGeometries[%u];\n", meshBuilders.size);
		fprintf(fp, "RecompilableMeshBuilder meshBuilders[] = {\n");
		for (int i = 0; i < meshBuilders.size; ++i)
		{
			const RecompilableMeshBuilder& builder = meshBuilders[i];
			if (builder.exportInfo.builderName != nullptr &&
				builder.exportInfo.vertexFormatName != nullptr)
			{
				fprintf(fp, "	{ %s,		%s::s_descriptor },\n",
					builder.exportInfo.builderName,
					builder.exportInfo.vertexFormatName);
			}
		}
		fprintf(fp, "};\n\n#endif // %s\n", macroName);
		fclose(fp);
	}
}
#endif // MESH_EDIT

MeshLoader::MeshLoader(Gfx::IGraphicLayer* gfxLayer,
					   runtime::RuntimeCompiler* runtimeCompiler):
#if MESH_EDIT
	m_gfxLayer(gfxLayer),
#ifdef ENABLE_RUNTIME_COMPILATION
	m_runtimeCompiler(runtimeCompiler),
#endif // ENABLE_RUNTIME_COMPILATION
	m_meshBuilders(MAX_BUILT_MESHES),
	m_meshGeometries(MAX_BUILT_MESHES)
#else // !MESH_EDIT
	m_gfxLayer(gfxLayer)
#endif // !MESH_EDIT
{
	m_meshPool.Init();

#if MESH_EDIT && defined(ENABLE_RUNTIME_COMPILATION)
	runtimeCompiler->AddListener(this);
#else // !MESH_EDIT || !ENABLE_RUNTIME_COMPILATION
	UNUSED_EXPR(runtimeCompiler);
#endif // !MESH_EDIT || !ENABLE_RUNTIME_COMPILATION
}

MeshLoader::~MeshLoader()
{
#if MESH_EDIT
	ExportMeshNames("src/" CONCATENATE_PATH(PROJECT_DIRECTORY,exported_meshNames.hh), "EXPORTED_MESH_IDS", m_meshBuilders);
	ExportMeshBuilders("src/" CONCATENATE_PATH(PROJECT_DIRECTORY,exported_meshBuilders.hh), "EXPORTED_MESH_BUILDERS", m_meshBuilders);
#endif // MESH_EDIT
}

#if MESH_EDIT

#ifdef ENABLE_RUNTIME_COMPILATION
void MeshLoader::OnConstructorsAdded()
{
	Rebuild();
}

void MeshLoader::Rebuild()
{
	for (int i = 0; i < m_meshBuilders.size; ++i)
	{
		RecompilableMeshBuilder& builder = m_meshBuilders[i];
		if (m_runtimeCompiler->UpdateCompiledCode(builder.compilationData))
		{
			BuildMesh(m_gfxLayer, m_meshPool, builder, m_meshGeometries[i]);
		}
	}
}
#endif // ENABLE_RUNTIME_COMPILATION

Render::MeshGeometry* MeshLoader::Load(RecompilableMeshBuilder& builder)
{
	// Search for this builder in the list of meshes already loaded.
	for (int i = 0; i < m_meshBuilders.size; ++i)
	{
		RecompilableMeshBuilder& test = m_meshBuilders[i];
		if (test.builder == builder.builder &&
			test.vertexFormatDescriptor == builder.vertexFormatDescriptor)
		{
			return &(m_meshGeometries[i]);
		}
	}

#ifdef ENABLE_RUNTIME_COMPILATION
	m_runtimeCompiler->LoadCompiledCode(builder.compilationData);
#endif // ENABLE_RUNTIME_COMPILATION

	Render::MeshGeometry meshGeometry;
	meshGeometry.geometry.vertexBuffer = m_gfxLayer->CreateVertexBuffer();
#if GFX_ENABLE_VERTEX_BUFFER_OFFSET
	meshGeometry.geometry.firstIndexOffset = 0;
#endif // GFX_ENABLE_VERTEX_BUFFER_OFFSET
	m_meshBuilders.add(builder);
	m_meshGeometries.add(meshGeometry);

	BuildMesh(m_gfxLayer, m_meshPool, m_meshBuilders.last(), m_meshGeometries.last());
	LoadingBar::update();

	return &m_meshGeometries.last();
}

#else // !MESH_EDIT

#include CONCATENATE_PATH(PROJECT_DIRECTORY,exported_meshBuilders.hh)

// FIXME: when MESH_EDIT is not defined, this function must be called
// before using the meshes. Otherwise the program might crash on the
// first drawcall with little explanation of why. Ideally, we'd want
// LOAD_MESH to report the error explicitely.
void MeshLoader::LoadMeshes()
{
	for (int i = 0; LoadMeshesIncrementally(i); ++i)
	{
	}
}

bool MeshLoader::LoadMeshesIncrementally(int i)
{
	if (i >= ARRAY_LEN(meshBuilders))
	{
		return false;
	}
	RecompilableMeshBuilder& builder = meshBuilders[i];
	Render::MeshGeometry& meshGeometry = meshGeometries[i];
	meshGeometry.geometry.vertexBuffer = m_gfxLayer->CreateVertexBuffer();
#if GFX_ENABLE_VERTEX_BUFFER_OFFSET
	meshGeometry.geometry.firstIndexOffset = 0;
#endif // GFX_ENABLE_VERTEX_BUFFER_OFFSET
	BuildMesh(m_gfxLayer, m_meshPool, builder, meshGeometry);
	return true;
}

Render::MeshGeometry* MeshLoader::GetMesh(const MeshName& name) const
{
	return &(meshGeometries[name]);
}

#endif // !MESH_EDIT

void MeshLoader::BuildMesh(Gfx::IGraphicLayer* gfxLayer,
						   mesh::Pool& meshPool,
						   RecompilableMeshBuilder& meshBuilder,
						   Render::MeshGeometry& meshGeometry)
{
#if MESH_EDIT
	LOG_INFO("Building mesh %s...", meshBuilder.exportInfo.builderName);

#ifdef ENABLE_RUNTIME_COMPILATION
	assert(meshBuilder.compilationData.hasCodeChanged);
	meshBuilder.compilationData.hasCodeChanged = false;
#endif // ENABLE_RUNTIME_COMPILATION
#endif // MESH_EDIT

	// Allocating over 2 million vertices, which should REALLY be enough
	// for any use. Real use cases shouldn't go above 1 million.
	mesh::Mesh mesh(32*65536);

	meshPool.Clear();
	meshBuilder.builder(mesh, meshPool);
	mesh::ComputeBoundingVolume(mesh, &meshGeometry);
	if (mesh.shouldCleanUp)
	{
		mesh.CleanUp();
	}
	MeshHelper::LoadVertexBuffer(gfxLayer,
								 meshGeometry.geometry.vertexBuffer,
								 mesh,
								 meshBuilder.vertexFormatDescriptor,
								 &meshGeometry.geometry.numberOfIndices);

#if MESH_EDIT && ENABLE_SAVE_MESHES_TO_DISK
	std::string directoryPath = Core::GetFullPath("build/assets/meshes/");
	if (!Core::CreateDirectoryPath(directoryPath.c_str()))
	{
		LOG_ERROR("Could not create directory %s.", directoryPath.c_str());
	}
	else
	{
		std::string fileName = directoryPath + meshBuilder.builderName + std::string(".obj");
		mesh::ExportAsWavefront(mesh, fileName.c_str());
	}
#endif // MESH_EDIT && ENABLE_SAVE_MESHES_TO_DISK
}
