#pragma once

#include "TheSubmersible.hh"
#include "engine/core/Light.hh"
#include "engine/timeline/Timeline.hh"
#include "gfx/DrawArea.hh"
#include "gfx/ShadingParameters.hh"
#include "meshes/Plant.hh"
#include "tool/CommonMaterial.hh"
#include "tool/FixedLightToolBox.hh"
#include "tool/ParticipatingMedium.hh"
#include "tool/ParticleField.hh"
#include "tool/ShaderHelper.hh"
#include "tool/TextureLoader.hh"

namespace Core
{
	class Settings;
}

namespace Gfx
{
	class IGraphicLayer;
	struct Geometry;
}

namespace Tool
{
	class MeshLoader;
	class RenderList;
	class ShaderLoader;
	class VolumetricLighting;
	struct FixedLightToolBox;
}

class RuinsScene : public Timeline::IScene
{
public:
	RuinsScene():
		m_gfxLayer(nullptr),
		m_rocks(nullptr),
		m_column(nullptr),
		m_floor(nullptr)
	{}

	void Init(Gfx::IGraphicLayer* gfxLayer,
			  Tool::RenderList* renderList,
			  const Core::Settings& settings,
			  Tool::MeshLoader* meshLoader,
			  Tool::ShaderLoader* shaderLoader,
			  Tool::TextureLoader* textureLoader,
			  const Gfx::TextureID& depthTexture,
			  Tool::FixedLightToolBox* lightToolBox,
			  Tool::VolumetricLighting* m_volumetricLighting,
			  const Gfx::Geometry& quad);

	void Draw(long currentStoryTime,
			  const Core::CameraDescription& cameraDesc,
			  const Gfx::DrawArea& drawArea) override;

private:
	void CreateScene(long currentStoryTime);

	Gfx::IGraphicLayer*		m_gfxLayer;
	Tool::RenderList*		m_renderList;

	Gfx::TextureID			m_depthTexture;
	Gfx::ShadingParameters	m_shading;

	Tool::FixedLightRig		m_lightRig;
	Tool::FixedLightToolBox* m_lightToolBox;
	Tool::ParticipatingMedium m_medium;
	Tool::VolumetricLighting* m_volumetricLighting;
	Tool::ParticleField		m_floatingParticles;
	Gfx::TextureID*			m_randomTexture;

	Render::MeshGeometry*	m_rocks;
	Render::MeshGeometry*	m_column;
	Render::MeshGeometry*	m_floor;
	Render::MeshGeometry*	m_plantDensePatch;
	Render::MeshGeometry*	m_plantSparsePatch;
	Tool::CommonMaterial	m_seaweedMaterial;
	Tool::CommonMaterial	m_groundMaterial;
	TheSubmersible			m_submersible;

	Render::MeshGeometry*	m_road;
	Tool::CommonMaterial	m_roadMaterial;
};
