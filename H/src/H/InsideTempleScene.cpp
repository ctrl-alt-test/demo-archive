#include "InsideTempleScene.hh"

#include "engine/algebra/Functions.hh"
#include "engine/algebra/Interpolation.hxx"
#include "engine/algebra/Quaternion.hxx"
#include "engine/algebra/Utils.hh"
#include "engine/algebra/Vector3.hxx"
#include "engine/container/Utils.hh"
#include "engine/core/Settings.hh"
#include "engine/noise/PerlinNoise.hh"
#include "engine/texture/Texture.hxx"
#include "gfx/IGraphicLayerImplementations.hh"
#include "meshes/Artemision.hh"
#include "meshes/Fence.hh"
#include "meshes/Lightning.hh"
#include "meshes/PoseidonSeated.hh"
#include "meshes/Statue.hh"
#include "meshes/Trident.hh"
#include "textures/GreekColumn.hh"
#include "textures/Marble.hh"
#include "textures/RandomTexture.hh"
#include "tool/FixedLightToolBox.hh"
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

void InsideTempleScene::Init(Gfx::IGraphicLayer* gfxLayer,
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
#if ENABLE_TEMPLE_PARTICLES
	m_floatingParticles.Init(gfxLayer, 512*512, 16, quad, shaderLoader, textureLoader);
#endif // ENABLE_TEMPLE_PARTICLES

	Gfx::TextureID* textureIDs;

	LOAD_TEXTURE_SIMPLE(*textureLoader, m_randomTexture, RandomTexture, 256, 256);

	LOAD_MESH(*meshLoader, m_artemision, Artemision, VertexDataPNTT);
	LOAD_SHADER(*shaderLoader, &m_marbleWallMaterial.shader, insideArtemisionWallShader, "assets/shaders/pbr.vs", "assets/shaders/pbrWithShadows.fs");
	LOAD_SHADER(*shaderLoader, &m_marbleWallMaterial.shadowShader, insideArtemisionWallShadow, "assets/shaders/pbr.vs", "assets/shaders/shadowMap.fs");
	LOAD_TEXTURE(*textureLoader, textureIDs, FineWhiteMarbleWall, settings.TextureSizeAbout(512), settings.TextureSizeAbout(512));
	m_marbleWallMaterial.SetDiffuseSpecularRoughnessNormalAndHeight(textureIDs);
	m_marbleWallMaterial.parallaxScale = FineWhiteMarbleWall::s_parallaxScale;

	LOAD_MESH(*meshLoader, m_artemisionColumns, ArtemisionColumns, VertexDataPNTT);
	LOAD_SHADER(*shaderLoader, &m_ionicColumnMaterial.shader, insideArtemisionColumnShader, "assets/shaders/pbr.vs", "assets/shaders/pbrWithIncandescentOrichalcum.fs");
	LOAD_SHADER(*shaderLoader, &m_ionicColumnMaterial.shadowShader, insideArtemisionColumnShadow, "assets/shaders/pbr.vs", "assets/shaders/shadowMap.fs");
	LOAD_TEXTURE(*textureLoader, textureIDs, IonicColumn, settings.TextureSizeAbout(1024), settings.TextureSizeAbout(1024));
	m_ionicColumnMaterial.SetDiffuseSpecularRoughnessNormalAndHeight(textureIDs);
	m_ionicColumnMaterial.parallaxScale = IonicColumn::s_parallaxScale * _TV(0.f); // FIXME: columns look bad with POM. Disable or use simple parallax instead.

	LOAD_MESH(*meshLoader, m_artemisionFloor, ArtemisionFloor, VertexDataPNTT);
	LOAD_SHADER(*shaderLoader, &m_floorMaterial.shader, insideArtemisionFloorShader, "assets/shaders/pbr.vs", "assets/shaders/pbrWithShadows.fs");
	//LOAD_SHADER(*shaderLoader, &m_floorMaterial.shadowShader, insideArtemisionFloorShadow, "assets/shaders/pbr.vs", "assets/shaders/shadowMap.fs");
	LOAD_TEXTURE(*textureLoader, textureIDs, FineRedAndWhiteMarbleFloor, settings.TextureSizeAbout(512), settings.TextureSizeAbout(512));
	m_floorMaterial.SetDiffuseSpecularRoughnessNormalAndHeight(textureIDs);
	m_floorMaterial.parallaxScale = FineRedAndWhiteMarbleFloor::s_parallaxScale;

	LOAD_MESH(*meshLoader, m_artemisionGreetingsFloor, ArtemisionGreetingsFloor, VertexDataPNTT);
	LOAD_SHADER(*shaderLoader, &m_greetingsMaterial.shader, insideArtemisionGreetingsShader, "assets/shaders/pbr.vs", "assets/shaders/pbrWithIncandescentOrichalcum.fs");
	//LOAD_SHADER(*shaderLoader, &m_greetingsMaterial.shadowShader, insideArtemisionGreetingsShadow, "assets/shaders/pbr.vs", "assets/shaders/shadowMap.fs");
	LOAD_TEXTURE(*textureLoader, textureIDs, GreetingsMarbleFloor, settings.TextureSizeAbout(512), settings.TextureSizeAbout(512));
	m_greetingsMaterial.SetDiffuseSpecularRoughnessNormalAndHeight(textureIDs);
	m_greetingsMaterial.parallaxScale = GreetingsMarbleFloor::s_parallaxScale;

	LOAD_MESH(*meshLoader, m_fence, InsideFence, VertexDataPNTT);
	LOAD_MESH(*meshLoader, m_poseidon, PoseidonSeated, VertexDataPNTT);
	LOAD_MESH(*meshLoader, m_poseidonEyes, PoseidonEyes, VertexDataPNTT);
	LOAD_MESH(*meshLoader, m_trident, Trident, VertexDataPNTT);

#if ENABLE_LIGHTNING
	LOAD_MESH(*meshLoader, m_lightning, Lightning, VertexDataPNTT);
#endif // ENABLE_LIGHTNING
	LOAD_SHADER(*shaderLoader, &m_lightningMaterial.shader, nodeShader, "assets/shaders/unicolor.vs", "assets/shaders/unicolorWithShadows.fs");

	LOAD_SHADER(*shaderLoader, &m_tridentMaterial.shader, insideArtemisionTridentShader, "assets/shaders/pbr.vs", "assets/shaders/pbrWithIncandescentOrichalcum.fs");
	LOAD_SHADER(*shaderLoader, &m_tridentMaterial.shadowShader, insideArtemisionTridentShadow, "assets/shaders/pbr.vs", "assets/shaders/shadowMap.fs");
	LOAD_TEXTURE(*textureLoader, textureIDs, Gold, settings.TextureSizeAbout(512), settings.TextureSizeAbout(512));
	m_tridentMaterial.SetDiffuseSpecularRoughnessNormalAndHeight(textureIDs);
	m_tridentMaterial.parallaxScale = Gold::s_parallaxScale;

	//LOAD_MESH(*meshLoader, m_theElder, SdfTest, VertexDataPNTT);
}

void InsideTempleScene::Draw(long currentStoryTime, const Core::CameraDescription& cameraDesc, const Gfx::DrawArea& drawArea)
{
	// Participating medium
	m_medium.DecodeColorAndDensity(VAR(insideTemple_mediumColor)->get4());
	m_medium.scatteringIntensity = _TV(0.05f);
	m_medium.forwardScatteringStrength = _TV(0.55f);

#if ENABLE_TEMPLE_PARTICLES
	// It's not necessary to call SetVolume each frame, but it makes
	// tweaking a lot easier.
	m_floatingParticles.SetVolume(
		_TV(36.5f), _TV(18.f), _TV(62.f),
		_TV(9.5f), _TV(14.f), _TV(17.f));
	m_floatingParticles.renderMaterial
		.SetDiffuse(_TV(0.5f), _TV(0.5f), _TV(0.5f))
		.SetOpacity(_TV(0.25f));
	m_floatingParticles.particleSize = _TV(0.05f);
	m_floatingParticles.forwardScatteringStrength = _TV(0.5f);
	m_floatingParticles.Update(currentStoryTime);
#endif // ENABLE_TEMPLE_PARTICLES

	// The lights
	m_lightRig.ambientLight = vector3f::zero;
	Algebra::vector3f lightColors[] = {
		// Ambient light, cella.
		{ _TV(2.f), _TV(4.f), _TV(12.f), },

		// Ambient light, front facade.
		{ _TV(2.5f), _TV(5.f), _TV(15.f), },

		// Lights left for the statue epic light, the passing
		// submersible effect and the orichalcum illumination.
		vector3f::zero,
		vector3f::zero,
		vector3f::zero,
	};

	float lightCones[ARRAY_LEN(m_lightRig.lights)] = {
		// Ambient light, narrow on the cella.
		DEG_TO_RAD * _TV(25.f),

		// Ambient light, on the facade.
		DEG_TO_RAD * _TV(40.f),

		// Statue epic light.
		DEG_TO_RAD * _TV(12.f),

		// Lights of the passing submersible effect.
		DEG_TO_RAD * _TV(15.f),
		DEG_TO_RAD * _TV(15.f),
	};

	int lightTypes[ARRAY_LEN(m_lightRig.lights)] = { 0, 0, 0, 1, 1 };

	Core::RigidTransform lightTransforms[ARRAY_LEN(m_lightRig.lights)] = {
		// Ambient light, on the cella with a bit of oblique so the
		// statue is hidden in shadow,
		{ { _TV(1.f), _TV(100.f), _TV(78.f), },
		Algebra::quaternion::rotation(_TV(-0.35f) * PI, vector3f::uy) *
		Algebra::quaternion::rotation(_TV(-0.35f) * PI, vector3f::ux)
		},

		// Ambient light, on the facade.
		{ { _TV(10.f), _TV(100.f), _TV(-20.f), },
		Algebra::quaternion::rotation(_TV(0.2f) * PI, vector3f::uy) *
		Algebra::quaternion::rotation(_TV(-0.63f) * PI, vector3f::ux)
		},

		// Statue epic light.
		{ { _TV(25.f), _TV(90.f), _TV(74.f) },
		Algebra::quaternion::rotation(_TV(-0.40f) * PI, vector3f::uy) *
		Algebra::quaternion::rotation(_TV(-0.45f) * PI, vector3f::ux)
		},

		// Two lights left for the passing submersible effect.
	};

	// Statue epic light
	const bool enableStatueLight = (currentStoryTime < _TV(3000));
	if (enableStatueLight)
	{
		m_medium.forwardScatteringStrength = _TV(0.8f);
		lightColors[2] = VAR(insideTemple_epicStatueLightColor)->getLightColor((float)currentStoryTime);
	}

	// Submersible light
	const bool enableSubmersibleLight = (currentStoryTime >= _TV(4500) && currentStoryTime < _TV(8000));
	if (enableSubmersibleLight)
	{
		vector3f submersibleLight = VAR(insideTemple_submersibleLightColor)->getLightColor(0);
		submersibleLight *= VAR(insideTemple_submersibleLightIntensity)->get1((float)currentStoryTime);
		lightColors[3] = submersibleLight;
		lightColors[4] = submersibleLight;

		const Algebra::vector3f p1 = { _TV(60.f), _TV(80.f), _TV(72.5f) };
		const Algebra::vector3f p2 = { _TV(10.f), _TV(80.f), _TV(72.5f) };
		const Algebra::quaternion q1 =
			Algebra::quaternion::rotation(_TV(0.3f) * PI, vector3f::uy) *
			Algebra::quaternion::rotation(_TV(-0.2f) * PI, vector3f::ux);

		const Algebra::quaternion q2 =
			Algebra::quaternion::rotation(_TV(0.4f) * PI, vector3f::uy) *
			Algebra::quaternion::rotation(_TV(-0.8f) * PI, vector3f::ux);

		long t1 = _TV(3500);
		long t2 = _TV(7500);
		lightTransforms[3].v = mix(p1, p2, interpolate((float)t1, (float)t2, (float)currentStoryTime));
		lightTransforms[3].q = mix(q1, q2, interpolate((float)t1, (float)t2, (float)currentStoryTime));
		lightTransforms[4].v = mix(p1, p2, interpolate((float)t1, (float)t2, (float)currentStoryTime + _TV(500.f)));
		lightTransforms[4].q = mix(q1, q2, interpolate((float)t1, (float)t2, (float)currentStoryTime + _TV(-200.f)));
	}

	// Orichalcum fake illumination,
	// overwriting the submersible lights
	const bool enableOrichalcumFakeBounceLight = (currentStoryTime > _TV(8000));
	if (enableOrichalcumFakeBounceLight)
	{
		const float intensity = smoothStep(_TV(9500), _TV(18500), currentStoryTime);
		lightColors[3] = vec3(_TV(1.f), _TV(0.0281f), _TV(0.002f)) *
			(_TV(0.08f) * Algebra::pow(2.f, intensity * 6.f - 1.f) * intensity);

		lightCones[3] = DEG_TO_RAD * _TV(180.f);

		const Algebra::vector3f p = { _TV(36.5f), _TV(0.f), _TV(59.f) };
		const Algebra::quaternion q = Algebra::quaternion::rotation(_TV(0.5f) * PI, vector3f::ux);

		lightTransforms[3].v = p;
		lightTransforms[3].q = q;
	}

	m_lightRig.SetLighting(lightTransforms, lightCones, lightTypes, lightColors);

	CreateScene(currentStoryTime);

	const float lightZNears[NUM_LIGHTS] = { 40.f, 40.f, 0.25f, 0.25f, 0.25f };
	const float lightZFars[NUM_LIGHTS] = { 120.f, 120.f, 120.f, 120.f, 120.f };
	m_lightToolBox->RenderShadowMaps(*m_renderList, currentStoryTime, m_lightRig, lightZNears, lightZFars, _TV(1));

	float zNear = _TV(1.f);
	float zFar = _TV(1000.f);

	Core::CameraDescription shakyCam = cameraDesc;
    if (currentStoryTime > _TV(19500)) { // earthquake
        float a = Noise::PerlinNoise::value(currentStoryTime/_TV(100.f), _TV(10000.f));
        shakyCam.transform.rotate(a * _TV(0.002f), _TV(1.f), _TV(0.f), _TV(0.f));
    }
    const Core::Camera camera = CreateCamera(shakyCam, (float)drawArea.viewport.width / (float)drawArea.viewport.height, zNear, zFar);

	const vector3f bgColor = m_medium.GetBackgroundColor(zFar);
	m_gfxLayer->Clear(drawArea.frameBuffer, bgColor.x, bgColor.y, bgColor.z, true);
	m_renderList->Render(currentStoryTime, drawArea, Gfx::RasterTests::RegularDepthTest, camera, nullptr, m_lightRig, m_medium, false);

	// Participating medium.
	m_volumetricLighting->Draw(currentStoryTime, drawArea, m_depthTexture, camera, m_lightRig,
		zNear, zFar, m_medium, _TV(100.f), _TV(1));

	// Transparent geometry.
#if ENABLE_TEMPLE_PARTICLES
	m_floatingParticles.Draw(currentStoryTime, drawArea, camera, m_lightRig, m_medium);
#endif // ENABLE_TEMPLE_PARTICLES
}

void InsideTempleScene::CreateScene(long currentStoryTime)
{
	m_renderList->size = 0;

	const Gfx::Uniform randomTextureSampler = Gfx::Uniform::Sampler1(U_RANDOMTEXTURE, m_randomTexture[0]);

	m_renderList->Add(*m_artemision, m_marbleWallMaterial, matrix4::identity);
	m_renderList->Add(*m_artemisionFloor, m_floorMaterial, matrix4::identity);

	int index = m_renderList->Add(*m_artemisionGreetingsFloor, m_greetingsMaterial,
		matrix4::translation(_TV(36.5f), _TV(4.06f), _TV(59.3f)));
	m_renderList->shadingParametersForRendering[index].uniforms.add(randomTextureSampler);

	index = m_renderList->Add(*m_artemisionColumns, m_ionicColumnMaterial, matrix4::identity);
	m_renderList->shadingParametersForRendering[index].uniforms.add(randomTextureSampler);

	index = m_renderList->Add(*m_poseidon, m_marbleWallMaterial,
		matrix4::translation(_TV(36.75f), _TV(11.3f), _TV(70.5f))
		.rotate(_TV(180.f) * DEG_TO_RAD, vector3f::uy));
	//s_renderList.shadingParametersForRendering[index].uniforms.add(randomTextureSampler);

	// Eyes
	float intensity = VAR(insideTemple_eyeIntensity)->get1((float)currentStoryTime);
	vector3f color = intensity * vec3(_TV(1.f), _TV(0.01f), _TV(0.f));
	m_lightningMaterial.SetEmissive(color.x, color.y, color.z);
	m_lightningMaterial.SetDiffuse(_TV(1.f), _TV(1.f), _TV(1.f));
	index = m_renderList->Add(*m_poseidonEyes, m_lightningMaterial,
		matrix4::translation(_TV(36.42f), _TV(16.62f), _TV(71.61f)));
	m_renderList->shadingParametersForRendering[index].uniforms.add(randomTextureSampler);

	// Trident
	index = m_renderList->Add(*m_trident, m_tridentMaterial,
		matrix4::translation(_TV(39.7f), _TV(17.8f), _TV(68.2f))
				.rotate(_TV(16.f) * DEG_TO_RAD, vector3f::uy)
				.rotate(_TV(-7.f) * DEG_TO_RAD, vector3f::uz)
				.rotate(_TV(32.f) * DEG_TO_RAD, vector3f::uy));
	m_renderList->shadingParametersForRendering[index].uniforms.add(randomTextureSampler);

#if ENABLE_LIGHTNING
	// lightning
	if (currentStoryTime > _TV(19500) &&
		currentStoryTime < _TV(120000) &&
		(currentStoryTime / _TV(150)) % _TV(5) == 0)
	{
		float intensity = 
			_TV(0.5f) + _TV(0.5f) * Noise::PerlinNoise::value(currentStoryTime/_TV(1000.f), _TV(1000.f));
		vector3f color = vec3(intensity * _TV(30.f));
		m_lightningMaterial.SetEmissive(color.x, color.y, color.z);
		m_lightningMaterial.SetDiffuse(0.f, 0.f, 0.f);
		float size1 = _TV(0.01f) + _TV(0.25f) * smoothStep(_TV(21000), _TV(22500), currentStoryTime);
		float size2 = smoothStep(_TV(19500), _TV(21000), currentStoryTime);

		m_renderList->Add(*m_lightning, m_lightningMaterial,
			matrix4::translation(_TV(40.0f), _TV(20.125f), _TV(68.f))
			.rotate(currentStoryTime * _TV(1.002f), vector3f::uy)
			.scale(size1, size2 * _TV(0.25f), size1));
	}
#endif // ENABLE_LIGHTNING

	vector3f fences[] = { // position * angle
		{ _TV(32.5f), _TV(66.f), _TV(0.f) },
		{ _TV(32.5f), _TV(66.f), _TV(1.f) },
		{ _TV(40.5f), _TV(66.f), _TV(1.f) },
		{ _TV(26.2f), _TV(53.f), _TV(0.f) },
		{ _TV(38.8f), _TV(53.f), _TV(0.f) },
	};
	for (size_t i = 0; i < ARRAY_LEN(fences); i++)
	{
		const vector3f& fence = fences[i];
		m_renderList->Add(*m_fence, m_marbleWallMaterial,
			matrix4::translation(fence.x, _TV(4.f), fence.y)
			.rotate(fence.z * -90.f * DEG_TO_RAD, vector3f::uy));
	}
}
