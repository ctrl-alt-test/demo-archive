#ifndef AABB3_HXX
#define AABB3_HXX

#include "AABB3.hh"
#include "engine/algebra/Vector3.hxx"
#include "engine/core/msys_temp.hh"

#define BIG_FLOAT 524288.f

namespace Algebra
{
	static const range everything = { -BIG_FLOAT, BIG_FLOAT };
	static const range nothing = { BIG_FLOAT, -BIG_FLOAT };

	inline
	float range::center() const
	{
		return 0.5f * (min + max);
	}

	inline
	float range::size() const
	{
		return max - min;
	}

	inline
	bool range::contains(float x) const
	{
		return x >= min && x <= max;
	}

	inline
	bool range::contains(const range& r) const
	{
		return contains(r.min) && contains(r.max);
	}

	inline
	bool range::intersects(const range& r) const
	{
		return contains(r.min) || contains(r.max);
	}

	inline
	void range::expand(float x)
	{
		min = msys_min(min, x);
		max = msys_max(max, x);
	}

	inline
	vector3f AABB3::center() const
	{
		return 0.5f * (min + max);
	}

	inline
	vector3f AABB3::size() const
	{
		return max - min;
	}

	inline
	range AABB3::xRange() const
	{
		range result = { min.x, max.x };
		return result;
	}

	inline
	range AABB3::yRange() const
	{
		range result = { min.y, max.y };
		return result;
	}

	inline
	range AABB3::zRange() const
	{
		range result = { min.z, max.z };
		return result;
	}

	inline
	bool AABB3::contains(const vector3f& p) const
	{
		return (xRange().contains(p.x) &&
				yRange().contains(p.y) &&
				zRange().contains(p.z));
	}

	inline
	bool AABB3::contains(const AABB3& box) const
	{
		return (xRange().contains(box.xRange()) &&
				yRange().contains(box.yRange()) &&
				zRange().contains(box.zRange()));
	}

	inline
	bool AABB3::intersects(const AABB3& box) const
	{
		return (xRange().intersects(box.xRange()) &&
				yRange().intersects(box.yRange()) &&
				zRange().intersects(box.zRange()));
	}

	inline
	void AABB3::expand(const vector3f& p)
	{
		min = Algebra::min(min, p);
		max = Algebra::max(max, p);
	}
}

#endif // AABB3_HXX
