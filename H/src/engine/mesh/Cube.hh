#ifndef CUBE_HH
#define CUBE_HH

namespace mesh
{
	struct Mesh;

	enum CubeFaces : unsigned int {
		face_left   = 1 << 0,
		face_right  = 1 << 1,
		face_bottom = 1 << 2,
		face_top    = 1 << 3,
		face_back   = 1 << 4,
		face_front  = 1 << 5,
		face_all    = 0xff,
	};

	Mesh& sharpCube(Mesh& dest, CubeFaces faces = face_all);
	Mesh& cube(Mesh& dest, CubeFaces faces = face_all);
}

#endif // CUBE_HH
