//
// Game of life base starfield
//

#ifndef		STARFIELD_HH
# define	STARFIELD_HH

/*
#include "anim.hh"
*/
#include "basicTypes.hh"
#include "conway.hh"
#include "renderable.hh"
#include "renderlist.hh"

namespace Conway
{
  extern const date stepTime;
  extern const date lifeTime;

  class Starfield
  {
  public:
    Starfield(int xMax, int yMax);
    ~Starfield();

    void createCubes();
    void update(RenderList & renderList, date updateDate);

  private:

    static const float  _cubeSize;
    static const float  _gridStep;
    Conway		_gameOfLife;
    Renderable **	_cubes;
    date		_nextUpdateDate;
  };
}

#endif		// STARFIELD_HH
