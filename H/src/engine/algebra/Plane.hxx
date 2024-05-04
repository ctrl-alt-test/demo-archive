#ifndef PLANE_HXX
#define PLANE_HXX

#include "Plane.hh"
#include "engine/algebra/Functions.hh"
#include "engine/core/msys_temp.hh"
#include <cassert>

namespace Algebra
{
	inline
	plane plane::create(const vector3f& origin, const vector3f& normal)
	{
		assert(abs(norm(normal) - 1.f) < 0.0001f);
		plane result;
		result.normal = normal;
		result.d = dot(origin, normal);
		return result;
	}

	inline
	plane plane::create(const vector3f& p1, const vector3f& p2, const vector3f& p3)
	{
		return create(p1, normalized(cross(p2 - p1, p3 - p1)));
	}

	inline
	float dist(const plane& s, const vector3f& p)
	{
		return dot(p, s.normal) - s.dist;
	}

	inline
	vector3f project(const plane& s, const vector3f& p)
	{
		return p - (dot(p, s.normal) - s.dist) * s.normal;
	}

	inline
	ray project(const plane& s, const ray& r)
	{
		ray result;
		result.origin = project(s, r.origin);
		result.dir = project(s, r.origin + r.dir) - result.origin;
		normalize(result.dir);
		return result;
	}

	inline
	bool intersect(const plane& s, const ray& r, vector3f& out_p)
	{
		const float dotln = dot(s.normal, r.dir);
		if (dotln < 0.000001f)
		{
			return false;
		}

		const float t = dot(s.normal * s.dist - r.origin, s.normal) / dotln;
		out_p = r.origin + t * s.normal;
		return true;
	}

	inline
	bool intersect(const disc& d, const ray& r, vector3f& p)
	{
		if (intersect(d.p, r, p) == false) return false;
		const vector3f cp = p - d.p.normal * d.p.dist;
		return dot(cp, cp) <= d.radius * d.radius;
	}

	inline
	ray intersect(const plane& lhs, const plane& rhs)
	{
		ray result;
		result.dir = cross(lhs.normal, rhs.normal);
		result.origin = project(lhs, rhs.normal * rhs.dist);
		return result;
	}
}

#endif // PLANE_HXX
