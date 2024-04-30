#ifndef SPLINE_HH
#define SPLINE_HH

namespace Container
{
	template<typename T>
	class Array;
}

namespace Algebra
{
	void spline(const float *key, int num, int dim, float t, float *v);
	void spline(const Container::Array<float> & data, int dim, float t, float *v);
	void spline4(const float *key, int dim, float t, float *v);
	void computeSplineWeights(const float x, float * keys, float * weights);

	void initSplineData(float * data, int size, int stride);
}

#endif // SPLINE_HH
