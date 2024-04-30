#include "VertexDataP2dT.hh"

#include "engine/container/Utils.hh"
#include "engine/mesh/Vertex.hh"
#include "MeshHelper.hh"

using namespace Tool;

static void SetData(void* dstVertexData, const mesh::vertex& vertex)
{
	VertexDataP2dT& dest = *static_cast<VertexDataP2dT*>(dstVertexData);

	dest.x = vertex.p.x;
	dest.y = vertex.p.y;
#if VERTEX_ATTR_TEX_COORD
	dest.u = vertex.u;
	dest.v = vertex.v;
#endif
}

const Gfx::VertexAttribute vertexAttributes[] = {
	{ Tool::positionAttributeName,	2, Gfx::VertexAttributeType::Float,	},
	{ Tool::texCoordAttributeName,	2, Gfx::VertexAttributeType::Float,	},
};

const VertexFormatDescriptor VertexDataP2dT::s_descriptor = {
	SetData,
	vertexAttributes,
	ARRAY_LEN(vertexAttributes),
	sizeof(VertexDataP2dT),
};
