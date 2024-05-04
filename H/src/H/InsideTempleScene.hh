#pragma once

#include "engine/core/Light.hh"
#include "engine/timeline/Timeline.hh"
#include "gfx/DrawArea.hh"
#include "gfx/ShadingParameters.hh"
#include "tool/CommonMaterial.hh"
#include "tool/FixedLightToolBox.hh"
#include "tool/ParticipatingMedium.hh"
#include "tool/ParticleField.hh"
#include "tool/ShaderHelper.hh"
#include "tool/TextureLoader.hh"

#define ENABLE_TEMPLE_PARTICLES 1 // SIZE: The option costs 40 to 60 bytes on the compressed binary.
#define ENABLE_LIGHTNING 0 // SIZE: The option costs 240 bytes on the compressed binary.

namespace Core
{
	class Settings;
}

namespace Gfx
{
	class IGraphicLayer;
	struct Geometry;
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
	class VolumetricLighting;
}

class InsideTempleScene : public Timeline::IScene
{
public:
	InsideTempleScene():
		m_gfxLayer(nullptr),
		m_artemision(nullptr)
	{}

	void Init(Gfx::IGraphicLayer* gfxLayer,
			  Tool::RenderList* renderList,
			  const Core::Settings& settings,
			  Tool::MeshLoader* meshLoader,
			  Tool::ShaderLoader* shaderLoader,
			  Tool::TextureLoader* textureLoader,
			  const Gfx::TextureID& depthTexture,
			  Tool::FixedLightToolBox* lightToolBox,
			  Tool::VolumetricLighting* volumetricLighting,
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
	Gfx::TextureID*			m_randomTexture;

	Tool::FixedLightRig		m_lightRig;
	Tool::FixedLightToolBox* m_lightToolBox;
	Tool::ParticipatingMedium m_medium;
	Tool::VolumetricLighting* m_volumetricLighting;
#if ENABLE_TEMPLE_PARTICLES
	Tool::ParticleField		m_floatingParticles;
#endif // ENABLE_TEMPLE_PARTICLES

	Render::MeshGeometry*	m_artemision;
	Tool::CommonMaterial	m_marbleWallMaterial;

	Render::MeshGeometry*	m_artemisionColumns;
	Tool::CommonMaterial	m_ionicColumnMaterial;

	Render::MeshGeometry*	m_artemisionFloor;
	Tool::CommonMaterial	m_floorMaterial;

	Render::MeshGeometry*	m_artemisionGreetingsFloor;
	Tool::CommonMaterial	m_greetingsMaterial;

	Render::MeshGeometry*	m_fence;

#if ENABLE_LIGHTNING
	Render::MeshGeometry*	m_lightning;
#endif // ENABLE_LIGHTNING
	Tool::CommonMaterial	m_lightningMaterial;

	Render::MeshGeometry*	m_poseidon;
	Render::MeshGeometry*	m_poseidonEyes;
	Render::MeshGeometry*	m_trident;
	Tool::CommonMaterial	m_tridentMaterial;
};
