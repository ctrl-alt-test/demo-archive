#ifndef PARTICLE_HXX_
# define PARTICLE_HXX_



// Includes

# include "particle.hh"



namespace phy
{


  // Particle class

  inline const point3f&
  Particle::position() const
  {
    return pos_;
  }

  inline vector3f
  Particle::speed() const
  {
    return pos_ - old_pos_;
  }

  inline float
  Particle::radius() const
  {
    return radius_;
  }

  inline float
  Particle::mobility() const
  {
    return inv_mass_;
  }

  inline float
  Particle::friction() const
  {
    return friction_;
  }


}



#endif /* !PARTICLE_HXX_ */
