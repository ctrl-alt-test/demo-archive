#include "engine/algebra/Functions.hh"
#include "engine/algebra/Vector3.hxx"
#include "UnitTest++/UnitTest++.h"
#include <cstdio>

using namespace Algebra;

SUITE(vector3)
{
	TEST(Equality)
	{
		CHECK(vector3f::zero == vector3f::zero);
		CHECK(vector3f::zero + vector3f::ux == vector3f::ux);
		CHECK(vector3f::zero + vector3f::uy == vector3f::uy);
		CHECK(vector3f::zero + vector3f::uz == vector3f::uz);

		CHECK(vector3f::ux == vector3f::ux);
		CHECK(vector3f::ux != vector3f::zero);
		CHECK(vector3f::ux != vector3f::uy);
		CHECK(vector3f::ux != vector3f::uz);

		CHECK(vector3f::uy == vector3f::uy);
		CHECK(vector3f::uy != vector3f::zero);
		CHECK(vector3f::uy != vector3f::ux);
		CHECK(vector3f::uy != vector3f::uz);

		CHECK(vector3f::uz == vector3f::uz);
		CHECK(vector3f::uz != vector3f::zero);
		CHECK(vector3f::uz != vector3f::ux);
		CHECK(vector3f::uz != vector3f::uy);
	}

	TEST(Arithmetic)
	{
		CHECK(vector3f::zero * 2.f == vector3f::zero);
		CHECK(vector3f::ux * 2.f == vector3f::ux + vector3f::ux);
		CHECK(vector3f::uy * 2.f == vector3f::uy + vector3f::uy);
		CHECK(vector3f::uz * 2.f == vector3f::uz + vector3f::uz);

		for (int n = 0; n <= 10; ++n)
		{
			vector3f u1 = vector3f::zero;
			vector3f u2 = vector3f::zero;
			for (int i = 1; i <= n; ++i)
			{
				u1 += vector3f::ux;
				CHECK(u1 == float(i) * vector3f::ux);
				CHECK(u1 == vector3f::ux * float(i));

				u2 -= vector3f::ux;
				CHECK(u2 == float(i) * -vector3f::ux);
				CHECK(u2 == vector3f::ux * float(-i));
			}

			vector3f v1 = vector3f::zero;
			vector3f v2 = vector3f::zero;
			for (int i = 1; i <= n; ++i)
			{
				v1 += vector3f::uy;
				CHECK(v1 == float(i) * vector3f::uy);
				CHECK(v1 == vector3f::uy * float(i));

				v2 -= vector3f::uy;
				CHECK(v2 == float(i) * -vector3f::uy);
				CHECK(v2 == vector3f::uy * float(-i));
			}

			vector3f w1 = vector3f::zero;
			vector3f w2 = vector3f::zero;
			for (int i = 1; i <= n; ++i)
			{
				w1 += vector3f::uz;
				CHECK(w1 == float(i) * vector3f::uz);
				CHECK(w1 == vector3f::uz * float(i));

				w2 -= vector3f::uz;
				CHECK(w2 == float(i) * -vector3f::uz);
				CHECK(w2 == vector3f::uz * float(-i));
			}

			vector3f s1 = vector3f::zero;
			vector3f s2 = vector3f::zero;
			vector3f t1 = vector3f::ux + vector3f::uy + vector3f::uz;
			vector3f t2 = vector3f::ux + vector3f::uy + vector3f::uz;
			for (int i = 1; i <= n; ++i)
			{
				s1 += 1.f;
				CHECK(s1 == float(i) * (vector3f::ux + vector3f::uy + vector3f::uz));

				s2 -= 1.f;
				CHECK(s2 == float(-i) * (vector3f::ux + vector3f::uy + vector3f::uz));

				t1 *= 2.f;
				CHECK(t1 == Algebra::pow(2.f, float(i)) * (vector3f::ux + vector3f::uy + vector3f::uz));
				CHECK(t1 == (vector3f::ux + vector3f::uy + vector3f::uz) * Algebra::pow(2.f, float(i)));

				t2 /= 2.f;
				CHECK(t2 == Algebra::pow(0.5f, float(i)) * (vector3f::ux + vector3f::uy + vector3f::uz));
				CHECK(t2 == (vector3f::ux + vector3f::uy + vector3f::uz) / Algebra::pow(2.f, float(i)));
			}
		}
	}

	TEST(dot)
	{
		CHECK(dot(vector3f::zero, vector3f::zero) == 0.f);
		CHECK(dot(vector3f::ux, vector3f::zero) == 0.f);
		CHECK(dot(vector3f::uy, vector3f::zero) == 0.f);
		CHECK(dot(vector3f::uz, vector3f::zero) == 0.f);

		CHECK(dot(vector3f::ux, vector3f::ux) == 1.f);
		CHECK(dot(vector3f::ux, -vector3f::ux) == -1.f);
		CHECK(dot(vector3f::ux, vector3f::uy) == 0.f);
		CHECK(dot(vector3f::ux, vector3f::uz) == 0.f);

		CHECK(dot(vector3f::uy, vector3f::uy) == 1.f);
		CHECK(dot(vector3f::uy, -vector3f::uy) == -1.f);
		CHECK(dot(vector3f::uy, vector3f::ux) == 0.f);
		CHECK(dot(vector3f::uy, vector3f::uz) == 0.f);

		CHECK(dot(vector3f::uz, vector3f::uz) == 1.f);
		CHECK(dot(vector3f::uz, -vector3f::uz) == -1.f);
		CHECK(dot(vector3f::uz, vector3f::ux) == 0.f);
		CHECK(dot(vector3f::uz, vector3f::uy) == 0.f);

		CHECK(dot(vector3f::ux + vector3f::uy, vector3f::ux - vector3f::uy) == 0.f);
		CHECK(dot(2.f * vector3f::ux, vector3f::ux) == 2.f);
		CHECK(dot(2.f * vector3f::ux, 3.f * vector3f::ux) == 6.f);
	}

	TEST(cross)
	{
		CHECK(cross(vector3f::zero, vector3f::zero) == vector3f::zero);
		CHECK(cross(vector3f::ux, vector3f::zero) == vector3f::zero);
		CHECK(cross(vector3f::uy, vector3f::zero) == vector3f::zero);
		CHECK(cross(vector3f::uz, vector3f::zero) == vector3f::zero);

		CHECK(cross(vector3f::ux, vector3f::ux) == vector3f::zero);
		CHECK(cross(vector3f::uy, vector3f::uy) == vector3f::zero);
		CHECK(cross(vector3f::uz, vector3f::uz) == vector3f::zero);

		CHECK(cross(vector3f::ux, vector3f::uy) == vector3f::uz);
		CHECK(cross(vector3f::uy, vector3f::uz) == vector3f::ux);
		CHECK(cross(vector3f::uz, vector3f::ux) == vector3f::uy);

		CHECK(cross(vector3f::uy, vector3f::ux) == -vector3f::uz);
		CHECK(cross(vector3f::uz, vector3f::uy) == -vector3f::ux);
		CHECK(cross(vector3f::ux, vector3f::uz) == -vector3f::uy);

		CHECK(cross(2.f * vector3f::ux, 3.f * vector3f::uy) == 6.f * vector3f::uz);
		CHECK(cross(2.f * vector3f::uy, 3.f * vector3f::uz) == 6.f * vector3f::ux);
		CHECK(cross(2.f * vector3f::uz, 3.f * vector3f::ux) == 6.f * vector3f::uy);
	}

	TEST(mul)
	{
		CHECK(mul(vector3f::ux, vector3f::zero) == vector3f::zero);
		CHECK(mul(vector3f::uy, vector3f::zero) == vector3f::zero);
		CHECK(mul(vector3f::uz, vector3f::zero) == vector3f::zero);

		CHECK(mul(vector3f::zero, vector3f::ux) == vector3f::zero);
		CHECK(mul(vector3f::zero, vector3f::uy) == vector3f::zero);
		CHECK(mul(vector3f::zero, vector3f::uz) == vector3f::zero);

		CHECK(mul(vector3f::ux, vector3f::uy) == vector3f::zero);
		CHECK(mul(vector3f::uy, vector3f::uz) == vector3f::zero);
		CHECK(mul(vector3f::uz, vector3f::ux) == vector3f::zero);

		CHECK(mul(vector3f::ux, -vector3f::ux) == -vector3f::ux);
		CHECK(mul(vector3f::uy, -vector3f::uy) == -vector3f::uy);
		CHECK(mul(vector3f::uz, -vector3f::uz) == -vector3f::uz);
	}

	TEST(norm)
	{
		for (int n = 1; n < 10000; n *= 3)
		{
			vector3f u = float(n) * vector3f::ux;
			CHECK_EQUAL(float(n), norm(u));

			vector3f v = float(n) * vector3f::uy;
			CHECK_EQUAL(float(n), norm(v));

			vector3f w = float(n) * vector3f::uz;
			CHECK_EQUAL(float(n), norm(w));

			vector3f a = float(n) * (vector3f::ux + vector3f::uy + vector3f::uz);
			CHECK_EQUAL(sqrtf(3.f * n * n), norm(a));
		}
	}

	TEST(normalize)
	{
		for (int n = 1; n < 10000; n *= 3)
		{
			vector3f u = (float(n) * vector3f::ux);
			vector3f u1 = normalized(u);
			CHECK_CLOSE(1.f, norm(u1), 0.0000001f);
			CHECK_CLOSE(0.f, norm(u1 - vector3f::ux), 0.0000001f);

			vector3f v = (float(n) * vector3f::uy);
			vector3f v1 = normalized(v);
			CHECK_CLOSE(1.f, norm(v1), 0.0000001f);
			CHECK_CLOSE(0.f, norm(v1 - vector3f::uy), 0.0000001f);

			vector3f w = (float(n) * vector3f::uz);
			vector3f w1 = normalized(w);
			CHECK_CLOSE(1.f, norm(w1), 0.0000001f);
			CHECK_CLOSE(0.f, norm(w1 - vector3f::uz), 0.0000001f);

			vector3f a = (float(n) * (vector3f::ux + vector3f::uy + vector3f::uz));
			vector3f a1 = normalized(a);
			CHECK_CLOSE(1.f, norm(a1), 0.0000001f);
			CHECK_CLOSE(0.f, norm(a1 - (vector3f::ux + vector3f::uy + vector3f::uz) / sqrtf(3.f)), 0.0000005f);
		}
	}
}
