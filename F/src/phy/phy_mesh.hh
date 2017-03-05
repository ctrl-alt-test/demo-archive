//
// Mesh: ensemble de points et de contraintes
//

#ifndef PHY_MESH_HH_
# define PHY_MESH_HH_



// Includes

# include "constraint.hh"
# include "particle.hh"



namespace phy
{


  // Mesh class

  class Mesh
  {
    public:
      Mesh(int p = 32, int c = 64, int f = 16);

      void move_of(const vector3f& p);
      void translate(const vector3f& p);
      void updateAABB();

      Array<Particle> particles;
      Array<Constraint> constraints;
      Array<int> faces;

      // AABB
      point3f min;
      point3f max;
  };



  // Constructors

  Mesh* chain(int size, float sep);
  Mesh* rope(int size, float sep_min, float sep_max);
  Mesh* cube(float size);



  // Functions

  float mobility(const Mesh* m);
  bool intersection(const Mesh* m1, const Mesh* m2);


}



#endif /* !PHY_MESH_HH_ */
