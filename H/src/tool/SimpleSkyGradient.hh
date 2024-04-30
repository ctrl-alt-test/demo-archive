#ifndef SIMPLE_SKY_GRADIENT_HH
#define SIMPLE_SKY_GRADIENT_HH

#include "engine/algebra/Vector3.hh"
#include "gfx/Geometry.hh"
#include "gfx/ShadingParameters.hh"

namespace Gfx
{
	class IGraphicLayer;
	struct DrawArea;
}

namespace Core
{
	struct Camera;
}

namespace Tool
{
	class ShaderLoader;

	struct SimpleSkyGradient
	{
	public:
		void Init(Gfx::IGraphicLayer* gfxLayer,
				  const Gfx::Geometry& quad,
				  Tool::ShaderLoader* shaderLoader);
		void Draw(long currentStoryTime,
				  const Gfx::DrawArea& drawArea,
				  const Core::Camera& camera,
				  const Algebra::vector3f& skyFogColor,
				  const Algebra::vector3f& skyColor);

	private:
		Gfx::IGraphicLayer*		m_gfxLayer;
		Gfx::Geometry			m_geometry;
		Gfx::ShadingParameters	m_shading;
	};
}

#endif // SIMPLE_SKY_GRADIENT_HH
