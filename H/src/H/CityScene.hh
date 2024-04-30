#pragma once

#include "TheCity.hh"
#include "TheSubmersible.hh"
#include "engine/core/Light.hh"
#include "engine/noise/Rand.hh"
#include "engine/timeline/Timeline.hh"
#include "gfx/DrawArea.hh"
#include "gfx/ShadingParameters.hh"
#include "tool/CommonMaterial.hh"
#include "tool/FixedLightToolBox.hh"
#include "tool/ParticipatingMedium.hh"
#include "tool/ParticleField.hh"
#include "tool/ShaderHelper.hh"

#define ENABLE_CITY_PARTICLES 1 // SIZE: The option costs 40 to 60 bytes on the compressed binary.
#define ENABLE_LIGHTNINGS 0 // SIZE: The option costs 190 bytes on the compressed binary.

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
	class TextureLoader;
	class VolumetricLighting;
}

class CityScene : public Timeline::IScene
{
public:
	CityScene():
		m_gfxLayer(nullptr),
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
	void CreateCityBlock(float angle,
						 float distanceFromCenter,
						 bool innerCircle,
						 Noise::Rand& rand);
	void CreateScene(long currentStoryTime);

	Gfx::IGraphicLayer*		m_gfxLayer;
	Tool::RenderList*		m_renderList;

	Gfx::TextureID			m_depthTexture;
	Gfx::ShadingParameters	m_shading;

	Tool::FixedLightRig		m_lightRig;
	Tool::FixedLightToolBox* m_lightToolBox;
	Tool::ParticipatingMedium m_medium;
	Tool::VolumetricLighting* m_volumetricLighting;
#if ENABLE_CITY_PARTICLES
	Tool::ParticleField		m_floatingParticles;
#endif // ENABLE_CITY_PARTICLES

	Render::MeshGeometry*	m_floor;
	Render::MeshGeometry*	m_islandFloor;

	Tool::CommonMaterial	m_floorMaterial;

#if ENABLE_LIGHTNINGS
	Render::MeshGeometry*	m_lightning;
	Render::MeshGeometry*	m_singleLightning;
	Tool::CommonMaterial	m_lightningMaterial;
#endif // ENABLE_LIGHTNINGS

	TheCity					m_city;
	TheSubmersible			m_submersible;
};
