#include "engine/algebra/Functions.hh"
#include "engine/algebra/Vector2.hxx"
#include "UnitTest++/UnitTest++.h"
#include <cstdio>

using namespace Algebra;

SUITE(vector2)
{
	TEST(Equality)
	{
		CHECK(vector2f::zero == vector2f::zero);
		CHECK(vector2f::zero + vector2f::ux == vector2f::ux);
		CHECK(vector2f::zero + vector2f::uy == vector2f::uy);

		CHECK(vector2f::ux == vector2f::ux);
		CHECK(vector2f::ux != vector2f::zero);
		CHECK(vector2f::ux != vector2f::uy);

		CHECK(vector2f::uy == vector2f::uy);
		CHECK(vector2f::uy != vector2f::zero);
		CHECK(vector2f::uy != vector2f::ux);
	}

	TEST(Arithmetic)
	{
		CHECK(vector2f::zero * 2.f == vector2f::zero);
		CHECK(vector2f::ux * 2.f == vector2f::ux + vector2f::ux);
		CHECK(vector2f::uy * 2.f == vector2f::uy + vector2f::uy);

		for (int n = 0; n <= 10; ++n)
		{
			vector2f u1 = vector2f::zero;
			vector2f u2 = vector2f::zero;
			for (int i = 1; i <= n; ++i)
			{
				u1 += vector2f::ux;
				CHECK(u1 == float(i) * vector2f::ux);
				CHECK(u1 == vector2f::ux * float(i));

				u2 -= vector2f::ux;
				CHECK(u2 == float(i) * -vector2f::ux);
				CHECK(u2 == vector2f::ux * float(-i));
			}

			vector2f v1 = vector2f::zero;
			vector2f v2 = vector2f::zero;
			for (int i = 1; i <= n; ++i)
			{
				v1 += vector2f::uy;
				CHECK(v1 == float(i) * vector2f::uy);
				CHECK(v1 == vector2f::uy * float(i));

				v2 -= vector2f::uy;
				CHECK(v2 == float(i) * -vector2f::uy);
				CHECK(v2 == vector2f::uy * float(-i));
			}

			vector2f s1 = vector2f::zero;
			vector2f s2 = vector2f::zero;
			vector2f t1 = vector2f::ux + vector2f::uy;
			vector2f t2 = vector2f::ux + vector2f::uy;
			for (int i = 1; i <= n; ++i)
			{
				s1 += 1.f;
				CHECK(s1 == float(i) * (vector2f::ux + vector2f::uy));

				s2 -= 1.f;
				CHECK(s2 == float(-i) * (vector2f::ux + vector2f::uy));

				t1 *= 2.f;
				CHECK(t1 == Algebra::pow(2.f, float(i)) * (vector2f::ux + vector2f::uy));
				CHECK(t1 == (vector2f::ux + vector2f::uy) * Algebra::pow(2.f, float(i)));

				t2 /= 2.f;
				CHECK(t2 == Algebra::pow(0.5f, float(i)) * (vector2f::ux + vector2f::uy));
				CHECK(t2 == (vector2f::ux + vector2f::uy) / Algebra::pow(2.f, float(i)));
			}
		}
	}

	TEST(dot)
	{
		CHECK(dot(vector2f::zero, vector2f::zero) == 0.f);
		CHECK(dot(vector2f::ux, vector2f::zero) == 0.f);
		CHECK(dot(vector2f::uy, vector2f::zero) == 0.f);

		CHECK(dot(vector2f::ux, vector2f::ux) == 1.f);
		CHECK(dot(vector2f::ux, -vector2f::ux) == -1.f);
		CHECK(dot(vector2f::ux, vector2f::uy) == 0.f);

		CHECK(dot(vector2f::uy, vector2f::uy) == 1.f);
		CHECK(dot(vector2f::uy, -vector2f::uy) == -1.f);
		CHECK(dot(vector2f::uy, vector2f::ux) == 0.f);

		CHECK(dot(vector2f::ux + vector2f::uy, vector2f::ux - vector2f::uy) == 0.f);
		CHECK(dot(2.f * vector2f::ux, vector2f::ux) == 2.f);
		CHECK(dot(2.f * vector2f::ux, 3.f * vector2f::ux) == 6.f);
	}

	TEST(mul)
	{
		CHECK(mul(vector2f::ux, vector2f::zero) == vector2f::zero);
		CHECK(mul(vector2f::uy, vector2f::zero) == vector2f::zero);

		CHECK(mul(vector2f::zero, vector2f::ux) == vector2f::zero);
		CHECK(mul(vector2f::zero, vector2f::uy) == vector2f::zero);

		CHECK(mul(vector2f::ux, vector2f::uy) == vector2f::zero);

		CHECK(mul(vector2f::ux, -vector2f::ux) == -vector2f::ux);
		CHECK(mul(vector2f::uy, -vector2f::uy) == -vector2f::uy);
	}

	TEST(norm)
	{
		for (int n = 1; n < 10000; n *= 3)
		{
			vector2f u = float(n) * vector2f::ux;
			CHECK_EQUAL(float(n), norm(u));

			vector2f v = float(n) * vector2f::uy;
			CHECK_EQUAL(float(n), norm(v));

			vector2f a = float(n) * (vector2f::ux + vector2f::uy);
			CHECK_EQUAL(sqrtf(2.f * n * n), norm(a));
		}
	}

	TEST(normalize)
	{
		for (int n = 1; n < 10000; n *= 3)
		{
			vector2f u = (float(n) * vector2f::ux);
			vector2f u1 = normalized(u);
			CHECK_CLOSE(1.f, norm(u1), 0.0000001f);
			CHECK_CLOSE(0.f, norm(u1 - vector2f::ux), 0.0000001f);

			vector2f v = (float(n) * vector2f::uy);
			vector2f v1 = normalized(v);
			CHECK_CLOSE(1.f, norm(v1), 0.0000001f);
			CHECK_CLOSE(0.f, norm(v1 - vector2f::uy), 0.0000001f);

			vector2f a = (float(n) * (vector2f::ux + vector2f::uy));
			vector2f a1 = normalized(a);
			CHECK_CLOSE(1.f, norm(a1), 0.0000005f);
			CHECK_CLOSE(0.f, norm(a1 - (vector2f::ux + vector2f::uy) / sqrtf(2.f)), 0.0000001f);
		}
	}
}
