#ifndef MESH_HELPER_HH
#define MESH_HELPER_HH

namespace Gfx
{
	class IGraphicLayer;
	struct VertexAttribute;
	struct VertexBufferID;
}

namespace mesh
{
	struct Mesh;
	struct vertex;
};

namespace Tool
{
	extern const char* positionAttributeName;
	extern const char* normalAttributeName;
	extern const char* tangentAttributeName;
	extern const char* texCoordAttributeName;

	struct VertexFormatDescriptor
	{
		void						(*SetVertexData)(void* dstVertexData, const mesh::vertex& vertex);
		const Gfx::VertexAttribute*	attributes;
		int							numberOfAttributes;
		int							size;

		bool operator == (const VertexFormatDescriptor& rhs) const
		{
			return
				SetVertexData == rhs.SetVertexData &&
				attributes == rhs.attributes &&
				numberOfAttributes == rhs.numberOfAttributes &&
				size == rhs.size;
		}
	};

	class MeshHelper
	{
	public:
		/// <summary>
		/// Generates the vertex data for the mesh and sends it to GPU,
		/// using the vertex attributes structure given as a template argument.
		/// </summary>
		static void LoadVertexBuffer(Gfx::IGraphicLayer* gfxLayer,
									 const Gfx::VertexBufferID& vertexBuffer,
									 const mesh::Mesh& mesh,
									 const VertexFormatDescriptor& formatDescriptor,
									 int* out_numberOfIndices);
	};
}

#endif // MESH_HELPER_HH
