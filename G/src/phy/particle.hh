//
// Particule basique. Object "atomique".
//

#ifndef PARTICLE_HH_
# define PARTICLE_HH_



// Includes

# include "../algebra/vector3.hh"
# include "constants.hh"



namespace phy
{


  // Particle class

  class Particle
  {
    public:
      Particle(const point3f& pos, const vector3f& speed = vector3f());

      const point3f& position() const;
      vector3f speed() const;
      float radius() const;
      float mobility() const;
      float friction() const;

      void speed(const vector3f& s);
      void radius(float r);
      void mobility(float m);
      void friction(float f);

      void step_to(const point3f& p);
      void move_to(const point3f& p);
      void move_of(const vector3f& p);

      void translate(const vector3f& d);
      void teleport(const point3f& d);

    private:
      point3f pos_;
      point3f old_pos_;
      float radius_;
      float inv_mass_;
      float friction_;
  };
}

#endif /* !PARTICLE_HH_ */
