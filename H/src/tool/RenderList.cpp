#include "RenderList.hh"

#include "engine/algebra/Functions.hh"
#include "engine/algebra/Plane.hxx"
#include "engine/core/Camera.hh"
#include "gfx/IGraphicLayerImplementations.hh"
#include "tool/CommonMaterial.hh"
#include "tool/FixedLightToolBox.hh"
#include "tool/ParticipatingMedium.hh"
#include "tool/ShaderHelper.hh"
#include "tool/TextureHelper.hh"
#include "tweakval/tweakval.h"
#include <cassert>
#include <new>

using namespace Tool;

void RenderList::Init(Gfx::IGraphicLayer* gfxLayer_)
{
	gfxLayer = gfxLayer_;
	for (int i = 0; i < RENDER_LIST_MAX_SIZE; ++i)
	{
		// Placement new so the array inside ShadingParameters gets
		// initialized.
		new (shadingParametersForRendering + i) Gfx::ShadingParameters();
		new (shadingParametersForShadowMap + i) Gfx::ShadingParameters();
	}
}

int RenderList::Add(const Render::MeshGeometry& mesh,
					const CommonMaterial& material,
					const Algebra::matrix4& modelMatrix)
{
	assert(size < RENDER_LIST_MAX_SIZE);
	material.PresetShaderParameters(shadingParametersForRendering[size], false);
	material.PresetShaderParameters(shadingParametersForShadowMap[size], true);
	meshes[size] = mesh;
	modelMatrices[size] = modelMatrix;
	return size++;
}

/// <summary>
/// Performs a bounding sphere and/or axis aligned bounding box based
/// frustum culling test, depending on ENABLE_SPHERE_CULLING and
/// ENABLE_AABB_CULLING.
/// </summary>
///
/// <returns>True if the volume intersects with the frustum, false
/// otherwise.</returns>
bool FrustumCullingTest(const Algebra::matrix4& modelViewProjectionMatrix,
						const Render::MeshGeometry& geometry)
{
#if ENABLE_SPHERE_CULLING || ENABLE_AABB_CULLING
	Algebra::plane clippingPlanes[] = {
		{{{
			modelViewProjectionMatrix.m30 + modelViewProjectionMatrix.m00,
			modelViewProjectionMatrix.m31 + modelViewProjectionMatrix.m01,
			modelViewProjectionMatrix.m32 + modelViewProjectionMatrix.m02,
			modelViewProjectionMatrix.m33 + modelViewProjectionMatrix.m03,
		}}},
		{{{
			modelViewProjectionMatrix.m30 - modelViewProjectionMatrix.m00,
			modelViewProjectionMatrix.m31 - modelViewProjectionMatrix.m01,
			modelViewProjectionMatrix.m32 - modelViewProjectionMatrix.m02,
			modelViewProjectionMatrix.m33 - modelViewProjectionMatrix.m03,
		}}},
		{{{
			modelViewProjectionMatrix.m30 + modelViewProjectionMatrix.m10,
			modelViewProjectionMatrix.m31 + modelViewProjectionMatrix.m11,
			modelViewProjectionMatrix.m32 + modelViewProjectionMatrix.m12,
			modelViewProjectionMatrix.m33 + modelViewProjectionMatrix.m13,
		}}},
		{{{
			modelViewProjectionMatrix.m30 - modelViewProjectionMatrix.m10,
			modelViewProjectionMatrix.m31 - modelViewProjectionMatrix.m11,
			modelViewProjectionMatrix.m32 - modelViewProjectionMatrix.m12,
			modelViewProjectionMatrix.m33 - modelViewProjectionMatrix.m13,
		}}},
		{{{
			modelViewProjectionMatrix.m30 + modelViewProjectionMatrix.m20,
			modelViewProjectionMatrix.m31 + modelViewProjectionMatrix.m21,
			modelViewProjectionMatrix.m32 + modelViewProjectionMatrix.m22,
			modelViewProjectionMatrix.m33 + modelViewProjectionMatrix.m23,
		}}},
		{{{
			modelViewProjectionMatrix.m30 - modelViewProjectionMatrix.m20,
			modelViewProjectionMatrix.m31 - modelViewProjectionMatrix.m21,
			modelViewProjectionMatrix.m32 - modelViewProjectionMatrix.m22,
			modelViewProjectionMatrix.m33 - modelViewProjectionMatrix.m23,
		}}},
	};
#endif // ENABLE_SPHERE_CULLING || ENABLE_AABB_CULLING

#if ENABLE_SPHERE_CULLING || ENABLE_AABB_CULLING
	for (unsigned int i = 0; i < ARRAY_LEN(clippingPlanes); ++i)
	{
		Algebra::plane& plane = clippingPlanes[i];
		const float norm = Algebra::norm(plane.normal);

#if ENABLE_AABB_CULLING
		// Normalize the plane.
		plane.normal /= norm;
		plane.dist /= norm;
#endif // !ENABLE_AABB_CULLING

#if ENABLE_SPHERE_CULLING && !ENABLE_AABB_CULLING
		if (clippingPlanes[i].dist < -norm * geometry.boundingSphereRadius)
		{
			return false;
		}
#elif ENABLE_SPHERE_CULLING && ENABLE_AABB_CULLING
		if (clippingPlanes[i].dist < -geometry.boundingSphereRadius)
		{
			return false;
		}
#endif // ENABLE_SPHERE_CULLING && ENABLE_AABB_CULLING

#if ENABLE_AABB_CULLING
		const Algebra::vector3f highestAABBVertex = {
			(plane.normal.x > 0.f ? geometry.aabb.max.x : geometry.aabb.min.x),
			(plane.normal.y > 0.f ? geometry.aabb.max.y : geometry.aabb.min.y),
			(plane.normal.z > 0.f ? geometry.aabb.max.z : geometry.aabb.min.z)
		};

		const float dist = Algebra::dot(highestAABBVertex, plane.normal) + plane.dist;
		if (dist < 0.f)
		{
			return false;
		}
#endif // ENABLE_AABB_CULLING
	}
#endif // ENABLE_SPHERE_CULLING || ENABLE_AABB_CULLING

	return true;
}

void RenderList::Render(long currentTime,
						const Gfx::DrawArea& drawArea,
						const Gfx::RasterTests& rasterTests,
						const Core::Camera& camera,
						const Algebra::plane* clipPlane,
						const FixedLightRig& lightRig,
						const ParticipatingMedium& medium,
						bool isShadowPass)
{
	assert(size <= RENDER_LIST_MAX_SIZE);

	// TODO: Split culling from rendering and insert sorting in between.

	for (int i = 0; i < size; ++i)
	{
		m_shading = (isShadowPass ? shadingParametersForShadowMap[i] : shadingParametersForRendering[i]);

		// Using InvalidID is a way to declare the object as not
		// rendered in this pass.
		if (m_shading.shader == Gfx::ShaderID::InvalidID)
		{
			continue;
		}

		const Algebra::matrix4& modelMatrix = modelMatrices[i];
		const Algebra::matrix4 modelViewProjectionMatrix = camera.viewProjection * modelMatrix;
		if (!FrustumCullingTest(modelViewProjectionMatrix, meshes[i]))
		{
			continue;
		}

		ShaderHelper::ExposeTimeInfo(m_shading.uniforms, currentTime);
		ShaderHelper::ExposeTransformationInfo(m_shading.uniforms, camera, modelMatrix);
		if (!isShadowPass)
		{
			ShaderHelper::ExposeLightInfo(m_shading.uniforms, lightRig.ambientLight, lightRig.lights, lightRig.lightCameras, lightRig.shadowMaps, modelMatrix);
		}
		ShaderHelper::ExposeFogInfo(m_shading.uniforms, medium.color, medium.density);
		if (clipPlane != nullptr)
		{
			m_shading.uniforms.add(Gfx::Uniform::Float4(U_CLIPPLANE, clipPlane->a, clipPlane->b, clipPlane->c, clipPlane->d));
		}
		gfxLayer->Draw(drawArea, rasterTests, meshes[i].geometry, m_shading);
	}
}
