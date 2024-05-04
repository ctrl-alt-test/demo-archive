#ifndef QUEUE_HH
#define QUEUE_HH

namespace Container
{
	template<typename T>
	/// <summary>
	/// Simple queue.
	/// </summary>
	class Queue
	{
	public:
		Queue(int size) : _size(size), _begin(0), _end(0) { elt = (T*)malloc(size * sizeof T); }
		~Queue() { free(elt); }

		void		Push(const T& item);
		const T&	Pop();
		bool		Empty();

	private:
		int			m_size;
		int			m_begin;
		int			m_end;
		T*			m_elt;
	};
}

#endif // QUEUE_HH
