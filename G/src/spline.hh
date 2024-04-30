#ifndef _SPLINE_H_
#define _SPLINE_H_

#include "array.hh"

void spline(const float *key, int num, int dim, float t, float *v);
void spline(const Array<float> & data, int dim, float t, float *v);
void spline4(const float *key, int dim, float t, float *v);
void computeSplineWeights(const float x, float * keys, float * weights);

void initSplineData(float * data, int size);

#endif
