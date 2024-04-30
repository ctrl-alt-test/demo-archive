//
// Tous les matériaux utilisés
//

#include "materials.hh"
#include "materialid.hh"
#include "tweakval.hh"
#include "sys/msys_libc.h"
#include "sys/msys_malloc.h"

namespace Material
{
  Element * list = NULL;

  vector3f seaColor;
  vector3f hillsColor;
  vector3f treesColor;
  vector3f trunkColor;
  vector3f roofsColor;
  vector3f road1Color;
  vector3f road2Color;
  vector3f tunnelColor;
  vector3f cityLightColor;

  vector3f beautifulWhite;
  vector3f beautifulBlack;
  vector3f beautifulRed;
  vector3f beautifulYellow;
  vector3f beautifulGreen;
  vector3f beautifulBlue;

  struct MaterialDesc
  {
    Element material;
    id id;
  };

  void loadMaterials()
  {
    seaColor =         vector3f(_TV(  4.f)/255.f, _TV( 34.f)/255.f, _TV( 64.f)/255.f);
    hillsColor =       vector3f(_TV( 87.f)/255.f, _TV(138.f)/255.f, _TV( 11.f)/255.f);
    treesColor =       vector3f(_TV( 51.f)/255.f, _TV( 96.f)/255.f, _TV( 12.f)/255.f);
    trunkColor =       vector3f(_TV(205.f)/255.f, _TV(127.f)/255.f, _TV( 50.f)/255.f);
    roofsColor =       vector3f(_TV( 14.f)/255.f, _TV( 14.f)/255.f, _TV( 50.f)/255.f);
    road1Color =       vector3f(_TV( 87.f)/255.f, _TV( 93.f)/255.f, _TV( 88.f)/255.f);
    road2Color =       vector3f(_TV( 75.f)/255.f, _TV( 83.f)/255.f, _TV( 76.f)/255.f);
    tunnelColor =      vector3f(_TV(255.f)/255.f, _TV(255.f)/255.f, _TV(255.f)/255.f);
    cityLightColor =   vector3f(_TV(100.f), _TV(32.f), _TV(5.f));

    beautifulWhite =   vector3f(_TV(255.f)/255.f, _TV(255.f)/255.f, _TV(255.f)/255.f);
    beautifulBlack =   vector3f(_TV( 10.f)/255.f, _TV( 10.f)/255.f, _TV( 10.f)/255.f);
    beautifulRed =     vector3f(_TV(255.f)/255.f, _TV(  0.f)/255.f, _TV(  0.f)/255.f);
    beautifulYellow =  vector3f(_TV(255.f)/255.f, _TV(255.f)/255.f, _TV(  0.f)/255.f);
    beautifulGreen =   vector3f(_TV(  0.f)/255.f, _TV(255.f)/255.f, _TV(  0.f)/255.f);
    beautifulBlue =    vector3f(_TV(  0.f)/255.f, _TV(128.f)/255.f, _TV(255.f)/255.f);

    list = new Element[numberOfMaterials];

    static const MaterialDesc materialDescs[] =
    {
      // -------------------------------
      { Element(Shader::retro,       Shader::staticGeomVSM, _TV( 20)), retro },
      { Element(Shader::seagulTest,  Shader::seagulTestVSM, _TV( 20)), retroSeagul },
      { Element(Shader::growTest,    Shader::growTestVSM,   _TV( 20)), retroGrowing },
      { Element(Shader::artDecoTest, Shader::artDecoTestVSM,_TV( 20)), retroDeveloping },
      { Element(Shader::danceFloor,  Shader::staticGeomVSM, _TV( 20)), danceFloor },
      { Element(Shader::hills,       Shader::hillsVSM,      _TV( 20), Texture::none, Texture::flatBump, Texture::none, Texture::hillsHM), hills },
      { Element(Shader::popping,     Shader::poppingVSM,    _TV( 20)), poppingBuilding },
      { Element(Shader::sea,         Shader::seaVSM,        _TV(255)), sea },
      { Element(Shader::floating,    Shader::floatingVSM,   _TV(100)), floatingBoat },
      { Element(Shader::lightSource, Shader::staticGeomVSM, _TV(  1)), star },
      { Element(Shader::lightSource, Shader::staticGeomVSM, _TV(  1)), tunnelLight },
      { Element(Shader::lightSource, Shader::staticGeomVSM, _TV(  1), Texture::lightBulb), light },
      { Element(Shader::text3D,      Shader::text3DVSM,     _TV(  1)), text3D },
      { Element(Shader::particles,   Shader::staticGeomVSM, _TV( 20)), particles },

#if DEBUG
      // -------------------------------
      // Tests de rendu
      { Element(Shader::parallax, Shader::staticGeomVSM,   1), testAniso0 },
      { Element(Shader::parallax, Shader::staticGeomVSM,   2), testAniso1 },
      { Element(Shader::parallax, Shader::staticGeomVSM,   4), testAniso2 },
      { Element(Shader::parallax, Shader::staticGeomVSM,   8), testAniso3 },
      { Element(Shader::parallax, Shader::staticGeomVSM,  16), testAniso4 },
      { Element(Shader::parallax, Shader::staticGeomVSM,  32), testAniso5 },
      { Element(Shader::parallax, Shader::staticGeomVSM,  64), testAniso6 },
      { Element(Shader::parallax, Shader::staticGeomVSM, 128), testAniso7 },
      { Element(Shader::parallax, Shader::staticGeomVSM, 255), testAniso8 },
#endif // DEBUG

    };

    for (unsigned int i = 0; i < ARRAY_LEN(materialDescs); ++i)
    {
      list[materialDescs[i].id] = materialDescs[i].material;
    }

    Loading::update();
  }
}
