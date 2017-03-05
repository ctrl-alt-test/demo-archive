#ifndef CONSTRAINT_HXX_
# define CONSTRAINT_HXX_



// Includes

# include "constraint.hh"



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



#endif /* !CONSTRAINT_HXX_ */
