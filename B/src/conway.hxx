//
// Conway's game of life
//

#ifndef		CONWAY_HXX
# define	CONWAY_HXX

namespace Conway
{
  inline
  int Conway::xMax() const
  {
    return _xMax;
  }

  inline
  int Conway::yMax() const
  {
    return _yMax;
  }

  inline
  Cell Conway::Get(int x, int y) const
  {
    return Array[x * _yMax + y];
  }

  inline
  void Conway::Set(int x, int y, Cell cell)
  {
    Array[x * _yMax + y] = cell;
  }

  inline
  Cell Conway::TmpGet(int x, int y) const
  {
    return TmpArray[x * _yMax + y];
  }

  inline
  void Conway::TmpSet(int x, int y, Cell cell)
  {
    TmpArray[x * _yMax + y] = cell;
  }

  inline
  bool Conway::IsFull(Cell cell) const
  {
    return cell != still_empty && cell != was_full;
  }
}

#endif		// CONWAY_HXX
