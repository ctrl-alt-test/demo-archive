//
// Environnement physique : collection de meshs et d'obstacles.
//

#ifndef WORLD_HH_
# define WORLD_HH_



// Includes

# include "constants.hh"
# include "particle.hh"
# include "phy_mesh.hh"



namespace phy
{


  // World class

  class World
  {
    public:
      void init() { meshes.init(16); particles.init(32); constraints.init(64); time_ = 0; }

      void update(unsigned ms);

      Array<Mesh*> meshes;
      Array<Particle> particles;
      Array<Constraint> constraints;

    private:
      void update();

      unsigned time_;
  };


}



#endif /* !WORLD_HH_ */
