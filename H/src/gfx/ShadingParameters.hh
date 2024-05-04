#ifndef MATERIAL_HH
#define MATERIAL_HH

#include "BlendingMode.hh"
#include "PolygonMode.hh"
#include "ResourceID.hh"
#include "Uniform.hh"
#include "engine/container/Array.hh"
// FIXME: ideally Gfx should not have dependency over Engine.

#define MAX_UNIFORMS	64

namespace Gfx
{
	/// <summary>
	/// Shading parameters for a drawcall. Consists in a blending mode,
	/// a set of textures, a shader and its set of parameters.
	/// </summary>
	struct ShadingParameters
	{
		BlendingMode				blendingMode;
		PolygonMode::Enum			polygonMode;
		ShaderID					shader;
		Container::Array<Uniform>	uniforms;

		ShadingParameters();
		ShadingParameters(const ShadingParameters& src);
		ShadingParameters& operator = (const ShadingParameters& src);
	};
}

#endif // MATERIAL_HH
