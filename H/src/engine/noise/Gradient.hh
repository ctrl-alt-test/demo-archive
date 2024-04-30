#ifndef GRADIENT_HH
#define GRADIENT_HH

#include "engine/algebra/Vector2.hh"
#include "engine/algebra/Vector3.hh"
#include "engine/algebra/Vector4.hh"

namespace Noise
{
	class Gradient
	{
	public:
		static void init();

		// Gets a N-D gradient from 1 to 4 integers.
		static float				get(int i);							// -1 or 1
		static Algebra::vector2f	get(int i, int j);					// one of the 8 directions.
		static Algebra::vector3f	getInCube(int i, int j, int k);		// centers of the edges of a cube.
		static Algebra::vector3f	getInSimplex(int i, int j, int k);	// centers of the edges and vertices of a cube.
		static Algebra::vector4f	get(int i, int j, int k, int l);	// centers of the edges of a tesseract.
	};
}

#endif // GRADIENT_HH
