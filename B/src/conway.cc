//
// Conway's game of life
//

#include "conway.hh"

#include "sys/msys.h"

namespace Conway
{
  Conway::Conway(int xMax, int yMax):
    _xMax(xMax), _yMax(yMax)
  {
    const int size = xMax * yMax;
    Array = (Cell *) msys_mallocAlloc(size * sizeof Cell);
    TmpArray = (Cell *) msys_mallocAlloc(size * sizeof Cell);
  }

  Conway::~Conway()
  {
    msys_mallocFree(Array);
    msys_mallocFree(TmpArray);
  }

  void Conway::RandomFill()
  {
    msys_srand(1);
    for (int i = 0; i < _xMax; i++)
      for (int j = 0; j < _yMax; j++)
	Set(i, j, (msys_rand() % 7) == 1 ? was_empty : was_full);
  }

  void Conway::NextStep()
  {
    for (int i = 0; i < _xMax; i++)
      for (int j = 0; j < _yMax; j++)
      {
	Cell oldCell = Get(i, j);
	int neighbors = 0;
	// toric topology
	for (int di = -1; di <= 1; di++)
	  for (int dj = -1; dj <= 1; dj++)
	    if (di != 0 || dj != 0)
	    {
	      int i2 = (i + di + _xMax) % _xMax;
	      int j2 = (j + dj + _yMax) % _yMax;
	      neighbors += (IsFull(Get(i2, j2))) ? 1 : 0;
	    }
	Cell newCell = IsFull(oldCell) ?
	  neighbors < 2 || neighbors > 3 ? was_full : still_full :
	  neighbors == 3 ? was_empty : still_empty;
	TmpSet(i, j, newCell);
      }
    // find the moves: naive
    for (int i = 0; i < _xMax; i++)
      for (int j = 0; j < _yMax; j++)
	if (TmpGet(i, j) == was_empty) // then find from where it could come
	  for (int di = -1; di <= 1; di++)
	    for (int dj = -1; dj <= 1; dj++)
	      if ((di == 0) != (dj == 0)) // detect [up down left right] moves
		//if (di != 0 || dj != 0) // diagonals (TODO: more enums)
	      {
		int i2 = (i + di + _xMax) % _xMax;
		int j2 = (j + dj + _yMax) % _yMax;
		if (TmpGet(i2, j2) == was_full && Get(i2, j2) != internal_use_only)
		{
		  TmpSet(i, j, di == -1 ? from_left : di == 1 ? from_right : dj == -1 ? from_top : from_bottom);
		  Set(i2, j2, internal_use_only); // mark (i + di, j + dj) as taken, don't use it twice
		  di = dj = 42;
		}
	      }
    for (int i = 0; i < _xMax; i++)
      for (int j = 0; j < _yMax; j++)
	Set(i, j, TmpGet(i, j)); // ou simple swap des pointeurs des tableaux, comme tu veux
  }
}
