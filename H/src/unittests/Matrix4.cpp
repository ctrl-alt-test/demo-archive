#include "engine/algebra/Functions.hh"
#include "engine/algebra/Matrix.hxx"
#include "engine/algebra/Utils.hh"
#include "engine/core/msys_temp.hh"
#include "engine/noise/Rand.hh"
#include "UnitTest++/UnitTest++.h"
#include <cstdio>

using namespace Algebra;

SUITE(matrix4)
{
	TEST(Equality)
	{
		CHECK(matrix4::identity == matrix4::identity);

		for (int i = 0; i < 16; ++i)
		{
			matrix4 m = matrix4::identity;
			m.m[i] = 2.f;
			CHECK(matrix4::identity != m);
		}
	}

	TEST(Rotation)
	{
		Noise::Rand rand;
		const float epsilon = 0.00001f;

		{
			const matrix4 mx = matrix4::rotation(PI / 2.f, vector3f::ux);
			const vector3f mx_uy = mx * vector3f::uy;
			const vector3f mx_uz = mx * vector3f::uz;
			CHECK_CLOSE(vector3f::uz.x, mx_uy.x, epsilon);
			CHECK_CLOSE(vector3f::uz.y, mx_uy.y, epsilon);
			CHECK_CLOSE(vector3f::uz.z, mx_uy.z, epsilon);

			CHECK_CLOSE(-vector3f::uy.x, mx_uz.x, epsilon);
			CHECK_CLOSE(-vector3f::uy.y, mx_uz.y, epsilon);
			CHECK_CLOSE(-vector3f::uy.z, mx_uz.z, epsilon);
		}
		{
			const matrix4 my = matrix4::rotation(PI / 2.f, vector3f::uy);
			const vector3f my_ux = my * vector3f::ux;
			const vector3f my_uz = my * vector3f::uz;
			CHECK_CLOSE(vector3f::ux.x, my_uz.x, epsilon);
			CHECK_CLOSE(vector3f::ux.y, my_uz.y, epsilon);
			CHECK_CLOSE(vector3f::ux.z, my_uz.z, epsilon);

			CHECK_CLOSE(-vector3f::uz.x, my_ux.x, epsilon);
			CHECK_CLOSE(-vector3f::uz.y, my_ux.y, epsilon);
			CHECK_CLOSE(-vector3f::uz.z, my_ux.z, epsilon);
		}
		{
			const matrix4 mz = matrix4::rotation(PI / 2.f, vector3f::uz);
			const vector3f mz_ux = mz * vector3f::ux;
			const vector3f mz_uy = mz * vector3f::uy;
			CHECK_CLOSE(vector3f::uy.x, mz_ux.x, epsilon);
			CHECK_CLOSE(vector3f::uy.y, mz_ux.y, epsilon);
			CHECK_CLOSE(vector3f::uy.z, mz_ux.z, epsilon);

			CHECK_CLOSE(-vector3f::ux.x, mz_uy.x, epsilon);
			CHECK_CLOSE(-vector3f::ux.y, mz_uy.y, epsilon);
			CHECK_CLOSE(-vector3f::ux.z, mz_uy.z, epsilon);
		}

		for (int i = 0; i < 10; ++i)
		{
			const float angle = 2.f * PI * rand.fgen();
			const float cosAngle = msys_cosf(angle);
			const float sinAngle = msys_sinf(angle);

			vector3f axis = { rand.sfgen(), rand.sfgen(), rand.sfgen() };
			normalize(axis);

			const vector3f p = {
				10.f * rand.sfgen(),
				10.f * rand.sfgen(),
				10.f * rand.sfgen()
			};

			const matrix4 m = matrix4::rotation(angle, axis);

			vector3f m_axis = axis; applyMatrixToPoint(m, m_axis);
			CHECK_CLOSE(axis.x, m_axis.x, epsilon);
			CHECK_CLOSE(axis.y, m_axis.y, epsilon);
			CHECK_CLOSE(axis.z, m_axis.z, epsilon);

			vector3f m_p = p; applyMatrixToPoint(m, m_p);
			CHECK_CLOSE(norm(p), norm(m_p), epsilon);

			if (dot(p, axis) < 0.99f)
			{
				vector3f h = p - dot(p, axis) * axis; normalize(h);
				vector3f m_h = m_p - dot(m_p, axis) * axis; normalize(m_h);
				CHECK_CLOSE(cosAngle, dot(h, m_h), epsilon);
			}
		}
	}

	TEST(Translation)
	{
		Noise::Rand rand;
		for (int i = 0; i < 10; ++i)
		{
			const vector3f t = {
				10.f * rand.sfgen(),
				10.f * rand.sfgen(),
				10.f * rand.sfgen()
			};
			const matrix4 m = matrix4::translation(t);

			{
				vector3f px = vector3f::ux;
				vector3f py = vector3f::uy;
				vector3f pz = vector3f::uz;
				applyMatrixToPoint(m, px);
				applyMatrixToPoint(m, py);
				applyMatrixToPoint(m, pz);
				CHECK(vector3f::ux + t == px);
				CHECK(vector3f::uy + t == py);
				CHECK(vector3f::uz + t == pz);
			}
			{
				vector3f ux = vector3f::ux;
				vector3f uy = vector3f::uy;
				vector3f uz = vector3f::uz;
				applyMatrixToDirection(m, ux);
				applyMatrixToDirection(m, uy);
				applyMatrixToDirection(m, uz);
				CHECK(vector3f::ux == ux);
				CHECK(vector3f::uy == uy);
				CHECK(vector3f::uz == uz);
			}
		}
	}

	TEST(TriviallyInvert)
	{
		Noise::Rand rand;
		const float epsilon = 0.0001f;

		for (int i = 0; i < 10; ++i)
		{
			const vector3f t = {
				10.f * rand.sfgen(),
				10.f * rand.sfgen(),
				10.f * rand.sfgen()
			};

			const float angle = 2.f * PI * rand.fgen();
			vector3f axis = {
				rand.sfgen(),
				rand.sfgen(),
				rand.sfgen()
			};
			normalize(axis);
			const matrix4 m = matrix4::translation(t).rotate(angle, axis);
			matrix4 inv_m = m;
			triviallyInvert(inv_m);

			const vector3f u = {
				10.f * rand.sfgen(),
				10.f * rand.sfgen(),
				10.f * rand.sfgen()
			};
			vector3f v = u;
			applyMatrixToPoint(m, v);
			vector3f w = v;
			applyMatrixToPoint(inv_m, w);

			CHECK_CLOSE(u.x, w.x, epsilon);
			CHECK_CLOSE(u.y, w.y, epsilon);
			CHECK_CLOSE(u.z, w.z, epsilon);
		}
	}
}
