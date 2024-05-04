#include "CommonMaterial.hh"

#include "engine/container/Array.hxx"
#include "gfx/ShadingParameters.hh"
#include "tool/FixedLightToolBox.hh"
#include "tool/ParticipatingMedium.hh"
#include "tool/ShaderHelper.hh"
#include <cassert>

using namespace Tool;
using namespace Gfx;

CommonMaterial& CommonMaterial::SetDiffuseSpecularRoughnessNormalAndHeight(const Gfx::TextureID* textures)
{
	SetDiffuse(textures[0]);
	SetSpecularAndRoughness(textures[1]);
	SetNormalAndHeight(textures[2]);
	return *this;
}

void CommonMaterial::SetShaderParameters(ShadingParameters& shadingParameters,
										 const Algebra::matrix4& model,
										 long currentTime,
										 const Core::Camera& camera,
										 const Tool::FixedLightRig& lightRig,
										 const Tool::ParticipatingMedium& medium,
										 bool isShadowPass,
										 bool isDeferredPass) const
{
	PresetShaderParameters(shadingParameters, isShadowPass);

	if (isDeferredPass)
	{
		Tool::ShaderHelper::ExposeTransformationInfoForFullScreen(shadingParameters.uniforms, camera);
	}
	else
	{
		Tool::ShaderHelper::ExposeTransformationInfo(shadingParameters.uniforms, camera, model);
	}
	Tool::ShaderHelper::ExposeTimeInfo(shadingParameters.uniforms, currentTime);
	Tool::ShaderHelper::ExposeLightInfo(shadingParameters.uniforms, lightRig.ambientLight, lightRig.lights, lightRig.lightCameras, lightRig.shadowMaps, model);
	Tool::ShaderHelper::ExposeFogInfo(shadingParameters.uniforms, medium.color, medium.density);
}

void CommonMaterial::PresetShaderParameters(ShadingParameters& shadingParameters,
											bool isShadowPass) const
{
	shadingParameters.shader = (isShadowPass ? shadowShader : shader);
	assert(shader != Gfx::ShaderID::InvalidID);

	shadingParameters.uniforms.empty();

	if (hasEmissiveTexture)
	{
		assert(emissiveTexture != TextureID::InvalidID);
		shadingParameters.uniforms.add(Uniform::Sampler1(U_EMISSIVETEXTURE, emissiveTexture));
	}
	else
	{
		assert(emissiveColor[0] >= 0.f && emissiveColor[1] >= 0.f && emissiveColor[2] >= 0.f);
		shadingParameters.uniforms.add(Uniform::Float3(U_EMISSIVECOLOR, emissiveColor[0], emissiveColor[1], emissiveColor[2]));
	}

	if (hasDiffuseTexture)
	{
		assert(diffuseTexture != TextureID::InvalidID);
		shadingParameters.uniforms.add(Uniform::Sampler1(U_DIFFUSETEXTURE, diffuseTexture));
	}
	else
	{
		assert(diffuseColor[0] >= 0.f && diffuseColor[1] >= 0.f && diffuseColor[2] >= 0.f);
		shadingParameters.uniforms.add(Uniform::Float3(U_DIFFUSECOLOR, diffuseColor[0], diffuseColor[1], diffuseColor[2]));
	}

	if (hasSpecularTexture && hasRoughnessTexture)
	{
		assert(specularAndRoughnessTexture != TextureID::InvalidID);
		shadingParameters.uniforms.add(Uniform::Sampler1(U_SPECULARANDROUGHNESSTEXTURE, specularAndRoughnessTexture));
	}
	else if (hasSpecularTexture)
	{
		assert(specularTexture != TextureID::InvalidID);
		shadingParameters.uniforms.add(Uniform::Sampler1(U_SPECULARTEXTURE, specularTexture));

		assert(roughness >= 0.f && roughness <= 1.f);
		shadingParameters.uniforms.add(Uniform::Float1(U_ROUGHNESS, roughness));
	}
	else
	{
		assert(specularColor[0] >= 0.f && specularColor[1] >= 0.f && specularColor[2] >= 0.f &&
			   specularColor[0] <= 1.f && specularColor[1] <= 1.f && specularColor[2] <= 1.f);
		shadingParameters.uniforms.add(Uniform::Float3(U_SPECULARCOLOR, specularColor[0], specularColor[1], specularColor[2]));

		assert(roughness >= 0.f && roughness <= 1.f);
		shadingParameters.uniforms.add(Uniform::Float1(U_ROUGHNESS, roughness));
	}

	if (hasNormalTexture && hasHeightTexture)
	{
		assert(normalAndHeightTexture != TextureID::InvalidID);
		shadingParameters.uniforms.add(Uniform::Sampler1(U_NORMALANDHEIGHTTEXTURE, normalAndHeightTexture));

		// In parallax mapping, we pretend the surface is not flat, and use a
		// height texture to represent the relief. The values in that texture
		// are in [0 .. 1]; but what height is 1?
		//
		// parallaxScale is a value in [-1 .. 1], where 1 is the width of the
		// texture. So for example is the texture represents a 1x1 square
		// meter, a parallaxScale of 1 would mean the heighest point is 1m
		// above the surface. A parallaxScale of 0.1 would mean 10cm. A
		// negative value means the relief is carved (below) in the surface.
		//
		// The height stays relative to the area: if the same texture is used
		// for two surfaces of different sizes, the height is also going to
		// be different.

		assert(parallaxScale >= -1.f && parallaxScale <= 1.f);
		shadingParameters.uniforms.add(Uniform::Float1(U_PARALLAXSCALE, parallaxScale));
	}
	else if (hasNormalTexture)
	{
		assert(normalTexture != TextureID::InvalidID);
		shadingParameters.uniforms.add(Uniform::Sampler1(U_NORMALTEXTURE, normalTexture));
	}

	if (hasOpacity)
	{
		shadingParameters.uniforms.add(Uniform::Float1(U_OPACITY, opacity));
	}
}
