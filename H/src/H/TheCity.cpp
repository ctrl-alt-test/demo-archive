#include "TheCity.hh"

#include "engine/algebra/Utils.hh"
#include "engine/algebra/Vector3.hxx"
#include "engine/core/RigidTransform.hxx"
#include "engine/core/Settings.hh"
#include "meshes/Artemision.hh"
#include "meshes/Fence.hh"
#include "meshes/Fountain.hh"
#include "meshes/Houses.hh"
#include "meshes/Obelisk.hh"
#include "meshes/Parthenon.hh"
#include "meshes/Road.hh"
#include "meshes/Stairs.hh"
#include "meshes/Statue.hh"
#include "meshes/Tholos.hh"
#include "meshes/Trident.hh"
#include "meshes/Viaduct.hh"
#include "textures/Cobbles.hh"
#include "textures/GreekColumn.hh"
#include "textures/Ground.hh"
#include "textures/Marble.hh"
#include "textures/OldStone.hh"
#include "tool/mesh/VertexDataPNTT.hh"
#include "tool/MeshLoader.hh"
#include "tool/RenderList.hh"
#include "tool/ShaderLoader.hh"
#include "tool/TextureLoader.hh"
#include "tweakval/tweakval.h"

#undef _TV
#define _TV(x) (x)

using namespace Algebra;
using namespace Tool;

void TheCity::Init(const Core::Settings& settings,
				   MeshLoader* meshLoader,
				   ShaderLoader* shaderLoader,
				   TextureLoader* textureLoader)
{
	Gfx::TextureID* textureIDs;

	LOAD_SHADER(*shaderLoader, &m_roadMaterial.shader, roadShader,
		"assets/shaders/pbr.vs",
		"assets/shaders/pbrWithShadows.fs");
	LOAD_SHADER(*shaderLoader, &m_roadMaterial.shadowShader, roadShadow,
		"assets/shaders/pbr.vs",
		"assets/shaders/shadowMap.fs");
	LOAD_TEXTURE(*textureLoader, textureIDs, Cobbles, settings.TextureSizeAbout(512), settings.TextureSizeAbout(512));
	m_roadMaterial.SetDiffuseSpecularRoughnessNormalAndHeight(textureIDs);
	m_roadMaterial.parallaxScale = Ground::s_parallaxScale * _TV(0.25f); // FIXME: check the texture parallax.

	LOAD_MESH(*meshLoader, m_parthenon, Parthenon, VertexDataPNTT);
	LOAD_MESH(*meshLoader, m_parthenonColumns, ParthenonColumns, VertexDataPNTT);
	LOAD_SHADER(*shaderLoader, &m_doricColumnMaterial.shader, doricColumnShader,
		"assets/shaders/pbr.vs",
		"assets/shaders/pbrWithShadows.fs");
	LOAD_SHADER(*shaderLoader, &m_doricColumnMaterial.shadowShader, doricColumnShadow,
		"assets/shaders/pbr.vs",
		"assets/shaders/shadowMap.fs");
	LOAD_TEXTURE(*textureLoader, textureIDs, DoricColumn, settings.TextureSizeAbout(512), settings.TextureSizeAbout(512));
	m_doricColumnMaterial.SetDiffuseSpecularRoughnessNormalAndHeight(textureIDs);
	m_doricColumnMaterial.parallaxScale = DoricColumn::s_parallaxScale;

	LOAD_MESH(*meshLoader, m_fence, Fence, VertexDataPNTT);
	LOAD_MESH(*meshLoader, m_fountain, Fountain, VertexDataPNTT);
	LOAD_MESH(*meshLoader, m_fountainStatues, FountainStatues, VertexDataPNTT);
	LOAD_MESH(*meshLoader, m_houses, Houses, VertexDataPNTT);
	LOAD_MESH(*meshLoader, m_obelisk, Obelisk, VertexDataPNTT);
	LOAD_MESH(*meshLoader, m_road, CityRoad, VertexDataPNTT);
	LOAD_MESH(*meshLoader, m_stairs, Stairs, VertexDataPNTT);
	LOAD_MESH(*meshLoader, m_statue, Statue, VertexDataPNTT);
	LOAD_MESH(*meshLoader, m_tholos, Tholos, VertexDataPNTT);
	LOAD_MESH(*meshLoader, m_trident, MiniTrident, VertexDataPNTT);
	LOAD_MESH(*meshLoader, m_viaduct, Viaduct, VertexDataPNTT);

	LOAD_SHADER(*shaderLoader, &m_goldMaterial.shader, goldShader,
		"assets/shaders/pbr.vs",
		"assets/shaders/pbrWithShadows.fs");
	LOAD_SHADER(*shaderLoader, &m_goldMaterial.shadowShader, goldShadow,
		"assets/shaders/pbr.vs",
		"assets/shaders/shadowMap.fs");
	LOAD_TEXTURE(*textureLoader, textureIDs, Gold, settings.TextureSizeAbout(512), settings.TextureSizeAbout(512));
	m_goldMaterial.SetDiffuseSpecularRoughnessNormalAndHeight(textureIDs);
	m_goldMaterial.parallaxScale = Gold::s_parallaxScale;

	LOAD_MESH(*meshLoader, m_artemision, Artemision, VertexDataPNTT);
	LOAD_SHADER(*shaderLoader, &m_marbleWallMaterial.shader, artemisionWallShader,
		"assets/shaders/pbr.vs",
		"assets/shaders/pbrWithShadows.fs");
	LOAD_SHADER(*shaderLoader, &m_marbleWallMaterial.shadowShader, artemisionWallShadow,
		"assets/shaders/pbr.vs",
		"assets/shaders/shadowMap.fs");
	LOAD_TEXTURE(*textureLoader, textureIDs, FineWhiteMarbleWall, settings.TextureSizeAbout(512), settings.TextureSizeAbout(512));
	m_marbleWallMaterial.SetDiffuseSpecularRoughnessNormalAndHeight(textureIDs);
	m_marbleWallMaterial.parallaxScale = FineWhiteMarbleWall::s_parallaxScale;

	LOAD_MESH(*meshLoader, m_artemisionColumns, ArtemisionColumns, VertexDataPNTT);
	LOAD_SHADER(*shaderLoader, &m_ionicColumnMaterial.shader, artemisionColumnShader,
		"assets/shaders/pbr.vs",
		"assets/shaders/pbrWithIncandescentOrichalcum.fs");
	LOAD_SHADER(*shaderLoader, &m_ionicColumnMaterial.shadowShader, artemisionColumnShadow,
		"assets/shaders/pbr.vs",
		"assets/shaders/shadowMap.fs");
	LOAD_TEXTURE(*textureLoader, textureIDs, IonicColumn, settings.TextureSizeAbout(1024), settings.TextureSizeAbout(1024));
	m_ionicColumnMaterial.SetDiffuseSpecularRoughnessNormalAndHeight(textureIDs);
	m_ionicColumnMaterial.parallaxScale = IonicColumn::s_parallaxScale * _TV(0.f); // Columns look bad with POM, so it's disabled.

	LOAD_SHADER(*shaderLoader, &m_stonesMaterial.shader, oldStoneShader,
		"assets/shaders/pbr.vs",
		"assets/shaders/pbrWithShadows.fs");
	LOAD_SHADER(*shaderLoader, &m_stonesMaterial.shadowShader, oldStoneShadow,
		"assets/shaders/pbr.vs",
		"assets/shaders/shadowMap.fs");
	LOAD_TEXTURE(*textureLoader, textureIDs, OldStone, settings.TextureSizeAbout(512), settings.TextureSizeAbout(512));
	m_stonesMaterial.SetDiffuseSpecularRoughnessNormalAndHeight(textureIDs);
	m_stonesMaterial.parallaxScale = OldStone::s_parallaxScale;
}

void TheCity::CreateCityBlock(RenderList* renderList,
							  const Core::RigidTransform& transform,
							  Noise::Rand& rand,
							  bool innerCircle,
							  bool forceParthenon)
{
	if (forceParthenon || rand.boolean(_TV(0.01f)))
	{
		Core::RigidTransform parthenonTransform = transform *
			(forceParthenon ?
			Core::RigidTransform::translation(_TV(20.f), _TV(0.f), _TV(-40.f)) *
			Core::RigidTransform::rotation(_TV(45.f) * DEG_TO_RAD, vector3f::uy)
			: Core::RigidTransform::translation(_TV(30.f), _TV(0.f), _TV(-40.f)));

		renderList->Add(*m_parthenon, m_marbleWallMaterial, ComputeMatrix(parthenonTransform));
		renderList->Add(*m_parthenonColumns, m_doricColumnMaterial, ComputeMatrix(parthenonTransform));
	}
	else
	{
		renderList->Add(*m_houses, m_stonesMaterial, ComputeMatrix(transform));

		// The !boolean is done on purpose: I want a low probability,
		// but with one Tholos in a specific spot.
		if (innerCircle && !rand.boolean(_TV(0.9f)))
		{
			renderList->Add(*m_tholos, m_marbleWallMaterial, ComputeMatrix(
				transform * Core::RigidTransform::translation(90.f, 0.f, 72.f)));
		}

		if (rand.boolean(_TV(0.4f)))
		{
			renderList->Add(*m_fence, m_stonesMaterial, ComputeMatrix(
				transform * Core::RigidTransform::translation(60.f, 0.7f, 6.f)));
			renderList->Add(*m_fence, m_stonesMaterial, ComputeMatrix(
				transform * Core::RigidTransform::translation(60.f, 0.7f, 10.f)));
		}

		if (rand.boolean(_TV(0.6f)))
		{
			renderList->Add(*m_fountain, m_marbleWallMaterial, ComputeMatrix(
				transform * Core::RigidTransform::translation(80.f, 0.f, 20.f)
				.rotate(PI * _TV(1.f), vector3f::uy))); // Hide the geometry seam.
			renderList->Add(*m_fountainStatues, m_marbleWallMaterial, ComputeMatrix(
				transform * Core::RigidTransform::translation(80.f, 2.55f, 20.f)));
		}

		if (rand.boolean(_TV(1.f)))
		{
			renderList->Add(*m_obelisk, m_marbleWallMaterial, ComputeMatrix(
				transform * Core::RigidTransform::translation(_TV(103.f), _TV(0.f), _TV(13.f))));
		}
	}
}

void TheCity::AddToScene(RenderList* renderList,
						 long currentTime,
						 const Core::RigidTransform& root)
{
	// Artemision
	{
		const matrix4 model = ComputeMatrix(root *
			Core::RigidTransform::translation(_TV(33.f), _TV(61.5f), _TV(80.f)) *
			Core::RigidTransform::rotation(_TV(1.f) * PI, vector3f::uy));
		renderList->Add(*m_artemision, m_marbleWallMaterial, model);
		renderList->Add(*m_artemisionColumns, m_ionicColumnMaterial, model);

		// Statues
		renderList->Add(*m_statue, m_goldMaterial, ComputeMatrix(root *
			Core::RigidTransform::translation(_TV(-3.f), _TV(85.75f), _TV(68.75f)) *
			Core::RigidTransform::rotation(_TV(90.f) * DEG_TO_RAD, vector3f::uy)));

		// Tridents on top of Artemision
		renderList->Add(*m_trident, m_goldMaterial, ComputeMatrix(root *
			Core::RigidTransform::translation(_TV(-4.5f), _TV(92.25f), _TV(69.375f)) *
			Core::RigidTransform::rotation(_TV(40.f) * DEG_TO_RAD, vector3f::uz)));

		renderList->Add(*m_trident, m_goldMaterial, ComputeMatrix(root *
			Core::RigidTransform::translation(_TV(-2.5f), _TV(92.25f), _TV(69.25f)) *
			Core::RigidTransform::rotation(_TV(-40.f) * DEG_TO_RAD, vector3f::uz)));

		// Stairs
		renderList->Add(*m_stairs, m_marbleWallMaterial, ComputeMatrix(root *
			Core::RigidTransform::translation(_TV(-40.f), _TV(0.25f), _TV(190.f)) *
			Core::RigidTransform::rotation(_TV(-15.f) * DEG_TO_RAD, vector3f::uy)));
	}

	// Lower city
	{
		const Core::RigidTransform lowerRoot = root * Core::RigidTransform::translation(0, -0.25f, 0);

		Noise::Rand rand;
		const int numBlocksInnerCircle = _TV(12);
		for (int i = 0; i < numBlocksInnerCircle; ++i)
		{
			float angle = _TV(0.21f) + (i * 2.f / numBlocksInnerCircle) * PI;
			float distanceFromCenter = _TV(200.f);
			const vector3f v = { distanceFromCenter, 0.f, 0.f };
			Core::RigidTransform transform = lowerRoot *
				Core::RigidTransform::rotation(angle, vector3f::uy);
			transform.translate(Algebra::rotate(transform.q, v));
			CreateCityBlock(renderList, transform, rand, true, false);
		}

		const int numBlocksOuterCircle = _TV(18);
		for (int i = 0; i < numBlocksOuterCircle; ++i)
		{
			float angle = _TV(0.f) + (i * 2.f / numBlocksOuterCircle) * PI;
			float distanceFromCenter = _TV(310.f);
			const vector3f v = { distanceFromCenter, 0.f, 0.f };
			Core::RigidTransform transform = lowerRoot *
				Core::RigidTransform::rotation(angle, vector3f::uy);
			transform.translate(Algebra::rotate(transform.q, v));
			CreateCityBlock(renderList, transform, rand, false, (i == _TV(12)));
		}

		// Bridges
		const int numBridges = 7;
		for (int i = 0; i < numBridges; ++i)
		{
			const float angle = 2.f * PI * float(i) / float(numBridges);
			const float distanceFromCenter = _TV(550.f);
			const vector3f v = { 0.f, 0.f, distanceFromCenter };
			Core::RigidTransform transform = lowerRoot *
				Core::RigidTransform::rotation(angle, vector3f::uy);
			transform.translate(Algebra::rotate(transform.q, v));
			renderList->Add(*m_viaduct, m_stonesMaterial, ComputeMatrix(transform));
		}

		renderList->Add(*m_road, m_roadMaterial, ComputeMatrix(
			root * Core::RigidTransform::translation(0, -0.1f, 0)));
	}
}
