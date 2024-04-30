#include "VertexDataPN.hh"

#include "engine/container/Utils.hh"
#include "engine/mesh/Vertex.hh"
#include "MeshHelper.hh"

using namespace Tool;

static void SetData(void* dstVertexData, const mesh::vertex& vertex)
{
	VertexDataPN& dest = *static_cast<VertexDataPN*>(dstVertexData);

	dest.x = vertex.p.x;
	dest.y = vertex.p.y;
	dest.z = vertex.p.z;
#if VERTEX_ATTR_NORMAL
	dest.nx = vertex.n.x;
	dest.ny = vertex.n.y;
	dest.nz = vertex.n.z;
#endif
}

const Gfx::VertexAttribute vertexAttributes[] = {
	{ Tool::positionAttributeName,	3, Gfx::VertexAttributeType::Float,	},
	{ Tool::normalAttributeName,	3, Gfx::VertexAttributeType::Float,	},
};

const VertexFormatDescriptor VertexDataPN::s_descriptor = {
	SetData,
	vertexAttributes,
	ARRAY_LEN(vertexAttributes),
	sizeof(VertexDataPN),
};
