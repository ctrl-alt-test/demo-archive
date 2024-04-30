#ifndef AABB3_HH
#define AABB3_HH

#include "Vector3.hh"

namespace Algebra
{
	/// <summary>
	/// A one dimensional range, defined by a min and a max.
	/// </summary>
	struct range
	{
		float min;
		float max;

		static const range nothing;
		static const range everything;

		float center() const;
		float size() const;

		bool contains(float x) const;
		bool contains(const range& r) const;
		bool intersects(const range& r) const;

		void expand(float x);
	};

	/// <summary>
	/// 3D Axis Aligned Bounding Box.
	/// </summary>
	struct AABB3
	{
		vector3f min;
		vector3f max;

		vector3f center() const;
		vector3f size() const;

		range xRange() const;
		range yRange() const;
		range zRange() const;

		bool contains(const vector3f& p) const;
		bool contains(const AABB3& box) const;
		bool intersects(const AABB3& box) const;

		void expand(const vector3f& p);
	};
}

#endif // AABB3_HH
