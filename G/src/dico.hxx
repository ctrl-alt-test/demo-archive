//
// Dictionaire
//
// Les clés NULL sont autorisées, ça fait simplement des éléments
// qu'on ne peut plus indexer par clé
//

#ifndef		DICO_HXX
# define	DICO_HXX

template<typename K, typename V>
Dico<K, V>::Dico():
  _keys(0),
  _values(0)
{}

template<typename K, typename V>
Dico<K, V>::Dico(int max):
  _keys(max),
  _values(max)
{}

template<typename K, typename V>
inline
const Array<K> & Dico<K, V>::keys() const
{
  return _keys;
}

template<typename K, typename V>
inline
const Array<V> & Dico<K, V>::values() const
{
  return _values;
}

template<typename K, typename V>
int Dico<K, V>::find(const K & key) const
{
  if (key != NULL)
    for (int i = 0; i < _keys.size; ++i)
      if (_keys[i] != NULL && msys_strcmp(key, _keys[i]) == 0)
	return i;
  return -1;
}

template<typename K, typename V>
int Dico<K, V>::find(const V * value) const
{
  if (value != NULL)
    for (int i = 0; i < _values.size; ++i)
      if (value == &_values[i])
	return i;
  return -1;
}

template<typename K, typename V>
inline
const K * Dico<K, V>::findKey(const V * value) const
{
  int index = find(value);
  if (index >= 0)
    return &_keys[index];
  return NULL;
}

template<typename K, typename V>
inline
const V & Dico<K, V>::operator [](const K & key) const
{
  int i = find(key);
  assert(i >= 0);
  return _values[i];
}

template<typename K, typename V>
inline
V & Dico<K, V>::operator [](const K & key)
{
  int i = find(key);
  assert(i >= 0);
  return _values[i];
}

template<typename K, typename V>
inline
const V & Dico<K, V>::operator [](int i) const
{
  return _values[i];
}

template<typename K, typename V>
inline
V & Dico<K, V>::operator [](int i)
{
  return _values[i];
}


template<typename K, typename V>
inline
const V & Dico<K, V>::last() const
{
  return _values.last();
}

template<typename K, typename V>
inline
V & Dico<K, V>::last()
{
  return _values.last();
}

template<typename K, typename V>
inline
void Dico<K, V>::add(const K & key, const T & value)
{
  assert(find(key) == -1);

  _keys.add(key);
  _values.add(value);
}

template<typename K, typename V>
inline
void Dico<K, V>::remove(const K & key)
{
  int i = find(key);
  if (i >= 0)
  {
    _keys.remove(i);
    _values.remove(i);
  }
}

template<typename K, typename V>
inline
void Dico<K, V>::empty()
{
  _keys.empty();
  _values.empty();
}

template<typename K, typename V>
inline
int Dico<K, V>::size() const
{
	return _values.size;
}

#endif // DICO_HH
