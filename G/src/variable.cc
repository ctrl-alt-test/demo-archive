
#include "variable.hh"

#include "array.hxx"
#include "interpolation.hh"
#include "interpolation.hxx"
#include "spline.hh"

#include "sys/msys_libc.h"
#include "sys/msys_malloc.h"

Variable::Variable(const float* data, int dim, int size, bool isSpline):
  _dim(dim), _size(size), _spline(isSpline ? modeSpline : modeSmoothstep)
{
#if VAR_EDIT
  _data = (float*) msys_memdup(data, sizeof(float) * dim * size);
#else
  _data = data;
#endif
}

void Variable::get(int dim, float* out) const
{
  assert(dim == _dim);
  for (int i = 0; i < _dim; ++i)
    out[i] = _data[i];
}

void Variable::get(float time, int dim, float* out) const
{
  assert(dim == _dim - 1);

  if (_spline == modeSpline)
  {
    spline(_data, _size, _dim - 1, time, out);
    return;
  }

  // Interpolation smoothstep

  // recherche de la keyframe
  int i = 0;
  while (i + _dim < _size * _dim &&
	 time >= _data[i + _dim])
    i += _dim;

  if (i + _dim >= _size * _dim) { // after the last
    for (int k = 0; k < dim; k++)
      out[k] = _data[i + k + 1];
    return;
  }

  float weight;
  if (_spline == modeSmoothstep)
    weight = smoothStep(_data[i], _data[i+_dim], time);
  else // linear
    weight = interpolate(time, _data[i], _data[i+_dim]);
  i++;

  // interpolation
  for (int k = 0; k < dim; k++)
    out[k] = mix(_data[i+k], _data[i+k+_dim], weight);
  //out[k] = _data[i+k] + weight * (_data[i+k+_dim] - _data[i+k]);
}

// Type-safe accessors
float Variable::get1(float time) const
{
  float res;
  get(time, _dim - 1, &res);
  return res;
}

vector2f Variable::get2(float time) const
{
  vector2f res;
  get(time, _dim - 1, &res.x);
  return res;
}

vector3f Variable::get3(float time) const
{
  vector3f res;
  get(time, _dim - 1, &res.x);
  return res;
}

vector4f Variable::get4(float time) const
{
  vector4f res;
  get(time, _dim - 1, &res.x);
  return res;
}

float Variable::maxTime() const
{
  assert(_size > 0);
  return _data[(_size - 1) * _dim];
}

#if VAR_EDIT

// Mettre à jour une variable en Debug (lors d'un message réseau)
void Variable::update(float* data, int dim, int size, bool isSpline)
{
  msys_mallocFree(_data);
  _data = (float*) msys_memdup(data, sizeof(float) * dim * size);
  _dim = dim;
  _size = size;
  _spline = isSpline ? modeSpline : modeSmoothstep;
}

Variables::Variables()
{
  assoc.init(500);
}

void Variables::define(char *name, Variable *var)
{
  assert(assoc.max_size != 0);

  Assoc a = {0};
  a.var = var;
  msys_strcpy(a.name, name);
  assoc.add(a);
}

Variable* Variables::tryFind(char *name)
{
  for(int i = 0; i < assoc.size; i++)
  {
    if (msys_strcmp(assoc.elt[i].name, name) == 0)
      return assoc.elt[i].var;
  }
  return NULL;
}

Variable* Variables::find(char *name)
{
  Variable* v = tryFind(name);
  assert(v != NULL);
  return v;
}

void Variables::clear()
{
  for (int i = 0; i < assoc.size; i++)
    delete assoc[i].var;
  assoc.empty();
}

#else

Variable allVariables[VariablesEnum::numberOfVariables];

#endif
