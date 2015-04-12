//
// Conteneur file
//

#ifndef		QUEUE_HH
# define	QUEUE_HH

#include "sys/msys.h"

template<typename T>
class Queue
{
public:
  Queue(int size) : _size(size), _begin(0), _end(0) { elt = (T*) msys_mallocAlloc(size * sizeof T); }
  ~Queue() { msys_mallocFree(elt); }

  void push(const T &item);
  const T& pop();
  bool empty();

private:
  int _size;
  int _begin;;
  int _end;
  T* elt;
};

template<typename T>
void Queue<T>::push(const T &item)
{
  elt[_end] = item;
  _end = (_end + 1) % _size;
  assert(_begin != _end);
}

template<typename T>
const T& Queue<T>::pop()
{
  assert(_begin != _end);
  const T&res = elt[_begin];
  _begin = (_begin + 1) % _size;
  return res;
}

template<typename T>
bool Queue<T>::empty()
{
  return _begin == _end;
}

#endif		// FILE_HH
