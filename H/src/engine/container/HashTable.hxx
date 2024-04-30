//
// Implémentation de table à adressage ouvert, où les collisions sont
// résolues en cherchant une autre cellule libre.
//

#ifndef HASH_TABLE_HXX
#define HASH_TABLE_HXX

#include "HashTable.hh"

#include "Array.hxx"
#include "engine/noise/Hash.hh"
#include "engine/core/msys_temp.hh"

#define MAX_COLLISION_SEQUENCE 5

namespace Container
{
	template<typename K, typename V>
	HashTable<K, V>::HashTable()
	{
	}

	template<typename K, typename V>
	HashTable<K, V>::HashTable(int max): cells(max), values(max)
	{
		cells.size = max;
		values.size = max;
		empty();
	}

	template<typename K, typename V>
	HashTable<K, V>::~HashTable()
	{
	}

	template<typename K, typename V>
	void HashTable<K, V>::init(int max)
	{
		cells.init(max);
		values.init(max);

		cells.size = max;
		values.size = max;
		empty();
	}

	template<typename T>
	inline bool areEqual(T const& a, T const& b)
	{
		return a == b;
	}
	//template<>		inline bool areEqual<char*>(const char* a, const char* b) { return strcmp(a, b) == 0; }

	template<typename K, typename V>
	int HashTable<K, V>::_findKey(const K& k) const
	{
		// Double hashing: h(k, i) = (h1(k) + i.h2(k)) mod m
		const int h1 = Noise::Hash::get32<K>(k);
		int h2 = Noise::Hash::get32(h1 + 1);

		// A special case to avoid.
		while (h2 % cells.size == 0)
		{
			++h2;
		}

		int h = 0;
		int i = 0;
		do
		{
			h = (h1 + i * h2) % cells.size;
			h += (h < 0) * cells.size;
			if (!cells[h].empty && !cells[h].deleted &&
				areEqual(cells[h].key, k))
			{
				return h;
			}
		}
		while (i++ < MAX_COLLISION_SEQUENCE && !cells[h].empty);

		return -1;
	}

	template<typename K, typename V>
	int HashTable<K, V>::_findFreeSpot(const K& k) const
	{
		// Double hashing: h(k, i) = (h1(k) + i.h2(k)) mod m
		const int h1 = Noise::Hash::get32(k);
		int h2 = Noise::Hash::get32(h1 + 1);

		// A special case to avoid.
		while (h2 % cells.size == 0)
		{
			++h2;
		}

		int h = 0;
		int i = 0;
		do
		{
			h = (h1 + i * h2) % cells.size;
			h += (h < 0) * cells.size;
			if (cells[h].empty || cells[h].deleted)
			{
				return h;
			}
		}
		while (i++ < MAX_COLLISION_SEQUENCE);

		// Hash table overflow.
		assert(false);
		return -1;
	}

	template<typename K, typename V>
	inline
	const V* HashTable<K, V>::operator [](const K& k) const
	{
		const int h = _findKey(k);
		return (h >= 0 ? &values[h] : NULL);
	}

	template<typename K, typename V>
	inline
	V* HashTable<K, V>::operator [](const K& k)
	{
		const int h = _findKey(k);
		return (h >= 0 ? &values[h] : NULL);
	}

	template<typename K, typename V>
	void HashTable<K, V>::add(const K& k, const V& v)
	{
		// Make sure the key hasn't been used already.
		assert(_findKey(k) < 0);

		const int h = _findFreeSpot(k);
		if (h >= 0)
		{
			cells[h].key = k;
			cells[h].empty = false;
			cells[h].deleted = false;

			values[h] = v;
		}
	}

	template<typename K, typename V>
	inline
	void HashTable<K, V>::remove(const K& k)
	{
		const int h = _findKey(k);
		if (h >= 0)
		{
			// Leave empty to false so the find function still works if
			// this key was colliding with another.
			cells[h].deleted = true;
		}
	}

	template<typename K, typename V>
	inline
	void HashTable<K, V>::empty()
	{
		for (int i = 0; i < cells.size; ++i)
		{
			cells[i].empty = true;
			cells[i].deleted = false;
		}
	}
}

#endif // HASH_TABLE_HXX
