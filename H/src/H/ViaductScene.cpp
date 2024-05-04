#include "ViaductScene.hh"

#include "engine/algebra/Utils.hh"
#include "engine/algebra/Vector3.hxx"
#include "engine/container/Utils.hh"
#include "engine/core/RigidTransform.hxx"
#include "engine/core/Settings.hh"
#include "engine/timeline/Anim.hxx"
#include "gfx/IGraphicLayerImplementations.hh"
#include "meshes/Viaduct.hh"
#include "textures/OldStone.hh"
#include "tool/FixedLightToolBox.hh"
#include "tool/mesh/VertexDataPNTT.hh"
#include "tool/MeshLoader.hh"
#include "tool/ProjectLoader.hh"
#include "tool/RenderList.hh"
#include "tool/ShaderLoader.hh"
#include "tool/TextureLoader.hh"
#include "tool/VolumetricLighting.hh"
#include "tweakval/tweakval.h"
#include <cassert>

// SIZE: Comment to have less pretty particles and save 51 bytes.
#define ENABLE_ADAPT_PARTICLES_TO_SHOTS

using namespace Algebra;
using namespace Tool;

void ViaductScene::Init(Gfx::IGraphicLayer* gfxLayer,
						RenderList* renderList,
						const Core::Settings& settings,
						Tool::MeshLoader* meshLoader,
						Tool::ShaderLoader* shaderLoader,
						Tool::TextureLoader* textureLoader,
						const Gfx::TextureID& depthTexture,
						Tool::FixedLightToolBox* lightToolBox,
						Tool::VolumetricLighting* volumetricLighting,
						const Gfx::Geometry& quad)
{
	m_gfxLayer = gfxLayer;
	m_renderList = renderList;

	m_depthTexture = depthTexture;
	m_lightToolBox = lightToolBox;
	m_lightRig.shadowMaps = m_lightToolBox->shadowVarianceTextures;
	m_volumetricLighting = volumetricLighting;
	m_floatingParticles.Init(gfxLayer, 512*512, 16, quad, shaderLoader, textureLoader);
	Gfx::TextureID* textureIDs;

	LOAD_MESH(*meshLoader, m_viaduct, Viaduct, VertexDataPNTT);
	LOAD_SHADER(*shaderLoader, &m_viaductMaterial.shader, viaductShader, "assets/shaders/pbr.vs", "assets/shaders/pbrWithShadows.fs");
	LOAD_SHADER(*shaderLoader, &m_viaductMaterial.shadowShader, viaductShadow, "assets/shaders/pbr.vs", "assets/shaders/shadowMap.fs");
	LOAD_TEXTURE(*textureLoader, textureIDs, OldStone, settings.TextureSizeAbout(512), settings.TextureSizeAbout(512));
	m_viaductMaterial.SetDiffuseSpecularRoughnessNormalAndHeight(textureIDs);
	m_viaductMaterial.parallaxScale = OldStone::s_parallaxScale;

	m_submersible.Init(settings, meshLoader, shaderLoader, textureLoader);
}

void ViaductScene::Draw(long currentStoryTime, const Core::CameraDescription& cameraDesc, const Gfx::DrawArea& drawArea)
{
	// Participating medium
	m_medium.DecodeColorAndDensity(VAR(viaductScene_mediumColor)->get4());
	m_medium.scatteringIntensity = VAR(viaductScene_scatteringIntensity)->get1((float)currentStoryTime);
	m_medium.forwardScatteringStrength = _TV(0.8f);

	// The code change is done with a if so we can still use tweakable
	// values. The compiler will remove the dead branch.
#ifdef ENABLE_ADAPT_PARTICLES_TO_SHOTS
	const bool enableAdaptParticlesToShots = true;
#else // !ENABLE_ADAPT_PARTICLES_TO_SHOTS
	const bool enableAdaptParticlesToShots = false;
#endif // !ENABLE_ADAPT_PARTICLES_TO_SHOTS
	if (enableAdaptParticlesToShots)
	{
		const bool isUnderArch = (currentStoryTime > _TV(20000));
		const float volumePositionY = (isUnderArch ? _TV(-20.f) : _TV(15.f));
		const float volumePositionZ = (isUnderArch ? _TV(25.f) : _TV(-15.f));
		m_floatingParticles.SetVolume(_TV(-15.f), volumePositionY, volumePositionZ, _TV(25.f), _TV(20.f), _TV(20.f));
		m_floatingParticles.renderMaterial
			.SetDiffuse(_TV(0.5f), _TV(0.5f), _TV(0.5f))
			.SetOpacity(isUnderArch ? _TV(0.25f) : _TV(0.035f));

		// As a quick hack, use the distance between the particle field
		// and the camera to deduce if it's a close shot, and set the particle
		// size accordingly.
		const float quickHack = volumePositionZ - cameraDesc.transform.v.z;
		m_floatingParticles.particleSize = (quickHack > _TV(32.f) ? _TV(0.21f) : _TV(0.1f));
	}
	else
	{
		m_floatingParticles.SetVolume(
			_TV(-15.f), _TV(0.f), _TV(10.f),
			_TV(25.f), _TV(50.f), _TV(40.f));
		m_floatingParticles.renderMaterial
			.SetDiffuse(_TV(0.5f), _TV(0.5f), _TV(0.5f))
			.SetOpacity(_TV(0.1f));
		m_floatingParticles.particleSize = _TV(0.15f);
	}

	m_floatingParticles.forwardScatteringStrength = _TV(0.25f);
	m_floatingParticles.Update(currentStoryTime);

	// The submersible
	const vector4f submersible = VAR(viaductScene_submersiblePosition)->get4((float)currentStoryTime);
	m_submersibleTransform = Core::RigidTransform::translation(submersible.x, submersible.y, submersible.z)
		.rotate(submersible.w, vector3f::uy);

	// The lights
	m_lightRig.ambientLight = vector3f::zero;
	const Algebra::vector3f lightColors[] = {
		VAR(viaductScene_ambientLightColor)->getLightColor((float)currentStoryTime),
		vector3f::zero, // Unused
		VAR(viaductScene_submersibleLightColor)->getLightColor((float)currentStoryTime),
		VAR(viaductScene_submersibleLightColor)->getLightColor((float)currentStoryTime),
		VAR(viaductScene_submersibleLightColor)->getLightColor((float)currentStoryTime),
	};

	float lightCones[ARRAY_LEN(m_lightRig.lights)] = {
		// Natural light from above.
		DEG_TO_RAD * _TV(80.f),
	};

	int lightTypes[ARRAY_LEN(m_lightRig.lights)] = { 0 };

	Core::RigidTransform lightTransforms[ARRAY_LEN(m_lightRig.lights)] = {
		// Natural light from above.
		{ { _TV(-50.f), _TV(160.f), _TV(  0.f), },
		Algebra::quaternion::rotation(_TV(-0.5f) * PI, vector3f::ux) },
	};

	TheSubmersible::GetLights(lightCones + 2, lightTypes + 2, lightTransforms + 2);
	m_lightRig.SetLighting(lightTransforms, lightCones, lightTypes, lightColors);

	// Attach the spot lights to the submersible.
	for (int i = 0; i < 3; ++i)
	{
		int base = 2;
		m_lightRig.lights[i + base].transform = m_submersibleTransform * m_lightRig.lights[i + base].transform;
	}

	CreateScene(currentStoryTime);

	const float lightZNears[NUM_LIGHTS] = { 20.f, 20.f, 0.25f, 0.25f, 0.25f };
	const float lightZFars[NUM_LIGHTS] = { 250.f, 250.f, 50.f, 50.f, 50.f };
	m_lightToolBox->RenderShadowMaps(*m_renderList, currentStoryTime, m_lightRig, lightZNears, lightZFars, _TV(1));

	float zNear = _TV(1.f);
	float zFar = _TV(600.f);
	const Core::Camera camera = CreateCamera(cameraDesc, (float)drawArea.viewport.width / (float)drawArea.viewport.height, zNear, zFar);

	const vector3f bgColor = m_medium.GetBackgroundColor(zFar);
	m_gfxLayer->Clear(drawArea.frameBuffer, bgColor.x, bgColor.y, bgColor.z, true);
	m_renderList->Render(currentStoryTime, drawArea, Gfx::RasterTests::RegularDepthTest, camera, nullptr, m_lightRig, m_medium, false);

	// Participating medium.
	m_volumetricLighting->Draw(currentStoryTime, drawArea, m_depthTexture, camera, m_lightRig,
		zNear, zFar, m_medium, _TV(100.f), _TV(1));

	// Transparent geometry.
	m_floatingParticles.Draw(currentStoryTime, drawArea, camera, m_lightRig, m_medium);
}

void ViaductScene::CreateScene(long currentStoryTime)
{
	m_renderList->size = 0;

	m_renderList->Add(*m_viaduct, m_viaductMaterial, matrix4::identity);

	m_submersible.AddToScene(m_renderList,
		currentStoryTime,
		Core::ComputeMatrix(m_submersibleTransform),
		VAR(viaductScene_submersibleLightColor)->getLightColor((float)currentStoryTime));
}
