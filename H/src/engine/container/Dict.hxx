#pragma once

#include "Dict.hh"
#include "Algorithm.hxx"
#include <cassert>

namespace Container
{
	template<typename K, typename V>
	Dict<K, V>::Dict()
	{
	}

	template<typename K, typename V>
	Dict<K, V>::Dict(int max):
		keys(max),
		values(max)
	{
	}

	template<typename K, typename V>
	inline
	void Dict<K, V>::init(int max)
	{
		keys.init(max);
		values.init(max);
	}

	template<typename K, typename V>
	inline
	const V* Dict<K, V>::operator [](const K& key) const
	{
		const int i = find(keys, key);
		return (i >= 0 ? &values[i] : NULL);
	}

	template<typename K, typename V>
	inline
	V* Dict<K, V>::operator [](const K& key)
	{
		const int i = find(keys, key);
		return (i >= 0 ? &values[i] : NULL);
	}

	template<typename K, typename V>
	inline
	const K* Dict<K, V>::findKey(const V& value) const
	{
		const int i = find(values, value);
		return (i >= 0 ? &keys[i] : NULL);
	}

	template<typename K, typename V>
	inline
	void Dict<K, V>::add(const K& key, const V& value)
	{
		assert(find(keys, key) == -1);

		keys.add(key);
		values.add(value);
	}

	template<typename K, typename V>
	inline
	void Dict<K, V>::remove(const K& key)
	{
		const int i = find(key);
		if (i >= 0)
		{
			keys.remove(i);
			values.remove(i);
		}
	}

	template<typename K, typename V>
	inline
	void Dict<K, V>::empty()
	{
		keys.empty();
		values.empty();
	}
}
