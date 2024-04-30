#pragma once

#include "engine/Core/RigidTransform.hh"
#include "gfx/ShadingParameters.hh"
#include "tool/CommonMaterial.hh"

namespace Core
{
	class Settings;
}

namespace Noise
{
	class Rand;
}

namespace Render
{
	struct MeshGeometry;
}

namespace Tool
{
	class MeshLoader;
	class RenderList;
	class ShaderLoader;
	class TextureLoader;
}

struct TheCity
{
	void Init(const Core::Settings& settings,
			  Tool::MeshLoader* meshLoader,
			  Tool::ShaderLoader* shaderLoader,
			  Tool::TextureLoader* textureLoader);
	void AddToScene(Tool::RenderList* renderList,
					long currentTime,
					const Core::RigidTransform& root);
	void CreateCityBlock(Tool::RenderList* renderList,
						 const Core::RigidTransform& transform,
						 Noise::Rand& rand,
						 bool innerCircle,
						 bool forceParthenon);

	Gfx::TextureID			m_depthTexture;
	Gfx::ShadingParameters	m_shading;
	Gfx::TextureID*			m_randomTexture;

	Render::MeshGeometry*	m_artemision;
	Render::MeshGeometry*	m_artemisionColumns;
	Render::MeshGeometry*	m_fence;
	Render::MeshGeometry*	m_fountain;
	Render::MeshGeometry*	m_fountainStatues;
	Render::MeshGeometry*	m_houses;
	Render::MeshGeometry*	m_obelisk;
	Render::MeshGeometry*	m_parthenon;
	Render::MeshGeometry*	m_parthenonColumns;
	Render::MeshGeometry*	m_road;
	Render::MeshGeometry*	m_stairs;
	Render::MeshGeometry*	m_statue;
	Render::MeshGeometry*	m_tholos;
	Render::MeshGeometry*	m_trident;
	Render::MeshGeometry*	m_viaduct;

	Tool::CommonMaterial	m_doricColumnMaterial;
	Tool::CommonMaterial	m_floorMaterial;
	Tool::CommonMaterial	m_goldMaterial;
	Tool::CommonMaterial	m_ionicColumnMaterial;
	Tool::CommonMaterial	m_marbleWallMaterial;
	Tool::CommonMaterial	m_roadMaterial;
	Tool::CommonMaterial	m_stonesMaterial;
};
