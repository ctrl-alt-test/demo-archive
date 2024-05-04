#ifndef PLANE_HH
#define PLANE_HH

#include "Ray.hh"

namespace Algebra
{
	struct plane
	{
		union
		{
			struct
			{
				vector3f	normal;
				float		dist;
			};
			struct
			{
				float a;
				float b;
				float c;
				float d;
			};
		};

		static plane create(const vector3f& origin, const vector3f& normal);
		static plane create(const vector3f& p1, const vector3f& p2, const vector3f& p3);
	};

	float		dist(const plane& s, const vector3f& p);

	vector3f	project(const plane& s, const vector3f& p);
	ray			project(const plane& s, const ray& r);

	bool		intersect(const plane& s, const ray& r, vector3f& out_p);
	ray			intersect(const plane& lhs, const plane& rhs);


	struct disc
	{
		plane p;
		float radius;
	};

	bool		intersect(const disc& d, const ray& r, vector3f& p);
}

#endif // PLANE_HH
