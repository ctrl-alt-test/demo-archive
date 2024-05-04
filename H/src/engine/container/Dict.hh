#pragma once

#include "Array.hh"

namespace Container
{
	template<typename K, typename V>
	/// <summary>
	/// Simple dictionary with linear search.
	/// </summary>
	class Dict
	{
	public:
		Dict();
		Dict(int max);

		void		init(int max);

		const V*	operator [](const K& key) const;
		V*			operator [](const K& key);
		const K*	findKey(const V& value) const;

		void		add(const K& key, const V& value);
		void		remove(const K& key);
		void		empty();

	public:
		Array<K>	keys;
		Array<V>	values;
	};
}
