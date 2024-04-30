#include "Quad.hh"

#include "engine/container/Utils.hh"
#include "gfx/IGraphicLayerImplementations.hh"
#include "tool/mesh/VertexDataP2dT.hh"

using namespace Tool;

// CR: Je pense que les types vec3/vec4 seraient utile
static const VertexDataP2dT quadVertices[] = {
	// Position  texture coordinates
	{ 0.0f, 0.0f,	 0.0f, 1.0f, },
	{ 0.0f, 1.0f,	 0.0f, 0.0f, },
	{ 1.0f, 1.0f,	 1.0f, 0.0f, },
	{ 1.0f, 1.0f,	 1.0f, 0.0f, },
	{ 1.0f, 0.0f,	 1.0f, 1.0f, },
	{ 0.0f, 0.0f,	 0.0f, 1.0f, },
};

Gfx::Geometry Tool::LoadQuadMesh(Gfx::IGraphicLayer* graphicsLayer)
{
	Gfx::Geometry result = {
		{ 0 },
		ARRAY_LEN(quadVertices),
		0,
#if GFX_ENABLE_VERTEX_BUFFER_OFFSET
		0,
#endif // GFX_ENABLE_VERTEX_BUFFER_OFFSET
	};
	result.vertexBuffer = graphicsLayer->LoadVertexBuffer(graphicsLayer->CreateVertexBuffer(),
														  VertexDataP2dT::s_descriptor.attributes,
														  VertexDataP2dT::s_descriptor.numberOfAttributes,
														  VertexDataP2dT::s_descriptor.size,
														  sizeof(quadVertices), (void*)quadVertices,
														  0, nullptr, Gfx::VertexIndexType::UInt32);
	return result;
}
