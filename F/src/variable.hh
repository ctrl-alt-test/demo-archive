//
// Gestion des variables qui évoluent en fonction du temps
//

#ifndef		VARIABLE_HH
# define	VARIABLE_HH
# include "sys/msys.h"

// EDIT_MODE indique si les variables peuvent être changées dynamiquement

#if DEBUG
# define VAR_EDIT 1
#else
# define VAR_EDIT 0 // pas d'édition en release
#endif

class Variable
{
public:
  Variable(int* times, float* data, int dim, int size);
  void get(int time, float* out);

  void update(int* times, float* data, int dim, int size);


private:
  float* _data;
  int* _times;
  int _dim;
  int _size;
};

namespace Variables
{
#if VAR_EDIT
  // Association d'un nom d'une variable à ses données
  Variable* find(char *name);
  void define(char *name, Variable *var);
#endif
}

#endif
