//
// Dictionaire
//

#ifndef		DICO_HH
# define	DICO_HH

template<typename K, typename V>
class Dico
{
public:

  Dico();
  Dico(int max);

  const Array<K> & keys() const;
  const Array<V> & values() const;

  int find(const K & key) const;
  int findValue(const V * value) const;
  bool containsKey(const K & key) const;
  bool containsValue(const V * value) const;

  const V & operator [](const K & key) const;
  V & operator [](const K & key);

  void add(const K & key, const V & value);
  void remove(const K & key);
  void empty();

  // _values array helpers
  const V & operator [](int i) const;
  V & operator [](int i);
  const V & last() const;
  V & last();

  int size() const;

private:
  Array<K>	_keys;
  Array<V>	_values;
};

#include "dico.hxx"

#endif // DICO_HH
