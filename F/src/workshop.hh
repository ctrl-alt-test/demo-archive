//
// Intérieur de l'atelier
//

#ifndef		WORKSHOP_HH
# define	WORKSHOP_HH

#include "array.hh"
#include "light.hh"
#include "node.hh"
#include "vbodata.hh"
#include "phy/world.hh"

namespace Workshop
{
  void generateMeshes();

  void doorAnimation(const Node & node, date time);
  void doorHandleAnimation(const Node & node, date time);

  Node * create();
  void update(date t);

  extern phy::World world;

  void changeLightAndFog(date renderDate,
			 Light::Light & light0,
			 Light::Light & light1,
			 GLfloat * fogColor);
}

#endif		// FACTORY_HH
