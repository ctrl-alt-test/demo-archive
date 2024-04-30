#ifndef HEIGHTMAP_HH
#define HEIGHTMAP_HH

#include <cassert>
#include <cstddef>

namespace Container
{
	template<typename T>
	class Array;
}

namespace mesh
{
	struct Mesh;
	struct vertex;

	class Heightmap
	{
	public:
		typedef float (*heightFunction)(float x, float z);

		explicit Heightmap(heightFunction heightFunc):
			m_heightFunc(heightFunc)
		{
			assert(heightFunc != NULL);
		}

		void			GenerateMesh(Mesh& mesh,
									 float x0, float z0, float x1, float z1,
									 int xFaces, int zFaces);

	private:
		void			ComputePositions(Container::Array<vertex>& vertices,
										 float x0, float z0, float x1, float z1,
										 int xFaces, int zFaces);
		static void		UpdateQuadsIndices(Mesh& mesh, int vertexStart, int xFaces, int zFaces);

		heightFunction	m_heightFunc;
	};
}

#endif // HEIGHTMAP_HH
