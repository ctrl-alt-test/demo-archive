#include "DescentScene.hh"

#include "engine/algebra/Utils.hh"
#include "engine/algebra/Vector3.hxx"
#include "engine/container/Utils.hh"
#include "engine/core/Settings.hh"
#include "engine/timeline/Anim.hxx"
#include "gfx/IGraphicLayerImplementations.hh"
#include "textures/ResearchVessel.hh"
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

using namespace Algebra;
using namespace Tool;

void DescentScene::Init(Gfx::IGraphicLayer* gfxLayer,
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

	m_submersibleDepth.SetAnim(0, -100, 60000);
	m_submersible.Init(settings, meshLoader, shaderLoader, textureLoader);
}

void DescentScene::Draw(long currentStoryTime, const Core::CameraDescription& cameraDesc, const Gfx::DrawArea& drawArea)
{
	// Participating medium
	m_medium.DecodeColorAndDensity(VAR(descentScene_mediumColor)->get4());
	m_medium.scatteringIntensity = _TV(0.01f);
	m_medium.forwardScatteringStrength = _TV(0.8f);

	// It's not necessary to call SetVolume each frame, but it makes
	// tweaking a lot easier.
	m_floatingParticles.SetVolume(
		_TV(-74.f), _TV(-90.f), _TV(-50.f),
		_TV(16.f), _TV(16.f), _TV(32.f));
	m_floatingParticles.renderMaterial
		.SetDiffuse(_TV(0.5f), _TV(0.5f), _TV(0.5f))
		.SetOpacity(_TV(0.5f));
	m_floatingParticles.particleSize = _TV(0.04f);
	m_floatingParticles.forwardScatteringStrength = _TV(0.5f);
	m_floatingParticles.Update(currentStoryTime);

	// The submersible
	const float depth = m_submersibleDepth.Get(currentStoryTime);
	m_submersibleTransform = Core::RigidTransform::translation(_TV(0.f), depth, _TV(8.f))
		.rotate(PI, vector3f::uy);
	const Core::RigidTransform submersibleLightTransform = m_submersibleTransform *
		Core::RigidTransform::translation(_TV(0.f), _TV(-1.48f), _TV( 3.15f))
		.rotate(_TV(1.1f) * PI, vector3f::ux);

	// The lights
	m_lightRig.ambientLight = vector3f::zero;
	const Algebra::vector3f lightColors[] = {
		VAR(descentScene_ambientLightColor)->getLightColor((float)currentStoryTime),
		VAR(descentScene_submersibleLightColor)->getLightColor((float)currentStoryTime),
		vector3f::zero, // Unused.
		vector3f::zero, // Unused.
		vector3f::zero, // Unused.
	};

	float lightCones[ARRAY_LEN(m_lightRig.lights)] = {
		DEG_TO_RAD * _TV(60.f),
		DEG_TO_RAD * _TV(15.f),
		0.f,
		0.f,
		0.f,
	};

	int lightTypes[ARRAY_LEN(m_lightRig.lights)] = { _TV(2), 1, 0, 0, 0 };

	Core::RigidTransform lightTransforms[ARRAY_LEN(m_lightRig.lights)] = {
		{ { _TV(  0.f), _TV(50.f), _TV(-50.f), },
		Algebra::quaternion::rotation(-PI / 2.f, vector3f::ux), },
		submersibleLightTransform,
	};

	// Don't use the submersible lights, because a single, narrow cone
	// looks better on this shot.
	/* TheSubmersible::GetLights(nullptr, lightTypes, lightTransforms + 2); */
	m_lightRig.SetLighting(lightTransforms, lightCones, lightTypes, lightColors);

	CreateScene(currentStoryTime);

	const float lightZNears[NUM_LIGHTS] = { 20.f, 0.5f, 0.5f, 0.5f, 0.5f };
	const float lightZFars[NUM_LIGHTS] = { 250.f, 250.f, 250.f, 250.f, 250.f };
	m_lightToolBox->RenderShadowMaps(*m_renderList, currentStoryTime, m_lightRig, lightZNears, lightZFars, _TV(1));

	float zNear = _TV(1.f);
	float zFar = _TV(1000.f);
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

void DescentScene::CreateScene(long currentStoryTime)
{
	m_renderList->size = 0;

	m_submersible.AddToScene(m_renderList,
		currentStoryTime,
		Core::ComputeMatrix(m_submersibleTransform),
		VAR(descentScene_submersibleLightColor)->getLightColor((float)currentStoryTime));
}
