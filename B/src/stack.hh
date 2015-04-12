//
// Conteneur pile
//

#ifndef		STACK_HH
# define	STACK_HH

#include "array.hh"

template<typename T>
class stack : public Array<T>
{
public:
  stack(int maxDepth);

  void push(const T & item);
  void pop();
  const T & top() const;
  T & top();
};

template<typename T>
stack<T>::stack(int maxDepth):
  Array<T>(maxDepth)
{
}

template<typename T>
void stack<T>::push(const T & item)
{
  Array<T>::add(item);
}

template<typename T>
void stack<T>::pop()
{
  remove(size - 1);
}

template<typename T>
const T & stack<T>::top() const
{
  return last();
}

template<typename T>
T & stack<T>::top()
{
  return last();
}

#endif		// STACK_H
