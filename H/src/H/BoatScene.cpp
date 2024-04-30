#include "BoatScene.hh"

#include "engine/algebra/Functions.hh"
#include "engine/algebra/Quaternion.hxx"
#include "engine/algebra/Utils.hh"
#include "engine/container/Utils.hh"
#include "engine/core/Camera.hxx"
#include "engine/core/RigidTransform.hxx"
#include "engine/core/Settings.hh"
#include "gfx/IGraphicLayerImplementations.hh"
#include "meshes/ResearchVessel.hh"
#include "meshes/SeaSurface.hh"
#include "textures/RandomTexture.hh"
#include "textures/ResearchVessel.hh"
#include "tool/FixedLightToolBox.hh"
#include "tool/mesh/MeshHelper.hh"
#include "tool/mesh/VertexDataPNT.hh"
#include "tool/mesh/VertexDataPNTT.hh"
#include "tool/MeshLoader.hh"
#include "tool/ProjectLoader.hh"
#include "tool/RenderList.hh"
#include "tool/ShaderLoader.hh"
#include "tool/TextureLoader.hh"
#include "tool/VolumetricLighting.hh"
#include "tweakval/tweakval.h"

using namespace Algebra;
using namespace Tool;

void BoatScene::Init(Gfx::IGraphicLayer* gfxLayer,
					 RenderList* renderList,
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
					 Tool::VolumetricLighting* volumetricLighting,
					 const Gfx::Geometry& quad)
{
	m_gfxLayer = gfxLayer;
	m_renderList = renderList;
	m_renderWidth = settings.renderWidth;
	m_renderHeight = settings.renderHeight;

	m_depthTexture = depthTexture;
	m_reflectionTextures = reflectionTextures;
	m_refractionTextures = refractionTextures;
	m_reflectionFB = reflectionFB;
	m_refractionFB = refractionFB;
	m_lightToolBox = lightToolBox;
	m_lightRig.shadowMaps = m_lightToolBox->shadowVarianceTextures;
	m_volumetricLighting = volumetricLighting;
#if ENABLE_SUBMERSIBLE_BUBBLES
	m_bubbles.Init(gfxLayer, 512*256, 16, quad, shaderLoader, textureLoader);
	LOAD_SHADER(*shaderLoader, &m_bubbles.computeProgram.shader, bubblesCompute, "assets/shaders/screenspace/blitShader.vs", "assets/shaders/compute/splashBubbles.fs");
	LOAD_SHADER(*shaderLoader, &m_bubbles.renderMaterial.shader, bubblesRender, "assets/shaders/particle.vs", "assets/shaders/bubbleParticle.fs");
#endif // ENABLE_SUBMERSIBLE_BUBBLES
	m_sky.Init(gfxLayer, quad, shaderLoader);

	Gfx::TextureID* textureIDs;

	LOAD_TEXTURE_SIMPLE(*textureLoader, m_randomTexture, RandomTexture, 256, 256);

	LOAD_MESH(*meshLoader, m_seaSurface, SeaSurface, VertexDataPNT);

	LOAD_MESH(*meshLoader, m_researchVesselHull, ResearchVesselHull, VertexDataPNTT);
	LOAD_MESH(*meshLoader, m_researchVesselBridgeWindow, ResearchVesselBridgeWindowPart, VertexDataPNTT);
	LOAD_MESH(*meshLoader, m_researchVesselBridgeNoWindow, ResearchVesselBridgeNoWindowPart, VertexDataPNTT);
	LOAD_MESH(*meshLoader, m_researchVesselCrane, ResearchVesselCrane, VertexDataPNT);

	LOAD_SHADER(*shaderLoader, &m_seaSurfaceShading.shader, boatSeaSurfaceShader, "assets/shaders/seaSurface.vs", "assets/shaders/seaSurface_boatScene.fs");

	LOAD_SHADER(*shaderLoader, &m_researchVesselHullMaterial.shader, researchVesselHullShader, "assets/shaders/researchVesselHull.vs", "assets/shaders/researchVesselHull.fs");
	LOAD_SHADER(*shaderLoader, &m_researchVesselHullMaterial.shadowShader, researchVesselHullShadowShader, "assets/shaders/researchVesselHull.vs", "assets/shaders/researchVesselHullShadowMap.fs");
	LOAD_TEXTURE(*textureLoader, textureIDs, RVHullTex, settings.TextureSizeAbout(512), settings.TextureSizeAbout(512));
	m_researchVesselHullMaterial.SetDiffuseSpecularRoughnessNormalAndHeight(textureIDs);
	m_researchVesselHullMaterial.parallaxScale = RVHullTex::s_parallaxScale;

	LOAD_SHADER(*shaderLoader, &m_researchVesselBridgeWindowMaterial.shader, researchVesselBridgeShader, "assets/shaders/pbr.vs", "assets/shaders/pbrWithShadows.fs");
	LOAD_SHADER(*shaderLoader, &m_researchVesselBridgeWindowMaterial.shadowShader, researchVesselBridgeShadow, "assets/shaders/pbr.vs", "assets/shaders/shadowMap.fs");
	LOAD_TEXTURE(*textureLoader, textureIDs, RVBridgeWindowTex, settings.TextureSizeAbout(512), settings.TextureSizeAbout(512));
	m_researchVesselBridgeWindowMaterial.SetDiffuseSpecularRoughnessNormalAndHeight(textureIDs);
	m_researchVesselBridgeWindowMaterial.parallaxScale = RVBridgeWindowTex::s_parallaxScale;

	LOAD_SHADER(*shaderLoader, &m_researchVesselBridgeNoWindowMaterial.shader, researchVesselBridgeShader, "assets/shaders/pbr.vs", "assets/shaders/pbrWithShadows.fs");
	LOAD_SHADER(*shaderLoader, &m_researchVesselBridgeNoWindowMaterial.shadowShader, researchVesselBridgeShadow, "assets/shaders/pbr.vs", "assets/shaders/shadowMap.fs");
	LOAD_TEXTURE(*textureLoader, textureIDs, RVBridgeNoWindowTex, settings.TextureSizeAbout(512), settings.TextureSizeAbout(512));
	m_researchVesselBridgeNoWindowMaterial.SetDiffuseSpecularRoughnessNormalAndHeight(textureIDs);
	m_researchVesselBridgeNoWindowMaterial.parallaxScale = RVBridgeNoWindowTex::s_parallaxScale;

	LOAD_SHADER(*shaderLoader, &m_researchVesselCraneMaterial.shader, researchVesselCraneShader, "assets/shaders/unicolor.vs", "assets/shaders/unicolorWithShadows.fs");
	LOAD_SHADER(*shaderLoader, &m_researchVesselCraneMaterial.shadowShader, researchVesselCraneShadow, "assets/shaders/unicolor.vs", "assets/shaders/shadowMap.fs");
	m_researchVesselCraneMaterial
		.SetDiffuse(0.6f, 0.05f, 0.f)
		.SetSpecular(0.02f)
		.SetRoughness(0.5f);

	m_submersible.Init(settings, meshLoader, shaderLoader, textureLoader);
}

void BoatScene::Draw(long currentStoryTime, const Core::CameraDescription& cameraDesc, const Gfx::DrawArea& drawArea)
{
	// Participating medium
	Tool::ParticipatingMedium atmosphereMedium;
	atmosphereMedium.DecodeColorAndDensity(VAR(boatScene_atmosphereMediumColor)->get4());
	atmosphereMedium.scatteringIntensity = _TV(0.f);
	atmosphereMedium.forwardScatteringStrength = _TV(0.f);

	Tool::ParticipatingMedium underWaterMedium;
	underWaterMedium.DecodeColorAndDensity(VAR(boatScene_underwaterMediumColor)->get4((float)currentStoryTime));
	underWaterMedium.scatteringIntensity = _TV(0.006f);
	underWaterMedium.forwardScatteringStrength = _TV(0.8f);

	// WARNING: Do not tweak the boat animation under any circumstance.
	// The position of the submersible depends on it, and it has already
	// been baked as a spline in H.project.
	const Core::RigidTransform boatTransform = Core::RigidTransform::rotation(0.021f * msys_sinf(0.001f * currentStoryTime), vector3f::ux)
		.rotate(0.05f * msys_sinf(0.0007f * currentStoryTime), vector3f::uz);
	const matrix4 boatMatrix = Core::ComputeMatrix(boatTransform);

	float craneAngle = PI * VAR(boatScene_craneRotation)->get1((float)currentStoryTime);
	const Core::RigidTransform craneTransform = boatTransform * Core::RigidTransform::translation(_TV(3.5f), _TV(0.f), _TV(16.f)).rotate(craneAngle, vector3f::uy);
	const matrix4 craneMatrix = Core::ComputeMatrix(craneTransform);

	// The submersible
	Core::RigidTransform submersibleTransform;
	if (currentStoryTime < _TV(21700))
	{
		// The submersible is dangling.
		submersibleTransform.q =
			quaternion::rotation(_TV(0.05f) * msys_sinf(_TV(0.0021f) * currentStoryTime), vector3f::ux) *
			quaternion::rotation(_TV(0.05f) * msys_sinf(_TV(0.001f) * currentStoryTime), vector3f::uy);
	}
	else
	{
		// The submersible rotates to the right, for the final oblique shot.
		float t = interpolate(_TV(30000), _TV(51400), currentStoryTime);
		submersibleTransform.q = quaternion::rotation(t * _TV(-1.2f), vector3f::uy);
	}
	const vector2f positionXZ = VAR(boatScene_submersiblePositionXZ)->get2((float)currentStoryTime);
	submersibleTransform.v.x = positionXZ.x;
	submersibleTransform.v.z = positionXZ.y;
	submersibleTransform.v.y = VAR(boatScene_submersiblePositionY)->get1((float)currentStoryTime);

	const matrix4 submersibleMatrix = Core::ComputeMatrix(submersibleTransform);

	// The lights
	const vector3f submersibleSpotLightColor = VAR(boatScene_submersibleLightColor)->getLightColor((float)currentStoryTime);
	m_lightRig.ambientLight = VAR(boatScene_ambientLightColor)->getLightColor((float)currentStoryTime);
	const vector3f lightColors[] = {
		VAR(boatScene_sunLightColor)->getLightColor((float)currentStoryTime),
		VAR(boatScene_waterBounceLightColor)->getLightColor((float)currentStoryTime),

		VAR(boatScene_submersibleLightColor)->getLightColor((float)currentStoryTime),
		VAR(boatScene_submersibleLightColor)->getLightColor((float)currentStoryTime),
		VAR(boatScene_submersibleLightColor)->getLightColor((float)currentStoryTime),
	};

	float lightCones[ARRAY_LEN(m_lightRig.lights)] = {
		// Sun light.
		DEG_TO_RAD * _TV(60.f),

		// Light bouncing off water.
		DEG_TO_RAD * _TV(60.f),
	};

	int lightTypes[ARRAY_LEN(m_lightRig.lights)] = { 0 };

	Core::RigidTransform lightTransforms[ARRAY_LEN(m_lightRig.lights)] = {
		// Sun light.
		{ { _TV(40.f), _TV(50.f), _TV(-20.f), },
		quaternion::rotation(_TV(0.75f) * PI, vector3f::uy) *
		quaternion::rotation(_TV(-0.3f) * PI, vector3f::ux), },

		// Light bouncing off water.
		{ { _TV(0.f), _TV(-50.f), _TV(5.f), },
		quaternion::rotation(_TV(0.5f) * PI, vector3f::ux), },
	};

	TheSubmersible::GetLights(lightCones + 2, lightTypes + 2, lightTransforms + 2);
	m_lightRig.SetLighting(lightTransforms, lightCones, lightTypes, lightColors);

	// Attach the spot lights to the submersible.
	for (int i = 0; i < 3; ++i)
	{
		int base = 2;
		m_lightRig.lights[i + base].transform = submersibleTransform * m_lightRig.lights[i + base].transform;
	}

	CreateScene(currentStoryTime, boatMatrix, craneMatrix, submersibleMatrix, submersibleSpotLightColor);

	const float lightZNears[NUM_LIGHTS] = { 10.f, 10.f, 0.25f, 0.25f, 0.25f };
	const float lightZFars[NUM_LIGHTS] = { 132.f, 132.f, 50.f, 50.f, 50.f };
	m_lightToolBox->RenderShadowMaps(*m_renderList, currentStoryTime, m_lightRig, lightZNears, lightZFars, _TV(2));
	FixedLightRig underWaterLightRig = m_lightRig;
	underWaterLightRig.ambientLight = vector3f::zero;
	underWaterLightRig.lights[0].type = 2;

	float zFar = _TV(1000.f);
	const Core::Camera camera = CreateCamera(cameraDesc, (float)drawArea.viewport.width / (float)drawArea.viewport.height, 1, zFar);

	const plane waterPlane = { { { vector3f::uy, 0.f } } };
	const bool aboveWater = (cameraDesc.transform.v.y > 0.f);

	// Compute bubbles' position.
#if ENABLE_SUBMERSIBLE_BUBBLES
	// It's not necessary to call SetVolume each frame, but it makes
	// tweaking a lot easier.
	m_bubbles.SetVolume(
		_TV(9.9f), _TV(0.f), _TV(16.5f),
		_TV(0.f), _TV(0.f), _TV(0.f));
	m_bubbles.particleSize = _TV(0.2f);
	m_bubbles.submersibleHeight = submersibleTransform.v.y;
	m_bubbles.Update(currentStoryTime);
#endif // ENABLE_SUBMERSIBLE_BUBBLES

	// Reflection
	{
		const FixedLightRig& lightRig = aboveWater ? m_lightRig : underWaterLightRig;
		const ParticipatingMedium& medium = aboveWater ? atmosphereMedium : underWaterMedium;

		const vector3f bgColor = medium.GetBackgroundColor(zFar);

		Core::Camera reflectionCamera = camera;
		reflectionCamera.view.scale(1.f, -1.f, 1.f);
		reflectionCamera.viewProjection = reflectionCamera.projection * reflectionCamera.view;
		Gfx::DrawArea reflectionFB = { m_reflectionFB, { 0, 0, m_renderWidth, m_renderHeight } };
		m_gfxLayer->Clear(reflectionFB.frameBuffer, bgColor.x, bgColor.y, bgColor.z, true);

		plane clippingPlane = waterPlane;
		clippingPlane.dist = _TV(-1.f);
		clippingPlane.normal.y *= (aboveWater ? 1.f : -1.f);
		applyMatrixToPlane(reflectionCamera.view, clippingPlane);

		Gfx::RasterTests reflectionTest(Gfx::FaceCulling::Front, Gfx::DepthFunction::Less, true, true);
		m_renderList->Render(currentStoryTime, reflectionFB, reflectionTest, reflectionCamera, &clippingPlane, lightRig, medium, false);
	}

	// Refraction
	{
		const FixedLightRig& lightRig = !aboveWater ? m_lightRig : underWaterLightRig;
		const ParticipatingMedium& medium = !aboveWater ? atmosphereMedium : underWaterMedium;

		const vector3f bgColor = medium.GetBackgroundColor(zFar);

		Core::Camera refractionCamera = camera;
		const float IoR = (aboveWater ? 1.f / 2.33f : 2.33f);
		refractionCamera.view.scale(1.f, IoR, 1.f);
		refractionCamera.viewProjection = refractionCamera.projection * refractionCamera.view;
		Gfx::DrawArea refractionFB = { m_refractionFB, { 0, 0, m_renderWidth, m_renderHeight } };
		m_gfxLayer->Clear(refractionFB.frameBuffer, bgColor.x, bgColor.y, bgColor.z, true);

		// Sky.
		if (!aboveWater)
		{
			m_sky.Draw(currentStoryTime, refractionFB, refractionCamera,
				VAR(skyFogColor)->getLightColor((float)currentStoryTime) * _TV(6.f),
				VAR(skyColor)->getLightColor((float)currentStoryTime) * _TV(6.f));
		}

		plane clippingPlane = waterPlane;
		clippingPlane.dist = _TV(-1.f);
		clippingPlane.normal.y *= (aboveWater ? -1.f : 1.f);
		applyMatrixToPlane(refractionCamera.view, clippingPlane);

		Gfx::RasterTests refractionTest(Gfx::FaceCulling::Back, Gfx::DepthFunction::Less, true, true);
		m_renderList->Render(currentStoryTime, refractionFB, refractionTest, refractionCamera, &clippingPlane, lightRig, medium, false);
	}

	// Final composite
	{
		const FixedLightRig& lightRig = aboveWater ? m_lightRig : underWaterLightRig;
		const ParticipatingMedium& medium = aboveWater ? atmosphereMedium : underWaterMedium;

		const vector3f bgColor = medium.GetBackgroundColor(zFar);
		m_gfxLayer->Clear(drawArea.frameBuffer, bgColor.x, bgColor.y, bgColor.z, true);

		float durationSinceSplash = float(currentStoryTime - _TV(23100)); // Includes a slight delay.
		float splashIntensity = smoothStep(0.f, _TV(500.f), durationSinceSplash) * smoothStep(_TV(10000.f), _TV(500.f), durationSinceSplash);
		float splashRadius = _TV(10.f) * interpolate(0.f, _TV(1000.f), durationSinceSplash);

		// Centered on the submersible splash, so there are more
		// polygons where the shape is distorded.
		const matrix4 model = matrix4::translation(_TV(9.9f), 0.f, _TV(16.5f));

		const bool firstShot = (currentStoryTime < _TV(5650));

		m_renderList->Render(currentStoryTime, drawArea, Gfx::RasterTests::RegularDepthTest, camera, nullptr, lightRig, medium, false);

		// Sea surface.
		m_seaSurfaceShading.uniforms.empty();
		Tool::ShaderHelper::ExposeTransformationInfo(m_seaSurfaceShading.uniforms, camera, model);
		Tool::ShaderHelper::ExposeLightInfo(m_seaSurfaceShading.uniforms, lightRig.ambientLight, lightRig.lights, lightRig.lightCameras, lightRig.shadowMaps, model);
		Tool::ShaderHelper::ExposeFogInfo(m_seaSurfaceShading.uniforms, medium.color, medium.density);
		Tool::ShaderHelper::ExposeTimeInfo(m_seaSurfaceShading.uniforms, currentStoryTime);
		m_seaSurfaceShading.uniforms.add(Gfx::Uniform::Float1(U_SUBMERSIBLEDEPTH, submersibleTransform.v.y));
		m_seaSurfaceShading.uniforms.add(Gfx::Uniform::Float1(U_SPLASHINTENSITY, splashIntensity));
		m_seaSurfaceShading.uniforms.add(Gfx::Uniform::Float1(U_SPLASHRADIUS, splashRadius));
		m_seaSurfaceShading.uniforms.add(Gfx::Uniform::Float1(U_ABOVEWATER, float(aboveWater)));
		m_seaSurfaceShading.uniforms.add(Gfx::Uniform::Float4(U_WAVEPARAMETERS,
			_TV(0.08f),							// Min amplitude
			(firstShot ? _TV(1.f) : _TV(0.25f)),	// Max amplitude
			(firstShot ? _TV(40.f) :_TV(90.f)), // Wave length
			_TV(0.2f)));						// Steepness
		m_seaSurfaceShading.uniforms.add(Gfx::Uniform::Sampler1(U_RANDOMTEXTURE, m_randomTexture[0]));
		m_seaSurfaceShading.uniforms.add(Gfx::Uniform::Sampler1(U_REFLECTION, m_reflectionTextures[0]));
		m_seaSurfaceShading.uniforms.add(Gfx::Uniform::Sampler1(U_REFLECTIONDEPTH, m_reflectionTextures[1]));
		m_seaSurfaceShading.uniforms.add(Gfx::Uniform::Sampler1(U_REFRACTION, m_refractionTextures[0]));
		m_seaSurfaceShading.uniforms.add(Gfx::Uniform::Sampler1(U_REFRACTIONDEPTH, m_refractionTextures[1]));

		Gfx::RasterTests dualFace(Gfx::FaceCulling::None, Gfx::DepthFunction::Less, true);

		m_gfxLayer->Draw(drawArea, dualFace, m_seaSurface->geometry, m_seaSurfaceShading);

		// Participating medium.
		m_volumetricLighting->Draw(currentStoryTime, drawArea, m_depthTexture, camera, lightRig,
			1.f, 100.f, medium, _TV(100.f), _TV(1));

		// Transparent geometry.
#if ENABLE_SUBMERSIBLE_BUBBLES
		if (currentStoryTime < _TV(30000))
		{
			m_bubbles.Draw(currentStoryTime, drawArea, camera, lightRig, medium);
		}
#endif // ENABLE_SUBMERSIBLE_BUBBLES
	}
}

void BoatScene::CreateScene(long currentStoryTime,
							const matrix4& boatMatrix,
							const matrix4& craneMatrix,
							const matrix4& submersibleMatrix,
							const vector3f& submersibleSpotLightColor)
{
	m_renderList->size = 0;

	if (currentStoryTime > _TV(100000))
	{
		return;
	}

	m_renderList->Add(*m_researchVesselHull, m_researchVesselHullMaterial, boatMatrix);
	m_renderList->Add(*m_researchVesselBridgeWindow, m_researchVesselBridgeWindowMaterial, boatMatrix);
	m_renderList->Add(*m_researchVesselBridgeNoWindow, m_researchVesselBridgeNoWindowMaterial, boatMatrix);
	m_renderList->Add(*m_researchVesselCrane, m_researchVesselCraneMaterial, craneMatrix);

	m_submersible.AddToScene(m_renderList,
		currentStoryTime,
		submersibleMatrix,
		submersibleSpotLightColor);
}
