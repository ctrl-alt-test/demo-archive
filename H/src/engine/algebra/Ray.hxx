#ifndef RAY_HXX
#define RAY_HXX

#include "ray.hh"

namespace Algebra
{
	ray ray::create(const vector3f& p1, const vector3f& p2)
	{
		ray result;
		result.origin = p1;
		result.dir = p2 - p1;
		normalize(result.dir);
		return result;
	}

	inline
	float dist(const ray& r, const vector3f& p)
	{
		//return dist(p, project(r, p));
		return norm((p - r.origin) - dot(p - r.origin, r.dir) * r.dir);
		// FIXME: write with cross products and no square root?
	}

	inline
	vector3f project(const ray& r, const vector3f& p)
	{
	}
}

#endif // RAY_HXX
