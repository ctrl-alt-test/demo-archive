#include "scenes.hh"

#include "artdeco.hh"
#include "city.hh"
#include "hills.hh"
#include "highway.hh"
#include "loading.hh"
#include "night.hh"
#include "node.hxx"
#include "particles.hh"
#include "sea.hh"
#include "seagul.hh"
#include "text.hh"
#include "tunnel.hh"
#include "windturbine.hh"

#include "timing.hh"
#include "tweakval.hh"

#include "sys/msys.h"

namespace Scenes
{
  void generateMeshes()
  {
    City::generateMeshes(); Loading::update();
    Hills::generateMeshes(); Loading::update();
    HighWay::generateMeshes(); Loading::update();
    Night::generateMesh(); Loading::update();
    Particles::generateMeshes(); Loading::update();
    Sea::generateMeshes(); Loading::update();
    Seagul::generateMesh(); Loading::update();
    Text::generateMeshes(); Loading::update();
    Tunnel::generateMeshes(); Loading::update();
    WindTurbine::generateMesh(); Loading::update();
  }

  struct sceneDesc
  {
    Node * (*sceneCreator)(date, date);
    date startDate;
    date endDate;
    float px;
    float py;
    float pz;
    float ry;

    void createAndAttach(Node * root) const
    {
      glLoadIdentity();
      glTranslatef(px, py, pz);
      glRotatef(ry, 0, 1.f, 0);
      sceneCreator(startDate, endDate)->attachToNode(root);
    }
  };

  Node * create()
  {
    START_TIME_EVAL;

    glLoadIdentity();
    Node * root = Node::New(worldStartDate, worldEndDate);
    const sceneDesc scenes[] =
      {
	// Create             startDate    endDate       pos.x     pos.y     pos.z     r.y
	{ Sea::create,         _TV(0),     _TV( 43000), _TV(0.f), _TV(0.f), _TV(0.f), _TV(0.f) },
	{ Hills::create,      _TV( 25000), _TV(142500), _TV(0.f), _TV(0.f), _TV(0.f), _TV(0.f) },
	{ Night::create,      _TV(115000), _TV(192000), _TV(0.f), _TV(10.f), _TV(0.f), _TV(0.f) },
	{ Particles::create,  _TV(117000), _TV(207000), _TV(0.f), _TV(9.f), _TV(0.f), _TV(0.f) },
	{ ArtDeco::create,    _TV(207000), _TV(238000), _TV(-45.f), _TV(0.f), _TV(122.5f), _TV(0.f) },
	{ HighWay::create,    _TV(222900), _TV(247000), _TV(0.f), _TV(0.f), _TV(150.f), _TV(0.f) },
	{ Tunnel::create,     _TV(231000), _TV(254100), _TV(0.f), _TV(0.f), _TV(0.f), _TV(0.f) },
	{ City::create,       _TV(254100), _TV(999999), _TV(0.f), _TV(0.f), _TV(0.f), _TV(0.f) },
      };

    const unsigned int numberOfScenes = ARRAY_LEN(scenes);
    for (unsigned int i = 0; i < numberOfScenes; ++i)
    {
      scenes[i].createAndAttach(root);
      Loading::update();
    }

    END_TIME_EVAL("Scene setup");
    return root;
  }
}
