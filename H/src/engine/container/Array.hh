#ifndef ARRAY_HH
#define ARRAY_HH

namespace Container
{
	template<typename T>
	/// <summary>
	/// Simple array.
	/// </summary>
	class Array
	{
	public:
		int size;
#if DEBUG
		int max_size;
#endif
		T* elt;

		Array();
		Array(int max);
		~Array();

		void		init(int max);
		void		copyFrom(const Array<T>& src);

		const T&	operator [](int i) const;
		T&			operator [](int i);

		const T&	first() const;
		T&			first();
		const T&	last() const;
		T&			last();
		T&			getNew();

		void		add(const T& item);
		void		remove(int n);
		void		pop(){remove(size - 1);}
		void		empty();

	private:
		// No array copy.
		Array(const Array<T>& src);
	};
}

#endif // ARRAY_HH
