#ifndef RENDER_LIST_HH
#define RENDER_LIST_HH

#include "engine/algebra/Matrix.hh"
#include "engine/render/MeshGeometry.hh"
#include "gfx/ShadingParameters.hh"

#define RENDER_LIST_MAX_SIZE 512

namespace Core
{
	struct Camera;
}

namespace Gfx
{
	class IGraphicLayer;
	struct DrawArea;
	struct RasterTests;
}

namespace Tool
{
	struct CommonMaterial;
	struct FixedLightRig;
	struct ParticipatingMedium;

	class RenderList
	{
	public:
		void Init(Gfx::IGraphicLayer* gfxLayer);
		int Add(const Render::MeshGeometry& mesh,
				const Tool::CommonMaterial& material,
				const Algebra::matrix4& modelMatrix);
		void Render(long currentTime,
					const Gfx::DrawArea& drawArea,
					const Gfx::RasterTests& rasterTests,
					const Core::Camera& camera,
					const Algebra::plane* clipPlane,
					const Tool::FixedLightRig& lightRig,
					const Tool::ParticipatingMedium& medium,
					bool isShadowPass);

		Gfx::IGraphicLayer*		gfxLayer;
		int						size;
		Render::MeshGeometry	meshes[RENDER_LIST_MAX_SIZE];
		Algebra::matrix4		modelMatrices[RENDER_LIST_MAX_SIZE];
		Gfx::ShadingParameters	shadingParametersForRendering[RENDER_LIST_MAX_SIZE];
		Gfx::ShadingParameters	shadingParametersForShadowMap[RENDER_LIST_MAX_SIZE];

	private:
		Gfx::ShadingParameters	m_shading;
	};
}

#endif // RENDER_LIST_HH
