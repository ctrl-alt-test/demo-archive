//
// L system
//

#include "lsystem.hh"

#include "sys/msys.h"

#include "node.hh"

#define HUMAN_ALPHABET "+-&^<>[]XFGH"

namespace LSystem
{
  System::System()
  {
  }

  enum alphabet
    {
      LEFT = 0,		// +
      RIGHT = 1,	// -
      DOWN = 2,		// &
      UP = 3,		// ^
      COUNTERCLOCK = 4,	// <
      CLOCK = 5,	// >
      PUSH = 6,		// [
      POP = 7,		// ]
      FORWARD1 = 8,	// X
      FORWARD2 = 9,	// F
      FORWARD3 = 10,	// G
      FORWARD4 = 11,	// H
      ALPHABET_LENGTH = 12
    };

  static void convertRule(Array<char> & rule)
  {
    const char * humanAlphabet = HUMAN_ALPHABET;

    for (int i = 0; i < rule.size; ++i)
    {
      IFDBG(bool found = false;)
      for (char k = 0; k < ALPHABET_LENGTH; ++k)
	if (rule[i] == humanAlphabet[k])
	{
	  rule[i] = k;
	  IFDBG(found = true;)
	}
      IFDBG(assert(found);)
    }
  }

  static void initRuleFromString(Array<char> & dest, const char * source)
  {
    msys_mallocFree(dest.elt);
    dest.elt = NULL;
    dest.size = 0;
    IFDBG(dest.max_size = 0;)
    if (source != NULL)
    {
      const int size = msys_strlen(source);
      dest.init(size);
      dest.size = size;
      msys_memcpy(dest.elt, source, size);
      convertRule(dest);
    }
  }

  void System::setRules(const char * X, const char * F,
			const char * G, const char * H)
  {
    assert(X != NULL);

    initRuleFromString(_X, X);
    initRuleFromString(_F, F);
    initRuleFromString(_G, G);
    initRuleFromString(_H, H);
  }

  // ==========================================================================
  // Première partie : développement de la grammaire

  static void applyForward(Array<char> & dest, const Array<char> & rule)
  {
    for (int k = 0; k < rule.size; ++k)
    {
      dest.add(rule[k]);
    }
  }

  //
  // Construit le résultat d'une application de la règle
  //
  void System::_iterate(Array<char> & newState, Array<char> & oldState)
  {
    for (int i = 0; i < oldState.size; ++i)
    {
      switch (oldState[i])
      {
      case (char)FORWARD1:
	applyForward(newState, _X);
	break;
      case (char)FORWARD2:
	applyForward(newState, _F);
	break;
      case (char)FORWARD3:
	applyForward(newState, _G);
	break;
      case (char)FORWARD4:
	applyForward(newState, _H);
	break;
      default:
	newState.add(oldState[i]);
      }
    }
  }

  //
  // Construit le résultat de n applications de la règle depuis le
  // postulat de départ X.
  //
  void System::develop(int iterations)
  {
    assert(iterations > 0);

    const int maxChunk = max(max(_X.size, _F.size), max(_G.size, _H.size));
    Array<char> old; // Ne pas oublier que le destructeur est appelé à la fin

    msys_mallocFree(_developped.elt); _developped.elt = NULL;
    _developped.size = 0;
    IFDBG(_developped.max_size = 0;)

    _developped.init(1);
    _developped.add((char)FORWARD1);

    for (int i = 0; i < iterations; ++i)
    {
      // Attention ici on fait un tour de passe passe pour échanger
      // old et dest
      msys_mallocFree(old.elt);
      old = _developped;
      _developped.elt = NULL;
      IFDBG(_developped.max_size = 0;)
      _developped.init(old.size * maxChunk);

      _iterate(_developped, old);
    }
  }


  // ==========================================================================
  // Seconde partie : interprétation de l'état

  struct params
  {
    float angle;
    float reduction;
    float forwardLength;
    unsigned int nodesPerForward;
    date birthDate;
    date deathDate;
    int birthDateIncrement;
    int deathDateIncrement;
  };

  typedef void(*reader)(const params & p,
			Array<Node *> & stack,
			Array<Node *> & list);

  static int getDepth(const Node * node)
  {
    if (NULL == node)
      return -1;
    return 1 + getDepth(node->parent());
  }

  static void forward(const params & p,
		      Array<Node *> & stack,
		      Array<Node *> & list)
  {
    // Translation en avant
    for (unsigned int i = 0; i < p.nodesPerForward; i++)
    {
      const int depth = getDepth(stack.last());
      const date birth = p.birthDate + depth * p.birthDateIncrement;
      const date death = p.deathDate + depth * p.deathDateIncrement;

      // Si on dépasse les bornes on arrête
      if (birth >= death)
      {
	break;
      }

      glTranslatef(0, 0, p.forwardLength);

      Node * node = Node::New(birth, death);
      Renderable r; // FIXME: on veut afficher d'autres choses qu'un cube.
      node->attachRenderable(r);
      node->attachToNode(stack.last());
      stack.last() = node;
      list.add(node);

      glLoadIdentity();
    }
  }

  // Rotation à gauche
  static void left(const params & p, Array<Node *> & s, Array<Node *> & l)
  { glRotatef(p.angle, 0, 1, 0); }

  // Rotation à droite
  static void right(const params & p, Array<Node *> & s, Array<Node *> & l)
  { glRotatef(-p.angle, 0, 1, 0); }

  // Rotation vers le bas
  static void down(const params & p, Array<Node *> & s, Array<Node *> & l)
  { glRotatef(p.angle, 1, 0, 0); }

  // Rotation vers le haut
  static void up(const params & p, Array<Node *> & s, Array<Node *> & l)
  { glRotatef(-p.angle, 1, 0, 0); }

  // Rotation dans le sens positif
  static void counterClock(const params & p, Array<Node *> & s, Array<Node *> & l)
  { glRotatef(p.angle, 0, 0, 1); }

  // Rotation dans le sens négatif
  static void clock(const params & p, Array<Node *> & s, Array<Node *> & l)
  { glRotatef(-p.angle, 0, 0, 1); }

  static void push(const params & p, Array<Node *> & stack, Array<Node *> & l)
  {
    glPushMatrix();
    glScalef(p.reduction, p.reduction, p.reduction);
    stack.add(stack.last());
  }

  static void pop(const params & p, Array<Node *> & stack, Array<Node *> & l)
  {
    stack.pop();
    glPopMatrix();
  }

  Array<Node *> * System::generateNodes(float angle,
					float reduction,
					float forwardLength,
					unsigned int nodesPerForward,
					date birthDate,
					date deathDate,
					int birthDateIncrement,
					int deathDateIncrement)
  {
    const params p =
      { angle, reduction, forwardLength, nodesPerForward,
	birthDate, deathDate, birthDateIncrement, deathDateIncrement };

    static const reader functions[] = {
      &left,
      &right,
      &down,
      &up,
      &counterClock,
      &clock,
      &push,
      &pop,
      &forward,
      &forward,
      &forward,
      &forward
    };

    Array<Node *> stack(_developped.size);
    Array<Node *> * list = new Array<Node *>(_developped.size);

    glPushMatrix();
    Node * root = Node::New(birthDate, deathDate);
    stack.add(root);
    list->add(root);

    glLoadIdentity();

    for (int i = 0; i < _developped.size; ++i)
    {
      alphabet operationId = (alphabet)_developped[i];
      reader f = functions[operationId];
      assert(f != NULL);

      (*f)(p, stack, *list);
    }
    assert(1 == stack.size);

    glPopMatrix();

    return list;
  }
}
