#include "ShaderHelper.hh"

#include "engine/algebra/Vector3.hxx"
#include "engine/container/Array.hxx"
#include "engine/core/Camera.hxx"
#include "engine/core/Light.hh"
#include "gfx/Uniform.hxx"
#include <cmath>
#include <cstring>

using namespace Gfx;
using namespace Tool;

const char* Tool::lightPositionParamNames[] = {
	U_LIGHTPOSITIONS "[0]",
	U_LIGHTPOSITIONS "[1]",
	U_LIGHTPOSITIONS "[2]",
	U_LIGHTPOSITIONS "[3]",
	U_LIGHTPOSITIONS "[4]",
};

const char* Tool::lightDirectionParamNames[] = {
	U_LIGHTDIRECTIONS "[0]",
	U_LIGHTDIRECTIONS "[1]",
	U_LIGHTDIRECTIONS "[2]",
	U_LIGHTDIRECTIONS "[3]",
	U_LIGHTDIRECTIONS "[4]",
};

const char* Tool::lightColorParamNames[] = {
	U_LIGHTCOLORS "[0]",
	U_LIGHTCOLORS "[1]",
	U_LIGHTCOLORS "[2]",
	U_LIGHTCOLORS "[3]",
	U_LIGHTCOLORS "[4]",
};

const char* Tool::lightConeCosParamNames[] = {
	U_LIGHTCONECOS "[0]",
	U_LIGHTCONECOS "[1]",
	U_LIGHTCONECOS "[2]",
	U_LIGHTCONECOS "[3]",
	U_LIGHTCONECOS "[4]",
};

const char* Tool::lightTypeParamNames[] = {
	U_LIGHTTYPES "[0]",
	U_LIGHTTYPES "[1]",
	U_LIGHTTYPES "[2]",
	U_LIGHTTYPES "[3]",
	U_LIGHTTYPES "[4]",
};

const char* Tool::shadowMapParamNames[] = {
	U_SHADOWMAPS "[0]",
	U_SHADOWMAPS "[1]",
	U_SHADOWMAPS "[2]",
	U_SHADOWMAPS "[3]",
	U_SHADOWMAPS "[4]",
};

const char* Tool::modelLightProjectionParamNames[] = {
	U_MODELLIGHTPROJECTIONMATRIX "[0]",
	U_MODELLIGHTPROJECTIONMATRIX "[1]",
	U_MODELLIGHTPROJECTIONMATRIX "[2]",
	U_MODELLIGHTPROJECTIONMATRIX "[3]",
	U_MODELLIGHTPROJECTIONMATRIX "[4]",
};

// SIZE: Ideally we'd like to use uniform names directly in the code,
// but the binary generated is a few bytes smaller when using an array.
// So we define macros to use the array while keeping the code readable.
static const char* s_matrixParamNames[] = {
	U_MODELMATRIX,
	U_VIEWMATRIX,
	U_PROJECTIONMATRIX,
	U_MODELVIEWMATRIX,
	U_MODELVIEWPROJECTIONMATRIX,
	U_INVERSEVIEWPROJECTIONMATRIX,
	U_INVERSEMODELVIEWPROJECTIONMATRIX,
};
#define MODELMATRIX						s_matrixParamNames[0]
#define VIEWMATRIX						s_matrixParamNames[1]
#define PROJECTIONMATRIX				s_matrixParamNames[2]
#define MODELVIEWMATRIX					s_matrixParamNames[3]
#define MODELVIEWPROJECTIONMATRIX		s_matrixParamNames[4]
#define INVERSEVIEWPROJECTIONMATRIX		s_matrixParamNames[5]
#define INVERSEMODELVIEWPROJECTIONMATRIX s_matrixParamNames[6]

Uniform ShaderHelper::CreateMatrixShaderParam(const char* name, const Algebra::matrix4& matrix)
{
	Uniform result = { name, 16, UniformType::Float, {} };
	memcpy(result.fValue, matrix.m, sizeof(matrix.m));
	return result;
}

void ShaderHelper::ExposeLightInfo(Container::Array<Uniform>& uniforms,
								   const Algebra::vector3f& ambientLight,
								   const Core::Light* lights,
								   const Core::Camera* lightCameras,
								   const TextureID* shadowMaps,
								   const Algebra::matrix4& model)
{
	uniforms.add(Uniform::Float3(U_AMBIENTLIGHT, ambientLight.x, ambientLight.y, ambientLight.z));

	for (int i = 0; i < NUM_LIGHTS; ++i)
	{
		const Core::Light& light = lights[i];
		const Algebra::vector3f& position = light.transform.v;
		const Algebra::vector3f& color = light.color;
		float cosAngle = msys_cosf(light.coneAngle / 2.f);

		uniforms.add(Uniform::Float4(lightPositionParamNames[i], position.x, position.y, position.z, 1.f));
		uniforms.add(Uniform::Float3(lightColorParamNames[i], color.x, color.y, color.z));
		uniforms.add(Uniform::Float1(lightConeCosParamNames[i], cosAngle));
		uniforms.add(Uniform::Int1(lightTypeParamNames[i], light.type));

		if (color == Algebra::vector3f::zero)
		{
			continue;
		}

		if (lightCameras != nullptr)
		{
			uniforms.add(ShaderHelper::CreateMatrixShaderParam(modelLightProjectionParamNames[i], lightCameras[i].viewProjection * model));
		}
		if (shadowMaps != nullptr)
		{
			uniforms.add(Uniform::Sampler1(shadowMapParamNames[i], shadowMaps[i]));
		}
	}
}

void ShaderHelper::ExposeFogInfo(Container::Array<Gfx::Uniform>& uniforms,
								 const Algebra::vector3f& mediumColor,
								 float mediumDensity)
{
	uniforms.add(Uniform::Float3(U_MEDIUMCOLOR, mediumColor.x, mediumColor.y, mediumColor.z));
	uniforms.add(Uniform::Float1(U_MEDIUMDENSITY, mediumDensity));
}

void ShaderHelper::ExposeTransformationInfo(Container::Array<Uniform>& uniforms,
											const Core::Camera& camera,
											const Algebra::matrix4& model)
{
	uniforms.add(CreateMatrixShaderParam(MODELMATRIX, model));
	uniforms.add(CreateMatrixShaderParam(VIEWMATRIX, camera.view));
	uniforms.add(CreateMatrixShaderParam(PROJECTIONMATRIX, camera.projection));
	uniforms.add(CreateMatrixShaderParam(MODELVIEWMATRIX, camera.view * model));

	// HACK: Find back zNear and zFar from the projection matrix. It's
	// not ideal because we lose precision, but I'm too lazy to figure
	// how to properly pass them here for now.
	float q = camera.projection.m22;
	float qn = camera.projection.m23;
	float zNear = qn / (q - 1.f);
	float zFar = qn / (q + 1.f);

	uniforms.add(Uniform::Float1(U_ZNEAR, zNear));
	uniforms.add(Uniform::Float1(U_ZFAR, zFar));

	const Algebra::vector3f position = { camera.inverseView.m03, camera.inverseView.m13, camera.inverseView.m23 };
	uniforms.add(Uniform::Float3(U_CAMERAPOSITION, position.x, position.y, position.z));
}

void ShaderHelper::ExposeTransformationInfoForFullScreen(Container::Array<Uniform>& uniforms, const Core::Camera& camera)
{
	uniforms.add(CreateMatrixShaderParam(PROJECTIONMATRIX, Algebra::matrix4::ortho(0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f)));
	uniforms.add(CreateMatrixShaderParam(MODELVIEWMATRIX, Algebra::matrix4::identity));
	uniforms.add(CreateMatrixShaderParam(INVERSEVIEWPROJECTIONMATRIX, camera.viewProjection.inverted()));

	const Algebra::vector3f position = { camera.inverseView.m03, camera.inverseView.m13, camera.inverseView.m23 };
	uniforms.add(Uniform::Float3(U_CAMERAPOSITION, position.x, position.y, position.z));
}

void ShaderHelper::ExposeFullScreenBlitTransform(Container::Array<Uniform>& uniforms)
{
	uniforms.add(CreateMatrixShaderParam(PROJECTIONMATRIX, Algebra::matrix4::ortho(0.0f, 1.0f, 0.0f, 1.0f, -1.0f, 1.0f)));
	uniforms.add(CreateMatrixShaderParam(MODELVIEWMATRIX, Algebra::matrix4::identity));
}

void ShaderHelper::ExposeResolutionInfo(Container::Array<Uniform>& uniforms, int bufferWidth, int bufferHeight)
{
	uniforms.add(Uniform::Float2(U_RESOLUTION, (float)bufferWidth, (float)bufferHeight));
	uniforms.add(Uniform::Float2(U_INVRESOLUTION, 1.0f / bufferWidth, 1.0f / bufferHeight));
	uniforms.add(Uniform::Float2(U_CENTER, 0.5f, 0.5f));
}

void ShaderHelper::ExposeTimeInfo(Container::Array<Uniform>& uniforms, long time)
{
	uniforms.add(Uniform::Float1(U_TIME, time / 1000.f));
}

// FIXME: this function feels too specific. Maybe it should be merged
// with ExposeTransformationInfo.
void ShaderHelper::ExposeModelViewProjectionInfo(Container::Array<Uniform>& uniforms, const Algebra::matrix4& modelViewProjection, const Algebra::matrix4& inverseModelViewProjection)
{
	uniforms.add(CreateMatrixShaderParam(MODELVIEWMATRIX, modelViewProjection));
	uniforms.add(CreateMatrixShaderParam(INVERSEMODELVIEWPROJECTIONMATRIX, inverseModelViewProjection));
}
