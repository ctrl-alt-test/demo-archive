#ifndef CONSTRAINT_HXX
# define CONSTRAINT_HXX

#include "constraint.hh"
#include "sys/msys_debug.h"

namespace phy
{
  // Constraint class

  inline bool
  Constraint::satisfy()
  {
    assert(function);
    return function(*this);
  }
}

#endif // !CONSTRAINT_HXX
