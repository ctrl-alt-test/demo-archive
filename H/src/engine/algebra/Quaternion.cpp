#include "Quaternion.hxx"

#include "engine/algebra/Functions.hh"
#include "engine/core/Debug.hh"
#include "Matrix.hh"
#include <cassert>

using namespace Algebra;

const quaternion quaternion::i = { 1.f, 0.f, 0.f, 0.f };
const quaternion quaternion::j = { 0.f, 1.f, 0.f, 0.f };
const quaternion quaternion::k = { 0.f, 0.f, 1.f, 0.f };
const quaternion quaternion::identity = { 0.f, 0.f, 0.f, 1.f, };

quaternion quaternion::rotation(float angle, float x, float y, float z)
{
	const float s = msys_sinf(angle / 2.f);
	quaternion result = {
		x * s,
		y * s,
		z * s,
		msys_cosf(angle / 2.f),
	};
	return result;
}

quaternion Algebra::operator * (const quaternion& a, const quaternion& b)
{
	quaternion result = {
		a.w * b.x + a.x * b.w + a.y * b.z - a.z * b.y,
		a.w * b.y + a.y * b.w + a.z * b.x - a.x * b.z,
		a.w * b.z + a.z * b.w + a.x * b.y - a.y * b.x,
		a.w * b.w - a.x * b.x - a.y * b.y - a.z * b.z
	};
	return result;
}

quaternion Algebra::slerp(const quaternion& a, const quaternion& b, float x)
{
	if (x <= 0.f) return a;
	if (x >= 1.f) return b;

	float cosTheta = dot(a, b);

	// Always return the shortest path.
	quaternion b_ = (cosTheta >= 0.f ? b : -b);
	cosTheta = Algebra::abs(cosTheta);

	if (cosTheta >= 1.f - 1e-03)
	{
		// a and b are close enough to interpolate; acos is losing
		// precision anyway.
		return a * (1.f - x) + b * x;
	}

	// slerp(a, b, x) = a * sin(theta*(1 - x))/sin(theta) + b * sin(theta*x)/sin(theta).
	const float sinTheta = msys_sqrtf(1.f - cosTheta * cosTheta);
	const float theta = msys_acosf(cosTheta);

	return (a * msys_sinf(theta * (1.f - x)) + b_ * msys_sinf(theta * x)) / sinTheta;
}

quaternion Algebra::computeQuaternion(const matrix4& a)
{
	quaternion q;

	const float trace = a._0 + a._5 + a._10;
	if (trace > 0)
	{
		const float s = 2.f * msys_sqrtf(1.f + trace);
		const float invS = 1.f / s;

		q.x = (a._9 - a._6) * invS;
		q.y = (a._2 - a._8) * invS;
		q.z = (a._4 - a._1) * invS;
		q.w = 0.25f * s;
	}
	else if (a._0 > a._5 && a._0 > a._10)
	{
		// a._0
		const float s = 2.f * msys_sqrtf(1.f + a._0 - a._5 - a._10);
		const float invS = 1.f / s;

		q.x = 0.25f * s;
		q.y = (a._1 + a._4) * invS;
		q.z = (a._2 + a._8) * invS;
		q.w = (a._9 - a._6) * invS;
	}
	else if (a._5 > a._10)
	{
		// a._5
		const float s = 2.f * msys_sqrtf(1.f - a._0 + a._5 - a._10);
		const float invS = 1.f / s;

		q.x = (a._1 + a._4) * invS;
		q.y = 0.25f * s;
		q.z = (a._6 + a._9) * invS;
		q.w = (a._2 - a._8) * invS;
	}
	else
	{
		// a._10
		const float s = 2.f * msys_sqrtf(1.f - a._0 - a._5 + a._10);
		const float invS = 1.f / s;

		q.x = (a._2 + a._8) * invS;
		q.y = (a._6 + a._9) * invS;
		q.z = 0.25f * s;
		q.w = (a._4 - a._1) * invS;
	}

	normalize(q);

	return q;
}

#if DEBUG

#define SCALE_TESTS 2048

compressedQuaternion Algebra::compressQuaternion(const quaternion& q, bool debug)
{
	compressedQuaternion bestBet = {
		char(q.x),
		char(q.y),
		char(q.z),
		char(q.w)
	};
	float bestDist = dist(q, bestBet.q());

	const float upper = 127.f / msys_max(msys_max(abs(q.x),
												  abs(q.y)),
										 msys_max(abs(q.z),
												  abs(q.w)));

	for (int i = 2; i <= SCALE_TESTS; ++i)
	{
		const float scale = upper * float(i)/float(SCALE_TESTS);
		const compressedQuaternion candidate = {
			char(scale * q.x),
			char(scale * q.y),
			char(scale * q.z),
			char(scale * q.w)
		};
		const quaternion q2 = candidate.q();
		const float candidateDist = dist(q, q2);

		if (debug)
		{
			const float error = 100.f * candidateDist;

			LOG_ERROR("Error: %.2f%%\t {%d, %d, %d, %d}\t-> {%.2f, %.2f, %.2f, %.2f}\t/ {%.2f, %.2f, %.2f, %.2f}",
				error,
				candidate.x, candidate.y, candidate.z, candidate.w,
				q2.x, q2.y, q2.z, q2.w,
				q.x, q.y, q.z, q.w);
		}

		if (candidateDist < bestDist)
		{
			bestBet = candidate;
			bestDist = candidateDist;
		}
	}

	return bestBet;
}

#endif
