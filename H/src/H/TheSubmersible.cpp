#include "TheSubmersible.hh"

#include "engine/algebra/Quaternion.hxx"
#include "engine/algebra/Utils.hh"
#include "engine/algebra/Vector3.hxx"
#include "engine/container/Utils.hh"
#include "engine/core/RigidTransform.hh"
#include "engine/core/Settings.hh"
#include "meshes/Submersible.hh"
#include "textures/ResearchVessel.hh"
#include "tool/mesh/VertexDataPNT.hh"
#include "tool/mesh/VertexDataPNTT.hh"
#include "tool/MeshLoader.hh"
#include "tool/RenderList.hh"
#include "tool/ShaderHelper.hh"
#include "tool/ShaderLoader.hh"
#include "tool/TextureLoader.hh"
#include "tweakval/tweakval.h"

using namespace Algebra;
using namespace Tool;

void TheSubmersible::Init(const Core::Settings& settings,
						  MeshLoader* meshLoader,
						  ShaderLoader* shaderLoader,
						  TextureLoader* textureLoader)
{
	Gfx::TextureID* textureIDs;
	LOAD_MESH(*meshLoader, body, Submersible, VertexDataPNTT);
	LOAD_MESH(*meshLoader, frame, SubmersibleFrame, VertexDataPNT);
	LOAD_MESH(*meshLoader, spots, SubmersibleSpots, VertexDataPNTT);

	LOAD_SHADER(*shaderLoader, &bodyMaterial.shader, submersibleShader,
		"assets/shaders/pbr.vs",
		"assets/shaders/pbrWithShadows.fs");
	LOAD_SHADER(*shaderLoader, &bodyMaterial.shadowShader, submersibleShadow,
		"assets/shaders/pbr.vs",
		"assets/shaders/shadowMap.fs");
	LOAD_TEXTURE(*textureLoader, textureIDs, SubmersibleBodyTex, settings.TextureSizeAbout(512), settings.TextureSizeAbout(512));
	bodyMaterial.SetDiffuseSpecularRoughnessNormalAndHeight(textureIDs);
	bodyMaterial.parallaxScale = SubmersibleBodyTex::s_parallaxScale;

	LOAD_SHADER(*shaderLoader, &frameMaterial.shader, submersibleFrameShader,
		"assets/shaders/unicolor.vs",
		"assets/shaders/unicolorWithShadows.fs");
	frameMaterial
		.SetSpecular(0.5f, 0.5f, 0.5f)
		.SetRoughness(0.8f);

	LOAD_SHADER(*shaderLoader, &spotsMaterial.shader, submersibleSpotsShader,
		"assets/shaders/pbr.vs",
		"assets/shaders/pbrWithEmissive.fs");
	LOAD_TEXTURE(*textureLoader, textureIDs, SubmersibleSpotTex, settings.TextureSizeAbout(128), settings.TextureSizeAbout(128));
	spotsMaterial
		.SetDiffuseSpecularRoughnessNormalAndHeight(textureIDs)
		.SetEmissive(textureIDs[3]);
	spotsMaterial.parallaxScale = SubmersibleSpotTex::s_parallaxScale;
}

void TheSubmersible::AddToScene(RenderList* renderList,
								long /*currentTime*/,
								const Algebra::matrix4& modelMatrix,
								const Algebra::vector3f& spotLightColor)
{
	renderList->Add(*body, bodyMaterial, modelMatrix);
	renderList->Add(*frame, frameMaterial, modelMatrix);
	int index = renderList->Add(*spots, spotsMaterial, modelMatrix);
	renderList->shadingParametersForRendering[index].uniforms.add(Gfx::Uniform::Float3(U_EMISSIVEMULTIPLIER, spotLightColor.x, spotLightColor.y, spotLightColor.z));
}

void TheSubmersible::GetLights(float* cones, int* types, Core::RigidTransform* transforms)
{
	const float spotLightCones[] = {
		DEG_TO_RAD * _TV(50.f),
		DEG_TO_RAD * _TV(60.f),
		DEG_TO_RAD * _TV(60.f),
	};
	
	const Core::RigidTransform spotLightTransforms[] = {
		{ { _TV(0.f), _TV(-1.48f), _TV(3.15f), },
		quaternion::rotation(_TV(1.05f) * PI, vector3f::ux), },
		{ { _TV(-1.7f), _TV(0.45f), _TV(2.2f), },
		quaternion::rotation(_TV(0.02f) * PI, vector3f::uy) *
		quaternion::rotation(_TV(1.1f) * PI, vector3f::ux), },
		{ { _TV(1.7f), _TV(0.45f), _TV(2.2f), },
		quaternion::rotation(_TV(-0.02f) * PI, vector3f::uy) *
		quaternion::rotation(_TV(1.1f) * PI, vector3f::ux), },
	};

	for (unsigned int i = 0; i < ARRAY_LEN(spotLightCones); ++i)
	{
		if (cones != nullptr)
		{
			cones[i] = spotLightCones[i];
		}
		if (types != nullptr)
		{
			types[i] = 1;
		}
		if (transforms != nullptr)
		{
			transforms[i] = spotLightTransforms[i];
		}
	}
}
