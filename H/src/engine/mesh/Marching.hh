#ifndef MARCHING_HH
#define MARCHING_HH

#include "Mesh.hh"
#include <cassert>

namespace mesh
{
	class Marching
	{
	public:
		/// <summary/>
		///
		/// <param name="sdfFunc">Signed Distance Field function.</param>
		explicit Marching(SDFFunction sdf):
			m_sdf(sdf)
		{
			assert(sdf != nullptr);
		}

		void GenerateMesh(mesh::Mesh& dest,
						  float x0, float y0, float z0,
						  float x1, float y1, float z1,
						  int xFaces, int yFaces, int zFaces);

	private:
		SDFFunction	m_sdf;
	};
}

#endif // MARCHING_HH
