
#include "array.hxx"
#include "interpolation.hxx"
#include "intro.hh"
#include "node.hxx"
#include "renderable.hxx"
#include "tweakval.hh"
#include "vbos.hh"

#include "sys/msys_libc.h"

namespace ArtDeco
{
  static float computeTransition(int objid)
  {
    date dates[] = {
      _TV(207000), _TV(223000),
      _TV(226000), _TV(231000),
    };
    date start = dates[objid * 2];
    date end = dates[objid * 2 + 1];
    return interpolate(clamp(intro.now.story, start, end), start, end);
  }

  Node * createGen(date startDate, date endDate, int id)
  {
    Node * root = Node::New(startDate, endDate);
    Renderable building(Material::retroDeveloping, VBO_(artDecoBuilding));
    building.setTransitionFct(computeTransition);
    building.setId(id);
    root->attachRenderable(building);
    return root;
  }

  Node * create(date startDate, date endDate)
  {
    return createGen(startDate, endDate, 0);
  }
  Node * create2(date startDate, date endDate)
  {
    return createGen(startDate, endDate, 1);
  }
}
