#ifndef QUEUE_HXX
#define QUEUE_HXX

#include "Queue.hh"
#include <cassert>
#include <cstdlib>

namespace Container
{
	template<typename T>
	Queue::Queue(int size) : m_size(size), m_begin(0), m_end(0)
	{
		m_elt = (T*)malloc(size * sizeof(T));
	}

	template<typename T>
	Queue::~Queue()
	{
		free(m_elt);
	}

	template<typename T>
	void Queue<T>::Push(const T& item)
	{
		m_elt[m_end] = item;
		m_end = (m_end + 1) % m_size;
		assert(m_begin != m_end);
	}

	template<typename T>
	const T& Queue<T>::Pop()
	{
		assert(m_begin != m_end);
		const T& res = m_elt[m_begin];
		m_begin = (m_begin + 1) % m_size;
		return res;
	}

	template<typename T>
	bool Queue<T>::Empty()
	{
		return m_begin == m_end;
	}
}

#endif // QUEUE_HXX
