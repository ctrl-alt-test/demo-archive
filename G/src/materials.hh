//
// Tous les matériaux utilisés
//

#ifndef		MATERIALS_HH
# define	MATERIALS_HH

#include "algebra/vector3.hh"
#include "loading.hh"
#include "material.hh"

namespace Material
{
  extern Element * list;

  extern vector3f seaColor;
  extern vector3f hillsColor;
  extern vector3f treesColor;
  extern vector3f trunkColor;
  extern vector3f roofsColor;
  extern vector3f road1Color;
  extern vector3f road2Color;
  extern vector3f tunnelColor;
  extern vector3f cityLightColor;

  extern vector3f beautifulWhite;
  extern vector3f beautifulBlack;
  extern vector3f beautifulRed;
  extern vector3f beautifulYellow;
  extern vector3f beautifulGreen;
  extern vector3f beautifulBlue;

  void loadMaterials();
}

#endif		// MATERIAL_HH
