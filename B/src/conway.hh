//
// Conway's game of life
//

#ifndef		CONWAY_HH
# define	CONWAY_HH

namespace Conway
{
  typedef enum
    {
      still_empty,
      still_full,
      was_full,
      was_empty,
      from_left,
      from_right,
      from_top,
      from_bottom,
      internal_use_only,
    } Cell;

  class Conway
  {
  public:
    Conway(int xMax, int yMax);
    ~Conway();

    int xMax() const;
    int yMax() const;
    Cell Get(int x, int y) const;

    void RandomFill();
    void NextStep();

  private:
    void Set(int x, int y, Cell cell);
    Cell TmpGet(int x, int y) const;
    void TmpSet(int x, int y, Cell cell);
    bool IsFull(Cell cell) const;

  private:
    int		_xMax;
    int		_yMax;
    Cell *	Array;
    Cell *	TmpArray;
  };
}


#include "conway.hxx"

#endif		// CONWAY_HH
