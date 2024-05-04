#include "engine/algebra/Functions.hh"
#include "engine/algebra/Vector4.hxx"
#include "UnitTest++/UnitTest++.h"
#include <cstdio>

using namespace Algebra;

SUITE(vector4)
{
	TEST(Equality)
	{
		CHECK(vector4f::zero == vector4f::zero);
		CHECK(vector4f::zero + vector4f::ux == vector4f::ux);
		CHECK(vector4f::zero + vector4f::uy == vector4f::uy);
		CHECK(vector4f::zero + vector4f::uz == vector4f::uz);
		CHECK(vector4f::zero + vector4f::uw == vector4f::uw);

		CHECK(vector4f::ux == vector4f::ux);
		CHECK(vector4f::ux != vector4f::zero);
		CHECK(vector4f::ux != vector4f::uy);
		CHECK(vector4f::ux != vector4f::uz);
		CHECK(vector4f::ux != vector4f::uw);

		CHECK(vector4f::uy == vector4f::uy);
		CHECK(vector4f::uy != vector4f::zero);
		CHECK(vector4f::uy != vector4f::ux);
		CHECK(vector4f::uy != vector4f::uz);
		CHECK(vector4f::uy != vector4f::uw);

		CHECK(vector4f::uz == vector4f::uz);
		CHECK(vector4f::uz != vector4f::zero);
		CHECK(vector4f::uz != vector4f::ux);
		CHECK(vector4f::uz != vector4f::uy);
		CHECK(vector4f::uz != vector4f::uw);

		CHECK(vector4f::uw == vector4f::uw);
		CHECK(vector4f::uw != vector4f::zero);
		CHECK(vector4f::uw != vector4f::ux);
		CHECK(vector4f::uw != vector4f::uy);
		CHECK(vector4f::uw != vector4f::uz);
	}

	TEST(Arithmetic)
	{
		CHECK(vector4f::zero * 2.f == vector4f::zero);
		CHECK(vector4f::ux * 2.f == vector4f::ux + vector4f::ux);
		CHECK(vector4f::uy * 2.f == vector4f::uy + vector4f::uy);
		CHECK(vector4f::uz * 2.f == vector4f::uz + vector4f::uz);
		CHECK(vector4f::uw * 2.f == vector4f::uw + vector4f::uw);

		for (int n = 0; n <= 10; ++n)
		{
			vector4f u1 = vector4f::zero;
			vector4f u2 = vector4f::zero;
			for (int i = 1; i <= n; ++i)
			{
				u1 += vector4f::ux;
				CHECK(u1 == float(i) * vector4f::ux);
				CHECK(u1 == vector4f::ux * float(i));

				u2 -= vector4f::ux;
				CHECK(u2 == float(i) * -vector4f::ux);
				CHECK(u2 == vector4f::ux * float(-i));
			}

			vector4f v1 = vector4f::zero;
			vector4f v2 = vector4f::zero;
			for (int i = 1; i <= n; ++i)
			{
				v1 += vector4f::uy;
				CHECK(v1 == float(i) * vector4f::uy);
				CHECK(v1 == vector4f::uy * float(i));

				v2 -= vector4f::uy;
				CHECK(v2 == float(i) * -vector4f::uy);
				CHECK(v2 == vector4f::uy * float(-i));
			}

			vector4f w1 = vector4f::zero;
			vector4f w2 = vector4f::zero;
			for (int i = 1; i <= n; ++i)
			{
				w1 += vector4f::uz;
				CHECK(w1 == float(i) * vector4f::uz);
				CHECK(w1 == vector4f::uz * float(i));

				w2 -= vector4f::uz;
				CHECK(w2 == float(i) * -vector4f::uz);
				CHECK(w2 == vector4f::uz * float(-i));
			}

			vector4f ww1 = vector4f::zero;
			vector4f ww2 = vector4f::zero;
			for (int i = 1; i <= n; ++i)
			{
				ww1 += vector4f::uw;
				CHECK(ww1 == float(i) * vector4f::uw);
				CHECK(ww1 == vector4f::uw * float(i));

				ww2 -= vector4f::uw;
				CHECK(ww2 == float(i) * -vector4f::uw);
				CHECK(ww2 == vector4f::uw * float(-i));
			}

			vector4f s1 = vector4f::zero;
			vector4f s2 = vector4f::zero;
			vector4f t1 = vector4f::ux + vector4f::uy + vector4f::uz + vector4f::uw;
			vector4f t2 = vector4f::ux + vector4f::uy + vector4f::uz + vector4f::uw;
			for (int i = 1; i <= n; ++i)
			{
				s1 += 1.f;
				CHECK(s1 == float(i) * (vector4f::ux + vector4f::uy + vector4f::uz + vector4f::uw));

				s2 -= 1.f;
				CHECK(s2 == float(-i) * (vector4f::ux + vector4f::uy + vector4f::uz + vector4f::uw));

				t1 *= 2.f;
				CHECK(t1 == Algebra::pow(2.f, float(i)) * (vector4f::ux + vector4f::uy + vector4f::uz + vector4f::uw));
				CHECK(t1 == (vector4f::ux + vector4f::uy + vector4f::uz + vector4f::uw) * Algebra::pow(2.f, float(i)));

				t2 /= 2.f;
				CHECK(t2 == Algebra::pow(0.5f, float(i)) * (vector4f::ux + vector4f::uy + vector4f::uz + vector4f::uw));
				CHECK(t2 == (vector4f::ux + vector4f::uy + vector4f::uz + vector4f::uw) / Algebra::pow(2.f, float(i)));
			}
		}
	}

	TEST(dot)
	{
		CHECK(dot(vector4f::zero, vector4f::zero) == 0.f);
		CHECK(dot(vector4f::ux, vector4f::zero) == 0.f);
		CHECK(dot(vector4f::uy, vector4f::zero) == 0.f);
		CHECK(dot(vector4f::uz, vector4f::zero) == 0.f);
		CHECK(dot(vector4f::uw, vector4f::zero) == 0.f);

		CHECK(dot(vector4f::ux, vector4f::ux) == 1.f);
		CHECK(dot(vector4f::ux, -vector4f::ux) == -1.f);
		CHECK(dot(vector4f::ux, vector4f::uy) == 0.f);
		CHECK(dot(vector4f::ux, vector4f::uz) == 0.f);
		CHECK(dot(vector4f::ux, vector4f::uw) == 0.f);

		CHECK(dot(vector4f::uy, vector4f::uy) == 1.f);
		CHECK(dot(vector4f::uy, -vector4f::uy) == -1.f);
		CHECK(dot(vector4f::uy, vector4f::ux) == 0.f);
		CHECK(dot(vector4f::uy, vector4f::uz) == 0.f);
		CHECK(dot(vector4f::uy, vector4f::uw) == 0.f);

		CHECK(dot(vector4f::uz, vector4f::uz) == 1.f);
		CHECK(dot(vector4f::uz, -vector4f::uz) == -1.f);
		CHECK(dot(vector4f::uz, vector4f::ux) == 0.f);
		CHECK(dot(vector4f::uz, vector4f::uy) == 0.f);
		CHECK(dot(vector4f::uz, vector4f::uw) == 0.f);

		CHECK(dot(vector4f::uw, vector4f::uw) == 1.f);
		CHECK(dot(vector4f::uw, -vector4f::uw) == -1.f);
		CHECK(dot(vector4f::uw, vector4f::ux) == 0.f);
		CHECK(dot(vector4f::uw, vector4f::uy) == 0.f);
		CHECK(dot(vector4f::uw, vector4f::uz) == 0.f);

		CHECK(dot(vector4f::ux + vector4f::uy, vector4f::ux - vector4f::uy) == 0.f);
		CHECK(dot(2.f * vector4f::ux, vector4f::ux) == 2.f);
		CHECK(dot(2.f * vector4f::ux, 3.f * vector4f::ux) == 6.f);
	}

	TEST(mul)
	{
		CHECK(mul(vector4f::ux, vector4f::zero) == vector4f::zero);
		CHECK(mul(vector4f::uy, vector4f::zero) == vector4f::zero);
		CHECK(mul(vector4f::uz, vector4f::zero) == vector4f::zero);
		CHECK(mul(vector4f::uw, vector4f::zero) == vector4f::zero);

		CHECK(mul(vector4f::zero, vector4f::ux) == vector4f::zero);
		CHECK(mul(vector4f::zero, vector4f::uy) == vector4f::zero);
		CHECK(mul(vector4f::zero, vector4f::uz) == vector4f::zero);
		CHECK(mul(vector4f::zero, vector4f::uw) == vector4f::zero);

		CHECK(mul(vector4f::ux, vector4f::uy) == vector4f::zero);
		CHECK(mul(vector4f::uy, vector4f::uz) == vector4f::zero);
		CHECK(mul(vector4f::uz, vector4f::uw) == vector4f::zero);
		CHECK(mul(vector4f::uw, vector4f::ux) == vector4f::zero);

		CHECK(mul(vector4f::ux, -vector4f::ux) == -vector4f::ux);
		CHECK(mul(vector4f::uy, -vector4f::uy) == -vector4f::uy);
		CHECK(mul(vector4f::uz, -vector4f::uz) == -vector4f::uz);
		CHECK(mul(vector4f::uw, -vector4f::uw) == -vector4f::uw);
	}

	TEST(norm)
	{
		for (int n = 1; n < 10000; n *= 3)
		{
			vector4f u = float(n) * vector4f::ux;
			CHECK_EQUAL(float(n), norm(u));

			vector4f v = float(n) * vector4f::uy;
			CHECK_EQUAL(float(n), norm(v));

			vector4f w = float(n) * vector4f::uz;
			CHECK_EQUAL(float(n), norm(w));

			vector4f t = float(n) * vector4f::uw;
			CHECK_EQUAL(float(n), norm(t));

			vector4f a = float(n) * (vector4f::ux + vector4f::uy + vector4f::uz + vector4f::uw);
			CHECK_EQUAL(sqrtf(4.f * n * n), norm(a));
		}
	}

	TEST(normalize)
	{
		for (int n = 1; n < 10000; n *= 3)
		{
			vector4f u = (float(n) * vector4f::ux);
			vector4f u1 = normalized(u);
			CHECK_CLOSE(1.f, norm(u1), 0.0000001f);
			CHECK_CLOSE(0.f, norm(u1 - vector4f::ux), 0.0000001f);

			vector4f v = (float(n) * vector4f::uy);
			vector4f v1 = normalized(v);
			CHECK_CLOSE(1.f, norm(v1), 0.0000001f);
			CHECK_CLOSE(0.f, norm(v1 - vector4f::uy), 0.0000001f);

			vector4f w = (float(n) * vector4f::uz);
			vector4f w1 = normalized(w);
			CHECK_CLOSE(1.f, norm(w1), 0.0000001f);
			CHECK_CLOSE(0.f, norm(w1 - vector4f::uz), 0.0000001f);

			vector4f t = (float(n) * vector4f::uw);
			vector4f t1 = normalized(t);
			CHECK_CLOSE(1.f, norm(t1), 0.0000001f);
			CHECK_CLOSE(0.f, norm(t1 - vector4f::uw), 0.0000001f);

			vector4f a = (float(n) * (vector4f::ux + vector4f::uy + vector4f::uz + vector4f::uw));
			vector4f a1 = normalized(a);
			CHECK_CLOSE(1.f, norm(a1), 0.0000001f);
			CHECK_CLOSE(0.f, norm(a1 - (vector4f::ux + vector4f::uy + vector4f::uz + vector4f::uw) / sqrtf(4.f)), 0.0000001f);
		}
	}
}
