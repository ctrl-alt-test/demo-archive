#ifndef POLYGON_MODE_HH
#define POLYGON_MODE_HH

namespace Gfx
{
	struct PolygonMode
	{
		enum Enum {
			Points = GL_POINT,
			Lines = GL_LINE,
			Filled = GL_FILL,
		};
	};
}

#endif // POLYGON_MODE_HH
