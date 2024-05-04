#pragma once

#include "TheSubmersible.hh"
#include "engine/core/Light.hh"
#include "engine/timeline/Anim.hh"
#include "engine/timeline/Timeline.hh"
#include "gfx/DrawArea.hh"
#include "tool/CommonMaterial.hh"
#include "tool/FixedLightToolBox.hh"
#include "tool/ParticipatingMedium.hh"
#include "tool/ParticleField.hh"
#include "tool/ShaderHelper.hh"
#include "tool/SimpleSkyGradient.hh"

#define ENABLE_SUBMERSIBLE_BUBBLES 1 // SIZE: The option costs 270 bytes on the compressed binary.

namespace Core
{
	class Settings;
}

namespace Gfx
{
	class IGraphicLayer;
	struct RasterTests;
}

namespace Tool
{
	class MeshLoader;
	class RenderList;
	class ShaderLoader;
	class TextureLoader;
	class VolumetricLighting;
}

class BoatScene : public Timeline::IScene
{
public:
	BoatScene():
		m_gfxLayer(nullptr),
		m_researchVesselHull(nullptr),
		m_researchVesselBridgeWindow(nullptr),
		m_researchVesselBridgeNoWindow(nullptr),
		m_researchVesselCrane(nullptr)
	{}

	void Init(Gfx::IGraphicLayer* gfxLayer,
			  Tool::RenderList* renderList,
			  const Core::Settings& settings,
			  Tool::MeshLoader* meshLoader,
			  Tool::ShaderLoader* shaderLoader,
			  Tool::TextureLoader* textureLoader,
			  const Gfx::TextureID& depthTexture,
			  const Gfx::TextureID* reflectionTextures,
			  const Gfx::TextureID* refractionTextures,
			  const Gfx::FrameBufferID& reflectionFB,
			  const Gfx::FrameBufferID& refractionFB,
			  Tool::FixedLightToolBox* lightToolBox,
			  Tool::VolumetricLighting* m_volumetricLighting,
			  const Gfx::Geometry& quad);

	void Draw(long currentStoryTime,
			  const Core::CameraDescription& cameraDesc,
			  const Gfx::DrawArea& drawArea) override;

private:
	void CreateScene(long currentStoryTime,
					 const Algebra::matrix4& boatMatrix,
					 const Algebra::matrix4& craneMatrix,
					 const Algebra::matrix4& submersibleMatrix,
					 const Algebra::vector3f& submersibleSpotLightColor);

	Gfx::IGraphicLayer*		m_gfxLayer;
	int						m_renderWidth;
	int						m_renderHeight;
	Tool::RenderList*		m_renderList;

	Gfx::TextureID			m_depthTexture;
	Gfx::ShadingParameters	m_shading;
	Gfx::TextureID*			m_randomTexture;

	const Gfx::TextureID*	m_reflectionTextures;
	const Gfx::TextureID*	m_refractionTextures;
	Gfx::FrameBufferID		m_reflectionFB;
	Gfx::FrameBufferID		m_refractionFB;

	Tool::FixedLightRig		m_lightRig;
	Tool::FixedLightToolBox* m_lightToolBox;
	Tool::VolumetricLighting* m_volumetricLighting;
#if ENABLE_SUBMERSIBLE_BUBBLES
	Tool::ParticleField		m_bubbles;
#endif // ENABLE_TEMPLE_PARTICLES
	Tool::SimpleSkyGradient	m_sky;

	Render::MeshGeometry*	m_seaSurface;
	Gfx::ShadingParameters	m_seaSurfaceShading;

	Render::MeshGeometry*	m_researchVesselHull;
	Tool::CommonMaterial	m_researchVesselHullMaterial;

	Render::MeshGeometry*	m_researchVesselBridgeWindow;
	Tool::CommonMaterial	m_researchVesselBridgeWindowMaterial;

	Render::MeshGeometry*	m_researchVesselBridgeNoWindow;
	Tool::CommonMaterial	m_researchVesselBridgeNoWindowMaterial;

	Render::MeshGeometry*	m_researchVesselCrane;
	Tool::CommonMaterial	m_researchVesselCraneMaterial;

	TheSubmersible			m_submersible;
};
