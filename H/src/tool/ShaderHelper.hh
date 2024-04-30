#ifndef SHADER_HELPER_HH
#define SHADER_HELPER_HH

#include "ShaderUniformNames.hh"
#include "engine/algebra/Vector3.hh"
#include "gfx/Uniform.hh"

namespace Algebra
{
	struct matrix4;
}

namespace Container
{
	template<typename T>
	class Array;
}

namespace Core
{
	struct Camera;
	struct Light;
}

namespace Tool
{
	extern const char* lightPositionParamNames[];
	extern const char* lightDirectionParamNames[];
	extern const char* lightColorParamNames[];
	extern const char* lightConeCosParamNames[];
	extern const char* lightTypeParamNames[];
	extern const char* shadowMapParamNames[];
	extern const char* modelLightProjectionParamNames[];

#define NUM_LIGHTS 5

	class ShaderHelper
	{
	public:
		static Gfx::Uniform		CreateMatrixShaderParam(const char* name, const Algebra::matrix4& matrix);
		static void				ExposeLightInfo(Container::Array<Gfx::Uniform>& uniforms,
												const Algebra::vector3f& ambientLight,
												const Core::Light* lights,
												const Core::Camera* lightCameras,
												const Gfx::TextureID* shadowMaps,
												const Algebra::matrix4& model);
		static void				ExposeFogInfo(Container::Array<Gfx::Uniform>& uniforms,
											  const Algebra::vector3f& mediumColor,
											  float mediumDensity);
		static void				ExposeModelViewProjectionInfo(Container::Array<Gfx::Uniform>& uniforms,
															  const Algebra::matrix4& modelViewProjection,
															  const Algebra::matrix4& inverseModelViewProjection);
		static void				ExposeTransformationInfo(Container::Array<Gfx::Uniform>& uniforms,
														 const Core::Camera& camera,
														 const Algebra::matrix4& model);
		static void				ExposeTransformationInfoForFullScreen(Container::Array<Gfx::Uniform>& uniforms,
																	  const Core::Camera& camera);
		static void				ExposeFullScreenBlitTransform(Container::Array<Gfx::Uniform>& uniforms);

		static void				ExposeResolutionInfo(Container::Array<Gfx::Uniform>& uniforms,
													 int bufferWidth,
													 int bufferHeight);
		static void				ExposeTimeInfo(Container::Array<Gfx::Uniform>& uniforms,
											   long time);
	};
}

#endif // SHADER_HELPER_HH
