#include "engine/container/Algorithm.hxx"
#include "engine/container/Array.hxx"
#include "engine/container/Utils.hh"
#include "UnitTest++/UnitTest++.h"
#include <cstdio>

using namespace Container;

SUITE(Algorithm)
{
	TEST(swap)
	{
		{
			const int a = 1;
			const int b = 2;
			int aa = a;
			int bb = b;
			swap(aa, bb);
			CHECK_EQUAL(b, aa);
			CHECK_EQUAL(a, bb);

			swap(aa, aa);
			CHECK_EQUAL(b, aa);
		}
		{
			const float a = 1.1f;
			const float b = 2.2f;
			float aa = a;
			float bb = b;
			swap(aa, bb);
			CHECK_EQUAL(b, aa);
			CHECK_EQUAL(a, bb);

			swap(aa, aa);
			CHECK_EQUAL(b, aa);
		}
	}

	TEST(find)
	{
		const int testData[] = {
			0, 2, 4, 1, 7, 3, 7, 3, 3, 6,
		};
		Array<int> list(ARRAY_LEN(testData));
		for (size_t i = 0; i < ARRAY_LEN(testData); ++i)
		{
			list.add(testData[i]);
		}

		CHECK_EQUAL(-1, find(list, 42));
		CHECK_EQUAL(0, find(list, 0));
		CHECK_EQUAL(1, find(list, 2));
		CHECK_EQUAL(5, find(list, 3));
		CHECK_EQUAL(9, find(list, 6));
	}

	TEST(binarySearch)
	{
	}

	TEST(isSorted)
	{
		int staticArray[1023];
		Array<int> array(ARRAY_LEN(staticArray));
		array.size = ARRAY_LEN(staticArray);
		{
			for (size_t i = 0; i < ARRAY_LEN(staticArray); ++i)
			{
				staticArray[i] = i;
				array[i] = i;
			}
			CHECK(isSorted(staticArray, 0, ARRAY_LEN(staticArray) - 1));
			CHECK(isSorted(array));
		}
		{
			for (size_t i = 0; i < ARRAY_LEN(staticArray); ++i)
			{
				staticArray[i] = 42;
				array[i] = 42;
			}
			CHECK(isSorted(staticArray, 0, ARRAY_LEN(staticArray) - 1));
			CHECK(isSorted(array));
		}
		{
			for (size_t i = 0; i < ARRAY_LEN(staticArray); ++i)
			{
				staticArray[i] = i;
				array[i] = i;
			}
			staticArray[345] = 1;
			array[345] = 1;
			CHECK(false == isSorted(staticArray, 0, ARRAY_LEN(staticArray) - 1));
			CHECK(false == isSorted(array));
		}
		{
			for (size_t i = 0; i < ARRAY_LEN(staticArray); ++i)
			{
				staticArray[i] = i;
				array[i] = i;
			}
			staticArray[0] = 42;
			array[0] = 42;
			CHECK(false == isSorted(staticArray, 0, ARRAY_LEN(staticArray) - 1));
			CHECK(false == isSorted(array));
		}
		{
			for (size_t i = 0; i < ARRAY_LEN(staticArray); ++i)
			{
				staticArray[i] = i;
				array[i] = i;
			}
			staticArray[ARRAY_LEN(staticArray) - 1] = 42;
			array.last() = 42;
			CHECK(false == isSorted(staticArray, 0, ARRAY_LEN(staticArray) - 1));
			CHECK(false == isSorted(array));
		}
	}

	TEST(shuffle)
	{
		Noise::Rand rand;
		int staticArray[1023];
		Array<int> array(ARRAY_LEN(staticArray));
		array.size = ARRAY_LEN(staticArray);
		for (int j = 0; j < 10; ++j)
		{
			for (size_t i = 0; i < ARRAY_LEN(staticArray); ++i)
			{
				staticArray[i] = i;
				array[i] = i;
			}
			shuffle(staticArray, rand, 0, ARRAY_LEN(staticArray) - 1);
			shuffle(array, rand);
			CHECK(false == isSorted(staticArray, 0, ARRAY_LEN(staticArray) - 1));
			CHECK(false == isSorted(array));
		}
	}

#define SORT_ARRAY_SIZE 1023

	TEST(bubbleSort)
	{
		int staticArrayZero[SORT_ARRAY_SIZE];
		int staticArraySorted[SORT_ARRAY_SIZE];
		int staticArrayRevSorted[SORT_ARRAY_SIZE];
		int staticArrayRandom[SORT_ARRAY_SIZE];
		Array<int> arrayZero(SORT_ARRAY_SIZE);
		Array<int> arraySorted(SORT_ARRAY_SIZE);
		Array<int> arrayRevSorted(SORT_ARRAY_SIZE);
		Array<int> arrayRandom(SORT_ARRAY_SIZE);

		Noise::Rand rand;
		for (size_t i = 0; i < SORT_ARRAY_SIZE; ++i)
		{
			staticArrayZero[i] = 0;
			staticArraySorted[i] = i;
			staticArrayRevSorted[i] = SORT_ARRAY_SIZE - i;
			staticArrayRandom[i] = rand.igen();

			arrayZero.add(0);
			arraySorted.add(i);
			arrayRevSorted.add(SORT_ARRAY_SIZE - i);
			arrayRandom.add(rand.igen());
		}
		bubbleSort(staticArrayZero, 0, SORT_ARRAY_SIZE - 1);
		bubbleSort(staticArraySorted, 0, SORT_ARRAY_SIZE - 1);
		bubbleSort(staticArrayRevSorted, 0, SORT_ARRAY_SIZE - 1);
		bubbleSort(staticArrayRandom, 0, SORT_ARRAY_SIZE - 1);
		CHECK(isSorted(staticArrayZero, 0, SORT_ARRAY_SIZE - 1));
		CHECK(isSorted(staticArraySorted, 0, SORT_ARRAY_SIZE - 1));
		CHECK(isSorted(staticArrayRevSorted, 0, SORT_ARRAY_SIZE - 1));
		CHECK(isSorted(staticArrayRandom, 0, SORT_ARRAY_SIZE - 1));

		bubbleSort(arrayZero);
		bubbleSort(arraySorted);
		bubbleSort(arrayRevSorted);
		bubbleSort(arrayRandom);
		CHECK(isSorted(arrayZero));
		CHECK(isSorted(arraySorted));
		CHECK(isSorted(arrayRevSorted));
		CHECK(isSorted(arrayRandom));
	}

	TEST(insertionSort)
	{
		int staticArrayZero[SORT_ARRAY_SIZE];
		int staticArraySorted[SORT_ARRAY_SIZE];
		int staticArrayRevSorted[SORT_ARRAY_SIZE];
		int staticArrayRandom[SORT_ARRAY_SIZE];
		Array<int> arrayZero(SORT_ARRAY_SIZE);
		Array<int> arraySorted(SORT_ARRAY_SIZE);
		Array<int> arrayRevSorted(SORT_ARRAY_SIZE);
		Array<int> arrayRandom(SORT_ARRAY_SIZE);

		Noise::Rand rand;
		for (size_t i = 0; i < SORT_ARRAY_SIZE; ++i)
		{
			staticArrayZero[i] = 0;
			staticArraySorted[i] = i;
			staticArrayRevSorted[i] = SORT_ARRAY_SIZE - i;
			staticArrayRandom[i] = rand.igen();

			arrayZero.add(0);
			arraySorted.add(i);
			arrayRevSorted.add(SORT_ARRAY_SIZE - i);
			arrayRandom.add(rand.igen());
		}
		insertionSort(staticArrayZero, 0, SORT_ARRAY_SIZE - 1);
		insertionSort(staticArraySorted, 0, SORT_ARRAY_SIZE - 1);
		insertionSort(staticArrayRevSorted, 0, SORT_ARRAY_SIZE - 1);
		insertionSort(staticArrayRandom, 0, SORT_ARRAY_SIZE - 1);
		CHECK(isSorted(staticArrayZero, 0, SORT_ARRAY_SIZE - 1));
		CHECK(isSorted(staticArraySorted, 0, SORT_ARRAY_SIZE - 1));
		CHECK(isSorted(staticArrayRevSorted, 0, SORT_ARRAY_SIZE - 1));
		CHECK(isSorted(staticArrayRandom, 0, SORT_ARRAY_SIZE - 1));

		insertionSort(arrayZero);
		insertionSort(arraySorted);
		insertionSort(arrayRevSorted);
		insertionSort(arrayRandom);
		CHECK(isSorted(arrayZero));
		CHECK(isSorted(arraySorted));
		CHECK(isSorted(arrayRevSorted));
		CHECK(isSorted(arrayRandom));
	}

	TEST(quickSort)
	{
		int staticArrayZero[SORT_ARRAY_SIZE];
		int staticArraySorted[SORT_ARRAY_SIZE];
		int staticArrayRevSorted[SORT_ARRAY_SIZE];
		int staticArrayRandom[SORT_ARRAY_SIZE];
		Array<int> arrayZero(SORT_ARRAY_SIZE);
		Array<int> arraySorted(SORT_ARRAY_SIZE);
		Array<int> arrayRevSorted(SORT_ARRAY_SIZE);
		Array<int> arrayRandom(SORT_ARRAY_SIZE);

		Noise::Rand rand;
		for (size_t i = 0; i < SORT_ARRAY_SIZE; ++i)
		{
			staticArrayZero[i] = 0;
			staticArraySorted[i] = i;
			staticArrayRevSorted[i] = SORT_ARRAY_SIZE - i;
			staticArrayRandom[i] = rand.igen();

			arrayZero.add(0);
			arraySorted.add(i);
			arrayRevSorted.add(SORT_ARRAY_SIZE - i);
			arrayRandom.add(rand.igen());
		}
		quickSort(staticArrayZero, 0, SORT_ARRAY_SIZE - 1);
		quickSort(staticArraySorted, 0, SORT_ARRAY_SIZE - 1);
		quickSort(staticArrayRevSorted, 0, SORT_ARRAY_SIZE - 1);
		quickSort(staticArrayRandom, 0, SORT_ARRAY_SIZE - 1);
		CHECK(isSorted(staticArrayZero, 0, SORT_ARRAY_SIZE - 1));
		CHECK(isSorted(staticArraySorted, 0, SORT_ARRAY_SIZE - 1));
		CHECK(isSorted(staticArrayRevSorted, 0, SORT_ARRAY_SIZE - 1));
		CHECK(isSorted(staticArrayRandom, 0, SORT_ARRAY_SIZE - 1));

		quickSort(arrayZero);
		quickSort(arraySorted);
		quickSort(arrayRevSorted);
		quickSort(arrayRandom);
		CHECK(isSorted(arrayZero));
		CHECK(isSorted(arraySorted));
		CHECK(isSorted(arrayRevSorted));
		CHECK(isSorted(arrayRandom));
	}

	TEST(mergeSort)
	{
		Array<int> arrayZero(SORT_ARRAY_SIZE);
		Array<int> arraySorted(SORT_ARRAY_SIZE);
		Array<int> arrayRevSorted(SORT_ARRAY_SIZE);
		Array<int> arrayRandom(SORT_ARRAY_SIZE);

		Noise::Rand rand;
		for (size_t i = 0; i < SORT_ARRAY_SIZE; ++i)
		{
			arrayZero.add(0);
			arraySorted.add(i);
			arrayRevSorted.add(SORT_ARRAY_SIZE - i);
			arrayRandom.add(rand.igen());
		}
		mergeSort(arrayZero);
		mergeSort(arraySorted);
		mergeSort(arrayRevSorted);
		mergeSort(arrayRandom);
		CHECK(isSorted(arrayZero));
		CHECK(isSorted(arraySorted));
		CHECK(isSorted(arrayRevSorted));
		CHECK(isSorted(arrayRandom));
	}

#if 0 // FIXME: Heap corruption in mergeSort2 implementation.
	TEST(mergeSort2)
	{
		int staticArrayZero[SORT_ARRAY_SIZE];
		int staticArraySorted[SORT_ARRAY_SIZE];
		int staticArrayRevSorted[SORT_ARRAY_SIZE];
		int staticArrayRandom[SORT_ARRAY_SIZE];
		Array<int> arrayZero(SORT_ARRAY_SIZE);
		Array<int> arraySorted(SORT_ARRAY_SIZE);
		Array<int> arrayRevSorted(SORT_ARRAY_SIZE);
		Array<int> arrayRandom(SORT_ARRAY_SIZE);

		Noise::Rand rand;
		for (size_t i = 0; i < SORT_ARRAY_SIZE; ++i)
		{
			staticArrayZero[i] = 0;
			staticArraySorted[i] = i;
			staticArrayRevSorted[i] = SORT_ARRAY_SIZE - i;
			staticArrayRandom[i] = rand.igen();

			arrayZero.add(0);
			arraySorted.add(i);
			arrayRevSorted.add(SORT_ARRAY_SIZE - i);
			arrayRandom.add(rand.igen());
		}
		mergeSort2(staticArrayZero, 0, SORT_ARRAY_SIZE - 1);
		mergeSort2(staticArraySorted, 0, SORT_ARRAY_SIZE - 1);
		mergeSort2(staticArrayRevSorted, 0, SORT_ARRAY_SIZE - 1);
		mergeSort2(staticArrayRandom, 0, SORT_ARRAY_SIZE - 1);
		CHECK(isSorted(staticArrayZero, 0, SORT_ARRAY_SIZE - 1));
		CHECK(isSorted(staticArraySorted, 0, SORT_ARRAY_SIZE - 1));
		CHECK(isSorted(staticArrayRevSorted, 0, SORT_ARRAY_SIZE - 1));
		CHECK(isSorted(staticArrayRandom, 0, SORT_ARRAY_SIZE - 1));

		mergeSort2(arrayZero);
		mergeSort2(arraySorted);
		mergeSort2(arrayRevSorted);
		mergeSort2(arrayRandom);
		CHECK(isSorted(arrayZero));
		CHECK(isSorted(arraySorted));
		CHECK(isSorted(arrayRevSorted));
		CHECK(isSorted(arrayRandom));
	}
#endif
}
