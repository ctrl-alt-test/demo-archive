#include "array.hh"
#include "variable.hh"

Variable::Variable(int* times, float* data, int dim, int size):
    _dim(dim), _size(size)
{
#if VAR_EDIT
  _times = (int*) msys_memdup(times, sizeof(int) * size);
  _data = (float*) msys_memdup(data, sizeof(float) * dim * size);
#else
  _times = times;
  _data = data;
#endif
}

void Variable::get(int time, float* out)
  {
    // recherche de la keyframe
    int i = 0;
    while (time >= _times[i+1]) i++;

    const float weight = (float)(time - _times[i]) / (_times[i+1] - _times[i]);

    i *= _dim;
    // interpolation linéaire
    for (int k = 0; k < _dim; k++)
	out[k] = _data[i+k] + weight * (_data[i+k+_dim] - _data[i+k]);
  }

#if VAR_EDIT
// Mettre à jour une variable en Debug (lors d'un message réseau)
void Variable::update(int* times, float* data, int dim, int size)
{
  msys_mallocFree(_times);
  msys_mallocFree(_data);
  _times = (int*) msys_memdup(times, sizeof(int) * size);
  _data = (float*) msys_memdup(data, sizeof(float) * dim * size);
  _times = times;
  _data = data;
  _dim = dim;
  _size = size;
}

namespace Variables
{
  struct Assoc
  {
    const char* name;
    Variable* var;
  };
  Array<Assoc> assoc;

  void define(char *name, Variable *var)
  {
    Assoc a = {name, var};
    assoc.add(a);
  }

  Variable* find(char *name)
  {
    for(int i = 0; i < assoc.size; i++)
    {
      if (strcmp(assoc.elt[i].name, name) == 0)
	return assoc.elt[i].var;
    }
    return NULL;
  }
}
#endif
