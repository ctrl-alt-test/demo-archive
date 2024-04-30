//
// Contrainte entre deux particules.
//

#ifndef CONSTRAINT_HH_
# define CONSTRAINT_HH_



// Includes

# include "particle.hh"



namespace phy
{


  // Constraint class

  class Constraint
  {
    public:
      typedef bool (*ConstraintFunction) (Constraint&);
      Constraint(ConstraintFunction fct);

      bool satisfy();

      ConstraintFunction function;
      Particle* particles[2];;
      float data1;
      float data2;
  };



  // Constraint functions

  bool cstf_rigid(Constraint& c);
  bool cstf_supple(Constraint& c);
  bool cstf_identity(Constraint& c);
  bool cstf_breakable(Constraint& c);



  // Constructors

  Constraint rigid_constraint(Particle* p1, Particle* p2, float d);
  Constraint breakable_constraint(Particle* p1, Particle* p2, float limit);
  Constraint supple_constraint(Particle* p1, Particle* p2, float dmin, float dmax);
  Constraint identity(Particle* p1, Particle* p2);


}

#endif /* !CONSTRAINT_HH_ */
