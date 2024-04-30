//
// Bâtiment
//

#ifndef		BUILDING_HH
# define	BUILDING_HH

#include "algebra/vector3.hh"
#include "mesh.hh"

class Rand;

namespace Mesh
{
  class Building
  {
  public:
    enum levelOfDetail
    {
      minDetail,
      medDetail,
      maxDetail,
    };

    // averageHieght: hauteur souhaitée
    //
    // maxWidth, maxLength: surface rectangulaire disponible
    // /* base: contour de la surface disponible */
    //
    // style: forme du bâtiment (à définir)
    //        on pourrait avoir N paramètres, 2 devraient suffire pour
    //        commencer; par exemple symmétrique ou non, base
    //        rectangulaire ou quelconque, style classique ou moderne,
    //        etc.
    Building(float averageHeight,
	     float maxWidth, float maxLength,
// 	     const Array<vector2f> & base,
	     const vector2f& style):
      _averageHeight(averageHeight),
      _maxWidth(maxWidth),
      _maxLength(maxLength),
//       _base(base.size),
      _style(style)
    {
//       assert(base.size >= 3);
//       for (int i = 0; i < base.size; ++i)
//       {
//         _base.add(base[i]);
//       }
    }

    void generateMesh(Mesh::MeshStruct & mesh,
		      float id,
		      float progression,
		      float noise,
		      Rand & rand,
		      levelOfDetail lod);

  private:
    float		_averageHeight;
    float		_maxWidth;
    float		_maxLength;
//     Array<vector2f> _base;
    vector2f		_style;
  };
}

#endif
