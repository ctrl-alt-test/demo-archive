#include "MeshHelper.hh"

#include "engine/container/Array.hxx"
#include "engine/core/Debug.hh"
#include "engine/mesh/Mesh.hh"
#include "gfx/IGraphicLayerImplementations.hh"
#include "tool/ShaderHelper.hh"
#include <cassert>
#include <cstdlib>

using namespace Tool;

//
// Define here the name of the vertex attributes commonly used in shaders.
//
const char* Tool::positionAttributeName	= U_VERTEXPOSITION;
const char* Tool::normalAttributeName	= U_VERTEXNORMAL;
const char* Tool::tangentAttributeName	= U_VERTEXTANGENT;
const char* Tool::texCoordAttributeName	= U_VERTEXTEXCOORD;

void MeshHelper::LoadVertexBuffer(Gfx::IGraphicLayer* gfxLayer,
								  const Gfx::VertexBufferID& vertexBuffer,
								  const mesh::Mesh& mesh,
								  const VertexFormatDescriptor& formatDescriptor,
								  int* out_numberOfIndices)
{
	const int vertexDataSize = mesh.vertices.size * formatDescriptor.size;
	char* vertices = (char*)malloc(vertexDataSize);
	for (int i = 0; i < mesh.vertices.size; ++i)
	{
		// Defined separately to allow different data formats or compression.
		formatDescriptor.SetVertexData(vertices + (i * formatDescriptor.size), mesh.vertices[i]);
	}

	assert(mesh.quads.size % 4 == 0);
	Container::Array<unsigned int> indices((mesh.quads.size * 6)/4);
	indices.size = (mesh.quads.size * 6) / 4;
	for (int i = 0, j = 0; i < mesh.quads.size; i += 4, j += 6)
	{
		indices[j    ] = mesh.quads[i + 0];
		indices[j + 1] = mesh.quads[i + 1];
		indices[j + 2] = mesh.quads[i + 2];
		indices[j + 3] = mesh.quads[i + 2];
		indices[j + 4] = mesh.quads[i + 3];
		indices[j + 5] = mesh.quads[i + 0];
	}

	*out_numberOfIndices = indices.size;
	gfxLayer->LoadVertexBuffer(vertexBuffer,
							   formatDescriptor.attributes,
							   formatDescriptor.numberOfAttributes,
							   formatDescriptor.size,
							   vertexDataSize,
							   vertices,
							   indices.size * sizeof(indices[0]),
							   (void*)indices.elt,
							   Gfx::VertexIndexType::UInt32);
	LOG_INFO("Loaded vertex buffer from mesh: %d vertices and %d indices.",
			 mesh.vertices.size, indices.size);
}
