#include "RuinsScene.hh"

#include "engine/algebra/Utils.hh"
#include "engine/container/Utils.hh"
#include "engine/core/Camera.hxx"
#include "engine/core/Debug.hh"
#include "engine/core/Settings.hh"
#include "engine/noise/Hash.hh"
#include "engine/noise/PerlinNoise.hh"
#include "engine/noise/Rand.hh"
#include "engine/texture/Texture.hxx"
#include "gfx/IGraphicLayerImplementations.hh"
#include "meshes/GreekColumn.hh"
#include "meshes/Plant.hh"
#include "meshes/Road.hh"
#include "meshes/Rocks.hh"
#include "meshes/RuinsFloor.hh"
#include "textures/Cobbles.hh"
#include "textures/Ground.hh"
#include "textures/RandomTexture.hh"
#include "tool/FixedLightToolBox.hh"
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

// SIZE: Comment to remove seaweed animation and save 120 bytes.
#define ENABLE_SEAWEEDS_ANIMATION

static Noise::Rand rnd;

void RuinsScene::Init(Gfx::IGraphicLayer* gfxLayer,
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
	m_floatingParticles.Init(gfxLayer, 512 * 512, 16, quad, shaderLoader, textureLoader);

	LOAD_TEXTURE_SIMPLE(*textureLoader, m_randomTexture, RandomTexture, 256, 256);

	Gfx::TextureID* textureIDs;
	LOAD_MESH(*meshLoader, m_rocks, Rocks, VertexDataPNTT);
	LOAD_SHADER(*shaderLoader, &m_groundMaterial.shader, ruinsGroundShader, "assets/shaders/pbr.vs", "assets/shaders/pbrWithShadows.fs");
	LOAD_SHADER(*shaderLoader, &m_groundMaterial.shadowShader, ruinsGroundShadow, "assets/shaders/pbr.vs", "assets/shaders/shadowMap.fs");
	LOAD_TEXTURE(*textureLoader, textureIDs, Ground, settings.TextureSizeAbout(512), settings.TextureSizeAbout(512));
	m_groundMaterial.SetDiffuseSpecularRoughnessNormalAndHeight(textureIDs);
	m_groundMaterial.parallaxScale = Ground::s_parallaxScale * _TV(1.f); // FIXME: check the texture parallax.

#ifdef ENABLE_SEAWEEDS_ANIMATION
	LOAD_SHADER(*shaderLoader, &m_seaweedMaterial.shader, seaweedShader, "assets/shaders/seaweed.vs", "assets/shaders/seaweed.fs");
	LOAD_SHADER(*shaderLoader, &m_seaweedMaterial.shadowShader, seaweedShadow, "assets/shaders/seaweed.vs", "assets/shaders/shadowMap.fs");
#else // !ENABLE_SEAWEEDS_ANIMATION
	LOAD_SHADER(*shaderLoader, &m_seaweedMaterial.shader, seaweedShader, "assets/shaders/pbr.vs", "assets/shaders/seaweed.fs");
	LOAD_SHADER(*shaderLoader, &m_seaweedMaterial.shadowShader, seaweedShadow, "assets/shaders/pbr.vs", "assets/shaders/shadowMap.fs");
#endif // !ENABLE_SEAWEEDS_ANIMATION
	m_seaweedMaterial.SetDiffuseSpecularRoughnessNormalAndHeight(textureIDs);
	//m_seaweedMaterial.parallaxScale = 0.f;

	LOAD_MESH(*meshLoader, m_floor, RuinsFloor, VertexDataPNTT);
	LOAD_MESH(*meshLoader, m_column, GreekColumn, VertexDataPNTT);
	LOAD_MESH(*meshLoader, m_plantDensePatch, PlantDensePatch, VertexDataPNTT);
	LOAD_MESH(*meshLoader, m_plantSparsePatch, PlantSparsePatch, VertexDataPNTT);
	LOAD_MESH(*meshLoader, m_road, Road, VertexDataPNTT);
	LOAD_SHADER(*shaderLoader, &m_roadMaterial.shader, ruinsRoadShader, "assets/shaders/pbr.vs", "assets/shaders/pbrWithShadows.fs");
	LOAD_SHADER(*shaderLoader, &m_roadMaterial.shadowShader, ruinsRoadShadow, "assets/shaders/pbr.vs", "assets/shaders/shadowMap.fs");
	LOAD_TEXTURE(*textureLoader, textureIDs, Cobbles, settings.TextureSizeAbout(512), settings.TextureSizeAbout(512));
	m_roadMaterial.SetDiffuseSpecularRoughnessNormalAndHeight(textureIDs);
	//m_roadMaterial.parallaxScale = 0.f; //Cobbles::s_parallaxScale; // FIXME: check the texture parallax.

	m_submersible.Init(settings, meshLoader, shaderLoader, textureLoader);
}

void RuinsScene::Draw(long currentStoryTime, const Core::CameraDescription& cameraDesc, const Gfx::DrawArea& drawArea)
{
	// HACK: Disable parallax mapping on the ground during the first
	// shot, to hide bug.
	m_groundMaterial.parallaxScale = (currentStoryTime > _TV(8000) ? Ground::s_parallaxScale : 0.f);

	// Participating medium
	m_medium.DecodeColorAndDensity(VAR(ruinsScene_mediumColor)->get4((float)currentStoryTime));
	m_medium.scatteringIntensity = _TV(0.05f);
	m_medium.forwardScatteringStrength = _TV(0.8f);

	// It's not necessary to call SetVolume each frame, but it makes
	// tweaking a lot easier.
	m_floatingParticles.SetVolume(
		_TV(100.f), _TV(0.f), _TV(0.f),
		_TV(40.f), _TV(20.f), _TV(100.f));
	m_floatingParticles.renderMaterial
		.SetDiffuse(_TV(0.5f), _TV(0.5f), _TV(0.5f))
		.SetOpacity(_TV(0.1f));
	m_floatingParticles.particleSize = _TV(0.2f);
	m_floatingParticles.forwardScatteringStrength = _TV(-0.25f);
	m_floatingParticles.Update(currentStoryTime);

	// The submersible
	const vector4f submersible = VAR(ruinsScene_submersiblePosition)->get4((float)currentStoryTime);
	const Core::RigidTransform submersibleTransform = Core::RigidTransform::translation(submersible.x, submersible.y, submersible.z)
		.rotate(submersible.w * DEG_TO_RAD, vector3f::uy);

	// The lights
	m_lightRig.ambientLight = vector3f::zero;
	vector3f lightColors[] = {
		VAR(ruinsScene_ambientLightColor)->getLightColor((float)currentStoryTime),
		vector3f::zero, // Unused.
		VAR(ruinsScene_submersibleLightColor)->getLightColor((float)currentStoryTime),
		VAR(ruinsScene_submersibleLightColor)->getLightColor((float)currentStoryTime),
		VAR(ruinsScene_submersibleLightColor)->getLightColor((float)currentStoryTime),
	};

	float lightCones[ARRAY_LEN(m_lightRig.lights)] = {
		DEG_TO_RAD * _TV(40.f),
		0.f,
		0.f,
		0.f,
		0.f,
	};

	int lightTypes[ARRAY_LEN(m_lightRig.lights)] = { 0 };

	Core::RigidTransform lightTransforms[ARRAY_LEN(m_lightRig.lights)] = {
		{ { _TV(100.f), _TV(220.f), _TV(-32.f), },
		Algebra::quaternion::rotation(_TV(-0.65f) * PI, vector3f::ux), },
	};

	TheSubmersible::GetLights(lightCones + 2, lightTypes + 2, lightTransforms + 2);
	m_lightRig.SetLighting(lightTransforms, lightCones, lightTypes, lightColors);

	// Attach the spot lights to the submersible.
	for (int i = 0; i < 3; ++i)
	{
		int base = 2;
		m_lightRig.lights[i + base].transform = submersibleTransform * m_lightRig.lights[i + base].transform;
	}

	CreateScene(currentStoryTime);

	const float lightZNears[NUM_LIGHTS] = { 150.f, 150.f, 0.25f, 0.25f, 0.25f };
	const float lightZFars[NUM_LIGHTS] = { 300.f, 300.f, 80.f, 80.f, 80.f };
	m_lightToolBox->RenderShadowMaps(*m_renderList, currentStoryTime, m_lightRig, lightZNears, lightZFars, _TV(1));

	float zNear = _TV(1.f);
	float zFar = _TV(1000.f);
	Core::CameraDescription shakyCam = cameraDesc;
	if (currentStoryTime > _TV(200000)) { // earthquake
		float a = Noise::PerlinNoise::value(currentStoryTime/_TV(100.f), _TV(10000.f));
		shakyCam.transform.rotate(a * _TV(0.0025f), _TV(1.f), _TV(0.5f), _TV(0.f));
	}
	const Core::Camera camera = CreateCamera(shakyCam, (float)drawArea.viewport.width / (float)drawArea.viewport.height, zNear, zFar);

	const vector3f bgColor = m_medium.GetBackgroundColor(zFar);
	m_gfxLayer->Clear(drawArea.frameBuffer, bgColor.x, bgColor.y, bgColor.z, true);
	m_renderList->Render(currentStoryTime, drawArea, Gfx::RasterTests::RegularDepthTest, camera, nullptr, m_lightRig, m_medium, false);

	// Participating medium.
	m_volumetricLighting->Draw(currentStoryTime, drawArea, m_depthTexture, camera, m_lightRig,
		zNear, zFar, m_medium, _TV(100.f), _TV(1));

	// Transparent geometry.
	m_floatingParticles.Draw(currentStoryTime, drawArea, camera, m_lightRig, m_medium);
}

void RuinsScene::CreateScene(long currentStoryTime)
{
	m_renderList->size = 0;

	const vector4f rocksPositions[] = {
		{ _TV( 98.f), _TV( -3.f), _TV(  9.f), _TV(1.0f) },
		{ _TV(-20.f), _TV( -4.0f), _TV( 10.f), _TV(2.0f) },
		{ _TV( 25.f), _TV(-12.0f), _TV(110.f), _TV(4.0f) },
		{ _TV( 90.f), _TV( -2.5f), _TV( 80.f), _TV(1.0f) },
		{ _TV( 15.f), _TV( -4.5f), _TV(-22.f), _TV(1.5f) },
	};
	for (unsigned i = 0; i < ARRAY_LEN(rocksPositions); i++)
	{
		m_renderList->Add(*m_rocks, m_groundMaterial,
			matrix4::translation(rocksPositions[i].x, rocksPositions[i].y, rocksPositions[i].z)
			.scale(rocksPositions[i].w));
	}
	m_renderList->Add(*m_floor, m_groundMaterial, matrix4::identity);

	// The columns
	{
		Noise::Rand rand;
		//const float maxVisibilityDistance = _TV(110.f); // Depends on the lighting.

		const float pStillPresent = _TV(0.6f);
		const float pStillUp = _TV(0.8f);
		const float yUp = _TV(0.5f);
		const float yFallen = _TV(1.2f);
		const float angleUp = _TV(5.f) * DEG_TO_RAD;
		const float angleFallen = _TV(92.f) * DEG_TO_RAD;
		const float px = _TV(95.f);
		const float dx = _TV(20.f);
		const float pz = _TV(35.f);
		const float dz = _TV(10.f);

		const int fallDuration = _TV(2800);
		const int fallStartDate = _TV(210000);

		for (int j = 0; j < 2; ++j)
		{
			for (int i = 0; i < 20; ++i)
			{
				float p = rand.fgen();
				if (p > pStillPresent)
				{
					int fallDate = fallStartDate + _TV(1000) * (Noise::Hash::get32(i, j) % 40);
					float fallAnimation = Algebra::pow(interpolate(fallDate + fallDuration, fallDate, currentStoryTime), _TV(0.6f));
					float isStillUp = msys_min(float(p > pStillUp), fallAnimation);
					float y = mix(yFallen, yUp, isStillUp);
					float angle = mix(angleFallen, angleUp, isStillUp);
					float orientation = 2.f * PI * rand.fgen();
					vector3f position = { px + dx * j, y, pz + dz * i };

					Core::RigidTransform transform = {
						position,
						Algebra::quaternion::rotation(orientation, vector3f::uy) *
						Algebra::quaternion::rotation(angle, vector3f::uz),
					};

					m_renderList->Add(*m_column, m_groundMaterial, ComputeMatrix(transform));
				}
			}
		}
	}

	// The plants
	{
		Noise::Rand rand;
		const float xmin = _TV(30.f);
		const float xmax = _TV(140.f);
		const float y = _TV(-0.2f);
		const float zmin = _TV(22.f);
		const float zmax = _TV(120.f);

		//const float maxVisibilityDistance = _TV(70.f); // Depends on the lighting.
		const int numPlants = _TV(50);

		const Gfx::Uniform randomTextureSampler = Gfx::Uniform::Sampler1(U_RANDOMTEXTURE, m_randomTexture[0]);

		for (int i = 0; i < numPlants; ++i)
		{
			const vector3f position = { mix(xmin, xmax, rand.fgen()), y, mix(zmin, zmax, rand.fgen()) };
			float angle = rand.fgen() * 2.f * PI;

			Render::MeshGeometry* mesh = (i % 2 == 0 ? m_plantDensePatch : m_plantSparsePatch);
			int index = m_renderList->Add(*mesh, m_seaweedMaterial,
				matrix4::translation(position)
				.rotate(angle, vector3f::uy));
			m_renderList->shadingParametersForRendering[index].uniforms.add(randomTextureSampler);
		}
	}

	m_renderList->Add(*m_road, m_roadMaterial,
		matrix4::translation(_TV(100.f), _TV(-0.5f), _TV(35.f)));

	// Submersible
	const vector4f& submersible = VAR(ruinsScene_submersiblePosition)->get4((float)currentStoryTime);
	Core::RigidTransform submersibleTransform =
		Core::RigidTransform::translation(submersible.x, submersible.y, submersible.z)
		.rotate(submersible.w * DEG_TO_RAD, vector3f::uy);

	m_submersible.AddToScene(m_renderList,
		currentStoryTime,
		Core::ComputeMatrix(submersibleTransform),
		VAR(ruinsScene_submersibleLightColor)->getLightColor((float)currentStoryTime));
}
