#include "EmergenceScene.hh"

#include "engine/algebra/Plane.hh"
#include "engine/algebra/Utils.hh"
#include "engine/algebra/Vector2.hxx"
#include "engine/container/Array.hxx"
#include "engine/core/RigidTransform.hxx"
#include "engine/core/Settings.hh"
#include "engine/noise/Hash.hh"
#include "gfx/IGraphicLayerImplementations.hh"
#include "meshes/CityFloor.hh"
#include "meshes/Fountain.hh"
#include "meshes/Obelisk.hh"
#include "meshes/Parthenon.hh"
#include "meshes/Seagull.hh"
#include "meshes/SeaSurface.hh"
#include "meshes/Tholos.hh"
#include "meshes/Viaduct.hh"
#include "textures/GreekColumn.hh"
#include "textures/Marble.hh"
#include "textures/OldStone.hh"
#include "textures/RandomTexture.hh"
#include "textures/SeaFoam.hh"
#include "textures/Ground.hh"
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

//#undef _TV
//#define _TV(x) (x)

using namespace Algebra;
using namespace Tool;

void EmergenceScene::Init(Gfx::IGraphicLayer* gfxLayer,
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
#if ENABLE_BLUR_REFLECTION
	m_reflectionGaussianBlur.Init(gfxLayer, m_renderWidth, m_renderHeight, quad, shaderLoader, Gfx::TextureFormat::R11G11B10f, m_reflectionTextures[0]);
#endif // ENABLE_BLUR_REFRACTION
#if ENABLE_BLUR_REFRACTION
	m_refractionGaussianBlur.Init(gfxLayer, m_renderWidth, m_renderHeight, quad, shaderLoader, Gfx::TextureFormat::R11G11B10f, m_refractionTextures[0]);
#endif // ENABLE_BLUR_REFRACTION
	m_lightToolBox = lightToolBox;
	m_lightRig.shadowMaps = m_lightToolBox->shadowVarianceTextures;
	m_volumetricLighting = volumetricLighting;
	m_sky.Init(gfxLayer, quad, shaderLoader);

	m_foamData.init(192);

	Gfx::TextureID* textureIDs;
	m_baseSeaSurfaceShadingUniforms.init(MAX_UNIFORMS);

	LOAD_TEXTURE_SIMPLE(*textureLoader, m_randomTexture, RandomTexture, 256, 256);

	LOAD_MESH(*meshLoader, m_seaSurface, SeaSurface, VertexDataPNT);
	LOAD_SHADER(*shaderLoader, &m_seaSurfaceShading.shader, emergenceSeaSurfaceShader,
		"assets/shaders/seaSurface.vs",
		"assets/shaders/seaSurface_emergenceScene.fs");

	LOAD_MESH(*meshLoader, m_foamPatch, TinySeaPatch, VertexDataPNT);
	LOAD_SHADER(*shaderLoader, &m_foamPatchDummyMaterial.shader, emergencefoamPatchShader,
		"assets/shaders/seaSurface.vs",
		"assets/shaders/seaSurfaceFoam.fs");
	LOAD_TEXTURE(*textureLoader, textureIDs, SeaFoam, settings.TextureSizeAbout(512), settings.TextureSizeAbout(512));
	m_foamTexture = textureIDs[0];

	LOAD_SHADER(*shaderLoader, &m_marbleWallMaterial.shader, artemisionWallShader,
		"assets/shaders/pbr.vs",
		"assets/shaders/pbrWithShadows.fs");
	LOAD_SHADER(*shaderLoader, &m_marbleWallMaterial.shadowShader, artemisionWallShadow,
		"assets/shaders/pbr.vs",
		"assets/shaders/shadowMap.fs");
	LOAD_TEXTURE(*textureLoader, textureIDs, FineWhiteMarbleWall, settings.TextureSizeAbout(512), settings.TextureSizeAbout(512));
	m_marbleWallMaterial.SetDiffuseSpecularRoughnessNormalAndHeight(textureIDs);
	m_marbleWallMaterial.parallaxScale = FineWhiteMarbleWall::s_parallaxScale;

	LOAD_MESH(*meshLoader, m_obelisk, Obelisk, VertexDataPNTT);
	LOAD_MESH(*meshLoader, m_tholos, Tholos, VertexDataPNTT);
	LOAD_MESH(*meshLoader, m_fountain, Fountain, VertexDataPNTT);

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

	LOAD_MESH(*meshLoader, m_seagull, Seagull, VertexDataPNTT);
	LOAD_SHADER(*shaderLoader, &m_seagullMaterial.shader, seagulShader,
		"assets/shaders/unicolor.vs",
		"assets/shaders/unicolorWithShadows.fs");
	m_seagullMaterial
		.SetDiffuse(0.12f, 0.2f, 0.36f)
		.SetSpecular(0.04f)
		.SetRoughness(0.9f);

	m_city.Init(settings, meshLoader, shaderLoader, textureLoader);
}

#if DEBUG
Core::RigidTransform Orbital(float x, float z, float distance, const quaternion& orientation)
{
	Core::RigidTransform result =
	{
		{ x, 0.f, z },
		orientation
	};
	result.v += distance * Algebra::rotate(orientation, vector3f::uz);
	return result;
}
#endif

void EmergenceScene::Draw(long currentStoryTime, const Core::CameraDescription& cameraDesc, const Gfx::DrawArea& drawArea)
{
	// Participating medium
	Tool::ParticipatingMedium atmosphereMedium;
	atmosphereMedium.DecodeColorAndDensity(VAR(emergenceScene_atmosphereMediumColor)->get4());
	atmosphereMedium.scatteringIntensity = _TV(0.0001f);
	atmosphereMedium.forwardScatteringStrength = _TV(0.f);

	Tool::ParticipatingMedium underWaterMedium;
	underWaterMedium.DecodeColorAndDensity(VAR(emergenceScene_underwaterMediumColor)->get4((float)currentStoryTime));
	underWaterMedium.scatteringIntensity = _TV(0.006f);
	underWaterMedium.forwardScatteringStrength = _TV(0.8f);

	// The lights
	m_lightRig.ambientLight = VAR(emergenceScene_ambientLightColor)->getLightColor((float)currentStoryTime);
	const vector3f lightColors[] = {
		VAR(emergenceScene_obelisk_sunLightColor)->getLightColor((float)currentStoryTime),
		VAR(emergenceScene_waterBounceLightColor)->getLightColor((float)currentStoryTime),
		VAR(emergenceScene_artemision_sunLightColor)->getLightColor((float)currentStoryTime),
		VAR(emergenceScene_waterBounceLightColor)->getLightColor((float)currentStoryTime),
		VAR(emergenceScene_island_sunLightColor)->getLightColor((float)currentStoryTime),
	};

	const float finalVistaStep = Algebra::interpolate(_TV(100000), _TV(110000), currentStoryTime);
	const float lightCones[ARRAY_LEN(m_lightRig.lights)] = {
		DEG_TO_RAD * _TV(35.f), // ObeliskLight
		DEG_TO_RAD * _TV(30.f), // ObeliskBounceLight
		DEG_TO_RAD * _TV(30.f), // ArtemisionLight
		DEG_TO_RAD * _TV(145.f), // ArtemisionBounceLight
		Algebra::mix(DEG_TO_RAD * _TV(10.f), DEG_TO_RAD * _TV(45.f), finalVistaStep), // IslandLight
	};

	const int lightTypes[ARRAY_LEN(m_lightRig.lights)] = { 0 };

	const Core::RigidTransform lightTransforms[ARRAY_LEN(m_lightRig.lights)] = {
#if DEBUG
		// ObeliskLight
		Orbital(_TV(0.f), _TV(0.f),
			_TV(100.f),
			quaternion::rotation(_TV(0.5f) * PI, vector3f::uy) *
			quaternion::rotation(_TV(-0.7f) * PI, vector3f::ux)),

		// ObeliskBounceLight
		Orbital(_TV(0.f), _TV(0.f),
			_TV(96.f),
			quaternion::rotation(_TV(0.5f) * PI, vector3f::ux)),

		// ArtemisionLight
		Orbital(_TV(330.f), _TV(260.f),
			_TV(400.f),
			quaternion::rotation(_TV(0.25f) * PI, vector3f::uy) *
			quaternion::rotation(_TV(-0.8f) * PI, vector3f::ux)),

		// ArtemisionBounceLight
		Orbital(_TV(330.f), _TV(245.f),
			_TV(4.f),
			quaternion::rotation(_TV(0.f) * PI, vector3f::uy) *
			quaternion::rotation(_TV(0.5f) * PI, vector3f::ux)),

		// IslandLight
		Orbital(_TV(230.f) + _TV(0.5f)*cameraDesc.transform.v.x, _TV(180.f) + _TV(0.5f)*cameraDesc.transform.v.z,
			_TV(1024.f),
			quaternion::rotation(_TV(1.2f) * PI, vector3f::uy) *
			quaternion::rotation(_TV(-0.3f) * PI, vector3f::ux)),
#else
		// SIZE: Explicit version, written manually after evaluating
		// the part above in debug.
		{ { -58.8f, 80.9f, 0.f },		{ -0.63f, 0.321f, 0.63f, 0.321f } },
		{ { 0.f, -96.f, 0.f },			{ 0.7071f, 0.f, 0.f, 0.7071f } },
		{ { 101.2f, 235.1f, 31.2f  },	{ -0.8787f, 0.1183f, 0.3639f, 0.2855f } },
		{ { 330.f, -4.f, 245.f },		{ 0.7071f, 0.f, 0.f, 0.7071f } },
		{ { -123.8f + 0.5f * cameraDesc.transform.v.x,
			828.4f,
			-306.9f + 0.5f * cameraDesc.transform.v.z },
		{ 0.14f, 0.847f, 0.432f, -0.275f } },
#endif
	};

	float lightZNears[NUM_LIGHTS] = { _TV(32.f), _TV(32.f), _TV(192.f), _TV(1.f), _TV(600.f) };
	float lightZFars[NUM_LIGHTS] = { _TV(256.f), _TV(128.f), _TV(512.f), _TV(48.f), _TV(1300.f) };

	m_lightRig.SetLighting(lightTransforms, lightCones, lightTypes, lightColors);

	CreateScene(currentStoryTime);

	m_lightToolBox->RenderShadowMaps(*m_renderList, currentStoryTime, m_lightRig, lightZNears, lightZFars, _TV(2));

	FixedLightRig underWaterLightRig = m_lightRig;
	underWaterLightRig.ambientLight = vector3f::zero;
	underWaterLightRig.lights[0].type = 2;

	float zFar = _TV(1000.f);
	const Core::Camera camera = CreateCamera(cameraDesc, (float)drawArea.viewport.width / (float)drawArea.viewport.height, 1, zFar);

	const plane waterPlane = { { { vector3f::uy, 0.f } } };
	const bool aboveWater = (cameraDesc.transform.v.y > 0.f);

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

		// Sky.
		if (aboveWater)
		{
			m_sky.Draw(currentStoryTime, reflectionFB, reflectionCamera,
				VAR(skyFogColor)->getLightColor((float)currentStoryTime),
				VAR(skyColor)->getLightColor((float)currentStoryTime));
		}

		plane clippingPlane = waterPlane;
		clippingPlane.dist = _TV(-1.f);
		clippingPlane.normal.y *= (aboveWater ? 1.f : -1.f);
		applyMatrixToPlane(reflectionCamera.view, clippingPlane);

		Gfx::RasterTests reflectionTest(Gfx::FaceCulling::Front, Gfx::DepthFunction::Less, true, true);
		m_renderList->Render(currentStoryTime, reflectionFB, reflectionTest, reflectionCamera, &clippingPlane, lightRig, medium, false);
#if ENABLE_BLUR_REFLECTION
		m_reflectionGaussianBlur.Apply(m_reflectionTextures[0], _TV(3), _TV(10));
#endif // ENABLE_BLUR_REFLECTION
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
				VAR(skyFogColor)->getLightColor((float)currentStoryTime) * _TV(16.f),
				VAR(skyColor)->getLightColor((float)currentStoryTime) * _TV(16.f));
		}

		plane clippingPlane = waterPlane;
		clippingPlane.dist = _TV(-2.5f);
		clippingPlane.normal.y *= (aboveWater ? -1.f : 1.f);
		applyMatrixToPlane(refractionCamera.view, clippingPlane);

		Gfx::RasterTests refractionTest(Gfx::FaceCulling::Back, Gfx::DepthFunction::Less, true, true);
		m_renderList->Render(currentStoryTime, refractionFB, refractionTest, refractionCamera, &clippingPlane, lightRig, medium, false);
#if ENABLE_BLUR_REFRACTION
		m_refractionGaussianBlur.Apply(m_refractionTextures[0], _TV(2), _TV(2));
#endif // ENABLE_BLUR_REFRACTION
	}

	// Final composite
	{
		const FixedLightRig& lightRig = aboveWater ? m_lightRig : underWaterLightRig;
		const ParticipatingMedium& medium = aboveWater ? atmosphereMedium : underWaterMedium;

		const vector3f bgColor = medium.GetBackgroundColor(zFar);
		m_gfxLayer->Clear(drawArea.frameBuffer, bgColor.x, bgColor.y, bgColor.z, true);

		// Sky.
		if (aboveWater)
		{
			m_sky.Draw(currentStoryTime, drawArea, camera,
				VAR(skyFogColor)->getLightColor((float)currentStoryTime),
				VAR(skyColor)->getLightColor((float)currentStoryTime));
		}

		// The scene.
		m_renderList->Render(currentStoryTime, drawArea, Gfx::RasterTests::RegularDepthTest, camera, nullptr, lightRig, medium, false);

		// Sea surface.
		const long splashOffsetTime = currentStoryTime + _TV(-7500);
		{
			m_baseSeaSurfaceShadingUniforms.empty();
			Tool::ShaderHelper::ExposeFogInfo(m_baseSeaSurfaceShadingUniforms, medium.color, medium.density);
			Tool::ShaderHelper::ExposeTimeInfo(m_baseSeaSurfaceShadingUniforms, splashOffsetTime);
			m_baseSeaSurfaceShadingUniforms.add(Gfx::Uniform::Float1(U_ABOVEWATER, float(aboveWater)));
			m_baseSeaSurfaceShadingUniforms.add(Gfx::Uniform::Float4(U_WAVEPARAMETERS, _TV(0.2f), _TV(2.f), _TV(80.f), _TV(0.2f)));
			m_baseSeaSurfaceShadingUniforms.add(Gfx::Uniform::Sampler1(U_RANDOMTEXTURE, m_randomTexture[0]));
			m_baseSeaSurfaceShadingUniforms.add(Gfx::Uniform::Sampler1(U_REFLECTION, m_reflectionTextures[0]));
			m_baseSeaSurfaceShadingUniforms.add(Gfx::Uniform::Sampler1(U_REFLECTIONDEPTH, m_reflectionTextures[1]));
			m_baseSeaSurfaceShadingUniforms.add(Gfx::Uniform::Sampler1(U_REFRACTION, m_refractionTextures[0]));
			m_baseSeaSurfaceShadingUniforms.add(Gfx::Uniform::Sampler1(U_REFRACTIONDEPTH, m_refractionTextures[1]));

			const float lockStep = _TV(128.f);
			const matrix4 seaOrigin = matrix4::translation(
				Algebra::floor(cameraDesc.transform.v.x / lockStep) * lockStep,
				0.f,
				Algebra::floor(cameraDesc.transform.v.z / lockStep) * lockStep);
			m_seaSurfaceShading.uniforms.copyFrom(m_baseSeaSurfaceShadingUniforms);
			Tool::ShaderHelper::ExposeTransformationInfo(m_seaSurfaceShading.uniforms, camera, seaOrigin);
			Tool::ShaderHelper::ExposeLightInfo(m_seaSurfaceShading.uniforms, lightRig.ambientLight, lightRig.lights, lightRig.lightCameras, lightRig.shadowMaps, seaOrigin);

			Gfx::RasterTests dualFace(Gfx::FaceCulling::None, Gfx::DepthFunction::Less, true);
			m_gfxLayer->Draw(drawArea, dualFace, m_seaSurface->geometry, m_seaSurfaceShading);
		}

		// Foam (clear the renderlist, add foam patches, and do a new render).
		{
			m_renderList->size = 0;
			for (int i = 0; i < m_foamData.size; i += 3)
			{
				const vector3f foamPosition = { m_foamData[i], _TV(0.4f), m_foamData[i + 1] };
				matrix4 foamModel = matrix4::translation(foamPosition)
					.rotate(2.f * PI * Noise::Hash::get32f(i), vector3f::uy)
					.scale(m_foamData[i + 2]);

				const int index = m_renderList->Add(*m_foamPatch, m_foamPatchDummyMaterial, foamModel);
				Gfx::ShadingParameters& shading = m_renderList->shadingParametersForRendering[index];
				shading.blendingMode = Gfx::BlendingMode::Translucent;
				shading.uniforms.copyFrom(m_baseSeaSurfaceShadingUniforms);
				shading.uniforms.add(Gfx::Uniform::Sampler1(U_TEXTURE0, m_foamTexture));
			}

			Gfx::RasterTests depthReadOnly(Gfx::FaceCulling::None, Gfx::DepthFunction::Less, false);
			m_renderList->Render(splashOffsetTime, drawArea, depthReadOnly, camera, nullptr, lightRig, medium, false);
		}

		// Participating medium.
		m_volumetricLighting->Draw(currentStoryTime, drawArea, m_depthTexture, camera, lightRig,
			1.f, 100.f, medium, _TV(100.f), _TV(1));
	}
}

void EmergenceScene::CreateFoamLine(const vector2f& start,
									const vector2f& end,
									float size,
									int numberOfPatches)
{
	const int maxi = numberOfPatches - 1; // Assuming that the last patch is shared with the next line.
	for (int i = 0; i < maxi && size != 0.f; ++i)
	{
		const vector2f position = mix(start, end, float(i) / float(numberOfPatches - 1));
		m_foamData.add(position.x);
		m_foamData.add(position.y);
		m_foamData.add(size);
	}
}

void EmergenceScene::CreateScene(long currentStoryTime)
{
	m_renderList->size = 0;
	m_foamData.size = 0;

	const float altantisHeight = mix(_TV(-100.f), _TV(10.f),
			interpolate(_TV(30000), _TV(90000), currentStoryTime));
	const float enableConstructionsFoam = float(altantisHeight < _TV(2.f));

	{
		const Core::RigidTransform cityTransform =
			Core::RigidTransform::translation(_TV(350.f), altantisHeight, _TV(255.f)) *
			Core::RigidTransform::rotation(_TV(-106.f) * DEG_TO_RAD, vector3f::uy);

		// Island.
		m_renderList->Add(*m_islandFloor, m_floorMaterial, ComputeMatrix(
			cityTransform * Core::RigidTransform::translation(_TV(-750.f), 0.f, _TV(-750.f))));

		// Entire city.
		m_city.AddToScene(m_renderList, currentStoryTime, cityTransform);

		// Artemision columns foam.
		{
			// The position of the foam around the Artemision is hard coded
			// by applying cityTransform to these coordinates:
			// float x0 = 22.f;
			// float x1 = -22.f;
			// float y0 = -34.f;
			// float y1 = 67.f;
			const vector2f a = { 376.6f, 285.5f };
			const vector2f b = { 388.7f, 243.2f };
			const vector2f c = { 291.7f, 215.4f };
			const vector2f d = { 279.5f, 257.7f };
			const float columnFoamSize = _TV(0.9f) * interpolate(_TV(-85.f), _TV(-80.f), altantisHeight) * enableConstructionsFoam;
			CreateFoamLine(a, b, columnFoamSize, _TV(8));
			CreateFoamLine(c, d, columnFoamSize, _TV(8));
			CreateFoamLine(b, c, columnFoamSize, _TV(17));
			CreateFoamLine(d, a, columnFoamSize, _TV(17));
		}

		// Coast foam.
		{
			const vector2f a = { _TV(50.f), _TV(-105.f) };
			const vector2f b = { _TV(-50.f), _TV(10.f) };
			const vector2f c = { _TV(-55.f), _TV(85.f) };
			const float coastFoamSize = _TV(4.f) * interpolate(_TV(2.f), _TV(9.f), altantisHeight);
			CreateFoamLine(a, b, coastFoamSize, _TV(10));
			CreateFoamLine(b, c, coastFoamSize, _TV(6));
		}
	}

	// Obelisk of the emergence shots.
	if (altantisHeight < 0.f)
	{
		const float height = mix(_TV(-80.f), _TV(-1.5f),
			interpolate(_TV(0), _TV(40000), currentStoryTime));

		m_renderList->Add(*m_obelisk, m_marbleWallMaterial,
			matrix4::translation(_TV(0.f), height, _TV(0.f)));

		const float foamSize = _TV(1.75f) * interpolate(_TV(-24.f), _TV(-15.f), height) * enableConstructionsFoam;
		CreateFoamLine(vector2f::zero, vector2f::zero, foamSize, _TV(2));
	}

	// Tholos of the final shots.
	{
		const float height = altantisHeight + _TV(-0.125f);

		m_renderList->Add(*m_tholos, m_marbleWallMaterial,
			matrix4::translation(_TV(-14.f), height, _TV(12.f)));
		m_renderList->Add(*m_fountain, m_marbleWallMaterial,
			matrix4::translation(_TV(-14.f), height + _TV(1.25f), _TV(12.f)));
	}

	// Seagulls
	{
		const bool firstSeagull = (currentStoryTime < 100000);

		Noise::Rand rand;
		const float direction = _TV(1.f);
		const float speed = _TV(0.00018f);
		const float circlingRadius = _TV(100.f);

		int numberOfSeagulls = (firstSeagull ? 1 : _TV(16));
		for (int i = 0; i < numberOfSeagulls; ++i)
		{
			const float angle = direction * speed * currentStoryTime + (firstSeagull ? 2.f * PI * _TV(0.535f) : rand.fgen(0.f, 2.f * PI));
			const vector3f p1 = { _TV(310.f), _TV(64.f), _TV(200.f) };
			const vector3f p2 = { rand.fgen(_TV(80.f), _TV(200.f)), rand.fgen(_TV(128.f), _TV(48.f)), rand.fgen(_TV(80.f), _TV(200.f)) };
			const vector3f& p = (firstSeagull ? p1 : p2);
			const Core::RigidTransform transform = {
				{ p.x + circlingRadius * msys_sinf(angle), p.y, p.z + circlingRadius * msys_cosf(angle) },
				quaternion::rotation(angle - direction * PI / 2.f, vector3f::uy)
			};
			m_renderList->Add(*m_seagull, m_seagullMaterial, ComputeMatrix(transform));
		}
	}
}
