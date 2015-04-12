//
// Game of life base starfield
//

#include "starfield.hh"

#include "animid.hh"
#include "conway.hh"
#include "keyframe.hh"
#include "renderable.hh"
#include "renderlist.hh"
#include "timing.hh"

#define START_DATE	starfieldStartDate
#define END_DATE	starfieldEndDate

namespace Conway
{
  const date stepTime = 1300;
  const date lifeTime = 8000;
  const float Starfield::_cubeSize = 4.f;
  const float Starfield::_gridStep = 6.f;

  Starfield::Starfield(int xMax, int yMax):
    _gameOfLife(xMax, yMax),
    _nextUpdateDate(0)
  {
    _cubes = (Renderable **)msys_mallocAlloc(xMax * yMax * sizeof(Renderable *));
    _gameOfLife.RandomFill();
    for (int j = 0; j < yMax; ++j)
      for (int i = 0; i < xMax; ++i)
      {
	_cubes[i + xMax * j] = NULL;
      }
  }

  Starfield::~Starfield()
  {
    const int xMax = _gameOfLife.xMax();
    const int yMax = _gameOfLife.yMax();
    for (int j = 0; j < yMax; ++j)
      for (int i = 0; i < xMax; ++i)
	{
	  delete (_cubes[i + xMax * j]);
	}

    msys_mallocFree(_cubes);
  }

  void Starfield::createCubes()
  {
    const int xMax = _gameOfLife.xMax();
    const int yMax = _gameOfLife.yMax();

    glPushMatrix();
    glLoadIdentity();
    glTranslatef(-(_gridStep / 2) * (1 + xMax), 0, -(_gridStep / 2) * (1 + yMax));
    for (int j = 0; j < yMax; ++j)
      for (int i = 0; i < xMax; ++i)
	{
	  glPushMatrix();
	  glTranslatef(_gridStep * i, 0, _gridStep * j);
          glScalef(_cubeSize, _cubeSize, _cubeSize);
          _cubes[i + xMax * j] = new Renderable(END_DATE, Shader::conway, START_DATE);
          _cubes[i + xMax * j]->setId(i + xMax * j);
	  glPopMatrix();
	}
    glPopMatrix();
  }

  void Starfield::update(RenderList & renderList, date updateDate)
  {
#if DEBUG
    static date max_date = 0;
    if (max_date > updateDate)
      return;
    max_date = updateDate;
#endif

    if (updateDate > _nextUpdateDate)
    {
      _gameOfLife.NextStep();
      _nextUpdateDate = min(END_DATE, updateDate + stepTime);
      const date stepEpsilon = stepTime / 2;

      const int xMax = _gameOfLife.xMax();
      const int yMax = _gameOfLife.yMax();
      for (int j = 0; j < yMax; ++j)
	for (int i = 0; i < xMax; ++i)
	{
	  const Cell cell = _gameOfLife.Get(i, j);
	  if (cell != still_empty)
	  {
	    Renderable & cube = *_cubes[i + xMax * j];
	    switch (cell)
	    {
	    case still_full:
	      cube.setDeathDate(_nextUpdateDate);
	      cube.setAnimation(Anim::none);
	      break;
	    case was_full:
	      {
		// Pour éviter l'effet départ de groupe
		date delay = msys_rand() % stepEpsilon;
		cube.setDeathDate(min(END_DATE, updateDate + lifeTime + delay));
		cube.setAnimation(Anim::conwayDie, updateDate + delay);
	      }
	      break;
	    case was_empty:
	      cube.setDeathDate(_nextUpdateDate);
	      cube.setAnimation(Anim::conwayGrow, updateDate);
	      break;
	    case from_top:
	      cube.setDeathDate(_nextUpdateDate);
	      cube.setAnimation(Anim::conwayFromTop, updateDate);
	      break;
	    case from_left:
	      cube.setDeathDate(_nextUpdateDate);
	      cube.setAnimation(Anim::conwayFromLeft, updateDate);
	      break;
	    case from_right:
	      cube.setDeathDate(_nextUpdateDate);
	      cube.setAnimation(Anim::conwayFromRight, updateDate);
	      break;
	    case from_bottom:
	      cube.setDeathDate(_nextUpdateDate);
	      cube.setAnimation(Anim::conwayFromBottom, updateDate);
	      break;
	    default:
	      break;
	    }
	    renderList.add(cube);
	  }
	}
    }
  }
}
