//
// Environnement physique : algorithme de collision GJK
//

#ifndef GJK_HH_
# define GJK_HH_



// Includes

# include "phy_mesh.hh"



namespace phy
{


  // GJK algorithm

  bool GJK(Mesh* m1, Mesh* m2, point3f& out1, point3f& out2);


}



#endif /* !GJK_HH_ */
