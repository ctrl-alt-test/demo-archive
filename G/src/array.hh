//
// Conteneur tableau dynamique
//

#ifndef		ARRAY_HH
# define	ARRAY_HH

#include "sys/msys_debug.h"

struct Sortable
{
  int key;
  int pos;
  Sortable(int k, int p): key(k), pos(p) {}
  bool operator > (const Sortable &other) const { return key > other.key; }
};

template<typename T>
class Array
{
public:
  int size;
  IFDBG(int max_size;)
  T* elt;

  Array();
  Array(int max);
  ~Array();

  void init(int max);

  const T & operator [](int i) const;
  T & operator [](int i);

  const T & last() const;
  T & last();
  T & getNew();

  int find(const T & item) const;
  void add(const T & item);
  void remove(int n);
  void pop(){remove(size - 1);}
  void empty();
  void swap(int n1, int n2);

  void _bubbleSort(int first, int last);
  void _insertionSort(int first, int last);
  void _quickSort(int first, int last);
  void _mergeSort();

  void _copyAndMergeSort(int first, int last);
  void _copyAndQuickSort(int first, int last);

protected:
  void _merge(Array<T> & temp, int de1, int de2, int de3);
  int _partition(int start, int end);

  void _copyForSort(Array<Sortable> & arr, int first, int last);
  void _copyBackAfterSort(Array<Sortable> & arr, int first, int last);

private:
  // Pas de copie implicite
  Array(Array<T> & arr);
};

#endif // ARRAY_HH
