// Includes

#include "particle.hh"
#include "sys/msys_debug.h"

namespace phy
{


  // Particle class

  Particle::Particle(const point3f& pos, const vector3f& s)
    : pos_(pos), radius_(1.f), inv_mass_(1.f), friction_(0.99f)
  {
    speed(s);
  }


  void
  Particle::step_to(const point3f& p)
  {
    old_pos_ = pos_;
    pos_ = p;
  }

  void
  Particle::move_to(const point3f& p)
  {
    pos_ = p;
  }

  void
  Particle::move_of(const vector3f& p)
  {
    pos_ += p;
  }

  void
  Particle::speed(const vector3f& s)
  {
    old_pos_ = pos_ - s * PHY_DELTA;
  }

  void
  Particle::radius(float r)
  {
    assert(r > 0);
    radius_ = r;
  }

  void
  Particle::mobility(float m)
  {
    inv_mass_ = m;
  }

  void
  Particle::friction(float f)
  {
    friction_ = f;
  }

  void
  Particle::translate(const vector3f& p)
  {
    old_pos_ += p;
    pos_ += p;
  }

  void
  Particle::teleport(const point3f& p)
  {
    old_pos_ = p + pos_ - old_pos_;
    pos_ = p;
  }


}
