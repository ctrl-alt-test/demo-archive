#ifndef MESH_POOL_HH
#define MESH_POOL_HH

#include "Mesh.hh"

namespace mesh
{
	/// <summary>
	/// A pool of meshes ready to use, meant to avoid repeated allocations and deallocations.
	/// </summary>
	struct Pool
	{
	public:
		void	Init();
		void	Destroy();

		void	Clear();

		// FIXME: provide a malloc/free interface.
		Mesh&	GetTempMesh();

	private:
		Mesh	m_meshes[32];
		int		m_meshId;
	};
}

#endif // MESH_POOL_HH
