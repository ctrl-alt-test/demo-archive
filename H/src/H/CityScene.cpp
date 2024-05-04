#include "CityScene.hh"

#include "engine/algebra/Utils.hh"
#include "engine/algebra/Vector3.hxx"
#include "engine/core/RigidTransform.hxx"
#include "engine/core/Settings.hh"
#include "engine/noise/PerlinNoise.hh"
#include "gfx/IGraphicLayerImplementations.hh"
#include "meshes/CityFloor.hh"
#include "textures/Ground.hh"
#include "tool/mesh/VertexDataPNTT.hh"
#include "tool/MeshLoader.hh"
#include "tool/ProjectLoader.hh"
#include "tool/RenderList.hh"
#include "tool/ShaderLoader.hh"
#include "tool/TextureLoader.hh"
#include "tool/VolumetricLighting.hh"
#include "tweakval/tweakval.h"

//#undef _TV
//#define _TV(x) (x)

using namespace Algebra;
using namespace Tool;

void CityScene::Init(Gfx::IGraphicLayer* gfxLayer,
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
	Gfx::TextureID* textureIDs;

#if ENABLE_CITY_PARTICLES
	m_floatingParticles.Init(gfxLayer, 512 * 512, 16, quad, shaderLoader, textureLoader);
#endif // ENABLE_CITY_PARTICLES

	LOAD_MESH(*meshLoader, m_floor, CityFloor, VertexDataPNTT);
	LOAD_MESH(*meshLoader, m_islandFloor, IslandFloor, VertexDataPNTT);
	LOAD_SHADER(*shaderLoader, &m_floorMaterial.shader, floorShader,
		"assets/shaders/pbr.vs",
		"assets/shaders/pbrWithShadows.fs");
	LOAD_SHADER(*shaderLoader, &m_floorMaterial.shadowShader, floorShadow,
		"assets/shaders/pbr.vs",
		"assets/shaders/shadowMap.fs");
	LOAD_TEXTURE(*textureLoader, textureIDs, Ground, settings.TextureSizeAbout(512), settings.TextureSizeAbout(512));
	m_floorMaterial.SetDiffuseSpecularRoughnessNormalAndHeight(textureIDs);
	m_floorMaterial.parallaxScale = Ground::s_parallaxScale * _TV(0.25f); // FIXME: check the texture parallax.

#if ENABLE_LIGHTNINGS
	LOAD_MESH(*meshLoader, m_lightning, Lightning, VertexDataPNTT);
	LOAD_MESH(*meshLoader, m_singleLightning, SingleLightning, VertexDataPNTT);
	LOAD_SHADER(*shaderLoader, &m_lightningMaterial.shader, lightningShader,
		"assets/shaders/unicolor.vs",
		"assets/shaders/unicolorWithShadows.fs");
#endif // ENABLE_LIGHTNINGS

	m_city.Init(settings, meshLoader, shaderLoader, textureLoader);
	m_submersible.Init(settings, meshLoader, shaderLoader, textureLoader);
}

void CityScene::Draw(long currentStoryTime, const Core::CameraDescription& cameraDesc, const Gfx::DrawArea& drawArea)
{
	bool awakening = (currentStoryTime >= _TV(200000));

	// Participating medium
	m_medium.DecodeColorAndDensity(VAR(cityScene_mediumColor)->get4((float)currentStoryTime));
	m_medium.scatteringIntensity = (awakening ? _TV(0.05f) : _TV(0.05f));
	m_medium.forwardScatteringStrength = _TV(0.8f);

#if ENABLE_CITY_PARTICLES
	const vector3f particlePosition = VAR(cityScene_particlePosition)->get3((float)currentStoryTime);

	m_floatingParticles.SetVolume(
		particlePosition.x, particlePosition.y, particlePosition.z,
		_TV(50.f), _TV(16.f), _TV(50.f));
	m_floatingParticles.renderMaterial
		.SetDiffuse(_TV(0.5f), _TV(0.5f), _TV(0.5f))
		.SetOpacity(_TV(0.125f));
	m_floatingParticles.particleSize = _TV(0.125f);
	m_floatingParticles.forwardScatteringStrength = _TV(0.f);
	m_floatingParticles.verticalSpeed = awakening ? _TV(-0.05f) : _TV(0.001f);
	m_floatingParticles.Update(currentStoryTime);
#endif // ENABLE_CITY_PARTICLES

	// The submersible
	vector4f submersible = VAR(cityScene_submersiblePosition)->get4((float)currentStoryTime);
	Core::RigidTransform submersibleTransform = Core::RigidTransform::translation(submersible.x, submersible.y, submersible.z)
		.rotate(submersible.w * DEG_TO_RAD, vector3f::uy);

	// The lights
	m_lightRig.ambientLight = vec3(_TV(0.00f), _TV(0.00f), _TV(0.00f));;
	float lightZNears[NUM_LIGHTS] = { 0.5f, 0.5f, 0.5f, _TV(64.f), _TV(64.f) };
	float lightZFars[NUM_LIGHTS] = { 200.f, 200.f, 200.f, _TV(512.f), _TV(512.f) };

	// Before emergence.
	Algebra::vector3f lightColors[] = {
		vector3f::zero,
		vector3f::zero,
		vector3f::zero,
		VAR(cityScene_ambientLightColor)->getLightColor((float)currentStoryTime),
		VAR(cityScene_sunLightColor)->getLightColor((float)currentStoryTime),
	};
	float lightCones[ARRAY_LEN(m_lightRig.lights)] = {
		0.f,
		0.f,
		0.f,
		DEG_TO_RAD * _TV(25.f),
		DEG_TO_RAD * _TV(9.f),
	};
	Core::RigidTransform lightTransforms[ARRAY_LEN(m_lightRig.lights)] = {
		Core::RigidTransform(),
	};
	int lightTypes[ARRAY_LEN(m_lightRig.lights)] = { 0 };

	// Submersible lights.
	if (_TV(1) || !awakening)
	{
		TheSubmersible::GetLights(lightCones, lightTypes, lightTransforms);
		for (int i = 0; i < 3; ++i)
		{
			lightZNears[i] = 0.25f;
			lightZFars[i] = 50.f;
			lightColors[i] = VAR(cityScene_submersibleLightColor)->getLightColor((float)currentStoryTime);

			// Attach the spot light to the submersible.
			lightTransforms[i] = submersibleTransform * lightTransforms[i];
		}
	}

	// Ambient and sun light.
	//if (awakening)
	{
		lightTransforms[3] =
			Core::RigidTransform::rotation(_TV(0.23f) * PI, vector3f::uy)
			.rotate(_TV(-0.85f) * PI, vector3f::ux)
			.translate(_TV(480.f), _TV(100.f), _TV(-300.f));

		lightTransforms[4] = 
			Core::RigidTransform::rotation(_TV(0.f) * PI, vector3f::uy)
			.rotate(_TV(-0.5f) * PI, vector3f::ux)
			.translate(_TV(350.f), _TV(150.f), _TV(-50.f));
	}

	m_lightRig.SetLighting(lightTransforms, lightCones, lightTypes, lightColors);

	CreateScene(currentStoryTime);

	m_lightToolBox->RenderShadowMaps(*m_renderList, currentStoryTime, m_lightRig, lightZNears, lightZFars, _TV(1));

	float zNear = _TV(1.f);
	float zFar = _TV(1000.f);
	Core::CameraDescription shakyCam = cameraDesc;
	if (awakening) { // earthquake
		float a = Noise::PerlinNoise::value(currentStoryTime/_TV(100.f), _TV(10000.f));
		shakyCam.transform.rotate(a * _TV(0.0025f), _TV(1.f), _TV(0.5f), _TV(0.f));
	}
	const Core::Camera camera = CreateCamera(shakyCam, (float)drawArea.viewport.width / (float)drawArea.viewport.height, zNear, zFar);
	
	const vector3f bgColor = m_medium.GetBackgroundColor(zFar);
	m_gfxLayer->Clear(drawArea.frameBuffer, bgColor.x, bgColor.y, bgColor.z, true);

	// hack because time in CityScene doesn't match awakening time inside the temple
	int orichalcumTime = currentStoryTime - _TV(190000);
	m_renderList->Render(orichalcumTime, drawArea, Gfx::RasterTests::RegularDepthTest, camera, nullptr, m_lightRig, m_medium, false);

	// Participating medium.
	m_volumetricLighting->Draw(currentStoryTime, drawArea, m_depthTexture, camera, m_lightRig,
		zNear, zFar, m_medium, _TV(100.f), _TV(1));

	// Transparent geometry.
#if ENABLE_CITY_PARTICLES
	m_floatingParticles.Draw(currentStoryTime, drawArea, camera, m_lightRig, m_medium);
#endif // ENABLE_CITY_PARTICLES
}

void CityScene::CreateScene(long currentStoryTime)
{
	m_renderList->size = 0;

	bool awakening = (currentStoryTime >= _TV(200000));

	Core::RigidTransform cityTransform = Core::RigidTransform::translation(_TV(350.f), _TV(-1.5f), _TV(-50.f));

	// Ground.
	m_renderList->Add(*m_floor, m_floorMaterial,
		matrix4::translation(-400.f, -1.5f, -800.f));

	// Entire city.
	m_city.AddToScene(m_renderList, currentStoryTime, cityTransform);

	// Submersible
	if (!awakening)
	{
		const vector4f& submersible = VAR(cityScene_submersiblePosition)->get4((float)currentStoryTime);
		Core::RigidTransform submersibleTransform =
			Core::RigidTransform::translation(submersible.x, submersible.y, submersible.z)
			.rotate(submersible.w * DEG_TO_RAD, vector3f::uy);

		m_submersible.AddToScene(m_renderList,
			currentStoryTime,
			Core::ComputeMatrix(submersibleTransform),
			VAR(cityScene_submersibleLightColor)->getLightColor((float)currentStoryTime));
	}

#if ENABLE_LIGHTNINGS
	if (awakening)
	{
		// Lightning
		for (int i = 0; i < 2; i++) 
		{
			int effectLength = _TV(3000);
			int localTime = currentStoryTime + i * effectLength / 2;
			int id = localTime / effectLength;
			float time = (float) (localTime % effectLength) / effectLength;

			float power = smoothStep(_TV(0.f), _TV(0.0625f), time)
				* smoothStep(_TV(0.2f), _TV(0.125f), time);
			if (power <= 0.f)
				continue;
			Noise::Rand rnd(id * 123 + i * 7);
			vector3f color = power * vec3(1.f, 1.f, 1.f) * rnd.fgen(_TV(25.f), _TV(50.f));
			m_lightningMaterial.SetDiffuse(_TV(1.f), _TV(1.f), _TV(1.f));
			m_lightningMaterial.SetEmissive(color.x, color.y, color.z);
			m_renderList->Add(*m_lightning, m_lightningMaterial,
				matrix4::translation(_TV(346.f), _TV(80.f), _TV(-40.f))
				.rotate(_TV(100000.f) * rnd.fgen(0, 2.f * PI), vector3f::uy));
		}

		// Additional lightning
		for (int i = 0; i < 2; i++) 
		{
			int effectLength = _TV(3000);
			int localTime = currentStoryTime + i * effectLength / 2;
			int id = localTime / effectLength;
			float time = (float) (localTime % effectLength) / effectLength;

			float power = smoothStep(_TV(0.f), _TV(0.0625f), time)
				* smoothStep(_TV(0.2f), _TV(0.125f), time);
			if (power <= 0.f)
				continue;
			Noise::Rand rnd(id * 123 + i * 7);
			vector3f color = power * vec3(1.f, 1.f, 1.f) * rnd.fgen(_TV(25.f), _TV(50.f));
			m_lightningMaterial.SetDiffuse(_TV(1.f), _TV(1.f), _TV(1.f));
			m_lightningMaterial.SetEmissive(color.x, color.y, color.z);
			m_renderList->Add(*m_singleLightning, m_lightningMaterial,
				matrix4::translation(_TV(346.f), _TV(480.f), _TV(-40.f))
				.rotate(_TV(10000.f) * rnd.fgen(0, 2.f * PI), vector3f::uz));
		}
	}
#endif // ENABLE_LIGHTNINGS
}
