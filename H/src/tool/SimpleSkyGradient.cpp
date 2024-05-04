#include "SimpleSkyGradient.hh"

#include "engine/algebra/Matrix.hh"
#include "gfx/IGraphicLayerImplementations.hh"
#include "tool/ShaderHelper.hh"
#include "tool/ShaderLoader.hh"

using namespace Gfx;
using namespace Tool;

void SimpleSkyGradient::Init(IGraphicLayer* gfxLayer,
							 const Geometry& quad,
							 Tool::ShaderLoader* shaderLoader)
{
	m_gfxLayer = gfxLayer;
	m_geometry = quad;

	LOAD_SHADER(*shaderLoader, &m_shading.shader, skyShader, "assets/shaders/screenspace/blitShader.vs", "assets/shaders/screenSpace/sky.fs");
}


void SimpleSkyGradient::Draw(long /*currentStoryTime*/,
							 const Gfx::DrawArea& drawArea,
							 const Core::Camera& camera,
							 const Algebra::vector3f& skyFogColor,
							 const Algebra::vector3f& skyColor)
{
	m_shading.uniforms.empty();
	Tool::ShaderHelper::ExposeTransformationInfoForFullScreen(m_shading.uniforms, camera);
	m_shading.uniforms.add(Gfx::Uniform::Float3(U_FOGCOLOR, skyFogColor.x, skyFogColor.y, skyFogColor.z));
	m_shading.uniforms.add(Gfx::Uniform::Float3(U_SKYCOLOR, skyColor.x, skyColor.y, skyColor.z));
	m_gfxLayer->Draw(drawArea, RasterTests::NoDepthTest, m_geometry, m_shading);
}
