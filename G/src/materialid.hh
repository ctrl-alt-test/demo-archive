//
// Identifiants de tous les matériaux utilisés
//

#ifndef		MATERIAL_ID_HH
# define	MATERIAL_ID_HH

namespace Material
{
  enum id : unsigned char
    {
      none = 0,

      star,
      text3D,
      particles,
      retro,
      retroSeagul,
      retroGrowing,
      retroDeveloping,
      danceFloor,
      white,
      hills,
      poppingBuilding,
      silver,
      sea,
      floatingBoat,
      light,
      tunnelLight,

      // Les matériaux de test doivent être en dernier :
      // ça permet de faire une comparaison d'id
#ifdef DEBUG

      testAniso0,
      testAniso1,
      testAniso2,
      testAniso3,
      testAniso4,
      testAniso5,
      testAniso6,
      testAniso7,
      testAniso8,

      testIso0,
      testIso1,
      testIso2,
      testIso3,
      testIso4,
      testIso5,
      testIso6,
      testIso7,
      testIso8,

#endif // DEBUG

      numberOfMaterials
    };
}

#endif		// MATERIAL_ID_HH
