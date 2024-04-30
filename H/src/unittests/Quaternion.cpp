#include "engine/algebra/Functions.hh"
#include "engine/algebra/Quaternion.hxx"
#include "engine/noise/Rand.hh"
#include "UnitTest++/UnitTest++.h"
#include <cstdio>

using namespace Algebra;

SUITE(Quaternion)
{
	TEST(Definition)
	{
		CHECK(quaternion::i * quaternion::i == -quaternion::identity);
		CHECK(quaternion::j * quaternion::j == -quaternion::identity);
		CHECK(quaternion::k * quaternion::k == -quaternion::identity);
		CHECK(quaternion::i * quaternion::j * quaternion::k == -quaternion::identity);
	}

	TEST(Multiplication)
	{
		CHECK(quaternion::i * quaternion::identity == quaternion::i);
		CHECK(quaternion::j * quaternion::identity == quaternion::j);
		CHECK(quaternion::k * quaternion::identity == quaternion::k);
		CHECK(quaternion::identity * quaternion::identity == quaternion::identity);

		CHECK(quaternion::i * quaternion::j == quaternion::k);
		CHECK(quaternion::j * quaternion::k == quaternion::i);
		CHECK(quaternion::k * quaternion::i == quaternion::j);

		CHECK(quaternion::j * quaternion::i == -quaternion::k);
		CHECK(quaternion::k * quaternion::j == -quaternion::i);
		CHECK(quaternion::i * quaternion::k == -quaternion::j);
	}

	TEST(Arithmetic)
	{
		const quaternion zero = { 0.f, 0.f, 0.f, 0.f };

		CHECK(zero * 2.f == zero);
		CHECK(quaternion::i * 2.f == quaternion::i + quaternion::i);
		CHECK(quaternion::j * 2.f == quaternion::j + quaternion::j);
		CHECK(quaternion::k * 2.f == quaternion::k + quaternion::k);
		CHECK(quaternion::identity * 2.f == quaternion::identity + quaternion::identity);

		for (int n = 0; n <= 10; ++n)
		{
			quaternion u1 = zero;
			quaternion u2 = zero;
			for (int i = 1; i <= n; ++i)
			{
				u1 += quaternion::i;
				CHECK(u1 == float(i) * quaternion::i);
				CHECK(u1 == quaternion::i * float(i));

				u2 -= quaternion::i;
				CHECK(u2 == float(i) * -quaternion::i);
				CHECK(u2 == quaternion::i * float(-i));
			}

			quaternion v1 = zero;
			quaternion v2 = zero;
			for (int i = 1; i <= n; ++i)
			{
				v1 += quaternion::j;
				CHECK(v1 == float(i) * quaternion::j);
				CHECK(v1 == quaternion::j * float(i));

				v2 -= quaternion::j;
				CHECK(v2 == float(i) * -quaternion::j);
				CHECK(v2 == quaternion::j * float(-i));
			}

			quaternion w1 = zero;
			quaternion w2 = zero;
			for (int i = 1; i <= n; ++i)
			{
				w1 += quaternion::k;
				CHECK(w1 == float(i) * quaternion::k);
				CHECK(w1 == quaternion::k * float(i));

				w2 -= quaternion::k;
				CHECK(w2 == float(i) * -quaternion::k);
				CHECK(w2 == quaternion::k * float(-i));
			}

			quaternion ww1 = zero;
			quaternion ww2 = zero;
			for (int i = 1; i <= n; ++i)
			{
				ww1 += quaternion::identity;
				CHECK(ww1 == float(i) * quaternion::identity);
				CHECK(ww1 == quaternion::identity * float(i));

				ww2 -= quaternion::identity;
				CHECK(ww2 == float(i) * -quaternion::identity);
				CHECK(ww2 == quaternion::identity * float(-i));
			}

			quaternion s1 = zero;
			quaternion s2 = zero;
			quaternion t1 = quaternion::i + quaternion::j + quaternion::k + quaternion::identity;
			quaternion t2 = quaternion::i + quaternion::j + quaternion::k + quaternion::identity;
			for (int i = 1; i <= n; ++i)
			{
				s1 += 1.f;
				CHECK(s1 == float(i) * (quaternion::i + quaternion::j + quaternion::k + quaternion::identity));

				s2 -= 1.f;
				CHECK(s2 == float(-i) * (quaternion::i + quaternion::j + quaternion::k + quaternion::identity));

				t1 *= 2.f;
				CHECK(t1 == Algebra::pow(2.f, float(i)) * (quaternion::i + quaternion::j + quaternion::k + quaternion::identity));
				CHECK(t1 == (quaternion::i + quaternion::j + quaternion::k + quaternion::identity) * Algebra::pow(2.f, float(i)));

				t2 /= 2.f;
				CHECK(t2 == Algebra::pow(0.5f, float(i)) * (quaternion::i + quaternion::j + quaternion::k + quaternion::identity));
				CHECK(t2 == (quaternion::i + quaternion::j + quaternion::k + quaternion::identity) / Algebra::pow(2.f, float(i)));
			}
		}
	}

	TEST(Dot)
	{
		const quaternion zero = { 0.f, 0.f, 0.f, 0.f };

		CHECK(dot(zero, zero) == 0.f);
		CHECK(dot(quaternion::i, zero) == 0.f);
		CHECK(dot(quaternion::j, zero) == 0.f);
		CHECK(dot(quaternion::k, zero) == 0.f);
		CHECK(dot(quaternion::identity, zero) == 0.f);

		CHECK(dot(quaternion::i, quaternion::i) == 1.f);
		CHECK(dot(quaternion::i, -quaternion::i) == -1.f);
		CHECK(dot(quaternion::i, quaternion::j) == 0.f);
		CHECK(dot(quaternion::i, quaternion::k) == 0.f);
		CHECK(dot(quaternion::i, quaternion::identity) == 0.f);

		CHECK(dot(quaternion::j, quaternion::j) == 1.f);
		CHECK(dot(quaternion::j, -quaternion::j) == -1.f);
		CHECK(dot(quaternion::j, quaternion::i) == 0.f);
		CHECK(dot(quaternion::j, quaternion::k) == 0.f);
		CHECK(dot(quaternion::j, quaternion::identity) == 0.f);

		CHECK(dot(quaternion::k, quaternion::k) == 1.f);
		CHECK(dot(quaternion::k, -quaternion::k) == -1.f);
		CHECK(dot(quaternion::k, quaternion::i) == 0.f);
		CHECK(dot(quaternion::k, quaternion::j) == 0.f);
		CHECK(dot(quaternion::k, quaternion::identity) == 0.f);

		CHECK(dot(quaternion::identity, quaternion::identity) == 1.f);
		CHECK(dot(quaternion::identity, -quaternion::identity) == -1.f);
		CHECK(dot(quaternion::identity, quaternion::i) == 0.f);
		CHECK(dot(quaternion::identity, quaternion::j) == 0.f);
		CHECK(dot(quaternion::identity, quaternion::k) == 0.f);

		CHECK(dot(quaternion::i + quaternion::j, quaternion::i - quaternion::j) == 0.f);
		CHECK(dot(2.f * quaternion::i, quaternion::i) == 2.f);
		CHECK(dot(2.f * quaternion::i, 3.f * quaternion::i) == 6.f);
	}

	TEST(norm)
	{
		CHECK_EQUAL(1.f, norm(quaternion::identity));

		for (int n = 1; n < 10000; n *= 3)
		{
			quaternion u = float(n) * quaternion::i;
			CHECK_EQUAL(float(n), norm(u));

			quaternion v = float(n) * quaternion::j;
			CHECK_EQUAL(float(n), norm(v));

			quaternion w = float(n) * quaternion::k;
			CHECK_EQUAL(float(n), norm(w));

			quaternion t = float(n) * quaternion::identity;
			CHECK_EQUAL(float(n), norm(t));

			quaternion a = float(n) * (quaternion::i + quaternion::j + quaternion::k + quaternion::identity);
			CHECK_EQUAL(sqrtf(4.f * n * n), norm(a));
		}
	}

	TEST(normalize)
	{
		for (int n = 1; n < 10000; n *= 3)
		{
			quaternion u = (float(n) * quaternion::i);
			quaternion u1 = normalized(u);
			CHECK_CLOSE(1.f, norm(u1), 0.00001f);
			CHECK_CLOSE(0.f, norm(u1 - quaternion::i), 0.00001f);

			quaternion v = (float(n) * quaternion::j);
			quaternion v1 = normalized(v);
			CHECK_CLOSE(1.f, norm(v1), 0.00001f);
			CHECK_CLOSE(0.f, norm(v1 - quaternion::j), 0.00001f);

			quaternion w = (float(n) * quaternion::k);
			quaternion w1 = normalized(w);
			CHECK_CLOSE(1.f, norm(w1), 0.00001f);
			CHECK_CLOSE(0.f, norm(w1 - quaternion::k), 0.00001f);

			quaternion t = (float(n) * quaternion::identity);
			quaternion t1 = normalized(t);
			CHECK_CLOSE(1.f, norm(t1), 0.00001f);
			CHECK_CLOSE(0.f, norm(t1 - quaternion::identity), 0.00001f);

			quaternion a = (float(n) * (quaternion::i + quaternion::j + quaternion::k + quaternion::identity));
			quaternion a1 = normalized(a);
			CHECK_CLOSE(1.f, norm(a1), 0.0000001f);
			CHECK_CLOSE(0.f, norm(a1 - (quaternion::i + quaternion::j + quaternion::k + quaternion::identity) / sqrtf(4.f)), 0.0000001f);
		}
	}

	TEST(compressQuaternion)
	{
		Noise::Rand rand;
		for (int i = 0; i < 1000; ++i)
		{
			Algebra::quaternion reference = { rand.sfgen(), rand.sfgen(), rand.sfgen(), rand.sfgen() };
			normalize(reference);

			Algebra::compressedQuaternion compressed = compressQuaternion(reference);
			Algebra::quaternion decompressed = compressed.q();

			float difference = norm(reference - decompressed);

			CHECK_CLOSE(0.f, difference, 1e-2f);
		}
	}
}
