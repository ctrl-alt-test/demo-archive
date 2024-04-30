#pragma once

#include "TheSubmersible.hh"
#include "engine/core/Light.hh"
#include "engine/timeline/Anim.hh"
#include "engine/timeline/Timeline.hh"
#include "gfx/DrawArea.hh"
#include "gfx/Geometry.hh"
#include "gfx/ShadingParameters.hh"
#include "tool/CommonMaterial.hh"
#include "tool/FixedLightToolBox.hh"
#include "tool/ParticipatingMedium.hh"
#include "tool/ParticleField.hh"
#include "tool/ShaderHelper.hh"

namespace Core
{
	class Settings;
}

namespace Gfx
{
	class IGraphicLayer;
}

namespace Tool
{
	class MeshLoader;
	class RenderList;
	class ShaderLoader;
	class TextureLoader;
	class VolumetricLighting;
}

class DescentScene : public Timeline::IScene
{
public:
	DescentScene():
		m_gfxLayer(nullptr)
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

	Timeline::Anim<float>	m_submersibleDepth;
	Core::RigidTransform	m_submersibleTransform;
	TheSubmersible			m_submersible;

	Tool::ParticleField		m_floatingParticles;
};
