#include "Spline.hh"

#include "engine/container/Array.hh"
#include <cassert>

using namespace Algebra;

// From IQ
// Adapted from http://iquilezles.org/www/articles/minispline/minispline.htm
static const float s_coefs[16] = {
    -1, 2,-1, 0,
	3, -5, 0, 2,
    -3, 4, 1, 0,
	1, -1, 0, 0,
};

// key: array of points to interpolate {t1, x1, y1, t2, x2, y2, t3...}
// num: number of points
// dim: dimension of the points
// t: time	
// v: output (will contain dim float values)
// key must contain (dim + 1) * num float values.
void Algebra::spline(const float *key, int num, int dim, float t, float *v)
{
    const int size = dim + 1;

    // find key
    int k = 0;
	while (k < num && key[k*size] < t) k++;

	if (k == num || key[k*size] == t)
    {
		if (k == num) k--; // time is outside the array, let's clamp and use the last point
		for (int i=0; i < dim; i++)
			v[i] = key[k*size + i + 1];
		return;
    }

    // interpolant
    const float h = (t-key[(k-1) * size]) / (key[k*size] - key[(k-1) * size]);

    // init result
    for (int i = 0; i < dim; ++i)
	{
		v[i] = 0.0f;
	}

    // add basis functions
    for (int i=0; i < 4; i++)
    {
        int kn = k+i-2; if( kn<0 ) kn=0; else if( kn>(num-1) ) kn=num-1;

        const float * co = s_coefs + 4*i;
        const float b  = 0.5f*(((co[0]*h + co[1])*h + co[2])*h + co[3]);
        for (int j = 0; j < dim; j++)
            v[j] += b * key[kn*size+j+1];
    }
}

void Algebra::spline(const Container::Array<float> & data, int dim, float t, float *v)
{
	return spline(data.elt, data.size / (dim + 1), dim, t, v);
}

// Spline avec 4 points : 2 avant et 2 apres t.
void Algebra::spline4(const float *key, int dim, float t, float *v)
{
    const int size = dim + 1;

    assert(key[size] <= t);
    assert(key[2*size] >= t);
    assert(key[size] != key[2*size]);

    // interpolant
    const float h = (t-key[size])/(key[2*size]-key[size]);

    // init result
    for (int i = 0; i < dim; ++i)
	{
		v[i] = 0.0f;
	}

    // add basis functions
    for (int i = 0; i<4; ++i)
    {
        const float *co = s_coefs + 4*i;
        const float b  = 0.5f*(((co[0]*h + co[1])*h + co[2])*h + co[3]);
        for( int j=0; j < dim; j++ ) v[j] += b * key[i*size+j+1];
    }
}

// Calcule les poids d'une spline avec 4 points : 2 avant et 2 apres t.
void Algebra::computeSplineWeights(const float x, float * keys, float * weights)
{
    // interpolant
    const float h = (x - keys[1]) / (keys[2] - keys[1]);

    // add basis functions
    for (int i = 0; i < 4; ++i)
    {
        const float * co = s_coefs + 4 * i;
        weights[i] = 0.5f * (((co[0] * h + co[1]) * h + co[2]) * h + co[3]);
    }
}

void Algebra::initSplineData(float * data, int size, int stride)
{
	for (int i = 0; i < size; i += stride)
	{
		data[i] = (float)i / (size - stride);
	}
}
