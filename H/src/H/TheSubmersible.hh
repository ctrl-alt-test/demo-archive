#pragma once

#include "gfx/ShadingParameters.hh"
#include "tool/CommonMaterial.hh"

namespace Core
{
	class Settings;
	struct RigidTransform;
}

namespace Gfx
{
	class IGraphicLayer;
	struct RasterTests;
	struct DrawArea;
	struct Geometry;
}

namespace Render
{
	struct MeshGeometry;
}

namespace Tool
{
	class MeshLoader;
	class RenderList;
	class ShaderLoader;
	class TextureLoader;
}

struct TheSubmersible
{
	void Init(const Core::Settings& settings,
			  Tool::MeshLoader* meshLoader,
			  Tool::ShaderLoader* shaderLoader,
			  Tool::TextureLoader* textureLoader);
	void AddToScene(Tool::RenderList* renderList,
					long currentTime,
					const Algebra::matrix4& modelMatrix,
					const Algebra::vector3f& spotLightColor);

	static void GetLights(float* cones, int* types, Core::RigidTransform* transforms);

	Gfx::ShadingParameters	m_shading;

	Render::MeshGeometry*	body;
	Tool::CommonMaterial	bodyMaterial;

	Render::MeshGeometry*	frame;
	Tool::CommonMaterial	frameMaterial;

	Render::MeshGeometry*	spots;
	Tool::CommonMaterial	spotsMaterial;
};
