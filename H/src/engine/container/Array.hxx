#ifndef ARRAY_HXX
#define ARRAY_HXX

#include "Array.hh"
#include <cassert>
#include <cstdlib>

#include "engine/core/msys_temp.hh"

namespace Container
{
	template<typename T>
	Array<T>::Array(): size(0), elt(NULL)
	{
#if DEBUG
		max_size = 0;
#endif
	}

	template<typename T>
	Array<T>::Array(int max): size(0)
	{
#if DEBUG
		max_size = max;
#endif
		elt = (T*)malloc(max * sizeof(T));
		assert(elt != NULL);
	}

	template<typename T>
	Array<T>::~Array()
	{
		if (elt != NULL)
		{
			free(elt);
			elt = NULL;
		}
	}

	template<typename T>
	void Array<T>::init(int max)
	{
		assert(NULL == elt);
		assert(max > 0);
#if DEBUG
		assert(max_size == 0);
		max_size = max;
#endif

		size = 0;
		elt = (T*)malloc(max * sizeof(T));
		assert(elt != NULL);
	}

	template<typename T>
	void Array<T>::copyFrom(const Array<T>& src)
	{
#if DEBUG
		assert(max_size >= src.size);
#endif
		size = src.size;
		for (int i = 0; i < size; ++i)
		{
			elt[i] = src[i];
		}
	}

	template<typename T>
	inline
	const T& Array<T>::operator [](int i) const
	{
		assert(i >= 0);
		assert(i < size);
		return elt[i];
	}

	template<typename T>
	inline
	T& Array<T>::operator [](int i)
	{
		assert(i >= 0);
		assert(i < size);
		return elt[i];
	}

	template<typename T>
	inline
	const T& Array<T>::first() const
	{
		assert(size > 0);
		return elt[0];
	}

	template<typename T>
	inline
	T& Array<T>::first()
	{
		assert(size > 0);
		return elt[0];
	}

	template<typename T>
	inline
	const T& Array<T>::last() const
	{
		assert(size > 0);
		return elt[size - 1];
	}

	template<typename T>
	inline
	T& Array<T>::last()
	{
		assert(size > 0);
		return elt[size - 1];
	}

	template<typename T>
	inline
	T& Array<T>::getNew()
	{
#if DEBUG
		assert(size < max_size);
#endif
		++size;
		return last();
	}

	template<typename T>
	inline
	void Array<T>::add(const T& item)
	{
#if DEBUG
		assert(size < max_size);
#endif
		elt[size] = item;
		++size;
	}

	template<typename T>
	inline
	void Array<T>::remove(int n)
	{
		assert(n >= 0);
		assert(n < size);
		--size;
		elt[n] = elt[size];
	}

	template<typename T>
	inline
	void Array<T>::empty()
	{
		size = 0;
	}
}

#endif // ARRAY_HXX
