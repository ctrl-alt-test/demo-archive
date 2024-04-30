#pragma once

#include "TheCity.hh"
#include "engine/core/Light.hh"
#include "engine/timeline/Anim.hh"
#include "engine/timeline/Timeline.hh"
#include "gfx/DrawArea.hh"
#include "gfx/Geometry.hh"
#include "tool/CommonMaterial.hh"
#include "tool/FixedLightToolBox.hh"
#include "tool/ParticipatingMedium.hh"
#include "tool/ShaderHelper.hh"
#include "tool/SimpleSkyGradient.hh"

#define ENABLE_BLUR_REFLECTION 1 // SIZE: The option costs ~40 bytes on the compressed binary.
#define ENABLE_BLUR_REFRACTION 1 // SIZE: The option costs ~40 bytes on the compressed binary.

namespace Core
{
	class Settings;
}

namespace Gfx
{
	class IGraphicLayer;
	struct RasterTests;
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
	class VolumetricLighting;
}

class EmergenceScene : public Timeline::IScene
{
public:
	EmergenceScene ():
		m_gfxLayer(nullptr)
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
	void CreateFoamLine(const Algebra::vector2f& start,
						const Algebra::vector2f& end,
						float size,
						int numberOfPatches);
	void CreateScene(long currentStoryTime);

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
#if ENABLE_BLUR_REFLECTION
	PostProcessing::GaussianBlur m_reflectionGaussianBlur;
#endif // ENABLE_BLUR_REFRACTION
#if ENABLE_BLUR_REFRACTION
	PostProcessing::GaussianBlur m_refractionGaussianBlur;
#endif // ENABLE_BLUR_REFRACTION

	Tool::FixedLightRig		m_lightRig;
	Tool::FixedLightToolBox* m_lightToolBox;
	Tool::VolumetricLighting* m_volumetricLighting;
	Tool::SimpleSkyGradient	m_sky;

	Container::Array<Gfx::Uniform> m_baseSeaSurfaceShadingUniforms;
	Container::Array<float>	m_foamData;

	Render::MeshGeometry*	m_seaSurface;
	Gfx::ShadingParameters	m_seaSurfaceShading;

	Render::MeshGeometry*	m_foamPatch;
	Gfx::TextureID			m_foamTexture;

	TheCity					m_city;

	Render::MeshGeometry*	m_fountain;
	Render::MeshGeometry*	m_islandFloor;
	Render::MeshGeometry*	m_obelisk;
	Render::MeshGeometry*	m_seagull;
	Render::MeshGeometry*	m_tholos;

	Tool::CommonMaterial	m_floorMaterial;
	Tool::CommonMaterial	m_foamPatchDummyMaterial;
	Tool::CommonMaterial	m_marbleWallMaterial;
	Tool::CommonMaterial	m_seagullMaterial;
};
