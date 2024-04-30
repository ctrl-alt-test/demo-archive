#ifndef ALGORITHM_HH
#define ALGORITHM_HH

namespace Noise
{
	class Rand;
}

namespace Container
{
	template<typename T> class Array;

	template<typename T> void swap(T& a, T& b);
	template<typename T> int find(const Array<T>& array, const T& item);
	template<typename T> int find(const T* array, const T& item, int first, int last);
	template<typename T> int binarySearch(const Array<T>& array, const T& item);
	template<typename T> int binarySearch(const T* array, const T& item, int first, int last);

	template<typename T> void shuffle(Array<T>& array, Noise::Rand& rand);
	template<typename T> void shuffle(T* array, Noise::Rand& rand, int first, int last);

	template<typename T> bool isSorted(const Array<T>& array);
	template<typename T> bool isSorted(const T* array, int first, int last);

	template<typename T> void bubbleSort(Array<T>& array);
	template<typename T> void bubbleSort(T* array, int first, int last);
	template<typename T> void insertionSort(Array<T>& array);
	template<typename T> void insertionSort(T* array, int first, int last);
	template<typename T> void quickSort(Array<T>& array);
	template<typename T> void quickSort(T* array, int first, int last);
	template<typename T> void mergeSort(Array<T>& array);
	template<typename T> void mergeSort2(Array<T>& array);
	template<typename T> void mergeSort2(T* array, int first, int last);

	struct Sortable
	{
		int key;
		int pos;
		Sortable(int k, int p): key(k), pos(p) {}
		bool operator > (const Sortable& other) const { return key > other.key; }
	};

	template<typename T> void copyAndSort(void (sort)(Array<Sortable>&), Array<T>& array, int first, int last);
}

#endif // ALGORITHM_HH
