#include "ShadingParameters.hh"

#include "engine/container/Array.hxx"
// FIXME: ideally Gfx should not have dependency over Engine.

using namespace Gfx;

ShadingParameters::ShadingParameters():
	blendingMode(BlendingMode::Opaque),
	polygonMode(PolygonMode::Filled),
	shader(ShaderID::InvalidID),
	uniforms(MAX_UNIFORMS)
{
}

ShadingParameters::ShadingParameters(const ShadingParameters& src):
	blendingMode(src.blendingMode),
	polygonMode(src.polygonMode),
	shader(src.shader),
	uniforms(src.uniforms.size)
{
	uniforms.copyFrom(src.uniforms);
}

ShadingParameters& ShadingParameters::operator = (const ShadingParameters& src)
{
	blendingMode = src.blendingMode;
	polygonMode = src.polygonMode;
	shader = src.shader;
	uniforms.copyFrom(src.uniforms);

	return *this;
}
