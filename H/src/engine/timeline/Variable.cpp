#include "Variable.hh"

#include "engine/algebra/Interpolation.hxx"
#include "engine/algebra/Spline.hh"
#include "engine/container/Array.hxx"

#include "engine/core/msys_temp.hh"
#include <cassert>

using namespace Timeline;

Variable::Variable():
	_data(NULL), _dim(0), _size(0), _interpolation(modeSmoothstep)
{
}

Variable::Variable(const float* data, int dim, int size, InterpolationMode interpolation):
	_dim(dim), _size(size), _interpolation(interpolation)
{
#if VAR_EDIT
	_data = (float*)msys_memdup(data, sizeof(float) * dim * size);
#else
	_data = data;
#endif
}

void Variable::get(int dim, float* out) const
{
	assert(dim == _dim);
	assert(_size == 1);
	for (int i = 0; i < _dim; ++i)
	{
		out[i] = _data[i];
	}
}

float Variable::get1() const
{
	float res;
	get(1, &res);
	return res;
}

Algebra::vector2f Variable::get2() const
{
	Algebra::vector2f res;
	get(2, &res.x);
	return res;
}

Algebra::vector3f Variable::get3() const
{
	Algebra::vector3f res;
	get(3, &res.x);
	return res;
}

Algebra::vector4f Variable::get4() const
{
	Algebra::vector4f res;
	get(4, &res.x);
	return res;
}

void Variable::get(float time, int dim, float* out) const
{
	assert(dim == _dim - 1);

	if (_interpolation == modeSpline)
	{
		Algebra::spline(_data, _size, _dim - 1, time, out);
		return;
	}

	// Linear or bicubic interpolation

	// recherche de la keyframe
	int i = 0;
	while (i + _dim < _size * _dim &&
		   time >= _data[i + _dim])
	{
		i += _dim;
	}

	if (i + _dim >= _size * _dim) // after the last
	{
		for (int k = 0; k < dim; k++)
		{
			out[k] = _data[i + k + 1];
		}
		return;
	}

	float weight;
	if (_interpolation == modeSmoothstep)
	{
		weight = Algebra::smoothStep(_data[i], _data[i+_dim], time);
	}
	else // linear
	{
		weight = Algebra::interpolate(_data[i], _data[i+_dim], time);
	}
	i++;

	// interpolation
	for (int k = 0; k < dim; k++)
	{
		out[k] = Algebra::mix(_data[i+k], _data[i+k+_dim], weight);
	}
}

// Type-safe accessors
float Variable::get1(float time) const
{
	float res;
	get(time, 1, &res);
	return res;
}

Algebra::vector2f Variable::get2(float time) const
{
	Algebra::vector2f res;
	get(time, 2, &res.x);
	return res;
}

Algebra::vector3f Variable::get3(float time) const
{
	Algebra::vector3f res;
	get(time, 3, &res.x);
	return res;
}

Algebra::vector4f Variable::get4(float time) const
{
	Algebra::vector4f res;
	get(time, 4, &res.x);
	return res;
}

Algebra::vector3f Variable::getLightColor(float time) const
{
	Algebra::vector4f data;
	get(time, 4, &data.x);

	// Arbitrary choice: light intensity can go up to 10^5.
	float intensity = Algebra::pow(10.f, 5.f * data.w) - 1.f;
	Algebra::vector3f res = { data.x * intensity, data.y * intensity, data.z * intensity };
	return res;
}

float Variable::maxTime() const
{
	assert(_size > 0);
	return _data[(_size - 1) * _dim];
}

#if VAR_EDIT

// Mettre à jour une variable en Debug (lors d'un message réseau)
void Variable::update(float* data, int dim, int size, InterpolationMode interpolation)
{
	free(_data);
	_data = (float*)msys_memdup(data, sizeof(float) * dim * size);
	_dim = dim;
	_size = size;
	_interpolation = interpolation;
}

#endif // !VAR_EDIT
