#ifndef COMMON_MATERIAL_HH
#define COMMON_MATERIAL_HH

#include "engine/algebra/Vector3.hh"
#include "gfx/ResourceID.hh"
#include <cassert>
#include <cstring>

namespace Algebra
{
	struct matrix4;
}

namespace Gfx
{
	struct ShadingParameters;
}

namespace Core
{
	struct Camera;
	struct Light;
}

namespace Tool
{
	struct FixedLightRig;
	struct ParticipatingMedium;

	/// <summary>
	/// A high level material meant to cover the most common, say 80%,
	/// cases.
	/// It is more rigid an not easily extensible, but easy to use. For
	/// the remaining 20%, the low level interface can still be used
	/// directly.
	/// </summary>
	struct CommonMaterial
	{
		CommonMaterial()
		{
			memset(this, 0, sizeof(CommonMaterial));
			shader = Gfx::ShaderID::InvalidID;
			shadowShader = Gfx::ShaderID::InvalidID;
		}

		// Emissive
		union
		{
			float				emissiveColor[3];
			Gfx::TextureID		emissiveTexture;
		};

		inline CommonMaterial& SetEmissive(float r, float g, float b)
		{
			assert(r >= 0.f && g >= 0.f && b >= 0.f);
			emissiveColor[0] = r;
			emissiveColor[1] = g;
			emissiveColor[2] = b;
			hasEmissiveTexture = false;
			return *this;
		}

		inline CommonMaterial& SetEmissive(const Gfx::TextureID& texture)
		{
			emissiveTexture = texture;
			hasEmissiveTexture = true;
			return *this;
		}

		// Diffuse
		union
		{
			float				diffuseColor[3];
			Gfx::TextureID		diffuseTexture;
		};

		inline CommonMaterial& SetDiffuse(float r, float g, float b)
		{
			assert(r >= 0.f && r <= 1.f &&
				   g >= 0.f && g <= 1.f &&
				   b >= 0.f && b <= 1.f);
			diffuseColor[0] = r;
			diffuseColor[1] = g;
			diffuseColor[2] = b;
			hasDiffuseTexture = false;
			return *this;
		}

		inline CommonMaterial& SetDiffuse(const Gfx::TextureID& texture)
		{
			diffuseTexture = texture;
			hasDiffuseTexture = true;
			return *this;
		}

		// Specular and roughness
		union
		{
			struct
			{
				union
				{
					float			specularColor[3];
					Gfx::TextureID	specularTexture;
				};
				float			roughness;
			};
			Gfx::TextureID		specularAndRoughnessTexture;
		};

		inline CommonMaterial& SetSpecular(float f0)
		{
			assert(f0 >= 0.f && f0 <= 1.f);
			specularColor[0] = f0;
			specularColor[1] = f0;
			specularColor[2] = f0;
			hasSpecularTexture = false;
			hasRoughnessTexture = false;
			return *this;
		}

		inline CommonMaterial& SetSpecular(float f0r, float f0g, float f0b)
		{
			assert(f0r >= 0.f && f0r <= 1.f &&
				   f0g >= 0.f && f0g <= 1.f &&
				   f0b >= 0.f && f0b <= 1.f);
			specularColor[0] = f0r;
			specularColor[1] = f0g;
			specularColor[2] = f0b;
			hasSpecularTexture = false;
			hasRoughnessTexture = false;
			return *this;
		}

		inline CommonMaterial& SetSpecular(const Gfx::TextureID& texture)
		{
			specularTexture = texture;
			hasSpecularTexture = true;
			hasRoughnessTexture = false;
			return *this;
		}

		inline CommonMaterial& SetRoughness(float _roughness)
		{
			assert(_roughness >= 0.f && _roughness <= 1.f);
			roughness = _roughness;
			hasRoughnessTexture = false;
			return *this;
		}

		inline CommonMaterial& SetSpecularAndRoughness(const Gfx::TextureID& texture)
		{
			specularAndRoughnessTexture = texture;
			hasSpecularTexture = true;
			hasRoughnessTexture = true;
			return *this;
		}

		// Normal and height
		union
		{
			Gfx::TextureID		normalTexture;
			struct
			{
				Gfx::TextureID	normalAndHeightTexture;
				float			parallaxScale;
			};
		};

		inline CommonMaterial& SetNormalAndHeight(const Gfx::TextureID& texture)
		{
			normalAndHeightTexture = texture;
			hasNormalTexture = true;
			hasHeightTexture = true;
			return *this;
		}

		// Opacity
		float					opacity;

		inline CommonMaterial& SetOpacity(float _opacity)
		{
			assert(_opacity >= 0.f && _opacity <= 1.f);
			opacity = _opacity;
			hasOpacity = true;
			return *this;
		}

		unsigned short int		hasEmissiveTexture:1;
		unsigned short int		hasDiffuseTexture:1;
		unsigned short int		hasSpecularTexture:1;
		unsigned short int		hasRoughnessTexture:1;
		unsigned short int		hasNormalTexture:1;
		unsigned short int		hasHeightTexture:1;
		unsigned short int		hasOpacity:1;

		Gfx::ShaderID			shader;
		Gfx::ShaderID			shadowShader;

		/// <summary>
		/// A case common enough to justify its own function.
		/// Sets the material assuming an array of 3 textures, that
		/// contain the diffuse, specular and roughness, normal and
		/// height.
		/// </summary>
		CommonMaterial& SetDiffuseSpecularRoughnessNormalAndHeight(const Gfx::TextureID* textures);

		void SetShaderParameters(Gfx::ShadingParameters& shadingParameters,
								 const Algebra::matrix4& model,
								 long currentTime,
								 const Core::Camera& camera,
								 const Tool::FixedLightRig& lightRig,
								 const Tool::ParticipatingMedium& medium,
								 bool isShadowPass,
								 bool isDeferredPass = false) const;
		void PresetShaderParameters(Gfx::ShadingParameters& shadingParameters,
									bool isShadowPass) const;
	};
}

#endif // COMMON_MATERIAL_HH
