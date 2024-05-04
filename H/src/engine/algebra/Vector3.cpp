#include "Vector3.hxx"

Algebra::vector3f& Algebra::normalize(vector3f& v)
{
	const float n = norm(v);
	assert(n != 0);

	// SIZE: it can be worth trying one or the other near release.
	// Although in theory it should generate the same code, when I
	// tested the size changed from one to the other.
	v /= n;
	//v *= 1.f / n;
	return v;
}

Algebra::vector3f Algebra::normalized(const vector3f& v)
{
	const float n = norm(v);
	assert(n != 0);

	// SIZE: it can be worth trying one or the other near release.
	// Although in theory it should generate the same code, when I
	// tested the size changed from one to the other.
	return v * (1.f / n);
	//return v / n;
}
