//
// Buildings divers
//

#ifndef		BUILDINGS_HH
# define	BUILDINGS_HH

#include "array.hh"
#include "basicTypes.hh"
#include "renderlist.hh"

namespace Building
{
  typedef struct
  {
    float height;
    float x;
    float y;
    float side;
    int id; // pour le shader
  } towerPart;

  void pacmanHouse(RenderList & renderList, date birthDate, date deathDate);
  void stackUp(Array <RenderList *> & renderLists, float size, int id, bool isRoof);

  void buildTower(Array <RenderList *> & renderLists,
		  const towerPart * description, unsigned int parts);

//   void addLandmarkTowerToList(Array <RenderList *> & renderLists, int id);
  void addRoundTowerToList(Array <RenderList *> & renderLists, float height, int id);
  void addAngularTowerToList(Array <RenderList *> & renderLists, float height, int id);
}

#endif		// BUILDINGS_HH
