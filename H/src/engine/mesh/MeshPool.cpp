//
// Mesh pool, to avoid repetitive allocations.
//

#include "MeshPool.hh"

#include "engine/container/Array.hxx"
#include "engine/container/Utils.hh"
#include "engine/core/Debug.hh"
#include "Mesh.hh"

using namespace mesh;

void Pool::Init()
{
	for (size_t i = 0; i < ARRAY_LEN(m_meshes); ++i)
	{
		m_meshes[i].vertices.init(4*65536);
		m_meshes[i].quads.init(4*65536);
	}
	m_meshId = 0;
}

void Pool::Clear()
{
	m_meshId = 0;
}

Mesh& Pool::GetTempMesh()
{
	assert(m_meshId < (int)ARRAY_LEN(m_meshes));
	m_meshes[m_meshId].vertices.empty();
	m_meshes[m_meshId].quads.empty();
	return m_meshes[m_meshId++];
}

void Pool::Destroy()
{
	for (size_t i = 0; i < ARRAY_LEN(m_meshes); ++i)
	{
		free(m_meshes[i].vertices.elt);
		IFDBG(m_meshes[i].vertices.max_size = 0);
		IFDBG(m_meshes[i].vertices.elt = 0);
		free(m_meshes[i].quads.elt);
		IFDBG(m_meshes[i].quads.max_size = 0);
		IFDBG(m_meshes[i].quads.elt = 0);
	}
	m_meshId = 0;
}
