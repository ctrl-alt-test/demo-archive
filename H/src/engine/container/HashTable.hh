#ifndef HASH_TABLE_HH
#define HASH_TABLE_HH

#include "Array.hh"

namespace Container
{
	template<typename K, typename V>
	/// <summary>
	/// Simple hash table.
	/// </summary>
	class HashTable
	{
	public:
		HashTable();
		HashTable(int max);
		~HashTable();

		void		init(int max);

		const V*	operator [](const K& k) const;
		V*			operator [](const K& k);

		void		add(const K& k, const V& v);
		void		remove(const K& k);
		void		empty();

	public:
		struct cell
		{
			K		key;
			bool	empty;
			bool	deleted;
		};

		Array<cell>	cells;
		Array<V>	values;

	private:
		int			_findKey(const K& k) const;
		int			_findFreeSpot(const K& k) const;
	};
}

#endif // HASH_TABLE_HH
