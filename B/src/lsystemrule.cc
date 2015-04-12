//
// L system rule
//

#include "lsystemrule.hh"

#include "sys/msys.h"

#include "lsystemalphabet.hh"

namespace LSystem
{
  const char * alphabet = ALPHABET;

  //
  // Construit une règle à partir d'une chaîne lisible utilisant
  // le formalisme { F, +, -, &, ^, <, >, [, ] }
  //
  Rule::Rule(const char * src):
    _length(msys_strlen(src))
  {
    _rule = (char *)msys_mallocAlloc((_length + 1) * sizeof(char));

    for (int i = 0; i < _length; ++i)
    {
      _rule[i] = '\0';
      for (char k = 1; k < ALPHABET_LENGTH; ++k)
	if (src[i] == alphabet[k])
	{
	  _rule[i] = k;
	}
    }
    _rule[_length] = '\0';
  }

  Rule::~Rule()
  {
    msys_mallocFree(_rule);
  }

  int Rule::length() const
  {
    return _length;
  }

  char Rule::operator [](unsigned int i) const
  {
    return _rule[i];
  }
}
