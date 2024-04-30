//
// Gestion des variables qui évoluent en fonction du temps
//

#ifndef		VARIABLE_HH
# define	VARIABLE_HH

# include "sys/msys.h"
# include "array.hh"
# include "algebra/vector2.hh"
# include "algebra/vector3.hh"
# include "algebra/vector4.hh"

// EDIT_MODE indique si les variables peuvent être changées dynamiquement

#if DEBUG
# define VAR_EDIT 1
#else
# define VAR_EDIT 0 // pas d'édition en release
#endif // DEBUG

#if !VAR_EDIT
namespace VariablesEnum {
# include "../data/exported-variableId.hh"
}
#endif

enum interpolationMode
{
  modeSpline,
  modeSmoothstep,
  modeLinear,
};

class Variable
{
public:
  Variable():_data(NULL), _dim(0), _size(0), _spline(modeSmoothstep) {}
  Variable(const float* data, int dim, int size, bool isSpline);

  const float * data() const { return _data; }
  int dim() const { return _dim; }
  int size() const { return _size; }
  bool isSpline() const { return _spline == modeSpline; }

  void get(int dim, float* out) const;
  void get(float time, int dim, float* out) const;

  float get1(float time) const;
  vector2f get2(float time) const;
  vector3f get3(float time) const;
  vector4f get4(float time) const;

  void update(float* data, int dim, int size, bool isSpline);
  float maxTime() const;
  void setSpline(interpolationMode mode) { _spline = mode; }

private:
#if VAR_EDIT
  float* _data;
#else
  const float* _data;
#endif
  int _dim;
  int _size;
  interpolationMode _spline;
};

#if VAR_EDIT
class Variables
{
public:
  struct Assoc
  {
    char name[80];
    Variable* var;
  };

private:
  Array<Assoc> assoc;

public:
  Variables();

  // Association d'un nom d'une variable à ses données
  Variable* find(char *name);
  Variable* tryFind(char *name);
  void define(char *name, Variable *var);
  void clear();

  const Array<Assoc> & All() const { return assoc; }

# define VAR(name) intro.variables.find(#name)
};

#else // !VAR_EDIT

# define VAR(name) (&allVariables[VariablesEnum::name])
// Global array which contains all variables
extern Variable allVariables[VariablesEnum::numberOfVariables];

#endif

#endif
