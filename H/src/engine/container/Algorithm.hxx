#ifndef ALGORITHM_HXX
#define ALGORITHM_HXX

#include "Algorithm.hh"
#include "Array.hxx"
#include "engine/noise/Rand.hh"
#include <cstring>

namespace Container
{
	//
	// Swap two elements.
	//
	template<typename T>
	void swap(T& a, T& b)
	{
		T temp = a;
		a = b;
		b = temp;
	}

	//
	// Linear search.
	//
	template<typename T>
	int find(const Array<T>& array, const T& item)
	{
		return find(array.elt, item, 0, array.size - 1);
	}

	template<typename T>
	int find(const T* array, const T& item, int first, int last)
	{
		for (int i = first; i <= last; ++i)
		{
			if (item == array[i])
			{
				return i;
			}
		}
		return -1;
	}

	template<>
	inline int find<const char*>(const char* const* array, const char* const& item, int first, int last)
	{
		for (int i = first; i <= last; ++i)
		{
			if (strcmp(item, array[i]) == 0)
			{
				return i;
			}
		}
		return -1;
	}

	//
	// Binary search.
	//
	// Returns the index of the value, or the index where the value should
	// be inserted to keep the ordering.
	//
	template<typename T>
	int binarySearch(const Array<T>& array, const T& item)
	{
		return binarySearch(array.elt, item, 0, array.size - 1);
	}

	template<typename T>
	int binarySearch(const T* array, const T& item, int first, int last)
	{
		while (first <= last)
		{
			assert(!(array[first] > array[last]));

			const int med = (first + last) / 2;
			if (array[med] >= item)
			{
				last = med - 1;
			}
			else
			{
				first = med + 1;
			}
		}
		return first;
	}

	//
	// Random permutation of a list.
	// Fisher-Yates / Knuth algorithm.
	//
	template<typename T>
	void shuffle(Array<T>& array, Noise::Rand& rand)
	{
		shuffle(array.elt, rand, 0, array.size - 1);
	}

	template<typename T>
	void shuffle(T* array, Noise::Rand& rand, int first, int last)
	{
		for (int i = last; i > first; --i)
		{
			swap(array[i], array[first + rand.igen(i)]);
		}
	}

	//
	// Returns true if the array is sorted in increasing order.
	//
	template<typename T>
	bool isSorted(const Array<T>& array)
	{
		return isSorted(array.elt, 0, array.size - 1);
	}

	template<typename T>
	bool isSorted(const T* array, int first, int last)
	{
		for (int i = first; i < last; ++i)
		{
			if (array[i] > array[i + 1])
			{
				return false;
			}
		}
		return true;
	}

	//
	// Bubble sort.
	//
	template<typename T>
	void bubbleSort(Array<T>& array)
	{
		bubbleSort(array.elt, 0, array.size - 1);
	}

	template<typename T>
	void bubbleSort(T* array, int first, int last)
	{
		assert(first <= last);

		for (int i = last; i > first; --i)
		{
			bool swapped = false;

			for (int j = first; j < i; ++j)
			{
				if (array[j] > array[j + 1])
				{
					swap(array[j], array[j + 1]);
					swapped = true;
				}
			}

			if (!swapped)
			{
				break;
			}
		}
	}

	//
	// Insertion sort.
	//
	template<typename T>
	void insertionSort(Array<T>& array)
	{
		insertionSort(array.elt, 0, array.size - 1);
	}

	template<typename T>
	void insertionSort(T* array, int first, int last)
	{
		assert(first <= last);

		for (int i = first; i <= last; ++i)
		{
			int j;
			const T v = array[i];
			for (j = i - 1; j >= first; --j)
			{
				if (!(array[j] > v))
				{
					break;
				}
				array[j + 1] = array[j];
			}
			array[j + 1] = v;
		}
	}

	//
	// Quick sort.
	//
	template<typename T>
	void quickSort(Array<T>& array)
	{
		quickSort(array.elt, 0, array.size - 1);
	}

	template<typename T>
	int partition(T* array, int p, int r)
	{
		T pivot = array[(p+r)/2];
		int i = p - 1;
		int j = r + 1;
		while (true)
		{
			do { --j; } while (array[j] > pivot);
			do { ++i; } while (pivot > array[i]);

			if (i < j)
			{
				swap(array[i], array[j]);
			}
			else
			{
				return j;
			}
		}
	}

	template<typename T>
	void quickSort(T* array, int first, int last)
	{
		if (first < last)
		{
			const int pivot = partition(array, first, last);
			quickSort(array, first, pivot);
			quickSort(array, pivot + 1, last);
		}
	}

	//
	// Merge sort.
	//
	template<typename T>
	void mergeSort(Array<T>& array)
	{
		// FIXME : donner des bornes
		//         mergeSort(array, 0, array.size - 1);
		Array<T> temp(array.size);
		temp.size = array.size;
		int sortLength = 1;
		int de1;
		int de2;
		int de3;

		while (sortLength < array.size)
		{
			de1 = 0;
			while (de1 < array.size)
			{
				de2 = de1 + sortLength;
				de3 = de2 + sortLength;
				if (de2 > array.size)
				{
					de2 = array.size;
				}
				if (de3 > array.size)
				{
					de3 = array.size;
				}
				merge(array, temp, de1, de2, de3);
				de1 = de3;
			}

			// FIXME: faire un ping pong plutôt que de repasser par temp à chaque fois.
			for (int i = 0; i < array.size ; ++i)
			{
				array[i] = temp[i];
			}

			sortLength *= 2;
		}
	}

	template<typename T>
	void merge(Array<T>& array, Array<T>& temp, int de1, int de2, int de3)
	{
		const int vers1 = de2 - 1;
		const int vers2 = de3 - 1;
		const int count = de3 - de1;

		assert(count > 0);
		assert(temp.max_size > de1);

		int j = de1;

		for (int i = 0; i < count; ++i)
		{
			if (de2 > vers2)
			{
				temp[j] = array[de1];
				de1++;
			}
			else if (de1 > vers1)
			{
				temp[j] = array[de2];
				de2++;
			}
			else if (array[de1] > array[de2]) // "i1 > i2" ==> Stable sort
			{
				temp[j] = array[de2];
				de2++;
			}
			else
			{
				temp[j] = array[de1];
				de1++;
			}
			++j;
		}
	}

	template<typename T>
	void mergeSort2(Array<T>& array)
	{
		mergeSort2(array.elt, 0, array.size - 1);
	}

	template<typename T>
	void merge2(T* dst,
				const T* left, int leftLength,
				const T* right, int rightLength)
	{
		int i = 0;
		int j = 0;
		while (i < leftLength || j < rightLength)
		{
			bool pickRight = (i >= leftLength ||
							  (j < rightLength && left[i] > right[j]));
			*dst++ = (pickRight ? right[j++] : left[i++]);
		}
	}

	template<typename T>
	void mergeSort2(T* array, int first, int last)
	{
		int arrayLength = last - first + 1;
		T* temp = (T*)malloc(arrayLength * sizeof(T));

		T* ping = array + first;
		T* pong = temp;

		int leftLength = 1;
		while (leftLength < arrayLength)
		{
			int subArrayStart = 0;
			while (subArrayStart < arrayLength)
			{
				const T* left = ping + subArrayStart;
				const T* right = ping + subArrayStart + leftLength;
				int rightLength = msys_min(leftLength, arrayLength - leftLength);
				merge2(pong + subArrayStart,
					   left, leftLength,
					   right, leftLength);
				subArrayStart += leftLength + rightLength;
			}
			swap(ping, pong);
			leftLength *= 2;
		}

		if (ping == temp)
		{
			for (int i = 0; i < arrayLength; ++i)
			{
				pong[i] = ping[i];
			}
		}
		free(temp);
	}

	//
	// Tri sur les indices, pour tableaux de grosses structures.
	//
	// FIXME : probablement bon à jeter. Si on a besoin de faire ça,
	//         c'est sans doute que la structure de données est à
	//         revoir.
	//
	template<typename T>
	void copyAndSort(void (sort)(Array<Sortable>&), Array<T>& array, int first, int last)
	{
		assert(first <= last);
		assert(first >= 0 && first < array.size);
		assert(last >= 0 && last < array.size);

		const int len = last - first + 1;
		Array<Sortable> lightCopy(len);
		copyForSort(array, lightCopy, first, last);

		sort(lightCopy);

		copyBackAfterSort(array, lightCopy, first, last);
	}

	template<typename T> // T = RenderableContainer
	void copyForSort(Array<T>& src, Array<Sortable>& array, int first, int last)
	{
		for (int i = first; i <= last; i++)
		{
			Sortable s(src[i].renderable->sortHint(), i - first);
			array.add(s);
		}
	}

	template<typename T>
	void copyBackAfterSort(Array<T>& dst, Array<Sortable>& array, int first, int last)
	{
		const int len = last - first + 1;
		T *copy = (T*)malloc(len * sizeof(T));
		assert(copy != NULL);
		memcpy(copy, &dst[first], (len * sizeof(T)));

		for (int i = first; i <= last; i++)
		{
			dst[i] = copy[array[i - first].pos];
		}
		free(copy);
	}
}

#endif // ALGORITHM_HXX
